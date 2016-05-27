/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "data_base16v2.h"

/*
   CA_system_id values CA system specifier

   0x0000 Reserved
   0x0001 to 0x00FF Standardized systems
   0x0100 to 0x01FF Canal Plus
   0x0200 to 0x02FF CCETT
   0x0300 to 0x03FF Deutsche Telecom
   0x0400 to 0x04FF Eurodec
   0x0500 to 0x05FF France Telecom
   0x0600 to 0x06FF Irdeto
   0x0700 to 0x07FF Jerrold/GI
   0x0800 to 0x08FF Matra Communication
   0x0900 to 0x09FF News Datacom
   0x0A00 to 0x0AFF Nokia
   0x0B00 to 0x0BFF Norwegian Telekom
   0x0C00 to 0x0CFF NTL
   0x0D00 to 0x0DFF Philips
   0x0E00 to 0x0EFF Scientific Atlanta
   0x0F00 to 0x0FFF Sony
   0x1000 to 0x10FF Tandberg Television
   0x1100 to 0x11FF Thomson
   0x1200 to 0x12FF TV/Com
   0x1300 to 0x13FF HPT - Croatian Post and Telecommunications
   0x1400 to 0x14FF HRT - Croatian Radio and Television
   0x1500 to 0x15FF IBM
   0x1600 to 0x16FF Nera
   0x1700 to 0x17FF BetaTechnik
  */

static char *g_ca_system_desc[] =
{
  "",
  "Standardized systems",
  "Canal Plus",
  "CCETT",
  "Deutsche Telecom",
  "Eurodec",
  "France Telecom",
  "Irdeto",
  "Jerrold/GI",
  "Matra Communication",
  "News Datacom",
  "Nokia",
  "Norwegian Telekom",
  "NTL",
  "Philips",
  "Scientific Atlanta",
  "Sony",
  "Tandberg Television",
  "Thomson",
  "TV/Com",
  "HPT - Croatian Post and Telecommunications",
  "HRT - Croatian Radio and Television",
  "IBM",
  "Nera",
  "BetaTechnik",
};

#define CA_SYSTIME_ID_MAX_CNT ((sizeof(g_ca_system_desc)) / (sizeof(char *)))
/*!
 * program sort struct
 */
typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  service number in stream 
  */
  u16 s_id;
}prog_sort_t;

/*!
 * logic number sort struct
 */
typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  logic number in stream
  */
  u16 logic_num;
}prog_logic_num_sort_t;

typedef struct
{
 /*!
  progame node id.
  */
  u16 node_id;
 /*!
  service number in stream 
  */
  u16 s_id;
 /*!
  service number and tp
  */
  u32 fre;
}prog_sort2_t;

static u8 g_pg_view_id = 0;

static u8 *p_sat_view_buf = NULL;
static u8 *p_sel_sat_view_buf = NULL;
static u8 *p_tp_view_buf = NULL;
static u8 *p_sat_tp_view_buf = NULL;
static u8 *p_pg_view_buf = NULL;

extern BOOL have_logic_number();

void ui_dbase_init(void)
{
  p_sat_view_buf = mtos_malloc(DB_DVBS_MAX_SAT * sizeof(item_type_t));
  MT_ASSERT(p_sat_view_buf != NULL);
  memset(p_sat_view_buf, 0, DB_DVBS_MAX_SAT * sizeof(item_type_t));

  p_sel_sat_view_buf = mtos_malloc(DB_DVBS_MAX_SAT * sizeof(item_type_t));
  MT_ASSERT(p_sel_sat_view_buf != NULL);
  memset(p_sel_sat_view_buf, 0, DB_DVBS_MAX_SAT * sizeof(item_type_t));

  p_tp_view_buf = mtos_malloc(DB_DVBS_MAX_TP * sizeof(item_type_t));
  MT_ASSERT(p_tp_view_buf != NULL);
  memset(p_tp_view_buf, 0, DB_DVBS_MAX_TP * sizeof(item_type_t));

  p_sat_tp_view_buf = mtos_malloc(DB_DVBS_MAX_TP * sizeof(item_type_t));
  MT_ASSERT(p_sat_tp_view_buf != NULL);
  memset(p_sat_tp_view_buf, 0, DB_DVBS_MAX_TP * sizeof(item_type_t));

  p_pg_view_buf = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(item_type_t));
  MT_ASSERT(p_pg_view_buf != NULL);
  memset(p_pg_view_buf, 0, DB_DVBS_MAX_PRO * sizeof(item_type_t));
}


