/*
 * Adobe HTTP Dynamic Streaming (HDS) demuxer
 * Copyright (c) 2013 Cory McCarthy
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
 * @brief Adobe HTTP Dynamic Streaming (HDS) demuxer
 * @author Cory McCarthy
 * @see http://www.adobe.com/devnet/hds.html
 * @see http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/devnet/hds/pdfs/adobe-hds-specification.pdf
 * @see http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/devnet/hds/pdfs/adobe-media-manifest-specification.pdf
 * @see http://download.macromedia.com/f4v/video_file_format_spec_v10_1.pdf
 *
 * @note Link for a HDS test player below:
 * @see http://mediapm.edgesuite.net/edgeflash/public/zeri/debug/Main.html
 *
 * @note Test streams are below:
 * @test http://multiplatform-f.akamaihd.net/z/multi/april11/hdworld/hdworld_,512x288_450_b,640x360_700_b,768x432_1000_b,1024x576_1400_m,1280x720_1900_m,1280x720_2500_m,1280x720_3500_m,.mp4.csmil/manifest.f4m?hdcore
 * @test http://multiplatform-f.akamaihd.net/z/multi/april11/cctv/cctv_,512x288_450_b,640x360_700_b,768x432_1000_b,1024x576_1400_m,1280x720_1900_m,1280x720_2500_m,1280x720_3500_m,.mp4.csmil/manifest.f4m?hdcore
 * @test http://multiplatform-f.akamaihd.net/z/multi/april11/sintel/sintel-hd_,512x288_450_b,640x360_700_b,768x432_1000_b,1024x576_1400_m,1280x720_1900_m,1280x720_2500_m,1280x720_3500_m,.mp4.csmil/manifest.f4m?hdcore
 * @test http://multiplatform-f.akamaihd.net/z/multi/akamai10year/Akamai_10_Year_,200,300,600,800,1000,1500,2500,4000,k.mp4.csmil/manifest.f4m?hdcore
 * @test http://zerihdndemo-f.akamaihd.net/z/h264/seeker/LegendofSeeker_16x9_24fps_H264_,400K,650K,1Mbps,1.4Mbps,1.8Mbps,2.5Mbps,.mp4.csmil/manifest.f4m?hdcore
 * @test http://multiplatform-f.akamaihd.net/z/multi/will/bunny/big_buck_bunny_,640x360_400,640x360_700,640x360_1000,950x540_1500,1280x720_2000,1280x720_3000,.f4v.csmil/manifest.f4m?hdcore
 * @test http://multiplatform-f.akamaihd.net/z/multi/companion/nba_game/nba_game.mov_,300,600,800,1000,2500,4000,9000,k.mp4.csmil/manifest.f4m?hdcore
 * @test http://multiplatform-f.akamaihd.net/z/multi/companion/big_bang_theory/big_bang_theory.mov_,300,600,800,1000,2500,4000,9000,k.mp4.csmil/manifest.f4m?hdcore
 * @test http://multiplatform-f.akamaihd.net/z/multi/shuttle/shuttle_,300,600,800,1000,k.mp4.csmil/manifest.f4m?hdcore
 * @test http://multiplatform-f.akamaihd.net/z/multi/up_trailer/up_trailer_720p_,300,600,800,1000,k.mp4.csmil/manifest.f4m?hdcore
 * @test http://multiformatlive-f.akamaihd.net/z/demostream_1@2131/manifest.f4m?hdcore
 * @test http://zerihdndemo-f.akamaihd.net/z/h264/darkknight/darkknight.smil/manifest.f4m?hdcore
 * @test http://zerihdndemo-f.akamaihd.net/z/h264/amours/amours.smil/manifest.f4m?hdcore
 * @test http://zerihdndemo-f.akamaihd.net/z/h264/robinhood/robinhood.smil/manifest.f4m?hdcore
 * @test http://zerihdndemo-f.akamaihd.net/z/h264/wallstreet/wallstreet.smil/manifest.f4m?hdcore
 * @test http://zerihdndemo-f.akamaihd.net/z/h264/rockandroll/rockandroll.smil/manifest.f4m?hdcore
 * @test http://184.72.239.149/vod/smil:bigbuckbunny.smil/manifest.f4m
 */

