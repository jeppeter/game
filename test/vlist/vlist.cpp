// vlist.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>


#define DEBUG_INFO(...)  do{fprintf(stdout,"%s:%d",__FILE__,__LINE__);fprintf(stdout,__VA_ARGS__);}while(0)

typedef struct
{
    int m_Int;
    unsigned int m_uInt;
} buffer_t;

std::vector<buffer_t*> st_FreeList;
std::vector<buffer_t*> st_FillList;

typedef struct
{
    int m_Num;
    buffer_t*m_pWholeList;
    std::vector<buffer_t*>* m_pFreeList;
    std::vector<buffer_t*>* m_pFillList;
} WHOLE_LIST_t;

void FreeWholeList(WHOLE_LIST_t**ppWholeList)
{
    WHOLE_LIST_t*pWholeList = *ppWholeList;
    if(pWholeList)
    {
        DEBUG_INFO("\n");
        if(pWholeList->m_pFreeList)
        {
            while(pWholeList->m_pFreeList->size() > 0)
            {
                DEBUG_INFO("\n");
                pWholeList->m_pFreeList->erase(pWholeList->m_pFreeList->begin());
            }
            delete pWholeList->m_pFreeList;
        }
        pWholeList->m_pFreeList = NULL;

        if(pWholeList->m_pFillList)
        {
            while(pWholeList->m_pFillList->size() > 0)
            {
                DEBUG_INFO("\n");
                pWholeList->m_pFillList->erase(pWholeList->m_pFillList->begin());
            }
            delete pWholeList->m_pFillList;
        }
        pWholeList->m_pFillList= NULL;
        if(pWholeList->m_pWholeList)
        {
            DEBUG_INFO("\n");
            free(pWholeList->m_pWholeList);
        }
        pWholeList->m_pWholeList =NULL;
        pWholeList->m_Num = 0;
        free(pWholeList);
    }
    *ppWholeList = NULL;
    return;
}

WHOLE_LIST_t* AllocateWholeList(int num)
{
    WHOLE_LIST_t* pWholeList=NULL;
    int i;

    pWholeList = (WHOLE_LIST_t*)calloc(sizeof(*pWholeList),1);
    if(pWholeList == NULL)
    {
        return NULL;
    }
    pWholeList->m_Num = num;
    DEBUG_INFO("\n");
    pWholeList->m_pFreeList=new std::vector<buffer_t*>();
    DEBUG_INFO("\n");
    pWholeList->m_pFillList=new std::vector<buffer_t*>();
    DEBUG_INFO("\n");
    pWholeList->m_pWholeList = (buffer_t*)calloc(sizeof(pWholeList->m_pWholeList[0]),num);
    if(pWholeList->m_pWholeList == NULL)
    {
        FreeWholeList(&pWholeList);
        return NULL;
    }

    for(i=0; i<num; i++)
    {
        pWholeList->m_pFreeList->push_back(&(pWholeList->m_pWholeList[i]));
    }

    return pWholeList;
}

int main(int argc, char *argv[])
{
    int num=10;
    int ret = -2;
    int i;
    WHOLE_LIST_t *pWholeList=NULL;

    if(argc > 1)
    {
        num = atoi(argv[1]);
    }

    pWholeList = AllocateWholeList(num);
    if(pWholeList == NULL)
    {
        return -3;
    }

    FreeWholeList(&pWholeList);
    return 0;
}

