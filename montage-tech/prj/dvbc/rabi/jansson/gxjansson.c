/*******************************************************************************
@brief: 本模块用于对json进行封装,使用key对结构成员赋值以及对
	结构成员的索引.模块自行分析key中表现出来的结构体层次,
	并根据得到的层次分配数据位置.本模块提供三种基本的数据
	类型,存储结构的最小成员,分别为:long long 的整形;double的浮点型
	以及字符串类型;(指针类型,bool类型用整形存储).
	模块存储数据成员时,并不一定顺序存储,模块以哈希表的方式
	存储各成员,以加快索引的速度.
	模块提供格式化的key输入,用于循环输入数组成员.
********************************************************************************/
#include <string.h>
#include "stdarg.h"
#include "gx_jansson.h"
#include "jansson.h"


typedef enum{
	OBJECT,
	ARRAY,

	OBJECT_MEMBER,
	ARRAY_MEMBER,
}item_type;

int gxjsonsub_parse_key(const char* keys,int *offset,item_type *type,char**subkey,int *index)
{
	int keylen,orglen;
	int i;
	static char _string[256];
	*index = 0;
	*type = (item_type)0xffff;
	orglen = strlen(keys);
	keys += *offset;

	//printf("keys : %s\n",keys);
	keylen = strlen(keys);
	for(i=0;i<keylen;i++){
		if(keys[i] == '.'){
			*type = OBJECT;
			_string[i] = 0;
			*offset += i+1;
			*subkey = _string;
			return orglen - *offset;
		}
		else if(keys[i] == '['){
			*type = ARRAY_MEMBER;
			_string[i] = 0;
			i++;
			while(keys[i] != ']' && i<keylen){
				*index *= 10;
				*index += (keys[i] - '0');
				i++;
			}
			if(keys[i+1] == '.'){
				i++;
				*type = ARRAY;
			}
			if(keys[i+1] == '['){
				*type = ARRAY;
			}
			*offset += i+1;
			*subkey = _string;
			return orglen - *offset;
		}else {
			_string[i] = keys[i];
		}
	}
	_string[i] = 0;
	*offset = i;
	*type = OBJECT_MEMBER;
	*subkey = _string;
	return 0;
}
/*{"name":"zhangsenyan",
  "address":{"city":["beijing","hangzhou","xiamen"],"street":"chaoyang road"}
  }*/

/*调用这些函数,将改变指向上一个json的指针,上一个索引值*/
int gxjsonsub_process_object(json_t** pre_json,int* pre_index,const char* cur_key,int cur_index)
{
	json_t* json_swap;
	json_t* json_new;

	/*如果上一级是object,查询是否存在(A.cur_key)*/
	if(json_is_object(*pre_json)){
		json_swap = json_object_get(*pre_json,cur_key);
		if(json_swap == NULL){
			json_new = json_object();/*new b*/
			json_object_set_new(*pre_json,cur_key,json_new);/*link b to A, named 'cur_key'*/
			*pre_json = json_new;	/*pre -> next*/
		}
		else{
			*pre_json = json_swap;
		}
	}
	/*如果上一级是数组,获取数组成员(A[index].B)*/
	else if(json_is_array(*pre_json)){
		json_swap = json_array_get( *pre_json,*pre_index);
		/*如果数组成员空,则增加一项object*/
		if(json_swap == NULL)
		{
			/*增加一项object用以存储当前级,这项对用户是透明的*/
			json_new = json_object();
			json_array_append_new(*pre_json,json_new);
			*pre_json = json_new;

		}
		else{
			*pre_json = json_swap;
		}
		if(!json_is_object(*pre_json)){
			printf("parse string type failure!");
			return -1;

			/*如果不存在,添加一项参数*/
			if((json_swap = json_object_get(*pre_json,cur_key)) == NULL){
				json_new = json_object();
				json_object_set_new(*pre_json,cur_key,json_new);
				*pre_json = json_new;
			}
			else{
				*pre_json = json_swap;
			}
			*pre_index = cur_index;
		}
	}
	return 0;
}
int gxjsonsub_process_array(json_t** pre_json,int* pre_index,const char* cur_key,int cur_index)
{
	json_t* json_swap;
	json_t* json_new;
	if(json_is_object(*pre_json)){
		/*不存在则在object下增加一个数组*/
		json_swap = json_object_get(*pre_json,cur_key);
		if(json_swap == NULL){
			json_new = json_array();
			json_object_set_new(*pre_json,cur_key,json_new);
			*pre_json= json_new;
		}
		else{
			*pre_json = json_swap;
		}
		*pre_index = cur_index;
	}
	else if(json_is_array(*pre_json)){
		json_swap = json_array_get( *pre_json,*pre_index);
		if(json_swap == NULL){
			/*[2][2]...,二维数组类型*/
			if(cur_key[0] == 0){
				json_new = json_array();
				json_array_append_new(*pre_json,json_new);
				*pre_json = json_new;
			}/*[].xx[].yy类型,需要增加一个object存储后续数组*/
			else{
				json_new = json_object();
				json_array_append_new(*pre_json,json_new);
				*pre_json = json_new;
				/*以object 才能存储有名字的数组*/
				json_new = json_array();
				json_object_set_new(*pre_json,cur_key,json_new);
				*pre_json = json_new;
			}
		}
		else
		{
			*pre_json = json_swap;
			/*[2][2]...,二维数组类型*/
			if(cur_key[0] == 0){
			}/*[].xx[].yy类型,需要增加一个object存储后续数组*/
			else{
				/*只有object 才能容纳有名字的数组成员*/
				if(!json_is_object(*pre_json)){
					printf("parse string type failure!");
					return -1;
				}
				/*判断是否存在*/
				json_swap = json_object_get(*pre_json,cur_key);
				if(json_swap == NULL){
					json_new = json_array();
					json_object_set_new(*pre_json,cur_key,json_new);
					*pre_json = json_new;
				}
				else{
					*pre_json = json_swap;
				}
			}
		}
		*pre_index = cur_index;
	}
	return 0;
}

