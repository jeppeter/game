// dikmaptest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#define  MAP_CHAR_NULL           0

#define  MAP_CHAR_CTRL_A         0x01
#define  MAP_CHAR_CTRL_B         0x02
#define  MAP_CHAR_CTRL_C         0x03
#define  MAP_CHAR_CTRL_D         0x04
#define  MAP_CHAR_CTRL_E         0x05
#define  MAP_CHAR_CTRL_F         0x06
#define  MAP_CHAR_CTRL_G         0x07
#define  MAP_CHAR_CTRL_H         0x08
#define  MAP_CHAR_CTRL_I         0x09
#define  MAP_CHAR_CTRL_J         0x0a
#define  MAP_CHAR_CTRL_K         0x0b
#define  MAP_CHAR_CTRL_L         0x0c
#define  MAP_CHAR_CTRL_M         0x0d
#define  MAP_CHAR_CTRL_N         0x0e
#define  MAP_CHAR_CTRL_O         0x0f
#define  MAP_CHAR_CTRL_P         0x10
#define  MAP_CHAR_CTRL_Q         0x11
#define  MAP_CHAR_CTRL_R         0x12
#define  MAP_CHAR_CTRL_S         0x13
#define  MAP_CHAR_CTRL_T         0x14
#define  MAP_CHAR_CTRL_U         0x15
#define  MAP_CHAR_CTRL_V         0x16
#define  MAP_CHAR_CTRL_W         0x17
#define  MAP_CHAR_CTRL_X         0x18
#define  MAP_CHAR_CTRL_Y         0x19
#define  MAP_CHAR_CTRL_Z         0x1a

#define  MAP_CHAR_BACKSPACE      0x08
#define  MAP_CHAR_TAB            0x09
#define  MAP_CHAR_LINEFEED       0x0a

#define  MAP_CHAR_ENTER          0x0d

#define  MAP_CHAR_ESCAPE         0x1b
#define  MAP_CHAR_CTRL_LBRACKET  0x1b
#define  MAP_CHAR_CTRL_SLASH     0x1c
#define  MAP_CHAR_CTRL_RBRACKET  0x1d

#define  MAP_CHAR_SPACE          0x20
#define  MAP_CHAR_EMOTION        0x21
#define  MAP_CHAR_DOUBLE_QUOTE   0x22
#define  MAP_CHAR_NUMSIGN        0x23
#define  MAP_CHAR_DOLLAR         0x24
#define  MAP_CHAR_PERCENT        0x25
#define  MAP_CHAR_AMPERSAND      0x26
#define  MAP_CHAR_SINGLE_QUOTE   0x27

#define  MAP_CHAR_LPAREN         0x28
#define  MAP_CHAR_RPAREN         0x29

#define  MAP_CHAR_NUMPAD_STAR    0x2a
#define  MAP_CHAR_NUMPAD_PLUS    0x2b
#define  MAP_CHAR_COMMA          0x2c
#define  MAP_CHAR_NUMPAD_MINUS   0x2d
#define  MAP_CHAR_MINUS          0x2d
#define  MAP_CHAR_DOT            0x2e
#define  MAP_CHAR_NUMPAD_SLASH   0x2f



#define  MAP_CHAR_0              0x30
#define  MAP_CHAR_1              0x31
#define  MAP_CHAR_2              0x32
#define  MAP_CHAR_3              0x33
#define  MAP_CHAR_4              0x34
#define  MAP_CHAR_5              0x35
#define  MAP_CHAR_6              0x36
#define  MAP_CHAR_7              0x37
#define  MAP_CHAR_8              0x38
#define  MAP_CHAR_9              0x39

#define  MAP_CHAR_COLON          0x3a
#define  MAP_CHAR_SEMICOLON      0x3b
#define  MAP_CHAR_LESS           0x3c
#define  MAP_CHAR_EQUAL          0x3d
#define  MAP_CHAR_GREATER        0x3e
#define  MAP_CHAR_QUESTION       0x3f

#define  MAP_CHAR_ATSYMBOL       0x40


#define  MAP_CHAR_A              0x41
#define  MAP_CHAR_B              0x42
#define  MAP_CHAR_C              0x43
#define  MAP_CHAR_D              0x44
#define  MAP_CHAR_E              0x45
#define  MAP_CHAR_F              0x46
#define  MAP_CHAR_G              0x47
#define  MAP_CHAR_H              0x48
#define  MAP_CHAR_I              0x49
#define  MAP_CHAR_J              0x4a
#define  MAP_CHAR_K              0x4b
#define  MAP_CHAR_L              0x4c
#define  MAP_CHAR_M              0x4d
#define  MAP_CHAR_N              0x4e
#define  MAP_CHAR_O              0x4f
#define  MAP_CHAR_P              0x50
#define  MAP_CHAR_Q              0x51
#define  MAP_CHAR_R              0x52
#define  MAP_CHAR_S              0x53
#define  MAP_CHAR_T              0x54
#define  MAP_CHAR_U              0x55
#define  MAP_CHAR_V              0x56
#define  MAP_CHAR_W              0x57
#define  MAP_CHAR_X              0x58
#define  MAP_CHAR_Y              0x59
#define  MAP_CHAR_Z              0x5a

#define  MAP_CHAR_LBRACKET       0x5b
#define  MAP_CHAR_BACKSLASH      0x5c
#define  MAP_CHAR_RBRACKET       0x5d
#define  MAP_CHAR_CARET          0x5e
#define  MAP_CHAR_UNDERSCORE     0x5f
#define  MAP_CHAR_APOSTROPHY     0x60

#define  MAP_CHAR_a              0x61
#define  MAP_CHAR_b              0x62
#define  MAP_CHAR_c              0x63
#define  MAP_CHAR_d              0x64
#define  MAP_CHAR_e              0x65
#define  MAP_CHAR_f              0x66
#define  MAP_CHAR_g              0x67
#define  MAP_CHAR_h              0x68
#define  MAP_CHAR_i              0x69
#define  MAP_CHAR_j              0x6a
#define  MAP_CHAR_k              0x6b
#define  MAP_CHAR_l              0x6c
#define  MAP_CHAR_m              0x6d
#define  MAP_CHAR_n              0x6e
#define  MAP_CHAR_o              0x6f
#define  MAP_CHAR_p              0x70
#define  MAP_CHAR_q              0x71
#define  MAP_CHAR_r              0x72
#define  MAP_CHAR_s              0x73
#define  MAP_CHAR_t              0x74
#define  MAP_CHAR_u              0x75
#define  MAP_CHAR_v              0x76
#define  MAP_CHAR_w              0x77
#define  MAP_CHAR_x              0x78
#define  MAP_CHAR_y              0x79
#define  MAP_CHAR_z              0x7a
#define  MAP_CHAR_LBRACE         0x7b
#define  MAP_CHAR_VERT_BAR       0x7c
#define  MAP_CHAR_RBRACE         0x7d

#define  MAP_CHAR_TILDE          0x7e
#define  MAP_CHAR_CTRL_BACKSPACE 0x7f


static unsigned char st_CapsChar[256] =
{
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*5*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_NULL          ,MAP_CHAR_BACKSPACE     ,MAP_CHAR_TAB           ,            /*10*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_NULL          ,MAP_CHAR_ENTER         ,MAP_CHAR_NULL          ,            /*15*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL 	      ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*20*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL 	      ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*25*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_ESCAPE        ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*30*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_SPACE         ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*35*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL	      ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*40*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL 	      ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*45*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_NULL          ,MAP_CHAR_0             ,MAP_CHAR_1             ,            /*50*/
    MAP_CHAR_2          ,MAP_CHAR_3           ,MAP_CHAR_4             ,MAP_CHAR_5             ,MAP_CHAR_6             ,            /*55*/
    MAP_CHAR_7          ,MAP_CHAR_8           ,MAP_CHAR_9             ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*60*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*65*/
    MAP_CHAR_A          ,MAP_CHAR_B           ,MAP_CHAR_C             ,MAP_CHAR_D             ,MAP_CHAR_E             ,            /*70*/
    MAP_CHAR_F          ,MAP_CHAR_G           ,MAP_CHAR_H             ,MAP_CHAR_I             ,MAP_CHAR_J             ,            /*75*/
    MAP_CHAR_K          ,MAP_CHAR_L           ,MAP_CHAR_M             ,MAP_CHAR_N             ,MAP_CHAR_O             ,            /*80*/
    MAP_CHAR_P          ,MAP_CHAR_Q           ,MAP_CHAR_R             ,MAP_CHAR_S             ,MAP_CHAR_T             ,            /*85*/
    MAP_CHAR_U          ,MAP_CHAR_V           ,MAP_CHAR_W             ,MAP_CHAR_X             ,MAP_CHAR_Y             ,            /*90*/
    MAP_CHAR_Z          ,MAP_CHAR_NULL        ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*95*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL 	      ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*100*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL 	      ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*105*/
    MAP_CHAR_NULL       ,MAP_CHAR_NUMPAD_STAR ,MAP_CHAR_NUMPAD_PLUS   ,MAP_CHAR_NULL          ,MAP_CHAR_NUMPAD_MINUS  ,            /*110*/
    MAP_CHAR_NULL       ,MAP_CHAR_NUMPAD_SLASH,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*115*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*120*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*125*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*130*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*135*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*140*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*145*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*150*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*155*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*160*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*165*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*170*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*175*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*180*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*185*/
    MAP_CHAR_NULL       ,MAP_CHAR_SEMICOLON   ,MAP_CHAR_EQUAL         ,MAP_CHAR_COMMA         ,MAP_CHAR_MINUS         ,            /*190*/
    MAP_CHAR_DOT        ,MAP_CHAR_NUMPAD_SLASH,MAP_CHAR_APOSTROPHY    ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*195*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*200*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*205*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*210*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*215*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_LBRACKET	  , 		   /*220*/
    MAP_CHAR_BACKSLASH  ,MAP_CHAR_RBRACKET    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*225*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL    	  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*230*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*235*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*240*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*245*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*250*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*255*/
    MAP_CHAR_NULL
};

int CapsCharTrans(int vk)
{
    int ret=MAP_CHAR_NULL;
    switch(vk)
    {
    case 67:
        ret=67;
        break;
    case 32:
        ret=32;
        break;
    case 90:
        ret=90;
        break;
    case 71:
        ret=71;
        break;
    case 70:
        ret=70;
        break;
    case 188:
        ret=44;
        break;
    case 68:
        ret=68;
        break;
    case 80:
        ret=80;
        break;
    case 88:
        ret=88;
        break;
    case 72:
        ret=72;
        break;
    case 186:
        ret=59;
        break;
    case 82:
        ret=82;
        break;
    case 55:
        ret=55;
        break;
    case 27:
        ret=27;
        break;
    case 74:
        ret=74;
        break;
    case 84:
        ret=84;
        break;
    case 190:
        ret=46;
        break;
    case 75:
        ret=75;
        break;
    case 83:
        ret=83;
        break;
    case 57:
        ret=57;
        break;
    case 192:
        ret=96;
        break;
    case 220:
        ret=92;
        break;
    case 109:
        ret=45;
        break;
    case 69:
        ret=69;
        break;
    case 191:
        ret=47;
        break;
    case 49:
        ret=49;
        break;
    case 89:
        ret=89;
        break;
    case 187:
        ret=61;
        break;
    case 189:
        ret=45;
        break;
    case 53:
        ret=53;
        break;
    case 78:
        ret=78;
        break;
    case 107:
        ret=43;
        break;
    case 48:
        ret=48;
        break;
    case 79:
        ret=79;
        break;
    case 87:
        ret=87;
        break;
    case 77:
        ret=77;
        break;
    case 106:
        ret=42;
        break;
    case 219:
        ret=91;
        break;
    case 13:
        ret=13;
        break;
    case 65:
        ret=65;
        break;
    case 50:
        ret=50;
        break;
    case 221:
        ret=93;
        break;
    case 85:
        ret=85;
        break;
    case 111:
        ret=47;
        break;
    case 9:
        ret=9;
        break;
    case 51:
        ret=51;
        break;
    case 81:
        ret=81;
        break;
    case 8:
        ret=8;
        break;
    case 52:
        ret=52;
        break;
    case 56:
        ret=56;
        break;
    case 66:
        ret=66;
        break;
    case 73:
        ret=73;
        break;
    case 86:
        ret=86;
        break;
    case 76:
        ret=76;
        break;
    case 54:
        ret=54;
        break;
    }
    return ret;
}


