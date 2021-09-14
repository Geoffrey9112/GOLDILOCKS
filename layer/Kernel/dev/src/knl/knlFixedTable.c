/*******************************************************************************
 * knlFixedTable.c
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
 * @file knlFixedTable.c
 * @brief Kernel Layer Fixed Table 관리 루틴
 */

#include <knl.h>
#include <knDef.h>

#include <knxDef.h>
#include <knxFixedTable.h>
#include <knsSegment.h>

/**
 * @addtogroup knlFixedTable
 * @{
 */

extern knsEntryPoint * gKnEntryPoint;
knlDumpNameHelper  gKnlDumpNameHelper;

/**
 * @brief Fixed Table 의 Data Type 에 대한 String 상수
 */
const stlChar * const gKnlFxTableDataTypeString[KNL_FXTABLE_DATATYPE_MAX] =
{
    "CHARACTER VARYING",          /**< KNL_FXTABLE_DATATYPE_VARCHAR */
    "CHARACTER VARYING",          /**< KNL_FXTABLE_DATATYPE_VARCHAR_POINTER */
    "LONG CHARACTER VARYING",     /**< KNL_FXTABLE_DATATYPE_LONGVARCHAR */
    "LONG CHARACTER VARYING",     /**< KNL_FXTABLE_DATATYPE_LONGVARCHAR_POINTER */
    "NATIVE_SMALLINT",            /**< KNL_FXTABLE_DATATYPE_SMALLINT */
    "NATIVE_INTEGER",             /**< KNL_FXTABLE_DATATYPE_INTEGER */
    "NATIVE_BIGINT",              /**< KNL_FXTABLE_DATATYPE_BIGINT */
    "NATIVE_REAL",                /**< KNL_FXTABLE_DATATYPE_REAL */
    "NATIVE_DOUBLE",              /**< KNL_FXTABLE_DATATYPE_DOUBLE */
    "BOOLEAN",                    /**< KNL_FXTABLE_DATATYPE_BOOL */
    "TIMESTAMP WITHOUT TIME ZONE" /**< KNL_FXTABLE_DATATYPE_TIMESTAMP */
};

/**
 * @brief Fixed Table 의 Usage Type 에 대한 String 상수
 */
const stlChar * const gKnlFxTableUsageTypeString[KNL_FXTABLE_USAGE_MAX] =
{
    "FIXED TABLE", /**< KNL_FXTABLE_USAGE_FIXED_TABLE */
    "DUMP TABLE"   /**< KNL_FXTABLE_USAGE_DUMP_TABLE */
};



/**
 * @brief Fixed Table을 관리를 위한 초기화
 * @param[in,out] aKnlEnv Kernel Environment Handle
 * @remarks
 */
stlStatus knlInitializeFxTableMgr( knlEnv * aKnlEnv )
{
    return knxInitializeFxTableMgr( aKnlEnv );
}

/**
 * @brief Fixed Table을 관리를 종료한다.
 * @param[in,out] aKnlEnv Kernel Environment Handle
 * @remarks
 */
stlStatus knlFinalizeFxTableMgr( knlEnv * aKnlEnv )
{
    return knxFinalizeFxTableMgr( aKnlEnv );
}


/**
 * @brief Dump Table 의 Option String 을 해석하기 위한 Dump Name Helper 를 등록한다.
 * @param[in]   aDumpHelper     Dump Name Callback 함수 목록
 * @remarks
 */
