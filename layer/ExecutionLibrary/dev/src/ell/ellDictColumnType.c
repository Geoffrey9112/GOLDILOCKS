/*******************************************************************************
 * ellDictColumnType.c
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

/**
 * @file ellDictColumnType.c
 * @brief Dictionary를 위한 Column Type 정보를 설정 루틴
 */

#include <ell.h>
#include <elDef.h>

#include <eldt.h>
#include <eldc.h>
#include <eldDictionary.h>

#include <eldColumnType.h>

/**
 * @addtogroup ellColumnType 
 * @{
 */

/**********************************************************************
 * Character String 유형의 컬럼
 **********************************************************************/

/**
 * @brief CHARACTER 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : CHARACTER
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - CHAR
 *                <BR> - CHARACTER
 * @param[in]     aLengthUnit        길이의 단위
 *                <BR> 문자(CHARACTER) 단위인지 바이트(OCTET) 인지를 설정
 *                <BR> - DTL_STRING_LENGTH_UNIT_CHARACTERS
 *                <BR> - DTL_STRING_LENGTH_UNIT_OCTETS
 * @param[in]     aDeclaredMaxLength 사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_LENGTH_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
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
                                       ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCharLen = 0;
    stlInt64   sOctetLen = 0;
    stlInt64   sCalcLen = 0;
    stlInt64 * sDeclareLen = NULL;

    dtlStringLengthUnit sLengthUnit;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_CHAR].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_CHAR;

    /**
     * - 입력된 길이값 계산
     */
    
    if ( aDeclaredMaxLength == DTL_LENGTH_NA )
    {
        /* 사용자가 입력한 길이값이 없음 */
        sCalcLen = gDataTypeDefinition[DTL_TYPE_CHAR].mDefStringLength;
        sDeclareLen = NULL;
    }
    else
    {
        sCalcLen = aDeclaredMaxLength;
        sDeclareLen = & sCalcLen;
    }

    /**
     * - Octet Length 와 Char Length를 계산 
     */
    
    sCharLen = eldGetCharacterLength( aLengthUnit,  sCalcLen );
    sOctetLen = eldGetOctetLength( aLengthUnit,  sCalcLen, aEnv );

    if ( aLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
    {
        sLengthUnit = ellGetDbCharLengthUnit();
    }
    else
    {
        sLengthUnit = aLengthUnit;
    }

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,           /* OWNER_ID */
                                      & aSchemaID,          /* SCHEMA_ID */
                                      & aTableID,           /* TABLE_ID */
                                      & aColumnID,          /* COLUMN_ID */
                                      sNormalTypeName,      /* DATA_TYPE */
                                      & sDataTypeID,        /* DATA_TYPE_ID */
                                      & sLengthUnit,        /* STRING_LENGTH_UNIT */
                                      & sCharLen,           /* CHARACTER_MAXIMUM_LENGTH */
                                      & sOctetLen,          /* CHARACTER_OCTET_LENGTH */
                                      NULL,                 /* NUMERIC_PRECISION */
                                      NULL,                 /* NUMERIC_PRECISION_RADIX */
                                      NULL,                 /* NUMERIC_SCALE */
                                      aDeclaredTypeName,    /* DECLARED_DATA_TYPE */
                                      sDeclareLen,          /* DECLARED_NUMERIC_PRECISION */
                                      NULL,                 /* DECLARED_NUMERIC_SCALE */
                                      NULL,                 /* DATETIME_PRECISION */
                                      NULL,                 /* INTERVAL_TYPE */
                                      NULL,                 /* INTERVAL_TYPE_ID */
                                      NULL,                 /* INTERVAL_PRECISION */
                                      sOctetLen,            /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief CHARACTER 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : CHARACTER
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - CHAR
 *                <BR> - CHARACTER
 * @param[in]     aLengthUnit        길이의 단위
 *                <BR> 문자(CHARACTER) 단위인지 바이트(OCTET) 인지를 설정
 *                <BR> - DTL_STRING_LENGTH_UNIT_CHARACTERS
 *                <BR> - DTL_STRING_LENGTH_UNIT_OCTETS
 * @param[in]     aDeclaredMaxLength 사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_LENGTH_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyCharColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       dtlStringLengthUnit aLengthUnit,
                                       stlInt64            aDeclaredMaxLength,
                                       ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCharLen = 0;
    stlInt64   sOctetLen = 0;
    stlInt64   sCalcLen = 0;
    stlInt64 * sDeclareLen = NULL;

    dtlStringLengthUnit sLengthUnit;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_CHAR].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_CHAR;

    /**
     * - 입력된 길이값 계산
     */
    
    if ( aDeclaredMaxLength == DTL_LENGTH_NA )
    {
        /* 사용자가 입력한 길이값이 없음 */
        sCalcLen = gDataTypeDefinition[DTL_TYPE_CHAR].mDefStringLength;
        sDeclareLen = NULL;
    }
    else
    {
        sCalcLen = aDeclaredMaxLength;
        sDeclareLen = & sCalcLen;
    }

    /**
     * - Octet Length 와 Char Length를 계산 
     */
    
    sCharLen = eldGetCharacterLength( aLengthUnit,  sCalcLen );
    sOctetLen = eldGetOctetLength( aLengthUnit,  sCalcLen, aEnv );

    if ( aLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
    {
        sLengthUnit = ellGetDbCharLengthUnit();
    }
    else
    {
        sLengthUnit = aLengthUnit;
    }
    
    /**
     * - 컬럼의 data type descriptor 변경
     */
    
    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,            /* COLUMN_ID */
                                      sNormalTypeName,      /* DATA_TYPE */
                                      & sDataTypeID,        /* DATA_TYPE_ID */
                                      & sLengthUnit,        /* STRING_LENGTH_UNIT */
                                      & sCharLen,           /* CHARACTER_MAXIMUM_LENGTH */
                                      & sOctetLen,          /* CHARACTER_OCTET_LENGTH */
                                      NULL,                 /* NUMERIC_PRECISION */
                                      NULL,                 /* NUMERIC_PRECISION_RADIX */
                                      NULL,                 /* NUMERIC_SCALE */
                                      aDeclaredTypeName,    /* DECLARED_DATA_TYPE */
                                      sDeclareLen,          /* DECLARED_NUMERIC_PRECISION */
                                      NULL,                 /* DECLARED_NUMERIC_SCALE */
                                      NULL,                 /* DATETIME_PRECISION */
                                      NULL,                 /* INTERVAL_TYPE */
                                      NULL,                 /* INTERVAL_TYPE_ID */
                                      NULL,                 /* INTERVAL_PRECISION */
                                      sOctetLen,            /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );


    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief CHARACTER VARYING 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : CHARACTER VARYING
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - VARCHAR
 *                <BR> - CHARACTER VARYING
 * @param[in]     aLengthUnit        길이의 단위
 *                <BR> 문자(CHARACTER) 단위인지 바이트(OCTET) 인지를 설정
 *                <BR> - DTL_STRING_LENGTH_UNIT_CHARACTERS
 *                <BR> - DTL_STRING_LENGTH_UNIT_OCTETS
 * @param[in]     aDeclaredMaxLength 사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_LENGTH_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertVarcharColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aOwnerID,
                                          stlInt64            aSchemaID,
                                          stlInt64            aTableID,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          dtlStringLengthUnit aLengthUnit,
                                          stlInt64            aDeclaredMaxLength,
                                          ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCharLen = 0;
    stlInt64   sOctetLen = 0;
    stlInt64   sCalcLen = 0;
    stlInt64 * sDeclareLen = NULL;

    dtlStringLengthUnit sLengthUnit;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_VARCHAR].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_VARCHAR;

    /**
     * - 입력된 길이값 계산
     */

    sCalcLen = aDeclaredMaxLength;
    sDeclareLen = & sCalcLen;

    /**
     * - Octet Length 와 Char Length를 계산 
     */
    
    sCharLen  = eldGetCharacterLength( aLengthUnit,  sCalcLen );
    sOctetLen = eldGetOctetLength( aLengthUnit,  sCalcLen, aEnv );

    if ( aLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
    {
        sLengthUnit = ellGetDbCharLengthUnit();
    }
    else
    {
        sLengthUnit = aLengthUnit;
    }
    
    /**
     * - 컬럼의 data type descriptor 추가
     */
    
    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,           /* OWNER_ID */
                                      & aSchemaID,          /* SCHEMA_ID */
                                      & aTableID,           /* TABLE_ID */
                                      & aColumnID,          /* COLUMN_ID */
                                      sNormalTypeName,      /* DATA_TYPE */
                                      & sDataTypeID,        /* DATA_TYPE_ID */
                                      & sLengthUnit,        /* STRING_LENGTH_UNIT */
                                      & sCharLen,           /* CHARACTER_MAXIMUM_LENGTH */
                                      & sOctetLen,          /* CHARACTER_OCTET_LENGTH */
                                      NULL,                 /* NUMERIC_PRECISION */
                                      NULL,                 /* NUMERIC_PRECISION_RADIX */
                                      NULL,                 /* NUMERIC_SCALE */
                                      aDeclaredTypeName,    /* DECLARED_DATA_TYPE */
                                      sDeclareLen,          /* DECLARED_NUMERIC_PRECISION */
                                      NULL,                 /* DECLARED_NUMERIC_SCALE */
                                      NULL,                 /* DATETIME_PRECISION */
                                      NULL,                 /* INTERVAL_TYPE */
                                      NULL,                 /* INTERVAL_TYPE_ID */
                                      NULL,                 /* INTERVAL_PRECISION */
                                      sOctetLen,            /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief CHARACTER VARYING 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : CHARACTER VARYING
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - VARCHAR
 *                <BR> - CHARACTER VARYING
 * @param[in]     aLengthUnit        길이의 단위
 *                <BR> 문자(CHARACTER) 단위인지 바이트(OCTET) 인지를 설정
 *                <BR> - DTL_STRING_LENGTH_UNIT_CHARACTERS
 *                <BR> - DTL_STRING_LENGTH_UNIT_OCTETS
 * @param[in]     aDeclaredMaxLength 사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_LENGTH_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyVarcharColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          dtlStringLengthUnit aLengthUnit,
                                          stlInt64            aDeclaredMaxLength,
                                          ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCharLen = 0;
    stlInt64   sOctetLen = 0;
    stlInt64   sCalcLen = 0;
    stlInt64 * sDeclareLen = NULL;

    dtlStringLengthUnit sLengthUnit;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_VARCHAR].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_VARCHAR;

    /**
     * - 입력된 길이값 계산
     */

    sCalcLen = aDeclaredMaxLength;
    sDeclareLen = & sCalcLen;

    /**
     * - Octet Length 와 Char Length를 계산 
     */
    
    sCharLen  = eldGetCharacterLength( aLengthUnit,  sCalcLen );
    sOctetLen = eldGetOctetLength( aLengthUnit,  sCalcLen, aEnv );

    if ( aLengthUnit == DTL_STRING_LENGTH_UNIT_NA )
    {
        sLengthUnit = ellGetDbCharLengthUnit();
    }
    else
    {
        sLengthUnit = aLengthUnit;
    }
    
    /**
     * - 컬럼의 data type descriptor 변경
     */
    
    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,            /* COLUMN_ID */
                                      sNormalTypeName,      /* DATA_TYPE */
                                      & sDataTypeID,        /* DATA_TYPE_ID */
                                      & sLengthUnit,        /* STRING_LENGTH_UNIT */
                                      & sCharLen,           /* CHARACTER_MAXIMUM_LENGTH */
                                      & sOctetLen,          /* CHARACTER_OCTET_LENGTH */
                                      NULL,                 /* NUMERIC_PRECISION */
                                      NULL,                 /* NUMERIC_PRECISION_RADIX */
                                      NULL,                 /* NUMERIC_SCALE */
                                      aDeclaredTypeName,    /* DECLARED_DATA_TYPE */
                                      sDeclareLen,          /* DECLARED_NUMERIC_PRECISION */
                                      NULL,                 /* DECLARED_NUMERIC_SCALE */
                                      NULL,                 /* DATETIME_PRECISION */
                                      NULL,                 /* INTERVAL_TYPE */
                                      NULL,                 /* INTERVAL_TYPE_ID */
                                      NULL,                 /* INTERVAL_PRECISION */
                                      sOctetLen,            /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief LONG VARCHAR 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : CHARACTER LONG VARYING (SQL 표준은 아님)
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TEXT
 *                <BR> - LONG VARCHAR
 *                <BR> - CHARACTER LONG VARYING
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertLongVarcharColumnTypeDesc( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              stlInt64            aOwnerID,
                                              stlInt64            aSchemaID,
                                              stlInt64            aTableID,
                                              stlInt64            aColumnID,
                                              stlChar           * aDeclaredTypeName,
                                              ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCalcLen = 0;
    stlInt64   sCharLen = 0;
    stlInt64   sOctetLen = 0;

    dtlStringLengthUnit  sLengthUnit;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_LONGVARCHAR].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_LONGVARCHAR;

    /**
     * - Octet Length 와 Char Length를 계산 
     */
    
    sCalcLen = DTL_LONGVARCHAR_MAX_PRECISION;
    sCharLen  = eldGetCharacterLength( DTL_STRING_LENGTH_UNIT_OCTETS,  sCalcLen );
    sOctetLen = eldGetOctetLength( DTL_STRING_LENGTH_UNIT_OCTETS,  sCalcLen, aEnv );
    sLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;

    /**
     * - 컬럼의 data type descriptor 추가
     */
    
    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,         /* OWNER_ID */
                                      & aSchemaID,        /* SCHEMA_ID */
                                      & aTableID,         /* TABLE_ID */
                                      & aColumnID,        /* COLUMN_ID */
                                      sNormalTypeName,    /* DATA_TYPE */
                                      & sDataTypeID,      /* DATA_TYPE_ID */
                                      & sLengthUnit,      /* STRING_LENGTH_UNIT */
                                      & sCharLen,         /* CHARACTER_MAXIMUM_LENGTH */
                                      & sOctetLen,        /* CHARACTER_OCTET_LENGTH */
                                      NULL,               /* NUMERIC_PRECISION */
                                      NULL,               /* NUMERIC_PRECISION_RADIX */
                                      NULL,               /* NUMERIC_SCALE */
                                      aDeclaredTypeName,  /* DECLARED_DATA_TYPE */
                                      NULL,               /* DECLARED_NUMERIC_PRECISION */
                                      NULL,               /* DECLARED_NUMERIC_SCALE */
                                      NULL,               /* DATETIME_PRECISION */
                                      NULL,               /* INTERVAL_TYPE */
                                      NULL,               /* INTERVAL_TYPE_ID */
                                      NULL,               /* INTERVAL_PRECISION */
                                      sOctetLen,          /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief LONG VARCHAR 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : CHARACTER LONG VARYING (SQL 표준은 아님)
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TEXT
 *                <BR> - LONG VARCHAR
 *                <BR> - CHARACTER LONG VARYING
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyLongVarcharColumnTypeDesc( smlTransId          aTransID,
                                              smlStatement      * aStmt,
                                              stlInt64            aColumnID,
                                              stlChar           * aDeclaredTypeName,
                                              ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;
    
    stlInt64   sCalcLen = 0;
    stlInt64   sCharLen = 0;
    stlInt64   sOctetLen = 0;

    dtlStringLengthUnit  sLengthUnit;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_LONGVARCHAR].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_LONGVARCHAR;

    /**
     * - Octet Length 와 Char Length를 계산 
     */
    
    sCalcLen = DTL_LONGVARCHAR_MAX_PRECISION;
    sCharLen  = eldGetCharacterLength( DTL_STRING_LENGTH_UNIT_OCTETS,  sCalcLen );
    sOctetLen = eldGetOctetLength( DTL_STRING_LENGTH_UNIT_OCTETS,  sCalcLen, aEnv );
    sLengthUnit = DTL_STRING_LENGTH_UNIT_OCTETS;

    /**
     * - 컬럼의 data type descriptor 변경
     */
    
    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,          /* COLUMN_ID */
                                      sNormalTypeName,    /* DATA_TYPE */
                                      & sDataTypeID,      /* DATA_TYPE_ID */
                                      & sLengthUnit,      /* STRING_LENGTH_UNIT */
                                      & sCharLen,         /* CHARACTER_MAXIMUM_LENGTH */
                                      & sOctetLen,        /* CHARACTER_OCTET_LENGTH */
                                      NULL,               /* NUMERIC_PRECISION */
                                      NULL,               /* NUMERIC_PRECISION_RADIX */
                                      NULL,               /* NUMERIC_SCALE */
                                      aDeclaredTypeName,  /* DECLARED_DATA_TYPE */
                                      NULL,               /* DECLARED_NUMERIC_PRECISION */
                                      NULL,               /* DECLARED_NUMERIC_SCALE */
                                      NULL,               /* DATETIME_PRECISION */
                                      NULL,               /* INTERVAL_TYPE */
                                      NULL,               /* INTERVAL_TYPE_ID */
                                      NULL,               /* INTERVAL_PRECISION */
                                      sOctetLen,          /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**********************************************************************
 * Binary String 유형의 컬럼
 **********************************************************************/