u8 ui_dbase_create_view(dvbs_view_t name, u32 context, u8 *p_ex_buf)
{
  u8 *p_view_buf = NULL;

  if(p_ex_buf != NULL)
  {
    p_view_buf = p_ex_buf;
  }
  else
  {
    switch(name)
    {
      case DB_DVBS_ALL_RADIO:
      case DB_DVBS_ALL_RADIO_FTA:
      case DB_DVBS_ALL_RADIO_CAS:
      case DB_DVBS_ALL_RADIO_HD:
      case DB_DVBS_ALL_TV:
      case DB_DVBS_ALL_TV_FTA:
      case DB_DVBS_ALL_TV_CAS:
      case DB_DVBS_ALL_TV_HD:
      case DB_DVBS_FAV_RADIO:
      case DB_DVBS_FAV_RADIO_FTA:
      case DB_DVBS_FAV_RADIO_CAS:
      case DB_DVBS_FAV_RADIO_HD:
      case DB_DVBS_FAV_TV:
      case DB_DVBS_FAV_ALL_TV:
      case DB_DVBS_FAV_TV_FTA:
      case DB_DVBS_FAV_TV_CAS:
      case DB_DVBS_FAV_TV_HD:
      case DB_DVBS_ALL_PG:
      case DB_DVBS_FAV_ALL:
      case DB_DVBS_SAT_ALL_RADIO:
      case DB_DVBS_SAT_ALL_RADIO_FTA:
      case DB_DVBS_SAT_ALL_RADIO_CAS:
      case DB_DVBS_SAT_ALL_RADIO_HD:
      case DB_DVBS_SAT_ALL_TV:
      case DB_DVBS_SAT_ALL_TV_FTA:
      case DB_DVBS_SAT_ALL_TV_CAS:
      case DB_DVBS_SAT_ALL_TV_HD:
      case DB_DVBS_SAT_PG:
      case DB_DVBS_TP_PG:
      case DB_DVBS_HIDE_TV:
      case DB_DVBS_ALL_HIDE_TV: 
      case DB_DVBS_ALL_HIDE_RADIO:
      case DB_DVBS_FAV_HIDE_TV:
      case DB_DVBS_FAV_HIDE_RADIO:       
      case DB_DVBS_SAT_ALL_HIDE_RADIO:    
      case DB_DVBS_SAT_ALL_HIDE_TV:         
        p_view_buf = p_pg_view_buf;
        break;
      case DB_DVBS_SAT_TP:
        p_view_buf = p_sat_tp_view_buf;
        break;
      case DB_DVBS_ALL_TP:
        p_view_buf = p_tp_view_buf;
        break;
      case DB_DVBS_SAT:
      case DB_DVBS_T2_SAT:
      case DB_DVBS_CABLE_SAT:
        p_view_buf = p_sat_view_buf;
        break;
      case DB_DVBS_SELECTED_SAT:
        p_view_buf = p_sel_sat_view_buf;
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }

  return db_dvbs_create_view(name, context, p_view_buf);
}


dvbs_view_t ui_dbase_get_prog_view(dvbs_prog_node_t *p_node)
{
  return (dvbs_view_t)(p_node->video_pid ? DB_DVBS_ALL_HIDE_TV : DB_DVBS_ALL_HIDE_RADIO);
}


u16 ui_dbase_get_sat_by_pos(u8 view_id, sat_node_t *p_node, u16 pos)
{
  u16 sat_id;

  sat_id = db_dvbs_get_id_by_view_pos(view_id, pos);
  db_dvbs_get_sat_by_id(sat_id, p_node);

  return sat_id;
}


u16 ui_dbase_get_tp_by_pos(u8 view_id, dvbs_tp_node_t *p_node, u16 pos)
{
  u16 tp_id;

  tp_id = db_dvbs_get_id_by_view_pos(view_id, pos);
  db_dvbs_get_tp_by_id(tp_id, p_node);

  return tp_id;
}


void ui_dbase_get_full_prog_name(dvbs_prog_node_t *p_node, u16 *str, u16 max_length)
{
  u32 len;

  str_asc2uni(p_node->is_scrambled ? "$ " : "", str);
  len = uni_strlen(str);

  if(len < max_length)
  {
    uni_strncpy(str + len, p_node->name, max_length - len);
  }
}


BOOL ui_dbase_pg_is_scambled(u16 rid)
{
  dvbs_prog_node_t pg;

  if(rid == INVALIDID)
  {
    return FALSE;
  }

  if(db_dvbs_get_pg_by_id(rid, &pg) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }

  return (BOOL)(pg.is_scrambled);
}


void ui_dbase_get_diseqc_str(u16 *str, u16 len, sat_node_t *p_node)
{
  u8 asc_buf[32];
  u16 uni_buf[32];

  if(p_node->diseqc_type == 0)  /* is disable ?*/
  {
    gui_get_string(IDS_OFF, uni_buf, 31);
  }
  else
  {
    sprintf(asc_buf, "%d/%d",
            p_node->diseqc_port + 1,
            1 << (p_node->diseqc_type + 1));
    str_asc2uni(asc_buf, uni_buf);
  }
  uni_strcat(str, uni_buf, len);
}


void ui_dbase_get_22k_str(u16 *str, u16 len, sat_node_t *p_node)
{
  u16 uni_buf[32];

  gui_get_string((u16)(p_node->k22 ? IDS_ON : IDS_OFF), uni_buf, 32);
  uni_strcat(str, uni_buf, len);
}


BOOL ui_dbase_pg_is_full(void)
{
  //u8 view = (u8)ui_sys_get_data(SS_CUR_VIEW);
  BOOL is_full = is_pg_full();

  ui_dbase_reset_pg_view();
  return is_full;
}


BOOL ui_dbase_pg_ask_for_belong(BOOL is_all_sat, u16 pg_id, u16 *sat_pos, u16 *tp_pos)
{
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;
  u8 view_id;

  *sat_pos = *tp_pos = INVALIDPOS;

  if(pg_id == INVALIDID)
  {
    return FALSE;
  }

  if(db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    return FALSE;
  }

  if(db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp) != DB_DVBS_OK)
  {
    return FALSE;
  }

  view_id = ui_dbase_create_view(is_all_sat ? DB_DVBS_SAT : DB_DVBS_SELECTED_SAT, 0, NULL);
  *sat_pos = db_dvbs_get_view_pos_by_id(view_id, (u16)tp.sat_id);

  if(*sat_pos != INVALIDPOS)
  {
    view_id = ui_dbase_create_view(DB_DVBS_SAT_TP, tp.sat_id, NULL);
    *tp_pos = db_dvbs_get_view_pos_by_id(view_id, (u16)pg.tp_id);
  }

  return TRUE;
}


