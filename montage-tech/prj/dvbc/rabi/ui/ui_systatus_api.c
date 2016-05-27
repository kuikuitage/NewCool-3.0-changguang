/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

//#define SS_DEBUG

#ifdef SS_DEBUG
#define SS_PRINTF    OS_PRINTF
#else
#ifndef WIN32
#define SS_PRINTF(param ...)    do { } while (0)
#else
#define SS_PRINTF
#endif
#endif

static BOOL _get_curn_info(group_set_t *p_group_set, u16 pos, u8 mode,
                           u16 *p_curn_rid)
{
  curn_info_t *p_group_info;
  u16 pg_id = INVALID;

  if (mode == CURN_MODE_NONE)
  {
    return FALSE;
  }

  p_group_info = &p_group_set->group_info[pos];
  pg_id =
    (mode == CURN_MODE_TV) ? p_group_info->curn_tv : p_group_info->curn_radio;

  if(pg_id == INVALIDID)
  {
    return FALSE;
  }

  *p_curn_rid = pg_id;

  return TRUE;
}


static BOOL _set_curn_info(group_set_t *p_group_set, u16 pos, u8 mode, u16 rid, u16 rpos)
{
  if (mode == CURN_MODE_NONE)
  {
    return FALSE;
  }

  if(mode == CURN_MODE_TV)
  {
    p_group_set->group_info[pos].curn_tv = rid;
    p_group_set->group_info[pos].tv_pos = rpos;
  }
  else
  {
    p_group_set->group_info[pos].curn_radio = rid;
    p_group_set->group_info[pos].rd_pos = rpos;
  }

  return TRUE;
}


static BOOL _check_curn_group(group_set_t *p_group_set, u8 mode)
{
  u16 i, rid, curn = p_group_set->curn_group;

  //check curn-> total-1
  for (i = curn; i < MAX_GROUP_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid))
    {
      p_group_set->curn_group = i;//curn;
      return TRUE;
    }
  }

  //check curn-1 -> 0
  for (i = 0; i < curn; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid))
    {
      p_group_set->curn_group = i;//curn;
      return TRUE;
    }
  }

  return FALSE;
}


static BOOL _check_curn_info(group_set_t *p_group_set)
{
  u8 mode = p_group_set->curn_mode;
  u32 org_context, new_context;
  u8 org_type, new_type;
  u16 pos;

  sys_status_get_group_info(p_group_set->curn_group, &org_type, &pos, &org_context);

  if (mode == CURN_MODE_NONE)
  {
    mode = CURN_MODE_TV;
  }

  // check curn mode
  if (_check_curn_group(p_group_set, mode))
  {
    goto _SET_CURN_MODE_;
  }

  // switch curn mode
  if (mode == CURN_MODE_TV)
  {
    mode = CURN_MODE_RADIO;
  }
  else
  {
    mode = CURN_MODE_TV;
  }

  // check other mode
  if (_check_curn_group(p_group_set, mode))
  {
    goto _SET_CURN_MODE_;
  }

  // no prog in all mode
  mode = CURN_MODE_NONE;
  p_group_set->curn_group = 0;

 _SET_CURN_MODE_:
  p_group_set->curn_mode = mode;

  sys_status_get_group_info(sys_status_get_curn_group(),
      &new_type, &pos, &new_context);

  return ((org_context == new_context) && (org_type == new_type));
}


