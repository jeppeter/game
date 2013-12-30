// vkext.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

#define   VK_NULL                 0
#define   VK_1                    0x31
#define   VK_2                    0x32
#define   VK_3                    0x33
#define   VK_4                    0x34
#define   VK_5                    0x35
#define   VK_6                    0x36
#define   VK_7                    0x37
#define   VK_8                    0x38
#define   VK_9                    0x39
#define   VK_0                    0x30
#define   VK_A                    0x41
#define   VK_B                    0x42
#define   VK_C                    0x43
#define   VK_D                    0x44
#define   VK_E                    0x45
#define   VK_F                    0x46
#define   VK_G                    0x47
#define   VK_H                    0x48
#define   VK_I                    0x49
#define   VK_J                    0x4a
#define   VK_K                    0x4b
#define   VK_L                    0x4c
#define   VK_M                    0x4d
#define   VK_N                    0x4e
#define   VK_O                    0x4f
#define   VK_P                    0x50
#define   VK_Q                    0x51
#define   VK_R                    0x52
#define   VK_S                    0x53
#define   VK_T                    0x54
#define   VK_U                    0x55
#define   VK_V                    0x56
#define   VK_W                    0x57
#define   VK_X                    0x58
#define   VK_Y                    0x59
#define   VK_Z                    0x5a


static unsigned int st_ScanNumLockVk[256]=
{
    VK_NULL          ,VK_ESCAPE        ,VK_1             ,VK_2             ,VK_3             ,    /* 005 */
    VK_4             ,VK_5             ,VK_6             ,VK_7             ,VK_8             ,    /* 010 */
    VK_9             ,VK_0             ,VK_OEM_MINUS     ,VK_OEM_PLUS      ,VK_BACK          ,    /* 015 */
    VK_TAB           ,VK_Q             ,VK_W             ,VK_E             ,VK_R             ,    /* 020 */
    VK_T             ,VK_Y             ,VK_U             ,VK_I             ,VK_O             ,    /* 025 */
    VK_P             ,VK_OEM_4         ,VK_OEM_6         ,VK_RETURN        ,VK_CONTROL       ,    /* 030 */
    VK_A             ,VK_S             ,VK_D             ,VK_F             ,VK_G             ,    /* 035 */
    VK_H             ,VK_J             ,VK_K             ,VK_L             ,VK_OEM_1         ,    /* 040 */
    VK_OEM_7         ,VK_OEM_3         ,VK_SHIFT         ,VK_OEM_5         ,VK_Z             ,    /* 045 */
    VK_X             ,VK_C             ,VK_V             ,VK_B             ,VK_N             ,    /* 050 */
    VK_M             ,VK_OEM_COMMA     ,VK_OEM_PERIOD    ,VK_OEM_2         ,VK_SHIFT         ,    /* 055 */
    VK_MULTIPLY      ,VK_MENU          ,VK_SPACE         ,VK_CAPITAL       ,VK_F1            ,    /* 060 */
    VK_F2            ,VK_F3            ,VK_F4            ,VK_F5            ,VK_F6            ,    /* 065 */
    VK_F7            ,VK_F8            ,VK_F9            ,VK_F10           ,VK_NUMLOCK       ,    /* 070 */
    VK_SCROLL        ,VK_NUMPAD7       ,VK_NUMPAD8       ,VK_NUMPAD9       ,VK_SUBTRACT      ,    /* 075 */
    VK_NUMPAD4       ,VK_NUMPAD5       ,VK_NUMPAD6       ,VK_ADD           ,VK_NUMPAD1       ,    /* 080 */
    VK_NUMPAD2       ,VK_NUMPAD3       ,VK_NUMPAD0       ,VK_DECIMAL       ,VK_NULL          ,    /* 085 */
    VK_NULL          ,VK_NULL          ,VK_F11           ,VK_F12           ,VK_NULL          ,    /* 090 */
    VK_NULL          ,VK_LWIN          ,VK_RWIN          ,VK_APPS          ,VK_NULL          ,    /* 095 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 100 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 105 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 110 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 115 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 120 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 125 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 130 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 135 */
    VK_SNAPSHOT      ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 140 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 145 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 150 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 155 */
    VK_NULL          ,VK_RETURN        ,VK_CONTROL       ,VK_NULL          ,VK_NULL          ,    /* 160 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 165 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 170 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 175 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 180 */
    VK_NULL          ,VK_DIVIDE        ,VK_NULL          ,VK_NULL          ,VK_MENU          ,    /* 185 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 190 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 195 */
    VK_NULL          ,VK_NULL          ,VK_PRINT         ,VK_NULL          ,VK_HOME          ,    /* 200 */
    VK_UP            ,VK_PRIOR         ,VK_NULL          ,VK_LEFT          ,VK_NULL          ,    /* 205 */
    VK_RIGHT         ,VK_NULL          ,VK_END           ,VK_DOWN          ,VK_NEXT          ,    /* 210 */
    VK_INSERT        ,VK_DELETE        ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 215 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 220 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 225 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 230 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 235 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 240 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 245 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 250 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 255 */
    VK_NULL          ,
};


