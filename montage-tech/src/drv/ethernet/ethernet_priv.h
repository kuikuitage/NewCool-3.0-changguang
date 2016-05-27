/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ETHERNET_PRIV_H_
#define __ETHERNET_PRIV_H_

/*!
  ethernet configuration
  */
typedef struct lld_ethernet
{
  /*!
    file descriptor for the new socket
    */
  s32 socketfd;
  /*!
    indicate whether dhcp is enabled
    */
  int is_dhcp;
  /*!
    indicate this is not a real ethernet device
    */
  int no_device;
  
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
  struct ip_addr primarydns;
  struct ip_addr alternatedns;

  u8 *p_send_buf;
  u8 *p_recv_buf;
  u16   recv_buf_len;
  void (*hw_send_buf)(void *p_lld, u8 *pbuf, u32 lens);  // use for hw send pbuf
  s32 (*is_send_buff_ready)(void *p_lld);  // use for hw send pbuf

  void (*stack_call_back)(void *p_lld); //hw recv data notify stack layer
  s32 (*eth_device_init)(void *p_lld, u8 *mac_addr);
  void (*link_stats_change)(int upordw);//link stats change
  int link_stats;
  void *p_drvsvc;
  void *p_stack;
  void *p_priv;
    
  /*! 
    link level hardware address of this interface 
  */
  u8 hwaddr[6];
  u8 active;
  u8 inital;
}lld_ethernet_t;


#endif //__ETHERNET_PRIV_H_
