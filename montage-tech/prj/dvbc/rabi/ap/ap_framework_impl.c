/******************************************************************************/

/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "lib_util.h"
#include "lib_rect.h"
#include "hal_gpio.h"
#include "hal_misc.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"

#include "common.h"
#include "drv_dev.h"
#include "nim.h"
#include "uio.h"
#include "hal_watchdog.h"
#include "hal_misc.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "class_factory.h"
#include "mdl.h"
#include "service.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "mosaic.h"
#include "dvb_protocol.h"
#include "cat.h"
#include "pmt.h"
#include "nit.h"
#include "data_manager.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "db_dvbs.h"
#include "mt_time.h"
#include "audio.h"
#include "video.h"

#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_signal_monitor.h"
//#include "ap_satcodx.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"
#include "sys_status.h"
#include "i2c.h"
#include "hal_base.h"

#include "lib_util.h"
#include "lpower.h"
//#include "standby.h"
#include "customer_config.h"
#if 0
#if 1
static  led_bitmap_t fp_bitmap_t[] =

{

{'.', 0x20},

{'0', 0xD7},  {'1', 0x14},  {'2', 0xCD},  {'3', 0x5D},

{'4', 0x1E},  {'5', 0x5B},  {'6', 0xDB},  {'7', 0x15},

{'8', 0xDF},  {'9', 0x5F},  {'a', 0x9F},  {'A', 0x9F},

{'b', 0xDA},  {'B', 0xDA},  {'c', 0xC3},  {'C', 0xC3},

{'d', 0xDC},  {'D', 0xDC},  {'e', 0xCB},  {'E', 0xCB},

{'f', 0x8B},  {'F', 0x8B},  {'g', 0x5F},  {'G', 0xD3},

{'h', 0x9E},  {'H', 0x9E},  {'i', 0x10},  {'I', 0x82},

{'j', 0x54},  {'J', 0x54},  {'l', 0xC2},  {'L', 0xC2},

{'n', 0x98},  {'N', 0x97},  {'o', 0xD8},  {'O', 0xD7},

{'p', 0x8F},  {'P', 0x8F},  {'q', 0x1F},  {'Q', 0x1F},

{'r', 0x88},  {'R', 0x9F},  {'s', 0x5B},  {'S', 0x5B},

{'t', 0xCA},  {'T', 0x83},  {'u', 0xD6},  {'U', 0xD6},

{'y', 0x5E},  {'Y', 0x5E},  {'z', 0xCD},  {'Z', 0xCD},

{':', 0x20},  {'-', 0x08},  {'_', 0x40},  {' ', 0x00},

};

#else

 static led_bitmap_t fp_bitmap_t[] =
{
  {'.', 0x80},
  {'0', 0x3F},  {'1', 0x06},  {'2', 0x5B},  {'3', 0x4F},
  {'4', 0x66},  {'5', 0x6D},  {'6', 0x7D},  {'7', 0x07},
  {'8', 0x7F},  {'9', 0x6F},  {'a', 0x77},  {'A', 0x77},
  {'b', 0x7C},  {'B', 0x7C},  {'c', 0x39},  {'C', 0x39},
  {'d', 0x5E},  {'D', 0x5E},  {'e', 0x79},  {'E', 0x79},
  {'f', 0x71},  {'F', 0x71},  {'g', 0x6F},  {'G', 0x3D},
  {'h', 0x76},  {'H', 0x76},  {'i', 0x04},  {'I', 0x30},
  {'j', 0x0E},  {'J', 0x0E},  {'l', 0x38},  {'L', 0x38},
  {'n', 0x54},  {'N', 0x37},  {'o', 0x5C},  {'O', 0x3F},
  {'p', 0x73},  {'P', 0x73},  {'q', 0x67},  {'Q', 0x67},
  {'r', 0x50},  {'R', 0x77},  {'s', 0x6D},  {'S', 0x6D},
  {'t', 0x78},  {'T', 0x31},  {'u', 0x3E},  {'U', 0x3E},
  {'y', 0x6E},  {'Y', 0x6E},  {'z', 0x5B},  {'Z', 0x5B},
  {':', 0x80},  {'-', 0x40},  {'_', 0x08},  {' ', 0x00},
};
#endif

#define FP_TABLE_SIZE_T sizeof(fp_bitmap_t)/sizeof(led_bitmap_t)
#endif