static unsigned char st_NormChar[256] =
{
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*5*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL        ,MAP_CHAR_NULL          ,MAP_CHAR_BACKSPACE     ,MAP_CHAR_TAB           ,            /*10*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_ENTER		  ,MAP_CHAR_NULL		  , 		   /*15*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*20*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*25*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_ESCAPE		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*30*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_SPACE		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*35*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*40*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*45*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_0			  ,MAP_CHAR_1			  , 		   /*50*/
    MAP_CHAR_2			,MAP_CHAR_3 		  ,MAP_CHAR_4			  ,MAP_CHAR_5			  ,MAP_CHAR_6			  , 		   /*55*/
    MAP_CHAR_7			,MAP_CHAR_8 		  ,MAP_CHAR_9			  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*60*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*65*/
    MAP_CHAR_a          ,MAP_CHAR_b           ,MAP_CHAR_c             ,MAP_CHAR_d             ,MAP_CHAR_e             ,            /*70*/
    MAP_CHAR_f			,MAP_CHAR_g 		  ,MAP_CHAR_h			  ,MAP_CHAR_i			  ,MAP_CHAR_j			  , 		   /*75*/
    MAP_CHAR_k			,MAP_CHAR_l 		  ,MAP_CHAR_m			  ,MAP_CHAR_n			  ,MAP_CHAR_o			  , 		   /*80*/
    MAP_CHAR_p			,MAP_CHAR_q 		  ,MAP_CHAR_r			  ,MAP_CHAR_s			  ,MAP_CHAR_t			  , 		   /*85*/
    MAP_CHAR_u			,MAP_CHAR_v 		  ,MAP_CHAR_w			  ,MAP_CHAR_x			  ,MAP_CHAR_y			  , 		   /*90*/
    MAP_CHAR_z          ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*95*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL    	  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*100*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*105*/
    MAP_CHAR_NULL       ,MAP_CHAR_NUMPAD_STAR ,MAP_CHAR_NUMPAD_PLUS   ,MAP_CHAR_NULL          ,MAP_CHAR_NUMPAD_MINUS  ,            /*110*/
    MAP_CHAR_NULL       ,MAP_CHAR_NUMPAD_SLASH,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*115*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*120*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*125*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*130*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*135*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*140*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*145*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*150*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*155*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*160*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*165*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*170*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*175*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*180*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*185*/
    MAP_CHAR_NULL       ,MAP_CHAR_SEMICOLON   ,MAP_CHAR_EQUAL         ,MAP_CHAR_COMMA         ,MAP_CHAR_MINUS         ,            /*190*/
    MAP_CHAR_DOT        ,MAP_CHAR_NUMPAD_SLASH,MAP_CHAR_APOSTROPHY    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*195*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*200*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*205*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*210*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*215*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_LBRACKET	  , 		   /*220*/
    MAP_CHAR_BACKSLASH  ,MAP_CHAR_RBRACKET    ,MAP_CHAR_SINGLE_QUOTE  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*225*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*230*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*235*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*240*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*245*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*250*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*255*/
    MAP_CHAR_NULL
};

int NormCharTrans(int vk)
{
    int ret=MAP_CHAR_NULL;
    switch(vk)
    {
    case 32:
        ret=32;
        break;
    case 67:
        ret=99;
        break;
    case 90:
        ret=122;
        break;
    case 71:
        ret=103;
        break;
    case 70:
        ret=102;
        break;
    case 188:
        ret=44;
        break;
    case 68:
        ret=100;
        break;
    case 80:
        ret=112;
        break;
    case 88:
        ret=120;
        break;
    case 72:
        ret=104;
        break;
    case 186:
        ret=59;
        break;
    case 222:
        ret=39;
        break;
    case 82:
        ret=114;
        break;
    case 55:
        ret=55;
        break;
    case 27:
        ret=27;
        break;
    case 74:
        ret=106;
        break;
    case 84:
        ret=116;
        break;
    case 190:
        ret=46;
        break;
    case 75:
        ret=107;
        break;
    case 83:
        ret=115;
        break;
    case 57:
        ret=57;
        break;
    case 192:
        ret=96;
        break;
    case 220:
        ret=92;
        break;
    case 109:
        ret=45;
        break;
    case 69:
        ret=101;
        break;
    case 191:
        ret=47;
        break;
    case 49:
        ret=49;
        break;
    case 89:
        ret=121;
        break;
    case 187:
        ret=61;
        break;
    case 189:
        ret=45;
        break;
    case 53:
        ret=53;
        break;
    case 78:
        ret=110;
        break;
    case 107:
        ret=43;
        break;
    case 48:
        ret=48;
        break;
    case 79:
        ret=111;
        break;
    case 87:
        ret=119;
        break;
    case 77:
        ret=109;
        break;
    case 106:
        ret=42;
        break;
    case 219:
        ret=91;
        break;
    case 13:
        ret=13;
        break;
    case 65:
        ret=97;
        break;
    case 50:
        ret=50;
        break;
    case 221:
        ret=93;
        break;
    case 85:
        ret=117;
        break;
    case 111:
        ret=47;
        break;
    case 9:
        ret=9;
        break;
    case 51:
        ret=51;
        break;
    case 81:
        ret=113;
        break;
    case 8:
        ret=8;
        break;
    case 52:
        ret=52;
        break;
    case 56:
        ret=56;
        break;
    case 66:
        ret=98;
        break;
    case 73:
        ret=105;
        break;
    case 86:
        ret=118;
        break;
    case 76:
        ret=108;
        break;
    case 54:
        ret=54;
        break;
    }
    return ret;
}


static unsigned char st_CtrlChar[256] =
{
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_CTRL_C        ,MAP_CHAR_NULL		  , 		   /*5*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL          ,MAP_CHAR_NULL        ,MAP_CHAR_CTRL_BACKSPACE,MAP_CHAR_NULL          ,            /*10*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL          ,MAP_CHAR_NULL        ,MAP_CHAR_LINEFEED      ,MAP_CHAR_NULL          ,            /*15*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL    	  ,MAP_CHAR_NULL		  , 		   /*20*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*25*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*30*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*35*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*40*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*45*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*50*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*55*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*60*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*65*/
    MAP_CHAR_CTRL_A     ,MAP_CHAR_CTRL_B        ,MAP_CHAR_CTRL_C      ,MAP_CHAR_CTRL_D        ,MAP_CHAR_CTRL_E        ,            /*70*/
    MAP_CHAR_CTRL_F 	,MAP_CHAR_CTRL_G	    ,MAP_CHAR_CTRL_H      ,MAP_CHAR_CTRL_I		  ,MAP_CHAR_CTRL_J		  , 		   /*75*/
    MAP_CHAR_CTRL_K 	,MAP_CHAR_CTRL_L	    ,MAP_CHAR_CTRL_M      ,MAP_CHAR_CTRL_N		  ,MAP_CHAR_CTRL_O		  , 		   /*80*/
    MAP_CHAR_CTRL_P 	,MAP_CHAR_CTRL_Q	    ,MAP_CHAR_CTRL_R      ,MAP_CHAR_CTRL_S		  ,MAP_CHAR_CTRL_T		  , 		   /*85*/
    MAP_CHAR_CTRL_U 	,MAP_CHAR_CTRL_V	    ,MAP_CHAR_CTRL_W      ,MAP_CHAR_CTRL_X		  ,MAP_CHAR_CTRL_Y		  , 		   /*90*/
    MAP_CHAR_CTRL_Z     ,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*95*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*100*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*105*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*110*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*115*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*120*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*125*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*130*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*135*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*140*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*145*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*150*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*155*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*160*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*165*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*170*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*175*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*180*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*185*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*190*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*195*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*200*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*205*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*210*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*215*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_CTRL_LBRACKET , 		   /*220*/
    MAP_CHAR_CTRL_SLASH ,MAP_CHAR_CTRL_RBRACKET ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*225*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*230*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*235*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*240*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*245*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*250*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*255*/
    MAP_CHAR_NULL
};

int CtrlCharTrans(int vk)
{
    int ret=MAP_CHAR_NULL;
    switch(vk)
    {
    case 67:
        ret=3;
        break;
    case 90:
        ret=26;
        break;
    case 71:
        ret=7;
        break;
    case 70:
        ret=6;
        break;
    case 68:
        ret=4;
        break;
    case 80:
        ret=16;
        break;
    case 88:
        ret=24;
        break;
    case 72:
        ret=8;
        break;
    case 82:
        ret=18;
        break;
    case 74:
        ret=10;
        break;
    case 84:
        ret=20;
        break;
    case 75:
        ret=11;
        break;
    case 83:
        ret=19;
        break;
    case 220:
        ret=28;
        break;
    case 69:
        ret=5;
        break;
    case 89:
        ret=25;
        break;
    case 78:
        ret=14;
        break;
    case 79:
        ret=15;
        break;
    case 87:
        ret=23;
        break;
    case 77:
        ret=13;
        break;
    case 219:
        ret=27;
        break;
    case 13:
        ret=10;
        break;
    case 65:
        ret=1;
        break;
    case 221:
        ret=29;
        break;
    case 85:
        ret=21;
        break;
    case 3:
        ret=3;
        break;
    case 8:
        ret=127;
        break;
    case 81:
        ret=17;
        break;
    case 73:
        ret=9;
        break;
    case 66:
        ret=2;
        break;
    case 76:
        ret=12;
        break;
    case 86:
        ret=22;
        break;
    }
    return ret;
}


