//////////////////////////////////////////////////////////////////////////////
//
//  Create a process with a DLL (creatwth.cpp of detours.lib)
//
//  Microsoft Research Detours Package, Version 2.1.
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//

#include <windows.h>
#include <stddef.h>
#if (_MSC_VER < 1299)
typedef DWORD DWORD_PTR;
#endif
#if (_MSC_VER < 1310)
#else
#include <strsafe.h>
#endif

//#define DETOUR_DEBUG 1
#define DETOURS_INTERNAL

#include "detours.h"
#include <output_debug.h>

#define IMPORT_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
#define BOUND_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT]
#define CLR_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR]
#define IAT_DIRECTORY OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT]

//////////////////////////////////////////////////////////////////////////////
//
#ifndef _STRSAFE_H_INCLUDED_
static inline HRESULT StringCchLengthA(const char* psz, size_t cchMax, size_t* pcch)
{
    HRESULT hr = S_OK;
    size_t cchMaxPrev = cchMax;

    if(cchMax > 2147483647)
    {
        return ERROR_INVALID_PARAMETER;
    }

    while(cchMax && (*psz != '\0'))
    {
        psz++;
        cchMax--;
    }

    if(cchMax == 0)
    {
        // the string is longer than cchMax
        hr = ERROR_INVALID_PARAMETER;
    }

    if(SUCCEEDED(hr) && pcch)
    {
        *pcch = cchMaxPrev - cchMax;
    }

    return hr;
}


static inline HRESULT StringCchCopyA(char* pszDest, size_t cchDest, const char* pszSrc)
{
    HRESULT hr = S_OK;

    if(cchDest == 0)
    {
        // can not null terminate a zero-byte dest buffer
        hr = ERROR_INVALID_PARAMETER;
    }
    else
    {
        while(cchDest && (*pszSrc != '\0'))
        {
            *pszDest++ = *pszSrc++;
            cchDest--;
        }

        if(cchDest == 0)
        {
            // we are going to truncate pszDest
            pszDest--;
            hr = ERROR_INVALID_PARAMETER;
        }

        *pszDest= '\0';
    }

    return hr;
}

static inline HRESULT StringCchCatA(char* pszDest, size_t cchDest, const char* pszSrc)
{
    HRESULT hr;
    size_t cchDestCurrent;

    if(cchDest > 2147483647)
    {
        return ERROR_INVALID_PARAMETER;
    }

    hr = StringCchLengthA(pszDest, cchDest, &cchDestCurrent);

    if(SUCCEEDED(hr))
    {
        hr = StringCchCopyA(pszDest + cchDestCurrent,
                            cchDest - cchDestCurrent,
                            pszSrc);
    }

    return hr;
}

#endif

//////////////////////////////////////////////////////////////////////////////
//
static WORD detour_sum_minus(WORD wSum, WORD wMinus)
{
    wSum = (WORD)(wSum - ((wSum < wMinus) ? 1 : 0));
    wSum = (WORD)(wSum - wMinus);
    return wSum;
}

static WORD detour_sum_done(DWORD PartialSum)
{
    // Fold final carry into a single word result and return the resultant value.
    return (WORD)(((PartialSum >> 16) + PartialSum) & 0xffff);
}

static WORD detour_sum_data(DWORD dwSum, PBYTE pbData, DWORD cbData)
{
    while(cbData > 0)
    {
        dwSum += *((PWORD&)pbData)++;
        dwSum = (dwSum >> 16) + (dwSum & 0xffff);
        cbData -= sizeof(WORD);
    }
    return detour_sum_done(dwSum);
}

static WORD detour_sum_final(WORD wSum, PIMAGE_NT_HEADERS pinh)
{
    DETOUR_TRACE((".... : %08x (value: %08x)\n", wSum, pinh->OptionalHeader.CheckSum));

    // Subtract the two checksum words in the optional header from the computed.
    wSum = detour_sum_minus(wSum, ((PWORD)(&pinh->OptionalHeader.CheckSum))[0]);
    wSum = detour_sum_minus(wSum, ((PWORD)(&pinh->OptionalHeader.CheckSum))[1]);

    return wSum;
}

static WORD ChkSumRange(WORD wSum, HANDLE hProcess, PBYTE pbBeg, PBYTE pbEnd)
{
    BYTE rbPage[4096];

    while(pbBeg < pbEnd)
    {
        if(!ReadProcessMemory(hProcess, pbBeg, rbPage, sizeof(rbPage), NULL))
        {
            DETOUR_TRACE(("ReadProcessMemory(idh) failed: %d\n", GetLastError()));
            break;
        }
        wSum = detour_sum_data(wSum, rbPage, sizeof(rbPage));
        pbBeg += sizeof(rbPage);
    }
    return wSum;
}

