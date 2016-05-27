/******************************************************************************/

/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "string.h"
#include "mtos_printk.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "string.h"

#include "drv_dev.h"
#include "uio.h"
#include "class_factory.h"
#include "mdl.h"
#include "data_manager.h"

#include "ap_framework.h"
#include "ap_uio.h"

#include "ui_util_api.h"
//#include "data_manager.h"

typedef struct _ir_block_info_t
{
  u8 active;
  u8 reserved;
  u8 ir_block_index;
  u8 ir_key_num;
}ir_block_info_t;

static ir_block_info_t ir_info[IRDA_MAX_USER] = {{0}};

#ifdef WIN32
static key_map_t ir_keymap[] =
{
  {0x20, V_KEY_OK},                 //enter
  {0x21, V_KEY_PAGE_UP},        //Page up
  {0x22, V_KEY_PAGE_DOWN},  //Page down
  {0x25, V_KEY_LEFT},             //direction key left
  {0x26, V_KEY_UP},                //direction key up
  {0x27, V_KEY_RIGHT},           //direction key right
  {0x28, V_KEY_DOWN},            //direction key down
  {0x30, V_KEY_0},                   //0
  {0x31, V_KEY_1},                   /*1*/
  {0x32, V_KEY_2},                   /*2*/
  {0x33, V_KEY_3},                  /*3*/
  {0x34, V_KEY_4},                  /*4*/
  {0x35, V_KEY_5},                  //5
  {0x36, V_KEY_6},                  //6
  {0x37, V_KEY_7},                  //7
  {0x38, V_KEY_8},                  //8
  {0x39, V_KEY_9},                  //9
  {0x41, V_KEY_AUDIO},           //a
  {0x42, V_KEY_BLUE},             //b
  {0x43, V_KEY_CANCEL},         //c
  {0x45, V_KEY_EPG},              //e
  {0x46, V_KEY_FAV},              //f
  {0x47, V_KEY_GREEN},          //g
  {0x48, V_KEY_RECALL},         //h
  {0x49, V_KEY_INFO},             //i
  {0x4A, V_KEY_MUTE},            //j
  {0x4B, V_KEY_REVSLOW},      //K
  {0x4C, V_KEY_SLOW},           //L
  {0x4D, V_KEY_MENU},           //m
  {0x4E, V_KEY_FIND},             //n
  {0x4F, V_KEY_STOP},           //O
  {0x55, V_KEY_POWER},          //p
  {0x51, V_KEY_PLAY},             //q
  {0x52, V_KEY_RED},              //r
  {0x53, V_KEY_SAT},              //s
  {0x56, V_KEY_SLEEP},           //u
  {0x58, V_KEY_VIDEO_MODE},     //v
  {0x54, V_KEY_TVRADIO},        //t  
  {0x57, V_KEY_PAUSE},          //w
  {0x5A, V_KEY_YELLOW},        //y
  {0x5B, V_KEY_ZOOM},           //Z
  {0x70, V_KEY_F1},              // f1
  {0x71, V_KEY_F2},               //f2
  {0x72, V_KEY_POS},            //f3
  {0x73, V_KEY_REC},            //f4
  {0xBA, V_KEY_BACK},           //;
  {0xDE, V_KEY_FORW},           //'
  {0xBC, V_KEY_BACK2},           //,
  {0xBE, V_KEY_FORW2},           //.
  {0xE0, V_KEY_CTRL0},          ////ctrl + 0
  {0x44, V_KEY_D},              
  {0xE1, V_KEY_CTRL1},
  {0xba, V_KEY_TTX},            // ;
  {0xbf, V_KEY_SUBT},           // /
};

#else                           

