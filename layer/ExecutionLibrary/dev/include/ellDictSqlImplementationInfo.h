/*******************************************************************************
 * ellDictSqlImplementationInfo.h
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


#ifndef _ELL_DICT_SQL_IMPLEMENTAION_INFO_H_
#define _ELL_DICT_SQL_IMPLEMENTAION_INFO_H_ 1

/**
 * @file ellDictSqlImplementationInfo.h
 * @brief SqlImplementationInfo Dictionary Management
 */

/**
 * @addtogroup ellSqlImplementationInfo
 * @{
 */


stlStatus ellInsertSqlImplementationInfoDesc( smlTransId     aTransID,
                                              smlStatement * aStmt,
                                              stlChar      * aID,
                                              stlChar      * aName,
                                              stlInt32       aIntegerValue,
                                              stlChar      * aCharacterValue,
                                              stlChar      * aComments,
                                              ellEnv       * aEnv );

/** @} ellSqlImplementationInfo */



#endif /* _ELL_DICT_SQL_IMPLEMENTAION_INFO_H_ */
