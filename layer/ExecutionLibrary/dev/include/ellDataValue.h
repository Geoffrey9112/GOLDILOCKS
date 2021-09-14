/*******************************************************************************
 * ellDataValue.h
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


#ifndef _ELL_DATA_VALUE_H_
#define _ELL_DATA_VALUE_H_ 1

/**
 * @file ellDataValue.h
 * @brief Data Value Management
 */


/*
 * Table Read/Write Value List
 */

knlValueBlockList * ellFindColumnValueList( knlValueBlockList * aTableValueList,
                                            stlInt64            aTableID,
                                            stlInt32            aColumnIdx );

knlValueBlockList * ellFindColumnValueListWithColumnHandle( knlValueBlockList * aTableValueList,
                                                            stlInt32            aColumnIdx );

/*
 * for INDEX Insert/Delete Value list
 */

stlStatus ellAllocIndexValueListForCREATE( smlTransId           aTransID,
                                           smlScn               aViewSCN,
                                           stlInt32             aKeyColumnCnt,
                                           stlInt64           * aKeyArrayColumnID,
                                           stlInt32           * aNewKeyColumnPos,
                                           ellAddColumnDesc   * aNewColumnDesc,
                                           knlRegionMem       * aRegionMem,
                                           knlValueBlockList ** aIndexInsertValueList,
                                           ellEnv             * aEnv );

stlStatus ellShareIndexValueList( knlValueBlockList  * aTableReadValueList,
                                  knlValueBlockList  * aTableWriteValueList,
                                  ellDictHandle      * aIndexDictHandle,
                                  knlRegionMem       * aRegionMem,
                                  knlValueBlockList ** aIndexDeleteValueList,
                                  knlValueBlockList ** aIndexInsertValueList,
                                  ellEnv             * aEnv );

stlStatus ellShareIndexValueListForINSERT( ellDictHandle      * aTableDictHandle,
                                           knlValueBlockList  * aTableWriteValueList,
                                           ellDictHandle      * aIndexDictHandle,
                                           knlRegionMem       * aRegionMem,
                                           knlValueBlockList ** aIndexInsertValueList,
                                           ellEnv             * aEnv );

stlStatus ellShareIndexValueListForDELETE( ellDictHandle      * aTableDictHandle,
                                           knlValueBlockList  * aTableReadValueList,
                                           ellDictHandle      * aIndexDictHandle,
                                           knlRegionMem       * aRegionMem,
                                           knlValueBlockList ** aIndexDeleteValueList,
                                           ellEnv             * aEnv );

stlStatus ellAllocIndexValueListForSELECT( ellDictHandle      * aIndexDictHandle,
                                           stlInt32             aUseKeyCnt,
                                           knlRegionMem       * aRegionMem,
                                           stlInt32             aBlockCnt,
                                           knlValueBlockList ** aIndexSelectValueList,
                                           ellEnv             * aEnv );

/*
 * for Index Scan
 */
stlStatus ellSetIndexColumnOrder( knlValueBlockList   * aValueBlock,
                                  ellDictHandle       * aIndexHandle,
                                  ellEnv              * aEnv );

#endif /* _ELL_DATA_VALUE_H_ */