/**
 * @brief BINARY 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BINARY
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - BINARY
 * @param[in]     aDeclaredMaxLength 사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_LENGTH_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertBinaryColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aOwnerID,
                                         stlInt64            aSchemaID,
                                         stlInt64            aTableID,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         stlInt64            aDeclaredMaxLength,
                                         ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCalcLen = 0;

    stlInt64 * sDeclareLen = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_BINARY].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_BINARY;

    /**
     * - 입력된 길이값 계산
     */
    
    if ( aDeclaredMaxLength == DTL_LENGTH_NA )
    {
        /* 사용자가 입력한 길이값이 없음 */
        sCalcLen = gDataTypeDefinition[DTL_TYPE_BINARY].mDefStringLength;
        sDeclareLen = NULL;
    }
    else
    {
        sCalcLen = aDeclaredMaxLength;
        sDeclareLen = & sCalcLen;
    }

    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,          /* OWNER_ID */
                                      & aSchemaID,         /* SCHEMA_ID */
                                      & aTableID,          /* TABLE_ID */
                                      & aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,     /* DATA_TYPE */
                                      & sDataTypeID,       /* DATA_TYPE_ID */
                                      NULL,                /* STRING_LENGTH_UNIT */
                                      & sCalcLen,          /* CHARACTER_MAXIMUM_LENGTH */
                                      & sCalcLen,          /* CHARACTER_OCTET_LENGTH */
                                      NULL,                /* NUMERIC_PRECISION */
                                      NULL,                /* NUMERIC_PRECISION_RADIX */
                                      NULL,                /* NUMERIC_SCALE */
                                      aDeclaredTypeName,   /* DECLARED_DATA_TYPE */
                                      sDeclareLen,         /* DECLARED_NUMERIC_PRECISION */
                                      NULL,                /* DECLARED_NUMERIC_SCALE */
                                      NULL,                /* DATETIME_PRECISION */
                                      NULL,                /* INTERVAL_TYPE */
                                      NULL,                /* INTERVAL_TYPE_ID */
                                      NULL,                /* INTERVAL_PRECISION */
                                      sCalcLen,            /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief BINARY 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BINARY
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - BINARY
 * @param[in]     aDeclaredMaxLength 사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_LENGTH_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyBinaryColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         stlInt64            aDeclaredMaxLength,
                                         ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCalcLen = 0;

    stlInt64 * sDeclareLen = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_BINARY].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_BINARY;

    /**
     * - 입력된 길이값 계산
     */
    
    if ( aDeclaredMaxLength == DTL_LENGTH_NA )
    {
        /* 사용자가 입력한 길이값이 없음 */
        sCalcLen = gDataTypeDefinition[DTL_TYPE_BINARY].mDefStringLength;
        sDeclareLen = NULL;
    }
    else
    {
        sCalcLen = aDeclaredMaxLength;
        sDeclareLen = & sCalcLen;
    }

    /**
     * - 컬럼의 data type descriptor 변경 
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,           /* COLUMN_ID */
                                      sNormalTypeName,     /* DATA_TYPE */
                                      & sDataTypeID,       /* DATA_TYPE_ID */
                                      NULL,                /* STRING_LENGTH_UNIT */
                                      & sCalcLen,          /* CHARACTER_MAXIMUM_LENGTH */
                                      & sCalcLen,          /* CHARACTER_OCTET_LENGTH */
                                      NULL,                /* NUMERIC_PRECISION */
                                      NULL,                /* NUMERIC_PRECISION_RADIX */
                                      NULL,                /* NUMERIC_SCALE */
                                      aDeclaredTypeName,   /* DECLARED_DATA_TYPE */
                                      sDeclareLen,         /* DECLARED_NUMERIC_PRECISION */
                                      NULL,                /* DECLARED_NUMERIC_SCALE */
                                      NULL,                /* DATETIME_PRECISION */
                                      NULL,                /* INTERVAL_TYPE */
                                      NULL,                /* INTERVAL_TYPE_ID */
                                      NULL,                /* INTERVAL_PRECISION */
                                      sCalcLen,            /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief VARBINARY 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BINARY VARYING
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - VARBINARY
 *                <BR> - BINARY VARYING
 * @param[in]     aDeclaredMaxLength 사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_LENGTH_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertVarBinaryColumnTypeDesc( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            stlInt64            aOwnerID,
                                            stlInt64            aSchemaID,
                                            stlInt64            aTableID,
                                            stlInt64            aColumnID,
                                            stlChar           * aDeclaredTypeName,
                                            stlInt64            aDeclaredMaxLength,
                                            ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCalcLen = 0;

    stlInt64 * sDeclareLen = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_VARBINARY].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_VARBINARY;

    /**
     * - 입력된 길이값 계산
     */

    sCalcLen = aDeclaredMaxLength;
    sDeclareLen = & sCalcLen;

    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,          /* OWNER_ID */
                                      & aSchemaID,         /* SCHEMA_ID */
                                      & aTableID,          /* TABLE_ID */
                                      & aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,     /* DATA_TYPE */
                                      & sDataTypeID,       /* DATA_TYPE_ID */
                                      NULL,                /* STRING_LENGTH_UNIT */
                                      & sCalcLen,          /* CHARACTER_MAXIMUM_LENGTH */
                                      & sCalcLen,          /* CHARACTER_OCTET_LENGTH */
                                      NULL,                /* NUMERIC_PRECISION */
                                      NULL,                /* NUMERIC_PRECISION_RADIX */
                                      NULL,                /* NUMERIC_SCALE */
                                      aDeclaredTypeName,   /* DECLARED_DATA_TYPE */
                                      sDeclareLen,         /* DECLARED_NUMERIC_PRECISION */
                                      NULL,                /* DECLARED_NUMERIC_SCALE */
                                      NULL,                /* DATETIME_PRECISION */
                                      NULL,                /* INTERVAL_TYPE */
                                      NULL,                /* INTERVAL_TYPE_ID */
                                      NULL,                /* INTERVAL_PRECISION */
                                      sCalcLen,            /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief VARBINARY 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BINARY VARYING
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - VARBINARY
 *                <BR> - BINARY VARYING
 * @param[in]     aDeclaredMaxLength 사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_LENGTH_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyVarBinaryColumnTypeDesc( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            stlInt64            aColumnID,
                                            stlChar           * aDeclaredTypeName,
                                            stlInt64            aDeclaredMaxLength,
                                            ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCalcLen = 0;

    stlInt64 * sDeclareLen = NULL;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_VARBINARY].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_VARBINARY;

    /**
     * - 입력된 길이값 계산
     */

    sCalcLen = aDeclaredMaxLength;
    sDeclareLen = & sCalcLen;

    /**
     * - 컬럼의 data type descriptor 변경 
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,           /* COLUMN_ID */
                                      sNormalTypeName,     /* DATA_TYPE */
                                      & sDataTypeID,       /* DATA_TYPE_ID */
                                      NULL,                /* STRING_LENGTH_UNIT */
                                      & sCalcLen,          /* CHARACTER_MAXIMUM_LENGTH */
                                      & sCalcLen,          /* CHARACTER_OCTET_LENGTH */
                                      NULL,                /* NUMERIC_PRECISION */
                                      NULL,                /* NUMERIC_PRECISION_RADIX */
                                      NULL,                /* NUMERIC_SCALE */
                                      aDeclaredTypeName,   /* DECLARED_DATA_TYPE */
                                      sDeclareLen,         /* DECLARED_NUMERIC_PRECISION */
                                      NULL,                /* DECLARED_NUMERIC_SCALE */
                                      NULL,                /* DATETIME_PRECISION */
                                      NULL,                /* INTERVAL_TYPE */
                                      NULL,                /* INTERVAL_TYPE_ID */
                                      NULL,                /* INTERVAL_PRECISION */
                                      sCalcLen,            /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}




