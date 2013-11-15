#ifndef  __OUTPUT_DEBUG_H__
#define  __OUTPUT_DEBUG_H__

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WINXP_VER  1

#ifdef WIN7_VER
#define  DEBUG_INFO(fmt,...) DebugOutString(__FILE__,__LINE__,fmt,__VA_ARGS__)
#define  ERROR_INFO(fmt,...) DebugOutString(__FILE__,__LINE__,fmt,__VA_ARGS__)
#else
#ifdef WINXP_VER
#define  DEBUG_INFO(fmt,...) do{fprintf(stderr,"[%s:%d]\t",__FILE__,__LINE__);fprintf(stderr,fmt,__VA_ARGS__);DebugOutString(__FILE__,__LINE__,fmt,__VA_ARGS__);}while(0)
#define  ERROR_INFO(fmt,...) do{fprintf(stderr,"[%s:%d]\t",__FILE__,__LINE__);fprintf(stderr,fmt,__VA_ARGS__);DebugOutString(__FILE__,__LINE__,fmt,__VA_ARGS__);}while(0)
#else
#define  DEBUG_INFO(fmt,...) do{;}while(0)
#define  ERROR_INFO(fmt,...) do{;}while(0)
#endif
#endif
#define  DEBUG_BUFFER(ptr,blen) DebugBuffer(__FILE__,__LINE__,(unsigned char*)ptr,blen)


extern "C" void DebugOutString(const char* file,int lineno,const char* fmt,...);
extern "C" void DebugBuffer(const char* file,int lineno,unsigned char* pBuffer,int buflen);

#ifdef __cplusplus
}
#endif



#endif /*__OUTPUT_DEBUG_H__*/