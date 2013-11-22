// tvec.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

typedef struct
{
    int x;
    int y;
    int z;
} DEMO_t;

int main(int argc, char* argv[])
{
    int i;

    std::vector<DEMO_t> demovec;
	std::vector<DEMO_t*>* pdemovec=NULL;
	DEMO_t* pdemo;
    DEMO_t demo;

	pdemovec = new std::vector<DEMO_t*>();

    for(i=0; i<10; i++)
    {
        demo.x = i;
        demo.y = i;
        demo.z = i;
        demovec.push_back(demo);
		pdemo = new DEMO_t;
		CopyMemory(pdemo,&demo,sizeof(demo));
		pdemovec->push_back(pdemo);
    }

    while(demovec.size()>0)
    {
    	pdemo = pdemovec->at(0);
        demo = demovec[0];
        fprintf(stdout,"x %d y %d z %d\n",demo.x,demo.y,demo.z);
		fprintf(stdout,"px %d py %d pd %d\n",pdemo->x,pdemo->y,pdemo->z);
		
		demovec.erase(demovec.begin());
		pdemovec->erase(pdemovec->begin());
		delete pdemo;
    }
    return 0;
}

