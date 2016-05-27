/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2012 Live Networks, Inc.  All rights reserved.
// Common routines for opening/closing named input files
// Implementation

#include "InputFile.hh"
#include <string.h>
#ifndef __LINUX__
#include <stdlib.h>
#endif

#ifdef __LINUX__
FILE* OpenInputFile(UsageEnvironment& env, char const* fileName)
{
  FILE* fid;

  // Check for a special case file name: "stdin"
  if (strcmp(fileName, "stdin") == 0) {
    fid = stdin;
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
    _setmode(_fileno(stdin), _O_BINARY); // convert to binary mode
#endif
  } else {
    fid = fopen(fileName, "rb");
    if (fid == NULL) {
      env.setResultMsg("unable to open file \"",fileName, "\"");
    }
  }

  return fid;
}

void CloseInputFile(FILE* fid) {
  // Don't close 'stdin', in case we want to use it again later.
  if (fid != NULL && fid != stdin) fclose(fid);
}

u_int64_t GetFileSize(char const* fileName, FILE* fid) {
  u_int64_t fileSize = 0; // by default

  if (fid != stdin) {
#if !defined(_WIN32_WCE)
    if (fileName == NULL) {
#endif
      if (fid != NULL && SeekFile64(fid, 0, SEEK_END) >= 0) {
	fileSize = (u_int64_t)TellFile64(fid);
	if (fileSize == (u_int64_t)-1) fileSize = 0; // TellFile64() failed
	SeekFile64(fid, 0, SEEK_SET);
      }
#if !defined(_WIN32_WCE)
    } else {
      struct stat sb;
      if (stat(fileName, &sb) == 0) {
	fileSize = sb.st_size;
      }
    }
#endif
  }

  return fileSize;
}

int64_t SeekFile64(FILE *fid, int64_t offset, int whence) {
  if (fid == NULL) return -1;

  clearerr(fid);
  fflush(fid);
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
  return _lseeki64(_fileno(fid), offset, whence) == (int64_t)-1 ? -1 : 0;
#else
#if defined(_WIN32_WCE)
  return fseek(fid, (long)(offset), whence);
#else
  return fseeko(fid, (off_t)(offset), whence);
#endif
#endif
}

int64_t TellFile64(FILE *fid) {
  if (fid == NULL) return -1;

  clearerr(fid);
  fflush(fid);
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
  return _telli64(_fileno(fid));
#else
#if defined(_WIN32_WCE)
  return ftell(fid);
#else
  return ftello(fid);
#endif
#endif
}

Boolean FileIsSeekable(FILE *fid) {
  if (SeekFile64(fid, 1, SEEK_CUR) < 0) {
    return False;
  }

  SeekFile64(fid, -1, SEEK_CUR); // seek back to where we were
  return True;
}
#else
UFILE* OpenInputFile(UsageEnvironment& env, char const* fileName)
{
  UFILE* fid;
  tchar_t p_filename_uni[MAX_FILE_NAME_LEN] = {0};
  if (strlen(fileName) >= MAX_FILE_NAME_LEN)
  {
    env.setResultMsg("fileName too long \n");
  }
  fid = (UFILE*)malloc(sizeof(UFILE));
  if (fid == NULL)
  {
    env.setResultMsg("fid malloc failed\n");
    return NULL;
  }
  memset(fid, 0, sizeof(ufs_file_t));
  ufs_asc2uni((u8 *)fileName, p_filename_uni);
  if (ufs_open(fid, p_filename_uni, UFS_READ) != FR_OK)
  {
    env.setResultMsg("open %s error\n");
    free(fid);
    return NULL;
  }

  return fid;
}

void CloseInputFile(UFILE* fid) {
  if (fid != NULL )
  {
    ufs_close(fid);
  }
}

u_int64_t GetFileSize(char const* fileName, UFILE* fid) {
  u_int64_t fileSize = 0; // by default

  if (fileName == NULL)
  {
      if (fid != NULL && SeekFile64(fid, 0, SEEK_END) >= 0)
      {
          fileSize = (u_int64_t)TellFile64(fid);
          if (fileSize == (u_int64_t)-1) fileSize = 0; // TellFile64() failed
          SeekFile64(fid, 0, SEEK_SET);
      }
  }
  else
  {
      struct stat sb;
      if (stat(fileName, &sb) == 0)
      {
          fileSize = sb.st_size;
      }
  }

  return fileSize;
}

int64_t SeekFile64(UFILE *fid, int64_t offset, int whence) {
  if (fid == NULL) return -1;

  //clearerr(fid);
  ufs_flush(fid);
  return ufs_lseek(fid, (s64)(offset), (u32)whence);
}

int64_t TellFile64(UFILE *fid) {
  if (fid == NULL) return -1;
  uint64_t offset;

  //clearerr(fid);
  ufs_flush(fid);

  ufs_tell(fid, &offset);

  return offset;
}

Boolean FileIsSeekable(UFILE *fid) {
  if (SeekFile64(fid, 1, SEEK_CUR) < 0) {
    return False;
  }

  SeekFile64(fid, -1, SEEK_CUR); // seek back to where we were
  return True;
}

#endif