u32 ui_dbase_get_pg_curn_audio_pid(dvbs_prog_node_t *p_node)
{
  return p_node->audio[p_node->audio_channel].p_id;
}


void ui_dbase_set_pg_curn_audio_pid(dvbs_prog_node_t *p_node, u32 pid)
{
  p_node->audio[p_node->audio_channel].p_id = (u16)pid;
}


void ui_dbase_set_pg_view_id(u8 view_id)
{
  g_pg_view_id = view_id;
}


u8 ui_dbase_get_pg_view_id(void)
{
  return g_pg_view_id;
}


void ui_dbase_reset_pg_view(void)
{
  //u16 view = ui_sys_get_data(SS_CUR_VIEW);
  u16 view;
  u32 context;

  sys_status_get_curn_view_info(&view, &context);

  if(view != DB_DVBS_INVALID_VIEW)
  {
    /* create view for new view */
    ui_dbase_set_pg_view_id(ui_dbase_create_view(view, context, NULL));
  }
}


void ui_dbase_delete_all_pg(void)
{
  u8 view_id;
  u16 i, cnt;
  u16 param = DB_DVBS_PARAM_ACTIVE_FLAG;

  view_id = ui_dbase_create_view(DB_DVBS_ALL_PG, 0, NULL);
  cnt = db_dvbs_get_count(view_id);

  for(i = 0; i < cnt; i++)
  {
    db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, param);
  }
  db_dvbs_save(view_id);
}


BOOL ui_dbase_pg_is_fav(u8 view_id, u16 pos)
{
  u8 dig;

  for(dig = 0; dig < MAX_FAV_GRP; dig++)
  {
    if(db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, dig) == TRUE)
    {
      //if(db_dvbs_get_mark_value(view_id, pos, dig))
      return TRUE;
    }
  }

  return FALSE;
}