static unsigned char st_ShiftChar[256] =
{
    MAP_CHAR_NULL		,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*5*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_BACKSPACE	  ,MAP_CHAR_TAB  		  , 		   /*10*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL          ,MAP_CHAR_NULL        ,MAP_CHAR_ENTER         ,MAP_CHAR_NULL          ,            /*15*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*20*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*25*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL          ,MAP_CHAR_ESCAPE      ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*30*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL          ,MAP_CHAR_SPACE       ,MAP_CHAR_NULL          ,MAP_CHAR_NULL          ,            /*35*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*40*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*45*/
    MAP_CHAR_NULL       ,MAP_CHAR_NULL          ,MAP_CHAR_NULL        ,MAP_CHAR_RPAREN        ,MAP_CHAR_EMOTION       ,            /*50*/
    MAP_CHAR_ATSYMBOL   ,MAP_CHAR_NUMSIGN       ,MAP_CHAR_DOLLAR      ,MAP_CHAR_PERCENT       ,MAP_CHAR_CARET         ,            /*55*/
    MAP_CHAR_AMPERSAND  ,MAP_CHAR_NUMPAD_STAR   ,MAP_CHAR_LPAREN      ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*60*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL  	  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*65*/
    MAP_CHAR_A			,MAP_CHAR_B 		    ,MAP_CHAR_C			  ,MAP_CHAR_D			  ,MAP_CHAR_E			  , 		   /*70*/
    MAP_CHAR_F			,MAP_CHAR_G 		    ,MAP_CHAR_H			  ,MAP_CHAR_I			  ,MAP_CHAR_J			  , 		   /*75*/
    MAP_CHAR_K			,MAP_CHAR_L 		    ,MAP_CHAR_M			  ,MAP_CHAR_N			  ,MAP_CHAR_O			  , 		   /*80*/
    MAP_CHAR_P			,MAP_CHAR_Q 		    ,MAP_CHAR_R			  ,MAP_CHAR_S			  ,MAP_CHAR_T			  , 		   /*85*/
    MAP_CHAR_U			,MAP_CHAR_V 		    ,MAP_CHAR_W			  ,MAP_CHAR_X			  ,MAP_CHAR_Y			  , 		   /*90*/
    MAP_CHAR_Z			,MAP_CHAR_NULL		    ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*95*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*100*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*105*/
    MAP_CHAR_NULL       ,MAP_CHAR_NUMPAD_STAR   ,MAP_CHAR_NUMPAD_PLUS ,MAP_CHAR_NULL          ,MAP_CHAR_NUMPAD_MINUS  ,            /*110*/
    MAP_CHAR_NULL       ,MAP_CHAR_NUMPAD_SLASH  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*115*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*120*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*125*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*130*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*135*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*140*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*145*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*150*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*155*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*160*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*165*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*170*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*175*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*180*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*185*/
    MAP_CHAR_NULL       ,MAP_CHAR_COLON         ,MAP_CHAR_NUMPAD_PLUS ,MAP_CHAR_LESS          ,MAP_CHAR_UNDERSCORE    ,            /*190*/
    MAP_CHAR_GREATER    ,MAP_CHAR_QUESTION      ,MAP_CHAR_TILDE       ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*195*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*200*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*205*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*210*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*215*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_LBRACE    	  , 		   /*220*/
    MAP_CHAR_VERT_BAR   ,MAP_CHAR_RBRACE        ,MAP_CHAR_DOUBLE_QUOTE,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*225*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*230*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*235*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*240*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*245*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*250*/
    MAP_CHAR_NULL		,MAP_CHAR_NULL			,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  ,MAP_CHAR_NULL		  , 		   /*255*/
    MAP_CHAR_NULL
};

int ShiftCharTrans(int vk)
{
	int ret=MAP_CHAR_NULL;
	switch(vk)
	{
		case 32:
			ret=32;
			break;
		case 67:
			ret=67;
			break;
		case 90:
			ret=90;
			break;
		case 71:
			ret=71;
			break;
		case 70:
			ret=70;
			break;
		case 188:
			ret=60;
			break;
		case 68:
			ret=68;
			break;
		case 80:
			ret=80;
			break;
		case 88:
			ret=88;
			break;
		case 72:
			ret=72;
			break;
		case 186:
			ret=58;
			break;
		case 222:
			ret=34;
			break;
		case 82:
			ret=82;
			break;
		case 55:
			ret=38;
			break;
		case 27:
			ret=27;
			break;
		case 74:
			ret=74;
			break;
		case 84:
			ret=84;
			break;
		case 190:
			ret=62;
			break;
		case 75:
			ret=75;
			break;
		case 83:
			ret=83;
			break;
		case 57:
			ret=40;
			break;
		case 192:
			ret=126;
			break;
		case 220:
			ret=124;
			break;
		case 109:
			ret=45;
			break;
		case 69:
			ret=69;
			break;
		case 191:
			ret=63;
			break;
		case 49:
			ret=33;
			break;
		case 89:
			ret=89;
			break;
		case 187:
			ret=43;
			break;
		case 189:
			ret=95;
			break;
		case 53:
			ret=37;
			break;
		case 78:
			ret=78;
			break;
		case 107:
			ret=43;
			break;
		case 48:
			ret=41;
			break;
		case 79:
			ret=79;
			break;
		case 87:
			ret=87;
			break;
		case 77:
			ret=77;
			break;
		case 106:
			ret=42;
			break;
		case 219:
			ret=123;
			break;
		case 13:
			ret=13;
			break;
		case 65:
			ret=65;
			break;
		case 50:
			ret=64;
			break;
		case 221:
			ret=125;
			break;
		case 85:
			ret=85;
			break;
		case 111:
			ret=47;
			break;
		case 9:
			ret=9;
			break;
		case 51:
			ret=35;
			break;
		case 81:
			ret=81;
			break;
		case 8:
			ret=8;
			break;
		case 52:
			ret=36;
			break;
		case 56:
			ret=42;
			break;
		case 66:
			ret=66;
			break;
		case 73:
			ret=73;
			break;
		case 86:
			ret=86;
			break;
		case 76:
			ret=76;
			break;
		case 54:
			ret=94;
			break;
	}
	return ret;
}
int ShiftCapsCharTrans(int vk)
{
	int ret=MAP_CHAR_NULL;
	switch(vk)
	{
		case 32:
			ret=32;
			break;
		case 67:
			ret=99;
			break;
		case 90:
			ret=122;
			break;
		case 71:
			ret=103;
			break;
		case 70:
			ret=102;
			break;
		case 188:
			ret=60;
			break;
		case 68:
			ret=100;
			break;
		case 80:
			ret=112;
			break;
		case 88:
			ret=120;
			break;
		case 72:
			ret=104;
			break;
		case 186:
			ret=58;
			break;
		case 222:
			ret=34;
			break;
		case 82:
			ret=114;
			break;
		case 55:
			ret=38;
			break;
		case 74:
			ret=106;
			break;
		case 84:
			ret=116;
			break;
		case 27:
			ret=27;
			break;
		case 190:
			ret=62;
			break;
		case 75:
			ret=107;
			break;
		case 83:
			ret=115;
			break;
		case 57:
			ret=40;
			break;
		case 192:
			ret=126;
			break;
		case 220:
			ret=124;
			break;
		case 109:
			ret=45;
			break;
		case 69:
			ret=101;
			break;
		case 191:
			ret=63;
			break;
		case 49:
			ret=33;
			break;
		case 89:
			ret=121;
			break;
		case 187:
			ret=43;
			break;
		case 189:
			ret=95;
			break;
		case 53:
			ret=37;
			break;
		case 78:
			ret=110;
			break;
		case 107:
			ret=43;
			break;
		case 48:
			ret=41;
			break;
		case 79:
			ret=111;
			break;
		case 87:
			ret=119;
			break;
		case 77:
			ret=109;
			break;
		case 106:
			ret=42;
			break;
		case 219:
			ret=123;
			break;
		case 13:
			ret=13;
			break;
		case 65:
			ret=97;
			break;
		case 50:
			ret=64;
			break;
		case 221:
			ret=125;
			break;
		case 85:
			ret=117;
			break;
		case 111:
			ret=47;
			break;
		case 9:
			ret=9;
			break;
		case 51:
			ret=35;
			break;
		case 8:
			ret=8;
			break;
		case 52:
			ret=36;
			break;
		case 81:
			ret=113;
			break;
		case 56:
			ret=42;
			break;
		case 66:
			ret=98;
			break;
		case 73:
			ret=105;
			break;
		case 86:
			ret=118;
			break;
		case 76:
			ret=108;
			break;
		case 54:
			ret=94;
			break;
	}
	return ret;
}

int ShiftCapsCharTrans(int vk)
{
	int ret=MAP_CHAR_NULL;
	switch(vk)
	{
		case 32:
			ret=32;
			break;
		case 67:
			ret=99;
			break;
		case 90:
			ret=122;
			break;
		case 71:
			ret=103;
			break;
		case 70:
			ret=102;
			break;
		case 188:
			ret=60;
			break;
		case 68:
			ret=100;
			break;
		case 80:
			ret=112;
			break;
		case 88:
			ret=120;
			break;
		case 72:
			ret=104;
			break;
		case 186:
			ret=58;
			break;
		case 222:
			ret=34;
			break;
		case 82:
			ret=114;
			break;
		case 55:
			ret=38;
			break;
		case 74:
			ret=106;
			break;
		case 84:
			ret=116;
			break;
		case 27:
			ret=27;
			break;
		case 190:
			ret=62;
			break;
		case 75:
			ret=107;
			break;
		case 83:
			ret=115;
			break;
		case 57:
			ret=40;
			break;
		case 192:
			ret=126;
			break;
		case 220:
			ret=124;
			break;
		case 109:
			ret=45;
			break;
		case 69:
			ret=101;
			break;
		case 191:
			ret=63;
			break;
		case 49:
			ret=33;
			break;
		case 89:
			ret=121;
			break;
		case 187:
			ret=43;
			break;
		case 189:
			ret=95;
			break;
		case 53:
			ret=37;
			break;
		case 78:
			ret=110;
			break;
		case 107:
			ret=43;
			break;
		case 48:
			ret=41;
			break;
		case 79:
			ret=111;
			break;
		case 87:
			ret=119;
			break;
		case 77:
			ret=109;
			break;
		case 106:
			ret=42;
			break;
		case 219:
			ret=123;
			break;
		case 13:
			ret=13;
			break;
		case 65:
			ret=97;
			break;
		case 50:
			ret=64;
			break;
		case 221:
			ret=125;
			break;
		case 85:
			ret=117;
			break;
		case 111:
			ret=47;
			break;
		case 9:
			ret=9;
			break;
		case 51:
			ret=35;
			break;
		case 8:
			ret=8;
			break;
		case 52:
			ret=36;
			break;
		case 81:
			ret=113;
			break;
		case 56:
			ret=42;
			break;
		case 66:
			ret=98;
			break;
		case 73:
			ret=105;
			break;
		case 86:
			ret=118;
			break;
		case 76:
			ret=108;
			break;
		case 54:
			ret=94;
			break;
	}
	return ret;
}