static void ap_proc(void)
{

}


static void ap_restore_to_factory(void)
{
  //db_dvbs_restore_to_factory(PRESET_BLOCK_ID, 0);
 #if ((!defined WIN32) )
#endif

}


BOOL ap_get_standby(void)
{
#if ((!defined WIN32) )
return FALSE;
#endif

}


void ap_set_standby(u32 flag)
{
  #if ((!defined WIN32) )
 #endif

}

#define R_FPGPIO_OUT_EN_REG 0xbfedc028
#define R_FPGPIO_OUT_REG 0xbfedc02c
#define R_FPGPIO_MASK_REG 0xbfedc034
#define R_FPGPIO_RESET_REG 0xbfedc038

void ap_enter_tkgs_standby()
{
    audio_device_t *p_audio_dev = NULL;
    video_device_t *p_video_dev = NULL;
    void * pDev = NULL;

#ifdef SCART_ENABLE   
      avc_cfg_scart_select_tv_master(class_get_handle_by_id(AVC_CLASS_ID), SCART_TERM_VCR);
      avc_cfg_scart_vcr_input(class_get_handle_by_id(AVC_CLASS_ID), SCART_TERM_TV);
#endif

    p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
    MT_ASSERT(NULL != p_video_dev);
    vdec_stop(p_video_dev);

    p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    MT_ASSERT(NULL != p_audio_dev);
    aud_mute_onoff_vsb(p_audio_dev,TRUE);

    pDev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != pDev);
    disp_cvbs_onoff(pDev, CVBS_GRP0,  FALSE);

    if(disp_layer_is_show(pDev, DISP_LAYER_ID_OSD0))
    disp_layer_show(pDev, DISP_LAYER_ID_OSD0, FALSE);
    if(disp_layer_is_show(pDev, DISP_LAYER_ID_OSD1))
    disp_layer_show(pDev, DISP_LAYER_ID_OSD1, FALSE);
        
    pDev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_POW);
    MT_ASSERT(NULL != pDev);
    dev_close(pDev);
    
    mtos_close_printk();
    mtos_task_delay_ms(100);
      
}

extern RET_CODE uio_display(uio_device_t *p_dev, u8 *p_data, u32 len);

