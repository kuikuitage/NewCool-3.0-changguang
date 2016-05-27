/********************************************************************************************/
/********************************************************************************************/
#ifndef __LRC_FILTER_H_
#define __LRC_FILTER_H_



/*!
  lrc filter command define
  */
typedef enum tag_lrc_parse_filter_cmd
{
/*!
  set lrc show line count
  */
   LRC_CUNT_SET,
/*!
  set lrc seek time
  */
   LRC_SEEK,
}lrc_parse_filter_cmd_t;

/*!
  create lrc transfer filter instance
  filter ID:LRC_FILTER
  \return return the instance of lrc_filter_t
  */
ifilter_t * lrc_filter_create(void *p_para);


#endif 