int gxjsonsub_process_array_member(json_t** pre_json,int* pre_index,const char* cur_key,int cur_index)
{
	json_t* json_swap;
	json_t* json_new;
	/*如果上一级是object*/
	if(json_is_object(*pre_json)){
		/*没有找到名字相同的成员,则添加*/
		json_swap = json_object_get(*pre_json,cur_key);
		if(json_swap == NULL){
			json_new = json_array();
			json_object_set_new(*pre_json,cur_key,json_new);
			*pre_json = json_new;
		}
		else{
			*pre_json = json_swap;
		}
	}
	/*如果上一级是数组*/
	else if(json_is_array(*pre_json)){
		json_swap = json_array_get( *pre_json,*pre_index);
		if(json_swap == NULL){
			/*[2][2]...,二维数组类型*/
			if(cur_key[0] == 0){
				json_new = json_array();
				json_array_append_new(*pre_json,json_new);
				*pre_json = json_new;
			}/*[].xx[].yy类型,需要增加一个object存储后续数组*/
			else{
				json_new = json_object();
				json_array_append_new(*pre_json,json_new);
				*pre_json = json_new;
				json_new = json_array();
				json_object_set_new(*pre_json,cur_key,json_new);
				*pre_json = json_new;
			}
		}
		else
		{
			*pre_json = json_swap;
			/*[2][2]...,二维数组类型*/
			if(cur_key[0] == 0){
			}/*[].xx[].yy类型,需要增加一个object存储后续数组*/
			else{
				if(!json_is_object(*pre_json)){
					printf("parse string type failure!");
					return -1;
				}
				json_swap = json_object_get(*pre_json,cur_key);
				if(json_swap == NULL){
					json_new = json_array();
					json_object_set_new(*pre_json,cur_key,json_new);
					*pre_json = json_new;
				}
				else{
					*pre_json = json_swap;
				}
			}
		}
		*pre_index = cur_index;;
	}
	return 0;
}

int gxjsonsub_process_object_member(json_t** pre_json,int* pre_index,const char* cur_key,int cur_index)
{
	json_t* json_swap;
	json_t* json_new;

	if(json_is_array(*pre_json)){
		json_swap = json_array_get( *pre_json,*pre_index);
		/*如果数组成员空,则增加一项object*/
		if(json_swap == NULL)
		{
			/*增加一项object用以存储当前级,这项对用户是透明的*/
			json_new = json_object();
			json_array_append_new(*pre_json,json_new);
			*pre_json = json_new;

		}
		else{
			*pre_json = json_swap;
		}
		*pre_index = cur_index;
	}
	return 0;
}
int gxjsonsub_append_sub_item(GxJson_t container,const char* key,
		GxJson_t *last_item,char** last_key,int* last_index)
{
	//	int remain = 0;
	int offset = 0;
	char * subkey;
	int index_prv=0,index_swap=0;
	item_type type;
	json_t * json_prv = (json_t *)container;
	/*address.city[2].road*/
	/*address .? ->address object
	  city[?  ->city 2 array*/


	/*address.city[2].road[2].name*/
	while(1){
		/*找到一级关键字*/
		gxjsonsub_parse_key(key,&offset,&type,&subkey,&index_swap);
		switch(type){
			/*后面有'.'则为object*/
			case OBJECT:
				gxjsonsub_process_object(&json_prv,&index_prv,subkey,index_swap);
				break;
				/*如果有'[]',则为数组*/
			case ARRAY:
				gxjsonsub_process_array(&json_prv,&index_prv,subkey,index_swap);
				break;
				/*xxx[2] = */
			case ARRAY_MEMBER:
				gxjsonsub_process_array_member(&json_prv,&index_prv,subkey,index_swap);
				*last_key = subkey;
				*last_index = index_swap;
				*last_item = json_prv;
				return 0;
			case OBJECT_MEMBER:
				gxjsonsub_process_object_member(&json_prv,&index_prv,subkey,index_swap);
				*last_key = subkey;
				*last_item = json_prv;
				return 0;
			default:
				printf("base param named:%s\n",subkey);
				*last_key = subkey;
				*last_item = json_prv;
				return 0;
		}
	}
}