BOOL sys_status_check_group(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;
  spec_check_group_t group = {{0}};
  u16 i = 0, j = 0;
  BOOL ret_boo = FALSE;
  //u32 ticks = mtos_ticks_get();

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  //to set the group info before check group
  //set all group
  group.all_group.orig_tv = p_group_set->group_info[0].curn_tv;
  group.all_group.orig_rd = p_group_set->group_info[0].curn_radio;
  group.all_group.orig_tv_pos = p_group_set->group_info[0].tv_pos;
  group.all_group.orig_rd_pos = p_group_set->group_info[0].rd_pos;

  //set sat group
  for(i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
    group.sat_group[i].context = (u16)p_group_set->group_info[i + 1].context;
    group.sat_group[i].orig_tv = p_group_set->group_info[i + 1].curn_tv;
    group.sat_group[i].orig_rd = p_group_set->group_info[i + 1].curn_radio;
    group.sat_group[i].orig_tv_pos = p_group_set->group_info[i + 1].tv_pos;
    group.sat_group[i].orig_rd_pos = p_group_set->group_info[i + 1].rd_pos;
  }

  //set fav group
  for(i = 0; i < MAX_FAV_GRP; i++)
  {
    group.fav_group[i].orig_tv = p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].curn_tv;
    group.fav_group[i].orig_rd = p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].curn_radio;
    group.fav_group[i].orig_tv_pos = p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].tv_pos;
    group.fav_group[i].orig_rd_pos = p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].rd_pos;
    group.fav_group[i].context = i;
  }

  //check group
  special_check_group(&group, NULL);

  //to reset the group info after check.
  //all group
    p_group_set->group_info[0].curn_tv = group.all_group.tv_id;
    p_group_set->group_info[0].tv_pos = group.all_group.tv_pos;
    p_group_set->group_info[0].curn_radio = group.all_group.rd_id;
    p_group_set->group_info[0].rd_pos = group.all_group.rd_pos;
  
  //reset sat group info
  for(i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
      p_group_set->group_info[i + 1].curn_tv = group.sat_group[i].tv_id;
      p_group_set->group_info[i + 1].tv_pos = group.sat_group[i].rd_id;
      p_group_set->group_info[i + 1].curn_radio = group.sat_group[i].rd_id;
      p_group_set->group_info[i + 1].rd_pos = group.sat_group[i].rd_id;
      p_group_set->group_info[i + 1].context = group.sat_group[i].context;      
    
    if((p_group_set->group_info[i + 1].curn_tv    == INVALIDID)
    && (p_group_set->group_info[i + 1].curn_radio == INVALIDID))
    {
      p_group_set->group_info[i + 1].context = INVALIDID;
    }

    group.sat_group[i].context = p_group_set->group_info[i + 1].context;
    group.sat_group[i].orig_rd = p_group_set->group_info[i + 1].curn_radio;
    group.sat_group[i].orig_tv = p_group_set->group_info[i + 1].curn_tv;
    group.sat_group[i].orig_rd_pos = p_group_set->group_info[i + 1].rd_pos;
    group.sat_group[i].orig_tv_pos = p_group_set->group_info[i + 1].tv_pos;
  }

  j=0;
  for(i=0; i<DB_DVBS_MAX_SAT; i++)
  {
    p_group_set->group_info[i + 1].context    = INVALIDID;
    p_group_set->group_info[i + 1].curn_radio = INVALIDID;
    p_group_set->group_info[i + 1].curn_tv    = INVALIDID;   
    p_group_set->group_info[i + 1].rd_pos = 0;
    p_group_set->group_info[i + 1].tv_pos = 0;  
    
    if(group.sat_group[i].context != INVALIDID)
    {
      p_group_set->group_info[j + 1].context    = group.sat_group[i].context;
      p_group_set->group_info[j + 1].curn_radio = group.sat_group[i].orig_rd;
      p_group_set->group_info[j + 1].curn_tv    = group.sat_group[i].orig_tv;
      p_group_set->group_info[j + 1].rd_pos = group.sat_group[i].orig_rd_pos;
      p_group_set->group_info[j + 1].tv_pos = group.sat_group[i].orig_tv_pos;
      
      j++;
    }
  }

  //reset fav group info
  for(i = 0; i < MAX_FAV_GRP; i++)
  {
      p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].curn_tv = group.fav_group[i].tv_id;
      p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].tv_pos = group.fav_group[i].tv_pos;      
      p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].curn_radio = group.fav_group[i].rd_id;
      p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].rd_pos = group.fav_group[i].rd_pos;
      p_group_set->group_info[i + DB_DVBS_MAX_SAT + 1].context = group.fav_group[i].context;
  }

  //set curn group and curn mode
  ret_boo = _check_curn_info(p_group_set);

  return ret_boo;
}


