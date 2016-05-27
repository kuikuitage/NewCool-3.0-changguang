/**************************************************************************
  * Description
  *   This file define all basic data types used in this software tree.
  *
  * Remarks
  *   None.
  *
  * Bugs
  *   None.
  *
  * TODO
  *     * None
  *
  * History
  *   <table>
  *   \Author     Date         Change Description
  *   ----------  -----------  -------------------
  *   </table>
  *
  **************************************************************************/
#ifndef __SYS_TYPES_H__
#define __SYS_TYPES_H__

#ifdef WIN32
//#include <basetsd.h>
#endif

/* Signed integer, weight is 8 bits. In most case it equal to char. */
typedef signed char         s8;
/* Unsigned integer, weight is 8 bits. In most case it equal to unsigned char. */
typedef unsigned char       u8;

/* Signed integer, weight is 16 bits. In most case it equal to short. */
typedef signed short        s16;
/* Unsigned integer, weight is 16 bits. In most case it equal to unsigned short. */
typedef unsigned short      u16;


/* Signed integer, weight is 32 bits. In most case it equal to long. */
typedef signed long         s32;
/* Unsigned integer, weight is 32 bits. In most case it equal to unsigned long. */
typedef unsigned long       u32;

/* Boolean type, the value should be FALSE and TRUE.
   Recommand replace BOOL by RET_CODE as return value. */
typedef int                 BOOL;
#ifndef FALSE
#define FALSE               (0)             /* Boolean false */
#endif
#ifndef TRUE
#define TRUE                (1)             /* Boolean false */
#endif

#ifndef INVALID
#define INVALID             (~0)
#endif

/* Null pointer type */
#ifndef NULL
#define NULL                ((void *)0)
#endif

/* Return code type. Please reference return code macro for the values. */
typedef s32                 RET_CODE;
#define SUCCESS             ((s32) 0)       /* Success return */
#define ERR_FAILURE         ((s32)-1)       /* Fail for common reason */
#define ERR_TIMEOUT         ((s32)-2)       /* Fail for waiting timeout */
#define ERR_PARAM           ((s32)-3)       /* Fail for function param invalid */
#define ERR_STATUS          ((s32)-4)       /* Fail for module status invalid */
#define ERR_BUSY            ((s32)-5)       /* Fail for module busy */
#define ERR_NO_MEM          ((s32)-6)       /* Fail for no enough memory */
#define ERR_NO_RSRC         ((s32)-7)       /* Fail for no enough resource */
#define ERR_HARDWARE        ((s32)-8)       /* Fail for hardware error */
#define ERR_NOFEATURE       ((s32)-9)       /* Fail for feature not support */


/******************************************************************************
* register read/wirte operation
******************************************************************************/
#define REG8(port)          (*((volatile u8*)(port)))
#define REG16(port)         (*((volatile u16*)(port)))
#define REG32(port)         (*((volatile u32*)(port)))

#endif //__SYS_TYPES_H__
