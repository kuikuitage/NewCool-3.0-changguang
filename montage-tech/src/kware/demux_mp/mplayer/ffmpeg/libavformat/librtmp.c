/*
 * RTMP network protocol
 * Copyright (c) 2010 Howard Chu
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file
 * RTMP protocol based on http://rtmpdump.mplayerhq.hu/ librtmp
 */

#include "libavutil/avstring.h"
#include "libavutil/mathematics.h"
#include "libavutil/opt.h"
#include "avformat.h"
#include "url.h"

#include <librtmp/lib_rtmp.h>
#include <librtmp/lib_log.h>
#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#else
#include "mp_func_trans.h"
#endif
typedef struct LibRTMPContext {
    const AVClass *class;
    RTMP rtmp;
    char *app;
    char *playpath;
} LibRTMPContext;
typedef struct LibRTMPArg {
    char *app;
    char *Playpath_r;
    char *playpath_y;
    char *swfUrl_s;
    char *pageUrl_p;
    char *swfUrl_W;
    int  live_v;
} LibRTMPArg;

static void rtmp_log(int level, const char *fmt, va_list args)
{
    switch (level) {
    default:
    case RTMP_LOGCRIT:    level = AV_LOG_FATAL;   break;
    case RTMP_LOGERROR:   level = AV_LOG_ERROR;   break;
    case RTMP_LOGWARNING: level = AV_LOG_WARNING; break;
    case RTMP_LOGINFO:    level = AV_LOG_INFO;    break;
    case RTMP_LOGDEBUG:   level = AV_LOG_VERBOSE; break;
    case RTMP_LOGDEBUG2:  level = AV_LOG_DEBUG;   break;
    }

    av_vlog(NULL, level, fmt, args);
    av_log(NULL, level, "\n");
}