static key_map_t ir_keymap[IRDA_MAX_USER][64] =
{
  //0
  {
  {0x0A, V_KEY_TVAV},
  {0x19, V_KEY_POWER},
  {0x12, V_KEY_MUTE},
  {0x00, V_KEY_ASPECT_MODE},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x03, V_KEY_AUDIO},
  {0x04, V_KEY_1},
  {0x05, V_KEY_2},
  {0x06, V_KEY_3},
  {0x07, V_KEY_4},
  {0x08, V_KEY_5},
  {0x09, V_KEY_6},
  {0x14, V_KEY_7},
  {0x1C, V_KEY_8},
  {0x0B, V_KEY_9},
  {0x11, V_KEY_0},
  {0x10, V_KEY_SUBT},
  {0x0E, V_KEY_TTX},
  {0x16, V_KEY_PAGE_DOWN},
  {0x1F, V_KEY_PAGE_UP},
  {0x1E, V_KEY_RECALL},
  {0x13, V_KEY_TVRADIO},
  {0x40, V_KEY_MENU},
  {0x42, V_KEY_CANCEL},
  {0x41, V_KEY_UP},
  {0x43, V_KEY_DOWN},
  {0x44, V_KEY_LEFT},
  {0x17, V_KEY_RIGHT},
  {0x1D, V_KEY_OK},
  {0x1B, V_KEY_FAV},
  {0x45, V_KEY_EPG},
  {0x46, V_KEY_INFO},
  {0xBE, V_KEY_INVALID},
  {0xBF, V_KEY_RED},
  {0x41, V_KEY_GREEN},
  {0x49, V_KEY_YELLOW},
  {0x4E, V_KEY_BLUE},
  {0xFF, V_KEY_FIND},   
  {0xFF, V_KEY_ZOOM},
  {0xFF, V_KEY_F3},
  {0xFF, V_KEY_SAT}, 
  {0xFF, V_KEY_BACK},
  {0xFF, V_KEY_FORW},   
  {0xFF, V_KEY_PAUSE},
  {0xFF, V_KEY_STOP},
  {0xFF, V_KEY_PREV}, //prev
  {0xFF, V_KEY_NEXT},  //next
  {0xFF, V_KEY_PLAY},
  {0xFF, V_KEY_REC},
  },
  ///1
  {
  {0x0A, V_KEY_POWER},
  {0x19, V_KEY_TVAV},
  {0x12, V_KEY_MUTE},
  {0x00, V_KEY_ASPECT_MODE},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x03, V_KEY_AUDIO},
  {0x04, V_KEY_1},
  {0x05, V_KEY_2},
  {0x06, V_KEY_3},
  {0x07, V_KEY_4},
  {0x08, V_KEY_5},
  {0x09, V_KEY_6},
  {0x14, V_KEY_7},
  {0x1C, V_KEY_8},
  {0x0B, V_KEY_9},
  {0x11, V_KEY_0},
  {0x10, V_KEY_SUBT},
  {0x0E, V_KEY_TTX},
  {0x16, V_KEY_PAGE_UP},
  {0x1F, V_KEY_PAGE_DOWN},
  {0x1E, V_KEY_RECALL},
  {0x13, V_KEY_TVRADIO},
  {0x40, V_KEY_MENU},
  {0x42, V_KEY_CANCEL},
  {0x41, V_KEY_UP},
  {0x43, V_KEY_DOWN},
  {0x44, V_KEY_LEFT},
  {0x17, V_KEY_RIGHT},
  {0x1D, V_KEY_OK},
  {0x1B, V_KEY_FAV},
  {0x45, V_KEY_EPG},
  {0x46, V_KEY_INFO},
  {0xBE, V_KEY_SAT},
  {0xBF, V_KEY_RED},
  {0x41, V_KEY_GREEN},
  {0x49, V_KEY_YELLOW},
  {0x4E, V_KEY_BLUE},
  {0xFF, V_KEY_FIND},   
  {0xFF, V_KEY_ZOOM},
  {0xFF, V_KEY_VUP},
  {0xFF, V_KEY_VDOWN}, 
  {0xFF, V_KEY_BACK},
  {0xFF, V_KEY_FORW},   
  {0xFF, V_KEY_PAUSE},
  {0xFF, V_KEY_STOP},
  {0xFF,V_KEY_REVSLOW},
  {0xFF,V_KEY_SLOW},
  {0xFF, V_KEY_PLAY},
  {0xFF, V_KEY_REC},
  {0xFF, V_KEY_PREV}, //prev
  {0xFF, V_KEY_NEXT},  //next
  {0xFF, V_KEY_REPEAT},
  {0xFF, V_KEY_LIST},
  {0xFF, V_KEY_GOTO},
  },
  ///2
  {
  {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},  //“ÙπÏ
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},     //∆µµ¿+
  {0xaa, V_KEY_P2P},      //∆µµ¿-
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
  ///3
  {
   {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
  ///4
  {
    {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
  ///5
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
  ///6
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
  ///7
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
    ///8
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
    ///9
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
    ///10
  {
    {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
    ///11
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
    ///12
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
    ///13
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
    ///14
  {
    {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  },
    ///15
  {
     {0x55, V_KEY_POWER},
  {0x4A, V_KEY_MUTE},
  {0x48, V_KEY_RECALL},
  {0x30, V_KEY_0},
  {0x31, V_KEY_1},
  {0x32, V_KEY_2},
  {0x33, V_KEY_3},
  {0x34, V_KEY_4},
  {0x35, V_KEY_5},
  {0x36, V_KEY_6},
  {0x37, V_KEY_7},
  {0x38, V_KEY_8},
  {0x39, V_KEY_9},
  {0x46, V_KEY_FAV},
  {0x54, V_KEY_TVRADIO},
  {0x26, V_KEY_UP},
  {0x25, V_KEY_LEFT},
  {0x20, V_KEY_OK},
  {0x27, V_KEY_RIGHT},
  {0x28, V_KEY_DOWN},
  {0x4D, V_KEY_MENU},
  {0x43, V_KEY_CANCEL},
  {0x21, V_KEY_PAGE_UP},
  {0x22, V_KEY_PAGE_DOWN},
  {0xde, V_KEY_RED},
  {0xbf, V_KEY_GREEN},
  {0xbe, V_KEY_BLUE},
  {0xbc, V_KEY_YELLOW},
  {0x45, V_KEY_TTX},
  {0x45, V_KEY_EPG},
  {0x53, V_KEY_SAT},
  {0x53, V_KEY_PLAY},
  {0x53, V_KEY_REVSLOW},
  {0x53, V_KEY_SLOW},
  {0x53, V_KEY_PAUSE},
  {0xaa, V_KEY_9PIC},
  {0xaa, V_KEY_AUDIO},
  {0xaa, V_KEY_INFO},
  {0xaa, V_KEY_PN},
  {0x0A, V_KEY_TVAV},
  {0x0A, V_KEY_PROGLIST},
  {0x0A, V_KEY_SCAN},
  {0x0A, V_KEY_INVALID},
  {0xaa, V_KEY_VUP},
  {0xaa, V_KEY_VDOWN},
  {0xaa, V_KEY_FIND},
  {0xaa, V_KEY_P2P},
  {0x01, V_KEY_VIDEO_MODE},
  {0x02, V_KEY_SLEEP},
  {0x00, V_KEY_ASPECT_MODE},
  {0xaa, V_KEY_DISPLAY_MODE},
  {0xaa, V_KEY_LANGUAGE_SWITCH},
  {0xaa, V_KEY_POS},
  {0xaa, V_KEY_BEEPER},
  {0xaa, V_KEY_ZOOM},
  {0xaa, V_KEY_STOP},
  {0xaa, V_KEY_SUBT},
  {0xaa, V_KEY_BACK},
  {0xaa, V_KEY_FORW},
  {0xaa, V_KEY_REC},
  }
};
#endif



#if 0//#ifdef CONFIG_CUSTOMER
static key_map_t fp_keymap[] =
{
  {0x7, V_KEY_POWER},
  {0x3, V_KEY_UP},
  {0x4, V_KEY_DOWN},
  #ifdef WIN32
  {0x10, V_KEY_LEFT},
  {0x20, V_KEY_RIGHT},
  #endif
};
#else
static key_map_t fp_keymap[] =
{
  {0x01, V_KEY_POWER},
  {0x08, V_KEY_UP},
  {0x04, V_KEY_DOWN},
  {0x10, V_KEY_LEFT},
  {0x20, V_KEY_RIGHT},
  {0x40, V_KEY_OK},
  {0x80, V_KEY_MENU},
  {0x04, V_KEY_VUP},
  {0x04, V_KEY_VDOWN},
};
#endif

#ifdef PATCH_ENABLE
static u16 enter_ucas_key_list[] =
{
  V_KEY_8,
  V_KEY_8,
  V_KEY_8,
  V_KEY_8,
};
#endif

static u16 enter_recall_list_key[] =
{
  V_KEY_RECALL,
  V_KEY_RECALL,
};


static u16 enter_twin_port_ui[] =
{
  V_KEY_MUTE,
  V_KEY_9,
  V_KEY_9,
  V_KEY_9,
};

static u16 enter_factory_test_key[] = 
{
  V_KEY_UP,
  V_KEY_DOWN,
  V_KEY_LEFT,
  V_KEY_RIGHT,
  
};

u32 last_ticks = 0;
u32 curn_ticks = 0;
//static u16 ir_usr_code[IRDA_MAX_USER] = {0};
//static BOOL enable_key_input = TRUE; 

#define MAX_MAGIC_LIST 6
#define GET_MAGIC_LIST_CNT(x) (sizeof(x)/sizeof(u16))

typedef struct
{
  u16 magic_key;
  u16 *p_key_list;
  u8 key_cnt;
  u8 key_state;
}magic_key_info_t;

magic_key_info_t g_magic_keylist[MAX_MAGIC_LIST];

static void init_magic_keylist(void)
{
  u8 i;
  magic_key_info_t *p_key_info = g_magic_keylist;

  for (i = 0; i < MAX_MAGIC_LIST; i++)
  {
    p_key_info->p_key_list = NULL;
    p_key_info->key_cnt = 0;
    p_key_info->key_state = 0;

    p_key_info++;
  }
}

//#ifdef PATCH_ENABLE
static BOOL register_magic_keylist(u16 *p_key_list, u8 key_cnt, u16 magic_key)
{
  u8 i;
  magic_key_info_t *p_key_info = g_magic_keylist;

  for (i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if (p_key_info->magic_key == V_KEY_INVALID)
    {
      p_key_info->magic_key = magic_key;
      p_key_info->p_key_list = p_key_list;
      p_key_info->key_cnt = key_cnt;
      p_key_info->key_state = 0;

      return TRUE;
    }

    p_key_info++;
  }

  return FALSE;
}
//#endif

static BOOL verify_key(magic_key_info_t *p_key_info, u16 key)
{
  if (key == p_key_info->p_key_list[p_key_info->key_state])
  {
    p_key_info->key_state++;
  }
  else
  {
    if (key == p_key_info->p_key_list[0])
    {
      p_key_info->key_state = 1;
    }
    else
    {
      p_key_info->key_state = 0;
    }
  }

  if (p_key_info->key_state == p_key_info->key_cnt)
  {
    p_key_info->key_state = 0;
    return TRUE;
  }

  return FALSE;
}

static void reset_magic_keylist(void)
{
  u8 i;
  magic_key_info_t *p_key_info = g_magic_keylist;

  for (i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if (p_key_info->magic_key != V_KEY_INVALID) 
    {
      p_key_info->key_state = 0;
    }

    p_key_info++;
  }
}


static u16 detect_magic_keylist(u16 key)
{
  u8 i;
  magic_key_info_t *p_key_info = g_magic_keylist;

  curn_ticks = mtos_ticks_get();

  if ((curn_ticks - last_ticks) > 700)
  {
    reset_magic_keylist();
  }

  last_ticks = curn_ticks;

  for (i = 0; i < MAX_MAGIC_LIST; i++)
  {
    if (p_key_info->p_key_list != NULL) 
    {
      if(verify_key(p_key_info, key))
      {
        return p_key_info->magic_key;
      }
    }

    p_key_info++;
  }

  return V_KEY_INVALID;
}


u8 get_key_value(u8 usr, u8 vkey)
{
  u8 i = 0;
  key_map_t *p_map = NULL;
  u8 num = 0;

  #ifdef WIN32
  if(usr == 0)
  {
    p_map = ir_keymap;
    num = sizeof(ir_keymap) / sizeof(key_map_t);
  }
  #else
   if(usr < 16)
  {
    p_map = ir_keymap[usr];
    num = ir_info[usr].ir_key_num;
  }
  #endif 
  else
  {
    p_map = fp_keymap;
    num = sizeof(fp_keymap) / sizeof(key_map_t);
  }
  for( ; i < num; i++)
  {
    if(p_map[i].v_key == vkey)
    {
      return p_map[i].h_key;
    }
  }
  MT_ASSERT(0);
  return 0;
}


void ap_uio_init_kep_map(void)
{
#ifndef WIN32 
  u8 i = 0 ,j =0;
  u8 cfg_tmp[128] = {0};
  u8 rpt_key[IRDA_MAX_USER + 1][8] = {{0}};
  
  u16 ir_head_len = 0;
  u16 ir_usr_code [IRDA_MAX_USER] = {0};

  u32 off_set = 0;
  u32 read_len = 0;
  u32 ir_max = IRDA_MAX_USER;

  uio_device_t *dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);

  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                              IR_BLOCK_ID, 0, off_set, 
                               sizeof(ir_max),
                              (u8 *)cfg_tmp);
  off_set =  sizeof(ir_max);
  if(read_len > 0)
  {
    ir_max = cfg_tmp[0];
  }

  
  ir_head_len = sizeof(ir_block_info_t) * ir_max;

  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                              IR_BLOCK_ID, 0, off_set, 
                               ir_head_len,
                              (u8 *)cfg_tmp);
  off_set += read_len; 

  if(read_len > 0)
  {
     for(i = 0;i < ir_head_len;i++)
    {
        *((u8 *)ir_info + i) = cfg_tmp[i];
    }
  }

  for(i = 0;i < ir_max;i++)
  {        
  
       read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                  IR_BLOCK_ID, 0, off_set, 
                  ir_info[i].ir_key_num,
                              (u8 *)cfg_tmp);

      ir_info[i].ir_key_num = ir_info[i].ir_key_num - 2;  //the last two for usr_high and usr_low

      off_set += read_len;
      if(read_len == 0)
      {
          continue;
      }
      if(ir_info[i].active == 1)
      {
          if(read_len > 0)
          {
              for(j = 0; j < ir_info[i].ir_key_num; j++)
              {
               ir_keymap[i][j].h_key = cfg_tmp[j];
               if(i == 2)
               {
                if(j == 36)
                {
                 ir_keymap[i][j].h_key = 0x01;
                }
                if(j == 45)
                {
                 ir_keymap[i][j].h_key = 0x08;
                }
                if(j == 46)
               {
                 ir_keymap[i][j].h_key = 0x10;
               }
               }
              }
              ir_usr_code[i] = (cfg_tmp[j] << 8) | cfg_tmp[j + 1];
              rpt_key[i][0] = get_key_value(i, V_KEY_DOWN);
              rpt_key[i][1] = get_key_value(i, V_KEY_UP);
              rpt_key[i][2] = get_key_value(i, V_KEY_RIGHT);
              rpt_key[i][3] = get_key_value(i, V_KEY_LEFT);
              if(i == 1)
              {
              rpt_key[i][4] = get_key_value(i, V_KEY_VDOWN);
              rpt_key[i][5] = get_key_value(i, V_KEY_VUP);
              }
              uio_set_rpt_key(dev, UIO_IRDA, rpt_key[i], 6, i);
          }
       }
  }
  
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                              FPKEY_BLOCK_ID, 0, 0, 
                              128,
                              (u8 *)cfg_tmp);
  if(read_len > 0)
  {
    for(i = 0; i < sizeof(fp_keymap) / sizeof(key_map_t); i++)
    {
      fp_keymap[i].h_key = cfg_tmp[i];
    }
  }

  rpt_key[ir_max][0] = get_key_value(ir_max, V_KEY_DOWN);
  rpt_key[ir_max][1] = get_key_value(ir_max, V_KEY_UP);
  rpt_key[ir_max][2] = get_key_value(ir_max, V_KEY_RIGHT);
  rpt_key[ir_max][3] = get_key_value(ir_max, V_KEY_LEFT);
  uio_set_rpt_key(dev, UIO_FRONTPANEL, rpt_key[ir_max], 4, ir_max);

  uio_set_user_code(dev, ir_max, ir_usr_code);
  dev_io_ctrl(dev, UIO_IR_SET_HW_USERCODE, 0);
#endif

  init_magic_keylist();
#ifdef PATCH_ENABLE
  register_magic_keylist(enter_ucas_key_list, 
  GET_MAGIC_LIST_CNT(enter_ucas_key_list), V_KEY_UCAS);
#endif
  register_magic_keylist(enter_twin_port_ui, 
    GET_MAGIC_LIST_CNT(enter_twin_port_ui), V_KEY_BISS_KEY);

  register_magic_keylist(enter_recall_list_key, 
  GET_MAGIC_LIST_CNT(enter_recall_list_key), V_KEY_UCAS);

  register_magic_keylist(enter_factory_test_key, 
  GET_MAGIC_LIST_CNT(enter_factory_test_key), V_KEY_GBOX);  
}


BOOL ap_uio_translate_key(v_key_t *key)
{
  key_map_t *key_map = NULL;
  u32 key_num = 0;
  u32 i = 0;
  u16 magic_key = V_KEY_INVALID;
  BOOL bRet = FALSE;
  u8  keyIndex = 1;
  u16 v_key_detect = V_KEY_INVALID;
  u16 v_key_input = key->v_key;
  MT_ASSERT(key != NULL);

  if(key->type == IRDA_KEY)
  {
    #ifdef WIN32
    if (key->usr == 0)
    {
      key_map = ir_keymap;
      key_num = sizeof(ir_keymap) / sizeof(key_map_t);
    }
    #else
    if (key->usr < IRDA_MAX_USER)
    {
      key_map = ir_keymap[key->usr];
      key_num = ir_info[key->usr].ir_key_num;
    }
   #endif
  }
  else if(key->type == FP_KEY)
  {
    key_map = fp_keymap;
    key_num = sizeof(fp_keymap) / sizeof(key_map_t);
  }
  MT_ASSERT(key_map != NULL);
  for(i = 0; i < key_num; i++)
  {
    key_map_t *temp = key_map + i;
    
    if(temp->h_key == v_key_input)
    {

     OS_PRINTF("AP_UIO: one key translated(h_key: %d, v_key: %d)\n",
           key->v_key, temp->v_key);

     if(temp->v_key == V_KEY_POWER)
     {
       OS_PRINTF("AP_UIO: one key translated(power_key: %d)\n", key->v_key);
       uio_set_power_key(key->v_key, 0);
     }
      switch(keyIndex)
      {
        case 1:
          key->v_key = temp->v_key;
          v_key_detect = key->v_key;
          keyIndex++;
          break;

        case 2:
          key->v_key_2 = temp->v_key;
          v_key_detect = key->v_key_2;
          keyIndex++;
          break;

        case 3:
          key->v_key_3 = temp->v_key;
          v_key_detect = key->v_key_3;
          keyIndex++;
          break;

        case 4:
          key->v_key_4 = temp->v_key;
          v_key_detect = key->v_key_4;
          keyIndex++;
          break;

        default:
          return bRet;
      }
      
      magic_key = detect_magic_keylist(v_key_detect);
      if (magic_key != V_KEY_INVALID)
      {
        //detect key serial succedd, change to magic.
        
        switch(keyIndex)
        {
          case 1:
            key->v_key = magic_key;
            break;

          case 2:
            key->v_key_2 = magic_key;
            break;

          case 3:
            key->v_key_3 = magic_key;
            break;

          case 4:
            key->v_key_4 = magic_key;
            break;

          default:
            key->v_key = magic_key;
            break;
        }
      }
      
      bRet = TRUE;
    }
    
  }
  
  return bRet;
}


ap_uio_policy_t *construct_ap_uio_policy(void)
{
  ap_uio_policy_t *p_uio_policy = mtos_malloc(sizeof(ap_uio_policy_t));

  MT_ASSERT(p_uio_policy != NULL);
  memset(p_uio_policy, 0, sizeof(ap_uio_policy_t));
  
  p_uio_policy->p_init_kep_map = ap_uio_init_kep_map;
  p_uio_policy->p_translate_key = ap_uio_translate_key;

  return p_uio_policy;
}


void destruct_ap_uio_policy(ap_uio_policy_t *p_uio_policy)
{
  mtos_free(p_uio_policy);
}

