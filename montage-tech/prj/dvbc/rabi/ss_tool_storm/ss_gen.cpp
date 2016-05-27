// ss_gen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

sys_status_t g_status;

unsigned long str_nasc2uni(const char *p_ascstr, unsigned short *p_unistr, unsigned long count)
{
  unsigned long ncpy = count;

  if((NULL == p_ascstr) || (NULL == p_unistr))
  {
    return 0;
  }

  while((count != 0) && (*p_unistr++ = (unsigned short)(*p_ascstr++)))
  {
    count--; /* copy p_src over p_dst */
  }

  *p_unistr = 0;

  return ncpy - count;
}


int _tmain(int argc, _TCHAR* argv[])
{
  FILE *fp;
  u32 i, j;
  char fav_name[MAX_FAV_NAME_LEN];
  
  // set default value
  memset(&g_status, 0, sizeof(sys_status_t));
  
  g_status.e_lang_set.default_lang = 1;//rus
  g_status.e_lang_set.first_lang = 1;
  g_status.e_lang_set.second_lang = 1;//eng
  //changeset
  for(i = 0; i < MAX_CHANGESET_LEN; i++)
  {
	  g_status.changeset[i] = 0;
  }

  g_status.last_sat.sat_id = 1;

  /* language settting */
  g_status.lang_set.osd_text = 1;
  g_status.lang_set.sub_title = 0;
  g_status.lang_set.tel_text = 0;
  g_status.lang_set.first_audio = 1;
  g_status.lang_set.second_audio = 1;
  g_status.lang_set.text_encode_video = 3;
  g_status.lang_set.text_encode_music = 3;
  /* av settting */
  g_status.av_set.tv_mode = 0;
  g_status.av_set.tv_resolution  = 3;
  g_status.av_set.tv_ratio = 0;
  g_status.av_set.video_output = 0;
  g_status.av_set.digital_audio_output = 1;
  g_status.av_set.rf_system = 0;
  g_status.av_set.rf_channel = 24;
  /*g_status.av_set.transcoding = 0;*/
  g_status.av_set.video_effects = 3;
  
  /* play settting */
  g_status.play_set.type = PLAY_T_ALL;
  g_status.play_set.mode = CHCHANGE_M_FREEZE;

  /* osd settting */
  g_status.osd_set.palette = 1;
  g_status.osd_set.timeout = 5;
  g_status.osd_set.transparent = 0;
  g_status.osd_set.enable_subtitle = 0;
  g_status.osd_set.enable_teltext = 0;
  g_status.osd_set.enable_vbinserter = 1;

  /* password settting */
  g_status.pwd_set.normal = 0;
  g_status.pwd_set.super = 9999;
  
  /* preset setting */

  /* time settting */
  g_status.time_set.sys_time.year = 2009;
  g_status.time_set.sys_time.month = 1;
  g_status.time_set.sys_time.day = 1;
  g_status.time_set.sys_time.hour = 0;
  g_status.time_set.sys_time.minute = 0;
  g_status.time_set.sys_time.second = 0;
  g_status.time_set.gmt_offset = 39;
  g_status.time_set.gmt_usage = 1;
  g_status.time_set.summer_time = 0;
  
  /* group settting */
  g_status.group_set.curn_mode = CURN_MODE_NONE;
  g_status.group_set.curn_group = 0;
  g_status.group_set.curn_type = 3;

  /* book_info*/
  for(i=0; i<MAX_BOOK_PG; i++)
  {
    g_status.book_info.pg_info[i].start_time.year = 2009;
    g_status.book_info.pg_info[i].start_time.month = 1;
    g_status.book_info.pg_info[i].start_time.day = 1;
  }
  /* local setting */
  g_status.local_set.longitude = 0;
  g_status.local_set.latitude = 0;
  g_status.local_set.lnb_l = 9750;
  g_status.local_set.lnb_h = 10600;
  g_status.local_set.positioner_type = 0; // DiSEqC 1.2
  g_status.local_set.lnb_type = 2; // universal

  /*fav group*/
  for (i = 0; i < MAX_FAV_CNT; i++)
  {
    switch(i)
	{
	  case 0:
		  strcpy(fav_name, "Movies");
		  break;
	  case 1:
		  strcpy(fav_name, "News");
		  break;
	  case 2:
		  strcpy(fav_name, "Music");
		  break;
	  case 3:
		  strcpy(fav_name, "Sports");
		  break;
	  case 4:
		  strcpy(fav_name, "Child");
		  break;
	  case 5:
		  strcpy(fav_name, "Educate");
		  break;
	  case 6:
		  strcpy(fav_name, "Social");
		  break;
	  case 7:
		  strcpy(fav_name, "Culture");
		  break;
	}
    for (j = 0; j < strlen(fav_name); j++)
    {
      g_status.fav_set.fav_group[i].fav_name[j] = fav_name[j];
    }
    g_status.fav_set.fav_group[i].fav_name[j] = '\0';
  }

  /*sleep time*/
  g_status.sleep_time.hour = 0;

  g_status.global_volume = 10;

  g_status.brightness = 50;
  g_status.contrast = 50;
  g_status.saturation = 50;

  g_status.dvr_jump_time = 48;
  
  g_status.pic_showmode.slide_show = 0;
  g_status.pic_showmode.slide_time = 5;
  g_status.pic_showmode.slide_repeat = 0;
  g_status.pic_showmode.special_effect = 1;

  /* ota info */
  g_status.ota_info.ota_tri = OTA_TRI_MON_IN_LOADER;
  g_status.ota_info.sys_mode = SYS_DVBC;
  g_status.ota_info.download_data_pid = 0x1b58;
  g_status.ota_info.lockc.nim_modulate = NIM_MODULA_QAM64;
  g_status.ota_info.lockc.tp_sym = 6875;
  g_status.ota_info.lockc.tp_freq = 163000;

  /* bit status */

  g_status.auto_sleep = 0;

  g_status.timeshift_switch = 1;

  /*!
	*	ip address set!
  !*/
   //ipaddress
  g_status.ipaddress_set.sys_ipaddress.s_a1 = 192;
  g_status.ipaddress_set.sys_ipaddress.s_a2 = 168;
  g_status.ipaddress_set.sys_ipaddress.s_a3 = 32;
  g_status.ipaddress_set.sys_ipaddress.s_a4 = 85;
  //netmask
  g_status.ipaddress_set.sys_netmask.s_a1 = 255;
  g_status.ipaddress_set.sys_netmask.s_a2 = 255;
  g_status.ipaddress_set.sys_netmask.s_a3 = 252;
  g_status.ipaddress_set.sys_netmask.s_a4 = 0;
  //gateway
  g_status.ipaddress_set.sys_gateway.s_a1 = 192;
  g_status.ipaddress_set.sys_gateway.s_a2 = 168;
  g_status.ipaddress_set.sys_gateway.s_a3 = 32;
  g_status.ipaddress_set.sys_gateway.s_a4 = 1;
  //dnsserver
  g_status.ipaddress_set.sys_dnsserver.s_a1 = 192;
  g_status.ipaddress_set.sys_dnsserver.s_a2 = 168;
  g_status.ipaddress_set.sys_dnsserver.s_a3 = 35;
  g_status.ipaddress_set.sys_dnsserver.s_a4 = 8;

  g_status.ip_path_cnt = 0;

  strcpy(g_status.mac_set.mac_one,"D8");
  strcpy(g_status.mac_set.mac_two,"A5");
  strcpy(g_status.mac_set.mac_three,"B7");
  strcpy(g_status.mac_set.mac_four,"80");
  strcpy(g_status.mac_set.mac_five,"01");
  strcpy(g_status.mac_set.mac_six,"01");

  g_status.net_config_set.config_mode = DHCP;
  g_status.net_config_set.link_type = LINK_TYPE_LAN;

  g_status.youtube_config_set.region_id = 0;
  g_status.youtube_config_set.time_mode = 0;
  g_status.youtube_config_set.resolution = 0;

  g_status.nm_simple_config_set.resolution = NETMEDIA_VIDEO_SD;

  g_status.t2_search.country_style = 0;
  g_status.t2_search.antenna_state = 0;
  g_status.t2_search.lcn_state = 0;
  
  g_status.main_tp1.tp_freq = 163000;
  g_status.main_tp1.tp_sym = 6875;
  g_status.main_tp1.nim_modulate = 6;

  // write file
  fp = fopen("ss_data.bin", "w+b");
  fwrite(&g_status, 1, sizeof(sys_status_t), fp);
  fclose(fp);

	return 0;
}