static int rtmp_close(URLContext *s)
{
    LibRTMPContext *ctx = s->priv_data;
    RTMP *r = &ctx->rtmp;

    RTMP_Close(r);
    return 0;
}
static short decode_rtmp_type(char *url )
{
	short url_len = 0;
	short path_r_len = 0;
	short path_y_len = 0;
	char *p1 = NULL;
	char *p2 = NULL;
	char *path_r;
	char *space_str = NULL;
	space_str = strstr(url," ");
	if(space_str)
    {
    OS_PRINTF("\n%s %d\n",__func__,__LINE__);
        return 1;
       }
        else
            {
            OS_PRINTF("\n%s %d\n",__func__,__LINE__);
	return 0;
            }
}
static short RTMP_ARG_Parser(char *url ,LibRTMPArg * rtmp_arg)
{
char *suburl = NULL;

int i = 0;
memset(rtmp_arg,0,sizeof(LibRTMPArg));
OS_PRINTF("\n%s %d %s\n",__func__,__LINE__,url);
//
suburl=strstr(url," ");
if(NULL == suburl)
    return -1;
rtmp_arg->Playpath_r = malloc(strlen(url)-strlen(suburl)+1);
if(rtmp_arg->Playpath_r == NULL)
    return -1;
memset(rtmp_arg->Playpath_r,0,(strlen(url)-strlen(suburl))+1);
memcpy(rtmp_arg->Playpath_r,url,(strlen(url)-strlen(suburl)));

OS_PRINTF("\n%s %d Playpath_r:%s\n",__func__,__LINE__,rtmp_arg->Playpath_r);

//
suburl=strstr(url,"-v");
if(suburl == NULL)
    rtmp_arg->live_v = 0;
else
     rtmp_arg->live_v = 1;


OS_PRINTF("\n%s %d live_v:%d\n",__func__,__LINE__,rtmp_arg->live_v);
//-W
suburl=strstr(url,"-W ");

if(suburl)
{
char *suburl2 = NULL;
char *ulr_tmp = NULL;
int suburl2_len =0;
suburl = suburl +strlen("-W ");
suburl2 = strstr(suburl," ");
if(suburl2)
  suburl2_len = strlen(suburl2);
ulr_tmp= malloc(strlen(suburl)-suburl2_len+1);
if(ulr_tmp == NULL)
    return -1;
memset(ulr_tmp,0,(strlen(suburl)-suburl2_len+1));
memcpy(ulr_tmp,suburl,(strlen(suburl)-suburl2_len));
rtmp_arg->swfUrl_W = ulr_tmp;
OS_PRINTF("\n%s %d swfUrl_W:%s\n",__func__,__LINE__,rtmp_arg->swfUrl_W);
}
//-s
suburl=strstr(url,"-s ");

if(suburl)
{
char *suburl2 = NULL;
char *ulr_tmp = NULL;
int suburl2_len =0;
suburl = suburl +strlen("-s ");
suburl2 = strstr(suburl," ");
if(suburl2)
  suburl2_len = strlen(suburl2);
ulr_tmp= malloc(strlen(suburl)-suburl2_len+1);
if(ulr_tmp == NULL)
    return -1;
memset(ulr_tmp,0,(strlen(suburl)-suburl2_len+1));
memcpy(ulr_tmp,suburl,(strlen(suburl)-suburl2_len));
rtmp_arg->swfUrl_s = ulr_tmp;
OS_PRINTF("\n%s %d swfUrl_s:%s\n",__func__,__LINE__,rtmp_arg->swfUrl_s);
}

//-s
suburl=strstr(url,"-p ");

if(suburl)
{
char *suburl2 = NULL;
char *ulr_tmp = NULL;
int suburl2_len =0;
suburl = suburl +strlen("-p ");
suburl2 = strstr(suburl," ");
if(suburl2)
  suburl2_len = strlen(suburl2);
ulr_tmp= malloc(strlen(suburl)-suburl2_len+1);
if(ulr_tmp == NULL)
    return -1;
memset(ulr_tmp,0,(strlen(suburl)-suburl2_len+1));
memcpy(ulr_tmp,suburl,(strlen(suburl)-suburl2_len));
rtmp_arg->pageUrl_p = ulr_tmp;
OS_PRINTF("\n%s %d pageUrl_p:%s\n",__func__,__LINE__,rtmp_arg->pageUrl_p);
}
//-y
suburl=strstr(url,"-y ");

if(suburl)
{
char *suburl2 = NULL;
char *ulr_tmp = NULL;
int suburl2_len =0;
suburl = suburl +strlen("-y ");
suburl2 = strstr(suburl," ");
if(suburl2)
  suburl2_len = strlen(suburl2);
ulr_tmp= malloc(strlen(suburl)-suburl2_len+1);
if(ulr_tmp == NULL)
    return -1;
memset(ulr_tmp,0,(strlen(suburl)-suburl2_len+1));
memcpy(ulr_tmp,suburl,(strlen(suburl)-suburl2_len));
rtmp_arg->playpath_y = ulr_tmp;
OS_PRINTF("\n%s %d playpath_y:%s\n",__func__,__LINE__,rtmp_arg->playpath_y);
}

//-y
suburl=strstr(url,"-a ");

if(suburl)
{
char *suburl2 = NULL;
char *ulr_tmp = NULL;
int suburl2_len =0;
suburl = suburl +strlen("-a ");
suburl2 = strstr(suburl," ");
if(suburl2)
  suburl2_len = strlen(suburl2);
ulr_tmp= malloc(strlen(suburl)-suburl2_len+1);
if(ulr_tmp == NULL)
    return -1;
memset(ulr_tmp,0,(strlen(suburl)-suburl2_len+1));
memcpy(ulr_tmp,suburl,(strlen(suburl)-suburl2_len));
rtmp_arg->app = ulr_tmp;
OS_PRINTF("\n%s %d app:%s\n",__func__,__LINE__,rtmp_arg->playpath_y);
}
//while(1)
{
}
return 0;
}
static short parse_rtmp_type(char *url, char **p_path_r, char **p_path_y)
{
	short rtmp_type = 0;
	short url_len = 0;
	short path_r_len = 0;
	short path_y_len = 0;
	char *p1 = NULL;
	char *p2 = NULL;
	char *path_r;
	char *path_y;
	
	if(url){
		url_len = strlen(url);
		p1 = strstr(url,"rtmpe://");
		p2 = strstr(url,"-y/");
		
		if(p1 && p2){
			path_r_len = p2 - p1;
			if(path_r_len > 0){
				path_r = (char *)mtos_malloc(path_r_len+1);
				memset(path_r,0,path_r_len+1);
				memcpy(path_r,p1,path_r_len);
				OS_PRINTF("[%s] -- path_r: %s\n",__func__,path_r);
				*p_path_r = path_r;

				path_y_len = url_len - path_r_len-3;
				if(path_y_len > 0){

					path_y = (char *)mtos_malloc(path_y_len+1);
					memset(path_y,0,path_y_len+1);
					memcpy(path_y,p2+3,path_y_len);
					OS_PRINTF("[%s] -- path_y: %s\n",__func__,path_y);
					*p_path_y = path_y;
					
					rtmp_type = 1;
				}
			}	
		}	
	}
	return rtmp_type;
}
static short RTMP_ARG_RUN(RTMP *rtmp,LibRTMPArg * rtmp_arg)
    
