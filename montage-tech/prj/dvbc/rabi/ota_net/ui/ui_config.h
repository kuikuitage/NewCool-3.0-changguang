/****************************************************************************

****************************************************************************/

#ifndef __PROJECT_CONFIG_H__
#define __PROJECT_CONFIG_H__

#define UI_DEBUG

#ifdef UI_DEBUG
#define UI_PRINTF    OS_PRINTF
#else
#ifndef WIN32
#define UI_PRINTF(param ...)    do { } while (0)
#else
#define UI_PRINTF
#endif
#endif

enum user_msg
{
  MSG_POWER_ON = MSG_USER_BEGIN,
  MSG_POWER_OFF,
  MSG_TIMER_EXPIRED,
  MSG_CLOSE_MENU,       /* only for the system timer with the attribute of autoclose */
  MSG_EXIT_ALL,
  MSG_MUTE,
  MSG_PAUSE,
  MSG_AUTO_SWITCH,
  MSG_TVSYS_SWITCH,
  MSG_OPEN_MENU,
  MSG_BLACK_SCREEN,
  MSG_1ST_PIC,
  MSG_CHK_PWD,
  MSG_CORRECT_PWD,
  MSG_WRONG_PWD,

  MSG_SUBT,
  MSG_AUTO_TEST,

  MSG_BEEPER,
  MSG_T9_CHANGED,

  MSG_INPUT_CHANGED,
  MSG_HIDE_KEYBOARD,

  MSG_RENAME_CHECK,
  MSG_RENAME_UPDATE,  
  MSG_FAV_UPDATE,
  MSG_SORT_UPDATE,
  MSG_NUM_SELECT,
  MSG_TIME_UPDATE,
  MSG_TVRADIO,
  MSG_KEY_UPDATE,
  MSG_LNB_SHORT,
  MSG_TEST_MENU_UPDATE,
  MSG_SLEEP_TMROUT,
  MSG_PLUG_IN,
  MSG_PLUG_OUT,
  MSG_USB_DEV_PLUG_IN,
  MSG_SEARCH_UPDATE,
  MSG_SEARCH_CHECK,
  MSG_UIO_EVENT,
  MSG_TIMER_OPEN,
  MSG_MEDIA_PLAY_TMROUT,
  //MSG_UP_FILEBAR,
  MSG_PING_TEST,
  MSG_CONFIG_TCPIP,
  MSG_INTERNET_PLUG_IN,
  MSG_INTERNET_PLUG_OUT,
  MSG_EDIT_IP_PATH_CHECK,
  MSG_EDIT_IP_PATH_UPDATE,
  MSG_EDIT_CITY_CHECK,
  MSG_EDIT_CITY_UPDATE, 
  MSG_NET_DEV_MOUNT, 
  MSG_NETWORK_MOUNT_SUCCESS,
  MSG_NETWORK_MOUNT_FAIL,
  MSG_CONFIG_IP,
  MSG_GET_WIFI_AP_CNT,
  MSG_WIFI_AP_CONNECT,
  MSG_WIFI_AP_DISCONNECT,
  MSG_WIFI_AP_CONNECTING,
  MSG_RECORD_PLAY_PAUSE,
  MSG_NETWORK_CONNECTED,
  MSG_NETWORK_UNCONNECTED,

  MSG_WIFI_AP_CONNECT_FAIL,


};

#define MAX_APP_CNT    (APP_LAST - APP_FRAMEWORK)

//user shift key define
enum user_shfit_msg
{
  MSG_OPEN_MENU_IN_TAB = MSG_SHIFT_USER_BEGIN + 0x0100, // increase 0x100 once.
};

// desktop config
#define SCREEN_WIDTH              (1280) //1280
#define SCREEN_HEIGHT             (720)//720

#define SCREEN_9PIC_WIDTH         (960)//1280
#define SCREEN_9PIC_HEIGHT        (600)//720

#define SCREEN_POS_PAL_L          ((1280 - SCREEN_WIDTH) / 2)
#define SCREEN_POS_PAL_T          ((720 - SCREEN_HEIGHT) / 2)
#define SCREEN_POS_NTSC_L         ((1280 - SCREEN_WIDTH) / 2)
#define SCREEN_POS_NTSC_T         ((720 - SCREEN_HEIGHT) / 2)

#define SCREEN_9PIC_POS_PAL_L     ((1280 - SCREEN_9PIC_WIDTH) / 2)
#define SCREEN_9PIC_POS_PAL_T     ((720 - SCREEN_9PIC_HEIGHT) / 2)
#define SCREEN_9PIC_POS_NTSC_L    ((1280 - SCREEN_9PIC_WIDTH) / 2)
#define SCREEN_9PIC_POS_NTSC_T    ((720 - SCREEN_9PIC_HEIGHT) / 2)

#define MAX_ROOT_CONT_CNT         8
#define MAX_MESSAGE_CNT           64
#define MAX_HOST_CNT              64
#define MAX_TMR_CNT               32

#define DST_IDLE_TMOUT            -1
#define POST_MSG_TMOUT            0     //100 ms
#define RECEIVE_MSG_TMOUT         0     //100 ms

// gdi config
#define MAX_CLIPRECT_CNT          512
#define MAX_SURFACE_CNT           (MAX_ROLL_CNT + 1)
//roll
#define MAX_ROLL_CNT              10

#define ROLL_PPS                  4

#define MAX_DC_CNT                6

#define MAX_CLASS_CNT             13//12
#define MAX_OBJ_CNT               (MAX_OBJECT_NUMBER_CNT +  \
                                   MAX_OBJECT_NUMBER_TXT +  \
                                   MAX_OBJECT_NUMBER_BMP +  \
                                   MAX_OBJECT_NUMBER_CBB +  \
                                   MAX_OBJECT_NUMBER_LIST + \
                                   MAX_OBJECT_NUMBER_MTB +  \
                                   MAX_OBJECT_NUMBER_PGB +  \
                                   MAX_OBJECT_NUMBER_SCB +  \
                                   MAX_OBJECT_NUMBER_EDT +  \
                                   MAX_OBJECT_NUMBER_NUM + \
                                   MAX_OBJECT_NUMBER_TIM + \
                                   MAX_OBJECT_NUMBER_SBX + \
                                   MAX_OBJECT_NUMBER_IP )

//how many control will be created at the same time
#define MAX_OBJECT_NUMBER_CNT     64
#define MAX_OBJECT_NUMBER_TXT     64
#define MAX_OBJECT_NUMBER_BMP     64
#define MAX_OBJECT_NUMBER_CBB     16
#define MAX_OBJECT_NUMBER_LIST    16
#define MAX_OBJECT_NUMBER_MTB     16
#define MAX_OBJECT_NUMBER_PGB     8
#define MAX_OBJECT_NUMBER_SCB     16
#define MAX_OBJECT_NUMBER_EDT     8
#define MAX_OBJECT_NUMBER_NUM     32
#define MAX_OBJECT_NUMBER_TIM     8
#define MAX_OBJECT_NUMBER_SBX     8
#define MAX_OBJECT_NUMBER_IP     8



// db

//string length
#define MAX_CTRL_STRING_BUF_SIZE    8192
#define MAX_PAINT_STRING_LENGTH     512
#define MAX_PAINT_STRING_LINES        10

// others
#define AP_VOLUME_MAX            31
#define SS_PWD_LENGTH            4

#define DEFAULT_YEAR             2009
#define DEFAULT_MONTH            1
#define DEFAULT_DAY              1

#define SECOND                   1000


#endif

