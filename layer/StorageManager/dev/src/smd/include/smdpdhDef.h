/*******************************************************************************
 * smdpdhDef.h
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


#ifndef _SMDPDHDEF_H_
#define _SMDPDHDEF_H_ 1


/**
 * @file smdpdhDef.h
 * @brief Storage Manager Layer Dump Table Internal Definition
 */

/**
 * @addtogroup smdpdh
 * @{
 */

typedef struct smdpdhIterator
{
    smdpfhIterator             mFxIterator;
    knlGetDumpObjectCallback   mGetDumpObj;
    void                     * mDumpHandle;
} smdpdhIterator;


/** @} */
    
#endif /* _SMDDEF_H_ */