typedef enum IO_KEYBOARD_CODE
{
    KEYBOARD_CODE_A = 0,
    KEYBOARD_CODE_B,
    KEYBOARD_CODE_C,
    KEYBOARD_CODE_D,
    KEYBOARD_CODE_E,          /*5*/
    KEYBOARD_CODE_F,
    KEYBOARD_CODE_G,
    KEYBOARD_CODE_H,
    KEYBOARD_CODE_I,
    KEYBOARD_CODE_J,          /*10*/
    KEYBOARD_CODE_K,
    KEYBOARD_CODE_L,
    KEYBOARD_CODE_M,
    KEYBOARD_CODE_N,
    KEYBOARD_CODE_O,          /*15*/
    KEYBOARD_CODE_P,
    KEYBOARD_CODE_Q,
    KEYBOARD_CODE_R,
    KEYBOARD_CODE_S,
    KEYBOARD_CODE_T,          /*20*/
    KEYBOARD_CODE_U,
    KEYBOARD_CODE_V,
    KEYBOARD_CODE_W,
    KEYBOARD_CODE_X,
    KEYBOARD_CODE_Y,          /*25*/
    KEYBOARD_CODE_Z,
    KEYBOARD_CODE_0,
    KEYBOARD_CODE_1,
    KEYBOARD_CODE_2,
    KEYBOARD_CODE_3,          /*30*/
    KEYBOARD_CODE_4,
    KEYBOARD_CODE_5,
    KEYBOARD_CODE_6,
    KEYBOARD_CODE_7,
    KEYBOARD_CODE_8,          /*35*/
    KEYBOARD_CODE_9,
    KEYBOARD_CODE_ESCAPE,
    KEYBOARD_CODE_MINUS,
    KEYBOARD_CODE_EQUALS,         /*equals */
    KEYBOARD_CODE_BACK,           /*back space*/        /*40*/
    KEYBOARD_CODE_TAB,            /**/
    KEYBOARD_CODE_LBRACKET,       /*[*/
    KEYBOARD_CODE_RBRACKET,       /*]*/
    KEYBOARD_CODE_RETURN,
    KEYBOARD_CODE_LCONTROL,                             /*45*/
    KEYBOARD_CODE_SEMICOLON,       /*;*/
    KEYBOARD_CODE_APOSTROPHE,      /*`*/
    KEYBOARD_CODE_GRAVE,           /*'*/
    KEYBOARD_CODE_LSHIFT,
    KEYBOARD_CODE_BACKSLASH,       /*\\*/               /*50*/
    KEYBOARD_CODE_COMMA,           /*,*/
    KEYBOARD_CODE_PERIOD,          /*.*/
    KEYBOARD_CODE_SLASH,           /* / */
    KEYBOARD_CODE_RSHIFT,
    KEYBOARD_CODE_NUM_MULTIPLY,    /* numpad * */       /*55*/
    KEYBOARD_CODE_LALT,
    KEYBOARD_CODE_SPACE,
    KEYBOARD_CODE_CAPITAL,         /*caps lock*/
    KEYBOARD_CODE_F1,
    KEYBOARD_CODE_F2,                                    /*60*/
    KEYBOARD_CODE_F3,
    KEYBOARD_CODE_F4,
    KEYBOARD_CODE_F5,
    KEYBOARD_CODE_F6,
    KEYBOARD_CODE_F7,                                    /*65*/
    KEYBOARD_CODE_F8,
    KEYBOARD_CODE_F9,
    KEYBOARD_CODE_F10,
    KEYBOARD_CODE_F11,
    KEYBOARD_CODE_F12,                                    /*70*/
    KEYBOARD_CODE_F13,
    KEYBOARD_CODE_F14,
    KEYBOARD_CODE_F15,
    KEYBOARD_CODE_NUMLOCK,
    KEYBOARD_CODE_SCROLL,                                 /*75*/
    KEYBOARD_CODE_SUBTRACT,         /*-*/
    KEYBOARD_CODE_NUM_0,
    KEYBOARD_CODE_NUM_1,
    KEYBOARD_CODE_NUM_2,
    KEYBOARD_CODE_NUM_3,                                  /*80*/
    KEYBOARD_CODE_NUM_4,
    KEYBOARD_CODE_NUM_5,
    KEYBOARD_CODE_NUM_6,
    KEYBOARD_CODE_NUM_7,
    KEYBOARD_CODE_NUM_8,                                  /*85*/
    KEYBOARD_CODE_NUM_9,
    KEYBOARD_CODE_NUM_ADD,
    KEYBOARD_CODE_DECIMAL,          /* . on numeric keypad */
    KEYBOARD_CODE_OEM_102,          /* <> or \| on RT 102-key keyboard (Non-U.S.) */
    KEYBOARD_CODE_KANA,             /* (Japanese keyboard)            */    /*90*/
    KEYBOARD_CODE_ABNT_C1,          /* /? on Brazilian keyboard */
    KEYBOARD_CODE_CONVERT,          /* (Japanese keyboard)            */
    KEYBOARD_CODE_NONCONVERT,       /* (Japanese keyboard)            */
    KEYBOARD_CODE_YEN,              /* (Japanese keyboard)            */
    KEYBOARD_CODE_ABNT_C2,          /* Numpad . on Brazilian keyboard */   /*95*/
    KEYBOARD_CODE_NUM_EQUALS,       /* = on numeric keypad (NEC PC98) */
    KEYBOARD_CODE_PREV_TRACK,       /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
    KEYBOARD_CODE_AT,               /*                     (NEC PC98) */
    KEYBOARD_CODE_COLON,            /*                     (NEC PC98) */
    KEYBOARD_CODE_UNDERLINE,        /*                     (NEC PC98) */         /*100*/
    KEYBOARD_CODE_KANJI,            /* (Japanese keyboard)            */
    KEYBOARD_CODE_STOP,             /*                     (NEC PC98) */
    KEYBOARD_CODE_AX,               /*                     (Japan AX) */
    KEYBOARD_CODE_UNLABELED,        /*                        (J3100) */
    KEYBOARD_CODE_NEXT_TRACK,       /* Next Track */                       /*105*/
    KEYBOARD_CODE_NUM_ENTER,
    KEYBOARD_CODE_RCONTROL,
    KEYBOARD_CODE_MUTE,
    KEYBOARD_CODE_CALCULATOR,
    KEYBOARD_CODE_PLAY_PAUSE,                                             /*110*/
    KEYBOARD_CODE_MEDIA_STOP,
    KEYBOARD_CODE_VOLUME_DOWN,
    KEYBOARD_CODE_VOLUME_UP,
    KEYBOARD_CODE_WEB_HOME,
    KEYBOARD_CODE_NUM_COMMA,        /* , on numeric keypad (NEC PC98) */     /*115*/
    KEYBOARD_CODE_NUM_DIVIDE,       /* / on numeric keypad */
    KEYBOARD_CODE_SYSRQ,
    KEYBOARD_CODE_RALT,
    KEYBOARD_CODE_PAUSE,
    KEYBOARD_CODE_HOME,                                                     /*120*/
    KEYBOARD_CODE_UP,
    KEYBOARD_CODE_PRIOR,            /*pageup*/
    KEYBOARD_CODE_LEFT,
    KEYBOARD_CODE_RIGHT,
    KEYBOARD_CODE_END,                                                      /*125*/
    KEYBOARD_CODE_DOWN,
    KEYBOARD_CODE_NEXT,             /*PAGEDOWN*/
    KEYBOARD_CODE_INSERT,
    KEYBOARD_CODE_DELETE,
    KEYBOARD_CODE_LWIN,                                                      /*130*/
    KEYBOARD_CODE_RWIN,
    KEYBOARD_CODE_APPMENU,
    KEYBOARD_CODE_POWER,
    KEYBOARD_CODE_SLEEP,
    KEYBOARD_CODE_WAKE,                                                      /*135*/
    KEYBOARD_CODE_WEB_SEARCH,
    KEYBOARD_CODE_WEB_FAVORITES,
    KEYBOARD_CODE_WEB_REFRESH,
    KEYBOARD_CODE_WEB_STOP,
    KEYBOARD_CODE_WEB_FORWARD,                                                /*140*/
    KEYBOARD_CODE_WEB_BACK,
    KEYBOARD_CODE_MY_COMPUTER,
    KEYBOARD_CODE_MAIL,
    KEYBOARD_CODE_MEDIA_SELECT,

    KEYBOARD_CODE_NULL = 255
} IO_KEYBOARD_CODE_t,*PIO_KEYBOARD_CODE_t;

/****************************************************************************
 *
 *      DirectInput keyboard scan codes
 *
 ****************************************************************************/
#define DIK_ESCAPE          0x01
#define DIK_1               0x02
#define DIK_2               0x03
#define DIK_3               0x04
#define DIK_4               0x05
#define DIK_5               0x06
#define DIK_6               0x07
#define DIK_7               0x08
#define DIK_8               0x09
#define DIK_9               0x0A
#define DIK_0               0x0B
#define DIK_MINUS           0x0C    /* - on main keyboard */
#define DIK_EQUALS          0x0D
#define DIK_BACK            0x0E    /* backspace */
#define DIK_TAB             0x0F
#define DIK_Q               0x10
#define DIK_W               0x11
#define DIK_E               0x12
#define DIK_R               0x13
#define DIK_T               0x14
#define DIK_Y               0x15
#define DIK_U               0x16
#define DIK_I               0x17
#define DIK_O               0x18
#define DIK_P               0x19
#define DIK_LBRACKET        0x1A
#define DIK_RBRACKET        0x1B
#define DIK_RETURN          0x1C    /* Enter on main keyboard */
#define DIK_LCONTROL        0x1D
#define DIK_A               0x1E
#define DIK_S               0x1F
#define DIK_D               0x20
#define DIK_F               0x21
#define DIK_G               0x22
#define DIK_H               0x23
#define DIK_J               0x24
#define DIK_K               0x25
#define DIK_L               0x26
#define DIK_SEMICOLON       0x27
#define DIK_APOSTROPHE      0x28
#define DIK_GRAVE           0x29    /* accent grave */
#define DIK_LSHIFT          0x2A
#define DIK_BACKSLASH       0x2B
#define DIK_Z               0x2C
#define DIK_X               0x2D
#define DIK_C               0x2E
#define DIK_V               0x2F
#define DIK_B               0x30
#define DIK_N               0x31
#define DIK_M               0x32
#define DIK_COMMA           0x33
#define DIK_PERIOD          0x34    /* . on main keyboard */
#define DIK_SLASH           0x35    /* / on main keyboard */
#define DIK_RSHIFT          0x36
#define DIK_MULTIPLY        0x37    /* * on numeric keypad */
#define DIK_LMENU           0x38    /* left Alt */
#define DIK_SPACE           0x39
#define DIK_CAPITAL         0x3A
#define DIK_F1              0x3B
#define DIK_F2              0x3C
#define DIK_F3              0x3D
#define DIK_F4              0x3E
#define DIK_F5              0x3F
#define DIK_F6              0x40
#define DIK_F7              0x41
#define DIK_F8              0x42
#define DIK_F9              0x43
#define DIK_F10             0x44
#define DIK_NUMLOCK         0x45
#define DIK_SCROLL          0x46    /* Scroll Lock */
#define DIK_NUMPAD7         0x47
#define DIK_NUMPAD8         0x48
#define DIK_NUMPAD9         0x49
#define DIK_SUBTRACT        0x4A    /* - on numeric keypad */
#define DIK_NUMPAD4         0x4B
#define DIK_NUMPAD5         0x4C
#define DIK_NUMPAD6         0x4D
#define DIK_ADD             0x4E    /* + on numeric keypad */
#define DIK_NUMPAD1         0x4F
#define DIK_NUMPAD2         0x50
#define DIK_NUMPAD3         0x51
#define DIK_NUMPAD0         0x52
#define DIK_DECIMAL         0x53    /* . on numeric keypad */
#define DIK_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define DIK_F11             0x57
#define DIK_F12             0x58
#define DIK_F13             0x64    /*                     (NEC PC98) */
#define DIK_F14             0x65    /*                     (NEC PC98) */
#define DIK_F15             0x66    /*                     (NEC PC98) */
#define DIK_KANA            0x70    /* (Japanese keyboard)            */
#define DIK_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define DIK_CONVERT         0x79    /* (Japanese keyboard)            */
#define DIK_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define DIK_YEN             0x7D    /* (Japanese keyboard)            */
#define DIK_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define DIK_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define DIK_PREVTRACK       0x90    /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
#define DIK_AT              0x91    /*                     (NEC PC98) */
#define DIK_COLON           0x92    /*                     (NEC PC98) */
#define DIK_UNDERLINE       0x93    /*                     (NEC PC98) */
#define DIK_KANJI           0x94    /* (Japanese keyboard)            */
#define DIK_STOP            0x95    /*                     (NEC PC98) */
#define DIK_AX              0x96    /*                     (Japan AX) */
#define DIK_UNLABELED       0x97    /*                        (J3100) */
#define DIK_NEXTTRACK       0x99    /* Next Track */
#define DIK_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define DIK_RCONTROL        0x9D
#define DIK_MUTE            0xA0    /* Mute */
#define DIK_CALCULATOR      0xA1    /* Calculator */
#define DIK_PLAYPAUSE       0xA2    /* Play / Pause */
#define DIK_MEDIASTOP       0xA4    /* Media Stop */
#define DIK_VOLUMEDOWN      0xAE    /* Volume - */
#define DIK_VOLUMEUP        0xB0    /* Volume + */
#define DIK_WEBHOME         0xB2    /* Web home */
#define DIK_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define DIK_DIVIDE          0xB5    /* / on numeric keypad */
#define DIK_SYSRQ           0xB7
#define DIK_RMENU           0xB8    /* right Alt */
#define DIK_PAUSE           0xC5    /* Pause */
#define DIK_HOME            0xC7    /* Home on arrow keypad */
#define DIK_UP              0xC8    /* UpArrow on arrow keypad */
#define DIK_PRIOR           0xC9    /* PgUp on arrow keypad */
#define DIK_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define DIK_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define DIK_END             0xCF    /* End on arrow keypad */
#define DIK_DOWN            0xD0    /* DownArrow on arrow keypad */
#define DIK_NEXT            0xD1    /* PgDn on arrow keypad */
#define DIK_INSERT          0xD2    /* Insert on arrow keypad */
#define DIK_DELETE          0xD3    /* Delete on arrow keypad */
#define DIK_LWIN            0xDB    /* Left Windows key */
#define DIK_RWIN            0xDC    /* Right Windows key */
#define DIK_APPS            0xDD    /* AppMenu key */
#define DIK_POWER           0xDE    /* System Power */
#define DIK_SLEEP           0xDF    /* System Sleep */
#define DIK_WAKE            0xE3    /* System Wake */
#define DIK_WEBSEARCH       0xE5    /* Web Search */
#define DIK_WEBFAVORITES    0xE6    /* Web Favorites */
#define DIK_WEBREFRESH      0xE7    /* Web Refresh */
#define DIK_WEBSTOP         0xE8    /* Web Stop */
#define DIK_WEBFORWARD      0xE9    /* Web Forward */
#define DIK_WEBBACK         0xEA    /* Web Back */
#define DIK_MYCOMPUTER      0xEB    /* My Computer */
#define DIK_MAIL            0xEC    /* Mail */
#define DIK_MEDIASELECT     0xED    /* Media Select */