static WORD ComputeChkSum(HANDLE hProcess, PBYTE pbModule, PIMAGE_NT_HEADERS pinh)
{
    // See LdrVerifyMappedImageMatchesChecksum.

    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));
    WORD wSum = 0;

    for(PBYTE pbLast = pbModule;;
            pbLast = (PBYTE)mbi.BaseAddress + mbi.RegionSize)
    {

        if(VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) <= 0)
        {
            if(GetLastError() == ERROR_INVALID_PARAMETER)
            {
                break;
            }
            DETOUR_TRACE(("VirtualQueryEx(%08x) failed: %d\n",
                          pbLast, GetLastError()));
            break;
        }

        if(mbi.AllocationBase != pbModule)
        {
            break;
        }

        wSum = ChkSumRange(wSum,
                           hProcess,
                           (PBYTE)mbi.BaseAddress,
                           (PBYTE)mbi.BaseAddress + mbi.RegionSize);

        DETOUR_TRACE(("[%8p..%8p] : %04x\n",
                      (PBYTE)mbi.BaseAddress,
                      (PBYTE)mbi.BaseAddress + mbi.RegionSize,
                      wSum));
    }

    return detour_sum_final(wSum, pinh);
}

//////////////////////////////////////////////////////////////////////////////
//
// Find a region of memory in which we can create a replacement import table.
//
static PBYTE FindAndAllocateNearBase(HANDLE hProcess, PBYTE pbBase, DWORD cbAlloc)
{
    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));

    for(PBYTE pbLast = pbBase;;
            pbLast = (PBYTE)mbi.BaseAddress + mbi.RegionSize)
    {

        if(VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) <= 0)
        {
            if(GetLastError() == ERROR_INVALID_PARAMETER)
            {
                break;
            }
            DETOUR_TRACE(("VirtualQueryEx(%08x) failed: %d\n",
                          pbLast, GetLastError()));
            break;
        }

        // Skip uncommitted regions and guard pages.
        //
        if((mbi.State != MEM_FREE))
        {
            continue;
        }

        PBYTE pbAddress = (PBYTE)(((DWORD_PTR)mbi.BaseAddress + 0xffff) & ~(DWORD_PTR)0xffff);

        DETOUR_TRACE(("Free region %p..%p\n",
                      mbi.BaseAddress,
                      (PBYTE)mbi.BaseAddress + mbi.RegionSize));

        for(; pbAddress < (PBYTE)mbi.BaseAddress + mbi.RegionSize; pbAddress += 0x10000)
        {
            PBYTE pbAlloc = (PBYTE)VirtualAllocEx(hProcess, pbAddress, cbAlloc,
                                                  MEM_RESERVE, PAGE_READWRITE);
            if(pbAlloc == NULL)
            {
                DETOUR_TRACE(("VirtualAllocEx(%p) failed: %d\n", pbAddress, GetLastError()));
                continue;
            }
            pbAlloc = (PBYTE)VirtualAllocEx(hProcess, pbAddress, cbAlloc,
                                            MEM_COMMIT, PAGE_READWRITE);
            if(pbAlloc == NULL)
            {
                DETOUR_TRACE(("VirtualAllocEx(%p) failed: %d\n", pbAddress, GetLastError()));
                continue;
            }
            DETOUR_TRACE(("[%p..%p] Allocated for import table.\n",
                          pbAlloc, pbAlloc + cbAlloc));
            return pbAlloc;
        }
    }
    return NULL;
}

static inline DWORD PadToDword(DWORD dw)
{
    return (dw + 3) & ~3u;
}

static inline DWORD PadToDwordPtr(DWORD dw)
{
    return (dw + 7) & ~7u;
}

