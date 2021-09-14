/*******************************************************************************
 * smdpfhDef.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _SMDPFHDEF_H_
#define _SMDPFHDEF_H_ 1


/**
 * @file smdpfhDef.h
 * @brief Storage Manager Layer Fixed Table Internal Definition
 */

/**
 * @addtogroup smdpfh
 * @{
 */

typedef struct smdpfhIterator
{
    smiIterator                mCommon;
    stlInt32                   mPathCtrlSize;
    void                     * mPathCtrl;
    knlOpenFxTableCallback     mOpenFunc;
    knlCloseFxTableCallback    mCloseFunc;
    knlBuildFxRecordCallback   mBuildFunc;
} smdpfhIterator;


/** @} */
    
#endif /* _SMDDEF_H_ */