u16 sys_status_get_sat_group_num(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  u8 mode;
  u16 i, num = 0;
  u16 rid;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;


  for (i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
    if (_get_curn_info(p_group_set, i + 1, mode, &rid))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_sat_group_pos(u16 group)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  u8 mode;
  u16 i, num = 0;
  u16 rid;

  MT_ASSERT(group > 0);
  MT_ASSERT(group <= DB_DVBS_MAX_SAT);

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;


  for (i = 0; i < group; i++)
  {
    if (_get_curn_info(p_group_set, i + 1, mode, &rid))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_fav_group_num(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;
  u8 mode = CURN_MODE_NONE;
  u16 i = 0, num = 0;
  u16 rid = INVALIDID;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;

  for (i = 0; i < MAX_FAV_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i + DB_DVBS_MAX_SAT + 1, mode, &rid))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_fav_group_pos(u16 group)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;
  u8 mode = CURN_MODE_NONE;
  u16 i = 0, num = 0;
  u16 rid = INVALIDID;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;

  MT_ASSERT(group > DB_DVBS_MAX_SAT);
  MT_ASSERT(group <= (DB_DVBS_MAX_SAT + MAX_FAV_CNT));

  for (i = 0; i < (group - DB_DVBS_MAX_SAT); i++)
  {
    if (_get_curn_info(p_group_set, i + DB_DVBS_MAX_SAT + 1, mode, &rid))
    {
      num++;
    }
  }

  return num;
}

u16 sys_status_get_all_group_num(void)
{
  u16 num = 0;

  num += sys_status_get_sat_group_num();
  num += sys_status_get_fav_group_num();

  if(num > 0)
  {
    num++;
  }

  return num;
}

BOOL sys_status_get_group_all_info(u8 mode, u16 *p_curn_rid)
{
  u16 rid;
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_get_curn_info(p_group_set, 0, mode, &rid))
  {
    return FALSE;
  }

  *p_curn_rid = rid;

  return TRUE;
}

BOOL sys_status_get_sat_group_info(u16 sat_pos, u8 mode, u16 *p_curn_rid)
{
  u16 rid;
  group_set_t *p_group_set;
  sys_status_t *p_status;


  if (sat_pos >= DB_DVBS_MAX_SAT)
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_get_curn_info(p_group_set, sat_pos + 1, mode, &rid))
  {
    *p_curn_rid = INVALIDID;

    return FALSE;
  }

  *p_curn_rid = rid;

  return TRUE;
}


BOOL sys_status_get_fav_group_info(u16 fav_pos, u8 mode, u16 *p_curn_rid)
{
  u16 rid;
  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (fav_pos >= MAX_FAV_CNT)
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_get_curn_info(p_group_set, fav_pos + DB_DVBS_MAX_SAT + 1, mode, &rid))
  {
    *p_curn_rid = INVALIDID;
    return FALSE;
  }

  *p_curn_rid = rid;

  return TRUE;
}

BOOL sys_status_set_group_all_info(u8 mode, u16 curn_rid, u16 pos)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_set_curn_info(p_group_set, 0, mode, curn_rid, pos))
  {
    return FALSE;
  }

  return TRUE;
}

static BOOL sys_status_set_sat_group_info(u16 sat_pos, u8 mode, u16 curn_rid, u16 pos)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (sat_pos >= DB_DVBS_MAX_SAT)
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_set_curn_info(p_group_set, sat_pos + 1, mode, curn_rid, pos))
  {
    return FALSE;
  }

  return TRUE;
}


static BOOL sys_status_set_fav_group_info(u16 fav_pos, u8 mode, u16 curn_rid, u16 pos)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (fav_pos >= MAX_FAV_CNT)
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  if (!_set_curn_info(p_group_set, fav_pos + DB_DVBS_MAX_SAT + 1, mode, curn_rid, pos))
  {
    return FALSE;
  }

  return TRUE;
}

BOOL sys_status_get_group_info(u16 group, u8 *p_group_type, u16 *p_pos_in_set, u32 *p_context)
{
  sys_status_t *p_status;
  group_set_t *p_group_set;

  u16 pos;

  if (group >= MAX_GROUP_CNT)
  {
    return FALSE;
  }

  if (group == 0)
  {
    pos = 0;
    *p_group_type = GROUP_T_ALL;
  }
  else if (group < DB_DVBS_MAX_SAT + 1)
  {
    pos = group - 1;
    *p_group_type = GROUP_T_SAT;
  }
  else
  {
    pos = group - DB_DVBS_MAX_SAT - 1;
    *p_group_type = GROUP_T_FAV;
  }

  *p_pos_in_set = pos;


  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  *p_context =  p_group_set->group_info[group].context;

  return TRUE;
}