/**
 * @brief LONG VARBINARY 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BINARY LONG VARYING (SQL 표준이 아님)
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - BYTEA
 *                <BR> - LONG VARBINARY
 *                <BR> - BINARY LONG VARYING
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertLongVarBinaryColumnTypeDesc( smlTransId          aTransID,
                                                smlStatement      * aStmt,
                                                stlInt64            aOwnerID,
                                                stlInt64            aSchemaID,
                                                stlInt64            aTableID,
                                                stlInt64            aColumnID,
                                                stlChar           * aDeclaredTypeName,
                                                ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCalcLen = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_LONGVARBINARY].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_LONGVARBINARY;
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    sCalcLen = DTL_LONGVARBINARY_MAX_PRECISION;

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      & sCalcLen,        /* CHARACTER_MAXIMUM_LENGTH */
                                      & sCalcLen,        /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sCalcLen,          /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief LONG VARBINARY 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BINARY LONG VARYING (SQL 표준이 아님)
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - BYTEA
 *                <BR> - LONG VARBINARY
 *                <BR> - BINARY LONG VARYING
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyLongVarBinaryColumnTypeDesc( smlTransId          aTransID,
                                                smlStatement      * aStmt,
                                                stlInt64            aColumnID,
                                                stlChar           * aDeclaredTypeName,
                                                ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sCalcLen = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_LONGVARBINARY].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_LONGVARBINARY;
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    sCalcLen = DTL_LONGVARBINARY_MAX_PRECISION;

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      & sCalcLen,        /* CHARACTER_MAXIMUM_LENGTH */
                                      & sCalcLen,        /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sCalcLen,          /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}




/**********************************************************************
 * Exact Numeric 유형의 컬럼
 **********************************************************************/

/**
 * @brief SMALLINT 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : SMALLINT
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - SMALLINT
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertSmallintColumnTypeDesc( smlTransId          aTransID,
                                           smlStatement      * aStmt,
                                           stlInt64            aOwnerID,
                                           stlInt64            aSchemaID,
                                           stlInt64            aTableID,
                                           stlInt64            aColumnID,
                                           stlChar           * aDeclaredTypeName,
                                           ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_SMALLINT;

    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mDefaultNumericPrecRadix;
    
    /**
     * - 기본 Scale 값 획득
     */

    sNumScale = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mDefNumericScale;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_SMALLINT,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,       /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief SMALLINT 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : SMALLINT
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - SMALLINT
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifySmallintColumnTypeDesc( smlTransId          aTransID,
                                           smlStatement      * aStmt,
                                           stlInt64            aColumnID,
                                           stlChar           * aDeclaredTypeName,
                                           ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_SMALLINT;

    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mDefaultNumericPrecRadix;
    
    /**
     * - 기본 Scale 값 획득
     */

    sNumScale = gDataTypeDefinition[DTL_TYPE_NATIVE_SMALLINT].mDefNumericScale;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_SMALLINT,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,       /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief INTEGER 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : INTEGER
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - INT
 *                <BR> - INTEGER
 *                <BR> - SERIAL
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertIntegerColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aOwnerID,
                                          stlInt64            aSchemaID,
                                          stlInt64            aTableID,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_INTEGER;
    
    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mDefaultNumericPrecRadix;
    
    /**
     * - 기본 Scale 값 획득
     */

    sNumScale = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mDefNumericScale;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_INTEGER,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,       /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief INTEGER 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : INTEGER
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - INT
 *                <BR> - INTEGER
 *                <BR> - SERIAL
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyIntegerColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_INTEGER;
    
    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mDefaultNumericPrecRadix;
    
    /**
     * - 기본 Scale 값 획득
     */

    sNumScale = gDataTypeDefinition[DTL_TYPE_NATIVE_INTEGER].mDefNumericScale;
    
    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_INTEGER,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,       /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}




