/*******************************************************************************
 * ellDictColumnType.h
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


#ifndef _ELLDICTCOLUMNTYPE_H_
#define _ELLDICTCOLUMNTYPE_H_ 1

/**
 * @file ellDictColumnType.h
 * @brief Dictionary를 위한 Column Type 정보 설정 루틴
 */





/**
 * @addtogroup ellColumnType
 * @{
 */


/*
 * Character String 유형의 컬럼
 */

stlStatus ellInsertCharColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aOwnerID,
                                       stlInt64            aSchemaID,
                                       stlInt64            aTableID,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       dtlStringLengthUnit aLengthUnit,
                                       stlInt64            aDeclaredMaxLength,
                                       ellEnv            * aEnv );

stlStatus ellModifyCharColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       dtlStringLengthUnit aLengthUnit,
                                       stlInt64            aDeclaredMaxLength,
                                       ellEnv            * aEnv );

stlStatus ellInsertVarcharColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aOwnerID,
                                          stlInt64            aSchemaID,
                                          stlInt64            aTableID,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          dtlStringLengthUnit aLengthUnit,
                                          stlInt64            aDeclaredMaxLength,
                                          ellEnv            * aEnv );

stlStatus ellModifyVarcharColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          dtlStringLengthUnit aLengthUnit,
                                          stlInt64            aDeclaredMaxLength,
                                          ellEnv            * aEnv );

stlStatus ellInsertLongVarcharColumnTypeDesc( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              stlInt64            aOwnerID,
                                              stlInt64            aSchemaID,
                                              stlInt64            aTableID,
                                              stlInt64            aColumnID,
                                              stlChar           * aDeclaredTypeName,
                                              ellEnv            * aEnv );

stlStatus ellModifyLongVarcharColumnTypeDesc( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              stlInt64            aColumnID,
                                              stlChar           * aDeclaredTypeName,
                                              ellEnv            * aEnv );

/*
 * Binary String 유형의 컬럼
 */

stlStatus ellInsertBinaryColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aOwnerID,
                                         stlInt64            aSchemaID,
                                         stlInt64            aTableID,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         stlInt64            aDeclaredMaxLength,
                                         ellEnv            * aEnv );

stlStatus ellModifyBinaryColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         stlInt64            aDeclaredMaxLength,
                                         ellEnv            * aEnv );

stlStatus ellInsertVarBinaryColumnTypeDesc( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            stlInt64            aOwnerID,
                                            stlInt64            aSchemaID,
                                            stlInt64            aTableID,
                                            stlInt64            aColumnID,
                                            stlChar           * aDeclaredTypeName,
                                            stlInt64            aDeclaredMaxLength,
                                            ellEnv            * aEnv );

stlStatus ellModifyVarBinaryColumnTypeDesc( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            stlInt64            aColumnID,
                                            stlChar           * aDeclaredTypeName,
                                            stlInt64            aDeclaredMaxLength,
                                            ellEnv            * aEnv );

stlStatus ellInsertLongVarBinaryColumnTypeDesc( smlTransId          aTransID,
                                                smlStatement      * aStmt,
                                                stlInt64            aOwnerID,
                                                stlInt64            aSchemaID,
                                                stlInt64            aTableID,
                                                stlInt64            aColumnID,
                                                stlChar           * aDeclaredTypeName,
                                                ellEnv            * aEnv );

stlStatus ellModifyLongVarBinaryColumnTypeDesc( smlTransId          aTransID,
                                                smlStatement      * aStmt,
                                                stlInt64            aColumnID,
                                                stlChar           * aDeclaredTypeName,
                                                ellEnv            * aEnv );


/*
 * Exact Numeric 유형의 컬럼
 */

stlStatus ellInsertSmallintColumnTypeDesc( smlTransId          aTransID,
                                           smlStatement      * aStmt,
                                           stlInt64            aOwnerID,
                                           stlInt64            aSchemaID,
                                           stlInt64            aTableID,
                                           stlInt64            aColumnID,
                                           stlChar           * aDeclaredTypeName,
                                           ellEnv            * aEnv );

stlStatus ellModifySmallintColumnTypeDesc( smlTransId          aTransID,
                                           smlStatement      * aStmt,
                                           stlInt64            aColumnID,
                                           stlChar           * aDeclaredTypeName,
                                           ellEnv            * aEnv );

stlStatus ellInsertIntegerColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aOwnerID,
                                          stlInt64            aSchemaID,
                                          stlInt64            aTableID,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          ellEnv            * aEnv );

