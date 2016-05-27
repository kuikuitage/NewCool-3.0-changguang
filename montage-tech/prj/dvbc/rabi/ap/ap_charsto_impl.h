/******************************************************************************/
/******************************************************************************/
#ifndef _AP_CHARSTO_IMPL_H_
#define _AP_CHARSTO_IMPL_H_

/*!
    The device id of GigaDevice
  */
#define FLASH_MF_GD     0xC8
/*!
    The device id of WINBOND
  */
#define FLASH_MF_WINB   0xEF

#define FLASH_MF_ISSI    0x9D

/*
  init ap charsto 
*/
void ap_charsto_init();

/*
  to set protect type by spi_prot_block_type_t in charsto.h
*/
RET_CODE ap_charsto_set_protect(spi_prot_block_type_t p_t);

/*
  get the flash current protect type
*/
spi_prot_block_type_t ap_charsto_get_protect();

/*
  to cancel protect 
*/
RET_CODE ap_charsto_cancel_protect(void);
#endif
