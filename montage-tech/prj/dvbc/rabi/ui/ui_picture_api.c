#include "ui_common.h"
#include "ui_config.h"
#include "pic_api.h"
#include "ui_usb_music.h"
#include "ui_picture_api.h"
#include "ui_pic_play_mode_set.h"

typedef enum
{
  PIC_API_EVT_DRAW_END = ((APP_PICTURE << 16) + 0),
    PIC_API_EVT_DRAW_START,
  PIC_API_EVT_SIZE_UPDATE,
  PIC_API_EVT_TOO_LARGE,
  PIC_API_EVT_UNSUPPORT,
  PIC_API_EVT_DATA_ERROR,
}pic_api_evt_t;

#define RAND(x)    ((u16)mtos_ticks_get()%(x)) 
#define UI_MUSIC_EVT_PLAY_END  ((APP_MUSIC_PLAYER << 16) + 0)

typedef struct pic_delete_flag
{
  int index;
  struct pic_delete_flag *p_next;
}pic_delete_flag_t;

static handle_t pic_handle = 0;
static handle_t pic_region = NULL;
static pic_flip_rotate_type_t pic_flip = PIC_NO_F_NO_R;
static pic_play_mode_t g_pic_play_mode = 0;
static mul_pic_param_t g_pic_param = {{0},};
static media_fav_t *g_pic_play_list_dir = NULL;
static media_fav_t *g_pic_play_list_fav = NULL;
static u16 g_pic_play_index_fav = 0;
static u16 g_pic_play_index_dir = 0;

static pic_delete_flag_t *g_pic_del = NULL;
static BOOL g_pic_modified = FALSE;

u16 ui_pic_evtmap(u32 event);

extern RET_CODE mul_pic_set_url(handle_t pic_handle, u8 *p_url);

void pic_api_callback(handle_t pic_handle, u32 content, u32 para1, u32 para2)
{
  event_t evt = {0};
  
  switch(content)
  {
    case MUL_PIC_EVT_DRAW_END:
      evt.id = PIC_API_EVT_DRAW_END;
      evt.data1 = para1;
      evt.data2 = para2;
      
      ap_frm_send_evt_to_ui(APP_PICTURE, &evt);
      break;
      case MUL_PIC_EVT_DRAW_START:
        evt.id = PIC_API_EVT_DRAW_START;
        evt.data1 = para1;
        evt.data2 = para2;
        ap_frm_send_evt_to_ui(APP_PICTURE, &evt);
        break;
    case MUL_PIC_EVT_SIZE_UPDATE:
      evt.id = PIC_API_EVT_SIZE_UPDATE;
      evt.data1 = para1;
      evt.data2 = para2;
      
      ap_frm_send_evt_to_ui(APP_PICTURE, &evt);    
      break;

    case MUL_PIC_EVT_TOO_LARGE:
      evt.id = PIC_API_EVT_TOO_LARGE;
      evt.data1 = para1;
      evt.data2 = para2;
      
      ap_frm_send_evt_to_ui(APP_PICTURE, &evt);        
      break;

    case MUL_PIC_EVT_UNSUPPORT_FMT:
      evt.id = PIC_API_EVT_UNSUPPORT;
      evt.data1 = para1;
      evt.data2 = para2;
      
      ap_frm_send_evt_to_ui(APP_PICTURE, &evt);        
      break;  
    
    case MUL_PIC_EVT_DATA_ERROR:
      evt.id = PIC_API_EVT_DATA_ERROR;
      evt.data1 = para1;
      evt.data2 = para2;
      
      ap_frm_send_evt_to_ui(APP_PICTURE, &evt);  
      break;
      
    default:
      break;
  }
}
#ifndef WIN32
static void ui_pic_region_init(u8 *p_buffer)
{
  rect_size_t rect_size = {0};
  point_t pos;  
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, 
SYS_DEV_TYPE_DISPLAY);
  pos.x = 0;
  pos.y = 0;
  rect_size.w = SCREEN_WIDTH;
  rect_size.h = SCREEN_HEIGHT;
  pic_region = region_create(&rect_size, PIX_FMT_ARGB8888);
  MT_ASSERT(NULL != pic_region);

  memset(p_buffer, 0, PICTURE_REGION_USED_SIZE);
  ret = disp_layer_add_region_ex(p_disp_dev, DISP_LAYER_ID_OSD0, pic_region, &pos,
		(void *)(((u32)p_buffer) | 0xa0000000),(void *)(((u32)(p_buffer +(PICTURE_REGION_USED_SIZE/2))) | 0xa0000000) );
  MT_ASSERT(SUCCESS == ret);

  region_show(pic_region, TRUE);

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_OSD0, TRUE);
}

