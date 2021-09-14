/*******************************************************************************
 * ztuDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: ztuDef.h $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#ifndef _ZTU_DEF_H_
#define _ZTU_DEF_H_ 1

/**
 * @file ztuDef.h
 * @brief gdump Definitions
 */

/**
 * @defgroup gdump Defintions
 * @{ 
 */

/**
 * @brief File Format Type
 */
#define ZTU_STR_FILE_TYPE_CONTROLFILE     "control"
#define ZTU_STR_FILE_TYPE_LOG             "log"
#define ZTU_STR_FILE_TYPE_DATAFILE        "data"
#define ZTU_STR_FILE_TYPE_PROPERTY        "property"
#define ZTU_STR_FILE_TYPE_BACKUP          "backup"

typedef enum
{
    ZTU_FILE_TYPE_INVALID = 0,
    ZTU_FILE_TYPE_CONTROLFILE,
    ZTU_FILE_TYPE_LOG,
    ZTU_FILE_TYPE_DATAFILE,
    ZTU_FILE_TYPE_PROPERTY,
    ZTU_FILE_TYPE_BACKUP
} ztuFileType;

/**
 * @brief Control File Section
 */ 
#define ZTU_STR_CTRL_SECTION_ALL    "all"
#define ZTU_STR_CTRL_SECTION_SYS    "sys"
#define ZTU_STR_CTRL_SECTION_LOG    "log"
#define ZTU_STR_CTRL_SECTION_DB     "db"
#define ZTU_STR_CTRL_SECTION_BACKUP "backup"

#define ZTU_CTRL_SECTION_SYS     0x01
#define ZTU_CTRL_SECTION_LOG     0x02
#define ZTU_CTRL_SECTION_DB      0x04
#define ZTU_CTRL_SECTION_BACKUP  0x08
#define ZTU_CTRL_SECTION_ALL     0x0F

/**
 * @brief Incremnetal Backup File Body (page section) 
 */ 
#define ZTU_STR_INC_BACKUP_BODY_ALL     "all"
#define ZTU_STR_INC_BACKUP_BODY_HEADER  "header"
#define ZTU_STR_INC_BACKUP_BODY_NONE    "none"

#define ZTU_FILE_TYPE_STRING_MAX_LENGTH  (20)

typedef struct gDumpArguments
{
    ztuFileType      mFileType;
    stlInt8          mCtrlSection;
    stlBool          mDataHeader;
    stlInt64         mNum;
    stlInt64         mOffset;
    stlInt64         mFetchCnt;
    stlInt8          mDumpBody;
    stlUInt16        mTbsId;
    stlBool          mSilent;
    stlChar          mFileName[STL_MAX_FILE_PATH_LENGTH];
} ztuArguments;

/** @} */

#endif /* _ZTU_DEF_H_ */
