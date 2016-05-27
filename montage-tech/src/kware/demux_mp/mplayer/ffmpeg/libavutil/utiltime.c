/*
 * Copyright (c) 2000-2003 Fabrice Bellard
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

#include "../../config.h"

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#if HAVE_GETTIMEOFDAY
#include <sys/time.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_WINDOWS_H
#include <windows.h>
#endif

#include "libavutil/time.h"
#include "error.h"


#ifndef __LINUX__
/*!
  * None
  */
struct ff_timeval {
	/*!
  * None
  */
  long    tv_sec;         /* seconds */
  /*!
  * None
  */
  long    tv_usec;        /* and microseconds */
};



static int ff_gettimeofday_replace(struct ff_timeval* tp, int* p/*tz*/)
{
	unsigned int ticks = mtos_ticks_get();
	tp->tv_sec = ticks*10/1000;
	tp->tv_usec = (ticks*10*1000)%1000000 ;

	return 0;
}
#endif


int64_t av_gettime(void)
{
#if HAVE_GETTIMEOFDAY
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#elif HAVE_GETSYSTEMTIMEASFILETIME
    FILETIME ft;
    int64_t t;
    GetSystemTimeAsFileTime(&ft);
    t = (int64_t)ft.dwHighDateTime << 32 | ft.dwLowDateTime;
    return t / 10 - 11644473600000000; /* Jan 1, 1601 */
#else

#ifndef __LINUX__
    struct ff_timeval tv;
    ff_gettimeofday_replace(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
#endif
    return -1;
#endif
}

int av_usleep(unsigned usec)
{
#if HAVE_NANOSLEEP
    struct timespec ts = { usec / 1000000, usec % 1000000 * 1000 };
    #ifdef __LINUX__
    while (nanosleep(&ts, &ts) < 0 && errno == EINTR);

    #else

    mtos_task_sleep(usec / 1000); //ms
    
    #endif
    return 0;
#elif HAVE_USLEEP
    return usleep(usec);
#elif HAVE_SLEEP
    Sleep(usec / 1000);
    return 0;
#else
    return AVERROR(ENOSYS);
#endif
}