BOOL ui_dbase_check_tvradio(BOOL is_tv)
{
  u8 old_mode;
  u16 old_group;
  u16 view_type;
  u32 group_context;
  BOOL ret = FALSE;

  old_mode = sys_status_get_curn_prog_mode();
  old_group = sys_status_get_curn_group();

  sys_status_get_view_info(old_group, old_mode, &view_type, &group_context);

  if(db_dvbs_get_count(ui_dbase_create_view(
                         is_tv ? DB_DVBS_ALL_HIDE_TV : DB_DVBS_ALL_HIDE_RADIO, 0, NULL)))
  {
    ret = TRUE;
  }

  //reset current view
  ui_dbase_set_pg_view_id(ui_dbase_create_view(view_type, group_context, NULL));

  return ret;
}


static u32 freq_val[FREQ_VAL_CNT] =
{
  5150, 5750, 5950, 9750, 10000, 10050,
  10450, 10600, 10700, 10750, 11250, 11300
};

static u32 universal_freq_map_tab[FREQ_UNIVERSAL_CNT] = 
{
  10550, 10600, 10700, 10750
};

void ui_dbase_pos2freq(u16 pos, u32 *p_lnbtype, u32 *p_freq1, u32 *p_freq2,
                       u8 *p_is_unicable, u8 *p_unicable_type)
{
  {
    *p_is_unicable = 0;
    *p_unicable_type = 0;
    *p_freq1 = 0;
    *p_freq2 = 0;
    *p_lnbtype = 0;

    if(pos < LNB_FREQ_CNT)
    {
      if(pos < FREQ_VAL_CNT)
      {
        *p_lnbtype = 0; // standard
        *p_freq1 = *p_freq2 = freq_val[pos];
      }
      else if(pos < FREQ_VAL_CNT + FREQ_USER_CNT)
      {
        *p_lnbtype = 1; // user
        if(pos == FREQ_VAL_CNT)
        {
          *p_freq1 = 5150;
          *p_freq2 = 5750;
        }
        else
        {
          *p_freq1 = 5750;
          *p_freq2 = 5150;
        }
      }
      else if(pos < (FREQ_VAL_CNT + FREQ_USER_CNT + FREQ_UNIVERSAL_CNT))
      {
        *p_lnbtype = 2; // universal
        *p_freq1 = 9750;
        *p_freq2 = universal_freq_map_tab[pos - (FREQ_VAL_CNT + FREQ_USER_CNT)];
      }
      else
      {
        *p_lnbtype = 2; //unicable_universal
        *p_is_unicable = TRUE;
        *p_unicable_type = (pos - (FREQ_VAL_CNT + FREQ_USER_CNT + FREQ_UNIVERSAL_CNT));
        *p_freq1 = 9750;
        *p_freq2 = 10600;
      }
    }
    else
    {
      UI_PRINTF("pos2feq: ERROR, invaild pos (%d)\n", pos);
      MT_ASSERT(0);
      *p_lnbtype = *p_freq1 = *p_freq2 = 0;
    }
  }
}


static u16 get_freq_index(u32 freq)
{
  u16 i;

  for(i = 0; i < FREQ_VAL_CNT; i++)
  {
    if(freq_val[i] == freq)
    {
      return i;
    }
  }

  UI_PRINTF("get_freq_index: ERROR, freq(%d) is NOT matched\n", freq);
  return 0;
}

u16 ui_dbase_freq2pos(u32 lnbtype, u32 freq1, u32 freq2, u8 p_is_unicable, u8 p_unicable_type)
{
  u16 pos;
  u16 i;

  {
    switch(lnbtype)
    {
      case 0: // standard
//      MT_ASSERT(freq1 == freq2);
        pos = get_freq_index(freq1);
        break;
      case 1: // user
//      MT_ASSERT(freq1 != freq2);
        pos = freq1 < freq2 ? FREQ_VAL_CNT : FREQ_VAL_CNT + 1;
        break;
      case 2: // universal
        if(p_is_unicable == 0)
        {
          for (i = 0; i < FREQ_UNIVERSAL_CNT; i++)
          {
            if (freq2 == universal_freq_map_tab[i])
            {
              break;
            }
          }
          if (i < FREQ_UNIVERSAL_CNT)
          {
            pos = FREQ_VAL_CNT + FREQ_USER_CNT + i;
          }
          else
          {
            MT_ASSERT(0);
            pos = 0;
          }
        }
        else
        {
          pos = FREQ_VAL_CNT + FREQ_USER_CNT + FREQ_UNIVERSAL_CNT + p_unicable_type;
        }
        break;
      default:
        MT_ASSERT(0);
        pos = 0;
    }
  }
  return pos;
}