/**
 * @brief BIGINT 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BIGINT
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - BIGINT
 *                <BR> - BIGSERIAL
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertBigintColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aOwnerID,
                                         stlInt64            aSchemaID,
                                         stlInt64            aTableID,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_BIGINT;
    
    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mDefaultNumericPrecRadix;
    
    /**
     * - 기본 Scale 값 획득
     */

    sNumScale = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mDefNumericScale;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_BIGINT,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,       /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief BIGINT 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BIGINT
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - BIGINT
 *                <BR> - BIGSERIAL
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyBigintColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_BIGINT;
    
    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mDefaultNumericPrecRadix;
    
    /**
     * - 기본 Scale 값 획득
     */

    sNumScale = gDataTypeDefinition[DTL_TYPE_NATIVE_BIGINT].mDefNumericScale;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_BIGINT,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,       /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief FLOAT 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : FLOAT
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - FLOAT
 * @param[in]     aDeclaredRadix          Type 특성의 Numeric Radix 값
 * @param[in]     aDeclaredDecimalPrec    사용자가 정의한 Decimal Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in]     aDeclaredScale     사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_SCALE_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
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
                                        ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sDeclPrec     = 0;
    stlInt64 * sDeclPrecPtr  = NULL;
    stlInt64 * sDeclScalePtr = NULL;
    
    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength   = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_FLOAT].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_FLOAT;

    /**
     * - Radix 값 획득
     */
    
    sNumPrecRadix = aDeclaredRadix;

    /**
     * - 기본 Scale 값 획득
     */
    
    sNumScale = DTL_SCALE_NA;
    sDeclScalePtr = NULL;
    
    /**
     * - 기본 Precision 값 획득
     */
    if ( aDeclaredDecimalPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 Precision 이 없음 */
        sNumPrec = gDataTypeDefinition[DTL_TYPE_FLOAT].mDefNumericPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sNumPrec = aDeclaredDecimalPrec;
        sDeclPrec = aDeclaredDecimalPrec;
        sDeclPrecPtr = & sDeclPrec;
    }
    
    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_FLOAT,
                                        sNumPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,           /* OWNER_ID */
                                      & aSchemaID,          /* SCHEMA_ID */
                                      & aTableID,           /* TABLE_ID */
                                      & aColumnID,          /* COLUMN_ID */
                                      sNormalTypeName,      /* DATA_TYPE */
                                      & sDataTypeID,        /* DATA_TYPE_ID */
                                      NULL,                 /* STRING_LENGTH_UNIT */
                                      NULL,                 /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,                 /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,           /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,      /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,          /* NUMERIC_SCALE */
                                      aDeclaredTypeName,    /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,         /* DECLARED_NUMERIC_PRECISION */
                                      sDeclScalePtr,        /* DECLARED_NUMERIC_SCALE */
                                      NULL,                 /* DATETIME_PRECISION */
                                      NULL,                 /* INTERVAL_TYPE */
                                      NULL,                 /* INTERVAL_TYPE_ID */
                                      NULL,                 /* INTERVAL_PRECISION */
                                      sPhyLength,           /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief NUMBER 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : NUMERIC
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - NUMERIC
 *                <BR> - NUMBER
 *                <BR> - DEC
 *                <BR> - DECIMAL
 * @param[in]     aDeclaredRadix          Type 특성의 Numeric Radix 값
 * @param[in]     aDeclaredDecimalPrec    사용자가 정의한 Decimal Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in]     aDeclaredScale     사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_SCALE_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
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
                                          ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sDeclPrec     = 0;
    stlInt64 * sDeclPrecPtr  = NULL;
    stlInt64   sDeclScale    = 0;
    stlInt64 * sDeclScalePtr = NULL;
    
    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength   = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NUMBER].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NUMBER;

    /**
     * - Radix 값 획득
     */
    
    sNumPrecRadix = aDeclaredRadix;

    /**
     * - 기본 Scale 값 획득
     */
    
    if ( aDeclaredScale == DTL_SCALE_NA )
    {
        /* 사용자가 정의한 Precision 이 없음 */
        sNumScale = DTL_SCALE_NA;
        sDeclScalePtr = NULL;
    }
    else
    {
        sNumScale = aDeclaredScale;

        sDeclScale = sNumScale;
        sDeclScalePtr = & sDeclScale;
    }

    /**
     * - 기본 Precision 값 획득
     */
    if ( aDeclaredDecimalPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 Precision 이 없음 */
        sNumPrec = gDataTypeDefinition[DTL_TYPE_NUMBER].mDefNumericPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sNumPrec = aDeclaredDecimalPrec;
        sDeclPrec = aDeclaredDecimalPrec;
        sDeclPrecPtr = & sDeclPrec;
    }
   
    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NUMBER,
                                        sNumPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,           /* OWNER_ID */
                                      & aSchemaID,          /* SCHEMA_ID */
                                      & aTableID,           /* TABLE_ID */
                                      & aColumnID,          /* COLUMN_ID */
                                      sNormalTypeName,      /* DATA_TYPE */
                                      & sDataTypeID,        /* DATA_TYPE_ID */
                                      NULL,                 /* STRING_LENGTH_UNIT */
                                      NULL,                 /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,                 /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,           /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,      /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,          /* NUMERIC_SCALE */
                                      aDeclaredTypeName,    /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,         /* DECLARED_NUMERIC_PRECISION */
                                      sDeclScalePtr,        /* DECLARED_NUMERIC_SCALE */
                                      NULL,                 /* DATETIME_PRECISION */
                                      NULL,                 /* INTERVAL_TYPE */
                                      NULL,                 /* INTERVAL_TYPE_ID */
                                      NULL,                 /* INTERVAL_PRECISION */
                                      sPhyLength,           /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief FLOAT 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : FLOAT
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - FLOAT
 * @param[in]     aDeclaredRadix          Precision Radix               
 * @param[in]     aDeclaredDecimalPrec    사용자가 정의한 Decimal Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in]     aDeclaredScale     사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_SCALE_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyFloatColumnTypeDesc( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aColumnID,
                                        stlChar           * aDeclaredTypeName,
                                        stlInt32            aDeclaredRadix,
                                        stlInt32            aDeclaredDecimalPrec,
                                        stlInt32            aDeclaredScale,
                                        ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sDeclPrec     = 0;
    stlInt64 * sDeclPrecPtr  = NULL;
    stlInt64 * sDeclScalePtr = NULL;
    
    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength   = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_FLOAT].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_FLOAT;

    /**
     * - Radix 값 획득
     */
    
    sNumPrecRadix = aDeclaredRadix;

    /**
     * - 기본 Scale 값 획득
     */
    
    /* 사용자가 정의한 Precision 이 없음 */
    sNumScale = DTL_SCALE_NA;
    sDeclScalePtr = NULL;

    /**
     * - 기본 Precision 값 획득
     */
    if ( aDeclaredDecimalPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 Precision 이 없음 */
        sNumPrec = gDataTypeDefinition[DTL_TYPE_FLOAT].mDefNumericPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sNumPrec = aDeclaredDecimalPrec;
        sDeclPrec = aDeclaredDecimalPrec;
        sDeclPrecPtr = & sDeclPrec;
    }
    
    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_FLOAT,
                                        sNumPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,            /* COLUMN_ID */
                                      sNormalTypeName,      /* DATA_TYPE */
                                      & sDataTypeID,        /* DATA_TYPE_ID */
                                      NULL,                 /* STRING_LENGTH_UNIT */
                                      NULL,                 /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,                 /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,           /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,      /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,          /* NUMERIC_SCALE */
                                      aDeclaredTypeName,    /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,         /* DECLARED_NUMERIC_PRECISION */
                                      sDeclScalePtr,        /* DECLARED_NUMERIC_SCALE */
                                      NULL,                 /* DATETIME_PRECISION */
                                      NULL,                 /* INTERVAL_TYPE */
                                      NULL,                 /* INTERVAL_TYPE_ID */
                                      NULL,                 /* INTERVAL_PRECISION */
                                      sPhyLength,           /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief NUMBER 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : NUMERIC
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - NUMERIC
 *                <BR> - NUMBER
 *                <BR> - DEC
 *                <BR> - DECIMAL
 * @param[in]     aDeclaredRadix          Precision Radix               
 * @param[in]     aDeclaredDecimalPrec    사용자가 정의한 Decimal Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in]     aDeclaredScale     사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_SCALE_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyNumericColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          stlInt32            aDeclaredRadix,
                                          stlInt32            aDeclaredDecimalPrec,
                                          stlInt32            aDeclaredScale,
                                          ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sDeclPrec     = 0;
    stlInt64 * sDeclPrecPtr  = NULL;
    stlInt64   sDeclScale    = 0;
    stlInt64 * sDeclScalePtr = NULL;
    
    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;
    stlInt32   sNumScale = 0;

    stlInt64   sPhyLength   = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NUMBER].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NUMBER;

    /**
     * - Radix 값 획득
     */
    
    sNumPrecRadix = aDeclaredRadix;

    /**
     * - 기본 Scale 값 획득
     */
    
    if ( aDeclaredScale == DTL_SCALE_NA )
    {
        /* 사용자가 정의한 Precision 이 없음 */
        sNumScale = DTL_SCALE_NA;
        sDeclScalePtr = NULL;
    }
    else
    {
        sNumScale = aDeclaredScale;

        sDeclScale = sNumScale;
        sDeclScalePtr = & sDeclScale;
    }

    /**
     * - 기본 Precision 값 획득
     */
    if ( aDeclaredDecimalPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 Precision 이 없음 */
        sNumPrec = gDataTypeDefinition[DTL_TYPE_NUMBER].mDefNumericPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sNumPrec = aDeclaredDecimalPrec;
        sDeclPrec = aDeclaredDecimalPrec;
        sDeclPrecPtr = & sDeclPrec;
    }

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NUMBER,
                                        sNumPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,            /* COLUMN_ID */
                                      sNormalTypeName,      /* DATA_TYPE */
                                      & sDataTypeID,        /* DATA_TYPE_ID */
                                      NULL,                 /* STRING_LENGTH_UNIT */
                                      NULL,                 /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,                 /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,           /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,      /* NUMERIC_PRECISION_RADIX */
                                      & sNumScale,          /* NUMERIC_SCALE */
                                      aDeclaredTypeName,    /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,         /* DECLARED_NUMERIC_PRECISION */
                                      sDeclScalePtr,        /* DECLARED_NUMERIC_SCALE */
                                      NULL,                 /* DATETIME_PRECISION */
                                      NULL,                 /* INTERVAL_TYPE */
                                      NULL,                 /* INTERVAL_TYPE_ID */
                                      NULL,                 /* INTERVAL_PRECISION */
                                      sPhyLength,           /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DECIMAL 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : NUMERIC (SQL 표준과 달리 NUMERIC의 alias 로 사용함)
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - DEC
 *                <BR> - DECIMAL
 * @param[in]     aDeclaredRadix           Precision Radix
 * @param[in]     aDeclaredDecimalPrec     사용자가 정의한 Decimal Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in]     aDeclaredScale     사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_SCALE_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 * @todo DECIMAL 이 정말 NUMERIC 의 Alias 인가?
 */
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
                                          ellEnv            * aEnv )
{
    return ellInsertNumericColumnTypeDesc( aTransID,
                                           aStmt,
                                           aOwnerID,
                                           aSchemaID,
                                           aTableID,
                                           aColumnID,
                                           aDeclaredTypeName,
                                           aDeclaredRadix,
                                           aDeclaredDecimalPrec,
                                           aDeclaredScale,
                                           aEnv );
}



