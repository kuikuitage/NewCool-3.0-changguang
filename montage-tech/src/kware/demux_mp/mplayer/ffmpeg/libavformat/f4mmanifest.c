/*
 * Adobe Media Manifest (F4M) File Parser
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
 * @brief Adobe Media Manifest (F4M) File Parser
 * @author Cory McCarthy
 * @see http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/devnet/hds/pdfs/adobe-media-manifest-specification.pdf
 */

#include "f4mmanifest.h"
#include "libavutil/avstring.h"
#include "libavutil/base64.h"
#include "expat/lib/expat.h"
#ifndef __LINUX__
#include "mp_func_trans.h"
#endif
#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define XML_PARSE_DONE 99999

static void XMLCALL xml_start(void *data, const char *el, const char **attr)
{
    int i;
    F4MManifest *manifest = data;

    //av_log(NULL,AV_LOG_DEBUG,"  In xml_start \n");

    manifest->data_type = DATA_TYPE_NONE;
    if      (!strcmp(el, "id"))         manifest->data_type = DATA_TYPE_ID;
    else if (!strcmp(el, "streamType")) manifest->data_type = DATA_TYPE_STREAM_TYPE;
    else if (!strcmp(el, "duration"))   manifest->data_type = DATA_TYPE_DURATION;
    else if (!strcmp(el, "media")) {
        av_log(NULL,AV_LOG_DEBUG,"  data_type is media. Allocating media struct. \n");
        struct F4MMedia *m = av_mallocz(sizeof(*m));
        if (!m) {
            manifest->parse_ret = AVERROR(ENOMEM);
            return;
        }
        av_log(NULL,AV_LOG_DEBUG,"  adding media to dynarray. manifest->nb_media[%d]\n", manifest->nb_media);
        dynarray_add(&manifest->media, &manifest->nb_media, m);


        for (i = 0; attr[i]; i += 2) {
            //if      (!strcmp(attr[i], "streamId"))        m->stream_id         = av_strdup(attr[i + 1]);
            if (!strcmp(attr[i], "url"))
            {
                memcpy(m->url, attr[i + 1], strlen(attr[i + 1]));
            }
            else if (!strcmp(attr[i], "bitrate"))
            {
                m->bitrate = strtol(attr[i + 1], NULL, 10);
            }
            else if (!strcmp(attr[i], "width"))
            {
                m->width= strtol(attr[i + 1], NULL, 10);
            }
            else if (!strcmp(attr[i], "height"))
            {
                m->height= strtol(attr[i + 1], NULL, 10);
            }
            else if (!strcmp(attr[i], "bootstrapInfoId"))
            {
                memcpy(m->bootstrap_info_id, attr[i + 1], strlen(attr[i + 1]));
            }
        }
    } else if (!strcmp(el, "metadata")) {
        manifest->data_type = DATA_TYPE_MEDIA_METADATA;
    } else if (!strcmp(el, "bootstrapInfo")) {
        av_log(NULL,AV_LOG_DEBUG,"  data_type is bootstrapInfo. Allocating BootstrapInfo type. \n");

        F4MBootstrapInfo *bi = av_mallocz(sizeof(*bi));
        if (!bi) {
            manifest->parse_ret = AVERROR(ENOMEM);
            return;
        }
        av_log(NULL,AV_LOG_DEBUG,"  adding BootstrapInfo to dynarray manifest->nb_bootstraps[%d]. \n", manifest->nb_bootstraps);
        dynarray_add(&manifest->bootstraps, &manifest->nb_bootstraps, bi);

        for (i = 0; attr[i]; i += 2) {
            if (!strcmp(attr[i], "profile"))
            {
                memcpy(bi->profile, attr[i + 1], strlen(attr[i + 1]));
            }
            else if (!strcmp(attr[i], "id"))
            {
                memcpy(bi->id, attr[i + 1], strlen(attr[i + 1]));
            }
            else if (!strcmp(attr[i], "url"))
            {
                memcpy(bi->url, attr[i + 1], strlen(attr[i + 1]));
            }
        }
        manifest->data_type = DATA_TYPE_BOOTSTRAP_INFO;
    }
}