void ap_enter_standby(u32 tm_out)
{
#ifndef WIN32    
    hw_cfg_t hw_cfg = dm_get_hw_cfg_info();
    static  void *p_dev_sty = NULL;
    void *p_dev_uio = NULL;
    void *p_dev_nim = NULL;
    RET_CODE ret = SUCCESS;
    utc_time_t  curn_time,p_time;
    u32 cmd = 0;

    p_dev_sty = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_POW);
    MT_ASSERT(NULL != p_dev_sty);
    p_dev_uio = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
    MT_ASSERT(NULL != p_dev_uio);
    p_dev_nim = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    MT_ASSERT(NULL != p_dev_nim);

    concerto_standby_config_t info;

    memset(&info, 0, sizeof(info));
    /*        set standby time       */
    time_get(&curn_time, FALSE);

    /* select the correct FP */
    switch ((hal_fp_type_t)hw_cfg.fp_type)
    {
      case HAL_FD650:
        info.panel_config.panel_type = FD650;
         break;
      case HAL_CT1642:
      default:
        info.panel_config.panel_type = CT1642;
        break;
    }

    hw_cfg.fp_disp = DISP_OFF;
    if(DISP_TIME == hw_cfg.fp_disp && hw_cfg.led_num >= 4)
    {
      info.panel_config.time_enable = 1;
      info.panel_config.time_hh = curn_time.hour;
      info.panel_config.time_mm = curn_time.minute;
      OS_PRINTF("Standby time %d:%d\n", curn_time.hour, curn_time.minute);
    }
    else
    {
      info.panel_config.led_chars_enable = 1;
      info.panel_config.led_dot_mask = 0x04;
      info.panel_config.led_chars[0] = ' ';		  
      info.panel_config.led_chars[1] = ' ';  
      info.panel_config.led_chars[3] = ' ';
	  switch(CUSTOMER_ID)
	  {
	  	case CUSTOMER_XINNEW_DEMO:
		case CUSTOMER_MAIKE_HUNAN_LINLI:
		case CUSTOMER_DTMB_DESAI_JIMO:
		case CUSTOMER_DTMB_SZXC_GUOMAO:
		case CUSTOMER_XINNEW_GAIZHOU:
		case CUSTOMER_MAIKE_HUNAN:
		case CUSTOMER_DTMB_SHANGSHUIXIAN:
			info.panel_config.led_chars[2] = '.';
			break;
		case CUSTOMER_DTMB_CHANGSHA_HHT:
			info.panel_config.led_chars[1] = 'O';
			info.panel_config.led_chars[2] = 'F';
			info.panel_config.led_chars[3] = 'F';
			break;	
		default:
			info.panel_config.led_chars[2] = ' ';
			break;
	  }  

      void *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

	  switch(CUSTOMER_ID)
	  {
	  	case CUSTOMER_XINNEW_DEMO:
		case CUSTOMER_MAIKE_HUNAN_LINLI:
		case CUSTOMER_DTMB_DESAI_JIMO:
		case CUSTOMER_DTMB_SZXC_GUOMAO:
		case CUSTOMER_XINNEW_GAIZHOU:
		case CUSTOMER_MAIKE_HUNAN:
		case CUSTOMER_DTMB_SHANGSHUIXIAN:
			uio_display(p_dev, "   . ", 5);
			break;
		case CUSTOMER_DTMB_CHANGSHA_HHT:
			uio_display(p_dev, " OF.F", 5);
			break;
		default:
			uio_display(p_dev, "    ", 4);
			break;
	  }
    }


    sys_status_set_status(BS_UNFIRST_UNPOWER_DOWN,TRUE);
    time_to_gmt(&curn_time, &p_time);
    p_time.second = 0;
    sys_status_set_utc_time(&p_time);
    
    info.panel_config.wakeup_enable = 1;
    info.ir_config.wakeup_enable = 1;
    
     /* get FP POWER key from DM */;
    dm_read(class_get_handle_by_id(DM_CLASS_ID), 
            FPKEY_BLOCK_ID, 0, 0, 
            sizeof(u8), 
            (u8*)&info.panel_config.wakeup_key);
    info.panel_config.wakeup_key |= 0x100;
    
    /*        time wake up AP       */
     if(tm_out > 0)
    {
      info.timer_config.wakeup_enable = 1;
      info.timer_config.wakeup_duration = tm_out;
    }
    else
    {
      //Nothing to do yet.
    }

    /*set nim to sleep*/
#ifdef DTMB_PROJECT
    dev_io_ctrl(p_dev_nim, DEV_IOCTRL_POWER, DEV_POWER_SLEEP);
#else
    dev_io_ctrl(p_dev_nim, NIM_IOCTRL_SET_TUNER_SLEEP, 0);
#endif

    /* To avoid noise */
    gpio_io_enable(62, TRUE);
    gpio_set_dir(62, GPIO_DIR_OUTPUT);
    gpio_set_value(62, GPIO_LEVEL_LOW);

    cmd = SET_STANDBY_CONFIG;
    ret = ap_lpower_ioctl(p_dev_sty, cmd, (u32)&info);
    MT_ASSERT(ret == SUCCESS);


    /* Set wavefilter */
    dev_io_ctrl(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO), 
                UIO_IR_SET_WAVEFILT, 
                0);
	
    /*        enter standby       */
    ap_lpower_enter(p_dev_sty);
#endif
}


void ap_test_uart(void)
{
}


static BOOL ap_consume_ui_event(u32 ui_state, os_msg_t *p_msg)
{
  return FALSE;
}


/*!
   Construct APP. framework policy
  */
ap_frm_policy_t *construct_ap_frm_policy(void)
{
  ap_frm_policy_t *p_policy = mtos_malloc(sizeof(ap_frm_policy_t));
  MT_ASSERT(p_policy != NULL);
  memset(p_policy, 0, sizeof(ap_frm_policy_t));

  p_policy->enter_standby = ap_enter_standby;
  p_policy->extand_proc = ap_proc;
  p_policy->is_standby = ap_get_standby;
  p_policy->resotre_to_factory = ap_restore_to_factory;
  p_policy->set_standby = ap_set_standby;
  p_policy->test_uart = ap_test_uart;
  p_policy->consume_ui_event = ap_consume_ui_event;


  return p_policy;
}
