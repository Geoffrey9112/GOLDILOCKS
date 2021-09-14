/*******************************************************************************
 * smtTbs.h
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


#ifndef _SMTTBS_H_
#define _SMTTBS_H_ 1

/**
 * @file smtTbs.h
 * @brief Storage Manager Layer Tablespace Component Internal Routines
 */

/**
 * @defgroup smtTbs Tablespace
 * @ingroup smtInternal
 * @{
 */

stlStatus smtFormatDatafile( smlStatement  * aStatement,
                             smlTbsId        aTbsId,
                             smlDatafileId   aDatafileId,
                             stlInt32        aReservedPageCount,
                             smlEnv        * aEnv );
/** @} */
    
#endif /* _SMTTBS_H_ */