int gxjsonsub_get_sub_item(GxJson_t container,const char* key,
		GxJson_t *last_item,char** last_key,int* last_index)
{
	//	int remain = 0;
	int offset = 0;
	char * subkey;
	int index_swap=0;
	item_type type;
	json_t * cur_json = (json_t *)container;
	json_t * json_swap;
	/*address.city[2].road*/
	/*address .? ->address object
	  city[?  ->city 2 array*/


	/*address.city[2].road[2].name*/
	while(1){
		/*找到一级关键字*/
		gxjsonsub_parse_key(key,&offset,&type,&subkey,&index_swap);
		if(json_is_object(cur_json)){
			json_swap = json_object_get(cur_json,subkey);
			if(json_swap == NULL){
				printf("object item not exit, make sure have append to the container!\n");
				return -1;
			}
			cur_json = json_swap;
			/*数组成员是结构时,增加了一层透明的object*/
			if(type == ARRAY || type == ARRAY_MEMBER){
				if(!json_is_array(cur_json)){
					printf("object item not exit, make sure have append to the container!\n");
				}
				cur_json = json_array_get(cur_json,index_swap);
			}
		}
		else if(json_is_array(cur_json)){
			json_swap = json_array_get(cur_json,index_swap);
			if(json_swap == NULL){
				printf("object item not exit, make sure have append to the container!\n");
				return -1;
			}
			cur_json = json_swap;
		}
		else{
		}
		switch(type){
			/*后面有'.'则为object*/
			case OBJECT:
				break;
				/*如果有'[]',则为数组*/
			case ARRAY:
				break;
				/*xxx[2] = */
			case ARRAY_MEMBER:
				*last_key = subkey;
				*last_index = index_swap;
				*last_item = cur_json;
				return 0;
			case OBJECT_MEMBER:
				*last_key = subkey;
				*last_item = cur_json;
				return 0;
			default:
				printf("base param named:%s\n",subkey);
				*last_key = subkey;
				*last_item = cur_json;
				return 0;
		}
	}
}


int gxjsonsub_append_string(GxJson_t container,const char* key,const char*string)
{
	char * last_key;
	void * last_item;
	int index=0;
	const char *buff = NULL;

	buff = json_string_to_jsonstring(string);
	gxjsonsub_append_sub_item(container,key,&last_item,&last_key,&index);
	if(json_is_object((json_t *)last_item)){
		if(json_object_set_new((json_t *)last_item,last_key,json_string_nocheck(buff))<0)
			printf("append string err!");
	}
	else if(json_is_array((json_t *)last_item)){
		if(json_array_get((json_t *)last_item,index)){
			if(json_array_set_new((json_t *)last_item,index,json_string_nocheck(buff))<0)
				printf("append string err!");
		}
		else{
			printf("append string !");
			if(json_array_append_new((json_t *)last_item,json_string_nocheck(buff))<0)
				printf("append string err!");
		}
	}
	free((char*)buff);

	return 0;
}


const char* gxjsonsub_get_string(GxJson_t container,const char* key)
{
	char * last_key = NULL;
	const char* ret_string = NULL;
	void * last_item = NULL;
	int index=0;
	gxjsonsub_get_sub_item(container,key,&last_item,&last_key,&index);
	ret_string = json_string_value((json_t*)last_item);
	//    printf("GetString->%s:  %s\n",key,ret_string);
	return ret_string;
}



int gxjsonsub_append_real(GxJson_t container,const char* key,double val)
{
	char * last_key;
	void * last_item;
	int index=0;
	gxjsonsub_append_sub_item(container,key,&last_item,&last_key,&index);
	if(json_is_object((json_t *)last_item)){
		if(json_object_set_new((json_t *)last_item,last_key,json_real(val))<0)
			printf("append real err!");
	}
	else if(json_is_array((json_t *)last_item)){
		if(json_array_get((json_t *)last_item,index)){
			if(json_array_set_new((json_t *)last_item,index,json_real(val))<0)
				printf("append real err!");
		}
		else{
			printf("append real !");
			if(json_array_append_new((json_t *)last_item,json_real(val))<0)
				printf("append real err!");
		}
	}
	return 0;
}