stlStatus ellModifyIntegerColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          ellEnv            * aEnv );

stlStatus ellInsertBigintColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aOwnerID,
                                         stlInt64            aSchemaID,
                                         stlInt64            aTableID,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         ellEnv            * aEnv );

stlStatus ellModifyBigintColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         ellEnv            * aEnv );

stlStatus ellInsertFloatColumnTypeDesc( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aOwnerID,
                                        stlInt64            aSchemaID,
                                        stlInt64            aTableID,
                                        stlInt64            aColumnID,
                                        stlChar           * aDeclaredTypeName,
                                        stlInt32            aDeclaredRadix,
                                        stlInt32            aDeclaredDecimalPrec,
                                        stlInt32            aDeclaredScale,
                                        ellEnv            * aEnv );

stlStatus ellInsertNumericColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aOwnerID,
                                          stlInt64            aSchemaID,
                                          stlInt64            aTableID,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          stlInt32            aDeclaredRadix,
                                          stlInt32            aDeclaredDecimalPrec,
                                          stlInt32            aDeclaredScale,
                                          ellEnv            * aEnv );

stlStatus ellModifyFloatColumnTypeDesc( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aColumnID,
                                        stlChar           * aDeclaredTypeName,
                                        stlInt32            aDeclaredRadix,
                                        stlInt32            aDeclaredDecimalPrec,
                                        stlInt32            aDeclaredScale,
                                        ellEnv            * aEnv );

stlStatus ellModifyNumericColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          stlInt32            aDeclaredRadix,
                                          stlInt32            aDeclaredDecimalPrec,
                                          stlInt32            aDeclaredScale,
                                          ellEnv            * aEnv );

stlStatus ellInsertDecimalColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aOwnerID,
                                          stlInt64            aSchemaID,
                                          stlInt64            aTableID,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          stlInt32            aDeclaredRadix,
                                          stlInt32            aDeclaredDecimalPrec,
                                          stlInt32            aDeclaredScale,
                                          ellEnv            * aEnv );

stlStatus ellModifyDecimalColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          stlInt32            aDeclaredRadix,
                                          stlInt32            aDeclaredDecimalPrec,
                                          stlInt32            aDeclaredScale,
                                          ellEnv            * aEnv );

/*
 * Approximate Numeric 유형의 컬럼
 */

stlStatus ellInsertRealColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aOwnerID,
                                       stlInt64            aSchemaID,
                                       stlInt64            aTableID,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       ellEnv            * aEnv );

stlStatus ellModifyRealColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       ellEnv            * aEnv );

stlStatus ellInsertDoubleColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aOwnerID,
                                         stlInt64            aSchemaID,
                                         stlInt64            aTableID,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         ellEnv            * aEnv );

stlStatus ellModifyDoubleColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         ellEnv            * aEnv );

/*
 * Boolean 유형의 컬럼
 */

stlStatus ellInsertBooleanColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aOwnerID,
                                          stlInt64            aSchemaID,
                                          stlInt64            aTableID,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          ellEnv            * aEnv );

stlStatus ellModifyBooleanColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          ellEnv            * aEnv );

/*
 * DateTime 유형의 컬럼
 */

stlStatus ellInsertDateColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aOwnerID,
                                       stlInt64            aSchemaID,
                                       stlInt64            aTableID,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       ellEnv            * aEnv );

stlStatus ellModifyDateColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       ellEnv            * aEnv );

stlStatus ellInsertTimeColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aOwnerID,
                                       stlInt64            aSchemaID,
                                       stlInt64            aTableID,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       stlInt32            aDeclaredFractionPrec,
                                       ellEnv            * aEnv );

stlStatus ellModifyTimeColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       stlInt32            aDeclaredFractionPrec,
                                       ellEnv            * aEnv );

stlStatus ellInsertTimestampColumnTypeDesc( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            stlInt64            aOwnerID,
                                            stlInt64            aSchemaID,
                                            stlInt64            aTableID,
                                            stlInt64            aColumnID,
                                            stlChar           * aDeclaredTypeName,
                                            stlInt32            aDeclaredFractionPrec,
                                            ellEnv            * aEnv );

stlStatus ellModifyTimestampColumnTypeDesc( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            stlInt64            aColumnID,
                                            stlChar           * aDeclaredTypeName,
                                            stlInt32            aDeclaredFractionPrec,
                                            ellEnv            * aEnv );