#include "avformat.h"
#include "internal.h"
#include "url.h"
#include "avio_internal.h"
#include "libavutil/avstring.h"
#include "libavutil/parseutils.h"
#include "libavutil/time.h"

#include "amfmetadata.h"
#include "f4mmanifest.h"
#include "f4fbox.h"
#include "flvtag.h"
#ifndef __LINUX__
#include "mp_func_trans.h"
#endif

#define MAX_NB_SAMPLES 1024

static int open_url_fail = 0;//maybe index error, update bootstrap_info immediately

typedef struct HDSBootstrapInfo {
    char id[MAX_URL_SIZE];
    char url[MAX_URL_SIZE];
    char profile[MAX_URL_SIZE];

    int  last_fragment;
    F4FBox box;
} HDSBootstrapInfo;

typedef struct HDSMedia {
    int bitrate;
    char url[MAX_URL_SIZE];
    char bootstrap_info_id[MAX_URL_SIZE];

    AVStream *audio_stream;
    AVStream *video_stream;

    int nb_samples;
    FLVMediaSample *samples[MAX_NB_SAMPLES];
    int sample_index;

    int nb_fragments_read;
} HDSMedia;

typedef struct HDSContext {
    char id[MAX_URL_SIZE];
    int is_live;
    char base_url[MAX_URL_SIZE];

    int nb_bootstraps;
    HDSBootstrapInfo *bootstrap_info[MAX_NB_BOOTSTRAPS];

    int nb_media;
    HDSMedia *media[MAX_NB_MEDIA];
} HDSContext;

static void construct_bootstrap_url(const char *base_url, const char *bootstrap_url,
    const char *suffix, char *url_out, size_t url_size)
{
    char *p;

    p = url_out;
    p += av_strlcat(p, base_url, url_size);
    p += av_strlcat(p, bootstrap_url, url_size);
    p += av_strlcat(p, suffix, url_size);
}