BOOL ui_dbase_reset_last_prog(BOOL focus_first)
{
  u8 mode, view_id;
  BOOL ret = FALSE;
  u16 view_type = 0;
  u32 context = 0;


  sys_status_check_group();

  mode = sys_status_get_curn_prog_mode();

  if(mode != CURN_MODE_NONE)
  {
    // set curn group to group_all
  //  sys_status_set_curn_group(0);
   // view_id = ui_dbase_create_view(
   //   (mode == CURN_MODE_TV) ? DB_DVBS_ALL_HIDE_TV : DB_DVBS_ALL_HIDE_RADIO, 0, NULL);
     sys_status_get_view_info(sys_status_get_curn_group(), mode, &view_type, &context);
    view_id = ui_dbase_create_view(view_type, context, NULL);
    ui_dbase_set_pg_view_id(view_id);

    //if it's first scan, focus on first pg.
    if(focus_first)
    {
      sys_status_set_curn_group_info(db_dvbs_get_id_by_view_pos(view_id, 0),0);
    }

    ret = TRUE;
  }
  // save
  sys_status_save();

  return ret;
}


void ui_dbase_delete_all_position(void)
{
  u16 total, i;
  u16 rid;
  u8 sat_vid;
  sat_node_t sat;

  sat_vid = ui_dbase_create_view(DB_DVBS_SAT, 0, NULL);
  total = db_dvbs_get_count(sat_vid);

  for(i = 0; i < total; i++)
  {
    rid = db_dvbs_get_id_by_view_pos(sat_vid, i);
    db_dvbs_get_sat_by_id(rid, &sat);
    if(sat.position != 0
      || sat.positioner_type == 1) // DiSEqC1.2
    {
      sat.position = 0;
      sat.positioner_type = 0;

      db_dvbs_edit_satellite(&sat);
      db_dvbs_save_sat_edit(&sat);
    }
  }
}


char *ui_dbase_get_ca_system_desc(dvbs_prog_node_t *p_pg)
{
  u32 index = 0;

  MT_ASSERT(p_pg != NULL);

  if(p_pg->ca_system_id == 0
    || !p_pg->is_scrambled)
  {
    index = 0; // reserved
  }
  else
  {
    index = SYS_GET_BYTE1(p_pg->ca_system_id) + 1 /* skip reserved */;
    if(index > CA_SYSTIME_ID_MAX_CNT)
    {
      OS_PRINTF("ERROR! ca_system_id = 0x%x, force change to 0\n", p_pg->ca_system_id);
      index = 0;
    }
  }

  if(index >= sizeof(g_ca_system_desc) / sizeof(char *))
  {
    index = 0; // reserved
  }
  return g_ca_system_desc[index];
}

int pg_cmp_logic_num(const void *node1, const void *node2)
{
  prog_logic_num_sort_t *pre_node = (prog_logic_num_sort_t *) node1;
  prog_logic_num_sort_t *cur_node = (prog_logic_num_sort_t *) node2;
  
  return (pre_node->logic_num- cur_node->logic_num);
}

int pg_cmp_sev_id(const void *node1, const void *node2)
{
  prog_sort_t *pre_node = (prog_sort_t *) node1;
  prog_sort_t *cur_node = (prog_sort_t *) node2;
  
  return (pre_node->s_id - cur_node->s_id);
}

//lint -e438 -e550
void ui_dbase_pg_sort_by_sid(u8 view_id)
{
  u16 view_cnt  = 0;
  u16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_sort_t *p_sort_buf = NULL;
  prog_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  #ifdef SHUSHITONG_CA
  dvbs_tp_node_t tp = {0};
  #endif
  
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, (u16)view_cnt * sizeof(u16));
  
  p_pro_item = (prog_sort_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;

	if(CUSTOMER_DTMB_CHANGSHA_HHT == CUSTOMER_ID)
	{
		#ifdef SHUSHITONG_CA
		db_dvbs_get_tp_by_id((u16)prog_node.tp_id, &tp);
		if(freq_is_valid(tp.freq))
			p_pro_item->s_id = (u16)prog_node.s_id;
		else
			p_pro_item->s_id = (u16)prog_node.s_id | 0x8000;
		#endif
	}
	else
	{
		p_pro_item->s_id = (u16)prog_node.s_id;
	}

    p_pro_item++;
  }

