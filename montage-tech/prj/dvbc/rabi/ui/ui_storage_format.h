/****************************************************************************

****************************************************************************/
#ifndef __UI_STORAGE_FORMAT_H__
#define __UI_STORAGE_FORMAT_H__

/* coordinate */
#define STORAGE_FORMAT_ITEM_CNT      6
#define STORAGE_FORMAT_ITEM_X        60
#define STORAGE_FORMAT_ITEM_Y        60
#define STORAGE_FORMAT_ITEM_LW       ((SYS_RIGHT_CONT_W-2*STORAGE_FORMAT_ITEM_X)/3)
#define STORAGE_FORMAT_ITEM_RW       ((SYS_RIGHT_CONT_W-2*STORAGE_FORMAT_ITEM_X)/3*2)
#define STORAGE_FORMAT_ITEM_H        COMM_ITEM_H
#define STORAGE_FORMAT_ITEM_V_GAP    16

#define STORAGE_FORMAT_TEXT_X        ((SYS_RIGHT_CONT_W - STORAGE_FORMAT_TEXT_W) / 2)
#define STORAGE_FORMAT_TEXT_Y        (STORAGE_FORMAT_ITEM_Y + STORAGE_FORMAT_ITEM_CNT*(STORAGE_FORMAT_ITEM_V_GAP+STORAGE_FORMAT_ITEM_H))
#define STORAGE_FORMAT_TEXT_W       200
#define STORAGE_FORMAT_TEXT_H       50

/*confirm format dlg container*/
#define STORAGE_FORMAT_CONFIRM_CONT_X       ((SYS_RIGHT_CONT_W - STORAGE_FORMAT_CONFIRM_CONT_W) / 2)
#define STORAGE_FORMAT_CONFIRM_CONT_Y       ((COMM_BG_H - STORAGE_FORMAT_CONFIRM_CONT_H) / 2)
#define STORAGE_FORMAT_CONFIRM_CONT_W       400
#define STORAGE_FORMAT_CONFIRM_CONT_H       200
/*dlg content*/
#define STORAGE_FORMAT_CONFIRM_CONTENT_X    ((STORAGE_FORMAT_CONFIRM_CONT_W - STORAGE_FORMAT_CONFIRM_CONTENT_W) / 2)
#define STORAGE_FORMAT_CONFIRM_CONTENT_Y    30
#define STORAGE_FORMAT_CONFIRM_CONTENT_W    (STORAGE_FORMAT_CONFIRM_CONT_W - 60)
#define STORAGE_FORMAT_CONFIRM_CONTENT_H    80
/*dlg fat button*/
#define STORAGE_FORMAT_CONFIRM_FAT_X       60
#define STORAGE_FORMAT_CONFIRM_FAT_Y       (STORAGE_FORMAT_CONFIRM_CONT_H - STORAGE_FORMAT_CONFIRM_FAT_H - 40)
#define STORAGE_FORMAT_CONFIRM_FAT_W       80
#define STORAGE_FORMAT_CONFIRM_FAT_H       30
/*dlg ntfs button*/
#define STORAGE_FORMAT_CONFIRM_NTFS_X       (STORAGE_FORMAT_CONFIRM_FAT_X + STORAGE_FORMAT_CONFIRM_FAT_W + STORAGE_FORMAT_CONFIRM_H_GAP)
#define STORAGE_FORMAT_CONFIRM_NTFS_Y       STORAGE_FORMAT_CONFIRM_FAT_Y
#define STORAGE_FORMAT_CONFIRM_NTFS_W       STORAGE_FORMAT_CONFIRM_FAT_W
#define STORAGE_FORMAT_CONFIRM_NTFS_H       STORAGE_FORMAT_CONFIRM_FAT_H
/*dlg cancel button*/
#define STORAGE_FORMAT_CONFIRM_CANCEL_X     (STORAGE_FORMAT_CONFIRM_NTFS_X + STORAGE_FORMAT_CONFIRM_NTFS_W + STORAGE_FORMAT_CONFIRM_H_GAP)
#define STORAGE_FORMAT_CONFIRM_CANCEL_Y     STORAGE_FORMAT_CONFIRM_FAT_Y
#define STORAGE_FORMAT_CONFIRM_CANCEL_W     STORAGE_FORMAT_CONFIRM_FAT_W
#define STORAGE_FORMAT_CONFIRM_CANCEL_H     STORAGE_FORMAT_CONFIRM_FAT_H

#define STORAGE_FORMAT_CONFIRM_H_GAP 20

/* others */

RET_CODE open_storage_format(u32 para1, u32 para2);

#endif