static void ui_pic_region_release(void)
{
  RET_CODE ret = ERR_FAILURE;
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, 
SYS_DEV_TYPE_DISPLAY);

  if(NULL != pic_region)
  {
    ret = disp_layer_remove_region(p_disp_dev, DISP_LAYER_ID_OSD0, pic_region);
    MT_ASSERT(SUCCESS == ret);

    ret = region_delete(pic_region);
    MT_ASSERT(SUCCESS == ret);
    pic_region = NULL;

    disp_layer_show(p_disp_dev, DISP_LAYER_ID_OSD0, FALSE);
  }
}
#endif

handle_t ui_get_pic_region(void)
{
  return pic_region;
}

void ui_pic_init(pic_source_t src)
{
  mul_pic_chan_t chan = {0};
  u8 *p_buffer = NULL;
  u8 *p_region_buffer = NULL;
  u32 buffer_size = 0;
  RET_CODE ret = 0;

  OS_PRINTF("xxxxxxxxxxxxxx %s, %d\n", __FUNCTION__, __LINE__);
//  chan.task_pri = JPEG_CHAIN_PRIORITY;
  chan.task_stk_size = 32 * KBYTES;
  chan.anim_pri = JPEG_CHAIN_ANIM_TASK_PRIORITY;
  chan.anim_stk_size = 16 * KBYTES;
//#ifndef WIN32
  if (src == PIC_SOURCE_NET)
  {
    ret = mul_pic_create_net_chain(&pic_handle, &chan);
  }
  else if( src  == PIC_SOURCE_FILE )
  {
    ret = mul_pic_create_chain(&pic_handle, &chan);
  }
  else if( src == PIC_SOURCE_BUF )
  {
    ret = mul_pic_create_buffer_chain(&pic_handle, &chan);
  }

//#endif
  
  if(SUCCESS != ret)
  {
    pic_handle = 0;

    return;
  }

  OS_PRINTF("pic chain created, 0x%x\n", pic_handle);  

  memset(&g_pic_param, 0, sizeof(mul_pic_param_t));

  p_buffer = (u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_buffer != 0);

  p_buffer += MUSIC_MODULE_USED_SIZE;

  buffer_size = PICTURE_MODULE_USED_SIZE;

  mem_mgr_release_block(BLOCK_AV_BUFFER);
  
  p_region_buffer = p_buffer + PICTURE_MODULE_USED_SIZE;
#ifndef WIN32
  ui_pic_region_init(p_region_buffer);
#else
  pic_region = (void *)gdi_get_screen_handle(FALSE);
#endif
  mul_pic_set_rend_rgn(pic_handle, pic_region); 

  mul_pic_set_dec_mode(pic_handle, DEC_FRAME_MODE); 

  mul_pic_set_anim_param(pic_handle, 12, 4); 

  mul_pic_attach_buffer(pic_handle, p_buffer, buffer_size); 

  mul_pic_register_evt(pic_handle, pic_api_callback);
  
  OS_PRINTF("mul pic attach buffer: 0x%x, size : 0x%x\n", (u32)p_buffer, buffer_size);  

  fw_register_ap_evtmap(APP_PICTURE, ui_pic_evtmap);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PICTURE);   
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_ALBUMS); 
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_NETWORK_MUSIC);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PHOTO_SHOW); 
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_USB_PICTURE); 
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_NETWORK_PLAY);   
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_ONMOV_WEBSITES);   
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_ONMOV_DESCRIPTION);   
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_YOUTUBE);   
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_YOUPORN);   
//  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_FLICKR); 
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_REDTUBE);    
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_IPTV);   
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_IPTV_DESCRIPTION);   
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_NEWS_TITLE);  
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_NEWS_INFO); 
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_NETMEDIA);
  fw_register_ap_msghost(APP_PICTURE, ROOT_ID_SUBMENU_NM);
}