static int st_DIKMapCode[256] =
{
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_ESCAPE              ,KEYBOARD_CODE_1                      ,KEYBOARD_CODE_2                 ,KEYBOARD_CODE_3                   ,  /*5*/
    KEYBOARD_CODE_4                  ,KEYBOARD_CODE_5                   ,KEYBOARD_CODE_6                      ,KEYBOARD_CODE_7                 ,KEYBOARD_CODE_8                   ,  /*10*/
    KEYBOARD_CODE_9                  ,KEYBOARD_CODE_0                   ,KEYBOARD_CODE_MINUS                  ,KEYBOARD_CODE_EQUALS            ,KEYBOARD_CODE_BACK                ,  /*15*/
    KEYBOARD_CODE_TAB                ,KEYBOARD_CODE_Q                   ,KEYBOARD_CODE_W                      ,KEYBOARD_CODE_E                 ,KEYBOARD_CODE_R                   ,  /*20*/
    KEYBOARD_CODE_T                  ,KEYBOARD_CODE_Y                   ,KEYBOARD_CODE_U                      ,KEYBOARD_CODE_I                 ,KEYBOARD_CODE_O                   ,  /*25*/
    KEYBOARD_CODE_P                  ,KEYBOARD_CODE_LBRACKET            ,KEYBOARD_CODE_RBRACKET               ,KEYBOARD_CODE_RETURN            ,KEYBOARD_CODE_LCONTROL            ,  /*30*/
    KEYBOARD_CODE_A                  ,KEYBOARD_CODE_S                   ,KEYBOARD_CODE_D                      ,KEYBOARD_CODE_F                 ,KEYBOARD_CODE_G                   ,  /*35*/
    KEYBOARD_CODE_H                  ,KEYBOARD_CODE_J                   ,KEYBOARD_CODE_K                      ,KEYBOARD_CODE_L                 ,KEYBOARD_CODE_SEMICOLON           ,  /*40*/
    KEYBOARD_CODE_APOSTROPHE         ,KEYBOARD_CODE_GRAVE               ,KEYBOARD_CODE_LSHIFT                 ,KEYBOARD_CODE_BACKSLASH         ,KEYBOARD_CODE_Z                   ,  /*45*/
    KEYBOARD_CODE_X                  ,KEYBOARD_CODE_C                   ,KEYBOARD_CODE_V                      ,KEYBOARD_CODE_B                 ,KEYBOARD_CODE_N                   ,  /*50*/
    KEYBOARD_CODE_M                  ,KEYBOARD_CODE_COMMA               ,KEYBOARD_CODE_PERIOD                 ,KEYBOARD_CODE_SLASH             ,KEYBOARD_CODE_RSHIFT              ,  /*55*/
    KEYBOARD_CODE_NUM_MULTIPLY       ,KEYBOARD_CODE_LALT                ,KEYBOARD_CODE_SPACE                  ,KEYBOARD_CODE_CAPITAL           ,KEYBOARD_CODE_F1                  ,  /*60*/
    KEYBOARD_CODE_F2                 ,KEYBOARD_CODE_F3                  ,KEYBOARD_CODE_F4                     ,KEYBOARD_CODE_F5                ,KEYBOARD_CODE_F6                  ,  /*65*/
    KEYBOARD_CODE_F7                 ,KEYBOARD_CODE_F8                  ,KEYBOARD_CODE_F9                     ,KEYBOARD_CODE_F10               ,KEYBOARD_CODE_NUMLOCK             ,  /*70*/
    KEYBOARD_CODE_SCROLL             ,KEYBOARD_CODE_NUM_7               ,KEYBOARD_CODE_NUM_8                  ,KEYBOARD_CODE_NUM_9             ,KEYBOARD_CODE_SUBTRACT            ,  /*75*/
    KEYBOARD_CODE_NUM_4              ,KEYBOARD_CODE_NUM_5               ,KEYBOARD_CODE_NUM_6                  ,KEYBOARD_CODE_NUM_ADD           ,KEYBOARD_CODE_NUM_1               ,  /*80*/
    KEYBOARD_CODE_NUM_2              ,KEYBOARD_CODE_NUM_3               ,KEYBOARD_CODE_NUM_0                  ,KEYBOARD_CODE_DECIMAL           ,KEYBOARD_CODE_NULL                ,  /*85*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_OEM_102             ,KEYBOARD_CODE_F11                    ,KEYBOARD_CODE_F12               ,KEYBOARD_CODE_NULL                ,  /*90*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*95*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*100*/
    KEYBOARD_CODE_F13                ,KEYBOARD_CODE_F14                 ,KEYBOARD_CODE_F15                    ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*105*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*110*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_KANA                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*115*/
    KEYBOARD_CODE_ABNT_C1            ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*120*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_CONVERT             ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NONCONVERT        ,KEYBOARD_CODE_NULL                ,  /*125*/
    KEYBOARD_CODE_YEN                ,KEYBOARD_CODE_ABNT_C2             ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*130*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*135*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*140*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NUM_EQUALS          ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_PREV_TRACK          ,  /*145*/
    KEYBOARD_CODE_AT                 ,KEYBOARD_CODE_COLON               ,KEYBOARD_CODE_UNDERLINE              ,KEYBOARD_CODE_KANJI             ,KEYBOARD_CODE_STOP                ,  /*150*/
    KEYBOARD_CODE_AX                 ,KEYBOARD_CODE_UNLABELED           ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NEXT_TRACK        ,KEYBOARD_CODE_NULL                ,  /*155*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NUM_ENTER           ,KEYBOARD_CODE_RCONTROL               ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*160*/
    KEYBOARD_CODE_MUTE               ,KEYBOARD_CODE_CALCULATOR          ,KEYBOARD_CODE_PLAY_PAUSE             ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_MEDIA_STOP          ,  /*165*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*170*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_VOLUME_DOWN         ,  /*175*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_VOLUME_UP           ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_WEB_HOME          ,KEYBOARD_CODE_NUM_COMMA           ,  /*180*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NUM_DIVIDE          ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_SYSRQ             ,KEYBOARD_CODE_RALT                ,  /*185*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*190*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*195*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_PAUSE                  ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_HOME                ,  /*200*/
    KEYBOARD_CODE_UP                 ,KEYBOARD_CODE_PRIOR               ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_LEFT              ,KEYBOARD_CODE_NULL                ,  /*205*/
    KEYBOARD_CODE_RIGHT              ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_END                    ,KEYBOARD_CODE_DOWN              ,KEYBOARD_CODE_NEXT                ,  /*210*/
    KEYBOARD_CODE_INSERT             ,KEYBOARD_CODE_DELETE              ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*215*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_LWIN                ,  /*220*/
    KEYBOARD_CODE_RWIN               ,KEYBOARD_CODE_APPMENU             ,KEYBOARD_CODE_POWER                  ,KEYBOARD_CODE_SLEEP             ,KEYBOARD_CODE_NULL                ,  /*225*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_WAKE                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_WEB_SEARCH          ,  /*230*/
    KEYBOARD_CODE_WEB_FAVORITES      ,KEYBOARD_CODE_WEB_REFRESH         ,KEYBOARD_CODE_WEB_STOP               ,KEYBOARD_CODE_WEB_FORWARD       ,KEYBOARD_CODE_WEB_BACK            ,  /*235*/
    KEYBOARD_CODE_MY_COMPUTER        ,KEYBOARD_CODE_MAIL                ,KEYBOARD_CODE_MEDIA_SELECT           ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*240*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*245*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*250*/
    KEYBOARD_CODE_NULL               ,KEYBOARD_CODE_NULL                ,KEYBOARD_CODE_NULL                   ,KEYBOARD_CODE_NULL              ,KEYBOARD_CODE_NULL                ,  /*255*/
    KEYBOARD_CODE_NULL
};


#define DIK_NULL  0xff