{
AVal hostname = { 0, 0 };
  AVal playpath = { 0, 0 };
  AVal subscribepath = { 0, 0 };
  AVal usherToken = { 0, 0 }; //Justin.tv auth token
  int port = -1;
  int protocol = RTMP_PROTOCOL_UNDEFINED;
  int retries = 0;
  int bLiveStream = FALSE;	// is it a live stream? then we can't seek/resume
  int bRealtimeStream = FALSE;  // If true, disable the BUFX hack (be patient)
  int bHashes = FALSE;		// display byte counters not hashes by default

  long int timeout = 30;	// timeout connection after 120 seconds
AVal fullUrl = { 0, 0 };
  AVal swfUrl = { 0, 0 };
  AVal tcUrl = { 0, 0 };
  AVal pageUrl = { 0, 0 };
  AVal app = { 0, 0 };
  AVal auth = { 0, 0 };
  AVal swfHash = { 0, 0 };
  uint32_t swfSize = 0;
  AVal flashVer = { 0, 0 };
  AVal sockshost = { 0, 0 };
  uint32_t dSeek = 0;		// seek position in resume mode, 0 otherwise
  uint32_t dStopOffset = 0;
   int swfVfy = 0;
   int swfAge = 30;
  AVal parsedHost, parsedApp, parsedPlaypath;
	    unsigned int parsedPort = 0;
	    int parsedProtocol = RTMP_PROTOCOL_UNDEFINED;
   
   unsigned char hash[RTMP_SWF_HASHLEN];
    OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
   if(rtmp_arg->live_v== 1)
   bLiveStream = TRUE;
   if(rtmp_arg->swfUrl_W)
    {
     swfVfy = 1;
     swfUrl.av_val = rtmp_arg->swfUrl_W;
     swfUrl.av_len = strlen(swfUrl.av_val);
    }
 if(rtmp_arg->swfUrl_s)
    {
  swfUrl.av_val = rtmp_arg->swfUrl_s;
     swfUrl.av_len = strlen(swfUrl.av_val);
    }
  if(rtmp_arg->pageUrl_p)
    {
  pageUrl.av_val = rtmp_arg->pageUrl_p;
     pageUrl.av_len = strlen(pageUrl.av_val);
    }
   if(rtmp_arg->playpath_y)
    {
  playpath.av_val = rtmp_arg->playpath_y;
     playpath.av_len = strlen(playpath.av_val);
    }
   if(rtmp_arg->app)
    {
  app.av_val = rtmp_arg->app;
     app.av_len = strlen(app.av_val);
    }
  #if 0
   if (swfVfy)
    {
      if (RTMP_HashSWF(swfUrl.av_val, &swfSize, hash, swfAge) == 0)
        {
          swfHash.av_val = (char *)hash;
          swfHash.av_len = RTMP_SWF_HASHLEN;
        }
    }
#endif
  if (swfHash.av_len == 0 && swfSize > 0)
    {
    
      swfSize = 0;
    }

  if (swfHash.av_len != 0 && swfSize == 0)
    {
  
      swfHash.av_len = 0;
      swfHash.av_val = NULL;
    }
// OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
 OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
 if (!RTMP_ParseURL
		(rtmp_arg->Playpath_r, &parsedProtocol, &parsedHost, &parsedPort,
		 &parsedPlaypath, &parsedApp))
	      {
 OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
	      }
	    else
	      {
		if (!hostname.av_len)
		  hostname = parsedHost;
		if (port == -1)
		  port = parsedPort;
		if (playpath.av_len == 0 && parsedPlaypath.av_len)
		  {
		    playpath = parsedPlaypath;
		  }
		if (protocol == RTMP_PROTOCOL_UNDEFINED)
		  protocol = parsedProtocol;
		if (app.av_len == 0 && parsedApp.av_len)
		  {
		    app = parsedApp;
		  }
	      }
	        if (tcUrl.av_len == 0)
    {
		
	  tcUrl.av_len = strlen(RTMPProtocolStringsLower[protocol]) +
	  	hostname.av_len + app.av_len + sizeof("://:65535/");
      tcUrl.av_val = (char *) av_malloc(tcUrl.av_len);
	
	 OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
      tcUrl.av_len = snprintf(tcUrl.av_val, tcUrl.av_len, "%s://%.*s:%d/%.*s",
	  	   RTMPProtocolStringsLower[protocol], hostname.av_len,
		   hostname.av_val, port, app.av_len, app.av_val);
      OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
    }

//
  
  if (!fullUrl.av_len)
    {
    OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
      RTMP_SetupStream(rtmp, protocol, &hostname, port, &sockshost, &playpath,
		       &tcUrl, &swfUrl, &pageUrl, &app, &auth, &swfHash, swfSize,
		       &flashVer, &subscribepath, &usherToken, dSeek, dStopOffset, bLiveStream, timeout);


     // RTMP_SetupStream(r, protocol, &hostname, port, &sockshost, &playpath,
		//       &tcUrl, NULL, NULL, &app, NULL, NULL, 0,
		  //     NULL, NULL, NULL, 0, 0, 0, timeout);
    }
  else
    {
    OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
      if (RTMP_SetupURL(rtmp, fullUrl.av_val) == FALSE)
        {
          RTMP_Log(RTMP_LOGERROR, "Couldn't parse URL: %s", fullUrl.av_val);
          return -1;
	}
    }
return 0;

}