void ui_pic_release(void)
{
  OS_PRINTF("pic chain destoryed, 0x%x\n", pic_handle);

  if(pic_handle != 0)
  {
    mul_pic_detach_buffer(pic_handle); 
    
    mul_pic_destroy_chain(pic_handle);

    pic_handle = 0;
  }
#ifndef WIN32
  ui_pic_region_release();
#endif

  fw_unregister_ap_evtmap(APP_PICTURE);
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_USB_PICTURE);   
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_ALBUMS); 
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_NETWORK_MUSIC); 
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_PHOTO_SHOW); 
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_PICTURE); 
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_NETWORK_PLAY);   
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_ONMOV_WEBSITES);   
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_ONMOV_DESCRIPTION);   
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_YOUTUBE);   
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_YOUPORN);   
//  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_FLICKR);
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_REDTUBE); 
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_NEWS_TITLE);  
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_NEWS_INFO);  
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_IPTV);   
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_IPTV_DESCRIPTION);   
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_NETMEDIA);
  fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_SUBMENU_NM);
}

void pic_set_url(u8 *p_url)
{
  MT_ASSERT(p_url != NULL);

  OS_PRINTF("@@@p_url=%s\n", p_url);
  if (pic_handle != 0)
  {
    mul_pic_set_url(pic_handle, p_url);
  }
}
/*
void pic_set_cache_buf(u8* p_buf, u32 len)
{
  MT_ASSERT(p_buf != NULL);
  MT_ASSERT(len > 0);
  
  if (pic_handle != 0)
  {
    mul_pic_set_cache_buffer(pic_handle, p_buf, len);
  }
}
*/
void pic_start(mul_pic_param_t *p_pic_param)
{
  MT_ASSERT(p_pic_param != NULL);
  
  OS_PRINTF("pic start decoder, 0x%x\n", pic_handle);

  if(pic_handle != 0)
  {
    mul_pic_start(pic_handle, p_pic_param);
  }
}

void pic_stop(void)
{
  OS_PRINTF("pic stop decoder, 0x%x\n", pic_handle);

  if(pic_handle != 0)
  {
    mul_pic_stop(pic_handle);
  }
}

void pic_pause(void)
{
  //pic_stop();
  if(pic_handle != 0)
  {
     mul_pic_anim_pause(pic_handle);
  }
}

void pic_resume()
{
 // pic_reset_flip();

 // ui_pic_play_curn(pp_media, p_rect);
 // ui_pic_play_next(pp_media, p_rect);
 if(pic_handle != 0)
 {
    mul_pic_anim_resume(pic_handle);
 }
}

void pic_reset_flip(void)
{
  pic_flip = PIC_NO_F_NO_R;
}

void ui_pic_set_play_mode(pic_play_mode_t mode)
{
  g_pic_play_mode = mode;
}

pic_play_mode_t ui_pic_get_play_mode(void)
{
  return g_pic_play_mode;
}


static void flist_unload_fav_xxx(media_fav_t** pp_media_fav)
{
  media_fav_t *p = NULL;
  media_fav_t *p_bak = NULL;

  p = *pp_media_fav;
  while (p)
  {
    p_bak = p;
    p = p->p_next;
    mtos_free(p_bak);
    p_bak = NULL;
  }

  *pp_media_fav = NULL;
}
void ui_pic_unload_fav_list(void)
{
  flist_unload_fav_xxx(&g_pic_play_list_fav);
}

void ui_pic_get_fav_list(media_fav_t **pp_desmedia)
{
  *pp_desmedia = g_pic_play_list_fav;
}

BOOL ui_pic_set_play_index_dir_by_name(u16 *p_name)
{
  u8 index=0;
  media_fav_t *p_temp = g_pic_play_list_dir;
  while (p_temp)
  {
    if (uni_strcmp(p_name, p_temp->path) == 0)
    {
      ui_pic_set_play_index_dir(index);
      return TRUE;
    }

    index++;

    if (p_temp->p_next == NULL)
    {
      return FALSE;
    }
    p_temp = p_temp->p_next;
  }

  return FALSE;
}