static int st_CodeMapDik[256] =
{
    DIK_A             ,DIK_B              ,DIK_C              ,DIK_D              ,DIK_E              ,  /*5*/
    DIK_F             ,DIK_G              ,DIK_H              ,DIK_I              ,DIK_J              ,  /*10*/
    DIK_K             ,DIK_L              ,DIK_M              ,DIK_N              ,DIK_O              ,  /*15*/
    DIK_P             ,DIK_Q              ,DIK_R              ,DIK_S              ,DIK_T              ,  /*20*/
    DIK_U             ,DIK_V              ,DIK_W              ,DIK_X              ,DIK_Y              ,  /*25*/
    DIK_Z             ,DIK_0              ,DIK_1              ,DIK_2              ,DIK_3              ,  /*30*/
    DIK_4             ,DIK_5              ,DIK_6              ,DIK_7              ,DIK_8              ,  /*35*/
    DIK_9             ,DIK_ESCAPE         ,DIK_MINUS          ,DIK_EQUALS         ,DIK_BACK           ,  /*40*/
    DIK_TAB           ,DIK_LBRACKET       ,DIK_RBRACKET       ,DIK_RETURN         ,DIK_LCONTROL       ,  /*45*/
    DIK_SEMICOLON     ,DIK_APOSTROPHE     ,DIK_GRAVE          ,DIK_LSHIFT         ,DIK_BACKSLASH      ,  /*50*/
    DIK_COMMA         ,DIK_PERIOD         ,DIK_SLASH          ,DIK_RSHIFT         ,DIK_MULTIPLY       ,  /*55*/
    DIK_LMENU         ,DIK_SPACE          ,DIK_CAPITAL        ,DIK_F1             ,DIK_F2             ,  /*60*/
    DIK_F3            ,DIK_F4             ,DIK_F5             ,DIK_F6             ,DIK_F7             ,  /*65*/
    DIK_F8            ,DIK_F9             ,DIK_F10            ,DIK_F11            ,DIK_F12            ,  /*70*/
    DIK_F13           ,DIK_F14            ,DIK_F15            ,DIK_NUMLOCK        ,DIK_SCROLL         ,  /*75*/
    DIK_SUBTRACT      ,DIK_NUMPAD0        ,DIK_NUMPAD1        ,DIK_NUMPAD2        ,DIK_NUMPAD3        ,  /*80*/
    DIK_NUMPAD4       ,DIK_NUMPAD5        ,DIK_NUMPAD6        ,DIK_NUMPAD7        ,DIK_NUMPAD8        ,  /*85*/
    DIK_NUMPAD9       ,DIK_ADD            ,DIK_DECIMAL        ,DIK_OEM_102        ,DIK_KANA           ,  /*90*/
    DIK_ABNT_C1       ,DIK_CONVERT        ,DIK_NOCONVERT      ,DIK_YEN            ,DIK_ABNT_C2        ,  /*95*/
    DIK_NUMPADEQUALS  ,DIK_PREVTRACK      ,DIK_AT             ,DIK_COLON          ,DIK_UNDERLINE      ,  /*100*/
    DIK_KANJI         ,DIK_STOP           ,DIK_AX             ,DIK_UNLABELED      ,DIK_NEXTTRACK      ,  /*105*/
    DIK_NUMPADENTER   ,DIK_RCONTROL       ,DIK_MUTE           ,DIK_CALCULATOR     ,DIK_PLAYPAUSE      ,  /*110*/
    DIK_MEDIASTOP     ,DIK_VOLUMEDOWN     ,DIK_VOLUMEUP       ,DIK_WEBHOME        ,DIK_NUMPADCOMMA    ,  /*115*/
    DIK_DIVIDE        ,DIK_SYSRQ          ,DIK_RMENU          ,DIK_PAUSE          ,DIK_HOME           ,  /*120*/
    DIK_UP            ,DIK_PRIOR          ,DIK_LEFT           ,DIK_RIGHT          ,DIK_END            ,  /*125*/
    DIK_DOWN          ,DIK_NEXT           ,DIK_INSERT         ,DIK_DELETE         ,DIK_LWIN           ,  /*130*/
    DIK_RWIN          ,DIK_APPS           ,DIK_POWER          ,DIK_SLEEP          ,DIK_WAKE           ,  /*135*/
    DIK_WEBSEARCH     ,DIK_WEBFAVORITES   ,DIK_WEBREFRESH     ,DIK_WEBSTOP        ,DIK_WEBFORWARD     ,  /*140*/
    DIK_WEBBACK       ,DIK_MYCOMPUTER     ,DIK_MAIL           ,DIK_MEDIASELECT    ,DIK_NULL           ,  /*144*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*150*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*155*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*160*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*165*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*170*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*175*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*180*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*185*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*190*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*195*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*200*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*205*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*210*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*215*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*220*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*225*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*230*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*235*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*240*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*245*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*250*/
    DIK_NULL          ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,DIK_NULL           ,  /*255*/
    DIK_NULL
};

int DikToMap(int dikcode)
{
    int ret=KEYBOARD_CODE_NULL;

    switch(dikcode)
    {
    case DIK_ESCAPE:
        ret = KEYBOARD_CODE_ESCAPE;
        break;
    case DIK_1:
        ret = KEYBOARD_CODE_1;
        break;
    case DIK_2:
        ret = KEYBOARD_CODE_2;
        break;
    case DIK_3:
        ret = KEYBOARD_CODE_3;
        break;
    case DIK_4:
        ret = KEYBOARD_CODE_4;
        break;
    case DIK_5:
        ret = KEYBOARD_CODE_5;
        break;
    case DIK_6:
        ret = KEYBOARD_CODE_6;
        break;
    case DIK_7:
        ret = KEYBOARD_CODE_7;
        break;
    case DIK_8:
        ret = KEYBOARD_CODE_8;
        break;
    case DIK_9:
        ret = KEYBOARD_CODE_9;
        break;
    case DIK_0:
        ret = KEYBOARD_CODE_0;
        break;
    case DIK_MINUS:
        ret = KEYBOARD_CODE_MINUS;
        break;
    case DIK_EQUALS:
        ret = KEYBOARD_CODE_EQUALS;
        break;
    case DIK_BACK:
        ret = KEYBOARD_CODE_BACK;
        break;
    case DIK_TAB:
        ret = KEYBOARD_CODE_TAB;
        break;
    case DIK_Q:
        ret = KEYBOARD_CODE_Q;
        break;
    case DIK_W:
        ret = KEYBOARD_CODE_W;
        break;
    case DIK_E:
        ret = KEYBOARD_CODE_E;
        break;
    case DIK_R:
        ret = KEYBOARD_CODE_R;
        break;
    case DIK_T:
        ret = KEYBOARD_CODE_T;
        break;
    case DIK_Y:
        ret = KEYBOARD_CODE_Y;
        break;
    case DIK_U:
        ret = KEYBOARD_CODE_U;
        break;
    case DIK_I:
        ret = KEYBOARD_CODE_I;
        break;
    case DIK_O:
        ret = KEYBOARD_CODE_O;
        break;
    case DIK_P:
        ret = KEYBOARD_CODE_P;
        break;
    case DIK_LBRACKET:
        ret = KEYBOARD_CODE_LBRACKET;
        break;
    case DIK_RBRACKET:
        ret = KEYBOARD_CODE_RBRACKET;
        break;
    case DIK_RETURN:
        ret = KEYBOARD_CODE_RETURN;
        break;
    case DIK_LCONTROL:
        ret = KEYBOARD_CODE_LCONTROL;
        break;
    case DIK_A:
        ret = KEYBOARD_CODE_A;
        break;
    case DIK_S:
        ret = KEYBOARD_CODE_S;
        break;
    case DIK_D:
        ret = KEYBOARD_CODE_D;
        break;
    case DIK_F:
        ret = KEYBOARD_CODE_F;
        break;
    case DIK_G:
        ret = KEYBOARD_CODE_G;
        break;
    case DIK_H:
        ret = KEYBOARD_CODE_H;
        break;
    case DIK_J:
        ret = KEYBOARD_CODE_J;
        break;
    case DIK_K:
        ret = KEYBOARD_CODE_K;
        break;
    case DIK_L:
        ret = KEYBOARD_CODE_L;
        break;
    case DIK_SEMICOLON:
        ret = KEYBOARD_CODE_SEMICOLON;
        break;
    case DIK_APOSTROPHE:
        ret = KEYBOARD_CODE_APOSTROPHE;
        break;
    case DIK_GRAVE:
        ret = KEYBOARD_CODE_GRAVE;
        break;
    case DIK_LSHIFT:
        ret = KEYBOARD_CODE_LSHIFT;
        break;
    case DIK_BACKSLASH:
        ret = KEYBOARD_CODE_BACKSLASH;
        break;
    case DIK_Z:
        ret = KEYBOARD_CODE_Z;
        break;
    case DIK_X:
        ret = KEYBOARD_CODE_X;
        break;
    case DIK_C:
        ret = KEYBOARD_CODE_C;
        break;
    case DIK_V:
        ret = KEYBOARD_CODE_V;
        break;
    case DIK_B:
        ret = KEYBOARD_CODE_B;
        break;
    case DIK_N:
        ret = KEYBOARD_CODE_N;
        break;
    case DIK_M:
        ret = KEYBOARD_CODE_M;
        break;
    case DIK_COMMA:
        ret = KEYBOARD_CODE_COMMA;
        break;
    case DIK_PERIOD:
        ret = KEYBOARD_CODE_PERIOD;
        break;
    case DIK_SLASH:
        ret = KEYBOARD_CODE_SLASH;
        break;
    case DIK_RSHIFT:
        ret = KEYBOARD_CODE_RSHIFT;
        break;
    case DIK_MULTIPLY:
        ret = KEYBOARD_CODE_NUM_MULTIPLY;
        break;
    case DIK_LMENU:
        ret = KEYBOARD_CODE_LALT;
        break;
    case DIK_SPACE:
        ret = KEYBOARD_CODE_SPACE;
        break;
    case DIK_CAPITAL:
        ret = KEYBOARD_CODE_CAPITAL;
        break;
    case DIK_F1:
        ret = KEYBOARD_CODE_F1;
        break;
    case DIK_F2:
        ret = KEYBOARD_CODE_F2;
        break;
    case DIK_F3:
        ret = KEYBOARD_CODE_F3;
        break;
    case DIK_F4:
        ret = KEYBOARD_CODE_F4;
        break;
    case DIK_F5:
        ret = KEYBOARD_CODE_F5;
        break;
    case DIK_F6:
        ret = KEYBOARD_CODE_F6;
        break;
    case DIK_F7:
        ret = KEYBOARD_CODE_F7;
        break;
    case DIK_F8:
        ret = KEYBOARD_CODE_F8;
        break;
    case DIK_F9:
        ret = KEYBOARD_CODE_F9;
        break;
    case DIK_F10:
        ret = KEYBOARD_CODE_F10;
        break;
    case DIK_NUMLOCK:
        ret = KEYBOARD_CODE_NUMLOCK;
        break;
    case DIK_SCROLL:
        ret = KEYBOARD_CODE_SCROLL;
        break;
    case DIK_NUMPAD7:
        ret = KEYBOARD_CODE_NUM_7;
        break;
    case DIK_NUMPAD8:
        ret = KEYBOARD_CODE_NUM_8;
        break;
    case DIK_NUMPAD9:
        ret = KEYBOARD_CODE_NUM_9;
        break;
    case DIK_SUBTRACT:
        ret = KEYBOARD_CODE_SUBTRACT;
        break;
    case DIK_NUMPAD4:
        ret = KEYBOARD_CODE_NUM_4;
        break;
    case DIK_NUMPAD5:
        ret = KEYBOARD_CODE_NUM_5;
        break;
    case DIK_NUMPAD6:
        ret = KEYBOARD_CODE_NUM_6;
        break;
    case DIK_ADD:
        ret = KEYBOARD_CODE_NUM_ADD;
        break;
    case DIK_NUMPAD1:
        ret = KEYBOARD_CODE_NUM_1;
        break;
    case DIK_NUMPAD2:
        ret = KEYBOARD_CODE_NUM_2;
        break;
    case DIK_NUMPAD3:
        ret = KEYBOARD_CODE_NUM_3;
        break;
    case DIK_NUMPAD0:
        ret = KEYBOARD_CODE_NUM_0;
        break;
    case DIK_DECIMAL:
        ret = KEYBOARD_CODE_DECIMAL;
        break;
    case DIK_OEM_102:
        ret = KEYBOARD_CODE_OEM_102;
        break;
    case DIK_F11:
        ret = KEYBOARD_CODE_F11;
        break;
    case DIK_F12:
        ret = KEYBOARD_CODE_F12;
        break;
    case DIK_F13:
        ret = KEYBOARD_CODE_F13;
        break;
    case DIK_F14:
        ret = KEYBOARD_CODE_F14;
        break;
    case DIK_F15:
        ret = KEYBOARD_CODE_F15;
        break;
    case DIK_KANA:
        ret = KEYBOARD_CODE_KANA;
        break;
    case DIK_ABNT_C1:
        ret = KEYBOARD_CODE_ABNT_C1;
        break;
    case DIK_CONVERT:
        ret = KEYBOARD_CODE_CONVERT;
        break;
    case DIK_NOCONVERT:
        ret = KEYBOARD_CODE_NONCONVERT;
        break;
    case DIK_YEN:
        ret = KEYBOARD_CODE_YEN;
        break;
    case DIK_ABNT_C2:
        ret = KEYBOARD_CODE_ABNT_C2;
        break;
    case DIK_NUMPADEQUALS:
        ret = KEYBOARD_CODE_NUM_EQUALS;
        break;
    case DIK_PREVTRACK:
        ret = KEYBOARD_CODE_PREV_TRACK;
        break;
    case DIK_AT:
        ret = KEYBOARD_CODE_AT;
        break;
    case DIK_COLON:
        ret = KEYBOARD_CODE_COLON;
        break;
    case DIK_UNDERLINE:
        ret = KEYBOARD_CODE_UNDERLINE;
        break;
    case DIK_KANJI:
        ret = KEYBOARD_CODE_KANJI;
        break;
    case DIK_STOP:
        ret = KEYBOARD_CODE_STOP;
        break;
    case DIK_AX:
        ret = KEYBOARD_CODE_AX;
        break;
    case DIK_UNLABELED:
        ret = KEYBOARD_CODE_UNLABELED;
        break;
    case DIK_NEXTTRACK:
        ret = KEYBOARD_CODE_NEXT_TRACK;
        break;
    case DIK_NUMPADENTER:
        ret = KEYBOARD_CODE_NUM_ENTER;
        break;
    case DIK_RCONTROL:
        ret = KEYBOARD_CODE_RCONTROL;
        break;
    case DIK_MUTE:
        ret = KEYBOARD_CODE_MUTE;
        break;
    case DIK_CALCULATOR:
        ret = KEYBOARD_CODE_CALCULATOR;
        break;
    case DIK_PLAYPAUSE:
        ret = KEYBOARD_CODE_PLAY_PAUSE;
        break;
    case DIK_MEDIASTOP:
        ret = KEYBOARD_CODE_MEDIA_STOP;
        break;
    case DIK_VOLUMEDOWN:
        ret = KEYBOARD_CODE_VOLUME_DOWN;
        break;
    case DIK_VOLUMEUP:
        ret = KEYBOARD_CODE_VOLUME_UP;
        break;
    case DIK_WEBHOME:
        ret = KEYBOARD_CODE_WEB_HOME;
        break;
    case DIK_NUMPADCOMMA:
        ret = KEYBOARD_CODE_NUM_COMMA;
        break;
    case DIK_DIVIDE:
        ret = KEYBOARD_CODE_NUM_DIVIDE;
        break;
    case DIK_SYSRQ:
        ret = KEYBOARD_CODE_SYSRQ;
        break;
    case DIK_RMENU:
        ret = KEYBOARD_CODE_RALT;
        break;
    case DIK_PAUSE:
        ret = KEYBOARD_CODE_PAUSE;
        break;
    case DIK_HOME:
        ret = KEYBOARD_CODE_HOME;
        break;
    case DIK_UP:
        ret = KEYBOARD_CODE_UP;
        break;
    case DIK_PRIOR:
        ret = KEYBOARD_CODE_PRIOR;
        break;
    case DIK_LEFT:
        ret = KEYBOARD_CODE_LEFT;
        break;
    case DIK_RIGHT:
        ret = KEYBOARD_CODE_RIGHT;
        break;
    case DIK_END:
        ret = KEYBOARD_CODE_END;
        break;
    case DIK_DOWN:
        ret = KEYBOARD_CODE_DOWN;
        break;
    case DIK_NEXT:
        ret = KEYBOARD_CODE_NEXT;
        break;
    case DIK_INSERT:
        ret = KEYBOARD_CODE_INSERT;
        break;
    case DIK_DELETE:
        ret = KEYBOARD_CODE_DELETE;
        break;
    case DIK_LWIN:
        ret = KEYBOARD_CODE_LWIN;
        break;
    case DIK_RWIN:
        ret = KEYBOARD_CODE_RWIN;
        break;
    case DIK_APPS:
        ret = KEYBOARD_CODE_APPMENU;
        break;
    case DIK_POWER:
        ret = KEYBOARD_CODE_POWER;
        break;
    case DIK_SLEEP:
        ret = KEYBOARD_CODE_SLEEP;
        break;
    case DIK_WAKE:
        ret = KEYBOARD_CODE_WAKE;
        break;
    case DIK_WEBSEARCH:
        ret = KEYBOARD_CODE_WEB_SEARCH;
        break;
    case DIK_WEBFAVORITES:
        ret = KEYBOARD_CODE_WEB_FAVORITES;
        break;
    case DIK_WEBREFRESH:
        ret = KEYBOARD_CODE_WEB_REFRESH;
        break;
    case DIK_WEBSTOP:
        ret = KEYBOARD_CODE_WEB_STOP;
        break;
    case DIK_WEBFORWARD:
        ret = KEYBOARD_CODE_WEB_FORWARD;
        break;
    case DIK_WEBBACK:
        ret = KEYBOARD_CODE_WEB_BACK;
        break;
    case DIK_MYCOMPUTER:
        ret = KEYBOARD_CODE_MY_COMPUTER;
        break;
    case DIK_MAIL:
        ret = KEYBOARD_CODE_MAIL;
        break;
    case DIK_MEDIASELECT:
        ret = KEYBOARD_CODE_MEDIA_SELECT;
        break;
    }

    return ret;
}