/**
 * Open RTMP connection and verify that the stream can be played.
 *
 * URL syntax: rtmp://server[:port][/app][/playpath][ keyword=value]...
 *             where 'app' is first one or two directories in the path
 *             (e.g. /ondemand/, /flash/live/, etc.)
 *             and 'playpath' is a file name (the rest of the path,
 *             may be prefixed with "mp4:")
 *
 *             Additional RTMP library options may be appended as
 *             space-separated key-value pairs.
 */
#define STR2AVAL(av,str)	av.av_val = str; av.av_len = strlen(av.av_val)
static int rtmp_open(URLContext *s, const char *uri, int flags)
{
	LibRTMPContext *ctx = s->priv_data;
	RTMP *r = &ctx->rtmp;
	int rc = 0, level;
	char *filename = s->filename;
	AVal playpath = { 0, 0 };
	AVal hostname = { 0, 0 };
	int port = -1;
	AVal sockshost = { 0, 0 };
	AVal tcUrl = { 0, 0 };
	long int timeout = 30;	
	int protocol = RTMP_PROTOCOL_UNDEFINED;
	AVal app = { 0, 0 };
	AVal parsedHost, parsedApp, parsedPlaypath;
	unsigned int parsedPort = 0;
	int parsedProtocol = RTMP_PROTOCOL_UNDEFINED;

	short rtmp_type = 0;//0: normal rtmp, 1:rtmpe, need parsing
	char *shahidr = NULL;
	char *shahidy = NULL;
    //char shahidr[]={"rtmpe://mbc2.csl.delvenetworks.com:1935/a6344/d1/"};
    //char shahidy[]={"mp4:media/2fda1d3fd7ab453cad983544e8ed70e4/97077312a6ac4572892a4bfdba75b9a3-098cac003c11481db1ae72e974a7244f/style_s2012_e59_vod.mp4" };
    //OS_PRINTF("\n%s %d %s\n",__func__,__LINE__,uri);
      rtmp_type =  decode_rtmp_type(s->filename);
    //rtmp_type = parse_rtmp_type(s->filename, &shahidr, &shahidy);
    OS_PRINTF("[%s] -- rtmp_type: %d, shahidr[0x%x], shahidy[0x%x]\n",__func__,rtmp_type,shahidr,shahidy);
    
    
    switch (av_log_get_level()) {
    default:
    case AV_LOG_FATAL:   level = RTMP_LOGCRIT;    break;
    case AV_LOG_ERROR:   level = RTMP_LOGERROR;   break;
    case AV_LOG_WARNING: level = RTMP_LOGWARNING; break;
    case AV_LOG_INFO:    level = RTMP_LOGINFO;    break;
    case AV_LOG_VERBOSE: level = RTMP_LOGDEBUG;   break;
    case AV_LOG_DEBUG:   level = RTMP_LOGDEBUG2;  break;
    }
    RTMP_LogSetLevel(level);
    RTMP_LogSetCallback(rtmp_log);

    if (ctx->app || ctx->playpath) {
        int len = strlen(s->filename) + 1;
        if (ctx->app)      len += strlen(ctx->app)      + sizeof(" app=");
        if (ctx->playpath) len += strlen(ctx->playpath) + sizeof(" playpath=");

        if (!(filename = av_malloc(len)))
            return AVERROR(ENOMEM);

        av_strlcpy(filename, s->filename, len);
        if (ctx->app) {
            av_strlcat(filename, " app=", len);
            av_strlcat(filename, ctx->app, len);
        }
        if (ctx->playpath) {
            av_strlcat(filename, " playpath=", len);
            av_strlcat(filename, ctx->playpath, len);
        }
    }

       RTMP_Init(r);
    OS_PRINTF("[%s] -- called RTMP_Init()\n",__func__);   
    if(rtmp_type==0){
	    if (!RTMP_SetupURL(r, s->filename)) {
	        rc = -1;
	        goto fail;
	    }
    }
   else{
LibRTMPArg *rtmp_arg =malloc(sizeof(LibRTMPArg)) ;
if(NULL == rtmp_arg)
goto fail;
    RTMP_ARG_Parser(s->filename,rtmp_arg);
   OS_PRINTF("[%s] -- else\n",__func__); 
   RTMP_ARG_RUN(r,rtmp_arg);
  //arg free
  if(rtmp_arg)
    {
    if(rtmp_arg->app)
        free(rtmp_arg->app);
    if(rtmp_arg->Playpath_r)
        free(rtmp_arg->Playpath_r);
    if(rtmp_arg->playpath_y)
        free(rtmp_arg->playpath_y);
    if(rtmp_arg->swfUrl_s)
        free(rtmp_arg->swfUrl_s);
    if(rtmp_arg->pageUrl_p)
        free(rtmp_arg->pageUrl_p);
    if(rtmp_arg->swfUrl_W)
        free(rtmp_arg->swfUrl_W);
   free(rtmp_arg); 
    }

  //
   #if 0
    STR2AVAL(playpath, shahidy);
    OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
     if (!RTMP_ParseURL
		(rtmp_arg->Playpath_r, &parsedProtocol, &parsedHost, &parsedPort,
		 &parsedPlaypath, &parsedApp))
	      {
 OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
	      }
	    else
	      {
		if (!hostname.av_len)
		  hostname = parsedHost;
		if (port == -1)
		  port = parsedPort;
		if (playpath.av_len == 0 && parsedPlaypath.av_len)
		  {
		    playpath = parsedPlaypath;
		  }
		if (protocol == RTMP_PROTOCOL_UNDEFINED)
		  protocol = parsedProtocol;
		if (app.av_len == 0 && parsedApp.av_len)
		  {
		    app = parsedApp;
		  }
	      }
	        if (tcUrl.av_len == 0)
    {
		
	  tcUrl.av_len = strlen(RTMPProtocolStringsLower[protocol]) +
	  	hostname.av_len + app.av_len + sizeof("://:65535/");
      tcUrl.av_val = (char *) av_malloc(tcUrl.av_len);
	
	 OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
      tcUrl.av_len = snprintf(tcUrl.av_val, tcUrl.av_len, "%s://%.*s:%d/%.*s",
	  	   RTMPProtocolStringsLower[protocol], hostname.av_len,
		   hostname.av_val, port, app.av_len, app.av_val);
      OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
    }

		            RTMP_SetupStream(r, protocol, &hostname, port, &sockshost, &playpath,
		       &tcUrl, NULL, NULL, &app, NULL, NULL, 0,
		       NULL, NULL, NULL, 0, 0, 0, timeout);
                     OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
                     free(rtmp_arg);
                     #endif
}

    if (flags & AVIO_FLAG_WRITE)
        RTMP_EnableWrite(r); 

        if (is_file_seq_exit()) {
            OS_PRINTF("\n[%s]  %d! \n", __func__, __LINE__);
             rc = AVERROR_UNKNOWN;
            goto fail;
        }

    if (!RTMP_Connect(r, NULL) || !RTMP_ConnectStream(r, 0)) {
        rc = AVERROR_UNKNOWN;
        goto fail;
    }

    s->is_streamed = 1;
    rc = 0;
fail:
    if (filename != s->filename)
        av_freep(&filename);
    if(shahidr){
    	mtos_free(shahidr);
    	shahidr = NULL;
    	}
    if(shahidy){
    	mtos_free(shahidy);
    	shahidy = NULL;
    	}
    return rc;
}