BOOL ui_pic_build_play_list_indir(file_list_t *p_filelist)
{
  u32 i;
  flist_type_t file_type = FILE_TYPE_UNKNOW;
  media_fav_t *p_temp = NULL;

  while(g_pic_play_list_dir)
  {
    p_temp = g_pic_play_list_dir;
    g_pic_play_list_dir = g_pic_play_list_dir->p_next;
    mtos_free(p_temp);
  }

  g_pic_play_list_dir = p_temp = NULL;

  for(i=0; i<p_filelist->file_count; i++)
  {
    if(p_filelist->p_file[i].type == NOT_DIR_FILE)
    {
      file_type = flist_get_file_type(p_filelist->p_file[i].p_name);
      if((file_type == FILE_TYPE_JPG)
        || (file_type == FILE_TYPE_BMP)
        || (file_type == FILE_TYPE_GIF)
        || (file_type == FILE_TYPE_PNG))
      {
        if (p_temp)
        {
          p_temp->p_next = mtos_malloc(sizeof(media_fav_t));
          MT_ASSERT(p_temp->p_next != NULL);
          memset(p_temp->p_next, 0, sizeof(media_fav_t));
          p_temp = p_temp->p_next;
        }
        else
        {
          p_temp = mtos_malloc(sizeof(media_fav_t));
          MT_ASSERT(p_temp != NULL);
          memset(p_temp, 0, sizeof(media_fav_t));
          g_pic_play_list_dir = p_temp;
        }

        p_temp->p_next = NULL;
        memset(p_temp->path, 0, MAX_FILE_PATH * sizeof(u16));
        uni_strcpy(p_temp->path, p_filelist->p_file[i].name);
      }
    }
  }

  return TRUE;
}

void ui_pic_set_play_index_dir(u16 index)
{
  g_pic_play_index_dir = index;
}

void ui_pic_set_play_index_fav(u16 index)
{
  g_pic_play_index_fav = index;
}

BOOL ui_pic_set_play_index_fav_by_name(u16 *p_name)
{
  u8 index = 0;
  media_fav_t *p_temp = g_pic_play_list_fav;
  while (p_temp)
  {
    if (uni_strcmp(p_name, p_temp->path) == 0)
    {
      ui_pic_set_play_index_fav(index);
      return TRUE;
    }

    index++;

    if (p_temp->p_next == NULL)
    {
      return FALSE;
    }
    p_temp = p_temp->p_next;
  }

  return FALSE;
}

u8 ui_pic_get_play_count_of_fav()
{
  media_fav_t *p_media=NULL;
  u8 count=0;

  p_media = g_pic_play_list_fav;

  while(p_media)
  {
    count++;
    p_media = p_media->p_next;
  }

  return count;
}

u8 ui_pic_get_play_count_of_dir()
{
  media_fav_t *p_media=NULL;
  u8 count=0;

  p_media = g_pic_play_list_dir;

  while(p_media)
  {
    count++;
    p_media = p_media->p_next;
  }

  return count;
}