//  shellsort(p_sort_buf, view_cnt);
  qsort(p_sort_buf, view_cnt, sizeof(prog_sort_t), pg_cmp_sev_id);
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}
//lint -e732
void ui_dbase_pg_sort_by_logic_num(u8 view_id)
{
  u16 view_cnt  = 0;
  u16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_logic_num_sort_t *p_sort_buf = NULL;
  prog_logic_num_sort_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(view_cnt * sizeof(prog_logic_num_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, view_cnt * sizeof(prog_logic_num_sort_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, view_cnt * sizeof(u16));
  
  p_pro_item = (prog_logic_num_sort_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;
    p_pro_item->logic_num= prog_node.logical_num;
    p_pro_item++;
  }

  qsort(p_sort_buf, view_cnt, sizeof(prog_logic_num_sort_t), pg_cmp_logic_num);
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);
  
  db_dvbs_save(view_id);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}

static int pg_cmp_fre_and_sid(const void *node1, const void *node2)
{
  prog_sort2_t *pre_node = (prog_sort2_t *) node1;
  prog_sort2_t *cur_node = (prog_sort2_t *) node2;

  if (pre_node->fre != cur_node->fre)
  	return (pre_node->fre - cur_node->fre);
  else
  	return (pre_node->s_id - cur_node->s_id);
}

static void ui_dbase_pg_sort_by_fre_and_sid(u8 view_id)
{
  u16 view_cnt = 0;
  u16 i = 0;
  dvbs_prog_node_t prog_node = {0};
  db_dvbs_ret_t   ret = DB_DVBS_OK;
  prog_sort2_t *p_sort_buf = NULL;
  prog_sort2_t *p_pro_item = NULL;
  u16 element_id = 0;
  u16 *p_node_id_mem = NULL;
  dvbs_tp_node_t tp;
  view_cnt = db_dvbs_get_count(view_id);

  p_sort_buf = mtos_malloc(/*DB_DVBS_MAX_PRO*/ view_cnt * sizeof(prog_sort2_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, /*DB_DVBS_MAX_PRO*/view_cnt * sizeof(prog_sort2_t));

  p_node_id_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_node_id_mem != NULL);
  memset(p_node_id_mem, 0x0, (u16)view_cnt * sizeof(u16));
  
  p_pro_item = (prog_sort2_t *)p_sort_buf;

  for(i = 0; i < view_cnt; i++)
  {
    element_id = db_dvbs_get_id_by_view_pos(view_id, i);
    ret = db_dvbs_get_pg_by_id(element_id, &prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);
    p_pro_item->node_id = element_id;

    if(CUSTOMER_DTMB_CHANGSHA_HHT == CUSTOMER_ID)
    {
    	if(prog_node.is_scrambled)
    		p_pro_item->s_id = (u16)prog_node.s_id;
    	else
    		p_pro_item->s_id = (u16)prog_node.s_id | 0x8000;
    }
    else
    {
    	p_pro_item->s_id = (u16)prog_node.s_id;
    }
    if (db_dvbs_get_tp_by_id(prog_node.tp_id, &tp) == DB_DVBS_OK)
	p_pro_item->fre = tp.freq;
    else
	p_pro_item->fre = 0;

    p_pro_item++;
  }

//  shellsort(p_sort_buf, view_cnt);
  qsort(p_sort_buf, view_cnt, sizeof(prog_sort2_t), pg_cmp_fre_and_sid);
  for(i = 0; i < view_cnt;i++)
  {
    *(p_node_id_mem + i) = p_sort_buf[i].node_id | 0x8000;
  }

  db_dvbs_dump_item_in_view(view_id, p_node_id_mem, view_cnt);

  mtos_free(p_node_id_mem);
  mtos_free(p_sort_buf);
}

void ui_dbase_pg_sort(dvbs_view_t name)
{
  u8 view_id;
  
  MT_ASSERT(name<DB_DVBS_INVALID_VIEW);
  
  view_id = db_dvbs_create_view(name, 0, NULL);
  if(db_dvbs_get_count(view_id) > 0)
  {
  	if(CUSTOMER_ID == CUSTOMER_DTMB_PCBA_GUOMAO)
  	{
		ui_dbase_pg_sort_by_fre_and_sid(view_id);
	}
	else
	{
		if(have_logic_number())
		  ui_dbase_pg_sort_by_logic_num(view_id);
		else
		  ui_dbase_pg_sort_by_sid(view_id);
	}
    //ui_dbase_pg_sort_by_sid(view_id);
    db_dvbs_save(view_id);
  }
}