static int download_bootstrap(AVFormatContext *s, HDSBootstrapInfo *bootstrap,
    uint8_t **buffer_out, int *buffer_size_out)
{
    HDSContext *c = s->priv_data;
    URLContext *puc;
    char url[MAX_URL_SIZE];
    uint8_t *buffer;
    int buffer_size;
    int ret;

    memset(url, 0x00, sizeof(url));

    if(!av_stristr(bootstrap->url, "?") && av_stristr(s->filename, "?")) {
        construct_bootstrap_url(c->base_url, bootstrap->url, av_stristr(s->filename, "?"), url, MAX_URL_SIZE);
    } else {
        construct_bootstrap_url(c->base_url, bootstrap->url, "", url, MAX_URL_SIZE);
    }

    av_log(NULL, AV_LOG_DEBUG, "download_bootstrap url:%s\n", url);
    if((ret = ffurl_open(&puc, url, AVIO_FLAG_READ, &s->interrupt_callback, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to start downloading bootstrap, ret: %d \n", ret);
        return ret;
    }

    buffer_size = ffurl_size(puc);
    buffer = av_mallocz(buffer_size+FF_INPUT_BUFFER_PADDING_SIZE);
    if(!buffer)
        return AVERROR(ENOMEM);

    if((ret = ffurl_read_complete(puc, buffer, buffer_size)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to downloaded bootstrap, ret: %d \n", ret);
        av_free(buffer);
        return ret;
    }

    if((ret = ffurl_close(puc)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to finish downloading bootstrap, ret: %d \n", ret);
        av_free(buffer);
        return ret;
    }

    if(buffer_out)
        *buffer_out = buffer;
    if(buffer_size_out)
        *buffer_size_out = buffer_size;

    return 0;
}

static int create_bootstrap_info(AVFormatContext *s, F4MBootstrapInfo *f4m_bootstrap_info)
{
    HDSContext *c = s->priv_data;
    HDSBootstrapInfo *bootstrap_info;
    uint8_t *buffer;
    int buffer_size, ret;

    bootstrap_info = av_mallocz(sizeof(HDSBootstrapInfo));
    if(!bootstrap_info)
        return AVERROR(ENOMEM);

    c->bootstrap_info[c->nb_bootstraps++] = bootstrap_info;

    memcpy(bootstrap_info->id, f4m_bootstrap_info->id, sizeof(bootstrap_info->id));
    memcpy(bootstrap_info->url, f4m_bootstrap_info->url, sizeof(bootstrap_info->url));
    memcpy(bootstrap_info->profile, f4m_bootstrap_info->profile, sizeof(bootstrap_info->profile));

    if(f4m_bootstrap_info->metadata_size > 0) {
        av_log(NULL, AV_LOG_DEBUG, "enter f4m_bootstrap_info->metadata_size > 0 !!!!!!\n");
        buffer = f4m_bootstrap_info->metadata;
        buffer_size = f4m_bootstrap_info->metadata_size;

        if((ret = ff_parse_f4f_box(buffer, buffer_size, &(bootstrap_info->box))) < 0) {
            av_log(NULL, AV_LOG_ERROR, "hds Failed to parse metadata bootstrap box, ret: %d \n", ret);
            return ret;
        }
    }
    else {
        if((ret = download_bootstrap(s, bootstrap_info, &buffer, &buffer_size)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "hds Failed to download bootstrap, ret: %d \n", ret);
            return ret;
        }

        if((ret = ff_parse_f4f_box(buffer, buffer_size, &(bootstrap_info->box))) < 0) {
            av_log(NULL, AV_LOG_ERROR, "hds Failed to parse downloaded bootstrap box, ret: %d \n", ret);
            av_free(buffer);
            return ret;
        }

        av_free(buffer);
    }

    return 0;
}

static int create_streams(AVFormatContext *s, HDSMedia *media, AMFMetadata *metadata)
{
    AVStream *st;

    st = avformat_new_stream(s, NULL);
    if(!st)
        return AVERROR(ENOMEM);

    media->video_stream = st;

    st->id = 0;
    avpriv_set_pts_info(st, 32, 1, 1000);

    st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codec->codec_id = metadata->video_codec_id;
    st->codec->width = metadata->width;
    st->codec->height = metadata->height;
    st->codec->bit_rate = metadata->video_data_rate * 1000;

    st = avformat_new_stream(s, NULL);
    if(!st)
        return AVERROR(ENOMEM);

    media->audio_stream = st;

    st->id = 0;
    avpriv_set_pts_info(st, 32, 1, 1000);

    st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id = metadata->audio_codec_id;
    st->codec->channels = metadata->nb_audio_channels;
    st->codec->sample_rate = metadata->audio_sample_rate;
    st->codec->sample_fmt = AV_SAMPLE_FMT_S16;
    st->codec->bit_rate = metadata->audio_data_rate * 1000;

    return 0;
}

static int create_media(AVFormatContext *s, F4MMedia *f4m_media)
{
    HDSContext *c = s->priv_data;
    HDSMedia *media;
    AMFMetadata metadata;
    int ret;

    media = av_mallocz(sizeof(HDSMedia));
    if(!media)
        return AVERROR(ENOMEM);

    c->media[c->nb_media++] = media;

    media->bitrate = f4m_media->bitrate;
    memcpy(media->url, f4m_media->url, sizeof(media->url));
    av_log(NULL, AV_LOG_DEBUG, "create_media media->url: %s \n", media->url);
    memcpy(media->bootstrap_info_id, f4m_media->bootstrap_info_id, sizeof(media->bootstrap_info_id));

    memset(&metadata, 0x00, sizeof(AMFMetadata));
    if((ret = ff_parse_amf_metadata(f4m_media->metadata, f4m_media->metadata_size, &metadata)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to parse metadata, ret: %d \n", ret);
        return ret;
    }

    if((ret = create_streams(s, media, &metadata)) < 0)
        return ret;

    return 0;
}

static int create_pmt(AVFormatContext *s)
{
    HDSContext *c = s->priv_data;
    HDSMedia *media;
    AVProgram *p;
    int i, j;

    j = 0;
    for(i = 0; i < c->nb_media; i++) {
        media = c->media[i];

        p = av_new_program(s, j++);
        if(!p)
            return AVERROR(ENOMEM);

        av_dict_set(&p->metadata,"name",
            av_asprintf("Bandwidth: %dKbps", media->bitrate), 0);

        ff_program_add_stream_index(s, p->id, media->video_stream->index);
        ff_program_add_stream_index(s, p->id, media->audio_stream->index);
    }

    return 0;
}

static int initialize_context(AVFormatContext *s, F4MManifest *manifest)
{
    HDSContext *c = s->priv_data;
    F4MBootstrapInfo *f4m_bootstrap_info;
    F4MMedia *f4m_media;
    int i, ret;

    int prefer_size = -1;
    for(i = 0; i < manifest->nb_media; i++) {
        f4m_media = manifest->media[i];
        if(f4m_media->width > 0)
        {
            av_log(NULL, AV_LOG_DEBUG, "hds i[%d] f4m_media->width:%d \n", i, f4m_media->width);
            if(f4m_media->width> 500 && f4m_media->width< 800)//d1
            {
                prefer_size = i;
                break;
            }
        }
    }
    if(prefer_size == -1)
    {
        for(i = 0; i < manifest->nb_media; i++) {
            f4m_media = manifest->media[i];
            if(f4m_media->width > 0)
            {
                if(f4m_media->width> 300 && f4m_media->width< 500)//cif
                {
                    prefer_size = i;
                    break;
                }
            }
        }
    }

    if(prefer_size == -1 && manifest->nb_media > 1)
    {
        prefer_size = 1;
    }
    else if(prefer_size == -1)
    {
        prefer_size = 0;
    }

    av_log(NULL, AV_LOG_DEBUG, "hds choosed index[%d] \n", prefer_size);
    f4m_bootstrap_info = manifest->bootstraps[prefer_size];
    if((ret = create_bootstrap_info(s, f4m_bootstrap_info)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to create bootstrap_info, ret: %d \n", ret);
        return ret;
    }

    for(i = 0; i < manifest->nb_media; i++) {
        f4m_media = manifest->media[i];
        if(strcmp(f4m_media->bootstrap_info_id, f4m_bootstrap_info->id) == 0)
        {
            if((ret = create_media(s, f4m_media)) < 0) {
                av_log(NULL, AV_LOG_ERROR, "hds Failed to create media, ret: %d \n", ret);
                return ret;
            }
        }
    }

    if((ret = create_pmt(s)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to create PMT, ret: %d \n", ret);
        return ret;
    }

    if(!av_strcasecmp(manifest->stream_type, "live"))
        c->is_live = 1;

    return 0;
}

static int hds_read_header(AVFormatContext *s)
{
    HDSContext *c = s->priv_data;
    AVIOContext *in = s->pb;
    F4MManifest manifest;
    int64_t filesize;
    uint8_t *buf;
    char *p;
    int ret;

    p = av_stristr(s->filename, "manifest.f4m");
    if(!p) {
        p = av_stristr(s->filename, "live.f4m");
        if(!p) {
            p = av_stristr(s->filename, "live_src.f4m");
            if(!p) {
                av_log(NULL, AV_LOG_ERROR, "hds Failed to build base url, url: %s \n", s->filename);
                return -1;
            }
        }
    }
    av_strlcpy(c->base_url, s->filename, p - s->filename + 1);

    filesize = avio_size(in);
    if(filesize <= 0)
        return -1;

    buf = av_mallocz(filesize*sizeof(uint8_t));
    if(!buf)
        return AVERROR(ENOMEM);

    int len = avio_read(in, buf, filesize);

    memset(&manifest, 0x00, sizeof(F4MManifest));
    if((ret = ff_parse_f4m_manifest(buf, len, &manifest)) < 0) {
        av_free(buf);
        ff_free_manifest(&manifest);
        return ret;
    }

    av_free(buf);

    ret = initialize_context(s, &manifest);
    ff_free_manifest(&manifest);

    return ret;
}

static void construct_fragment_url(const char *base_url, const char *media_url,
    int segment, int fragment, const char *suffix, char *url_out, size_t url_size)
{
    char *p;
    char *fragment_str;

    p = url_out;
    p += av_strlcat(p, base_url, url_size);
    p += av_strlcat(p, media_url, url_size);

    fragment_str = av_asprintf("Seg%d-Frag%d", segment, fragment);
    p += av_strlcat(p, fragment_str, url_size);
    av_free(fragment_str);

    p += av_strlcat(p, suffix, url_size);

    av_log(NULL, AV_LOG_DEBUG, "construct_fragment_url url: %s \n", url_out);
}

static int get_segment_fragment(AVFormatContext *s, int is_live, HDSBootstrapInfo *bootstrap_info,
    HDSMedia *media, int *segment_out, int *fragment_out)
{
    F4FBootstrapInfoBox *abst = &(bootstrap_info->box.abst);
    F4FSegmentRunTableBox *asrt;
    F4FFragmentRunTableBox *afrt;
    F4FSegmentRunEntry *segment_entry;
    F4FFragmentRunEntry *fragment_entry;
    int segment, fragment;
    int i, j;

    if(open_url_fail)
    {
        open_url_fail = 0;
        ff_free_f4f_box(&bootstrap_info->box);
        int buffer_size, ret;
        uint8_t *buffer;
        av_log(NULL, AV_LOG_DEBUG, "last open url failed! update bootstrap!\n");
        if((ret = download_bootstrap(s, bootstrap_info, &buffer, &buffer_size)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "hds Failed to download bootstrap, ret: %d \n", ret);
            return ret;
        }

        if((ret = ff_parse_f4f_box(buffer, buffer_size, &(bootstrap_info->box))) < 0) {
            av_log(NULL, AV_LOG_ERROR, "hds Failed to parse downloaded bootstrap box, ret: %d \n", ret);
            av_free(buffer);
            return ret;
        }
        afrt = abst->fragment_run_table_boxes[abst->nb_fragment_run_table_boxes-1];
        fragment_entry = afrt->fragment_run_entries[0];
        bootstrap_info->last_fragment = fragment_entry->first_fragment;
    }

    for(i = 0; i < abst->nb_segment_run_table_boxes; i++) {
        asrt = abst->segment_run_table_boxes[i];
        if(asrt)
        {
            for(j = 0; j < asrt->nb_segment_run_entries; j++) {
                segment_entry = asrt->segment_run_entries[j];

                segment = segment_entry->first_segment;
            }
        }
    }

    if(!segment_entry) {
        av_log(NULL, AV_LOG_ERROR, "hds segment entry not found");
        return -1;
    }

    av_log(NULL, AV_LOG_DEBUG, "get_segment_fragment nb_segment_run_table_boxes[%d] nb_fragment_run_table_boxes[%d]\n",
            abst->nb_segment_run_table_boxes, abst->nb_fragment_run_table_boxes);
    afrt = abst->fragment_run_table_boxes[abst->nb_fragment_run_table_boxes-1];
    fragment_entry = afrt->fragment_run_entries[0];

    if(bootstrap_info->last_fragment == 0)
    {
        bootstrap_info->last_fragment = fragment_entry->first_fragment;//start
    }
    else
    {
        bootstrap_info->last_fragment++;
    }


    fragment = bootstrap_info->last_fragment;
    if(!fragment_entry) {
        av_log(NULL, AV_LOG_ERROR, "hds fragment entry not found\n");
        return -1;
    }

    int request_fragment_dis_num = segment_entry->fragments_per_segment/2 - 3;
    if((fragment + request_fragment_dis_num) == (fragment_entry->first_fragment + segment_entry->fragments_per_segment))
    {
        if(abst->nb_segment_run_table_boxes == (MAX_NB_SEGMENT_RUN_TABLE_BOXES-2)
            || abst->nb_fragment_run_table_boxes == (MAX_NB_FRAGMENT_RUN_TABLE_BOXES-2))
        {
            av_log(NULL, AV_LOG_DEBUG, "update bootstrap ff_free_f4f_box\n");
            ff_free_f4f_box(&bootstrap_info->box);
        }
        int buffer_size, ret;
        uint8_t *buffer;
        av_log(NULL, AV_LOG_DEBUG, "update bootstrap\n");
        if((ret = download_bootstrap(s, bootstrap_info, &buffer, &buffer_size)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "hds Failed to download bootstrap, ret: %d \n", ret);
            return ret;
        }

        if((ret = ff_parse_f4f_box(buffer, buffer_size, &(bootstrap_info->box))) < 0) {
            av_log(NULL, AV_LOG_ERROR, "hds Failed to parse downloaded bootstrap box, ret: %d \n", ret);
            av_free(buffer);
            return ret;
        }
    }

    if(segment_out)
        //*segment_out = 1;
        *segment_out = segment;
    if(fragment_out)
        *fragment_out = fragment;

    return 0;
}

static int download_fragment(AVFormatContext *s,
    HDSBootstrapInfo *bootstrap_info, HDSMedia *media,
    uint8_t **buffer_out, int *buffer_size_out)
{
    HDSContext *c = s->priv_data;
    URLContext *puc;
    char url[MAX_URL_SIZE];
    uint8_t *buffer;
    int buffer_size;
    int segment, fragment;
    int ret;

    if((ret = get_segment_fragment(s, c->is_live, bootstrap_info, media, &segment, &fragment)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "error, ret[%d] download_fragment segment:%d, fragment:%d \n", ret, segment, fragment);
        return ret;
    }

    av_log(NULL, AV_LOG_ERROR, "download_fragment segment:%d, fragment:%d \n", segment, fragment);

    memset(url, 0x00, sizeof(url));

    if(!av_stristr(media->url, "?") && av_stristr(s->filename, "?")) {
        construct_fragment_url(c->base_url, media->url,
            segment, fragment, av_stristr(s->filename, "?"), url, MAX_URL_SIZE);
    } else {
        construct_fragment_url(c->base_url, media->url,
            segment, fragment, "", url, MAX_URL_SIZE);
    }

    if((ret = ffurl_open(&puc, url, AVIO_FLAG_READ, &s->interrupt_callback, NULL)) < 0) {
        if(ret != AVERROR(EIO))
            av_log(NULL, AV_LOG_ERROR, "hds Failed to start downloading fragment, url:%s, ret:%d \n", url, ret);
        open_url_fail = 1;
        return ret;
    }

    buffer_size = ffurl_size(puc);
    buffer = av_mallocz(buffer_size+FF_INPUT_BUFFER_PADDING_SIZE);
    if(!buffer)
        return AVERROR(ENOMEM);

    if((ret = ffurl_read_complete(puc, buffer, buffer_size)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to downloaded fragment, ret: %d \n", ret);
        av_free(buffer);
        return ret;
    }

     av_log(NULL, AV_LOG_DEBUG, "download_fragment url[%s] buffer_size[%d] media->nb_fragments_read[%d]\n", url, buffer_size, media->nb_fragments_read);

    if((ret = ffurl_close(puc)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to finish downloading fragment, ret: %d \n", ret);
        av_free(buffer);
        return ret;
    }

    media->nb_fragments_read++;

    if(buffer_out)
        *buffer_out = buffer;
    if(buffer_size_out)
        *buffer_size_out = buffer_size;

    return 0;
}

static int get_next_fragment(AVFormatContext *s,
    HDSBootstrapInfo *bootstrap_info, HDSMedia *media)
{
    F4FBox box;
    uint8_t *buffer;
    int buffer_size, ret;

    if((ret = download_fragment(s, bootstrap_info, media, &buffer, &buffer_size)) < 0) {
        return ret;
    }

    memset(&box, 0x00, sizeof(F4FBox));
    if((ret = ff_parse_f4f_box(buffer, buffer_size, &box)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to parse bootstrap box, ret: %d \n", ret);
        av_free(buffer);
        ff_free_f4f_box(&box);
        return ret;
    }
    av_free(buffer);

    if((ret = ff_decode_flv_body(box.mdat.data, box.mdat.size, media->samples, &media->nb_samples)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "hds Failed to decode FLV body, ret: %d \n", ret);
        ff_free_f4f_box(&box);
        return ret;
    }

    ff_free_f4f_box(&box);

    return 0;
}

static void read_next_sample(HDSMedia *media, AVPacket *pkt)
{
    FLVMediaSample *sample;

    sample = media->samples[media->sample_index];
    media->sample_index++;

    av_new_packet(pkt, sample->data_size);
    memcpy(pkt->data, sample->data, sample->data_size);

    pkt->dts = sample->timestamp;
    if(sample->type == AVMEDIA_TYPE_VIDEO && media->video_stream) {
        pkt->stream_index = media->video_stream->index;
    }
    else
    if(sample->type == AVMEDIA_TYPE_AUDIO && media->audio_stream) {
        pkt->stream_index = media->audio_stream->index;
    }
}

static void clear_samples(HDSMedia *media)
{
    FLVMediaSample *sample;
    int i;

    for(i = 0; i < media->nb_samples; i++) {
        sample = media->samples[i];
        av_freep(&sample->data);
        av_freep(&sample);
        media->samples[i] = NULL;
    }

    media->nb_samples = 0;
    media->sample_index = 0;
}

static int get_next_packet(AVFormatContext *s,
    HDSBootstrapInfo *bootstrap_info, HDSMedia *media, AVPacket *pkt)
{
    int ret;

    if(media->nb_samples == 0) {
        if((ret = get_next_fragment(s, bootstrap_info, media)) < 0) {
            return ret;
        }
    }

    if(media->nb_samples > 0) {
        read_next_sample(media, pkt);
    }

    if(media->sample_index >= media->nb_samples) {
        clear_samples(media);
    }

    return 0;
}

static int hds_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    HDSContext *c = s->priv_data;
    HDSBootstrapInfo *bootstrap_info = NULL;
    HDSMedia *media = NULL;
    int i, j, ret;

    for(i = 0; i < c->nb_media; i++) {
        media = c->media[i];
        bootstrap_info = NULL;

        if(media->video_stream->discard == AVDISCARD_ALL
        && media->audio_stream->discard == AVDISCARD_ALL)
        {
            av_log(NULL, AV_LOG_ERROR, "media->video_stream audio_stream is set to AVDISCARD_ALL\n");
            continue;
        }

        for(j = 0; j < c->nb_bootstraps; j++) {
            if(av_strcasecmp(media->bootstrap_info_id, c->bootstrap_info[j]->id))
                continue;

            bootstrap_info = c->bootstrap_info[j];
            break;
        }

        if(!bootstrap_info)
            continue;

        break;
    }

    if(i == c->nb_media)
        return -1;

    if(!bootstrap_info)
        return 0;
    if(!media)
        return 0;

    if((ret = get_next_packet(s, bootstrap_info, media, pkt)) < 0) {
        if(ret == AVERROR(EIO))
            return 0;
        if(ret == AVERROR_EOF)
            return ret;

        av_log(NULL, AV_LOG_ERROR, "hds Failed to get next packet, ret: %d \n", ret);
        return ret;
    }

    return 0;
}

static int hds_close(AVFormatContext *s)
{
    HDSContext *c = s->priv_data;
    HDSBootstrapInfo *bootstrap_info;
    HDSMedia *media;
    int i;

    for(i = 0; i < c->nb_bootstraps; i++) {
        bootstrap_info = c->bootstrap_info[i];

        ff_free_f4f_box(&bootstrap_info->box);
        av_freep(&bootstrap_info);
    }

    for(i = 0; i < c->nb_media; i++) {
        media = c->media[i];

        clear_samples(media);
        av_freep(&media);
    }

    memset(c, 0x00, sizeof(HDSContext));

    return 0;
}

static int hds_probe(AVProbeData *p)
{
    if(p->filename && av_stristr(p->filename, ".f4m"))
        return AVPROBE_SCORE_MAX;
    return 0;
}

AVInputFormat ff_hds_demuxer = {
    .name           = "hds",
    .long_name      = "Adobe HTTP Dynamic Streaming Demuxer",
    .priv_data_size = sizeof(HDSContext),
    .read_probe     = hds_probe,
    .read_header    = hds_read_header,
    .read_packet    = hds_read_packet,
    .read_close     = hds_close,
};