static int rtmp_write(URLContext *s, const uint8_t *buf, int size)
{
    LibRTMPContext *ctx = s->priv_data;
    RTMP *r = &ctx->rtmp;

    return RTMP_Write(r, buf, size);
}

static int rtmp_read(URLContext *s, uint8_t *buf, int size)
{
    LibRTMPContext *ctx = s->priv_data;
    RTMP *r = &ctx->rtmp;
    int ret = 0;

        if (is_file_seq_exit()) {
            OS_PRINTF("\n[%s]  %d! \n", __func__, __LINE__);
            
           return -1;
        }

ret = RTMP_Read(r, buf, size);
}

static int rtmp_read_pause(URLContext *s, int pause)
{
    LibRTMPContext *ctx = s->priv_data;
    RTMP *r = &ctx->rtmp;

    if (!RTMP_Pause(r, pause))
        return AVERROR_UNKNOWN;
    return 0;
}

static int64_t rtmp_read_seek(URLContext *s, int stream_index,
                              int64_t timestamp, int flags)
{
    LibRTMPContext *ctx = s->priv_data;
    RTMP *r = &ctx->rtmp;

    if (flags & AVSEEK_FLAG_BYTE)
        return AVERROR(ENOSYS);

    /* seeks are in milliseconds */
    if (stream_index < 0)
        timestamp = av_rescale_rnd(timestamp, 1000, AV_TIME_BASE,
            flags & AVSEEK_FLAG_BACKWARD ? AV_ROUND_DOWN : AV_ROUND_UP);

    if (!RTMP_SendSeek(r, timestamp))
        return AVERROR_UNKNOWN;
    return timestamp;
}

