

#include "strtohex.h"
#include <string.h>
#include <ctype.h>
unsigned int StrToHex(const char * pStart)
{
    unsigned int ret=0;
    char* pCurPtr=(char*)pStart;
    int base = 10;
    int curnum=0;

    if(strncmp(pCurPtr,"0x",2) == 0 ||
            strncmp(pCurPtr,"0X",2) == 0)
    {
        base = 16;
        pCurPtr += 2;
    }
    else if(strncmp(pCurPtr,"x",1) == 0 ||
            strncmp(pCurPtr,"X",1)==0)
    {
        base = 16;
        pCurPtr += 1;
    }

    while(1)
    {
        if(base==16 && !isxdigit(*pCurPtr))
        {
            break;
        }
        else if(base == 10 && ! isdigit(*pCurPtr))
        {
            break;
        }

        if(base == 16)
        {

            if(*pCurPtr >= 'a' && *pCurPtr <= 'f')
            {
                curnum = 10 + *pCurPtr - 'a';
            }
            else if(*pCurPtr >= 'A' && *pCurPtr <= 'F')
            {
                curnum = 10 + *pCurPtr - 'A';
            }
                     else
            {
                curnum = *pCurPtr - '0';
            }
        }
        else
        {
            curnum = *pCurPtr - '0';
        }
        ret *= base;
        ret += curnum;
        pCurPtr += 1;
    }

    return ret;
}


unsigned long long StrToHexLL(const char * pStart)
{
    unsigned long long ret;
    char* pCurPtr=(char*)pStart;
    int base = 10;
    int curnum=0;

    if(strncmp(pCurPtr,"0x",2) == 0 ||
            strncmp(pCurPtr,"0X",2) == 0)
    {
        base = 16;
        pCurPtr += 2;
    }
    else if(strncmp(pCurPtr,"x",1) == 0 ||
            strncmp(pCurPtr,"X",1)==0)
    {
        base = 16;
        pCurPtr += 1;
    }

    while(1)
    {
        if(base==16 && !isxdigit(*pCurPtr))
        {
            break;
        }
        else if(base == 10 && ! isdigit(*pCurPtr))
        {
            break;
        }

        if(base == 16)
        {

            if(*pCurPtr >= 'a' && *pCurPtr <= 'f')
            {
                curnum = 10 + *pCurPtr - 'a';
            }
            else if(*pCurPtr >= 'A' && *pCurPtr <= 'F')
            {
                curnum = 10 + *pCurPtr - 'A';
            }
                     else
            {
                curnum = *pCurPtr - '0';
            }
        }
        else
        {
            curnum = *pCurPtr - '0';
        }
        ret *= base;
        ret += curnum;
        pCurPtr += 1;
    }

    return ret;
}