static BOOL IsExe(HANDLE hProcess, PBYTE pbModule)
{
    IMAGE_DOS_HEADER idh;
    ZeroMemory(&idh, sizeof(idh));

    if(!ReadProcessMemory(hProcess, pbModule, &idh, sizeof(idh), NULL))
    {
        DETOUR_TRACE(("ReadProcessMemory(idh) failed: %d\n", GetLastError()));
        return FALSE;
    }

    if(idh.e_magic != IMAGE_DOS_SIGNATURE)
    {
        // DETOUR_TRACE(("  No IMAGE_DOS_SIGNATURE\n"));
        return FALSE;
    }

    IMAGE_NT_HEADERS inh;
    ZeroMemory(&inh, sizeof(inh));

    if(!ReadProcessMemory(hProcess, pbModule + idh.e_lfanew, &inh, sizeof(inh), NULL))
    {
        DETOUR_TRACE(("ReadProcessMemory(inh) failed: %d\n", GetLastError()));
        return FALSE;
    }

    if(inh.Signature != IMAGE_NT_SIGNATURE)
    {
        DETOUR_TRACE(("  No IMAGE_NT_SIGNATURE\n"));
        return FALSE;
    }

    if(inh.FileHeader.Characteristics & IMAGE_FILE_DLL)
    {
        DETOUR_TRACE(("  Characteristics: %08x\n", inh.FileHeader.Characteristics));
        return FALSE;
    }

    return TRUE;
}

PVOID FindExe(HANDLE hProcess)
{
    MEMORY_BASIC_INFORMATION mbi;
    ZeroMemory(&mbi, sizeof(mbi));

    // Find the next memory region that contains a mapped PE image.
    //
    for(PBYTE pbLast = (PBYTE)0x10000;;
            pbLast = (PBYTE)mbi.BaseAddress + mbi.RegionSize)
    {

        if(VirtualQueryEx(hProcess, (PVOID)pbLast, &mbi, sizeof(mbi)) <= 0)
        {
            if(GetLastError() == ERROR_INVALID_PARAMETER)
            {
                break;
            }
            DETOUR_TRACE(("VirtualQueryEx(%08x) failed: %d\n",
                          pbLast, GetLastError()));
            break;
        }

        // Skip uncommitted regions and guard pages.
        //
        if((mbi.State != MEM_COMMIT) || (mbi.Protect & PAGE_GUARD))
        {
            continue;
        }

        DETOUR_TRACE(("%8p..%8p [%8p]\n",
                      mbi.BaseAddress,
                      (PBYTE)mbi.BaseAddress + mbi.RegionSize,
                      mbi.AllocationBase));

        if(IsExe(hProcess, pbLast))
        {
#if DETOUR_DEBUG
            for(PBYTE pbNext = (PBYTE)mbi.BaseAddress + mbi.RegionSize;;
                    pbNext = (PBYTE)mbi.BaseAddress + mbi.RegionSize)
            {

                if(VirtualQueryEx(hProcess, (PVOID)pbNext, &mbi, sizeof(mbi)) <= 0)
                {
                    if(GetLastError() == ERROR_INVALID_PARAMETER)
                    {
                        break;
                    }
                    DETOUR_TRACE(("VirtualQueryEx(%08x) failed: %d\n",
                                  pbNext, GetLastError()));
                    break;
                }

                // Skip uncommitted regions and guard pages.
                //
                if((mbi.State != MEM_COMMIT) || (mbi.Protect & PAGE_GUARD))
                {
                    continue;
                }
                DETOUR_TRACE(("%8p..%8p [%8p]\n",
                              mbi.BaseAddress,
                              (PBYTE)mbi.BaseAddress + mbi.RegionSize,
                              mbi.AllocationBase));

                IsExe(hProcess, pbNext);
            }
#endif
            return pbLast;
        }
    }
    return NULL;
}


typedef struct  _insert_dlls
{
    HMODULE(WINAPI *pLoadLibraryFn)(const char* pDllName);
    char* pLoadLibraryFiles[];
} INSERT_DLLS_t,*PINSERT_DLLS_t;

static int StartInsertDlls(void)
{
    HMODULE hRetMod=NULL;
    int i;
    /*we put 0xcccccccc as it will make very clear code*/
    volatile PINSERT_DLLS_t pInsertDlls=(PINSERT_DLLS_t)0xc3c3c3c3;

    for(i=0; pInsertDlls->pLoadLibraryFiles[i]; i++)
    {
        hRetMod = pInsertDlls->pLoadLibraryFn(pInsertDlls->pLoadLibraryFiles[i]);
    }

	return 0;
}

static int EndInsertDlls(void)
{
    return 0;
}






BOOL UpdateImports(HANDLE hProcess, LPCSTR *plpDlls, DWORD nDlls)
{
    BOOL fSucceeded = FALSE;
    PBYTE pbModule = (PBYTE)FindExe(hProcess);


}