static void XMLCALL xml_data(void *data, const char *s, int len)
{
    F4MManifest *manifest = data;
    uint8_t *src;
    uint8_t *dst;
    int   ret = 0;
    //av_log(NULL,AV_LOG_DEBUG,"  In xml_data manifest->data_type[%d]\n", manifest->data_type);

    if (!len )
        return;
    switch (manifest->data_type) {
        case DATA_TYPE_NONE:
        {
            break;
        }
        case DATA_TYPE_ID:
        {
            memcpy(manifest->id, s, len);
            break;
        }
        case DATA_TYPE_STREAM_TYPE:
            {
            memcpy(manifest->stream_type, s, len);
            break;
        }
        case DATA_TYPE_MEDIA_METADATA:
        {
            if(len < 8)
                return;
            src = av_mallocz(sizeof(uint8_t)*(len+1));
            dst = av_mallocz(sizeof(uint8_t)*(len+1));
            if(!dst)
                return;
            memcpy(src, s, len);
            if((ret = av_base64_decode(dst, src, len)) < 0) {
                av_log(NULL, AV_LOG_ERROR, "f4mmanifest Failed to decode base64 metadata, ret: %d \n", ret);
                av_free(src);
                av_free(dst);
                return;
            }

            manifest->media[(manifest->nb_media - 1)]->metadata = av_mallocz(sizeof(uint8_t)*ret);
            if(!manifest->media[(manifest->nb_media - 1)]->metadata)
            {
                av_free(src);
                av_free(dst);
                return;
            }

            manifest->media[manifest->nb_media  - 1]->metadata_size = ret;
            memcpy(manifest->media[manifest->nb_media  - 1]->metadata, dst, ret);;
            av_free(src);
            av_free(dst);
            break;
        }
        case DATA_TYPE_BOOTSTRAP_INFO:
        {
            if(len < 8)
                return;
            src = av_mallocz(sizeof(uint8_t)*(len+1));
            dst = av_mallocz(sizeof(uint8_t)*(len+1));
            if(!dst)
                return;
            memcpy(src, s, len);
            if((ret = av_base64_decode(dst, src, len)) < 0) {
                av_log(NULL, AV_LOG_ERROR, "f4mmanifest Failed to decode base64 metadata, ret: %d \n", ret);
                av_free(src);
                av_free(dst);
                return;
            }

            manifest->bootstraps[manifest->nb_bootstraps - 1]->metadata = av_mallocz(sizeof(uint8_t)*ret);
            if(!manifest->bootstraps[manifest->nb_bootstraps - 1]->metadata)
            {
                av_free(src);
                av_free(dst);
                return;
            }

            manifest->bootstraps[manifest->nb_bootstraps - 1]->metadata_size = ret;
            memcpy(manifest->bootstraps[manifest->nb_bootstraps - 1]->metadata, dst, ret);

            av_free(src);
            av_free(dst);
            break;
        }
        default:
        {
            av_log(NULL,AV_LOG_ERROR," Invalid manifest data_type[%d]\n", manifest->data_type);
            break;
        }
    }

    return;
}

static void XMLCALL xml_end(void *data, const char *el)
{
    F4MManifest *manifest = data;
    //av_log(NULL,AV_LOG_DEBUG,"  In xml_end el[%s]\n", el);
    if(!strcmp(el, "manifest"))//end
        manifest->parse_ret = XML_PARSE_DONE;
}

int ff_parse_f4m_manifest(uint8_t *buffer, int size, F4MManifest *manifest)
{
    int done = 0;
    char buf[4096];
    XML_Parser xmlp;
    int i, j;

    av_log(NULL,AV_LOG_DEBUG,"In parse_manifest \n");

    xmlp = XML_ParserCreate(NULL);
    if (!xmlp) {
        av_log(NULL, AV_LOG_ERROR, "Unable to allocate memory for the libexpat XML parser\n");
        return AVERROR(ENOMEM);
    }
    XML_SetUserData(xmlp, manifest);
    XML_SetElementHandler(xmlp, xml_start, xml_end);
    XML_SetCharacterDataHandler(xmlp, xml_data);

    while (!done && !manifest->parse_ret) {

        if (XML_Parse(xmlp, buffer, size, done) == XML_STATUS_ERROR) {
            av_log(NULL, AV_LOG_ERROR, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
                   XML_GetCurrentLineNumber(xmlp),
                   XML_ErrorString(XML_GetErrorCode(xmlp)));
            return AVERROR_INVALIDDATA;
        }
    }
    if(manifest->parse_ret == XML_PARSE_DONE)
    {
        manifest->parse_ret = 0;
    }

    /*for (i = 0; i < manifest->nb_bootstraps; i++) {
        for (j = 0; j < manifest->nb_media; j++) {
            if (strcmp(manifest->media[j]->bootstrap_info_id, manifest->bootstraps[i]->id) == 0)
            {
                av_log(s, AV_LOG_DEBUG, "Linking media at index [%d] to bootstrap info at index [%d] (bootstrap_id: [%s])\n", j, i, manifest->bootstraps[i]->id);
                //manifest->bootstraps[i]->media_id = j;
                //manifest->media[j]->bootstrap_id = i;
            }
        }
    }*/

    XML_ParserFree(xmlp);
    return manifest->parse_ret;
}



int ff_free_manifest(F4MManifest *manifest)
{
    F4MBootstrapInfo *bootstrap_info;
    F4MMedia *media;
    int i;

    for(i = 0; i < manifest->nb_bootstraps; i++) {
        bootstrap_info = manifest->bootstraps[i];
        av_freep(&bootstrap_info->metadata);
        av_freep(&bootstrap_info);
    }

    for(i = 0; i < manifest->nb_media; i++) {
        media = manifest->media[i];
        av_freep(&media->metadata);
        av_freep(&media);
    }

    memset(manifest, 0x00, sizeof(F4MManifest));

    return 0;
}