u16 sys_status_get_curn_group(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  return p_group_set->curn_group;
}


BOOL sys_status_set_curn_group(u16 group)
{
  u8 type;
  u16 pos;
  u32 context;

  group_set_t *p_group_set;
  sys_status_t *p_status;

  // check
  if (!sys_status_get_group_info(group, &type, &pos, &context))
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  p_group_set->curn_group = group;

  return TRUE;
}


static u16 goto_next_group(group_set_t *p_group_set, BOOL is_reverse)
{
  s16 curn_group, next_group;
  u16 rid = INVALIDID;

  curn_group = (s16)p_group_set->curn_group;
  next_group = curn_group;

  do
  {
    if (is_reverse)
    {
      next_group--;
      if (next_group < 0)
      {
        next_group = MAX_GROUP_CNT-1;
      }
    }
    else
    {
      next_group++;
      if (next_group == MAX_GROUP_CNT)
      {
        next_group = 0;
      }
    }

    if (next_group == curn_group)
    {
      // no group has pgs, except curn group
      break;
    }
  }while(!_get_curn_info(p_group_set, next_group, p_group_set->curn_mode, &rid));

  return (u16)next_group;
}

u16 sys_status_shift_curn_group(s16 offset)
{
  BOOL is_reverse;
  u16 i, count, curn_group;

  group_set_t *p_group_set;
  sys_status_t *p_status;

  count = ABS(offset);
  is_reverse = (BOOL)(offset < 0);

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  curn_group = p_group_set->curn_group;

  for (i = 0; i < count; i++)
  {
    curn_group = goto_next_group(p_group_set, is_reverse);
  }

//  curn_group = ((curn_group + offset) % all_num + all_num) % all_num;
  p_group_set->curn_group = curn_group;

  return curn_group;
}


BOOL sys_status_get_curn_prog_in_group(u16 group, u8 mode, u16 *p_rid, u32 *p_context)
{
  u8 group_type;
  u16 group_pos;
  u32 context;

  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  if (mode == CURN_MODE_NONE)
  {
    *p_rid = INVALIDID;
    return TRUE;
  }

  if (!sys_status_get_group_info(group, &group_type, &group_pos, &context))
  {
    return FALSE;
  }

  *p_context = context;

  switch (group_type)
  {
    case GROUP_T_FAV:
      sys_status_get_fav_group_info(group_pos, mode, p_rid);
      break;

    case GROUP_T_SAT:
      sys_status_get_sat_group_info(group_pos, mode, p_rid);
      break;

    default:
      p_status = sys_status_get();
      p_group_set = &p_status->group_set;
      _get_curn_info(p_group_set, 0, mode, p_rid);
  }

  return TRUE;
}


static BOOL sys_status_set_curn_prog_in_group(u16 group, u8 mode, u16 rid, u16 pos)
{
  u8 group_type;
  u16 group_pos;
  u32 context;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  if (mode == CURN_MODE_NONE)
  {
    return TRUE;
  }

  if (!sys_status_get_group_info(group, &group_type, &group_pos, &context))
  {
    return FALSE;
  }

  switch (group_type)
  {
    case GROUP_T_FAV:
      sys_status_set_fav_group_info(group_pos, mode, rid, pos);
      break;

    case GROUP_T_SAT:
      sys_status_set_sat_group_info(group_pos, mode, rid, pos);
      break;

    default: /* all group */
        sys_status_set_group_all_info(mode, rid, pos);
        break;
  }

  return TRUE;
}


u8 sys_status_get_curn_prog_mode(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  return p_group_set->curn_mode;
}


BOOL sys_status_set_curn_prog_mode(u8 mode)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  if (mode >= CURN_MODE_CNT)
  {
    return FALSE;
  }

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  p_group_set->curn_mode = mode;

  return TRUE;
}

u8 sys_status_get_group_curn_type(void)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;

  return p_group_set->curn_type;
}

BOOL sys_status_set_group_curn_type(u8 type)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  p_group_set->curn_type = type;

  return TRUE;
}


