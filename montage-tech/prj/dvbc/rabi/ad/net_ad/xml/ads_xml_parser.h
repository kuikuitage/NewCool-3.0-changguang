#include "sys_types.h"

#define XML_GET_CHILD(node) node->childs
#define XML_GET_ROOT(doc)   doc->root
#define XML_GET_PROPERTIES(node) node->properties->name
#define XML_GET_PROPERTIES_VAL(node) node->properties->val
#define XML_FREE(x) if(x){free(x);x=NULL;}
#ifndef xmlChar
#define xmlChar unsigned char
#endif

typedef enum{
	ADS_MENU=1,
	ADS_FULL_SCREEN,
	FULL_SCREEN_PUSH,
	FULL_SCREEN_LOCK,
	SUB_MENUS,
	BOOT_LOGO,
	PROGBAR,
	VOLUME_PROGBAR,
	SUBTITLES,
}ADS_POS_TYPE;
typedef enum{
  ADS_FILE_TYPE_JPG,
  /*!
    bitmap
    */
  ADS_FILE_TYPE_BMP,
  /*!
    text file
    */
  ADS_FILE_TYPE_TEXT,
  /*!
    gif
    */
  ADS_FILE_TYPE_GIF,
  /*!
    png
    */
  ADS_FILE_TYPE_PNG,
}ADS_FILE_TYPE;

typedef struct{
	ADS_POS_TYPE type;
	xmlChar type_name[16];
	xmlChar name[8];
	u16 x;
	u16 y;
	u16 w;
	u16 h;
	u16 duration;
}ads_public_info;

typedef enum{
	PIC_COMPRESSED_FLAG_NO = 0,
	PIC_COMPRESSED_FLAG_YES = 1,
}PIC_COMPRESSED_FLAG;

enum data_status_e{
	EMPTY,
	RECEIVING,
	FINISH,
};

typedef struct _CHANNEL_SHOW_AD{
       u16 channel_id;
       struct _CHANNEL_SHOW_AD *next;
}ads_channel_show_id;

typedef struct _Data_INFO{
       ADS_FILE_TYPE p_format;
	u8 *p_ads_data;
       ads_channel_show_id channel_id;
       u16 channel_count;
       u16 version;
       u8 *p_ads_date_url;
	struct _Data_INFO *next;
}ads_data_info;

typedef struct _Data_Node{
	ads_data_info *p_ads_data;
       u16 data_info_count;
	ads_public_info ads_public_info;
	u16 text_size;
	enum data_status_e status;
	struct _Data_Node *next;
}ads_data_node;

typedef struct{
	xmlChar ads_area_name[32];
	xmlChar ads_manufacturer_name[32];
	xmlChar rcvtype_name[32];
	xmlChar portion_name[32];
	u32 stb_start_id;
	u32 stb_end_id;
       u8 *date_url;
}ads_rcv_config;

typedef struct{
    u16 ads_data_version;
    u16 ads_rcv_version;
}ads_update_flag;

typedef struct{
        ads_update_flag netad_update_flag;
        /*此结构对应state.xml，标记广告数据与地区信息是否需要更新*/
        ads_data_node netad_attribute_node;
        /*所有广告信息与广告数据都在此结构中，与之对应的是all.xml*/
        u16 ads_count;
        ads_rcv_config netad_ads_rcv_config;
        /*当机顶盒的地区信息符合时此结构将会存下地区信息，与广告信息的连接*/
        BOOL is_rcv_legal;
        /*机顶盒是否处于广告分发范围*/
}netad_ads_data;

#define STATES_URL                        ((xmlChar*)"http://192.168.2.12/test/State.xml")
#define AREA_URL                            ((xmlChar*)"http://192.168.2.12/test/Area.xml")


#define ADVERTISINGS_STR		((xmlChar*)"advertisings")
#define ADVERTISING_STR		((xmlChar*)"advertising")
#define NUM_STR                             ((xmlChar*)"num")
#define MANE_STR                           ((xmlChar*)"name")
#define ID_STR                                ((xmlChar*)"id")
#define CHANNELS_STR                   ((xmlChar*)"channels")
#define CHANNEL_STR                     ((xmlChar*)"channel")
#define FILE_URL_STR                     ((xmlChar*)"file_url")
#define FORMAT_STR				((xmlChar*)"format")
#define POSITION_X_STR			((xmlChar*)"position_x")
#define POSITION_Y_STR			((xmlChar*)"position_y")
#define WIDTH_STR				((xmlChar*)"width")
#define HEIGHT_STR				((xmlChar*)"height")
#define DURATION_STR			((xmlChar*)"duration")
#define FILE_VERSION_STR		((xmlChar*)"file_version")
#define ATTRIBUTE_NAME_STR		((xmlChar*)"name")
#define BOOTLOGO_STR			((xmlChar*)"Bootlogo")
#define MAINMENU_STR			((xmlChar*)"MainMenu")
#define SUBMENUS_STR			((xmlChar*)"SubMenus")
#define STRIPE_STR				((xmlChar*)"Stripe")
#define RCV_CONFIGR_STR			((xmlChar*)"RCVconfig")
#define CFG_AREA_STR			((xmlChar*)"area")
#define CFG_MANUFACTURER_STR	((xmlChar*)"manufacturer") 
#define CFG_RCVTYPE_STR			((xmlChar*)"rcvtype") 
#define CFG_PORTION_STR			((xmlChar*)"portion") 
#define CFG_STB_START_ID_STR	((xmlChar*)"stb_start_id")
#define CFG_STV_END_ID_STR		((xmlChar*)"stb_end_id")
#define CURRENT_FOLLOW_STR		((xmlChar*)"current_follow")
#define CHANNER_INFO_STR		((xmlChar*)"channel_info")
#define CHANNER_TEXT_STR		((xmlChar*)"Text")
#define SUBMENU_MENU_NAME_STR	((xmlChar*)"Menu")
#define PIC_FORMAT_GIF_STR	        ((xmlChar*)"gif")
#define PIC_FORMAT_BMP_STR	((xmlChar*)"bmp")
#define PIC_FORMAT_JPG_STR	        ((xmlChar*)"jpg")
#define PIC_FORMAT_PNG_STR	((xmlChar*)"png")



RET_CODE ads_xml_parse(const xmlChar *p_buf, u32 buf_len, dvbad_ads_pic_data *p_dvbad_ads_pic_attribute_data);