//////////////////////////////////////////////////////////////////////////////
//
int WINAPI DetourCreateProcessWithDllA(LPCSTR lpApplicationName,
                                       __in_z LPSTR lpCommandLine,
                                       LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                       LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                       BOOL bInheritHandles,
                                       DWORD dwCreationFlags,
                                       LPVOID lpEnvironment,
                                       LPCSTR lpCurrentDirectory,
                                       LPSTARTUPINFOA lpStartupInfo,
                                       LPPROCESS_INFORMATION lpProcessInformation,
                                       LPCSTR lpDetouredDllFullName,
                                       LPCSTR lpDllName,
                                       PDETOUR_CREATE_PROCESS_ROUTINEA pfCreateProcessA)
{
    DWORD dwMyCreationFlags = (dwCreationFlags | CREATE_SUSPENDED);
    PROCESS_INFORMATION pi;
    int ret;
    DWORD processid=0;

    if(pfCreateProcessA == NULL)
    {
        pfCreateProcessA = CreateProcessA;
    }

    if(!pfCreateProcessA(lpApplicationName,
                         lpCommandLine,
                         lpProcessAttributes,
                         lpThreadAttributes,
                         bInheritHandles,
                         dwMyCreationFlags,
                         lpEnvironment,
                         lpCurrentDirectory,
                         lpStartupInfo,
                         &pi))
    {
        return -1;
    }

    LPCSTR rlpDlls[2];
    DWORD nDlls = 0;
    if(lpDetouredDllFullName != NULL)
    {
        rlpDlls[nDlls++] = lpDetouredDllFullName;
    }
    if(lpDllName != NULL)
    {
        rlpDlls[nDlls++] = lpDllName;
    }

    if(!UpdateImports(pi.hProcess, rlpDlls, nDlls))
    {
        return -1;
    }
    processid = GetProcessId(pi.hProcess);

    if(lpProcessInformation)
    {
        CopyMemory(lpProcessInformation, &pi, sizeof(pi));
    }

    if(!(dwCreationFlags & CREATE_SUSPENDED))
    {
        ResumeThread(pi.hThread);
    }
    ret = processid;
    return ret;
}


int WINAPI DetourCreateProcessWithDllW(LPCWSTR lpApplicationName,
                                       __in_z LPWSTR lpCommandLine,
                                       LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                       LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                       BOOL bInheritHandles,
                                       DWORD dwCreationFlags,
                                       LPVOID lpEnvironment,
                                       LPCWSTR lpCurrentDirectory,
                                       LPSTARTUPINFOW lpStartupInfo,
                                       LPPROCESS_INFORMATION lpProcessInformation,
                                       LPCSTR lpDetouredDllFullName,
                                       LPCSTR lpDllName,
                                       PDETOUR_CREATE_PROCESS_ROUTINEW pfCreateProcessW)
{
    DWORD dwMyCreationFlags = (dwCreationFlags | CREATE_SUSPENDED);
    PROCESS_INFORMATION pi;
    DWORD processid;
    int ret;

    if(pfCreateProcessW == NULL)
    {
        pfCreateProcessW = CreateProcessW;
    }

    if(!pfCreateProcessW(lpApplicationName,
                         lpCommandLine,
                         lpProcessAttributes,
                         lpThreadAttributes,
                         bInheritHandles,
                         dwMyCreationFlags,
                         lpEnvironment,
                         lpCurrentDirectory,
                         lpStartupInfo,
                         &pi))
    {
        DEBUG_INFO("lasterror %d\n",GetLastError());
        return -1;
    }

    DEBUG_INFO("\n");
    LPCSTR rlpDlls[2];
    DWORD nDlls = 0;
    if(lpDetouredDllFullName != NULL)
    {
        rlpDlls[nDlls++] = lpDetouredDllFullName;
        DEBUG_INFO("lpDetouredDllFullName %s\n",lpDetouredDllFullName);
    }
    if(lpDllName != NULL)
    {
        rlpDlls[nDlls++] = lpDllName;
        DEBUG_INFO("lpDllName %s\n",lpDllName);
    }
    DEBUG_INFO("\n");

    if(!UpdateImports(pi.hProcess, rlpDlls, nDlls))
    {
        DEBUG_INFO("\n");
        return -1;
    }
    processid = GetProcessId(pi.hProcess);

    if(lpProcessInformation)
    {
        CopyMemory(lpProcessInformation, &pi, sizeof(pi));
    }

    if(!(dwCreationFlags & CREATE_SUSPENDED))
    {
        ResumeThread(pi.hThread);
    }
    DEBUG_INFO("processid %d\n",processid);
    ret = processid;
    return ret;
}