u16 sys_status_get_curn_group_curn_prog_id(void)
{
  u16 curn_group;
  u8 curn_mode;
  u16 pg_id;
  u32 context;

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();

  if(sys_status_get_curn_prog_in_group(curn_group, curn_mode, &pg_id, &context))
  {
    return pg_id;
  }
  else
  {
    return INVALIDID;
  }
}

BOOL sys_status_set_curn_group_info(u16 rid, u16 pos)
{
  u16 curn_group;
  u8 curn_mode;


  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();

  return sys_status_set_curn_prog_in_group(curn_group, curn_mode, rid,pos);

}

void sys_status_get_view_info(u16 group, u8 mode, u16 *p_view_type, u32 *p_context)
{
  u8 group_type, view_type;
  u16 pos_in_set;
  u32 context;

  if (mode == CURN_MODE_NONE)
  {
    *p_view_type = DB_DVBS_INVALID_VIEW;
    *p_context = 0;
    return;
  }

  sys_status_get_group_info(group, &group_type, &pos_in_set, &context);

  switch(group_type)
  {
    case GROUP_T_ALL:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_ALL_HIDE_TV:DB_DVBS_ALL_HIDE_RADIO;
      break;
    case GROUP_T_FAV:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_FAV_HIDE_TV:DB_DVBS_FAV_HIDE_RADIO;
      break;
    case GROUP_T_SAT:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_SAT_ALL_HIDE_TV:DB_DVBS_SAT_ALL_HIDE_RADIO;
      break;
    default:
      MT_ASSERT(0);
      return;
  }

  *p_view_type = view_type;
  *p_context = context;
}

void sys_status_get_view_all_info(u16 group, u8 mode, u16 *p_view_type, u32 *p_context)
{
  u8 group_type, view_type;
  u16 pos_in_set;
  u32 context;

  if (mode == CURN_MODE_NONE)
  {
    *p_view_type = DB_DVBS_INVALID_VIEW;
    *p_context = 0;
    return;
  }

  sys_status_get_group_info(group, &group_type, &pos_in_set, &context);

  switch(group_type)
  {
    case GROUP_T_ALL:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_ALL_TV:DB_DVBS_ALL_RADIO;
      break;
    case GROUP_T_FAV:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_FAV_TV:DB_DVBS_FAV_RADIO;
      break;
    case GROUP_T_SAT:
      view_type = (mode == CURN_MODE_TV)?DB_DVBS_SAT_ALL_TV:DB_DVBS_SAT_ALL_RADIO;
      break;
    default:
      MT_ASSERT(0);
      return;
  }

  *p_view_type = view_type;
  *p_context = context;
}


void sys_status_get_curn_view_info(u16 *p_view_type, u32 *p_context)
{
  u8 curn_mode;
  u16 curn_group;

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  sys_status_get_view_info(curn_group, curn_mode, p_view_type, p_context);
}


u16 sys_status_get_pos_by_group(u16 group)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;
  u8 mode;
  u16 i = 0, num = 0;
  u16 rid = INVALIDID;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;

  for(i = 0; i <= group; i++)
  {
    if(_get_curn_info(p_group_set, i, mode, &rid))
    {
      num++;
    }
  }

  return (num - 1);
}

u16 sys_status_get_group_by_pos(u16 pos)
{
  group_set_t *p_group_set;
  sys_status_t *p_status;
  u8 mode = CURN_MODE_NONE;
  u16 i = 0, num = 0;
  u16 rid = INVALIDID;

  p_status = sys_status_get();
  p_group_set = &p_status->group_set;
  mode = p_group_set->curn_mode;


  for (i = 0; i < MAX_GROUP_CNT; i++)
  {
    if (_get_curn_info(p_group_set, i, mode, &rid))
    {
      if(num == pos)
      {
        return i;
      }
      num++;
    }
  }

  return INVALIDPOS;
}

void sys_status_get_group_name(u16 group, u16 *p_name, u32 len)
{
  u8 type = 0;
  u16 pos = 0;
  u32 context = 0;
  sat_node_t sat;

  sys_status_get_group_info(group, &type, &pos, &context);
  switch (type)
  {
    case GROUP_T_ALL:
      gui_get_string(IDS_ALL, p_name, (u16)len);
      break;
    case GROUP_T_FAV:
      sys_status_get_fav_name((u8)context, p_name);
      break;
    case GROUP_T_SAT:
      db_dvbs_get_sat_by_id((u16)context, &sat);
      uni_strncpy(p_name, sat.name, len);
      break;
    default:
      MT_ASSERT(0);
  }
}