double gxjsonsub_get_real(GxJson_t container,const char* key)
{
	char * last_key;
	void * last_item;
	double val;
	int index=0;
	gxjsonsub_get_sub_item(container,key,&last_item,&last_key,&index);
	val = json_real_value(last_item);
	//	printf("GetFloat->%s:  %f\n",key,val);
	return val;
}

int gxjsonsub_append_integer(GxJson_t container,const char* key,long long val)
{
	char * last_key;
	void * last_item;
	int index=0;
	gxjsonsub_append_sub_item(container,key,&last_item,&last_key,&index);
	if(json_is_object((json_t *)last_item)){
		if(json_object_set_new((json_t *)last_item,last_key,json_integer(val))<0)
			printf("append integer err!");
	}
	else if(json_is_array((json_t *)last_item)){
		if(json_array_get((json_t *)last_item,index)){
			if(json_array_set_new((json_t *)last_item,index,json_integer(val))<0)
				printf("append integer err!");
		}
		else{
			if(json_array_append_new((json_t *)last_item,json_integer(val))<0)
				printf("append integer err!");
		}
	}
	return 0;
}

long long  gxjsonsub_get_integer(GxJson_t container,const char* key)
{
	char * last_key;
	void * last_item;
	long long	val;
	int index=0;
	gxjsonsub_get_sub_item(container,key,&last_item,&last_key,&index);
	val = json_number_value(last_item);
	//	printf("GetInterger->%s:  %lld \n",key,val);
	return val;
}


#define   ___EXPORT_FUNCTIONS__
/**
 * @brief New a container to contain all member
 * @return pointer to container. NULL: failure
 */
GxJson_t  GxJson_New(void)
{
	json_t * pjson;
	pjson = json_object();
	return (GxJson_t )pjson;
}

/**
 * @brief append a string member to container
 * @param GxJson_t  container: pointer to container.
 * @param const char* key: key to bind the member
 * @param const char* string: the value
 * @return int <0,failure;>=o,success;
 */
int GxJson_SetString(GxJson_t container,const char* key,const char*string)
{
	return gxjsonsub_append_string(container,key,string);
}


/**
 * @brief get a string member from container
 * @param GxJson_t  container: pointer to container.
 * @param const char* key: key to find the member
 * @return char* if(NULL),failure;else,the string;
 */
const char* GxJson_GetString(GxJson_t container,const char* key)
{
	return gxjsonsub_get_string(container,key);
}

/**
 * @brief append a float member to container
 * @param GxJson_t  container: pointer to container.
 * @param const char* key: key to bind the member
 * @param double val: the value
 * @return int <0,failure;>=o,success;
 */
int GxJson_SetFloat(GxJson_t container,const char* key,double val)
{
	return gxjsonsub_append_real(container,key,val);
}


/**
 * @brief get a flot member from container
 * @param GxJson_t  container: pointer to container.
 * @param const char* key: key to find the member
 * @return double if(0.0) the value of float is 0.0 or failure;else,the float value;
 */
double GxJson_GetFloat(GxJson_t container,const char* key)
{
	return gxjsonsub_get_real(container,key);
}

/**
 * @brief append a integer member to container
 * @param GxJson_t  container: pointer to container.
 * @param const char* key: key to bind the member
 * @param long long val: the value
 * @return int <0,failure;>=o,success;
 */
int GxJson_SetInteger(GxJson_t container,const char* key,long long val)
{
	return gxjsonsub_append_integer(container,key,val);
}


/**
 * @brief get a integer member from container
 * @param GxJson_t  container: pointer to container.
 * @param const char* key: key to find the member
 * @return long long  if(0) the value of integer is 0 or failure;else,the integer value;
 */
long long  GxJson_GetInteger(GxJson_t container,const char* key)
{
	return gxjsonsub_get_integer(container,key);
}
/**
 * @brief append a string member to container with format key
 * @param GxJson_t  container: pointer to container.
 * @param const char*string: the value
 * @param const char* fmt ...: key to bind the member. i.e ("frends[%d].englishname",1)
 * @return int <0,failure;>=o,success;
 */
int GxJson_SetStringWithFmtKey(GxJson_t container,const char*string,const char* fmt,...)
{
	char buffer[MAX_JSON_KEY_LEN];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buffer,MAX_JSON_KEY_LEN,fmt,ap);
	return gxjsonsub_append_string(container,buffer,string);
}

