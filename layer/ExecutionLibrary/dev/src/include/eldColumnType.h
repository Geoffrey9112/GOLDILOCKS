/*******************************************************************************
 * eldColumnType.h
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


#ifndef _ELD_COLUMN_TYPE_H_
#define _ELD_COLUMN_TYPE_H_ 1

/**
 * @file eldColumnType.h
 * @brief Column Type Dictionary 관리 루틴
 */


/**
 * @defgroup eldColumnType Column 의 Data Type Descriptor 관리 루틴
 * @internal 
 * @ingroup eldDesc
 * @{
 */

/*
 * Character String 유형의 컬럼
 */

stlInt64  eldGetCharacterLength( dtlStringLengthUnit aLengthUnit,
                                 stlInt64            aLength );

stlInt64  eldGetOctetLength( dtlStringLengthUnit   aLengthUnit,
                             stlInt64              aLength,
                             ellEnv              * aEnv );
    


/*
 * 모든 타입을 위한 descriptor 추가 로직
 */

stlStatus eldInsertColumnTypeDesc( smlTransId            aTransID,
                                   smlStatement        * aStmt,
                                   stlInt64            * aOwnerID,
                                   stlInt64            * aSchemaID,
                                   stlInt64            * aTableID,
                                   stlInt64            * aColumnID,
                                   stlChar             * aNormalTypeName,
                                   stlInt32            * aDataTypeID,
                                   dtlStringLengthUnit * aLengthUnit,
                                   stlInt64            * aCharMaxLength,
                                   stlInt64            * aCharOctetLength,
                                   stlInt32            * aNumPrec,
                                   stlInt32            * aNumPrecRadix,
                                   stlInt32            * aNumScale,
                                   stlChar             * aDeclaredTypeName,
                                   stlInt64            * aDeclaredPrecOrLen,
                                   stlInt64            * aDeclaredPrecOrScale,
                                   stlInt32            * aDatetimePrec,
                                   stlChar             * aIntervalType,
                                   stlInt32            * aIntervalTypeID,
                                   stlInt32            * aIntervalPrec,
                                   stlInt64              aPhysicalLength,
                                   ellEnv              * aEnv );

/*
 * 모든 타입을 위한 descriptor 변경 로직
 */


stlStatus eldModifyColumnTypeDesc( smlTransId            aTransID,
                                   smlStatement        * aStmt,
                                   stlInt64              aColumnID,
                                   stlChar             * aNormalTypeName,
                                   stlInt32            * aDataTypeID,
                                   dtlStringLengthUnit * aLengthUnit,
                                   stlInt64            * aCharMaxLength,
                                   stlInt64            * aCharOctetLength,
                                   stlInt32            * aNumPrec,
                                   stlInt32            * aNumPrecRadix,
                                   stlInt32            * aNumScale,
                                   stlChar             * aDeclaredTypeName,
                                   stlInt64            * aDeclaredPrecOrLen,
                                   stlInt64            * aDeclaredPrecOrScale,
                                   stlInt32            * aDatetimePrec,
                                   stlChar             * aIntervalType,
                                   stlInt32            * aIntervalTypeID,
                                   stlInt32            * aIntervalPrec,
                                   stlInt64              aPhysicalLength,
                                   ellEnv              * aEnv );

/** @} eldColumnType */

#endif /* _ELD_COLUMN_TYPE_H_ */