scart_v_format_t sys_status_get_scart_out_cfg(u8 focus)
{
  scart_v_format_t mode = SCART_VID_CVBS;

  switch(focus)
  {
    case 0: // RGB: DONT change the order.
      mode = SCART_VID_RGB;
      break;
    case 1: //YUV
      mode = SCART_VID_YUV;
      break;
    case 2: // CVBS
      mode = SCART_VID_CVBS;
      break;
    default:
      MT_ASSERT(0);
  }

  return mode;
}

avc_video_mode_1_t sys_status_get_sd_mode(u8 focus)
{
  avc_video_mode_1_t sd_mode = AVC_VIDEO_MODE_AUTO_1;

  switch(focus)
  {
    case 0:
      sd_mode = AVC_VIDEO_MODE_PAL_1;
      break;

    case 1:
      sd_mode = AVC_VIDEO_MODE_NTSC_1;
      break;

    case 2:
      sd_mode = AVC_VIDEO_MODE_AUTO_1;
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  return sd_mode;
}

avc_video_mode_1_t sys_status_get_hd_mode(u8 focus)
{
  avc_video_mode_1_t hd_mode = AVC_VIDEO_MODE_AUTO_1;
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  disp_sys_t video_std = VID_SYS_AUTO;

  avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);
  video_std = avc_get_video_mode_1(avc_handle);

  switch (video_std)
  {
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      switch(focus)
      {
        case 0:
          hd_mode = AVC_VIDEO_MODE_480I;
          break;
          
        case 1:
          hd_mode = AVC_VIDEO_MODE_480P;
          break;

        case 2:
          hd_mode = AVC_VIDEO_MODE_720P_60HZ;
          break;

        case 3:
          hd_mode = AVC_VIDEO_MODE_1080I_60HZ;
          break;

        case 4:
          hd_mode = AVC_VIDEO_MODE_1080P_60HZ;
            break;

        default:
          MT_ASSERT(0);
          break;
      }
      break;

    case VID_SYS_PAL:
    case VID_SYS_PAL_N:
    case VID_SYS_PAL_NC:
      switch(focus)
      {
        case 0:
          hd_mode = AVC_VIDEO_MODE_576I;
          break;
          
        case 1:
          hd_mode = AVC_VIDEO_MODE_576P;
          break;

        case 2:
          hd_mode = AVC_VIDEO_MODE_720P_50HZ;
          break;

        case 3:
          hd_mode = AVC_VIDEO_MODE_1080I_50HZ;
          break;

        case 4:
          hd_mode = AVC_VIDEO_MODE_1080P_50HZ;
            break;

        default:
          MT_ASSERT(0);
          break;
      }
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  return hd_mode;
}

avc_video_aspect_1_t sys_status_get_video_aspect(u8 focus)
{
  switch(focus)
  {
  case 0:
    return AVC_VIDEO_ASPECT_AUTO_1;

  case 1:
    return AVC_VIDEO_ASPECT_43_LETTERBOX_1;

  case 2:
    return AVC_VIDEO_ASPECT_43_PANSCAN_1;

  case 3:
    return AVC_VIDEO_ASPECT_169_1;

  default:
    return AVC_VIDEO_ASPECT_AUTO_1;
  }
}

scart_v_aspect_t sys_status_get_scart_aspect(u8 focus)
{
  MT_ASSERT(focus < 5);

  return (sys_status_get_video_aspect(focus) >= AVC_VIDEO_ASPECT_169_1) ?
         SCART_ASPECT_16_9 : SCART_ASPECT_4_3;
}


rf_sys_t sys_status_get_rf_system(u8 focus)
{
  rf_sys_t sys = RF_SYS_NTSC;

  return (rf_sys_t)(sys + focus);
}

u16 sys_status_get_rf_channel(BOOL is_pal, u8 focus)
{
  if(is_pal)  // PAL
  {
    focus += RF_PAL_MIN;
  }
  else // NTSC
  {
    focus += RF_NTSC_MIN;
  }

  return (u16)focus;
}

u8 sys_status_get_global_volume(void)
{
  sys_status_t *p_status = sys_status_get();
  return p_status->global_volume;
}

void sys_status_set_global_volume(u8 volume)
{
  sys_status_t *p_status = sys_status_get();
  p_status->global_volume = volume;
}

u8 sys_status_get_brightness(void)
{
  sys_status_t *p_status = sys_status_get();
  return p_status->brightness;
}

void sys_status_set_brightness(u8 brightness)
{
  sys_status_t *p_status = sys_status_get();
  p_status->brightness = brightness;
}

u8 sys_status_get_contrast(void)
{
  sys_status_t *p_status = sys_status_get();
  return p_status->contrast;
}

void sys_status_set_contrast(u8 contrast)
{
  sys_status_t *p_status = sys_status_get();
  p_status->contrast = contrast;
}

u8 sys_status_get_saturation(void)
{
  sys_status_t *p_status = sys_status_get();
  return p_status->saturation;
}

void sys_status_set_saturation(u8 saturation)
{
  sys_status_t *p_status = sys_status_get();
  p_status->saturation = saturation;
}

extern void ui_set_ttx_curn_sub(u8 index);
void sys_status_reload_environment_setting(void)
{
  BOOL is_enable;
  av_set_t av_set;
  osd_set_t osd_set;
  language_set_t lang_set;
  epg_lang_t epg_lang_set;
  void *p_disp_dev;
  u8 lang_code[3][3] = {{0}};
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);

  nim_device_t *p_dev0 = dev_find_identifier(NULL,
                                            DEV_IDT_TYPE,
                                            SYS_DEV_TYPE_NIM);

  //set lnb power
  sys_status_get_status(BS_LNB_POWER, &is_enable);
#ifndef WIN32
  dev_io_ctrl(p_dev0, NIM_IOCTRL_SET_LNB_ONOFF, (u32)is_enable);
#endif

  

 #ifndef WIN32
 #if ENABLE_BISS_KEY
 {
  cmd_t cmd_id = {0};
  sys_status_get_status(BS_TWIN_PORT, &is_enable);
  cmd_id.id = is_enable;
  ap_frm_do_command(APP_TWIN_PORT, &cmd_id);
  }
 #endif
 #endif
  //set video
  
  #ifdef TV_BIRTHNESS_EN
  avc_set_hd_video_bright(avc_handle, sys_status_get_brightness());
  #endif
  p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  disp_set_tv_sys(p_disp_dev, DISP_CHANNEL_HD, VID_SYS_1080I_50HZ);
  disp_set_tv_sys(p_disp_dev, DISP_CHANNEL_SD, VID_SYS_PAL);
 
  sys_status_get_av_set(&av_set);
  sys_status_get_epg_lang(&epg_lang_set);
  avc_switch_video_mode_1(avc_handle, sys_status_get_sd_mode(av_set.tv_mode));
  avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));
  avc_set_video_aspect_mode_1(avc_handle, sys_status_get_video_aspect(av_set.tv_ratio));
  #ifndef WIN32
   {
    void *p_disp = dev_find_identifier(NULL,
                                    DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
      p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
      MT_ASSERT(p_disp != NULL);
   disp_set_postprocess_mode(p_disp, av_set.video_effects);
   }
 #endif

  //set osd
  sys_status_get_osd_set(&osd_set);
  gdi_set_global_alpha((100 - osd_set.transparent) * 255 / 100);
#if ENABLE_TTX_SUBTITLE
  ui_enable_vbi_insert((BOOL)osd_set.enable_vbinserter);
#endif

  //set language
  sys_status_get_lang_set(&lang_set);

  rsc_set_curn_language(gui_get_rsc_handle(), lang_set.osd_text + 1);

  iso_639_get_lang_code(lang_code[0], epg_lang_set.first_lang);
  iso_639_get_lang_code(lang_code[1], epg_lang_set.second_lang);
  iso_639_get_lang_code(lang_code[2], epg_lang_set.default_lang);
#ifdef IMPL_NEW_EPG
  if(ui_epg_is_initialized())
  {
    epg_db_set_lang_code(lang_code[0], lang_code[1], lang_code[2]);
  }
#else
  ui_epg_set_lang_code(lang_code[0], lang_code[1], lang_code[2]);
#endif
}