int MapToDik(int mapcode)
{
    int ret = DIK_NULL;

    switch(mapcode)
    {
    case KEYBOARD_CODE_A:
        ret = DIK_A;
        break;
    case KEYBOARD_CODE_B:
        ret = DIK_B;
        break;
    case KEYBOARD_CODE_C:
        ret = DIK_C;
        break;
    case KEYBOARD_CODE_D:
        ret = DIK_D;
        break;
    case KEYBOARD_CODE_E:
        ret = DIK_E;
        break;
    case KEYBOARD_CODE_F:
        ret = DIK_F;
        break;
    case KEYBOARD_CODE_G:
        ret = DIK_G;
        break;
    case KEYBOARD_CODE_H:
        ret = DIK_H;
        break;
    case KEYBOARD_CODE_I:
        ret = DIK_I;
        break;
    case KEYBOARD_CODE_J:
        ret = DIK_J;
        break;
    case KEYBOARD_CODE_K:
        ret = DIK_K;
        break;
    case KEYBOARD_CODE_L:
        ret = DIK_L;
        break;
    case KEYBOARD_CODE_M:
        ret = DIK_M;
        break;
    case KEYBOARD_CODE_N:
        ret = DIK_N;
        break;
    case KEYBOARD_CODE_O:
        ret = DIK_O;
        break;
    case KEYBOARD_CODE_P:
        ret = DIK_P;
        break;
    case KEYBOARD_CODE_Q:
        ret = DIK_Q;
        break;
    case KEYBOARD_CODE_R:
        ret = DIK_R;
        break;
    case KEYBOARD_CODE_S:
        ret = DIK_S;
        break;
    case KEYBOARD_CODE_T:
        ret = DIK_T;
        break;
    case KEYBOARD_CODE_U:
        ret = DIK_U;
        break;
    case KEYBOARD_CODE_V:
        ret = DIK_V;
        break;
    case KEYBOARD_CODE_W:
        ret = DIK_W;
        break;
    case KEYBOARD_CODE_X:
        ret = DIK_X;
        break;
    case KEYBOARD_CODE_Y:
        ret = DIK_Y;
        break;
    case KEYBOARD_CODE_Z:
        ret = DIK_Z;
        break;
    case KEYBOARD_CODE_0:
        ret = DIK_0;
        break;
    case KEYBOARD_CODE_1:
        ret = DIK_1;
        break;
    case KEYBOARD_CODE_2:
        ret = DIK_2;
        break;
    case KEYBOARD_CODE_3:
        ret = DIK_3;
        break;
    case KEYBOARD_CODE_4:
        ret = DIK_4;
        break;
    case KEYBOARD_CODE_5:
        ret = DIK_5;
        break;
    case KEYBOARD_CODE_6:
        ret = DIK_6;
        break;
    case KEYBOARD_CODE_7:
        ret = DIK_7;
        break;
    case KEYBOARD_CODE_8:
        ret = DIK_8;
        break;
    case KEYBOARD_CODE_9:
        ret = DIK_9;
        break;
    case KEYBOARD_CODE_ESCAPE:
        ret = DIK_ESCAPE;
        break;
    case KEYBOARD_CODE_MINUS:
        ret = DIK_MINUS;
        break;
    case KEYBOARD_CODE_EQUALS:
        ret = DIK_EQUALS;
        break;
    case KEYBOARD_CODE_BACK:
        ret = DIK_BACK;
        break;
    case KEYBOARD_CODE_TAB:
        ret = DIK_TAB;
        break;
    case KEYBOARD_CODE_LBRACKET:
        ret = DIK_LBRACKET;
        break;
    case KEYBOARD_CODE_RBRACKET:
        ret = DIK_RBRACKET;
        break;
    case KEYBOARD_CODE_RETURN:
        ret = DIK_RETURN;
        break;
    case KEYBOARD_CODE_LCONTROL:
        ret = DIK_LCONTROL;
        break;
    case KEYBOARD_CODE_SEMICOLON:
        ret = DIK_SEMICOLON;
        break;
    case KEYBOARD_CODE_APOSTROPHE:
        ret = DIK_APOSTROPHE;
        break;
    case KEYBOARD_CODE_GRAVE:
        ret = DIK_GRAVE;
        break;
    case KEYBOARD_CODE_LSHIFT:
        ret = DIK_LSHIFT;
        break;
    case KEYBOARD_CODE_BACKSLASH:
        ret = DIK_BACKSLASH;
        break;
    case KEYBOARD_CODE_COMMA:
        ret = DIK_COMMA;
        break;
    case KEYBOARD_CODE_PERIOD:
        ret = DIK_PERIOD;
        break;
    case KEYBOARD_CODE_SLASH:
        ret = DIK_SLASH;
        break;
    case KEYBOARD_CODE_RSHIFT:
        ret = DIK_RSHIFT;
        break;
    case KEYBOARD_CODE_NUM_MULTIPLY:
        ret = DIK_MULTIPLY;
        break;
    case KEYBOARD_CODE_LALT:
        ret = DIK_LMENU;
        break;
    case KEYBOARD_CODE_SPACE:
        ret = DIK_SPACE;
        break;
    case KEYBOARD_CODE_CAPITAL:
        ret = DIK_CAPITAL;
        break;
    case KEYBOARD_CODE_F1:
        ret = DIK_F1;
        break;
    case KEYBOARD_CODE_F2:
        ret = DIK_F2;
        break;
    case KEYBOARD_CODE_F3:
        ret = DIK_F3;
        break;
    case KEYBOARD_CODE_F4:
        ret = DIK_F4;
        break;
    case KEYBOARD_CODE_F5:
        ret = DIK_F5;
        break;
    case KEYBOARD_CODE_F6:
        ret = DIK_F6;
        break;
    case KEYBOARD_CODE_F7:
        ret = DIK_F7;
        break;
    case KEYBOARD_CODE_F8:
        ret = DIK_F8;
        break;
    case KEYBOARD_CODE_F9:
        ret = DIK_F9;
        break;
    case KEYBOARD_CODE_F10:
        ret = DIK_F10;
        break;
    case KEYBOARD_CODE_F11:
        ret = DIK_F11;
        break;
    case KEYBOARD_CODE_F12:
        ret = DIK_F12;
        break;
    case KEYBOARD_CODE_NUMLOCK:
        ret = DIK_NUMLOCK;
        break;
    case KEYBOARD_CODE_SCROLL:
        ret = DIK_SCROLL;
        break;
    case KEYBOARD_CODE_NUM_7:
        ret = DIK_NUMPAD7;
        break;
    case KEYBOARD_CODE_NUM_8:
        ret = DIK_NUMPAD8;
        break;
    case KEYBOARD_CODE_NUM_9:
        ret = DIK_NUMPAD9;
        break;
    case KEYBOARD_CODE_SUBTRACT:
        ret = DIK_SUBTRACT;
        break;
    case KEYBOARD_CODE_NUM_4:
        ret = DIK_NUMPAD4;
        break;
    case KEYBOARD_CODE_NUM_5:
        ret = DIK_NUMPAD5;
        break;
    case KEYBOARD_CODE_NUM_6:
        ret = DIK_NUMPAD6;
        break;
    case KEYBOARD_CODE_NUM_ADD:
        ret = DIK_ADD;
        break;
    case KEYBOARD_CODE_NUM_1:
        ret = DIK_NUMPAD1;
        break;
    case KEYBOARD_CODE_NUM_2:
        ret = DIK_NUMPAD2;
        break;
    case KEYBOARD_CODE_NUM_3:
        ret = DIK_NUMPAD3;
        break;
    case KEYBOARD_CODE_NUM_0:
        ret = DIK_NUMPAD0;
        break;
    case KEYBOARD_CODE_DECIMAL:
        ret = DIK_DECIMAL;
        break;
    case KEYBOARD_CODE_OEM_102:
        ret = DIK_OEM_102;
        break;
    case KEYBOARD_CODE_F13:
        ret = DIK_F13;
        break;
    case KEYBOARD_CODE_F14:
        ret = DIK_F14;
        break;
    case KEYBOARD_CODE_F15:
        ret = DIK_F15;
        break;
    case KEYBOARD_CODE_KANA:
        ret = DIK_KANA;
        break;
    case KEYBOARD_CODE_ABNT_C1:
        ret = DIK_ABNT_C1;
        break;
    case KEYBOARD_CODE_CONVERT:
        ret = DIK_CONVERT;
        break;
    case KEYBOARD_CODE_NONCONVERT:
        ret = DIK_NOCONVERT;
        break;
    case KEYBOARD_CODE_YEN:
        ret = DIK_YEN;
        break;
    case KEYBOARD_CODE_ABNT_C2:
        ret = DIK_ABNT_C2;
        break;
    case KEYBOARD_CODE_NUM_EQUALS:
        ret = DIK_NUMPADEQUALS;
        break;
    case KEYBOARD_CODE_PREV_TRACK:
        ret = DIK_PREVTRACK;
        break;
    case KEYBOARD_CODE_AT:
        ret = DIK_AT;
        break;
    case KEYBOARD_CODE_COLON:
        ret = DIK_COLON;
        break;
    case KEYBOARD_CODE_UNDERLINE:
        ret = DIK_UNDERLINE;
        break;
    case KEYBOARD_CODE_KANJI:
        ret = DIK_KANJI;
        break;
    case KEYBOARD_CODE_STOP:
        ret = DIK_STOP;
        break;
    case KEYBOARD_CODE_AX:
        ret = DIK_AX;
        break;
    case KEYBOARD_CODE_UNLABELED:
        ret = DIK_UNLABELED;
        break;
    case KEYBOARD_CODE_NEXT_TRACK:
        ret = DIK_NEXTTRACK;
        break;
    case KEYBOARD_CODE_NUM_ENTER:
        ret = DIK_NUMPADENTER;
        break;
    case KEYBOARD_CODE_RCONTROL:
        ret = DIK_RCONTROL;
        break;
    case KEYBOARD_CODE_MUTE:
        ret = DIK_MUTE;
        break;
    case KEYBOARD_CODE_CALCULATOR:
        ret = DIK_CALCULATOR;
        break;
    case KEYBOARD_CODE_PLAY_PAUSE:
        ret = DIK_PLAYPAUSE;
        break;
    case KEYBOARD_CODE_MEDIA_STOP:
        ret = DIK_MEDIASTOP;
        break;
    case KEYBOARD_CODE_VOLUME_DOWN:
        ret = DIK_VOLUMEDOWN;
        break;
    case KEYBOARD_CODE_VOLUME_UP:
        ret = DIK_VOLUMEUP;
        break;
    case KEYBOARD_CODE_WEB_HOME:
        ret = DIK_WEBHOME;
        break;
    case KEYBOARD_CODE_NUM_COMMA:
        ret = DIK_NUMPADCOMMA;
        break;
    case KEYBOARD_CODE_NUM_DIVIDE:
        ret = DIK_DIVIDE;
        break;
    case KEYBOARD_CODE_SYSRQ:
        ret = DIK_SYSRQ;
        break;
    case KEYBOARD_CODE_RALT:
        ret = DIK_RMENU;
        break;
    case KEYBOARD_CODE_PAUSE:
        ret = DIK_PAUSE;
        break;
    case KEYBOARD_CODE_HOME:
        ret = DIK_HOME;
        break;
    case KEYBOARD_CODE_UP:
        ret = DIK_UP;
        break;
    case KEYBOARD_CODE_PRIOR:
        ret = DIK_PRIOR;
        break;
    case KEYBOARD_CODE_LEFT:
        ret = DIK_LEFT;
        break;
    case KEYBOARD_CODE_RIGHT:
        ret = DIK_RIGHT;
        break;
    case KEYBOARD_CODE_END:
        ret = DIK_END;
        break;
    case KEYBOARD_CODE_DOWN:
        ret = DIK_DOWN;
        break;
    case KEYBOARD_CODE_NEXT:
        ret = DIK_NEXT;
        break;
    case KEYBOARD_CODE_INSERT:
        ret = DIK_INSERT;
        break;
    case KEYBOARD_CODE_DELETE:
        ret = DIK_DELETE;
        break;
    case KEYBOARD_CODE_LWIN:
        ret = DIK_LWIN;
        break;
    case KEYBOARD_CODE_RWIN:
        ret = DIK_RWIN;
        break;
    case KEYBOARD_CODE_APPMENU:
        ret = DIK_APPS;
        break;
    case KEYBOARD_CODE_POWER:
        ret = DIK_POWER;
        break;
    case KEYBOARD_CODE_SLEEP:
        ret = DIK_SLEEP;
        break;
    case KEYBOARD_CODE_WAKE:
        ret = DIK_WAKE;
        break;
    case KEYBOARD_CODE_WEB_SEARCH:
        ret = DIK_WEBSEARCH;
        break;
    case KEYBOARD_CODE_WEB_FAVORITES:
        ret = DIK_WEBFAVORITES;
        break;
    case KEYBOARD_CODE_WEB_REFRESH:
        ret = DIK_WEBREFRESH;
        break;
    case KEYBOARD_CODE_WEB_STOP:
        ret = DIK_WEBSTOP;
        break;
    case KEYBOARD_CODE_WEB_FORWARD:
        ret = DIK_WEBFORWARD;
        break;
    case KEYBOARD_CODE_WEB_BACK:
        ret = DIK_WEBBACK;
        break;
    case KEYBOARD_CODE_MY_COMPUTER:
        ret = DIK_MYCOMPUTER;
        break;
    case KEYBOARD_CODE_MAIL:
        ret = DIK_MAIL;
        break;
    case KEYBOARD_CODE_MEDIA_SELECT:
        ret = DIK_MEDIASELECT;
        break;
    }

    return ret;
}