/**
 * @brief get a string member from container
 * @param GxJson_t  container: pointer to container.
 * @param const char* fmt,...: format key to find the member  i.e ("frends[%d].englishname",1)
 * @return char* if(NULL),failure;else,the string;
 */
const char* GxJson_GetStringWithFmtKey(GxJson_t container,const char* fmt,...)
{
	char buffer[MAX_JSON_KEY_LEN];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buffer,MAX_JSON_KEY_LEN,fmt,ap);
	return gxjsonsub_get_string(container,buffer);
}

/**
 * @brief append a float member to container with format key
 * @param GxJson_t  container: pointer to container.
 * @param double val: the value
 * @param const char* fmt ...: key to bind the member. i.e ("frends[%d].englishname",1)
 * @return int <0,failure;>=o,success;
 */
int GxJson_SetFloatWithFmtKey(GxJson_t container,double val,const char* fmt,...)
{
	char buffer[MAX_JSON_KEY_LEN];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buffer,MAX_JSON_KEY_LEN,fmt,ap);
	return gxjsonsub_append_real(container,buffer,val);
}

/**
 * @brief get a float member from container
 * @param GxJson_t  container: pointer to container.
 * @param const char* fmt,...: format key to find the member  i.e ("frends[%d].englishname",1)
 * @return double  if(0.0) the value of float is 0.0 or failure;else,the float value;
 */
double GxJson_GetFloatWithFmtKey(GxJson_t container,const char* fmt,...)
{
	char buffer[MAX_JSON_KEY_LEN];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buffer,MAX_JSON_KEY_LEN,fmt,ap);
	return gxjsonsub_get_real(container,buffer);
}


/**
 * @brief append a integer member to container with format key
 * @param GxJson_t  container: pointer to container.
 * @param long long val: the value
 * @param const char* fmt ...: key to bind the member. i.e ("frends[%d].englishname",1)
 * @return int <0,failure;>=o,success;
 */
int GxJson_SetIntegerWithFmtKey(GxJson_t container,long long val,const char* fmt,...)
{
	char buffer[MAX_JSON_KEY_LEN];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buffer,MAX_JSON_KEY_LEN,fmt,ap);
	return gxjsonsub_append_integer(container,buffer,val);
}

/**
 * @brief get a integer member from container
 * @param GxJson_t  container: pointer to container.
 * @param const char* fmt,...: format key to find the member  i.e ("frends[%d].englishname",1)
 * @return long long  if(0) the value of integer is 0 or failure;else,the integer value;
 */
long long  GxJson_GetIntegerWithFmtKey(GxJson_t container,const char* fmt,...)
{
	char buffer[MAX_JSON_KEY_LEN];
	va_list ap;
	va_start(ap,fmt);
	vsnprintf(buffer,MAX_JSON_KEY_LEN,fmt,ap);
	return gxjsonsub_get_integer(container,buffer);
}


/**
 * @brief free the container and all item that appended to it will be free
 * @param GxJson_t  container: pointer to container.
 * @return int <0,failure;>=o,success;
 */
int GxJson_Free(GxJson_t  container)
{
	json_decref((json_t *)container);
	return 0;
}

/**
 * @brief  dump string,make the content of the container as a format string.
 * @param GxJson_t  container: pointer to container.
 * @return char* if(NULL),failure;else,the string;
 */
char* GxJson_DumpString(GxJson_t container)
{
	return json_dumps((json_t *)container,2);
}

/**
 * @brief  load string,change string to a json container.
 * @param const char* string: pointer to string;
 * @return GxJson_t  if(NULL),failure;else,the string;
 */
GxJson_t GxJson_LoadString(const char* string)
{
	json_error_t error;
	json_t *json;
	json = json_loads(string,2,&error);
	//	printf("%s,%s\n",error.source,error.text);
	return (GxJson_t )json;
}

/**
 * @brief  free string buffer
 * @param const char* string: pointer to string;
 * @return void;
 */
void GxJson_FreeString(char* string)
{
	return free(string);
}

#define __TEST_CODE__
/*
   type

   struct{
   char* name;
   struct{
   char* city;
   char* stree;
   char* postcode;

   }address;

   struct {
   char* mobile;
   char* home;
   int ppcall;
   }phone number;

   struct {
   char* eng;
   char* chn;
   }frends[];

   money[2][2];//[0][],rmb ,usd 存款.[1][] rmb,usd 欠款
   }


   var j={"name":"zhangsenyan","address":
   {"city":"hangzhou","street":" wen san Road ","postcode":300025},
   "phone number":
   {"mobile":"13958182972","home":"057188156088","ppcall":881882},
   "frends":
   [
   {"eng":"WHB","chn":"王海波"},
   {"eng":"SB","chn":"沈斌"},
   {"eng":"ZZG","chn":"朱志国"},
   {"eng":"ZFZ","chn":"赵方正"},
   ],
   "money":
   [[0,0],[99999,66666]]
   }


*/

