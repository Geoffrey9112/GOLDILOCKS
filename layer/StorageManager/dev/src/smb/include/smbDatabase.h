/*******************************************************************************
 * smbDatabase.h
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


#ifndef _SMBDATABASE_H_
#define _SMBDATABASE_H_ 1

/**
 * @file smbDatabase.h
 * @brief Storage Manager Layer Database Internal Routines
 */

/**
 * @defgroup smbDatabase Database
 * @ingroup smbInternal
 * @{
 */

stlStatus smbCreateFirstDictRelation( smlStatement * aStatement,
                                      smlEnv       * aEnv );
stlStatus smbCreateSystemTablespace( smlStatement  * aStatement,
                                     knlPropertyID   aPropertyId,
                                     stlChar       * aName,
                                     stlInt32        aAttr,
                                     stlInt64        aExtSize,
                                     stlChar       * aDatafileName,
                                     smlEnv        * aEnv );

/** @} */
    
#endif /* _SMBDATABASE_H_ */