/**
 * @brief DECIMAL 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : NUMERIC (SQL 표준과 달리 NUMERIC의 alias 로 사용함)
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - DEC
 *                <BR> - DECIMAL
 * @param[in]     aDeclaredRadix           Precision Radix
 * @param[in]     aDeclaredDecimalPrec     사용자가 정의한 Decimal Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in]     aDeclaredScale     사용자가 정의한 타입의 길이
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_SCALE_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 * @todo DECIMAL 이 정말 NUMERIC 의 Alias 인가?
 */
stlStatus ellModifyDecimalColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          stlInt32            aDeclaredRadix,
                                          stlInt32            aDeclaredDecimalPrec,
                                          stlInt32            aDeclaredScale,
                                          ellEnv            * aEnv )
{
    return ellModifyNumericColumnTypeDesc( aTransID,
                                           aStmt,
                                           aColumnID,
                                           aDeclaredTypeName,
                                           aDeclaredRadix,
                                           aDeclaredDecimalPrec,
                                           aDeclaredScale,
                                           aEnv );
}


/**********************************************************************
 * Approximate Numeric 유형의 컬럼
 **********************************************************************/

/**
 * @brief REAL 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : REAL
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - REAL
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertRealColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aOwnerID,
                                       stlInt64            aSchemaID,
                                       stlInt64            aTableID,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_REAL].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_REAL;

    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_REAL].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_REAL].mDefaultNumericPrecRadix;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_REAL,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief REAL 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : REAL
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - REAL
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyRealColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_REAL].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_REAL;

    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_REAL].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_REAL].mDefaultNumericPrecRadix;
    
    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_REAL,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief DOUBLE 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : DOUBLE PRECISION
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - DOUBLE
 *                <BR> - DOUBLE PRECISION
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertDoubleColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aOwnerID,
                                         stlInt64            aSchemaID,
                                         stlInt64            aTableID,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_DOUBLE].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_DOUBLE;
    
    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_DOUBLE].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_DOUBLE].mDefaultNumericPrecRadix;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_DOUBLE,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief DOUBLE 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : DOUBLE PRECISION
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - DOUBLE
 *                <BR> - DOUBLE PRECISION
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyDoubleColumnTypeDesc( smlTransId          aTransID,
                                         smlStatement      * aStmt,
                                         stlInt64            aColumnID,
                                         stlChar           * aDeclaredTypeName,
                                         ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sNumPrec = 0;
    stlInt32   sNumPrecRadix = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_NATIVE_DOUBLE].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_NATIVE_DOUBLE;
    
    /**
     * - 기본 Precision 값 획득
     */

    sNumPrec = gDataTypeDefinition[DTL_TYPE_NATIVE_DOUBLE].mDefNumericPrec;
                                            
    /**
     * - 기본 Precision Radix 값 획득
     */

    sNumPrecRadix = gDataTypeDefinition[DTL_TYPE_NATIVE_DOUBLE].mDefaultNumericPrecRadix;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_NATIVE_DOUBLE,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      & sNumPrec,        /* NUMERIC_PRECISION */
                                      & sNumPrecRadix,   /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**********************************************************************
 * Boolean 유형의 컬럼
 **********************************************************************/

/**
 * @brief BOOLEAN 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BOOLEAN
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - BOOL
 *                <BR> - BOOLEAN
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertBooleanColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aOwnerID,
                                          stlInt64            aSchemaID,
                                          stlInt64            aTableID,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_BOOLEAN].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_BOOLEAN;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_BOOLEAN,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief BOOLEAN 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : BOOLEAN
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - BOOL
 *                <BR> - BOOLEAN
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyBooleanColumnTypeDesc( smlTransId          aTransID,
                                          smlStatement      * aStmt,
                                          stlInt64            aColumnID,
                                          stlChar           * aDeclaredTypeName,
                                          ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_BOOLEAN].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_BOOLEAN;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_BOOLEAN,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );

    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**********************************************************************
 * DateTime 유형의 컬럼
 **********************************************************************/