BOOL WINAPI DetourCopyPayloadToProcess(HANDLE hProcess,
                                       REFGUID rguid,
                                       PVOID pData,
                                       DWORD cbData)
{
    DWORD cbTotal = (sizeof(IMAGE_DOS_HEADER) +
                     sizeof(IMAGE_NT_HEADERS) +
                     sizeof(IMAGE_SECTION_HEADER) +
                     sizeof(DETOUR_SECTION_HEADER) +
                     sizeof(DETOUR_SECTION_RECORD) +
                     cbData);

    PBYTE pbBase = (PBYTE)VirtualAllocEx(hProcess, NULL, cbTotal,
                                         MEM_COMMIT, PAGE_READWRITE);
    if(pbBase == NULL)
    {
        DETOUR_TRACE(("VirtualAllocEx(%d) failed: %d\n", cbTotal, GetLastError()));
        return FALSE;
    }

    PBYTE pbTarget = pbBase;
    IMAGE_DOS_HEADER idh;
    IMAGE_NT_HEADERS inh;
    IMAGE_SECTION_HEADER ish;
    DETOUR_SECTION_HEADER dsh;
    DETOUR_SECTION_RECORD dsr;
    SIZE_T cbWrote = 0;

    ZeroMemory(&idh, sizeof(idh));
    idh.e_magic = IMAGE_DOS_SIGNATURE;
    idh.e_lfanew = sizeof(idh);
    if(!WriteProcessMemory(hProcess, pbTarget, &idh, sizeof(idh), &cbWrote) ||
            cbWrote != sizeof(idh))
    {
        DETOUR_TRACE(("WriteProcessMemory(idh) failed: %d\n", GetLastError()));
        return FALSE;
    }
    pbTarget += sizeof(idh);

    ZeroMemory(&inh, sizeof(inh));
    inh.Signature = IMAGE_NT_SIGNATURE;
    inh.FileHeader.SizeOfOptionalHeader = sizeof(inh.OptionalHeader);
    inh.FileHeader.Characteristics = IMAGE_FILE_DLL;
    inh.FileHeader.NumberOfSections = 1;
    if(!WriteProcessMemory(hProcess, pbTarget, &inh, sizeof(inh), &cbWrote) ||
            cbWrote != sizeof(inh))
    {
        return FALSE;
    }
    pbTarget += sizeof(inh);

    ZeroMemory(&ish, sizeof(ish));
    memcpy(ish.Name, ".detour", sizeof(ish.Name));
    ish.VirtualAddress = (DWORD)((pbTarget + sizeof(ish)) - pbBase);
    ish.SizeOfRawData = (sizeof(DETOUR_SECTION_HEADER) +
                         sizeof(DETOUR_SECTION_RECORD) +
                         cbData);
    if(!WriteProcessMemory(hProcess, pbTarget, &ish, sizeof(ish), &cbWrote) ||
            cbWrote != sizeof(ish))
    {
        return FALSE;
    }
    pbTarget += sizeof(ish);

    ZeroMemory(&dsh, sizeof(dsh));
    dsh.cbHeaderSize = sizeof(dsh);
    dsh.nSignature = DETOUR_SECTION_HEADER_SIGNATURE;
    dsh.nDataOffset = sizeof(DETOUR_SECTION_HEADER);
    dsh.cbDataSize = (sizeof(DETOUR_SECTION_HEADER) +
                      sizeof(DETOUR_SECTION_RECORD) +
                      cbData);
    if(!WriteProcessMemory(hProcess, pbTarget, &dsh, sizeof(dsh), &cbWrote) ||
            cbWrote != sizeof(dsh))
    {
        return FALSE;
    }
    pbTarget += sizeof(dsh);

    ZeroMemory(&dsr, sizeof(dsr));
    dsr.cbBytes = cbData + sizeof(DETOUR_SECTION_RECORD);
    dsr.nReserved = 0;
    dsr.guid = rguid;
    if(!WriteProcessMemory(hProcess, pbTarget, &dsr, sizeof(dsr), &cbWrote) ||
            cbWrote != sizeof(dsr))
    {
        return FALSE;
    }
    pbTarget += sizeof(dsr);

    if(!WriteProcessMemory(hProcess, pbTarget, pData, cbData, &cbWrote) ||
            cbWrote != cbData)
    {
        return FALSE;
    }
    pbTarget += cbData;

    DETOUR_TRACE(("Copied %d bytes into target process at %p\n",
                  cbTotal, pbTarget - cbTotal));
    return TRUE;
}

//
///////////////////////////////////////////////////////////////// End of File.