static unsigned int st_ScanNumNoLockvk[256]=
{
    VK_NULL          ,VK_ESCAPE        ,VK_1             ,VK_2             ,VK_3             ,    /* 005 */
    VK_4             ,VK_5             ,VK_6             ,VK_7             ,VK_8             ,    /* 010 */
    VK_9             ,VK_0             ,VK_OEM_MINUS     ,VK_OEM_PLUS      ,VK_BACK          ,    /* 015 */
    VK_TAB           ,VK_Q             ,VK_W             ,VK_E             ,VK_R             ,    /* 020 */
    VK_T             ,VK_Y             ,VK_U             ,VK_I             ,VK_O             ,    /* 025 */
    VK_P             ,VK_OEM_4         ,VK_OEM_6         ,VK_RETURN        ,VK_CONTROL       ,    /* 030 */
    VK_A             ,VK_S             ,VK_D             ,VK_F             ,VK_G             ,    /* 035 */
    VK_H             ,VK_J             ,VK_K             ,VK_L             ,VK_OEM_1         ,    /* 040 */
    VK_OEM_7         ,VK_OEM_3         ,VK_SHIFT         ,VK_OEM_5         ,VK_Z             ,    /* 045 */
    VK_X             ,VK_C             ,VK_V             ,VK_B             ,VK_N             ,    /* 050 */
    VK_M             ,VK_OEM_COMMA     ,VK_OEM_PERIOD    ,VK_OEM_2         ,VK_SHIFT         ,    /* 055 */
    VK_MULTIPLY      ,VK_MENU          ,VK_SPACE         ,VK_CAPITAL       ,VK_F1            ,    /* 060 */
    VK_F2            ,VK_F3            ,VK_F4            ,VK_F5            ,VK_F6            ,    /* 065 */
    VK_F7            ,VK_F8            ,VK_F9            ,VK_F10           ,VK_NUMLOCK       ,    /* 070 */
    VK_SCROLL        ,VK_HOME          ,VK_UP            ,VK_PRIOR         ,VK_SUBTRACT      ,    /* 075 */
    VK_LEFT          ,VK_CLEAR         ,VK_RIGHT         ,VK_ADD           ,VK_END           ,    /* 080 */
    VK_DOWN          ,VK_NEXT          ,VK_INSERT        ,VK_DELETE        ,VK_NULL          ,    /* 085 */
    VK_NULL          ,VK_NULL          ,VK_F11           ,VK_F12           ,VK_NULL          ,    /* 090 */
    VK_NULL          ,VK_LWIN          ,VK_RWIN          ,VK_APPS          ,VK_NULL          ,    /* 095 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 100 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 105 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 110 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 115 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 120 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 125 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 130 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 135 */
    VK_SNAPSHOT      ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 140 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 145 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 150 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 155 */
    VK_NULL          ,VK_RETURN        ,VK_CONTROL       ,VK_NULL          ,VK_NULL          ,    /* 160 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 165 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 170 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 175 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 180 */
    VK_NULL          ,VK_DIVIDE        ,VK_NULL          ,VK_NULL          ,VK_MENU          ,    /* 185 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 190 */
    VK_NULL          ,VK_NULL          ,VK_PRINT         ,VK_NULL          ,VK_NULL          ,    /* 195 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_HOME          ,    /* 200 */
    VK_UP            ,VK_PRIOR         ,VK_NULL          ,VK_LEFT          ,VK_NULL          ,    /* 205 */
    VK_RIGHT         ,VK_NULL          ,VK_END           ,VK_DOWN          ,VK_NEXT          ,    /* 210 */
    VK_INSERT        ,VK_DELETE        ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 215 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 220 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 225 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 230 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 235 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 240 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 245 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 250 */
    VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,VK_NULL          ,    /* 255 */
    VK_NULL          ,
};


static unsigned char st_VkExtMap[256]=
{
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*005*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*010*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*015*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*020*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*025*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*030*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*035*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*040*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*045*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*050*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*055*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*060*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*065*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*070*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*075*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*080*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*085*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*090*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*095*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*100*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*105*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*110*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*115*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*120*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*125*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*130*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*135*/
    0x2a    ,0x0     ,0x0     ,0x0     ,0x0     ,     /*140*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*145*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*150*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*155*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*160*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*165*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*170*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*175*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*180*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*185*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*190*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*195*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x2a    ,     /*200*/
    0x2a    ,0x2a    ,0x0     ,0x2a    ,0x0     ,     /*205*/
    0x2a    ,0x0     ,0x2a    ,0x2a    ,0x2a    ,     /*210*/
    0x2a    ,0x2a    ,0x0     ,0x0     ,0x0     ,     /*215*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*220*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*225*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*230*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*235*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*240*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*245*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*250*/
    0x0     ,0x0     ,0x0     ,0x0     ,0x0     ,     /*255*/
    0x0     ,
};


int main(int argc, char* argv[])
{
    int i;

    for(i=0; i<256; i++)
    {
        if(st_VkExtMap[i])
        {
            fprintf(stdout,"numlockextened  0x%02x 0x%02x\n",i,st_ScanNumLockVk[i]);
        }
    }
    return 0;
}

