/*******************************************************************************
 * qlvSelect.h
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

#ifndef _QLVSELECT_H_
#define _QLVSELECT_H_ 1

/**
 * @file qlvSelect.h
 * @brief SQL Processor Layer Validation Select Definition 
 */

#include <qlDef.h>


/**
 * @addtogroup qlvSelect
 * @{
 */

stlStatus qlvValidate4ResultSetDesc( qllStatement  * aSQLStmt,
                                     qlpSelect     * aSelectTree,
                                     qlvInitSelect * aInitPlan,
                                     qlvStmtInfo   * aStmtInfo,
                                     qllEnv        * aEnv );
                                  
stlBool qlvIsUpdatableQuery( qlvInitSelect * aInitPlan );

stlBool qlvHasUpdatableColumnInFROM( qlvInitNode * aTableNode );

stlStatus qlvSetSelectResultCursorProperty( qlvStmtInfo   * aStmtInfo,
                                            qlvInitSelect * aInitPlan,
                                            qllEnv        * aEnv );

stlStatus qlvSetLockInfo4ForUpdate( qlvStmtInfo        * aStmtInfo,
                                    qlpUpdatableClause * aForUpdateTree,
                                    qlvInitSelect      * aInitPlan,
                                    qllEnv             * aEnv );

stlStatus qlvSetLockItemList4ForUpdateOnly( qlvStmtInfo        * aStmtInfo,
                                            qlvInitSelect      * aInitPlan,
                                            qllEnv             * aEnv );

stlStatus qlvSetCursorItemListByFROM( knlRegionMem       * aRegionMem,
                                      qlvInitNode        * aTableNode,
                                      qlvRefTableList    * aCursorItemList,
                                      qllEnv             * aEnv );
                                    
stlStatus qlvSetLockItemList4ForUpdateColumns( qlvStmtInfo   * aStmtInfo,
                                               qlpList       * aUpdateColumnList,
                                               qlvInitSelect * aInitPlan,
                                               qllEnv        * aEnv );


/** @} qlvSelect */

#endif /* _QLVSELECT_H_ */