static int rtmp_get_file_handle(URLContext *s)
{
    LibRTMPContext *ctx = s->priv_data;
    RTMP *r = &ctx->rtmp;

    return RTMP_Socket(r);
}

#define OFFSET(x) offsetof(LibRTMPContext, x)
#define DEC AV_OPT_FLAG_DECODING_PARAM
#define ENC AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    {"rtmp_app",      "Name of application to connect to on the RTMP server", OFFSET(app),      AV_OPT_TYPE_STRING, {.str = NULL }, 0, 0, DEC|ENC},
    {"rtmp_playpath", "Stream identifier to play or to publish",              OFFSET(playpath), AV_OPT_TYPE_STRING, {.str = NULL }, 0, 0, DEC|ENC},
    { NULL },
};

#define RTMP_CLASS(flavor)\
static const AVClass lib ## flavor ## _class = {\
    .class_name = "lib" #flavor " protocol",\
    .item_name  = av_default_item_name,\
    .option     = options,\
    .version    = LIBAVUTIL_VERSION_INT,\
};

RTMP_CLASS(rtmp)
URLProtocol ff_librtmp_protocol = {
    .name                = "rtmp",
    .url_open            = rtmp_open,
    .url_read            = rtmp_read,
    .url_write           = rtmp_write,
    .url_close           = rtmp_close,
    .url_read_pause      = rtmp_read_pause,
    .url_read_seek       = rtmp_read_seek,
    .url_get_file_handle = rtmp_get_file_handle,
    .priv_data_size      = sizeof(LibRTMPContext),
    .priv_data_class     = &librtmp_class,
    .flags               = URL_PROTOCOL_FLAG_NETWORK,
};

