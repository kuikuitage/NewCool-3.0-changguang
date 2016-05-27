/*
 * Register all the formats and protocols
 * Copyright (c) 2000, 2001, 2002 Fabrice Bellard
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
#include "avformat.h"
#include "rtp.h"
#include "rdt.h"
#include "url.h"
#include "version.h"
#include "config.h"
#ifndef __LINUX__
#include "mp_func_trans.h"
#endif
#define REGISTER_MUXER(X,x) { \
    extern AVOutputFormat ff_##x##_muxer; \
    if(CONFIG_##X##_MUXER) av_register_output_format(&ff_##x##_muxer); }

#define REGISTER_DEMUXER(X,x) { \
    extern AVInputFormat ff_##x##_demuxer; \
    if(CONFIG_##X##_DEMUXER) av_register_input_format(&ff_##x##_demuxer); }

#define REGISTER_MUXDEMUX(X,x)  REGISTER_MUXER(X,x); REGISTER_DEMUXER(X,x)

#define REGISTER_PROTOCOL(X,x) { \
    extern URLProtocol ff_##x##_protocol; \
    if(CONFIG_##X##_PROTOCOL) ffurl_register_protocol(&ff_##x##_protocol, sizeof(ff_##x##_protocol)); }
extern int rtmp_protocol_type;
void av_register_all(void)
{
#ifdef  WITH_TCPIP_PROTOCOL
    static int initialized;

    if (initialized)
        return;
    initialized = 1;
    REGISTER_DEMUXER(ASF,              asf);
    REGISTER_DEMUXER (HDS, hds);
  #ifdef __LINUX__
    REGISTER_DEMUXER (FLV, flv); 
    REGISTER_DEMUXER (MATROSKA, matroska);
    REGISTER_DEMUXER (AVI, avi);
    REGISTER_DEMUXER (MOV, mov);
     //REGISTER_DEMUXER (MP3, mp3);
     REGISTER_DEMUXER (WAV, wav);
    REGISTER_DEMUXER (HLS, hls);
    REGISTER_DEMUXER (MPEGTS, mpegts);
    //REGISTER_DEMUXER  (MPEGTSRAW, mpegtsraw);
    //avcodec_register_all();
	
    REGISTER_PROTOCOL(HTTP, http);
    REGISTER_PROTOCOL(TCP, tcp);
    REGISTER_PROTOCOL (RTMP, rtmp);
  #else
    //yliu:rm avi support in demux_lavf
    //REGISTER_DEMUXER (AVI, avi);
    REGISTER_MUXDEMUX (MOV, mov);
    REGISTER_MUXDEMUX (MATROSKA, matroska);
    REGISTER_MUXDEMUX (FLV, flv); 
     //REGISTER_DEMUXER (MP3, mp3);
     REGISTER_MUXDEMUX (WAV, wav);
    REGISTER_DEMUXER (HLS, hls);
    REGISTER_DEMUXER (MPEGTS, mpegts);
    REGISTER_DEMUXER (AC3, ac3);
    REGISTER_DEMUXER (EAC3, eac3);

	

    REGISTER_PROTOCOL(HTTP, http);
    REGISTER_PROTOCOL(TCP, tcp);
    REGISTER_PROTOCOL(CRYPTO, crypto);

    #ifdef ENABLE_OPEN_HTTPS
    REGISTER_PROTOCOL(HTTPS, https);
    REGISTER_PROTOCOL(TLS, tls);
    #endif
    
  #endif
#ifdef ENABLE_DEMUX_RTMP
   if((rtmp_protocol_type == 0)||(rtmp_protocol_type == 2))
    {
     #ifndef __LINUX__
    OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
     #endif
      REGISTER_PROTOCOL (RTMP, rtmp);
    }

  #if 1
  #ifndef __LINUX__
     if((rtmp_protocol_type == 0)||(rtmp_protocol_type == 1))
        {
        OS_PRINTF("\n%s %d\n",__FUNCTION__,__LINE__);
    REGISTER_PROTOCOL (LIBRTMP, librtmp);
    REGISTER_PROTOCOL (LIBRTMPE, librtmpe);
    REGISTER_PROTOCOL (LIBRTMPS, librtmps);
    REGISTER_PROTOCOL (LIBRTMPT, librtmpt);
    REGISTER_PROTOCOL (LIBRTMPTE, librtmpte);
        }
    #endif
   #endif
#endif
    //REGISTER_MUXDEMUX (FLV, flv);
    //REGISTER_MUXER    (F4V, f4v);
      avcodec_register_all();
#else
    static int initialized;

    if (initialized)
        return;
    initialized = 1;

    REGISTER_MUXDEMUX (MOV, mov);
    REGISTER_MUXDEMUX (MATROSKA, matroska);
    REGISTER_MUXDEMUX (FLV, flv); 
    //REGISTER_DEMUXER (MPEGTS, mpegts);
       avcodec_register_all();
    
    #endif
    
}