stlStatus ellInsertTimeWithZoneColumnTypeDesc( smlTransId          aTransID,
                                               smlStatement      * aStmt,
                                               stlInt64            aOwnerID,
                                               stlInt64            aSchemaID,
                                               stlInt64            aTableID,
                                               stlInt64            aColumnID,
                                               stlChar           * aDeclaredTypeName,
                                               stlInt32            aDeclaredFractionPrec,
                                               ellEnv            * aEnv );

stlStatus ellModifyTimeWithZoneColumnTypeDesc( smlTransId          aTransID,
                                               smlStatement      * aStmt,
                                               stlInt64            aColumnID,
                                               stlChar           * aDeclaredTypeName,
                                               stlInt32            aDeclaredFractionPrec,
                                               ellEnv            * aEnv );

stlStatus ellInsertTimestampWithZoneColumnTypeDesc( smlTransId          aTransID,
                                                    smlStatement      * aStmt,
                                                    stlInt64            aOwnerID,
                                                    stlInt64            aSchemaID,
                                                    stlInt64            aTableID,
                                                    stlInt64            aColumnID,
                                                    stlChar           * aDeclaredTypeName,
                                                    stlInt32            aDeclaredFractionPrec,
                                                    ellEnv            * aEnv );

stlStatus ellModifyTimestampWithZoneColumnTypeDesc( smlTransId          aTransID,
                                                    smlStatement      * aStmt,
                                                    stlInt64            aColumnID,
                                                    stlChar           * aDeclaredTypeName,
                                                    stlInt32            aDeclaredFractionPrec,
                                                    ellEnv            * aEnv );


/*
 * Interval 유형의 컬럼
 */

stlStatus ellInsertIntervalYearToMonthColumnTypeDesc( smlTransId          aTransID,
                                                      smlStatement      * aStmt,
                                                      stlInt64            aOwnerID,
                                                      stlInt64            aSchemaID,
                                                      stlInt64            aTableID,
                                                      stlInt64            aColumnID,
                                                      stlChar           * aDeclaredTypeName,
                                                      dtlIntervalIndicator aIndicator,
                                                      stlInt32            aDeclaredStartPrec,
                                                      stlInt32            aDeclaredEndPrec,
                                                      ellEnv            * aEnv );

stlStatus ellModifyIntervalYearToMonthColumnTypeDesc( smlTransId          aTransID,
                                                      smlStatement      * aStmt,
                                                      stlInt64            aColumnID,
                                                      stlChar           * aDeclaredTypeName,
                                                      dtlIntervalIndicator aIndicator,
                                                      stlInt32            aDeclaredStartPrec,
                                                      stlInt32            aDeclaredEndPrec,
                                                      ellEnv            * aEnv );

stlStatus ellInsertIntervalDayToSecondColumnTypeDesc( smlTransId          aTransID,
                                                      smlStatement      * aStmt,
                                                      stlInt64            aOwnerID,
                                                      stlInt64            aSchemaID,
                                                      stlInt64            aTableID,
                                                      stlInt64            aColumnID,
                                                      stlChar           * aDeclaredTypeName,
                                                      dtlIntervalIndicator aIndicator,
                                                      stlInt32            aDeclaredStartPrec,
                                                      stlInt32            aDeclaredEndPrec,
                                                      ellEnv            * aEnv );

stlStatus ellModifyIntervalDayToSecondColumnTypeDesc( smlTransId          aTransID,
                                                      smlStatement      * aStmt,
                                                      stlInt64            aColumnID,
                                                      stlChar           * aDeclaredTypeName,
                                                      dtlIntervalIndicator aIndicator,
                                                      stlInt32            aDeclaredStartPrec,
                                                      stlInt32            aDeclaredEndPrec,
                                                      ellEnv            * aEnv );

/*
 * ROWID 컬럼
 */

stlStatus ellInsertRowIdColumnTypeDesc( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aOwnerID,
                                        stlInt64            aSchemaID,
                                        stlInt64            aTableID,
                                        stlInt64            aColumnID,
                                        stlChar           * aDeclaredTypeName,
                                        ellEnv            * aEnv );

stlStatus ellModifyRowIdColumnTypeDesc( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aColumnID,
                                        stlChar           * aDeclaredTypeName,
                                        ellEnv            * aEnv );

/** @} ellColumnType */

#endif /* _ELLDICTCOLUMNTYPE_H_ */