int main(int argc,char* argv[])
{
    int i;
    int caseret,vecret;

    for(i=0; i<256; i++)
    {
        caseret = DikToMap(i);
        vecret = st_DIKMapCode[i];
        if(caseret != vecret)
        {
            fprintf(stderr,"DikCode[%d] caseret(0x%x:%d) vecret(0x%x:%d)\n",i,caseret,caseret,vecret,vecret);
        }
    }

    for(i=0; i<256; i++)
    {
        caseret = MapToDik(i);
        vecret = st_CodeMapDik[i];
        if(caseret != vecret)
        {
            fprintf(stderr,"MapCode[%d] caseret(0x%x:%d) vecret(0x%x:%d)\n",i,caseret,caseret,vecret,vecret);
        }
    }

    for(i=0; i<256; i++)
    {
        caseret = CapsCharTrans(i);
        vecret = st_CapsChar[i];
        if(caseret != vecret)
        {
            fprintf(stderr,"capschar[%d] caseret(0x%x:%d) vecret(0x%x:%d)\n",i,caseret,caseret,vecret,vecret);
        }
    }

    for(i=0; i<256; i++)
    {
        caseret = NormCharTrans(i);
        vecret = st_NormChar[i];
        if(caseret != vecret)
        {
            fprintf(stderr,"normchar[%d] caseret(0x%x:%d) vecret(0x%x:%d)\n",i,caseret,caseret,vecret,vecret);
        }
    }

    for(i=0; i<256; i++)
    {
        caseret = CtrlCharTrans(i);
        vecret = st_CtrlChar[i];
        if(caseret != vecret)
        {
            fprintf(stderr,"ctrlchar[%d] caseret(0x%x:%d) vecret(0x%x:%d)\n",i,caseret,caseret,vecret,vecret);
        }
    }

    for(i=0; i<256; i++)
    {
        caseret = ShiftCharTrans(i);
        vecret = st_ShiftChar[i];
        if(caseret != vecret)
        {
            fprintf(stderr,"shiftchar[%d] caseret(0x%x:%d) vecret(0x%x:%d)\n",i,caseret,caseret,vecret,vecret);
        }
    }

    for(i=0; i<256; i++)
    {
        caseret = ShiftCapsCharTrans(i);
        vecret = st_ShiftCapsChar[i];
        if(caseret != vecret)
        {
            fprintf(stderr,"shiftcapschar[%d] caseret(0x%x:%d) vecret(0x%x:%d)\n",i,caseret,caseret,vecret,vecret);
        }
    }

    return 0;
}