int test1(void)
{
	GxJson_t  box;
	char *pstring;
	box = GxJson_New();
	if(box == NULL){
		printf("GxCore_Malloc failure!\n");
		exit(1);
	}



	GxJson_SetString(box,"phone number.mobile","13958182972");
	GxJson_SetString(box,"phone number.home","057188156088");
	GxJson_SetInteger(box,"phone number.ppcall",881882);

	GxJson_SetString(box,"address.postcode","310025");
	GxJson_SetString(box,"address.street","wen san road,文三 ");
	GxJson_SetString(box,"address.city","hanzhou");
	GxJson_SetString(box,"name","zhangsenyan");


	GxJson_SetString(box,"frends[0].eng","WHB");
	GxJson_SetString(box,"frends[0].chn","王海波");

	GxJson_SetString(box,"frends[1].eng","SB");
	GxJson_SetString(box,"frends[1].chn","沈斌");

	GxJson_SetString(box,"frends[2].eng","ZZG");
	GxJson_SetString(box,"frends[2].chn","朱志国");

	GxJson_SetString(box,"frends[3].eng","ZFZ");
	GxJson_SetString(box,"frends[3].chn","赵方正");


	GxJson_SetFloat(box,"money[0][0]",0.0);
	GxJson_SetFloat(box,"money[0][1]",0.0);

	GxJson_SetFloat(box,"money[1][0]",99999.0099);
	GxJson_SetFloat(box,"money[1][1]",66666.0066);

	pstring = GxJson_DumpString( box);
	GxJson_Free(box);

	printf("Result: %s\n",pstring);
	box = GxJson_LoadString(pstring);


	GxJson_GetString(box,"phone number.mobile");
	GxJson_GetString(box,"phone number.home");
	GxJson_GetInteger(box,"phone number.ppcall");

	GxJson_GetString(box,"address.postcode");
	GxJson_GetString(box,"address.street");
	GxJson_GetString(box,"address.city");
	GxJson_GetString(box,"name");


	GxJson_GetString(box,"frends[0].eng");
	GxJson_GetString(box,"frends[0].chn");

	GxJson_GetString(box,"frends[1].eng");
	GxJson_GetString(box,"frends[1].chn");

	GxJson_GetString(box,"frends[2].eng");
	GxJson_GetString(box,"frends[2].chn");

	GxJson_GetString(box,"frends[3].eng");
	GxJson_GetString(box,"frends[3].chn");


	GxJson_GetFloat(box,"money[0][0]");
	GxJson_GetFloat(box,"money[0][1]");

	GxJson_GetFloat(box,"money[1][0]");
	GxJson_GetFloat(box,"money[1][1]");

	GxJson_Free(box);
	return 0;
}
/*
   val
   struct{

   struct{
   char* flag;
   struct {
   char* flag;
   struct{
   char* flag;
   char* string;
   int	 val;
   }third[2];
   }second[2];
   }first[2];

   }

   3*2+
   */
