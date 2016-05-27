/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __SIMPLE_SCRIPT_API_H__
#define __SIMPLE_SCRIPT_API_H__
#ifdef __cplusplus
extern "C" {
#endif
#include "download_api.h"

char* load_scripts(char* input_url, char * function_script);
int init_script_engine();
void scripts_get_page_items(char* input_url, char* execute_scripts, void *para, int *count, int *total_count, int search_limit);
char* scripts_get_url(char* input_url, char * execute_scripts);
int  scripts_get_youtube_url(char* input_url, char * execute_scripts, u8 size, request_url_t * p_request_url);
void scripts_get_category(char* execute_scripts, void *para, int *count, int search_limit);
char *scripts_youtube_get_page_items(char* input_url, char* execute_scripts, void *para, int *count, int *total_count, int search_limit);
char *scripts_webyoutube_get_page_items(char* execute_scripts,int len,void *para);

#ifdef __cplusplus
typedef struct _tagScriptConfigItem
{
  char *key;
  int   value_number;
  int   value_len;
  char *value;
  struct list_head list;
} SCRIPT_CONFIG_ITEM;

class ScriptConfig
{
public:
  ScriptConfig();
  ~ScriptConfig();
  int GetConfigInt(const char*key, int value_index = 0);
  const char *GetConfig(const char*key, int value_index = 0);
  bool AddConfig(const char*key, const char*value);
private:
  SCRIPT_CONFIG_ITEM _config_item;
};
#endif

#ifdef __cplusplus
}
#endif

#endif