void knlSetDumpTableNameHelper( knlDumpNameHelper * aDumpHelper )
{
    gKnlDumpNameHelper.mGetBaseTableHandle = aDumpHelper->mGetBaseTableHandle;
    gKnlDumpNameHelper.mGetIndexHandle     = aDumpHelper->mGetIndexHandle;
    gKnlDumpNameHelper.mGetSequenceHandle  = aDumpHelper->mGetSequenceHandle;
    gKnlDumpNameHelper.mGetSpaceHandle     = aDumpHelper->mGetSpaceHandle;
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
 * - KNL_MAX_FIXEDTABLE_COMMENT_LENGTH(128) 보다 작아야 함.
 * @endcode
 * @remarks
 */
stlStatus knlRegisterFxTable( knlFxTableDesc * aFxTableDesc,
                              knlEnv         * aKnlEnv )
{
    return knxRegisterFixedTable( aFxTableDesc, aKnlEnv );
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
stlStatus knlBuildFxRecord( knlFxColumnDesc   * aFxColumnDesc,
                            void              * aStructData,
                            knlValueBlockList * aValueList,
                            stlInt32            aBlockIdx,
                            knlEnv            * aKnlEnv )
{
    return knxBuildFxRecord( aFxColumnDesc,
                             aStructData,
                             aValueList,
                             aBlockIdx,
                             aKnlEnv );
}



/**
 * @brief 등록된 Fixed Table의 갯수를 얻는다.
 * @param[out]    aFxTableCount Fixed Table의 갯수
 * @param[in,out] aKnlEnv        Kernel Environment
 * @remarks
 */
stlStatus knlGetFixedTableCount( stlInt32 * aFxTableCount,
                                 knlEnv   * aKnlEnv )
{
    return knxGetFixedTableCount( aFxTableCount, aKnlEnv );
}



/**
 * @brief Fixed Table 의 Usage Type을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxUsageType      Fixed Table Type
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetFixedTableUsageType( void                * aFxTableHeapDesc,
                                     knlFxTableUsageType * aFxUsageType,
                                     knlEnv              * aKnlEnv )
{
    return knxGetFixedTableUsageType( aFxTableHeapDesc,
                                      aFxUsageType,
                                      aKnlEnv );
}

/**
 * @brief Fixed Table 의 사용가능한 Startup 단계를 얻는다.
 * @param[in]     aFxTableHandle        Fixed Table Handle
 * @param[out]    aUsableStartupPhase   Fixed Table 의 사용가능한 Startup Phase
 * @param[in,out] aKnlEnv               Kernel Environment
 */
stlStatus knlGetFixedTableUsablePhase( void            * aFxTableHandle,
                                       knlStartupPhase * aUsableStartupPhase,
                                       knlEnv          * aKnlEnv )
{
    void * sFxTableHeapDesc = knlGetFxTableHeapDesc( aFxTableHandle );
    
    return knxGetFixedTableUsablePhase( sFxTableHeapDesc,
                                        aUsableStartupPhase,
                                        aKnlEnv );
}

/**
 * @brief Fixed Table 의 Path Control 을 위한 공간의 크기를 얻는다.
 * @param[in]     aFxTableHandle    Fixed Table Handle
 * @param[out]    aFxPathCtrlSize   Fixed Table 의 Path Control을 위한 공간의 크기
 * @param[in,out] aKnlEnv           Kernel Environment
 * @remarks
 * Path Control 영역은 
 * Storage Manager의 Iterator가 Fixed Table에 해당하는 자료구조를
 * traverse 하기 위해 필요한 공간이다.
 * 이는 자료구조의 특성에 따라 fixed table을 정의한 개발자에 의해 결정된다.
 * - Array 자료구조의 경우, integer 크기의 공간
 * - Pointer(next list)로 연결된 자료구조의 경우, pointer 크기의 공간
 * - 보다 복잡한 자료구조의 경우,
 * <BR> knlBuildFxRecordCallback 함수 구현시
 * <BR> 현재 위치와 다음 위치를 판단할 수 있을 수 있는 자료구조를 정의하고
 * <BR> 이 크기를 확보하여야 한다.
 */
stlStatus knlGetFixedTablePathCtrlSize( void     * aFxTableHandle,
                                        stlInt32 * aFxPathCtrlSize,
                                        knlEnv   * aKnlEnv )
{
    void * sFxTableHeapDesc = knlGetFxTableHeapDesc( aFxTableHandle );
    
    return knxGetFixedTablePathCtrlSize( sFxTableHeapDesc,
                                         aFxPathCtrlSize,
                                         aKnlEnv );
}


/**
 * @brief Fixed Table 의 이름을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxTableName      Fixed Table 의 이름
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetFixedTableName( void     * aFxTableHeapDesc,
                                stlChar ** aFxTableName,
                                knlEnv   * aKnlEnv )
{
    return knxGetFixedTableName( aFxTableHeapDesc,
                                 aFxTableName,
                                 aKnlEnv );
}


/**
 * @brief Fixed Table 의 주석을 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxTableComment   Fixed Table 의 주석
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetFixedTableComment( void     * aFxTableHeapDesc,
                                   stlChar ** aFxTableComment,
                                   knlEnv   * aKnlEnv )
{
    return knxGetFixedTableComment( aFxTableHeapDesc,
                                    aFxTableComment,
                                    aKnlEnv );
}

/**
 * @brief Fixed Table 의 컬럼갯수를 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxColumnCount    Fixed Table 의 Column 갯수
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetFixedColumnCount( void             * aFxTableHeapDesc,
                                  stlInt32         * aFxColumnCount,
                                  knlEnv           * aKnlEnv )
{
    return knxGetFixedColumnCount( aFxTableHeapDesc,
                                   aFxColumnCount,
                                   aKnlEnv );
    }

/**
 * @brief Fixed Table 의 Column Descriptor Array 정보를 얻는다.
 * @param[in]     aFxTableHeapDesc  Fixed Table의 Heap Descriptor
 * @param[out]    aFxColumnDesc     Fixed Table 의 Column Descriptor Array
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetFixedColumnDesc( void             * aFxTableHeapDesc,
                                 knlFxColumnDesc ** aFxColumnDesc,
                                 knlEnv           * aKnlEnv )
{
    return knxGetFixedColumnDesc( aFxTableHeapDesc,
                                  aFxColumnDesc,
                                  aKnlEnv );
}

/**
 * @brief Fixed Table 의 Column Descriptor Array 정보를 얻는다.
 * @param[in]     aFxColumnDesc   Fixed Column Descriptor
 * @param[out]    aDtlDataTypeID  Data Type Layer 의 Data Type ID
 * @param[in,out] aKnlEnv         Kernel Environment
 */
stlStatus knlGetDataTypeIDOfFixedColumn( knlFxColumnDesc * aFxColumnDesc,
                                         dtlDataType     * aDtlDataTypeID,
                                         knlEnv          * aKnlEnv )
{
    return knxGetDataTypeIDOfFixedColumn( aFxColumnDesc,
                                          aDtlDataTypeID,
                                          aKnlEnv );
}
    


/**
 * @brief Fixed Table 의 Get DumpObject Handle Callback Function을 얻는다.
 * @param[in]     aFxTableHandle    Fixed Table Handle
 * @param[out]    aGetDumpCallback  Dump Object 의 Get Handle Callback Function
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetDumpObjHandleCallback( void                     * aFxTableHandle,
                                       knlGetDumpObjectCallback * aGetDumpCallback,
                                       knlEnv                   * aKnlEnv )
{
    void * sFxTableHeapDesc = knlGetFxTableHeapDesc( aFxTableHandle );
    
    return knxGetDumpObjHandleCallback( sFxTableHeapDesc,
                                        aGetDumpCallback,
                                        aKnlEnv );

}


/**
 * @brief Fixed Table 의 Open Callback Function을 얻는다.
 * @param[in]     aFxTableHandle    Fixed Table Handle
 * @param[out]    aOpenCallback     Fixed Table 의 Open Callback Function
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetFixedTableOpenCallback( void                    * aFxTableHandle,
                                        knlOpenFxTableCallback  * aOpenCallback,
                                        knlEnv                  * aKnlEnv )
{
    void * sFxTableHeapDesc = knlGetFxTableHeapDesc( aFxTableHandle );
    
    return knxGetFixedTableOpenCallback( sFxTableHeapDesc,
                                         aOpenCallback,
                                         aKnlEnv );
}

/**
 * @brief Fixed Table 의 Close Callback Function을 얻는다.
 * @param[in]     aFxTableHandle    Fixed Table Handle
 * @param[out]    aCloseCallback    Fixed Table 의 Close Callback Function
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetFixedTableCloseCallback( void                    * aFxTableHandle,
                                         knlCloseFxTableCallback * aCloseCallback,
                                         knlEnv                  * aKnlEnv )
{
    void * sFxTableHeapDesc = knlGetFxTableHeapDesc( aFxTableHandle );
    
    return knxGetFixedTableCloseCallback( sFxTableHeapDesc,
                                          aCloseCallback,
                                          aKnlEnv );
}

/**
 * @brief Fixed Table 의 Build Record Callback Function을 얻는다.
 * @param[in]     aFxTableHandle    Fixed Table Handle
 * @param[out]    aBuildCallback    Fixed Table 의 Build Record Callback Function
 * @param[in,out] aKnlEnv           Kernel Environment
 */
stlStatus knlGetFixedTableBuildRecordCallback( void                     * aFxTableHandle,
                                               knlBuildFxRecordCallback * aBuildCallback,
                                               knlEnv                   * aKnlEnv )
{
    void * sFxTableHeapDesc = knlGetFxTableHeapDesc( aFxTableHandle );
    
    return knxGetFixedTableBuildRecordCallback( sFxTableHeapDesc,
                                                aBuildCallback,
                                                aKnlEnv );
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
stlStatus knlTokenizeNextDumpOption( stlChar  ** aDumpOptionStr,
                                     stlChar  ** aToken1,
                                     stlChar  ** aToken2,
                                     stlChar  ** aToken3,
                                     stlInt32  * aTokenCount,
                                     knlEnv    * aEnv )
{
    return knxTokenizeNextDumpOption( aDumpOptionStr,
                                      aToken1,
                                      aToken2,
                                      aToken3,
                                      aTokenCount,
                                      aEnv );
}


/**
 * @brief Fixed Table 의 Relation Handle 을 얻는다.
 * @param[in] aHandleIdx  Fixed Table 의 Relation Index
 */
void * knlGetFxRelationHandle( stlInt32 aHandleIdx )
{
    knxFxTableRelationHeader * sFxHeader = (knxFxTableRelationHeader *) gKnEntryPoint->mFxRelationInfo;
    
    return & sFxHeader[aHandleIdx];
}

/**
 * @brief Fixed Relation Handle 로부터 Fixed Table 의 Heap Descriptor 를 획득한다.
 * @param[in] aFxRelationHandle  Fixed Table 의 Relation Handle
 */
void * knlGetFxTableHeapDesc( void * aFxRelationHandle )
{
    knxFxTableRelationHeader * sRelHeader = (knxFxTableRelationHeader *) aFxRelationHandle;

    return gKnxFxTableMgr.mFxHeapDescMap[sRelHeader->mFxHeapDescID];
}
    
/**
 * @brief Process Heap 영역에 Fixed Table 의 Heap Descriptor 를  위한 Map 을 구성한다.
 * @param[in] aEnv Environment
 * @remarks
 * Fixed Table 의 Plan Cache 는 공유 영역에 구축되지만,
 * Fixed Table 의 Handle 은 Process Code 영역에 존재한다.
 * 이러한 검색을 위해 최상위 layer에서 Fixed Table 에 대한 Handle Map 을 구성한다.
 */
stlStatus knlBuildHeapFixedTableDescMap( knlEnv * aEnv )
{
    stlInt32 i = 0;

    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( gKnxFxTableMgr.mFxTableCount > 0, KNL_ERROR_STACK(aEnv) );

    /**
     * Fixed Table Heap Descriptor 의 Map 공간 확보
     */
    
    STL_TRY( knxFxTableMemAlloc( & gKnxFxTableMgr,
                                 (void**) & gKnxFxTableMgr.mFxHeapDescMap,
                                 STL_SIZEOF(knxFxTableHeapDesc*) * gKnxFxTableMgr.mFxTableCount,
                                 aEnv )
             == STL_SUCCESS );

    /**
     * Heap Descriptor Map 구성
     */

    for ( i = 0; i < gKnxFxTableMgr.mFxTableCount; i++ )
    {
        STL_TRY( knxGetNthFixedTableDesc( (void **) & gKnxFxTableMgr.mFxHeapDescMap[i],
                                          i,
                                          aEnv )
                 == STL_SUCCESS );
    }
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Shared 영역에 Fixed Table 의 Relation Header 를  위한 Map 을 구성한다.
 * @param[in] aEnv Environment
 * @remarks
 * Fixed Table 의 Relation Header 는 Multiplexing 등을 위해 공유 메모리 상에 유지되어야 함
 */
stlStatus knlBuildSharedFixedRelationHeader( knlEnv * aEnv )
{
    stlInt32 i = 0;
    knxFxTableRelationHeader * sFxHeader = NULL;
    knxFxTableHeapDesc       * sFxHeapDesc = NULL;
    
    /**
     * Parameter Validation
     */
    
    STL_PARAM_VALIDATE( gKnxFxTableMgr.mFxTableCount > 0, KNL_ERROR_STACK(aEnv) );
    STL_PARAM_VALIDATE( gKnxFxTableMgr.mFxHeapDescMap != NULL, KNL_ERROR_STACK(aEnv) );

    /**
     * Relation Header Information 공간 할당
     */

    STL_TRY( knsAllocVariableStaticArea( STL_SIZEOF(knxFxTableRelationHeader) * gKnxFxTableMgr.mFxTableCount,
                                         (void **) & sFxHeader,
                                         aEnv )
             == STL_SUCCESS );

    for ( i = 0; i < gKnxFxTableMgr.mFxTableCount; i++ )
    {
        sFxHeapDesc = gKnxFxTableMgr.mFxHeapDescMap[i];

        /*
         * Storage Manager (smeRelationHeader) 와 약속된 영역
         */
        
        sFxHeader[i].mRelationType      = (stlInt16) sFxHeapDesc->mUsageType;
        sFxHeader[i].mRelHeaderSize     = 0;
        sFxHeader[i].mObjectScn         = 0;
        sFxHeader[i].mRelState          = 0;
        sFxHeader[i].mCreateTransId     = 0;
        sFxHeader[i].mDropTransId       = 0;
        sFxHeader[i].mCreateScn         = 0;
        sFxHeader[i].mDropScn           = 0;
        sFxHeader[i].mSegmentHandle     = 0;

        /*
         * Heap 영역의 Fixed Table Descriptor 를 찾기위한 값
         */
        sFxHeader[i].mFxHeapDescID      = i;
    }

    /**
     * Kernel Entry Pointer 에 연결 
     */

    gKnEntryPoint->mFxRelationInfo = sFxHeader;
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