int test1withfmtkey(void)
{
	GxJson_t  box;
	char *pstring;
	box = GxJson_New();
	if(box == NULL){
		printf("GxCore_Malloc failure!\n");
		exit(1);
	}



	GxJson_SetString(box,"phone number.mobile","13958182972");
	GxJson_SetString(box,"phone number.home","057188156088");
	GxJson_SetIntegerWithFmtKey(box,881882,"phone number.ppcall");

	GxJson_SetString(box,"address.postcode","310025");
	GxJson_SetString(box,"address.street","wen san road");
	GxJson_SetString(box,"address.city","hanzhou");
	GxJson_SetString(box,"name","zhangsenyan");


	GxJson_SetStringWithFmtKey(box,"WHB","frends[%d].eng",0);

	GxJson_SetStringWithFmtKey(box,"王海波","frends[%d].chn",0);

	GxJson_SetStringWithFmtKey(box,"SB","frends[%d].eng",1);
	GxJson_SetStringWithFmtKey(box,"沈斌","frends[%d].chn",1);

	GxJson_SetStringWithFmtKey(box,"ZZG","frends[%d].eng",2);
	GxJson_SetStringWithFmtKey(box,"朱志国","frends[%d].chn",2);

	GxJson_SetStringWithFmtKey(box,"ZFZ","frends[%d].eng",3);
	GxJson_SetStringWithFmtKey(box,"赵方正","frends[%d].chn",3);


	GxJson_SetFloatWithFmtKey(box,0.0,"money[%d][%d]",0,0);
	GxJson_SetFloatWithFmtKey(box,0.0,"money[%d][%d]",0,1);

	GxJson_SetFloatWithFmtKey(box,99999.0099,"money[%d][%d]",1,0);
	GxJson_SetFloatWithFmtKey(box,66666.0066,"money[%d][%d]",1,1);


	pstring = GxJson_DumpString( box);
	GxJson_Free(box);

	printf("Result: %s\n",pstring);
	box = GxJson_LoadString(pstring);


	GxJson_GetString(box,"phone number.mobile");
	GxJson_GetString(box,"phone number.home");
	GxJson_GetIntegerWithFmtKey(box,"phone number.ppcall");

	GxJson_GetString(box,"address.postcode");
	GxJson_GetString(box,"address.street");
	GxJson_GetString(box,"address.city");
	GxJson_GetString(box,"name");


	GxJson_GetStringWithFmtKey(box,"frends[%d].eng",0);
	GxJson_GetStringWithFmtKey(box,"frends[%d].chn",0);

	GxJson_GetStringWithFmtKey(box,"frends[%d].eng",1);
	GxJson_GetStringWithFmtKey(box,"frends[%d].chn",1);

	GxJson_GetStringWithFmtKey(box,"frends[%d].eng",2);
	GxJson_GetStringWithFmtKey(box,"frends[%d].chn",2);

	GxJson_GetStringWithFmtKey(box,"frends[%d].eng",3);
	GxJson_GetStringWithFmtKey(box,"frends[%d].chn",3);


	GxJson_GetFloatWithFmtKey(box,"money[%d][%d]",0,0);
	GxJson_GetFloatWithFmtKey(box,"money[%d][%d]",0,1);

	GxJson_GetFloatWithFmtKey(box,"money[%d][%d]",1,0);
	GxJson_GetFloatWithFmtKey(box,"money[%d][%d]",1,1);

	GxJson_Free(box);
	return 0;
}
/*
   val
   struct{

   struct{
   char* flag;
   struct {
   char* flag;
   struct{
   char* flag;
   char* string;
   int	 val;
   }third[2];
   }second[2];
   }first[2];
   }

   3*2+
   */