static BOOL check_picture_format_is_gif(u16 *filename)
{
    u16 picture_format[2][5] = {{0x2e, 0x47, 0x49, 0x46, 0x0}, {0x2e, 0x67, 0x69, 0x66, 0x0}};
    u16 len_filename = 0, len_format = 0, i = 0;
    s32 ret = 0;
	
    len_filename = (u16)uni_strlen(filename);
    len_format = (u16)uni_strlen(picture_format[0]);
    
    if(len_filename < len_format)
    {
	        return FALSE;
    }

    for(i = 0; i < 2; i++)
    {
        ret = uni_strcmp(&filename[len_filename - len_format], &picture_format[i][0]);
        if(ret == 0)
            break;
    }

    
    if(ret == 0)
    {

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*
BOOL ui_pic_play_by_mem(u8* addr, u32 len, rect_t *p_rect)
{
  MT_ASSERT(addr != NULL);
  MT_ASSERT(len > 0);
  MT_ASSERT(p_rect != NULL);

  g_pic_param.file_size = len;
  g_pic_param.is_file = FALSE;
  g_pic_param.anim = REND_ANIM_NONE;
  g_pic_param.style = REND_STYLE_CENTER;
  g_pic_param.flip = PIC_NO_F_NO_R;
  g_pic_param.buf_get = 0;
  copy_rect(&g_pic_param.win_rect, p_rect);

  pic_start(&g_pic_param);

  return TRUE;
}
*/

BOOL ui_pic_play_by_name(u16 *file_name, rect_t *p_rect)
{
  MT_ASSERT(file_name != NULL);
  MT_ASSERT(p_rect != NULL);

  memcpy((void*)g_pic_param.file_name, file_name, MAX_FILE_PATH * sizeof(u16));
  g_pic_param.file_size = file_list_get_file_size(file_name);
  g_pic_param.is_file = TRUE;
  g_pic_param.anim = REND_ANIM_NONE;
  g_pic_param.style = REND_STYLE_CENTER;
  g_pic_param.flip = pic_flip;
  copy_rect(&g_pic_param.win_rect, p_rect);

  pic_start(&g_pic_param);

  return TRUE;
}

BOOL ui_pic_play_by_url(u8 *p_url, rect_t *p_rect, u32 identify)
{
  MT_ASSERT(p_url != NULL);
  MT_ASSERT(p_rect != NULL);

  pic_set_url(p_url);

  g_pic_param.anim = REND_ANIM_NONE;
  g_pic_param.style = REND_STYLE_CENTER;
  g_pic_param.flip = pic_flip;
  g_pic_param.handle = identify;
  copy_rect(&g_pic_param.win_rect, p_rect);

  pic_start(&g_pic_param);

  return TRUE;
}

BOOL ui_pic_play_curn(media_fav_t **pp_media, rect_t *p_rect)
{
  pic_play_mode_t mode = PIC_MODE_NORMAL;


  MT_ASSERT(pp_media != NULL);
  MT_ASSERT(p_rect != NULL);
 
  mode = ui_pic_get_play_mode();

  switch(mode)
  {
  case PIC_MODE_NORMAL:
      *pp_media = flist_get_fav_by_index(g_pic_play_list_dir, g_pic_play_index_dir);
      break;

  case PIC_MODE_FAV:
      *pp_media = flist_get_fav_by_index(g_pic_play_list_fav, g_pic_play_index_fav);
      break;

    default:
      break;
  }

  memcpy((void*)g_pic_param.file_name, (*pp_media)->path, MAX_FILE_PATH * sizeof(u16));
  g_pic_param.file_size = file_list_get_file_size((*pp_media)->path);
  g_pic_param.is_file = TRUE;

  if(check_picture_format_is_gif(g_pic_param.file_name))
  {
    g_pic_param.anim = REND_ANIM_NONE;
  }
  else
  {
    if(pic_play_get_special_effect() == TRUE)
    {
      g_pic_param.anim= RAND(REND_ANIM_MAX);
    }
    else
    {
      g_pic_param.anim = REND_ANIM_NONE;
    }
  }

  g_pic_param.style = REND_STYLE_CENTER;
  g_pic_param.flip = pic_flip;
  copy_rect(&g_pic_param.win_rect, p_rect);

  pic_start(&g_pic_param);

  return TRUE;
}

BOOL ui_pic_play_prev(media_fav_t **pp_media, rect_t *p_rect)
{
  u8 count_fav=0;
  u8 count_dir=0;
  u8 mode = 0;

  MT_ASSERT(pp_media != NULL);
  MT_ASSERT(p_rect != NULL);  
 
  count_fav = ui_pic_get_play_count_of_fav();
  count_dir = ui_pic_get_play_count_of_dir();
  mode = ui_pic_get_play_mode();

  switch(mode)
  {
    case PIC_MODE_NORMAL:
      if(!pic_play_get_slide_repeat() && (g_pic_play_index_dir == 0))
      {
        return FALSE;
      }

      if(g_pic_play_index_dir == 0)
      {
        g_pic_play_index_dir = (count_dir - 1);
      }
      else
      {
        g_pic_play_index_dir--;
      }
      
      *pp_media = flist_get_fav_by_index(g_pic_play_list_dir, g_pic_play_index_dir);
      break;

    case PIC_MODE_FAV:
      if(!pic_play_get_slide_repeat() && (g_pic_play_index_fav == 0))
      {
        return FALSE;
      }

      if(g_pic_play_index_fav == 0)
      {
        g_pic_play_index_fav = (count_fav - 1);
      }
      else
      {
        g_pic_play_index_fav--;
      }
    
      *pp_media = flist_get_fav_by_index(g_pic_play_list_fav, g_pic_play_index_fav);
      break;

    default:
      break;
  }

  memcpy((void*)g_pic_param.file_name, (void*)(*pp_media)->path, MAX_FILE_PATH * sizeof(u16));
  g_pic_param.file_size = file_list_get_file_size((*pp_media)->path);
  g_pic_param.is_file = TRUE;
  g_pic_param.flip = pic_flip;
  
  if(check_picture_format_is_gif(g_pic_param.file_name))
  {
    g_pic_param.anim = REND_ANIM_NONE;
  }
  else
  {
    if(pic_play_get_special_effect() == TRUE)
    {
      g_pic_param.anim= RAND(REND_ANIM_MAX);
    }
    else
    {
      g_pic_param.anim = REND_ANIM_NONE;
    }
  }
    
  
  g_pic_param.style = REND_STYLE_CENTER;
  copy_rect(&g_pic_param.win_rect, p_rect);

  pic_start(&g_pic_param);

  return TRUE;
}

BOOL ui_pic_play_next(media_fav_t **pp_media, rect_t *p_rect)
{
  u8 count_fav=0;
  u8 count_dir=0;
  u8 mode = 0;

  MT_ASSERT(pp_media != NULL);
  MT_ASSERT(p_rect != NULL);  
 
  count_fav = ui_pic_get_play_count_of_fav();
  count_dir = ui_pic_get_play_count_of_dir();
  mode = ui_pic_get_play_mode();

  switch(mode)
  {
    case PIC_MODE_NORMAL:
      g_pic_play_index_dir++;
    
      if(!pic_play_get_slide_repeat() && (g_pic_play_index_dir >= count_dir))
      {
        return FALSE;
      }

      g_pic_play_index_dir %= count_dir;

      *pp_media = flist_get_fav_by_index(g_pic_play_list_dir, g_pic_play_index_dir);
      break;

    case PIC_MODE_FAV:
      g_pic_play_index_fav++;
    
      if(!pic_play_get_slide_repeat() && (g_pic_play_index_fav >= count_fav))      
      {
        return FALSE;
      }

      g_pic_play_index_fav %= count_fav;

      *pp_media = flist_get_fav_by_index(g_pic_play_list_fav, g_pic_play_index_fav);
      break;

    default:
      break;
  }

  memcpy(g_pic_param.file_name, (void*)(*pp_media)->path, MAX_FILE_PATH * sizeof(u16));
  g_pic_param.file_size = file_list_get_file_size((*pp_media)->path);
  g_pic_param.is_file = TRUE;
  g_pic_param.flip = pic_flip;

  if(check_picture_format_is_gif(g_pic_param.file_name))
  {
    g_pic_param.anim = REND_ANIM_NONE;
  }
  else
  {
    if(pic_play_get_special_effect() == TRUE)
    {
      g_pic_param.anim= RAND(REND_ANIM_MAX);
    }
    else
    {
      g_pic_param.anim = REND_ANIM_NONE;
    }
  }

  g_pic_param.style = REND_STYLE_CENTER;
  copy_rect(&g_pic_param.win_rect, p_rect);

  pic_start(&g_pic_param);

  return TRUE;
}

BOOL ui_pic_is_rotate(void)
{
  return (pic_flip != PIC_NO_F_NO_R) ? TRUE : FALSE;
}

BOOL ui_pic_play_clock(media_fav_t **pp_media, rect_t *p_rect, BOOL is_clock)
{
  u8 count_fav=0;
  u8 count_dir=0;
  u8 mode = 0;

  MT_ASSERT(pp_media != NULL);
  MT_ASSERT(p_rect != NULL);  

  count_fav = ui_pic_get_play_count_of_fav();
  count_dir = ui_pic_get_play_count_of_dir();
  mode = ui_pic_get_play_mode();

  switch(pic_flip)
  {
    case PIC_NO_F_NO_R:
      pic_flip = is_clock?PIC_R_270:PIC_R_90;
      break;

    case PIC_R_90:
      pic_flip = is_clock?PIC_NO_F_NO_R:PIC_R_180;
      break;

    case PIC_R_180:
      pic_flip = is_clock?PIC_R_90:PIC_R_270;
      break;

    case PIC_R_270:
      pic_flip = is_clock?PIC_R_180:PIC_NO_F_NO_R;
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  switch(mode)
  {
    case PIC_MODE_NORMAL:
      g_pic_play_index_dir %= count_dir;
      *pp_media = flist_get_fav_by_index(g_pic_play_list_dir, g_pic_play_index_dir);
      break;

    case PIC_MODE_FAV:
      g_pic_play_index_fav%= count_fav;
      *pp_media = flist_get_fav_by_index(g_pic_play_list_fav, g_pic_play_index_fav);
      break;

    default:
      break;
  }

  memcpy(g_pic_param.file_name, (void*)(*pp_media)->path, MAX_FILE_PATH * sizeof(u16));
  g_pic_param.file_size = file_list_get_file_size((*pp_media)->path);
  g_pic_param.is_file = TRUE;
  g_pic_param.flip = pic_flip;
  g_pic_param.anim = REND_ANIM_NONE;
  g_pic_param.style = REND_STYLE_CENTER;
  copy_rect(&g_pic_param.win_rect, p_rect);
  
  pic_start(&g_pic_param);

  return TRUE;
}

BOOL ui_pic_get_cur(media_fav_t **pp_media)
{
  pic_play_mode_t mode;
  BOOL bRet = TRUE;
  u8 cunt = 0;

  mode = ui_pic_get_play_mode();

  switch(mode)
  {
    case PIC_MODE_NORMAL:
      cunt = ui_pic_get_play_count_of_dir();
      if(g_pic_play_index_dir > cunt)
      {
         OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
         return FALSE;
      }
      *pp_media = flist_get_fav_by_index(g_pic_play_list_dir, g_pic_play_index_dir);
      break;

    case PIC_MODE_FAV:
      cunt = ui_pic_get_play_count_of_fav();
      if(g_pic_play_index_fav > cunt)
      {
         OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
         return FALSE;
      }
      *pp_media = flist_get_fav_by_index(g_pic_play_list_fav, g_pic_play_index_fav);
      break;

    default:
      bRet = FALSE;
      break;
  }

  return bRet;
}

void ui_pic_load_fav_list(u16 letter)
{
  media_fav_t* p;

  if( NULL != g_pic_play_list_fav )
  {
    flist_unload_fav_xxx(&g_pic_play_list_fav);
    g_pic_play_list_fav = NULL;
  }
  //OS_PRINTF("@@@@ui_pic_load_fav_list##, load fav list from letter ascii is  :%d@@@\n ",letter);
  flist_load_fav(letter, (void **)(&g_pic_play_list_fav), PIC_FAV);

  p = g_pic_play_list_fav;
  while(p)
  {
    OS_PRINTF("%s, pic fav path: %s, node addr: 0x%x, ticks = %d\n",
	__FUNCTION__, p->path, p, mtos_ticks_get());
    p = p->p_next;
  }

}

BOOL ui_pic_add_one_fav(u16 *p_name)
{
  media_fav_t* p;

  p = g_pic_play_list_fav;
  while(p)
  {
    if (!uni_strcmp(p_name, p->path))
    {
      OS_PRINTF("\n##debug: flist_add_fav_by_name [%s] exist!!\n", p_name);
      return FALSE; //already added to fav list
    }
	p = p->p_next;
  }

  flist_add_fav_by_name(p_name, (void **)(&g_pic_play_list_fav), PIC_FAV);

  return TRUE;
}

void ui_pic_del_one_fav(u16 index)
{
  media_fav_t *p_media = NULL;
  u16 i;

  p_media = g_pic_play_list_fav;

  for(i = 0; i < index && p_media != NULL; i++)
  {
    p_media = p_media->p_next;
  }

  if(p_media == NULL)
  {
    return;
  }

  flist_del_fav((void **)(&g_pic_play_list_fav), p_media);
}

void ui_pic_save_fav_list(u16 letter)
{
//	OS_PRINTF("@@@@##ui_pic_save_fav_list###, this letter ascii is %d@@@\n",letter);
  flist_save_fav(letter, g_pic_play_list_fav, PIC_FAV);
}

u32 ui_pic_get_fav_count(void)
{
  media_fav_t *p_media = NULL;
  u32 count=0;

  p_media = g_pic_play_list_fav;

  while(p_media)
  {
    count++;
   // OS_PRINTF("@@@ui_pic_get_fav_count, count == %d @@@@\n",count);
    p_media = p_media->p_next;
  }

  return count;
}

void ui_pic_undo_save_del(void)
{
  pic_delete_flag_t *p_temp = NULL;

  while(g_pic_del)
  {
    p_temp = g_pic_del;
    g_pic_del = g_pic_del->p_next;

    mtos_free(p_temp);
    p_temp = NULL;
  }
  g_pic_modified = FALSE;
}

void ui_pic_set_one_del(u16 index)
{
  pic_delete_flag_t *temp = NULL;
  pic_delete_flag_t *p_new = NULL;
  pic_delete_flag_t *temp2 = NULL;

  g_pic_modified = TRUE;

  if(g_pic_del != NULL)
  {
    temp = g_pic_del;

    if(g_pic_del->index == index)
    {
      if(g_pic_del->p_next == NULL)
      {
        mtos_free(g_pic_del);
        g_pic_del = NULL;
      }
      else
      {
        g_pic_del = g_pic_del->p_next;
        mtos_free(temp);
      }

      return;
    }
    else
    {
      temp = g_pic_del;
      temp2 = temp;
      temp = temp->p_next;

      while(temp != NULL)
      {
        //if the index is already del, then cancel the del
        if(temp->index == index)
        {
          temp2->p_next = temp->p_next;
          mtos_free(temp);
          return;
        }

        temp2 = temp;
        temp = temp->p_next;
      }
    }
  }

  if(g_pic_del == NULL)
  {
    g_pic_del = mtos_malloc(sizeof(pic_delete_flag_t));
    MT_ASSERT(g_pic_del != NULL);
    memset(g_pic_del, 0, sizeof(pic_delete_flag_t));
    g_pic_del->index = index;
    g_pic_del->p_next = NULL;
  }
  else
  {
    p_new = mtos_malloc(sizeof(pic_delete_flag_t));
    MT_ASSERT(p_new != NULL);
    memset(p_new, 0, sizeof(pic_delete_flag_t));
    p_new->index = index;
    p_new->p_next = NULL;

    temp = g_pic_del;

    //add in order big-->small index
    while(temp != NULL)
    {
      temp2 = temp->p_next;

      if(p_new->index >= temp->index)
      {
        if(temp != g_pic_del)
        {
          temp->p_next = p_new;
          p_new->p_next = temp2;
        }
        else
        {
          p_new->p_next = temp;
          g_pic_del = p_new;
        }
        break;
      }
      else if(temp->p_next == NULL)
      {
        temp->p_next = p_new;
        break;
      }

      temp = temp->p_next;
    }
  }
}

BOOL ui_pic_is_one_fav_del(u16 index)
{
  pic_delete_flag_t *temp = g_pic_del;
  BOOL ret_boo = FALSE;

  while(temp)
  {
    if(temp->index == index)
    {
      ret_boo = TRUE;
      break;
    }
    temp = temp->p_next;
  }

  return ret_boo;
}

void ui_pic_save_del(u8 letter)
{
  pic_delete_flag_t *p_temp = NULL;
  //OS_PRINTF("@@@@###save delete, partition letter ascii is:%d@@@@\n",letter);
  while(g_pic_del)
  {
    ui_pic_del_one_fav(g_pic_del->index);

    p_temp = g_pic_del;
    g_pic_del = g_pic_del->p_next;

    mtos_free(p_temp);
  }

  ui_pic_save_fav_list(letter);

  g_pic_modified = FALSE;
}

BOOL ui_pic_is_del_modified(void)
{
  return g_pic_modified;
}

media_fav_t *ui_pic_get_fav_media_by_index(u16 index)
{
  return flist_get_fav_by_index(g_pic_play_list_fav, index);
}

void ui_pic_clear(u32 color)
{
  OS_PRINTF("pic clear, 0x%x\n", color);

  if(pic_handle != 0)
  {
    mul_pic_clear(pic_handle, color);
  }
}

void ui_pic_clear_rect(rect_t *p_rect, u32 color)
{
  OS_PRINTF("pic clear rect, 0x%x\n", color);

  if(pic_handle != 0)
  {
    mul_pic_clear_rect(pic_handle, p_rect, color);
  }
}

void ui_pic_clear_all(u32 color)
{
  rect_t rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  
  OS_PRINTF("pic clear rect, 0x%x\n", color);

  if(pic_handle != 0)
  {
    mul_pic_clear_rect(pic_handle, &rect, color);
  }
}

void ui_pic_set_url(u8 *p_url)
{
  if(pic_handle != 0)
  {
    mul_pic_set_url(pic_handle, p_url);
  }
}


BEGIN_AP_EVTMAP(ui_pic_evtmap)
  CONVERT_EVENT(UI_MUSIC_EVT_PLAY_END, MSG_MUSIC_EVT_PLAY_END)
  CONVERT_EVENT(PIC_API_EVT_DRAW_END, MSG_PIC_EVT_DRAW_END)
  CONVERT_EVENT(PIC_API_EVT_DRAW_START, MSG_PIC_EVT_DRAW_START)
  CONVERT_EVENT(PIC_API_EVT_DATA_ERROR, MSG_PIC_EVT_DATA_ERROR)
  CONVERT_EVENT(PIC_API_EVT_SIZE_UPDATE, MSG_PIC_EVT_UPDATE_SIZE)
  CONVERT_EVENT(PIC_API_EVT_TOO_LARGE, MSG_PIC_EVT_TOO_LARGE)
  CONVERT_EVENT(PIC_API_EVT_UNSUPPORT, MSG_PIC_EVT_UNSUPPORT)
END_AP_EVTMAP(ui_pic_evtmap)




