// tvec.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

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
    DEMO_t demo;

    for(i=0; i<10; i++)
    {
        demo.x = i;
        demo.y = i;
        demo.z = i;
        demovec.push_back(demo);
    }

    while(demovec.size()>0)
    {
        demo = demovec[0];
        fprintf(stdout,"x %d y %d z %d\n",demo.x,demo.y,demo.z);
		demovec.erase(demovec.begin());
    }
    return 0;
}