int test2(void)
{
	GxJson_t  box;
	char *pstring;
	box = GxJson_New();
	if(box == NULL){
		printf("GxCore_Malloc failure!\n");
		exit(1);
	}

	GxJson_SetString(box,"first[0].flag","first->0");
	GxJson_SetString(box,"first[0].second[0].flag","second->0");
	GxJson_SetString(box,"first[0].second[1].flag","second->1");
	GxJson_SetString(box,"first[0].second[0].thrid[0].flag","third->0");
	GxJson_SetString(box,"first[0].second[0].thrid[1].flag","third->1");
	GxJson_SetString(box,"first[0].second[1].thrid[0].flag","third->0");
	GxJson_SetString(box,"first[0].second[1].thrid[1].flag","third->1");
	GxJson_SetString(box,"first[0].second[0].thrid[0].string","first[0]second[0].thrid[0].string ->0");
	GxJson_SetString(box,"first[0].second[0].thrid[1].string","first[0]second[0].thrid[1].string ->1");
	GxJson_SetString(box,"first[0].second[1].thrid[0].string","first[0]second[1].thrid[0].string ->0");
	GxJson_SetString(box,"first[0].second[1].thrid[1].string","first[0]second[1].thrid[1].string ->1");
	GxJson_SetInteger(box,"first[0].second[0].thrid[0].val",0);
	GxJson_SetInteger(box,"first[0].second[0].thrid[1].val",1);
	GxJson_SetInteger(box,"first[0].second[1].thrid[0].val",2);
	GxJson_SetInteger(box,"first[0].second[1].thrid[1].val",3);

	GxJson_SetString(box,"first[1].flag","first->1");
	GxJson_SetString(box,"first[1].second[0].flag","second->0");
	GxJson_SetString(box,"first[1].second[1].flag","second->1");
	GxJson_SetString(box,"first[1].second[0].thrid[0].flag","third->0");
	GxJson_SetString(box,"first[1].second[0].thrid[1].flag","third->1");
	GxJson_SetString(box,"first[1].second[1].thrid[0].flag","third->0");
	GxJson_SetString(box,"first[1].second[1].thrid[1].flag","third->1");
	GxJson_SetString(box,"first[1].second[0].thrid[0].string","first[1]second[0].thrid[0].string ->0");
	GxJson_SetString(box,"first[1].second[0].thrid[1].string","first[1]second[0].thrid[1].string ->1");
	GxJson_SetString(box,"first[1].second[1].thrid[0].string=%s","first[1]second[1].thrid[0].string ->0");
	GxJson_SetString(box,"first[1].second[1].thrid[1].string","first[1]second[1].thrid[1].string ->1");


	GxJson_SetString(box,"first[1].second[1].thrid[0].flag","third->0");

	GxJson_SetInteger(box,"first[1].second[0].thrid[0].val",4);
	GxJson_SetInteger(box,"first[1].second[0].thrid[1].val",5);
	GxJson_SetInteger(box,"a",6);
	GxJson_SetInteger(box,"first[1].second[1].thrid[1].val",7);

	pstring = GxJson_DumpString( box);
	printf("Result: %s\n",pstring);
	GxJson_Free(box);

	box = GxJson_LoadString(pstring);


	GxJson_GetString(box,"first[0].flag");
	GxJson_GetString(box,"first[0].second[0].flag");
	GxJson_GetString(box,"first[0].second[1].flag");
	GxJson_GetString(box,"first[0].second[0].thrid[0].flag");
	GxJson_GetString(box,"first[0].second[0].thrid[1].flag");
	GxJson_GetString(box,"first[0].second[1].thrid[0].flag");
	GxJson_GetString(box,"first[0].second[1].thrid[1].flag");
	GxJson_GetString(box,"first[0].second[0].thrid[0].string");
	GxJson_GetString(box,"first[0].second[0].thrid[1].string");
	GxJson_GetString(box,"first[0].second[1].thrid[0].string");
	GxJson_GetString(box,"first[0].second[1].thrid[1].string");
	GxJson_GetInteger(box,"first[0].second[0].thrid[0].val");
	GxJson_GetInteger(box,"first[0].second[0].thrid[1].val");
	GxJson_GetInteger(box,"first[0].second[1].thrid[0].val");
	GxJson_GetInteger(box,"first[0].second[1].thrid[1].val");

	GxJson_GetString(box,"first[1].flag");
	GxJson_GetString(box,"first[1].second[0].flag");
	GxJson_GetString(box,"first[1].second[1].flag");
	GxJson_GetString(box,"first[1].second[0].thrid[0].flag");
	GxJson_GetString(box,"first[1].second[0].thrid[1].flag");
	GxJson_GetString(box,"first[1].second[1].thrid[0].flag");
	GxJson_GetString(box,"first[1].second[1].thrid[1].flag");
	GxJson_GetString(box,"first[1].second[0].thrid[0].string");
	GxJson_GetString(box,"first[1].second[0].thrid[1].string");
	GxJson_GetString(box,"first[1].second[1].thrid[0].string");
	GxJson_GetString(box,"first[1].second[1].thrid[1].string");
	GxJson_GetInteger(box,"first[1].second[0].thrid[0].val");
	GxJson_GetInteger(box,"first[1].second[0].thrid[1].val");
	GxJson_GetInteger(box,"first[1].second[1].thrid[0].val");
	GxJson_GetInteger(box,"first[1].second[1].thrid[1].val");

	GxJson_Free(box);
	return 0;
}



/*
   struct ippv{
   char* sp_id;
   char* service_id;
   char* event_id;
   char* product_id;
   int price_type;
   int price[2];

   int viewed_time;
   int payed;
   int balance;
   }
   */
int  test3(void)
{
	GxJson_t  box;
	char *pstring;
	box = GxJson_New();
	if(box == NULL){
		printf("GxCore_Malloc failure!\n");
		exit(1);
	}

	GxJson_SetString(box,"sp_id","SP0000001");
	GxJson_SetString(box,"servce","2012");
	GxJson_SetString(box,"event_id","1221");
	GxJson_SetString(box,"product_id","ZYYT001");
	GxJson_SetInteger(box,"price_type",0);
	GxJson_SetInteger(box,"price[0]",12);
	GxJson_SetInteger(box,"price[1]",21);
	GxJson_SetInteger(box,"viewed_time",60*10);
	GxJson_SetInteger(box,"payed",99999999);
	GxJson_SetInteger(box,"balance",-1);

	pstring = GxJson_DumpString( box);
	GxJson_Free(box);
	printf("Result: %s\n",pstring);

	box = GxJson_LoadString(pstring);
	GxJson_GetString(box,"sp_id");
	GxJson_GetString(box,"servce");
	GxJson_GetString(box,"event_id");
	GxJson_GetString(box,"product_id");
	GxJson_GetInteger(box,"price_type");
	GxJson_GetInteger(box,"price[0]");
	GxJson_GetInteger(box,"price[1]");
	GxJson_GetInteger(box,"viewed_time");
	GxJson_GetInteger(box,"payed");
	GxJson_GetInteger(box,"balance");
	GxJson_Free(box);
	return 0;

}
//int main(void)
//{
//	printf("hello 奸商\n");
//	test1();
//	test2();
//	test3();
//	return 0;
//}

