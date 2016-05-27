#ifndef  __IQY_DATA_PROVIDER__
#define  __IQY_DATA_PROVIDER__
#include "iqiyiTV_C.h"
#include "IIPTVDataProvider.h"

#ifdef __cplusplus
extern "C" {
#endif 
int iqy_iptv_dp_init(IPTV_DP_HDL_T * hdl);
int iqy_iptv_dp_deinit(IPTV_DP_HDL_T * hdl);
VDO_ID_t *iqy_get_current_video_id(void);
int	iqy_get_current_channel_id(void);
char* iqy_get_current_plid(void);
int iqy_get_current_episode_num(void);

#ifdef __cplusplus
}
#endif 
#endif



