/*******************************************************************************
 * knxFixedTable.c
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
 * @file knxFixedTable.c
 * @brief Fixed Table 관리 루틴
 */

#include <knl.h>

#include <knxDef.h>
#include <knxFixedTable.h>
#include <knxBuiltInFixedTable.h>

/**
 * @addtogroup KNX
 * @{
 */

/**
 * @brief Fixed Table Manager
 */
knxFxTableMgr gKnxFxTableMgr;

/**
 * @brief Fixed Table 용 메모리 관리자를 초기화한다.
 * @param[in]  aFxMgr   Fixed Table Manager
 * @param[in]  aEnv     Environment
 * @remarks
 */
stlStatus knxFxTableMemInitialize( knxFxTableMgr * aFxMgr,
                                   knlEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxMgr != NULL, KNL_ERROR_STACK(aEnv) );


    /**
     * - Process마다 정의한 Fixed Table이 다를 수 있으므로, Process 별로 관리한다.
     * - Life Time은 Process 시작 -> 종료이다.
     * @todo KNL_MEM_CATEGORY_PROCESS 가 구현되면 변경해야 함.
     */
    STL_TRY( knlCreateRegionMem( & aFxMgr->mMemMgr,
                                 KNL_ALLOCATOR_KERNEL_FIXED_TABLE,
                                 NULL,  /* aParentMem */
                                 KNL_MEM_STORAGE_TYPE_HEAP,
                                 KNX_FXTABLE_MEM_INIT_SIZE,
                                 KNX_FXTABLE_MEM_NEXT_SIZE,
                                 aEnv )
             == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 용 메모리 관리자를 종료한다.
 * @param[in]  aFxMgr   Fixed Table Manager
 * @param[in]  aEnv     Environment
 * @remarks
 */
stlStatus knxFxTableMemFinalize( knxFxTableMgr * aFxMgr,
                                 knlEnv        * aEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxMgr != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * Fixed Table Cache를 위한 메모리 관리자를 종료
     */
    STL_TRY( knlDestroyRegionMem( & aFxMgr->mMemMgr, aEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 을 위한 메모리 공간을 할당한다.
 * @param[in]  aFxMgr   Fixed Table Manager
 * @param[out] aMemory  할당받을 메모리 Pointer
 * @param[in]  aSize    할당할 공간의 크기 
 * @param[in]  aEnv     Environment
 * @remarks
 */
stlStatus knxFxTableMemAlloc ( knxFxTableMgr   * aFxMgr,
                               void           ** aMemory,
                               stlInt32          aSize,
                               knlEnv          * aEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxMgr != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aMemory != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aSize > 0, KNL_ERROR_STACK(aEnv) );

    /**
     * Region Memory 를 할당받는다.
     */
    
    STL_TRY( knlAllocRegionMem( & aFxMgr->mMemMgr,
                                aSize,
                                aMemory,
                                aEnv )
             == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}




/**
 * @brief Fixed Table 관리를 위한 초기화를 한다.
 * @param[in,out] aKnlEnv      Kernel Environment
 * @remarks
 */
stlStatus knxInitializeFxTableMgr( knlEnv * aKnlEnv )
{
    /**
     * Fixed Table Manager를 초기화한다.
     */
    stlMemset( & gKnxFxTableMgr, 0x00, STL_SIZEOF(knxFxTableMgr) );

    /**
     * Fixed Table 용 Memory Manager를 초기화한다.
     */
    
    STL_TRY( knxFxTableMemInitialize( & gKnxFxTableMgr, aKnlEnv ) == STL_SUCCESS );

    gKnxFxTableMgr.mFxTableCount = 0;
    STL_RING_INIT_HEADLINK( &gKnxFxTableMgr.mFxTableList,
                            STL_OFFSETOF( knxFxTableHeapDesc, mLink ) );         
    gKnxFxTableMgr.mFxHeapDescMap = NULL;
                                  
    /**************************************************
     * Built-In Fixed Table 들을 등록한다.
     **************************************************/

    /**
     * - X$TABLES 등록
     */
    
    STL_TRY( knxRegisterFixedTable( &gXTablesTabDesc, aKnlEnv ) == STL_SUCCESS );

    /**
     * - X$COLUMNS 등록
     */
    
    STL_TRY( knxRegisterFixedTable( &gXColumnsTabDesc, aKnlEnv ) == STL_SUCCESS );
    
    /**
     * - X$ERROR_CODE 등록
     */
    
    STL_TRY( knxRegisterFixedTable( &gXErrCodeTabDesc, aKnlEnv ) == STL_SUCCESS );
    

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 관리를 종료한다.
 * @param[in,out] aKnlEnv      Kernel Environment
 * @remarks
 */
stlStatus knxFinalizeFxTableMgr( knlEnv * aKnlEnv )
{
    STL_TRY( knxFxTableMemFinalize( & gKnxFxTableMgr, aKnlEnv ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 개발자가 정의한 Fixed Table을 등록한다.
 * @param[in]     aFxTableDesc 개발자가 정의한 Fixed Table Descriptor
 * @param[in,out] aKnlEnv      Kernel Environment
 * @par Error Codes:
 * @code
 * [KNL_ERRCODE_TOO_LONG_IDENTIFIER]
 * Fixed Table의 Identifier의 이름이 너무 길다.
 * - STL_MAX_SQL_IDENTIFIER_LENGTH(128) 보다 작아야 함.
 * [KNL_ERRCODE_TOO_LONG_COMMENT]
 * Fixed Table의 주석의 내용이 너무 길다.
 * - KNL_MAX_FIXEDTABLE_COMMENT_LENGTH(1024) 보다 작아야 함.
 * @endcode
 * @remarks
 */
stlStatus knxRegisterFixedTable( knlFxTableDesc * aFxTableDesc,
                                 knlEnv         * aKnlEnv )
{
    knxFxTableHeapDesc   * sTabDef     = NULL;
    
    knlFxColumnDesc      * sColDesc    = NULL;
    stlInt32               sColCount   = 0;

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );

    STL_TRY_THROW( stlStrlen(aFxTableDesc->mTableName)
                   < STL_MAX_SQL_IDENTIFIER_LENGTH,
                   RAMP_ERR_TOO_LONG_IDENTIFIER );

    STL_TRY_THROW( stlStrlen(aFxTableDesc->mTableComment)
                   < KNL_MAX_FIXEDTABLE_COMMENTS_LENGTH,
                   RAMP_ERR_TOO_LONG_COMMENT );
    
    /**
     * 입력받은 Table Descriptor 정보를 이용해 Table Definition을 구성한다.
     */
    
    /**
     * - Table Definition 메모리 할당
     */
    STL_TRY( knxFxTableMemAlloc( & gKnxFxTableMgr,
                                 (void**) &sTabDef,
                                 STL_SIZEOF( knxFxTableHeapDesc ),
                                 aKnlEnv )
             == STL_SUCCESS );

    /**
     * - Table Definition Link 정보 초기화
     */
    STL_RING_INIT_DATALINK( sTabDef,
                            STL_OFFSETOF( knxFxTableHeapDesc, mLink ) );
    
    /**
     * - Table Descriptor 정보 복사
     */

    sTabDef->mUsageType         = aFxTableDesc->mUsageType;
    sTabDef->mStartupPhase      = aFxTableDesc->mStartupPhase;

    sTabDef->mGetDumpObjectFunc = aFxTableDesc->mGetDumpObjectFunc;
    sTabDef->mOpenFunc          = aFxTableDesc->mOpenFunc;
    sTabDef->mCloseFunc         = aFxTableDesc->mCloseFunc;
    sTabDef->mBuildRecordFunc   = aFxTableDesc->mBuildRecordFunc;
    
    sTabDef->mPathCtrlSize    = aFxTableDesc->mPathCtrlSize;
    
    sTabDef->mTableName       = aFxTableDesc->mTableName;
    sTabDef->mTableComment    = aFxTableDesc->mTableComment;
    
    sTabDef->mColumnDesc      = aFxTableDesc->mColumnDesc;

    /**
     * - Table Definition의 컬럼 갯수 계산
     */
    sColCount   = 0;
    for ( sColDesc = sTabDef->mColumnDesc;
          sColDesc->mColumnName != NULL;
          sColDesc = &sTabDef->mColumnDesc[sColCount] )
    {
        STL_TRY_THROW( stlStrlen(sColDesc->mColumnName)
                       < STL_MAX_SQL_IDENTIFIER_LENGTH,
                       RAMP_ERR_TOO_LONG_IDENTIFIER );
        sColCount++;
    }
    sTabDef->mColumnCount = sColCount;

    /**
     * Table Definition을 Fixed Table Manager에 등록
     */
    STL_RING_ADD_LAST( &gKnxFxTableMgr.mFxTableList, sTabDef );

    gKnxFxTableMgr.mFxTableCount++;
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_TOO_LONG_IDENTIFIER )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_TOO_LONG_IDENTIFIER,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }
    STL_CATCH( RAMP_ERR_TOO_LONG_COMMENT )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      KNL_ERRCODE_TOO_LONG_COMMENT,
                      NULL,
                      KNL_ERROR_STACK(aKnlEnv) );
    }

    STL_FINISH;

    /**
     * Startup 단계에서 Fixed Table 등록 도중 발생하는 에러상황임.
     * 할당받은 메모리를 개별적으로 제거하지 않는다
     * 에러 발생후 종료과정에서 모든 메모리가 제거된다.
     */
    return STL_FAILURE;
}


/**
 * @brief 레코드를 위한 Value를 구성한다.
 * @param[in]     aFxColumnDesc 개발자가 정의한 Fixed Table 의 Column Descriptor
 * @param[in]     aStructData   입력된 소스 데이터
 * @param[out]    aValueList    출력할 데이터 공간 ( nullable, ex, COUNT(*) )
 * @param[in]     aBlockIdx     Block Idx
 * @param[in,out] aKnlEnv       Kernel Environment
 * @remarks
 */
stlStatus knxBuildFxRecord( knlFxColumnDesc   * aFxColumnDesc,
                            void              * aStructData,
                            knlValueBlockList * aValueList,
                            stlInt32            aBlockIdx,
                            knlEnv            * aKnlEnv )
{
    knlFxColumnDesc * sColDesc = NULL;
    
    knlValueBlockList * sValList = NULL;
    
    stlChar         * sValStr  = NULL;

    stlInt16          sInt16Value   = 0;
    stlInt32          sInt32Value   = 0;
    stlInt64          sInt64Value   = 0;
    stlFloat32        sFloat32Value = 0;
    stlFloat64        sFloat64Value = 0;
    stlBool           sBoolValue    = STL_FALSE;
    stlTime           sTimeValue    = 0;

    stlBool           sSuccessWithInfo = STL_FALSE;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxColumnDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aStructData != NULL, KNL_ERROR_STACK(aKnlEnv) );

    STL_TRY( knlCheckQueryTimeout( aKnlEnv ) == STL_SUCCESS );

    for ( sValList = aValueList;
          sValList != NULL;
          sValList = sValList->mNext )
    {
        sColDesc = & aFxColumnDesc[sValList->mColumnOrder];
        
        /**
         * Data Type 유형별 값을 저장
         */
        switch ( sColDesc->mDataType )
        {
            case KNL_FXTABLE_DATATYPE_VARCHAR:
            case KNL_FXTABLE_DATATYPE_LONGVARCHAR:
                {
                    /**
                     * 모든 C String은 NULL-terminated 를 가정하고 있다.
                     * String 의 시작 주소 계산
                     */

                    sValStr = (stlChar*)aStructData + sColDesc->mOffset;
                    
                    /**
                     * String 으로부터 DB Value 로 변환
                     */
                    
                    STL_TRY( dtlSetValueFromString(
                                 KNL_GET_BLOCK_DB_TYPE(sValList),
                                 sValStr,
                                 stlStrlen(sValStr),
                                 sColDesc->mLength,    /* precision */
                                 0,                    /* scale */
                                 DTL_STRING_LENGTH_UNIT_OCTETS, /* string length unit */
                                 DTL_INTERVAL_INDICATOR_NA,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValList), 
                                 KNL_GET_BLOCK_DATA_VALUE(sValList, aBlockIdx),
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aKnlEnv),
                                 aKnlEnv,
                                 KNL_DT_VECTOR(aKnlEnv),
                                 aKnlEnv,
                                 KNL_ERROR_STACK(aKnlEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case KNL_FXTABLE_DATATYPE_VARCHAR_POINTER:
            case KNL_FXTABLE_DATATYPE_LONGVARCHAR_POINTER:
                {
                    /**
                     * 모든 C String은 NULL-terminated 를 가정하고 있다.
                     * String 의 시작 주소 계산
                     */
                    sValStr = *(stlChar**)((stlChar*)aStructData + sColDesc->mOffset);
                        
                    /**
                     * String 으로부터 DB Value 로 변환
                     */
                    
                    STL_TRY( dtlSetValueFromString(
                                 KNL_GET_BLOCK_DB_TYPE(sValList),
                                 sValStr,
                                 stlStrlen(sValStr),
                                 sColDesc->mLength,    /* precision */
                                 0,                    /* scale */
                                 DTL_STRING_LENGTH_UNIT_OCTETS, /* string length unit */
                                 DTL_INTERVAL_INDICATOR_NA,     /* interval indicator */
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValList), 
                                 KNL_GET_BLOCK_DATA_VALUE(sValList, aBlockIdx),
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aKnlEnv),
                                 aKnlEnv,
                                 KNL_DT_VECTOR(aKnlEnv),
                                 aKnlEnv,
                                 KNL_ERROR_STACK(aKnlEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case KNL_FXTABLE_DATATYPE_SMALLINT:
                {
                    /**
                     * C type 정수형 값을 획득
                     */
                    
                    sInt16Value =
                        *(stlInt16*) ( (stlChar *)aStructData + sColDesc->mOffset );

                    /**
                     * C 정수값으로부터 DB Value 로 변환
                     */

                    STL_TRY( dtlSetValueFromInteger(
                                 KNL_GET_BLOCK_DB_TYPE(sValList),
                                 (stlInt64) sInt16Value,
                                 0, /* precision */
                                 0, /* scale */
                                 DTL_STRING_LENGTH_UNIT_NA,  /* string length unit */
                                 DTL_INTERVAL_INDICATOR_NA,  /* interval indicator */
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValList), 
                                 KNL_GET_BLOCK_DATA_VALUE(sValList, aBlockIdx),
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aKnlEnv),
                                 aKnlEnv,
                                 KNL_ERROR_STACK(aKnlEnv) )
                             == STL_SUCCESS );
                                                     
                    break;
                }
            case KNL_FXTABLE_DATATYPE_INTEGER:
                {
                    /**
                     * C type 정수형 값을 획득
                     */
                    
                    sInt32Value =
                        *(stlInt32*) ( (stlChar *)aStructData + sColDesc->mOffset );
                    
                    /**
                     * C 정수값으로부터 DB Value 로 변환
                     */

                    STL_TRY( dtlSetValueFromInteger(
                                 KNL_GET_BLOCK_DB_TYPE(sValList),
                                 (stlInt64) sInt32Value,
                                 0, /* precision */
                                 0, /* scale */
                                 DTL_STRING_LENGTH_UNIT_NA, /* string length unit */
                                 DTL_INTERVAL_INDICATOR_NA, /* interval indicator */
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValList), 
                                 KNL_GET_BLOCK_DATA_VALUE(sValList, aBlockIdx),
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aKnlEnv),
                                 aKnlEnv,
                                 KNL_ERROR_STACK(aKnlEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case KNL_FXTABLE_DATATYPE_BIGINT:
                {
                    /**
                     * C type 정수형 값을 획득
                     */
                    
                    sInt64Value =
                        *(stlInt64*) ( (stlChar *)aStructData + sColDesc->mOffset );
                    
                    /**
                     * C 정수값으로부터 DB Value 로 변환
                     */

                    STL_TRY( dtlSetValueFromInteger(
                                 KNL_GET_BLOCK_DB_TYPE(sValList),
                                 sInt64Value,
                                 0, /* precision */
                                 0, /* scale */
                                 DTL_STRING_LENGTH_UNIT_NA, /* string length unit */
                                 DTL_INTERVAL_INDICATOR_NA, /* interval indicator */
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValList), 
                                 KNL_GET_BLOCK_DATA_VALUE(sValList, aBlockIdx),
                                 & sSuccessWithInfo,
                                 KNL_DT_VECTOR(aKnlEnv),
                                 aKnlEnv,
                                 KNL_ERROR_STACK(aKnlEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case KNL_FXTABLE_DATATYPE_REAL:
                {
                    /**
                     * - REAL Data Type 인 경우, 메모리 복사.
                     */
                    sFloat32Value =
                        *(stlFloat32*) ( (stlChar *)aStructData + sColDesc->mOffset );
                    KNL_SET_BLOCK_DATA_LENGTH( sValList, aBlockIdx, sColDesc->mLength );
                    stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sValList, aBlockIdx ),
                               & sFloat32Value,
                               sColDesc->mLength );
                    break;
                }
            case KNL_FXTABLE_DATATYPE_DOUBLE:
                {
                    /**
                     * - DOUBLE Data Type 인 경우, 메모리 복사.
                     */
                    sFloat64Value =
                        *(stlFloat64*) ( (stlChar *)aStructData + sColDesc->mOffset );
                    KNL_SET_BLOCK_DATA_LENGTH( sValList, aBlockIdx, sColDesc->mLength );
                    stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sValList, aBlockIdx ),
                               & sFloat64Value,
                               sColDesc->mLength );
                    break;
                }
            case KNL_FXTABLE_DATATYPE_BOOL:
                {
                    /**
                     * - BOOL Data Type 인 경우, 메모리 복사.
                     */
                    sBoolValue =
                        *(stlBool *) ( (stlChar *)aStructData + sColDesc->mOffset );
                    KNL_SET_BLOCK_DATA_LENGTH( sValList, aBlockIdx, sColDesc->mLength );
                    stlMemcpy( KNL_GET_BLOCK_DATA_PTR( sValList, aBlockIdx ),
                               & sBoolValue,
                               sColDesc->mLength );
                    break;
                }
            case KNL_FXTABLE_DATATYPE_TIMESTAMP:
                {
                    /**
                     * C Time 획득
                     */
                    
                    sTimeValue =
                        *(stlTime *) ( (stlChar *)aStructData + sColDesc->mOffset );
                    
                    /**
                     * C 정수값으로부터 DB Value 로 변환
                     */

                    STL_TRY( dtlTimestampSetValueFromStlTime(
                                 sTimeValue,
                                 gDefaultDataTypeDef[ DTL_TYPE_TIMESTAMP ].mArgNum1,
                                 gDefaultDataTypeDef[ DTL_TYPE_TIMESTAMP ].mArgNum2,
                                 KNL_GET_BLOCK_DATA_BUFFER_SIZE(sValList),
                                 KNL_GET_BLOCK_DATA_VALUE(sValList, aBlockIdx),
                                 KNL_DT_VECTOR(aKnlEnv),
                                 aKnlEnv,
                                 KNL_ERROR_STACK(aKnlEnv) )
                             == STL_SUCCESS );
                    break;
                }
            case KNL_FXTABLE_DATATYPE_MAX:
            default:
                {
                    /**
                     * - 잘못된 Data Type 정의를 사용한 경우엔 에러.
                     */
                    STL_PARAM_VALIDATE( 0, KNL_ERROR_STACK(aKnlEnv) );
                    break;
                }
        }
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 등록된 Fixed Table의 갯수를 얻는다.
 * @param[out]    aFxTableCount Fixed Table의 갯수
 * @param[in,out] aKnlEnv        Kernel Environment
 * @remarks
 */
stlStatus knxGetFixedTableCount( stlInt32 * aFxTableCount,
                                 knlEnv   * aKnlEnv )
{
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableCount != NULL, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * Fixed Table Manager의 갯수 설정
     */
    
    *aFxTableCount = gKnxFxTableMgr.mFxTableCount;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 지정한 위치의 Fixed Table 의 Heap Descriptor 를 얻는다.
 * @param[out]    aFxTableHeapDesc Fixed Table 의 Heap Descriptor
 * @param[in]     aIdx             원하는 위치(0부터 시작)
 * @param[in,out] aKnlEnv          Kernel Environment
 * @remarks
 */
stlStatus knxGetNthFixedTableDesc( void    ** aFxTableHeapDesc,
                                   stlInt32   aIdx,
                                   knlEnv   * aKnlEnv )
{
    stlInt32  i = 0;

    knxFxTableHeapDesc * sFxDesc = NULL;
    knxFxTableHeapDesc * sFoundFxDesc = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIdx >= 0, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aIdx < gKnxFxTableMgr.mFxTableCount, KNL_ERROR_STACK(aKnlEnv) );

    /**
     * N번째  Fixed Table 정보 획득 
     */

    i = 0;

    sFoundFxDesc = NULL;
    STL_RING_FOREACH_ENTRY( & gKnxFxTableMgr.mFxTableList, sFxDesc )
    {
        if ( i == aIdx )
        {
            sFoundFxDesc = sFxDesc;
            break;
        }
        i++;
    }

    STL_DASSERT( sFoundFxDesc != NULL );
    *aFxTableHeapDesc = sFoundFxDesc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Table 의 Usage Type을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxUsageType      Fixed Table Type
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetFixedTableUsageType( void                * aFxTableHeapDesc,
                                     knlFxTableUsageType * aFxUsageType,
                                     knlEnv              * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aFxUsageType     != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    stlMemcpy( aFxUsageType,
               & sFxTabDef->mUsageType,
               STL_SIZEOF(knlFxTableUsageType) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 의 사용가능한 Startup 단계를 얻는다.
 * @param[in]     aFxTableHeapDesc      Fixed Table의 Heap Descriptor
 * @param[out]    aUsableStartupPhase   Fixed Table 의 사용가능한 Startup Phase
 * @param[in,out] aKnlEnv               Kernel Environment
 */
stlStatus knxGetFixedTableUsablePhase( void            * aFxTableHeapDesc,
                                       knlStartupPhase * aUsableStartupPhase,
                                       knlEnv          * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableHeapDesc    != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aUsableStartupPhase != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    stlMemcpy( aUsableStartupPhase,
               & sFxTabDef->mStartupPhase,
               STL_SIZEOF(knlStartupPhase) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 의 Path Control 을 위한 공간의 크기를 얻는다.
 * @param[in]     aFxTableHeapDesc Fixed Table의 Heap Descriptor
 * @param[out]    aFxPathCtrlSize  Fixed Table 의 Path Control을 위한 공간의 크기
 * @param[in,out] aKnlEnv          Kernel Environment
 */
stlStatus knxGetFixedTablePathCtrlSize( void     * aFxTableHeapDesc,
                                        stlInt32 * aFxPathCtrlSize,
                                        knlEnv   * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aFxPathCtrlSize  != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    stlMemcpy( aFxPathCtrlSize,
               & sFxTabDef->mPathCtrlSize,
               STL_SIZEOF(stlInt32) );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 의 이름을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxTableName      Fixed Table 의 이름
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetFixedTableName( void     * aFxTableHeapDesc,
                                stlChar ** aFxTableName,
                                knlEnv   * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aFxTableName     != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    *aFxTableName = sFxTabDef->mTableName;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Table 의 주석을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxTableComment   Fixed Table 의 주석
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetFixedTableComment( void     * aFxTableHeapDesc,
                                   stlChar ** aFxTableComment,
                                   knlEnv   * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aFxTableComment  != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    *aFxTableComment = sFxTabDef->mTableComment;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 의 컬럼갯수를 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxColumnCount    Fixed Table 의 Column 갯수
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetFixedColumnCount( void             * aFxTableHeapDesc,
                                  stlInt32         * aFxColumnCount,
                                  knlEnv           * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aFxColumnCount   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    stlMemcpy( aFxColumnCount,
               & sFxTabDef->mColumnCount,
               STL_SIZEOF(stlInt32) );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 의 Column Descriptor Array 정보를 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxColumnDesc     Fixed Table 의 Column Descriptor Array
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetFixedColumnDesc( void             * aFxTableHeapDesc,
                                 knlFxColumnDesc ** aFxColumnDesc,
                                 knlEnv           * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;

    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aFxColumnDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    *aFxColumnDesc = sFxTabDef->mColumnDesc;
        
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 의 Column Descriptor Array 정보를 얻는다.
 * @param[in]     aFxColumnDesc   Fixed Column Descriptor
 * @param[out]    aDtlDataTypeID  Data Type Layer 의 Data Type ID
 * @param[in,out] aKnlEnv         Kernel Environment
 */
stlStatus knxGetDataTypeIDOfFixedColumn( knlFxColumnDesc * aFxColumnDesc,
                                         dtlDataType     * aDtlDataTypeID,
                                         knlEnv          * aKnlEnv )
{
    dtlDataType sDataType = DTL_TYPE_MAX;
    
    /**
     * Parameter Validation
     */
    STL_PARAM_VALIDATE( aFxColumnDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aDtlDataTypeID!= NULL, KNL_ERROR_STACK(aKnlEnv) );

    switch ( aFxColumnDesc->mDataType )
    {
        case KNL_FXTABLE_DATATYPE_VARCHAR :
            sDataType = DTL_TYPE_VARCHAR;
            break;
        case KNL_FXTABLE_DATATYPE_VARCHAR_POINTER :
            sDataType = DTL_TYPE_VARCHAR;
            break;
        case KNL_FXTABLE_DATATYPE_LONGVARCHAR :
            sDataType = DTL_TYPE_LONGVARCHAR;
            break;
        case KNL_FXTABLE_DATATYPE_LONGVARCHAR_POINTER :
            sDataType = DTL_TYPE_LONGVARCHAR;
            break;
        case KNL_FXTABLE_DATATYPE_SMALLINT :
            sDataType = DTL_TYPE_NATIVE_SMALLINT;
            break;
        case KNL_FXTABLE_DATATYPE_INTEGER :
            sDataType = DTL_TYPE_NATIVE_INTEGER;
            break;
        case KNL_FXTABLE_DATATYPE_BIGINT :
            sDataType = DTL_TYPE_NATIVE_BIGINT;
            break;
        case KNL_FXTABLE_DATATYPE_REAL :
            sDataType = DTL_TYPE_NATIVE_REAL;
            break;
        case KNL_FXTABLE_DATATYPE_DOUBLE :
            sDataType = DTL_TYPE_NATIVE_DOUBLE;
            break;
        case KNL_FXTABLE_DATATYPE_BOOL :
            sDataType = DTL_TYPE_BOOLEAN;
            break;
        case KNL_FXTABLE_DATATYPE_TIMESTAMP :
            sDataType = DTL_TYPE_TIMESTAMP;
            break;
        default :
            STL_PARAM_VALIDATE( 1 == 0, KNL_ERROR_STACK(aKnlEnv) );
            break;
    }

    *aDtlDataTypeID = sDataType;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Table 의 Get DumpObject Handle Callback Function을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aGetDumpCallback  Dump Object 의 Get Handle Callback Function
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetDumpObjHandleCallback( void                     * aFxTableHeapDesc,
                                       knlGetDumpObjectCallback * aGetDumpCallback,
                                       knlEnv                   * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;

    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aGetDumpCallback != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    
    *aGetDumpCallback = sFxTabDef->mGetDumpObjectFunc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Fixed Table 의 Open Callback Function을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aOpenCallback     Fixed Table 의 Open Callback Function
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetFixedTableOpenCallback( void                    * aFxTableHeapDesc,
                                        knlOpenFxTableCallback  * aOpenCallback,
                                        knlEnv                  * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;

    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aOpenCallback    != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    
    *aOpenCallback = sFxTabDef->mOpenFunc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 의 Close Callback Function을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aCloseCallback    Fixed Table 의 Close Callback Function
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetFixedTableCloseCallback( void                    * aFxTableHeapDesc,
                                         knlCloseFxTableCallback * aCloseCallback,
                                         knlEnv                  * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;

    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aCloseCallback   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    
    *aCloseCallback = sFxTabDef->mCloseFunc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Table 의 Build Record Callback Function을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aBuildCallback    Fixed Table 의 Build Record Callback Function
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knxGetFixedTableBuildRecordCallback( void                     * aFxTableHeapDesc,
                                               knlBuildFxRecordCallback * aBuildCallback,
                                               knlEnv                   * aKnlEnv )
{
    knxFxTableHeapDesc * sFxTabDef = NULL;

    STL_PARAM_VALIDATE( aFxTableHeapDesc != NULL, KNL_ERROR_STACK(aKnlEnv) );
    STL_PARAM_VALIDATE( aBuildCallback   != NULL, KNL_ERROR_STACK(aKnlEnv) );

    sFxTabDef = (knxFxTableHeapDesc *) aFxTableHeapDesc;
    
    *aBuildCallback = sFxTabDef->mBuildRecordFunc;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief Fixed Dump Table의 Dump Option String으로부터 Schema Name,
 *   <BR> Object Name, Option Name을 얻는다.
 * @param[in,out] aDumpOptionStr  Dump Option String
 * @param[out]    aToken1         첫번째 Token
 * @param[out]    aToken2         두번째 Token
 * @param[out]    aToken3         세번째 Token
 * @param[out]    aTokenCount     유효한 Token의 개수
 * @param[in,out] aEnv            Kernel Environment
 * @remarks
 * <BR> Option String의 Delimeter는 ','이고, Option내에서의 Token Delimeter는 '.'이다.
 * <BR>
 * <BR> 예) D$DUMP_TABLE( 'schema_name.t1, 0, 1, 2' )
 * <BR> 위는 4개의 Option을 가지는 예제이다.
 * <BR> 각 Option은 "schema_name.t1", "0", "1", "2"가 된다.
 * <BR> 특히, 첫번째 Option은 2개의 Token을 가지며 각 Token은 "Schema_name"과 "t1"으로 파싱된다.
 * @note
 * <BR> 한개의 Option에 설정될수 있는 Token의 최대 개수는 3개이며,
 * <BR> 각 Token들을 aToken1, aToken2, aToken3에 설정된다.
 * <BR> 만약 4개의 Token을 갖는 스트링의 경우("schema_name.tablespace.t1.xxx")라면
 * <BR> 에러는 발생하지 않지만 4번째 Token은 무시하게 된다.
 * <BR> 또한, Tokenizing하는 동안 aDumpOptionStr 원본에 NULL 문자가 기록될수 있기 때문에
 * <BR> 원본이 변경되도 상관없는 경우에만 사용해야 한다.
 */
stlStatus knxTokenizeNextDumpOption( stlChar  ** aDumpOptionStr,
                                     stlChar  ** aToken1,
                                     stlChar  ** aToken2,
                                     stlChar  ** aToken3,
                                     stlInt32  * aTokenCount,
                                     knlEnv    * aEnv )
{
    stlChar * sOptionDelimeter = ",";
    stlChar * sTokenDelimeter = ".";
    stlChar * sOption;
    stlChar * sTokenSavePtr;

    STL_PARAM_VALIDATE( aDumpOptionStr != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aToken1 != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aToken2 != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aToken3 != NULL, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( aTokenCount != NULL, KNL_ERROR_STACK(aEnv) );
    
    *aTokenCount = 0;
    *aToken1 = NULL;
    *aToken2 = NULL;
    *aToken3 = NULL;
    
    sOption = stlStrtok( *aDumpOptionStr,
                         sOptionDelimeter,
                         aDumpOptionStr );

    if( sOption != NULL )
    {
        *aToken1 = stlStrtok( sOption,
                              sTokenDelimeter,
                              &sTokenSavePtr );
        if( *aToken1 != NULL )
        {
            (*aTokenCount)++;
        }

        *aToken2 = stlStrtok( NULL,
                              sTokenDelimeter,
                              &sTokenSavePtr );
        if( *aToken2 != NULL )
        {
            (*aTokenCount)++;
        }

        *aToken3 = stlStrtok( NULL,
                              sTokenDelimeter,
                              &sTokenSavePtr );
        if( *aToken3 != NULL )
        {
            (*aTokenCount)++;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */

