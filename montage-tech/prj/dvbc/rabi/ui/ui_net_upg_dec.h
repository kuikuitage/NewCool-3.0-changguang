/****************************************************************************

 ****************************************************************************/
#ifndef _UI_NET_UPG_DEC_H_
#define _UI_NET_UPG_DEC_H_

/*!
  net_upg_info_item_t
  */
typedef struct
{
/*!
  customer_id
  */
  u32 customer_id;
/*!
  serial_start
  */
  u32 serial_start;
/*!
  serial_end
  */
  u32 serial_end;
/*!
  hw
  */
  u32 hw;
/*!
  sw
  */
  u32 sw;
/*!
  url
  */
  u8 url[256];
/*!
  size
  */
  u32 size;
/*!
  name
  */
  u8 name[32];
}net_upg_info_item_t;

/*!
  net_upg_xml_info_t
  */
typedef struct
{
/*!
  cnt
  */
  u16 item_cnt;
  /*!
    customer_name
    */
  u8 customer_name[16];
/*!
  items
  */
  net_upg_info_item_t item[64];

}net_upg_info_t;

#ifdef __cplusplus
extern "C" {
#endif

BOOL ui_net_upg_info_parse(u8 *p_data, u32 customer_id, u32 serial_num, u32 software_version, net_upg_info_t *p_out);

#ifdef __cplusplus
}
#endif

#endif