RTMP_CLASS(rtmpt)
URLProtocol ff_librtmpt_protocol = {
    .name                = "rtmpt",
    .url_open            = rtmp_open,
    .url_read            = rtmp_read,
    .url_write           = rtmp_write,
    .url_close           = rtmp_close,
    .url_read_pause      = rtmp_read_pause,
    .url_read_seek       = rtmp_read_seek,
    .url_get_file_handle = rtmp_get_file_handle,
    .priv_data_size      = sizeof(LibRTMPContext),
    .priv_data_class     = &librtmpt_class,
    .flags               = URL_PROTOCOL_FLAG_NETWORK,
};

RTMP_CLASS(rtmpe)
URLProtocol ff_librtmpe_protocol = {
    .name                = "rtmpe",
    .url_open            = rtmp_open,
    .url_read            = rtmp_read,
    .url_write           = rtmp_write,
    .url_close           = rtmp_close,
    .url_read_pause      = rtmp_read_pause,
    .url_read_seek       = rtmp_read_seek,
    .url_get_file_handle = rtmp_get_file_handle,
    .priv_data_size      = sizeof(LibRTMPContext),
    .priv_data_class     = &librtmpe_class,
    .flags               = URL_PROTOCOL_FLAG_NETWORK,
};

RTMP_CLASS(rtmpte)
URLProtocol ff_librtmpte_protocol = {
    .name                = "rtmpte",
    .url_open            = rtmp_open,
    .url_read            = rtmp_read,
    .url_write           = rtmp_write,
    .url_close           = rtmp_close,
    .url_read_pause      = rtmp_read_pause,
    .url_read_seek       = rtmp_read_seek,
    .url_get_file_handle = rtmp_get_file_handle,
    .priv_data_size      = sizeof(LibRTMPContext),
    .priv_data_class     = &librtmpte_class,
    .flags               = URL_PROTOCOL_FLAG_NETWORK,
};

RTMP_CLASS(rtmps)
URLProtocol ff_librtmps_protocol = {
    .name                = "rtmps",
    .url_open            = rtmp_open,
    .url_read            = rtmp_read,
    .url_write           = rtmp_write,
    .url_close           = rtmp_close,
    .url_read_pause      = rtmp_read_pause,
    .url_read_seek       = rtmp_read_seek,
    .url_get_file_handle = rtmp_get_file_handle,
    .priv_data_size      = sizeof(LibRTMPContext),
    .priv_data_class     = &librtmps_class,
    .flags               = URL_PROTOCOL_FLAG_NETWORK,
};