/**
 * @brief DATE 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : DATE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - DATE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertDateColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aOwnerID,
                                       stlInt64            aSchemaID,
                                       stlInt64            aTableID,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_DATE].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_DATE;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_DATE,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_DATE,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief DATE 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : DATE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - DATE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyDateColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_DATE].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_DATE;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_DATE,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief TIME 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : TIME WITHOUT TIME ZONE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TIME
 *                <BR> - TIME WITHOUT TIME ZONE
 * @param[in]     aDeclaredFractionPrec   사용자가 정의한 Fractional Second Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertTimeColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aOwnerID,
                                       stlInt64            aSchemaID,
                                       stlInt64            aTableID,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       stlInt32            aDeclaredFractionPrec,
                                       ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sCalcPrec = 0;

    stlInt64   sDeclPrec = 0;
    stlInt64 * sDeclPrecPtr = NULL;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_TIME].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_TIME;
    
    /**
     * - Fractional Second Precision 을 획득
     */

    if ( aDeclaredFractionPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 fractional second precision 이 없음 */
        sCalcPrec = gDataTypeDefinition[DTL_TYPE_TIME].mDefFractionalSecondPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sCalcPrec = aDeclaredFractionPrec;
        sDeclPrec = aDeclaredFractionPrec;
        sDeclPrecPtr = & sDeclPrec;
    }

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_TIME,
                                        sCalcPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,      /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      & sCalcPrec,       /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief TIME 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : TIME WITHOUT TIME ZONE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TIME
 *                <BR> - TIME WITHOUT TIME ZONE
 * @param[in]     aDeclaredFractionPrec   사용자가 정의한 Fractional Second Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyTimeColumnTypeDesc( smlTransId          aTransID,
                                       smlStatement      * aStmt,
                                       stlInt64            aColumnID,
                                       stlChar           * aDeclaredTypeName,
                                       stlInt32            aDeclaredFractionPrec,
                                       ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sCalcPrec = 0;

    stlInt64   sDeclPrec = 0;
    stlInt64 * sDeclPrecPtr = NULL;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_TIME].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_TIME;
    
    /**
     * - Fractional Second Precision 을 획득
     */

    if ( aDeclaredFractionPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 fractional second precision 이 없음 */
        sCalcPrec = gDataTypeDefinition[DTL_TYPE_TIME].mDefFractionalSecondPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sCalcPrec = aDeclaredFractionPrec;
        sDeclPrec = aDeclaredFractionPrec;
        sDeclPrecPtr = & sDeclPrec;
    }

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_TIME,
                                        sCalcPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,      /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      & sCalcPrec,       /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief TIMESTAMP 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : TIMESTAMP WITHOUT TIME ZONE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TIMESTAMP
 *                <BR> - TIMESTAMP WITHOUT TIME ZONE
 * @param[in]     aDeclaredFractionPrec  사용자가 정의한 Fractional Second Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertTimestampColumnTypeDesc( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            stlInt64            aOwnerID,
                                            stlInt64            aSchemaID,
                                            stlInt64            aTableID,
                                            stlInt64            aColumnID,
                                            stlChar           * aDeclaredTypeName,
                                            stlInt32            aDeclaredFractionPrec,
                                            ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sCalcPrec = 0;

    stlInt64   sDeclPrec = 0;
    stlInt64 * sDeclPrecPtr = NULL;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_TIMESTAMP].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_TIMESTAMP;

    
    /**
     * - Fractional Second Precision 을 획득
     */

    if ( aDeclaredFractionPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 fractional second precision 이 없음 */
        sCalcPrec = gDataTypeDefinition[DTL_TYPE_TIMESTAMP].mDefFractionalSecondPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sCalcPrec = aDeclaredFractionPrec;
        sDeclPrec = aDeclaredFractionPrec;
        sDeclPrecPtr = & sDeclPrec;
    }

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_TIMESTAMP,
                                        sCalcPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,      /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      & sCalcPrec,       /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief TIMESTAMP 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : TIMESTAMP WITHOUT TIME ZONE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TIMESTAMP
 *                <BR> - TIMESTAMP WITHOUT TIME ZONE
 * @param[in]     aDeclaredFractionPrec  사용자가 정의한 Fractional Second Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyTimestampColumnTypeDesc( smlTransId          aTransID,
                                            smlStatement      * aStmt,
                                            stlInt64            aColumnID,
                                            stlChar           * aDeclaredTypeName,
                                            stlInt32            aDeclaredFractionPrec,
                                            ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sCalcPrec = 0;

    stlInt64   sDeclPrec = 0;
    stlInt64 * sDeclPrecPtr = NULL;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_TIMESTAMP].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_TIMESTAMP;

    
    /**
     * - Fractional Second Precision 을 획득
     */

    if ( aDeclaredFractionPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 fractional second precision 이 없음 */
        sCalcPrec = gDataTypeDefinition[DTL_TYPE_TIMESTAMP].mDefFractionalSecondPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sCalcPrec = aDeclaredFractionPrec;
        sDeclPrec = aDeclaredFractionPrec;
        sDeclPrecPtr = & sDeclPrec;
    }

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_TIMESTAMP,
                                        sCalcPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,      /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      & sCalcPrec,       /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief TIME WITH TIME ZONE 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : TIME WITH TIME ZONE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TIME WITH TIME ZONE
 * @param[in]     aDeclaredFractionPrec  사용자가 정의한 Fractional Second Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertTimeWithZoneColumnTypeDesc( smlTransId          aTransID,
                                               smlStatement      * aStmt,
                                               stlInt64            aOwnerID,
                                               stlInt64            aSchemaID,
                                               stlInt64            aTableID,
                                               stlInt64            aColumnID,
                                               stlChar           * aDeclaredTypeName,
                                               stlInt32            aDeclaredFractionPrec,
                                               ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sCalcPrec = 0;

    stlInt64   sDeclPrec = 0;
    stlInt64 * sDeclPrecPtr = NULL;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_TIME_WITH_TIME_ZONE].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_TIME_WITH_TIME_ZONE;
    

    /**
     * - Fractional Second Precision 을 획득
     */

    if ( aDeclaredFractionPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 fractional second precision 이 없음 */
        sCalcPrec = gDataTypeDefinition[DTL_TYPE_TIME_WITH_TIME_ZONE].mDefFractionalSecondPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sCalcPrec = aDeclaredFractionPrec;
        sDeclPrec = aDeclaredFractionPrec;
        sDeclPrecPtr = & sDeclPrec;
    }

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                        sCalcPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,      /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      & sCalcPrec,       /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief TIME WITH TIME ZONE 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : TIME WITH TIME ZONE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TIME WITH TIME ZONE
 * @param[in]     aDeclaredFractionPrec  사용자가 정의한 Fractional Second Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyTimeWithZoneColumnTypeDesc( smlTransId          aTransID,
                                               smlStatement      * aStmt,
                                               stlInt64            aColumnID,
                                               stlChar           * aDeclaredTypeName,
                                               stlInt32            aDeclaredFractionPrec,
                                               ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sCalcPrec = 0;

    stlInt64   sDeclPrec = 0;
    stlInt64 * sDeclPrecPtr = NULL;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_TIME_WITH_TIME_ZONE].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_TIME_WITH_TIME_ZONE;
    

    /**
     * - Fractional Second Precision 을 획득
     */

    if ( aDeclaredFractionPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 fractional second precision 이 없음 */
        sCalcPrec = gDataTypeDefinition[DTL_TYPE_TIME_WITH_TIME_ZONE].mDefFractionalSecondPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sCalcPrec = aDeclaredFractionPrec;
        sDeclPrec = aDeclaredFractionPrec;
        sDeclPrecPtr = & sDeclPrec;
    }
    
    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_TIME_WITH_TIME_ZONE,
                                        sCalcPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,      /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      & sCalcPrec,       /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief TIMESTAMP WITH TIME ZONE 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : TIMESTAMP WITH TIME ZONE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TIMESTAMP WITH TIME ZONE
 * @param[in]     aDeclaredFractionPrec  사용자가 정의한 Fractional Second Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertTimestampWithZoneColumnTypeDesc( smlTransId          aTransID,
                                                    smlStatement      * aStmt,
                                                    stlInt64            aOwnerID,
                                                    stlInt64            aSchemaID,
                                                    stlInt64            aTableID,
                                                    stlInt64            aColumnID,
                                                    stlChar           * aDeclaredTypeName,
                                                    stlInt32            aDeclaredFractionPrec,
                                                    ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sCalcPrec = 0;

    stlInt64   sDeclPrec = 0;
    stlInt64 * sDeclPrecPtr = NULL;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE;

    /**
     * - Fractional Second Precision 을 획득
     */

    if ( aDeclaredFractionPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 fractional second precision 이 없음 */
        sCalcPrec = gDataTypeDefinition[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mDefFractionalSecondPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sCalcPrec = aDeclaredFractionPrec;
        sDeclPrec = aDeclaredFractionPrec;
        sDeclPrecPtr = & sDeclPrec;
    }
    
    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                        sCalcPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,      /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      & sCalcPrec,       /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief TIMESTAMP WITH TIME ZONE 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : TIMESTAMP WITH TIME ZONE
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - TIMESTAMP WITH TIME ZONE
 * @param[in]     aDeclaredFractionPrec  사용자가 정의한 Fractional Second Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyTimestampWithZoneColumnTypeDesc( smlTransId          aTransID,
                                                    smlStatement      * aStmt,
                                                    stlInt64            aColumnID,
                                                    stlChar           * aDeclaredTypeName,
                                                    stlInt32            aDeclaredFractionPrec,
                                                    ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sCalcPrec = 0;

    stlInt64   sDeclPrec = 0;
    stlInt64 * sDeclPrecPtr = NULL;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE;

    /**
     * - Fractional Second Precision 을 획득
     */

    if ( aDeclaredFractionPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 fractional second precision 이 없음 */
        sCalcPrec = gDataTypeDefinition[DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE].mDefFractionalSecondPrec;
        sDeclPrecPtr = NULL;
    }
    else
    {
        sCalcPrec = aDeclaredFractionPrec;
        sDeclPrec = aDeclaredFractionPrec;
        sDeclPrecPtr = & sDeclPrec;
    }
    
    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE,
                                        sCalcPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclPrecPtr,      /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      & sCalcPrec,       /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**********************************************************************
 * Interval 유형의 컬럼
 **********************************************************************/

/**
 * @brief INTERVAL YEAR TO MONTH 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  선언한 타입의 이름
 * @param[in]     aIndicator         컬럼의 Interval 유형
 *                <BR> 다음 중 하나의 값이 가능하다.
 *                <BR> - DTL_INTERVAL_INDICATOR_YEAR
 *                <BR> - DTL_INTERVAL_INDICATOR_MONTH
 *                <BR> - DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : INTERVAL
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - INTERVAL YEAR
 *                <BR> - INTERVAL MONTH
 *                <BR> - INTERVAL YEAR TO MONTH
 * @param[in]     aDeclaredStartPrec (Leading Precision) 사용자가 정의한 시작 범위의 Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - YEAR(2) TO MONTH : 2 값에 해당
 * @param[in]     aDeclaredEndPrec (Fractional Precision)  사용자가 정의한 종료 범위의 Precision 값
 *                <BR> - 사용자가 직접 정의 할 수 없으며, DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - YEAR(2) TO MONTH : DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
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
                                                      ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sLeadingPrec     = 0;
    stlInt32 * sFractionPrecPtr = NULL;

    stlInt64   sDeclLeadingPrec    = 0;
    stlInt64 * sDeclLeadingPtrPrec = NULL;

    stlInt64 * sDeclFractionPrecPtr = NULL;

    stlInt64   sPhyLength = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_INTERVAL_YEAR_TO_MONTH;

    /**
     * - Leading Precision 을 획득
     */

    if ( aDeclaredStartPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 leading precision 이 없음 */
        sLeadingPrec = gDataTypeDefinition[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mDefIntervalPrec;

        sDeclLeadingPtrPrec = NULL;
    }
    else
    {
        sLeadingPrec = aDeclaredStartPrec;

        sDeclLeadingPrec    = sLeadingPrec;
        sDeclLeadingPtrPrec = & sDeclLeadingPrec;
    }
    
    /**
     * - End Precision 을 획득
     */

    sFractionPrecPtr     = NULL;
    sDeclFractionPrecPtr = NULL;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                                        sLeadingPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclFractionPrecPtr, /* DECLARED_NUMERIC_PRECISION */
                                      sDeclLeadingPtrPrec,  /* DECLARED_NUMERIC_SCALE */
                                      sFractionPrecPtr,  /* DATETIME_PRECISION */
                                      (stlChar *) gDtlIntervalIndicatorString[aIndicator],
                                      (stlInt32 *) & aIndicator, /* INTERVAL_TYPE_ID */
                                      & sLeadingPrec,    /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief INTERVAL YEAR TO MONTH 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  선언한 타입의 이름
 * @param[in]     aIndicator         컬럼의 Interval 유형
 *                <BR> 다음 중 하나의 값이 가능하다.
 *                <BR> - DTL_INTERVAL_INDICATOR_YEAR
 *                <BR> - DTL_INTERVAL_INDICATOR_MONTH
 *                <BR> - DTL_INTERVAL_INDICATOR_YEAR_TO_MONTH
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : INTERVAL
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - INTERVAL YEAR
 *                <BR> - INTERVAL MONTH
 *                <BR> - INTERVAL YEAR TO MONTH
 * @param[in]     aDeclaredStartPrec (Leading Precision) 사용자가 정의한 시작 범위의 Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - YEAR(2) TO MONTH : 2 값에 해당
 * @param[in]     aDeclaredEndPrec (Fractional Precision)  사용자가 정의한 종료 범위의 Precision 값
 *                <BR> - 사용자가 직접 정의 할 수 없으며, DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - YEAR(2) TO MONTH : DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyIntervalYearToMonthColumnTypeDesc( smlTransId          aTransID,
                                                      smlStatement      * aStmt,
                                                      stlInt64            aColumnID,
                                                      stlChar           * aDeclaredTypeName,
                                                      dtlIntervalIndicator aIndicator,
                                                      stlInt32            aDeclaredStartPrec,
                                                      stlInt32            aDeclaredEndPrec,
                                                      ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sLeadingPrec     = 0;
    stlInt32 * sFractionPrecPtr = NULL;

    stlInt64   sDeclLeadingPrec    = 0;
    stlInt64 * sDeclLeadingPtrPrec = NULL;

    stlInt64 * sDeclFractionPrecPtr = NULL;

    stlInt64   sPhyLength = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_INTERVAL_YEAR_TO_MONTH;

    /**
     * - Leading Precision 을 획득
     */

    if ( aDeclaredStartPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 leading precision 이 없음 */
        sLeadingPrec = gDataTypeDefinition[DTL_TYPE_INTERVAL_YEAR_TO_MONTH].mDefIntervalPrec;

        sDeclLeadingPtrPrec = NULL;
    }
    else
    {
        sLeadingPrec = aDeclaredStartPrec;

        sDeclLeadingPrec    = sLeadingPrec;
        sDeclLeadingPtrPrec = & sDeclLeadingPrec;
    }
    
    /**
     * - End Precision 을 획득
     */

    sFractionPrecPtr     = NULL;
    sDeclFractionPrecPtr = NULL;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_INTERVAL_YEAR_TO_MONTH,
                                        sLeadingPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclFractionPrecPtr, /* DECLARED_NUMERIC_PRECISION */
                                      sDeclLeadingPtrPrec,  /* DECLARED_NUMERIC_SCALE */
                                      sFractionPrecPtr,  /* DATETIME_PRECISION */
                                      (stlChar *) gDtlIntervalIndicatorString[aIndicator],
                                      (stlInt32 *) & aIndicator, /* INTERVAL_TYPE_ID */
                                      & sLeadingPrec,    /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**
 * @brief INTERVAL DAY TO SECOND 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  선언한 타입의 이름
 * @param[in]     aIndicator         컬럼의 Interval 유형
 *                <BR> 다음 중 하나의 값이 가능하다.
 *                <BR> - DTL_INTERVAL_INDICATOR_DAY
 *                <BR> - DTL_INTERVAL_INDICATOR_HOUR
 *                <BR> - DTL_INTERVAL_INDICATOR_MINUTE
 *                <BR> - DTL_INTERVAL_INDICATOR_SECOND
 *                <BR> - DTL_INTERVAL_INDICATOR_DAY_TO_HOUR
 *                <BR> - DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE
 *                <BR> - DTL_INTERVAL_INDICATOR_DAY_TO_SECOND
 *                <BR> - DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE
 *                <BR> - DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND
 *                <BR> - DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : INTERVAL
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - INTERVAL DAY
 *                <BR> - INTERVAL HOUR
 *                <BR> - INTERVAL MINUTE
 *                <BR> - INTERVAL SECOND
 *                <BR> - INTERVAL DAY TO HOUR
 *                <BR> - INTERVAL DAY TO MINUTE
 *                <BR> - INTERVAL DAY TO SECOND
 *                <BR> - INTERVAL HOUR TO MINUTE
 *                <BR> - INTERVAL HOUR TO SECOND
 *                <BR> - INTERVAL MINUTE TO SECOND
 * @param[in]     aDeclaredStartPrec (Leading Precision) 사용자가 정의한 시작 범위의 Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - HOUR(2) TO SECOND(6) : 2 값에 해당
 *                <BR> - HOUR(2) TO MINUTE : 2 값에 해당
 *                <BR> - SECOND(2, 6) : 2 에 해당
 *                <BR> - SECOND(2)    : 2 에 해당 
 * @param[in]     aDeclaredEndPrec (Fractional Precision)  사용자가 정의한 종료 범위의 Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - HOUR(2) TO SECOND(6) : 6 값에 해당 
 *                <BR> - HOUR(2) TO MINUTE : DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - SECOND(2, 6) : 6 에 해당
 *                <BR> - SECOND(2) : DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
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
                                                      ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sLeadingPrec     = 0;
    stlInt32   sFractionPrec    = 0;
    stlInt32 * sFractionPrecPtr = NULL;

    stlInt64   sDeclLeadingPrec    = 0;
    stlInt64 * sDeclLeadingPtrPrec = NULL;

    stlInt64   sDeclFractionPrec    = 0;
    stlInt64 * sDeclFractionPrecPtr = NULL;

    stlInt64   sPhyLength = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_INTERVAL_DAY_TO_SECOND;

    /**
     * - Leading Precision 을 획득
     */

    if ( aDeclaredStartPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 leading precision 이 없음 */
        sLeadingPrec = gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mDefIntervalPrec;

        sDeclLeadingPtrPrec = NULL;
    }
    else
    {
        sLeadingPrec = aDeclaredStartPrec;

        sDeclLeadingPrec    = sLeadingPrec;
        sDeclLeadingPtrPrec = & sDeclLeadingPrec;
    }
    
    /**
     * - End Precision 을 획득
     */

    switch( aIndicator )
    {
        case DTL_INTERVAL_INDICATOR_SECOND :
        case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND :
        case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND :
        case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND :
            if ( aDeclaredEndPrec == DTL_PRECISION_NA )
            {
                /* 사용자가 정의한 end precision 이 없음 */
                sFractionPrec    =
                    gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mDefFractionalSecondPrec;
                sFractionPrecPtr = &sFractionPrec;

                sDeclFractionPrecPtr = NULL;
            }
            else
            {
                sFractionPrec    = aDeclaredEndPrec;
                sFractionPrecPtr = &sFractionPrec;
                
                sDeclFractionPrec    = aDeclaredEndPrec;
                sDeclFractionPrecPtr = & sDeclFractionPrec;
            }
            break;
        default :
            sFractionPrecPtr     = NULL;
            sDeclFractionPrecPtr = NULL;
            break;
    }

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                                        sLeadingPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclFractionPrecPtr, /* DECLARED_NUMERIC_PRECISION */
                                      sDeclLeadingPtrPrec,  /* DECLARED_NUMERIC_SCALE */
                                      sFractionPrecPtr,  /* DATETIME_PRECISION */
                                      (stlChar *) gDtlIntervalIndicatorString[aIndicator],
                                      (stlInt32 *) & aIndicator, /* INTERVAL_TYPE_ID */
                                      & sLeadingPrec,    /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}



/**
 * @brief INTERVAL DAY TO SECOND 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  선언한 타입의 이름
 * @param[in]     aIndicator         컬럼의 Interval 유형
 *                <BR> 다음 중 하나의 값이 가능하다.
 *                <BR> - DTL_INTERVAL_INDICATOR_DAY
 *                <BR> - DTL_INTERVAL_INDICATOR_HOUR
 *                <BR> - DTL_INTERVAL_INDICATOR_MINUTE
 *                <BR> - DTL_INTERVAL_INDICATOR_SECOND
 *                <BR> - DTL_INTERVAL_INDICATOR_DAY_TO_HOUR
 *                <BR> - DTL_INTERVAL_INDICATOR_DAY_TO_MINUTE
 *                <BR> - DTL_INTERVAL_INDICATOR_DAY_TO_SECOND
 *                <BR> - DTL_INTERVAL_INDICATOR_HOUR_TO_MINUTE
 *                <BR> - DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND
 *                <BR> - DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : INTERVAL
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - INTERVAL DAY
 *                <BR> - INTERVAL HOUR
 *                <BR> - INTERVAL MINUTE
 *                <BR> - INTERVAL SECOND
 *                <BR> - INTERVAL DAY TO HOUR
 *                <BR> - INTERVAL DAY TO MINUTE
 *                <BR> - INTERVAL DAY TO SECOND
 *                <BR> - INTERVAL HOUR TO MINUTE
 *                <BR> - INTERVAL HOUR TO SECOND
 *                <BR> - INTERVAL MINUTE TO SECOND
 * @param[in]     aDeclaredStartPrec (Leading Precision) 사용자가 정의한 시작 범위의 Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - HOUR(2) TO SECOND(6) : 2 값에 해당
 *                <BR> - HOUR(2) TO MINUTE : 2 값에 해당
 *                <BR> - SECOND(2, 6) : 2 에 해당
 *                <BR> - SECOND(2)    : 2 에 해당 
 * @param[in]     aDeclaredEndPrec (Fractional Precision)  사용자가 정의한 종료 범위의 Precision 값
 *                <BR> - 사용자가 정의한 값이 없다면, DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - HOUR(2) TO SECOND(6) : 6 값에 해당 
 *                <BR> - HOUR(2) TO MINUTE : DTL_PRECISION_NOT_AVAILABLE
 *                <BR> - SECOND(2, 6) : 6 에 해당
 *                <BR> - SECOND(2) : DTL_PRECISION_NOT_AVAILABLE
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyIntervalDayToSecondColumnTypeDesc( smlTransId          aTransID,
                                                      smlStatement      * aStmt,
                                                      stlInt64            aColumnID,
                                                      stlChar           * aDeclaredTypeName,
                                                      dtlIntervalIndicator aIndicator,
                                                      stlInt32            aDeclaredStartPrec,
                                                      stlInt32            aDeclaredEndPrec,
                                                      ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt32   sLeadingPrec     = 0;
    stlInt32   sFractionPrec    = 0;
    stlInt32 * sFractionPrecPtr = NULL;

    stlInt64   sDeclLeadingPrec    = 0;
    stlInt64 * sDeclLeadingPtrPrec = NULL;

    stlInt64   sDeclFractionPrec    = 0;
    stlInt64 * sDeclFractionPrecPtr = NULL;

    stlInt64   sPhyLength = 0;

    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_INTERVAL_DAY_TO_SECOND;

    /**
     * - Leading Precision 을 획득
     */

    if ( aDeclaredStartPrec == DTL_PRECISION_NA )
    {
        /* 사용자가 정의한 leading precision 이 없음 */
        sLeadingPrec = gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mDefIntervalPrec;

        sDeclLeadingPtrPrec = NULL;
    }
    else
    {
        sLeadingPrec = aDeclaredStartPrec;

        sDeclLeadingPrec    = sLeadingPrec;
        sDeclLeadingPtrPrec = & sDeclLeadingPrec;
    }
    
    /**
     * - End Precision 을 획득
     */

    switch( aIndicator )
    {
        case DTL_INTERVAL_INDICATOR_SECOND :
        case DTL_INTERVAL_INDICATOR_DAY_TO_SECOND :
        case DTL_INTERVAL_INDICATOR_HOUR_TO_SECOND :
        case DTL_INTERVAL_INDICATOR_MINUTE_TO_SECOND :
            if ( aDeclaredEndPrec == DTL_PRECISION_NA )
            {
                /* 사용자가 정의한 end precision 이 없음 */
                sFractionPrec    =
                    gDataTypeDefinition[DTL_TYPE_INTERVAL_DAY_TO_SECOND].mDefFractionalSecondPrec;
                sFractionPrecPtr = &sFractionPrec;

                sDeclFractionPrecPtr = NULL;
            }
            else
            {
                sFractionPrec    = aDeclaredEndPrec;
                sFractionPrecPtr = &sFractionPrec;
                
                sDeclFractionPrec    = aDeclaredEndPrec;
                sDeclFractionPrecPtr = & sDeclFractionPrec;
            }
            break;
        default :
            sFractionPrecPtr     = NULL;
            sDeclFractionPrecPtr = NULL;
            break;
    }

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_INTERVAL_DAY_TO_SECOND,
                                        sLeadingPrec,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      sDeclFractionPrecPtr, /* DECLARED_NUMERIC_PRECISION */
                                      sDeclLeadingPtrPrec,  /* DECLARED_NUMERIC_SCALE */
                                      sFractionPrecPtr,  /* DATETIME_PRECISION */
                                      (stlChar *) gDtlIntervalIndicatorString[aIndicator],
                                      (stlInt32 *) & aIndicator, /* INTERVAL_TYPE_ID */
                                      & sLeadingPrec,    /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;
    
    STL_FINISH;
    
    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}


/**********************************************************************
 * ROWID 유형의 컬럼
 **********************************************************************/

/**
 * @brief ROWID 타입을 위한 Data Type Descriptor 정보를 생성한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aOwnerID           Column 이 속한 Table의 Owner ID
 * @param[in]     aSchemaID          Column 이 속한 Schema ID
 * @param[in]     aTableID           Column 이 속한 Table ID
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : 
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - ROWID
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellInsertRowIdColumnTypeDesc( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aOwnerID,
                                        stlInt64            aSchemaID,
                                        stlInt64            aTableID,
                                        stlInt64            aColumnID,
                                        stlChar           * aDeclaredTypeName,
                                        ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_ROWID].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_ROWID;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_ROWID,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 추가
     */

    STL_TRY( eldInsertColumnTypeDesc( aTransID,
                                      aStmt,
                                      & aOwnerID,        /* OWNER_ID */
                                      & aSchemaID,       /* SCHEMA_ID */
                                      & aTableID,        /* TABLE_ID */
                                      & aColumnID,       /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}




/**
 * @brief ROWID 타입을 위한 Data Type Descriptor 정보를 변경한다.
 * @param[in]     aTransID           Transaction ID
 * @param[in]     aStmt              Statement
 * @param[in]     aColumnID          Column ID
 * @param[in]     aDeclaredTypeName  사용자가 정의한 데이타 타입 이름 (null-terminated)
 *                <BR> 표준 이름 : 
 *                <BR> 다음과 같은 이름이 가능하다.
 *                <BR> - ROWID
 * @param[in,out] aEnv       Execution Library Environment
 * @remarks
 */
stlStatus ellModifyRowIdColumnTypeDesc( smlTransId          aTransID,
                                        smlStatement      * aStmt,
                                        stlInt64            aColumnID,
                                        stlChar           * aDeclaredTypeName,
                                        ellEnv            * aEnv )
{
    stlChar  * sNormalTypeName = NULL;
    stlInt32   sDataTypeID = 0;

    stlInt64   sPhyLength = 0;
    
    /*
     * Parameter Validation
     */

    STL_PARAM_VALIDATE( aStmt != NULL, ELL_ERROR_STACK(aEnv) );

    /**
     * - Standard Type Name 을 획득
     */
    
    sNormalTypeName = gDataTypeDefinition[DTL_TYPE_ROWID].mSqlNormalTypeName;
    sDataTypeID = DTL_TYPE_ROWID;

    /**
     * Physical Length 획득
     */
    
    STL_TRY( dtlGetDataValueBufferSize( DTL_TYPE_ROWID,
                                        DTL_LENGTH_NA,
                                        DTL_STRING_LENGTH_UNIT_NA,
                                        & sPhyLength,
                                        KNL_DT_VECTOR(aEnv),
                                        aEnv,
                                        KNL_ERROR_STACK( aEnv ) )
             == STL_SUCCESS );
    
    /**
     * - 컬럼의 data type descriptor 변경
     */

    STL_TRY( eldModifyColumnTypeDesc( aTransID,
                                      aStmt,
                                      aColumnID,         /* COLUMN_ID */
                                      sNormalTypeName,   /* DATA_TYPE */
                                      & sDataTypeID,     /* DATA_TYPE_ID */
                                      NULL,              /* STRING_LENGTH_UNIT */
                                      NULL,              /* CHARACTER_MAXIMUM_LENGTH */
                                      NULL,              /* CHARACTER_OCTET_LENGTH */
                                      NULL,              /* NUMERIC_PRECISION */
                                      NULL,              /* NUMERIC_PRECISION_RADIX */
                                      NULL,              /* NUMERIC_SCALE */
                                      aDeclaredTypeName, /* DECLARED_DATA_TYPE */
                                      NULL,              /* DECLARED_NUMERIC_PRECISION */
                                      NULL,              /* DECLARED_NUMERIC_SCALE */
                                      NULL,              /* DATETIME_PRECISION */
                                      NULL,              /* INTERVAL_TYPE */
                                      NULL,              /* INTERVAL_TYPE_ID */
                                      NULL,              /* INTERVAL_PRECISION */
                                      sPhyLength,        /* PHYSICAL_MAXIMUM_LENGTH */
                                      aEnv )
             == STL_SUCCESS );
    
    STL_TRY( eldClearOPMem( aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    (void) eldClearOPMem( aEnv );
    
    return STL_FAILURE;
}






/** @} ellColumnType */

