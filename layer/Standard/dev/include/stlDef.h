/*******************************************************************************
 * stlDef.h
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


#ifndef _STLDEF_H_
#define _STLDEF_H_ 1

/**
 * @file stlDef.h
 * @brief Standard Layer Definitions
 */

#include <stlTypes.h>

/* SQLSTATE code list */
#include <stlSqlState.h>

/* memory부족 test */
//#define __MEMORY_TEST__

/**
 * @addtogroup STL
 * @{
 */

#ifdef __cplusplus
#define STL_BEGIN_DECLS     extern "C" {
#define STL_END_DECLS       }
#else
#define STL_BEGIN_DECLS
#define STL_END_DECLS
#endif

#ifndef STL_DEBUG 
#define STL_IGNORE_VALIDATION  (1)
#endif

STL_BEGIN_DECLS

/**
 * @brief 레이어 클래스
 */
typedef enum stlLayerClass
{
    STL_LAYER_NONE = 0,              /**< Invalid Layer */
    STL_LAYER_STANDARD,              /**< Standard Layer */ 
    STL_LAYER_DATATYPE,              /**< DataType Layer */ 
    STL_LAYER_COMMUNICATION,         /**< Communication Layer */ 
    STL_LAYER_KERNEL,                /**< Kernel Layer */
    STL_LAYER_SERVER_COMMUNICATION,  /**< Server Communication Layer */
    STL_LAYER_STORAGE_MANAGER,       /**< Storage Manager Layer */
    STL_LAYER_EXECUTION_LIBRARY,     /**< Execution Library Layer */
    STL_LAYER_SQL_PROCESSOR,         /**< SQL Processor Layer */
    STL_LAYER_PSM_PROCESSOR,         /**< PSM Processor Layer */
    STL_LAYER_SESSION,               /**< Session Layer */
    STL_LAYER_SERVER_LIBRARY,        /**< Server Library Layer */
    STL_LAYER_GLIESE_LIBRARY,        /**< Glise Library */
    STL_LAYER_GLIESE_TOOL,           /**< Gliese Tool */
    STL_LAYER_GOLDILOCKS_LIBRARY,    /**< Goldilocks Library */
    STL_LAYER_GOLDILOCKS_TOOL,       /**< Goldilocks Tool */
    STL_LAYER_MAX
} stlLayerClass;

/**
 * @brief 에러 모듈 클래스
 */
typedef enum stlErrorModule
{
    STL_ERROR_MODULE_NONE = 0,                 /**< Invalid Layer */
    STL_ERROR_MODULE_STANDARD,                 /**< Standard Layer */ 
    STL_ERROR_MODULE_DATATYPE,                 /**< DataType Layer */ 
    STL_ERROR_MODULE_KERNEL,                   /**< Kernel Layer */
    STL_ERROR_MODULE_STORAGE_MANAGER,          /**< Storage Manager Layer */
    STL_ERROR_MODULE_EXECUTION_LIBRARY,        /**< Execution Library Layer */
    STL_ERROR_MODULE_SQL_PROCESSOR,            /**< SQL Processor Layer */
    STL_ERROR_MODULE_PSM_PROCESSOR,            /**< PSM Processor Layer */
    STL_ERROR_MODULE_SESSION,                  /**< Session Layer */
    STL_ERROR_MODULE_GLIESE_LIBRARY_AODBC,     /**< Glise Attached ODBC Library */
    STL_ERROR_MODULE_GLIESE_LIBRARY_ODBC,      /**< Glise ODBC Library */
    STL_ERROR_MODULE_GLIESE_LIBRARY_JDBC,      /**< Glise JDBC Library */
    STL_ERROR_MODULE_GLIESE_LIBRARY_GDBC,      /**< Glise Gliese DBC Library */
    STL_ERROR_MODULE_GLIESE_LIBRARY_GOLDILOCKSESQL, /**< GOLDILOCKS ESQL Library */
    STL_ERROR_MODULE_COMMUNICATION,            /**< Communication Layer */
    STL_ERROR_MODULE_SERVER_LIBRARY,           /**< Server Library Layer */
    STL_ERROR_MODULE_GLIESE_TOOL_GSQL = 30,    /**< gsql */
    STL_ERROR_MODULE_GLIESE_TOOL_GPEC,         /**< gpec */
    STL_ERROR_MODULE_GLIESE_TOOL_GLOADER,      /**< gloader */
    STL_ERROR_MODULE_GLIESE_TOOL_GMIG,         /**< gmigrator */
    STL_ERROR_MODULE_GLIESE_TOOL_GMASTER,      /**< gmaster */
    STL_ERROR_MODULE_GLIESE_TOOL_GLISTENER,    /**< glsnr */
    STL_ERROR_MODULE_GLIESE_TOOL_CYCLONE,      /**< Cyclone */
    STL_ERROR_MODULE_GLIESE_TOOL_LOGMIRROR,    /**< LogMirror */
    STL_ERROR_MODULE_GLIESE_TOOL_CYMON,        /**< Cyclone Monitor */
    STL_ERROR_MODULE_GLIESE_TOOL_GSERVER,      /**< gserver */
    STL_ERROR_MODULE_GLIESE_TOOL_GDISPATCHER,  /**< gdispatcher */
    STL_ERROR_MODULE_GLIESE_TOOL_GBALANCER,    /**< gbalancer */
    STL_ERROR_MODULE_SERVER_COMMUNICATION,     /**< Server Communication Layer */
    STL_ERROR_MODULE_GLIESE_TOOL_GSYNCHER,     /**< gsyncher */
    STL_ERROR_MODULE_GLIESE_TOOL_CYCLONEM,     /**< Cyclonem */
    STL_ERROR_MODULE_GLIESE_TOOL_CYMONM,       /**< Cyclonem Monitor */
    STL_ERROR_MODULE_MAX
} stlErrorModule;


typedef void (*stlFatalHandler)( const stlChar * aCauseString,
                                 void          * aSigInfo,
                                 void          * aContext );

/**
 * @brief Layer 이름이 가질수 있는 최대 길이
 */
#define STL_MAX_LAYER_NAME  (30)

extern const stlChar *const gLayerString[];

/**
 * @brief ASSERT 매크로
 */
#define STL_ASSERT( aExpression )                       \
    do                                                  \
    {                                                   \
        if( !(aExpression) )                            \
        {                                               \
            stlAssert( "ASSERT(%s)\n"                   \
                       "failed in file %s line %d\n",   \
                       #aExpression,                    \
                       __FILE__,                        \
                       __LINE__ );                      \
        }                                               \
    } while( 0 )

#if defined( STL_DEBUG ) || defined( DOXYGEN )
#define STL_DASSERT( aExpression ) STL_ASSERT( aExpression )
#else
#define STL_DASSERT( aExpression )
#endif

#define STL_THROW( aLabel ) goto aLabel;

#define STL_TRY_THROW( aExpression, aLabel )    \
    do                                          \
    {                                           \
        if( !(aExpression) )                    \
        {                                       \
            goto aLabel;                        \
        }                                       \
    } while( 0 )

#define STL_TRY( aExpression )                  \
    do                                          \
    {                                           \
        if( !(aExpression) )                    \
        {                                       \
            goto STL_FINISH_LABEL;              \
        }                                       \
    } while( 0 )

#define STL_CATCH( aLabel )                     \
    goto STL_FINISH_LABEL;                      \
    aLabel:

#define STL_RAMP( aLabel ) aLabel:

/**
 * @brief aExpression이 대부분 경우에 TRUE일때 사용되며, if 구문안의 명령어들이 prefetch 된다.
 */
#define STL_EXPECT_TRUE( aExpression )    __builtin_expect( aExpression, 1 )
/**
 * @brief aExpression이 대부분 경우에 FALSE일때 사용되며, if 구문밖의 명령어들이 prefetch 된다.
 */
#define STL_EXPECT_FALSE( aExpression )   __builtin_expect( aExpression, 0 )

#if defined( STL_IGNORE_VALIDATION )
/*
 * STL_FINISH에서 "goto STL_FINISH_LABEL"을 추가한 이유는 STL_IGNORE_VALIDATION이
 * TRUE인 상황에서의 compile warning을 제거하기 위함이다.
 */
#define STL_FINISH                              \
    goto STL_FINISH_LABEL;                      \
    STL_FINISH_LABEL:
#else
#define STL_FINISH                              \
    STL_FINISH_LABEL:
#endif

/**
 * @brief 파라미터 유효성 검사 매크로 ( 디버그 모드에서만 프로세스를 종료시킴 )
 */
#if defined( STL_IGNORE_VALIDATION )
#define STL_PARAM_VALIDATE( aExpression, aErrorStack )
#else
#define STL_PARAM_VALIDATE( aExpression, aErrorStack )          \
    do                                                          \
    {                                                           \
        if( !(aExpression) )                                    \
        {                                                       \
            stlChar sSource[STL_MAX_ERROR_MESSAGE];             \
            stlSnprintf( sSource,                               \
                         STL_MAX_ERROR_MESSAGE,                 \
                         "(%s) failed in file %s line %d",      \
                         #aExpression,                          \
                         __FILE__,                              \
                         __LINE__ );                            \
            stlPushError( STL_ERROR_LEVEL_ABORT,                \
                          STL_ERRCODE_INVALID_ARGUMENT,         \
                          sSource,                              \
                          aErrorStack );                        \
            STL_DASSERT( aExpression );                         \
            goto STL_FINISH_LABEL;                              \
        }                                                       \
    } while( 0 )
#endif
/**
 * @brief FALSE
 */
#define STL_FALSE 0

/**
 * @brief TRUE
 */
#define STL_TRUE  1

/**
 * @brief NO
 */
#define STL_NO    0

/**
 * @brief YES
 */
#define STL_YES   1

/**
 * @brief INACTIVE
 */
#define STL_INACTIVE 0

/**
 * @brief ACTIVE
 */
#define STL_ACTIVE 1

/**
 * @brief Maximum callstack count
 */
#define STL_MAX_CALLSTACK_FRAME_SIZE 100

/**
 * @brief SQL 객체의 이름 길이의 최대값
 * @remark SQL표준의 SQL_IDENTIFIER 길이로 식별자 이름은 128보다 작아야 함.
 */ 
#define STL_MAX_SQL_IDENTIFIER_LENGTH (128)

/**
 * @brief 입력받은 크기를 @a aBoundary 크기에 맞춰 조정한다.
 * @param aSize 조절할 크기
 * @param aBoundary 정렬할 크기
 * @remark @a aBoundary 는 2의 거듭제곱이어야 한다.
 */
#define STL_ALIGN( aSize, aBoundary )                           \
    (((aSize) + ((aBoundary) - 1)) & ~((aBoundary) - 1))

/**
 * @brief A와 B중 작은값을 얻는다.
 */
#define STL_MIN( aA, aB )                       \
    ( ( aA < aB ) ? aA : aB )

/**
 * @brief A와 B중 큰값을 얻는다.
 */
#define STL_MAX( aA, aB )                       \
    ( ( aA > aB ) ? aA : aB )

/**
 * @brief 입력받은 크기를 기본값(8) 크기에 맞춰 조정한다.
 */
#define STL_ALIGN_DEFAULT( aSize ) STL_ALIGN( aSize, 8 )

#define STL_OFFSET(aType, aMember)                                                      \
    ((stlSize) (((stlChar *) (&(((aType)NULL)->aMember))) - ((stlChar *) NULL)))

/**
 * @brief structure 안의 멤버의 offset을 구한다.
 * @param aType structure 타입 이름
 * @param aMember structure 안의 멤버 이름
 * @return offset
 */
#define STL_OFFSETOF(aType, aMember) STL_OFFSET(aType*, aMember)

/**
 * @brief 주어진 파라미터의 사이즈를 구한다.
 * @param aType 타입 이름
 * @return size
 */
#define STL_SIZEOF( aType ) sizeof( aType )

/**
 * @brief 주어진 파라미터의 비트수를 구한다.
 * @param aType 타입 이름
 * @return Bit Count
 */
#define STL_BIT_COUNT( aType ) ( 8 * STL_SIZEOF(aType) )

/**
 * @brief PATH의 최대 길이
 */
#if defined(PATH_MAX) || defined( DOXYGEN )
#define STL_PATH_MAX       PATH_MAX
#elif defined(_POSIX_PATH_MAX)
#define STL_PATH_MAX       _POSIX_PATH_MAX
#else
#define STL_PATH_MAX       1024
#endif

/**
 * @brief difference pointers
 */
#define STL_DIFF_POINTER( aPointer1, aPointer2 )                                \
    ( STL_INT_FROM_POINTER(aPointer1) - STL_INT_FROM_POINTER(aPointer2) )
#define STL_INT_FROM_POINTER( aPointer ) ( (stlVarInt)(aPointer) )

/**
 * @brief 무한대 시간
 */
#define STL_INFINITE_TIME ( STL_UINT64_MAX )

/**
 * @brief Product Version Length
 */
#define STL_PRODUCT_VERSION_LENGTH       (4)
#define STL_SOURCE_SHORT_REVISION_LENGTH (15)
#define STL_SOURCE_REVISION_LENGTH       (41)

/**
 * @brief Version information
 */
typedef struct stlVersionInfo
{
    stlChar  mProduct[STL_PRODUCT_VERSION_LENGTH];             /**< product version */
    stlInt32 mMajor;                                           /**< major version */
    stlInt32 mMinor;                                           /**< minor version */
    stlInt32 mPatch;                                           /**< patch version */
    stlChar  mRevision[STL_SOURCE_REVISION_LENGTH];            /**< revision */
    stlChar  mShortRevision[STL_SOURCE_SHORT_REVISION_LENGTH]; /**< short revision */
} stlVersionInfo;
    
/** @} */

/**
 * @addtogroup stlAtomic
 * @{
 */

/**
 * @brief Memory Barrier를 설정한다.
 */
#define stlMemBarrier()    __sync_synchronize()

/**
 * @brief aMem의 기존값이 aCmp와 같다면 aWith로 교체한다.
 * @param aMem 설정할 포인터
 * @param aWith 교체하고자 하는 32비트 정수값
 * @param aCmp 비교값
 * @return *aMem의 기존값(old value)
 */
#ifdef WIN32
#define stlAtomicCas32( aMem, aWith, aCmp )  InterlockedCompareExchange((long*)aMem, aWith, aCmp)
#else
#define stlAtomicCas32( aMem, aWith, aCmp )  __sync_val_compare_and_swap( aMem, aCmp, aWith )
#endif
/** @} */

/**
 * @addtogroup stlAllocator
 * @{
 */

typedef struct stlAllocator
{
    stlInt64   mInitSize;             /**< Init size */
    stlInt64   mNextSize;             /**< Next size */
    stlInt64   mUsedChunkCount;       /**< Used chunk count */
    stlInt64   mTotalChunkCount;      /**< Total allocated chunk count */
    void     * mCurChunk;             /**< Current chunk */
    void     * mFirstChunk;           /**< First chunk */
    void     * mLastChunk;            /**< Last chunk */
} stlAllocator;

typedef struct stlRegionMark
{
    void     * mChunk;                /**< Marked chunk */
    stlInt64   mUsedChunkCount;       /**< Used chunk count  */
    stlInt32   mAllocHwm;             /**< Allocated HWM(High Water Mark) */
} stlRegionMark;

/** @} */


/**
 * @addtogroup stlError
 * @{
 */

/**
 * @brief 에러 메세지의 최대 크기
 */
#define STL_MAX_ERROR_MESSAGE          (512)
/**
 * @brief LARGE 에러 메세지의 최대 크기
 */
#define STL_MAX_LARGE_ERROR_MESSAGE    (32*1024)
/**
 * @brief 에러 스택의 최대 크기
 */
#define STL_MAX_ERROR_STACK_DEPTH      (4)
/**
 * @brief 에러 스택의 콜스택 최대 크기
 */
#define STL_MAX_ERROR_CALLSTACK_DEPTH  (50)
/**
 * @brief 에러 레벨
 */
typedef enum
{
    STL_ERROR_LEVEL_FATAL = 0, /**< FATAL ERROR (서버가 운영될수 없는 수준의 에러) */
    STL_ERROR_LEVEL_ABORT,     /**< ABORT ERROR (롤백 가능한 수준의 에러) */
    STL_ERROR_LEVEL_WARNING,   /**< WARNING (경고 수준의 에러) */
    STL_ERROR_LEVEL_MAX
} stlErrorLevel;

/**
 * @brief 에러 스택 초기화
 */
#define STL_INIT_ERROR_STACK( aErrorDataStack ) \
{                                               \
    (aErrorDataStack)->mTop = -1;               \
}

/*
 * Internal Error Code 구조
 * - 5자리의 10진수
 *  - 앞 두자리 : Layer 정보 (10 부터 시작)
 *  - 뒤 세자리 : Value 정보 
 */

/**
 * @brief 에러 레이어 기저 (10)
 */
#define STL_ERROR_MODULE_BASE         (10) 

/**
 * @brief 에러 순차값 최대치 
 */
#define STL_ERROR_MAX_VALUE_COUNT    (1000)

/**
 * @brief 에러 코드로부터 에러 레이어값을 얻는다.
 */
#define STL_GET_ERROR_LAYER( aErrorCode )                                               \
    ( (stlInt32) ( aErrorCode / STL_ERROR_MAX_VALUE_COUNT ) - STL_ERROR_MODULE_BASE )

/**
 * @brief 에러 코드로부터 에러 순차값 얻는다.
 */
#define STL_GET_ERROR_VALUE( aErrorCode )       \
    ( aErrorCode % STL_ERROR_MAX_VALUE_COUNT )

/**
 * @brief 에러 코드를 구성한다.
 * 에러 코드는 32비트 정수를 사용하며,
 * 상위 10비트의 레이어와 하위 22비트의 에러 순차값으로 구성된다.
 */
#define STL_MAKE_ERRCODE( aErrorLayer, aErrorValue )                                            \
    ( aErrorValue + ( aErrorLayer + STL_ERROR_MODULE_BASE ) * STL_ERROR_MAX_VALUE_COUNT )

/**
 * @brief 유효하지 않은 에러 코드.
 * 잘못된 에러 코드 식별을 위해 사용됨.
 */
#define STL_INVALID_ERROR_CODE   STL_MAKE_ERRCODE( STL_ERROR_MODULE_NONE, 0 )

/**
 * @brief 에러 데이터 구조
 */
typedef struct stlErrorData
{
    stlErrorLevel mErrorLevel;                                   /**< 에러 레벨 */
    stlUInt32     mExternalErrorCode;                            /**< External Error Code */
    stlUInt32     mErrorCode;                                    /**< 에러 코드 */
    stlUInt32     mSystemErrorCode;                              /**< 플랫폼별 에러 코드(일반적으로 errno) */
    stlChar       mErrorMessage[STL_MAX_ERROR_MESSAGE];          /**< 기본 에러 메세지 */
    stlChar       mDetailErrorMessage[STL_MAX_ERROR_MESSAGE];    /**< 자세한 에러 메세지 */
} stlErrorData;

/**
 * @brief 에러 스택 구조
 */
typedef struct stlErrorStack
{
    stlInt32       mTop;                                            /**< 스택 Top을 가리킴 */
    stlInt32       mCallStackFrameCount;                            /**< mErrorCallstack에 저장된 프레임 개수 */
    stlErrorData   mErrorData[STL_MAX_ERROR_STACK_DEPTH];           /**< 에러 데이터의 배열 */
    void         * mErrorCallStack[STL_MAX_ERROR_CALLSTACK_DEPTH];  /**< 콜스택 프레임 배열 */
} stlErrorStack;

/**
 * @brief 에러 레코드 구조.
 * stlErrorTable에 한개의 레코드를 표현한다.
 */
typedef struct stlErrorRecord
{
    stlUInt32   mExternalErrorCode;    /**< 내부 에러코드와 매핑되는 외부 에러코드 */
    stlChar   * mErrorMessage;         /**< 내부 에러코드의 에러 메세지 */
} stlErrorRecord;

extern stlErrorRecord * gLayerErrorTables[STL_ERROR_MODULE_MAX];

/**
 * @brief 주어진 aErrorStack에 에러 데이터를 가지고 있는 검사한다.
 */
#define STL_HAS_ERROR( aErrorStack )   ( (aErrorStack)->mTop > -1 )

/**
 * @defgroup stlInternalErrorCode Internal Error Codes
 * @ingroup stlError
 * @{
 */
/**
 * @brief invalid argument error
 */
#define STL_ERRCODE_INVALID_ARGUMENT       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 0 )
/**
 * @brief operation incomplete error
 */
#define STL_ERRCODE_INCOMPLETE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 1 )
/**
 * @brief access denied error
 */
#define STL_ERRCODE_ACCESS                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 2 )
/**
 * @brief end of file error
 */
#define STL_ERRCODE_EOF                    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 3 )
/**
 * @brief duplicate file error (대표 에러)
 */
#define STL_ERRCODE_FILE_DUP               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 4 )
/**
 * @brief lock file error (대표 에러)
 */
#define STL_ERRCODE_FILE_LOCK              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 5 )
/**
 * @brief unlock file error (대표 에러)
 */
#define STL_ERRCODE_FILE_UNLOCK            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 6 )
/**
 * @brief open file error (대표 에러)
 */
#define STL_ERRCODE_FILE_OPEN              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 7 )
/**
 * @brief close file error (대표 에러)
 */
#define STL_ERRCODE_FILE_CLOSE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 8 )
/**
 * @brief remove file error (대표 에러)
 */
#define STL_ERRCODE_FILE_REMOVE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 9 )
/**
 * @brief link file error (대표 에러)
 */
#define STL_ERRCODE_FILE_LINK              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 10 )
/**
 * @brief read file error (대표 에러)
 */
#define STL_ERRCODE_FILE_READ              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 11 )
/**
 * @brief write file error (대표 에러)
 */
#define STL_ERRCODE_FILE_WRITE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 12 )
/**
 * @brief sync(flush) file error (대표 에러)
 */
#define STL_ERRCODE_FILE_SYNC              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 13 )
/**
 * @brief seek file error (대표 에러)
 */
#define STL_ERRCODE_FILE_SEEK              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 14 )
/**
 * @brief truncate file error (대표 에러)
 */
#define STL_ERRCODE_FILE_TRUNCATE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 15 )
/**
 * @brief stat file error (대표 에러)
 */
#define STL_ERRCODE_FILE_STAT              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 16 )
/**
 * @brief create pipe error (대표 에러)
 */
#define STL_ERRCODE_PIPE                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 17 )
/**
 * @brief wait process error (대표 에러)
 */
#define STL_ERRCODE_PROC_WAIT              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 18 )
/**
 * @brief insufficient resource error
 */
#define STL_ERRCODE_INSUFFICIENT_RESOURCE  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 19 )
/**
 * @brief kill signal error (대표 에러)
 */
#define STL_ERRCODE_SIGNAL_KILL            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 20 )
/**
 * @brief set signal handler error (대표 에러)
 */
#define STL_ERRCODE_SIGNAL_SET_HANDLER     STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 21 )
/**
 * @brief block signal error (대표 에러)
 */
#define STL_ERRCODE_SIGNAL_BLOCK           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 22 )
/**
 * @brief set thread attribute error (대표 에러)
 */
#define STL_ERRCODE_THREAD_ATTR            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 23 )
/**
 * @brief create thread error (대표 에러)
 */
#define STL_ERRCODE_THREAD_CREATE          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 24 )
/**
 * @brief join thread error (대표 에러)
 */
#define STL_ERRCODE_THREAD_JOIN            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 25 )
/**
 * @brief set thread once error (대표 에러)
 */
#define STL_ERRCODE_THREAD_ONCE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 26 )
/**
 * @brief string truncation error
 */
#define STL_ERRCODE_STRING_TRUNCATION      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 27 )
/**
 * @brief name too long error
 */
#define STL_ERRCODE_NAMETOOLONG            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 28 )
/**
 * @brief shared memory segment exist error
 */
#define STL_ERRCODE_SHM_EXIST              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 29 )
/**
 * @brief create shared memory error (대표 에러)
 */
#define STL_ERRCODE_SHM_CREATE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 30 )
/**
 * @brief attach shared memory error (대표 에러)
 */
#define STL_ERRCODE_SHM_ATTACH             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 31 )
/**
 * @brief detach shared memory error (대표 에러)
 */
#define STL_ERRCODE_SHM_DETACH             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 32 )
/**
 * @brief destroy shared memory error (대표 에러)
 */
#define STL_ERRCODE_SHM_DESTROY            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 33 )
/**
 * @brief create semaphore error (대표 에러)
 */
#define STL_ERRCODE_SEM_CREATE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 34 )
/**
 * @brief acquire semaphore error (대표 에러)
 */
#define STL_ERRCODE_SEM_ACQUIRE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 35 )
/**
 * @brief release semaphore error (대표 에러)
 */
#define STL_ERRCODE_SEM_RELEASE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 36 )
/**
 * @brief destroy semaphore error (대표 에러)
 */
#define STL_ERRCODE_SEM_DESTROY            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 37 )
/**
 * @brief resource busy error
 */
#define STL_ERRCODE_BUSY                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 38 )
/**
 * @brief timed-out error
 */
#define STL_ERRCODE_TIMEDOUT               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 39 )
/**
 * @brief no entry error
 */
#define STL_ERRCODE_NO_ENTRY               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 40 )
/**
 * @brief mismatch error
 */
#define STL_ERRCODE_MISMATCH               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 41 )
/**
 * @brief out of memory error
 */
#define STL_ERRCODE_OUT_OF_MEMORY          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 42 )
/**
 * @brief disk full error
 */
#define STL_ERRCODE_DISK_FULL              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 43 )
/**
 * @brief execute process error (대표 에러)
 */
#define STL_ERRCODE_PROC_EXECUTE           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 44 )
/* By Network I/O */
#define STL_ERRCODE_NOTSUPPORT             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 45 )
#define STL_ERRCODE_NOSPACE                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 46 )
#define STL_ERRCODE_INET_PTON_INVAL        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 47 )
#define STL_ERRCODE_SENDTO                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 48 )
#define STL_ERRCODE_RECVFROM               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 49 )
#define STL_ERRCODE_GETSOCKNAME            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 50 )
#define STL_ERRCODE_GETPEERNAME            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 51 )
#define STL_ERRCODE_FCNTL                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 52 )
#define STL_ERRCODE_SETSOCKOPT             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 53 )
#define STL_ERRCODE_NOTIMPL                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 54 )
#define STL_ERRCODE_IOCTL                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 55 )
#define STL_ERRCODE_GETHOSTNAME            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 56 )
#define STL_ERRCODE_SOCKET_CLOSE           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 57 )
#define STL_ERRCODE_SOCKET_SHUTDOWN        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 58 )
#define STL_ERRCODE_SOCKET                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 59 )
#define STL_ERRCODE_GETFD                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 60 )
#define STL_ERRCODE_SETFD                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 61 )
#define STL_ERRCODE_BIND                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 62 )
#define STL_ERRCODE_LISTEN                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 63 )
#define STL_ERRCODE_ACCEPT                 STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 64 )
#define STL_ERRCODE_INTR                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 65 )
#define STL_ERRCODE_GETSOCKOPT             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 66 )
#define STL_ERRCODE_CONNECT                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 67 )
#define STL_ERRCODE_POLL                   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 68 )
#define STL_ERRCODE_INTERNAL               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 69 )
#define STL_ERRCODE_SENDMSG                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 70 )
#define STL_ERRCODE_RECVMSG                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 71 )
#define STL_ERRCODE_SOCKETPAIR             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 72 )
/**
 * @brief range overflow or underflow
 */
#define STL_ERRCODE_OUT_OF_RANGE           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 73 )
/**
 * @brief string is not number
 */
#define STL_ERRCODE_STRING_IS_NOT_NUMBER   STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 74 )
/**
 * @brief bad file error
 */
#define STL_ERRCODE_FILE_BAD               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 75 )
/**
 * @brief again error
 */
#define STL_ERRCODE_AGAIN                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 76 )
/**
 * @brief The given address is already in use
 */
#define STL_ERRCODE_ADDR_INUSE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 77 )
/**
 * @brief fail to initialize communication context
 */
#define STL_ERRCODE_INITIALIZE_CONTEXT     STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 78 )
/**
 * @brief fail to finalize communication context
 */
#define STL_ERRCODE_FINALIZE_CONTEXT       STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 79 )
/**
 * @brief fail to connect communication context
 */
#define STL_ERRCODE_CONNECT_CONTEXT        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 80 )
/**
 * @brief fail to poll communication context
 */
#define STL_ERRCODE_POLL_CONTEXT           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 81 )
/**
 * @brief fail to write packet through communication context
 */
#define STL_ERRCODE_WRITE_PACKET           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 82 )
/**
 * @brief fail to read packet through communication context
 */
#define STL_ERRCODE_READ_PACKET            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 83 )
/**
 * @brief fail to send descriptor through communication context
 */
#define STL_ERRCODE_SEND_CONTEXT           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 84 )
/**
 * @brief fail to receive descriptor through communication context
 */
#define STL_ERRCODE_RECV_CONTEXT           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 85 )
/**
 * @brief fail to get current file path (대표 에러)
 */
#define STL_ERRCODE_FILE_PATH              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 86 )
/**
 * @brief fail to open library (대표 에러)
 */
#define STL_ERRCODE_OPEN_LIBRARY           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 87 )
/**
 * @brief fail to close library (대표 에러)
 */
#define STL_ERRCODE_CLOSE_LIBRARY          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 88 )
/**
 * @brief fail to get symbol (대표 에러)
 */
#define STL_ERRCODE_GET_SYMBOL             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 89 )
/**
 * @brief query is not supported
 */
#define STL_ERRCODE_PARSER_NOTSUPPORT      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 90 )
/**
 * @brief open file error (대표 에러)
 */
#define STL_ERRCODE_FILE_EXIST             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 91 )
/**
 * @brief input is too long
 */
#define STL_ERRCODE_PARSER_INPUT_TOO_LONG  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 92 )
/**
 * @brief system call error 
 */
#define STL_ERRCODE_PARSER_SYSTEM_CALL_ERROR STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 93 )
/**
 * @brief divide zero 
 */
#define STL_ERRCODE_PARSER_DIVIDE_ZERO STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 94 )
/**
 * @brief open semaphore error (대표 에러)
 */
#define STL_ERRCODE_SEM_OPEN               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 96 )
/**
 * @brief close semaphore error (대표 에러)
 */
#define STL_ERRCODE_SEM_CLOSE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 96 )
/**
 * @brief unlink semaphore error (대표 에러)
 */
#define STL_ERRCODE_SEM_UNLINK             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 97 )
/**
 * @brief permission denied - '%s'
 */
#define STL_ERRCODE_ACCESS_FILE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 98 )
/**
 * @brief not supported os
 */
#define STL_ERRCODE_OS_NOTSUPPORT          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 99 )
/**
 * @brief Environment Variable '%s' is not defined
 */
#define STL_ERRCODE_UNDEFINED_ENV_VARIABLE STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 100 )
/**
 * @brief License file(%s) is not exist
 */
#define STL_ERRCODE_NOT_EXIST_LICENSE      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 101 )
/**
 * @brief License is out of date. License is valid after %d-%d-%d
 */
#define STL_ERRCODE_LICENSE_OUT_OF_DATE    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 102 )
/**
 * @brief License date is expired. License is expired in %d-%d-%d
 */
#define STL_ERRCODE_EXPIRED_LICENSE        STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 103 )
/**
 * @brief License core count mismatch.
 */
#define STL_ERRCODE_MISMATCH_CORECOUNT     STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 104 )
/**
 * @brief License key is corrupted.
 */
#define STL_ERRCODE_INVALID_LICENSE_KEY    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 105 )
/**
 * @brief License host name mismatch.
 */
#define STL_ERRCODE_MISMATCH_HOSTNAME      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 106 )
/**
 * @brief failed to set thread affinity.
 */
#define STL_ERRCODE_SET_THREAD_AFFINITY    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 107 )
/**
 * @brief failed to creeate timer
 */
#define STL_ERRCODE_CREATE_TIMER           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 108 )
/**
 * @brief failed to set timer
 */
#define STL_ERRCODE_SET_TIMER              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 109 )
/**
 * @brief failed to destroy timer
 */
#define STL_ERRCODE_DESTROY_TIMER          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 110 )
/**
 * @brief address is not given
 */
#define STL_ERRCODE_ADDR_NOTGIVEN          STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 111 )
/**
 * @brief Unable to access the message queue
 */
#define STL_ERRCODE_MSGQ_ACCESS            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 112 )
/**
 * @brief A message queue exists
 */
#define STL_ERRCODE_MSGQ_EXIST             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 113 )
/**
 * @brief Unable to create a message queue
 */
#define STL_ERRCODE_MSGQ_CREATE            STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 114 )
/**
 * @brief The message queue was removed
 */
#define STL_ERRCODE_MSGQ_REMOVED           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 115 )
/**
 * @brief Unable to destroy the message queue
 */
#define STL_ERRCODE_MSGQ_DESTROY           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 116 )
/**
 * @brief Unable to send message
 */
#define STL_ERRCODE_MSGQ_SEND              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 117 )
/**
 * @brief Unable to receive message
 */
#define STL_ERRCODE_MSGQ_RECV              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 118 )
/**
 * @brief No message was available in the message queue
 */
#define STL_ERRCODE_MSGQ_AGAIN             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 119 )
/**
 * @brief fail to get property of a file status
 */
#define STL_ERRCODE_GETFL                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 120 )
/**
 * @brief fail to set property of a file status
 */
#define STL_ERRCODE_SETFL                  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 121 )
/**
 * @brief infiniband verbs returned error
 */
#define STL_ERRCODE_IBV_VERBS_FAILURE      STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 122 )
/**
 * @brief no infiniband device was found
 */
#define STL_ERRCODE_NO_IB_DEVICE           STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 123 )
/**
 * @brief cannot find specified infiniband device
 */
#define STL_ERRCODE_CANNOT_FIND_IB_DEVICE  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 124 )
/**
 * @brief cannot find suitable Memory Region for specified ptr and size
 */
#define STL_ERRCODE_CANNOT_FIND_MEMORY_REGION  STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 125 )
/**
 * @brief fail to get semaphore value (대표 에러)
 */
#define STL_ERRCODE_SEM_GET_VALUE              STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 126)
/**
 * @brief fail to control pollset
 */
#define STL_ERRCODE_POLLSET_CTRL               STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 127)
/**
 * @brief fail to create pollset
 */
#define STL_ERRCODE_POLLSET_CREATE             STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 128)
/**
 * @brief overflow fd
 */
#define STL_ERRCODE_OVERFLOW_FD                STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 129)
/**
 * @brief invalid INI file format
 */
#define STL_ERRCODE_INVALID_INI_FILE_FORMAT    STL_MAKE_ERRCODE( STL_ERROR_MODULE_STANDARD, 130)


#define STL_MAX_ERROR     131

/** @} */

/** @} */

/**
 * @addtogroup stlFile
 * @{
 */

/**
 * @brief 파일 이름의 최대 크기
 */
#define STL_MAX_FILE_NAME_LENGTH 128
/**
 * @brief 파일 경로(+이름)의 최대 크기
 */
#define STL_MAX_FILE_PATH_LENGTH 1024
/**
 * @brief BULK I/O의 최대 바이트수
 */
#define STL_BULK_IO_BYTES        (32*1024*1024)

/**
 * @defgroup stlFileInfoFlags File Information Flags
 * @ingroup stlFile
 * @{
 */
#define STL_FINFO_LINK   0x00000001 /**< Stat the link not the file itself if it is a link */
#define STL_FINFO_MTIME  0x00000010 /**< Modification Time */
#define STL_FINFO_CTIME  0x00000020 /**< Creation or inode-changed time */
#define STL_FINFO_ATIME  0x00000040 /**< Access Time */
#define STL_FINFO_SIZE   0x00000100 /**< Size of the file */
#define STL_FINFO_ASIZE  0x00000200 /**< Storage size consumed by the file */
#define STL_FINFO_DEV    0x00001000 /**< Device */
#define STL_FINFO_INODE  0x00002000 /**< Inode */
#define STL_FINFO_NLINK  0x00004000 /**< Number of links */
#define STL_FINFO_TYPE   0x00008000 /**< Type */
#define STL_FINFO_USER   0x00010000 /**< User */
#define STL_FINFO_GROUP  0x00020000 /**< Group */
#define STL_FINFO_UPROT  0x00100000 /**< User protection bits */
#define STL_FINFO_GPROT  0x00200000 /**< Group protection bits */
#define STL_FINFO_WPROT  0x00400000 /**< World protection bits */

#define STL_FINFO_MIN    0x00008170 /**< type, mtime, ctime, atime, size */
#define STL_FINFO_IDENT  0x00003000 /**< dev and inode */
#define STL_FINFO_OWNER  0x00030000 /**< user and group */
#define STL_FINFO_PROT   0x00700000 /**< all protections */
#define STL_FINFO_NORM   0x0073b170 /**< platform-independent flags (all flags - INODE - CSIZE) */
/** @} */

typedef enum
{
    STL_FTYPE_NOFILE = 0,     /**< no file type determined */
    STL_FTYPE_REG,            /**< a regular file */
    STL_FTYPE_DIR,            /**< a directory */
    STL_FTYPE_CHR,            /**< a character device */
    STL_FTYPE_BLK,            /**< a block device */
    STL_FTYPE_PIPE,           /**< a FIFO / pipe */
    STL_FTYPE_LNK,            /**< a symbolic link */
    STL_FTYPE_SOCK,           /**< a [unix domain] socket */
    STL_FTYPE_UNKFILE = 127   /**< a file of some other unknown type */
} stlFileType; 

/**
 * @defgroup stlFileLockFlags File Lock Flags
 * @ingroup stlFile
 * @{
 */
#define STL_FLOCK_SHARED        1       /**< Shared lock. More than one process
                                           or thread can hold a shared lock
                                           at any given time. Essentially,
                                           this is a "read lock", preventing
                                           writers from establishing an
                                           exclusive lock. */
#define STL_FLOCK_EXCLUSIVE     2       /**< Exclusive lock. Only one process
                                           may hold an exclusive lock at any
                                           given time. This is analogous to
                                           a "write lock". */

#define STL_FLOCK_TYPEMASK      0x000F  /**< mask to extract lock type */
#define STL_FLOCK_NONBLOCK      0x0010  /**< do not block while acquiring the
                                           file lock */
/** @} */

/**
 * @defgroup stlFilePermFlags File Permission Flags
 * @ingroup stlFile
 * @{
 */
#define STL_FPERM_USETID      0x8000 /**< Set user id */
#define STL_FPERM_UREAD       0x0400 /**< Read by user */
#define STL_FPERM_UWRITE      0x0200 /**< Write by user */
#define STL_FPERM_UEXECUTE    0x0100 /**< Execute by user */

#define STL_FPERM_GSETID      0x4000 /**< Set group id */
#define STL_FPERM_GREAD       0x0040 /**< Read by group */
#define STL_FPERM_GWRITE      0x0020 /**< Write by group */
#define STL_FPERM_GEXECUTE    0x0010 /**< Execute by group */

#define STL_FPERM_WSTICKY     0x2000 /**< Sticky bit */
#define STL_FPERM_WREAD       0x0004 /**< Read by others */
#define STL_FPERM_WWRITE      0x0002 /**< Write by others */
#define STL_FPERM_WEXECUTE    0x0001 /**< Execute by others */

#define STL_FPERM_OS_DEFAULT  0x0FFF /**< use OS's default permissions */

/* additional permission flags for stlCopyFile  and stlAppendFile */
#define STL_FPERM_FILE_SOURCE 0x1000 /**< Copy source file's permissions */
/** @} */

/**
 * @defgroup stlFileOpenFlags File Open Flags
 * @ingroup stlFile
 * @{
 */
#define STL_FOPEN_READ       0x00001  /**< Open the file for reading */
#define STL_FOPEN_WRITE      0x00002  /**< Open the file for writing */
#define STL_FOPEN_CREATE     0x00004  /**< Create the file if not there */
#define STL_FOPEN_APPEND     0x00008  /**< Append to the end of the file */
#define STL_FOPEN_TRUNCATE   0x00010  /**< Open the file and truncate
                                           to 0 length */
#define STL_FOPEN_BINARY     0x00020  /**< Open the file in binary mode */
#define STL_FOPEN_EXCL       0x00040  /**< Open should fail if STL_FOPEN_CREATE
                                           and file exists. */
#define STL_FOPEN_DIRECT     0x00080  /**< Open the file in direct mode */
#define STL_FOPEN_LARGEFILE  0x04000  /**< Platform dependent flag to enable
                                       * large file support, see WARNING below
                                       */
/** @} */

/**
 * @defgroup stlFileSeekFlags File Seek Flags
 * @ingroup stlFile
 * @{
 */
#define STL_FSEEK_SET SEEK_SET          /**< Set the file position */
#define STL_FSEEK_CUR SEEK_CUR          /**< Current file position */
#define STL_FSEEK_END SEEK_END          /**< Go to end of file */
/** @} */

/**
 * @defgroup stlFileAttrFlags File Attribute Flags
 * @ingroup stlFile
 * @{
 */
/* flags for apr_file_attrs_set */
#define STL_FATTR_READONLY   0x01       /**< File is read-only */
#define STL_FATTR_EXECUTABLE 0x02       /**< File is executable */
#define STL_FATTR_HIDDEN     0x04       /**< File is hidden */
/** @} */

/**
 * @brief The file information structure.
 * This is analogous to the POSIX stat structure.
 */
typedef struct stlFileInfo
{
    stlInt32     mValid;                /**< The bitmask describing
                                             valid fields of this structure */
    stlInt32     mProtection;           /**< The access permissions of the file */
    stlFileType  mFileType;             /**< The type of file */
    stlUserID    mUserID;               /**< The user id that owns the file */
    stlGroupID   mGroupID;              /**< The group id that owns the file */
    stlInode     mInodeNumber;          /**< The inode of the file */
    stlDeviceID  mDeviceID;             /**< The id of the device the file is on */
    stlInt32     mLinks;                /**< The number of hard links to the file */
    stlOffset    mSize;                 /**< The size of the file */
    stlOffset    mAllocatedSize;        /**< The storage size consumed by the file */
    stlTime      mLastAccessedTime;     /**< The time the file was last accessed */
    stlTime      mLastModifiedTime;     /**< The time the file was last modified */
    stlTime      mCreateTime;           /**< The time the file was created,
                                             or the inode was last changed */
} stlFileInfo;

/** @} */

/**
 * @addtogroup stlGeneral
 * @{
 */

/**
 * @brief Startup시 StartupInfo를 위한 공유메모리 Seed Key
 */
#define STL_STARTUP_MSG_KEY  (22581)
/**
 * OS default time-slice time(ms)
 */
#define STL_TIMESLICE_TIME   (10000)

/** @} */

/**
 * @addtogroup stlXid
 * @{
 */

#define STL_XID_DATA_SIZE    ( 128 )  /**< Xid.mData field size */
#define STL_XID_TRANS_SIZE   ( 64 )   /**< Xid.mData의 global tranaction id field size */
#define STL_XID_BRANCH_SIZE  ( 64 )   /**< Xid.mData의 branch qualifier field size */

/**
 * @brief XID structure
 */
typedef struct stlXid
{
    stlInt64   mFormatId;                 /**< format identifier */
    stlInt64   mTransLength;              /**< byte length of global transaction identifier */
    stlInt64   mBranchLength;             /**< byte length of branch qualifier identifier */
    stlChar    mData[STL_XID_DATA_SIZE];  /**< actual data */
} stlXid;

/**
 * @brief maximum XID string size
 */
#define STL_XID_STRING_SIZE   ( STL_XID_DATA_SIZE * 2 + 64 )

/** @} */

/**
 * @addtogroup stlMemorys
 * @{
 */

/**
 * @brief 8비트 aSrc를 aDest에 저장한다.
 */
#define STL_WRITE_INT8( aDest, aSrc )                   \
    {                                                   \
        ((stlChar*)(aDest))[0] = ((stlChar*)(aSrc))[0]; \
    }

/**
 * @brief (aSrc + aOffset)의 8비트를 aDest에 저장하고, aOffset을 증가한다.
 */
#define STL_READ_MOVE_INT8( aDest, aSrc, aOffset )      \
    {                                                   \
        STL_WRITE_INT8( aDest, aSrc + aOffset );        \
        aOffset += 1;                                   \
    }

/**
 * @brief aSrc의 8비트를 (aDest + aOffset)에 저장하고, aOffset을 증가한다.
 */
#define STL_WRITE_MOVE_INT8( aDest, aSrc, aOffset )     \
    {                                                   \
        STL_WRITE_INT8( aDest + aOffset, aSrc );        \
        aOffset += 1;                                   \
    }

/**
 * @brief 16비트 aSrc를 aDest에 저장한다.
 */
#ifdef STL_SUPPORT_MISSALIGNED_ADDRESS
#define STL_WRITE_INT16( aDest, aSrc )                  \
    {                                                   \
        *(stlUInt16*)(aDest) = *(stlUInt16*)(aSrc);     \
    }
#else
#define STL_WRITE_INT16( aDest, aSrc )                  \
    {                                                   \
        ((stlChar*)(aDest))[0] = ((stlChar*)(aSrc))[0]; \
        ((stlChar*)(aDest))[1] = ((stlChar*)(aSrc))[1]; \
    }
#endif

/**
 * @brief (aSrc + aOffset)의 16비트를 aDest에 저장하고, aOffset을 증가한다.
 */
#define STL_READ_MOVE_INT16( aDest, aSrc, aOffset )     \
    {                                                   \
        STL_WRITE_INT16( aDest, aSrc + aOffset );       \
        aOffset += 2;                                   \
    }

/**
 * @brief aSrc의 16비트를 (aDest + aOffset)에 저장하고, aOffset을 증가한다.
 */
#define STL_WRITE_MOVE_INT16( aDest, aSrc, aOffset )    \
    {                                                   \
        STL_WRITE_INT16( aDest + aOffset, aSrc );       \
        aOffset += 2;                                   \
    }

/**
 * @brief 32비트 aSrc를 aDest에 저장한다.
 */
#ifdef STL_SUPPORT_MISSALIGNED_ADDRESS
#define STL_WRITE_INT32( aDest, aSrc )                  \
    {                                                   \
        *(stlUInt32*)(aDest) = *(stlUInt32*)(aSrc);     \
    }
#else
#define STL_WRITE_INT32( aDest, aSrc )                  \
    {                                                   \
        ((stlChar*)(aDest))[0] = ((stlChar*)(aSrc))[0]; \
        ((stlChar*)(aDest))[1] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDest))[2] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDest))[3] = ((stlChar*)(aSrc))[3]; \
    }
#endif

/**
 * @brief (aSrc + aOffset)의 32비트를 aDest에 저장하고, aOffset을 증가한다.
 */
#define STL_READ_MOVE_INT32( aDest, aSrc, aOffset )     \
    {                                                   \
        STL_WRITE_INT32( aDest, aSrc + aOffset );       \
        aOffset += 4;                                   \
    }

/**
 * @brief aSrc의 32비트를 (aDest + aOffset)에 저장하고, aOffset을 증가한다.
 */
#define STL_WRITE_MOVE_INT32( aDest, aSrc, aOffset )    \
    {                                                   \
        STL_WRITE_INT32( aDest + aOffset, aSrc );       \
        aOffset += 4;                                   \
    }

/**
 * @brief 64비트 aSrc를 aDest에 저장한다.
 */
#ifdef STL_SUPPORT_MISSALIGNED_ADDRESS
#define STL_WRITE_INT64( aDest, aSrc )                  \
    {                                                   \
        *(stlUInt64*)(aDest) = *(stlUInt64*)(aSrc);     \
    }
#else
#define STL_WRITE_INT64( aDest, aSrc )                  \
    {                                                   \
        ((stlChar*)(aDest))[0] = ((stlChar*)(aSrc))[0]; \
        ((stlChar*)(aDest))[1] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDest))[2] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDest))[3] = ((stlChar*)(aSrc))[3]; \
        ((stlChar*)(aDest))[4] = ((stlChar*)(aSrc))[4]; \
        ((stlChar*)(aDest))[5] = ((stlChar*)(aSrc))[5]; \
        ((stlChar*)(aDest))[6] = ((stlChar*)(aSrc))[6]; \
        ((stlChar*)(aDest))[7] = ((stlChar*)(aSrc))[7]; \
    }
#endif

/**
 * @brief (aSrc + aOffset)의 32비트를 aDest에 저장하고, aOffset을 증가한다.
 */
#define STL_READ_MOVE_INT64( aDest, aSrc, aOffset )     \
    {                                                   \
        STL_WRITE_INT64( aDest, aSrc + aOffset );       \
        aOffset += 8;                                   \
    }

/**
 * @brief aSrc의 64비트를 (aDest + aOffset)에 저장하고, aOffset을 증가한다.
 */
#define STL_WRITE_MOVE_INT64( aDest, aSrc, aOffset )    \
    {                                                   \
        STL_WRITE_INT64( aDest + aOffset, aSrc );       \
        aOffset += 8;                                   \
    }

/**
 * @brief 32비트 Float aSrc를 aDest에 저장한다.
 */
#ifdef STL_SUPPORT_MISSALIGNED_ADDRESS
#define STL_WRITE_FLOAT32( aDest, aSrc )                \
    {                                                   \
        *(stlFloat32*)(aDest) = *(stlFloat32*)(aSrc);   \
    }
#else
#define STL_WRITE_FLOAT32( aDest, aSrc )                \
    {                                                   \
        ((stlChar*)(aDest))[0] = ((stlChar*)(aSrc))[0]; \
        ((stlChar*)(aDest))[1] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDest))[2] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDest))[3] = ((stlChar*)(aSrc))[3]; \
    }
#endif

/**
 * @brief 64비트 Float aSrc를 aDest에 저장한다.
 */
#ifdef STL_SUPPORT_MISSALIGNED_ADDRESS
#define STL_WRITE_FLOAT64( aDest, aSrc )                \
    {                                                   \
        *(stlFloat64*)(aDest) = *(stlFloat64*)(aSrc);   \
    }
#else
#define STL_WRITE_FLOAT64( aDest, aSrc )                \
    {                                                   \
        ((stlChar*)(aDest))[0] = ((stlChar*)(aSrc))[0]; \
        ((stlChar*)(aDest))[1] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDest))[2] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDest))[3] = ((stlChar*)(aSrc))[3]; \
        ((stlChar*)(aDest))[4] = ((stlChar*)(aSrc))[4]; \
        ((stlChar*)(aDest))[5] = ((stlChar*)(aSrc))[5]; \
        ((stlChar*)(aDest))[6] = ((stlChar*)(aSrc))[6]; \
        ((stlChar*)(aDest))[7] = ((stlChar*)(aSrc))[7]; \
    }
#endif

/**
 * @brief 64비트 Pointer를 aDest에 저장한다.
 */
#define STL_WRITE_POINTER( aDest, aSrc )                \
    {                                                   \
        ((stlChar*)(aDest))[0] = ((stlChar*)(aSrc))[0]; \
        ((stlChar*)(aDest))[1] = ((stlChar*)(aSrc))[1]; \
        ((stlChar*)(aDest))[2] = ((stlChar*)(aSrc))[2]; \
        ((stlChar*)(aDest))[3] = ((stlChar*)(aSrc))[3]; \
        ((stlChar*)(aDest))[4] = ((stlChar*)(aSrc))[4]; \
        ((stlChar*)(aDest))[5] = ((stlChar*)(aSrc))[5]; \
        ((stlChar*)(aDest))[6] = ((stlChar*)(aSrc))[6]; \
        ((stlChar*)(aDest))[7] = ((stlChar*)(aSrc))[7]; \
    }

/**
 * @brief (aSrc + aOffset)의 Pointer를 aDest에 저장하고, aOffset을 증가한다.
 */
#define STL_READ_MOVE_POINTER( aDest, aSrc, aOffset )   \
    {                                                   \
        STL_WRITE_POINTER( aDest, aSrc + aOffset );     \
        aOffset += 8;                                   \
    }

/**
 * @brief aSrc의 Pointer를 (aDest + aOffset)에 저장하고, aOffset을 증가한다.
 */
#define STL_WRITE_MOVE_POINTER( aDest, aSrc, aOffset )  \
    {                                                   \
        STL_WRITE_POINTER( aDest + aOffset, aSrc );     \
        aOffset += 8;                                   \
    }

/**
 * @brief aSrc에서 aBytes만큼의 바이트를 aDest에 저장한다.
 */
#define STL_WRITE_BYTES( aDest, aSrc, aBytes )                          \
    {                                                                   \
        stlMemcpy( (stlChar*)(aDest), (stlChar*)(aSrc), aBytes );       \
    }

/**
 * @brief (aSrc + aOffset)의 aBytes만큼의 바이트를 aDest에 저장하고, aOffset을 증가한다.
 */
#define STL_READ_MOVE_BYTES( aDest, aSrc, aBytes, aOffset )     \
    {                                                           \
        STL_WRITE_BYTES( aDest, aSrc + aOffset, aBytes );       \
        aOffset += aBytes;                                      \
    }

/**
 * @brief aSrc의 aBytes만큼의 바이트를 (aDest + aOffset)에 저장하고, aOffset을 증가한다.
 */
#define STL_WRITE_MOVE_BYTES( aDest, aSrc, aBytes, aOffset )    \
    {                                                           \
        STL_WRITE_BYTES( aDest + aOffset, aSrc, aBytes );       \
        aOffset += aBytes;                                      \
    }


/** @} */

/**
 * @addtogroup stlGetOpt
 * @{
 */

/**
 * @brief 명령행 인자 정보를 저장하기 위한 구조체
 */ 
typedef struct stlGetOpt
{
    stlInt32                mArgc;   /**< 인자의 수  */
    const stlChar * const * mArgv;   /**< 인자의 배열 포인터  */
    stlInt32                mIndex;  /**< 인자 배열 포인터에서 위치 */
    stlInt32                mOptCh;  /**< 옵션 */
    stlChar const *         mOptArg; /**< 옵션의 인자 */
} stlGetOpt;

typedef struct stlGetOptOption
{
    const stlChar * mName;         /**< Long Option Name  */
    stlInt32        mOptCh;        /**< Short Option Letter */
    stlInt32        mHasArg;       /**< Argument를 갖는가 */
    stlChar const * mDescription;  /**< Decription */
} stlGetOptOption;

/** @} */

/**
 * @addtogroup stlStrings
 * @{
 */

#define STL_NTS              (-3) /**< null-terminated string */

/**
 * @defgroup stlPaintTextAttr Paint Text Attribute
 * @ingroup stlStrings
 * @{
 */
#define STL_PTA_OFF          (0)   /**< All attributes off */
#define STL_PTA_BOLD         (1)   /**< Bold on */
#define STL_PTA_UNDERSCORE   (4)   /**< Underscore */
#define STL_PTA_BLINK        (5)   /**< Blink on */
#define STL_PTA_REVERSE      (7)   /**< Reverse */
/** @} */

/**
 * @defgroup stlPaintForeColor Paint Foreground Color
 * @ingroup stlStrings
 * @{
 */
#define STL_PFC_OFF            (-1)   /**< Ignore */
#define STL_PFC_BLACK          (0)    /**< Black */
#define STL_PFC_GRAY           (1)    /**< Gray */
#define STL_PFC_RED            (2)    /**< Red */
#define STL_PFC_LIGHT_RED      (3)    /**< Light Red */
#define STL_PFC_GREEN          (4)    /**< Green */
#define STL_PFC_LIGHT_GREEN    (5)    /**< Light Green */
#define STL_PFC_BROWN          (6)    /**< Brown */
#define STL_PFC_YELLOW         (7)    /**< Yellow */
#define STL_PFC_BLUE           (8)    /**< Blue */
#define STL_PFC_LIGHT_BLUE     (9)    /**< Light Blue */
#define STL_PFC_MAGENTA        (10)   /**< Magenta */
#define STL_PFC_LIGHT_MAGENTA  (11)   /**< Light Magenta */
#define STL_PFC_CYAN           (12)   /**< Cyan */
#define STL_PFC_LIGHT_CYAN     (13)   /**< Light Cyan */
#define STL_PFC_WHITE          (14)   /**< White */
/** @} */

/**
 * @defgroup stlPaintBackColor Paint Background Color
 * @ingroup stlStrings
 * @{
 */
#define STL_PBC_OFF          (0)    /**< Ignore */
#define STL_PBC_BLACK        (40)   /**< Black */
#define STL_PBC_RED          (41)   /**< Red */
#define STL_PBC_GREEN        (42)   /**< Green */
#define STL_PBC_YELLOW       (43)   /**< Yellow */
#define STL_PBC_BLUE         (44)   /**< Blue */
#define STL_PBC_MAGENTA      (45)   /**< Magenta */
#define STL_PBC_CYAN         (46)   /**< Cyan */
#define STL_PBC_WHITE        (47)   /**< White */
/** @} */

/**
 * @brief Paint Attribute
 */
typedef struct stlPaintAttr
{
    stlInt32 mTextAttr;       /**< Text Attribute @see stlPaintTextAttr */
    stlInt32 mForeColor;      /**< Foreground Color @see stlPaintForeColor */
    stlInt32 mBackColor;      /**< Background Color @see stlPaintBackColor */
} stlPaintAttr;

/** @} */

/**
 * @addtogroup stlNetworkIo
 * @{
 */

#ifndef STL_MAX_SECS_TO_LINGER
/** SO_LINGER에 적용할 최대 대기 초 */
#define STL_MAX_SECS_TO_LINGER 30
#endif

#ifndef STL_MAX_HOST_LEN
/** hostname의 최대 길이 */
#define STL_MAX_HOST_LEN 256
#endif

#ifndef STL_ANYADDR
/** 디폴트 'any' address */
#define STL_ANYADDR "0.0.0.0"
#endif


/**
 * @defgroup stlSocketOption Socket Option Flags
 * @ingroup stlNetworkIo
 * @{
 */
#define STL_SOPT_SO_LINGER        1    /**< Linger */
#define STL_SOPT_SO_KEEPALIVE     2    /**< Keepalive */
#define STL_SOPT_SO_DEBUG         4    /**< Debug */
#define STL_SOPT_SO_NONBLOCK      8    /**< Non-blocking IO */
#define STL_SOPT_SO_REUSEADDR     16   /**< Reuse addresses */
#define STL_SOPT_SO_SNDBUF        64   /**< Send buffer */
#define STL_SOPT_SO_RCVBUF        128  /**< Receive buffer */
#define STL_SOPT_TCP_NODELAY      512  /**< For SCTP socket,
                                        * 내부적으로 SCTP_NODELAY에 매핑된다
                                        */
#define STL_SOPT_TCP_NOPUSH       1024 /**< No push */
#define STL_SOPT_RESET_NODELAY    2048 /**< This flag is ONLY set internally
                                        * when we set STL_TCP_NOPUSH with
                                        * STL_TCP_NODELAY set to tell us that
                                        * STL_TCP_NODELAY should be turned on
                                        * again when NOPUSH is turned off
                                        */
#define STL_SOPT_IPV6_V6ONLY     16384 /**< IPv6로 listen하고 있는 소켓에 대해
                                        * IPv4 연결 요청을 받지 않는다
                                        */
#define STL_SOPT_TCP_DEFER_ACCEPT 32768 /**< 읽을 data가 올때까지 accept를 완료하지 않는다
                                         */
/** @} */

/**
 * @defgroup stlShutdownHow shutdown시에 닫을 채널
 * @ingroup stlNetworkIo
 * @{
 */
typedef enum {
    STL_SHUTDOWN_READ,          /**< read 채널을 닫음 */
    STL_SHUTDOWN_WRITE,         /**< write 채널을 닫음 */
    STL_SHUTDOWN_READWRITE      /**< read와 write 둘 다 닫음 */
} stlShutdownHow;
/** @} */


/** @def STL_INADDR_NONE
 * 모든 플랫폼이 INADDR_NONE을 갖고있지 않아서 정의함
 */
#if defined(INADDR_NONE) || defined( DOXYGEN )
#define STL_INADDR_NONE INADDR_NONE
#else
#define STL_INADDR_NONE ((stlUInt32) 0xffffffff)
#endif

/** @def STL_INADDR_ANY_STR
 * INADDR_ANY에 대한 IP를 string 형태로 정의
 */
#define STL_INADDR_ANY_STR      "0.0.0.0"

/** @def STL_FLAG_IPV6_SUPPORT
 * IPv6를 지원하는 플랫폼인지에 대한 체크 플래그
 */
#if defined(AF_INET6) || defined( DOXYGEN )
#define STL_FLAG_IPV6_SUPPORT   1
#else
#define STL_FLAG_IPV6_SUPPORT   0
#endif

/** @def STL_FLAG_AF_UNSPEC_SUPPORT
 * AF_UNSPEC을 지원하는 플랫폼인지에 대한 체크 플래그
 */
#if defined(AF_UNSPEC) || defined( DOXYGEN )
#define STL_FLAG_AF_UNSPEC_SUPPORT   1
#else
#define STL_FLAG_AF_UNSPEC_SUPPORT   0
#endif

/**
 * @defgroup stlAddressFamily Socket에서 사용하는 Address Family
 * @ingroup stlNetworkIo
 * @{
 */

/**
 * 모든 플랫폼이 AF_INET을 갖고있지는 않아서 정의함
 * 디폴트 값은 FreeBSD 4.1.1에서 가져옴
 */
#define STL_AF_INET     AF_INET
/** 
 * system이 알아서 address family를 정하도록 함
 * 모든 플랫폼에 다 정의되어 있지는 않음
 * STL_FLAG_AF_UNSPEC_SUPPORT로 확인 후 사용해야 함
 */
#if defined(AF_UNSPEC) || defined( DOXYGEN )
#define STL_AF_UNSPEC   AF_UNSPEC
#else
#define STL_AF_UNSPEC   0
#endif

#if defined(AF_INET6) || defined( DOXYGEN )
/**
 * IPv6 Address Family. 모든 플랫폼에 다 정의되어 있지는 않음
 * STL_FLAG_IPV6_SUPPORT로 확인 후 사용해야 함
 */
#define STL_AF_INET6    AF_INET6
#endif

#if defined(AF_UNIX)  || defined( DOXYGEN )
/** 
 * UNIX DOMIAN SOCKET(== AF_LOCAL). 모든 플랫폼에 다 정의되어 있지는 않음
 */
#define STL_AF_UNIX  AF_UNIX
#endif

/** @} */

/**
 * @defgroup stlSocketType Socket Types
 * @ingroup stlNetworkIo
 * @{
 */

/**
 * 연결 기반의 믿을 수 있는 양방향 바이트 스트림 통신
 */
#define STL_SOCK_STREAM  SOCK_STREAM

/**
 * 연결이 필요없고 부분 소실이 날 수도 있는 데이터그램 통신
 */
#define STL_SOCK_DGRAM  SOCK_DGRAM

/**
 * 소켓 옵션에 O_NONBLOCK을 설정
 */
#define STL_SOCK_NONBLOCK  SOCK_NONBLOCK

/**
 * 소켓 옵션에 FD_CLOEXEC를 설정
 */
#if defined (SOCK_CLOEXEC) || defined (DOXYGEN)
#define STL_SOCK_CLOEXEC  SOCK_CLOEXEC
#endif

/** @} */


/**
 * @defgroup stlNetworkProtocol Socket의 Protocol
 * @ingroup stlNetworkIo
 * @{
 */

/**
 * TCP 프로토콜
 */
#define STL_PROTO_TCP       6
/**
 * UDP 프로토콜
 */
#define STL_PROTO_UDP      17
/**
 * SCTP 프로토콜
 */
#define STL_PROTO_SCTP    132

/**
 * Endian
 */
#define STL_BIG_ENDIAN      (0)
#define STL_LITTLE_ENDIAN   (-1)

#ifdef STL_IS_BIGENDIAN
#define STL_PLATFORM_ENDIAN             STL_BIG_ENDIAN
#else
#define STL_PLATFORM_ENDIAN             STL_LITTLE_ENDIAN
#endif


/** @} */

/**
 * @defgroup stlCommunicationGroupContext Communication Context
 * @ingroup stlCommMgr
 * @{
 */

#define STL_SOCKET_IN_CONTEXT(aContext)    ((aContext).mPollFd.mSocketHandle)

/** @} */


/**
 * @addtogroup stlPoll
 * @{
 */

/**
 * @brief stlDispatchPollSet()에서 이벤트가 발생한 소켓마다 호출되는 함수의 prototype
 * @param[in] aPollFd 이벤트가 발생한 소켓
 * @param[in] aErrorStack 에러 스택
 * @remark @a aPollFd 의 mRetEnvents에는 해당 소켓에 어떤 이벤트가 발생했는지 설정되어 있다.
 */
typedef stlStatus (*stlPollSetCallback)(stlPollFd     * aPollFd,
                                        stlErrorStack * aErrorStack);

/**
 * @brief Poll 옵션
 */

#if defined( POLLIN ) || defined( DOXYGEN )
#define STL_POLLIN    POLLIN      /**< 읽을 데이터가 있다. */
#else
#define STL_POLLIN    0x00000001
#endif
#if defined( POLLPRI ) || defined( DOXYGEN )
#define STL_POLLPRI   POLLPRI     /**< 읽을 긴급한 데이터가 있다. */
#else
#define STL_POLLPRI   0x00000002
#endif
#if defined( POLLOUT ) || defined( DOXYGEN )
#define STL_POLLOUT   POLLOUT     /**< 쓰기가 block 이 아니다. */
#else
#define STL_POLLOUT   0x00000004
#endif
#if defined( POLLERR ) || defined( DOXYGEN )
#define STL_POLLERR   POLLERR     /**< 에러 발생 */
#else
#define STL_POLLERR   0x00000008
#endif

#define STL_POLLMASK  0xFFFFFFFF  /**< MASK */

/** @} */

/**
 * @addtogroup stlShm
 * @{
 */

/**
 * @brief 공유 메모리 세그먼트 이름의 최대 길이
 */
#define STL_MAX_SHM_NAME      7

/**
 * @brief 공유 메모리 세그먼트 정보를 저장하는 구조체
 */
typedef struct stlShm
{
    stlChar       mName[STL_MAX_SHM_NAME+1]; /**< 공유 메모리 이름(Null Terminated String) */
    void        * mAddr;   /**< 할당된 공유 메모리 세그먼트의 주소 */
    stlSize       mSize;   /**< 할당된 공유 메모리 세그먼트의 크기 */
    stlShmHandle  mHandle; /**< 할당된 공유 메모리 세그먼트의 핸들 */
    stlInt32      mKey;    /**< 공유 메모리 세그먼트의 키 */
} stlShm;


/** @} */

/**
 * @addtogroup stlMsgQueue
 * @{
 */

/**
 * @defgroup stlMsgQueueType Message Queue Type
 * @ingroup stlMsgQueue
 * @{
 */
#define STL_MSGQ_TYPE_EXEC_PROCESS      ( 1 )  /**< Execute Process Message */

/** @} */

/**
 * @brief 메세지 큐 구조체
 */
typedef struct stlMsgQueue
{
    stlInt32  mKey;     /**< message queue key */
    stlInt32  mId;      /**< message queue identifier */
} stlMsgQueue;

/**
 * @brief 메세지 Buffer 구조체
 */
typedef struct stlMsgBuffer
{
    stlInt64  mType;       /**< @see stlMsgQueue */
    stlChar   mText[1];    /**< message body */
} stlMsgBuffer;

/**
 * @brief Message Queue Flag - NOWAIT
 */
#define STL_MSGQ_FLAG_NOWAIT  IPC_NOWAIT

/** @} */

/**
 * @addtogroup stlSignal
 * @{
 */

/**
 * @brief Signal Handler Function의 프로토타입
 */
typedef void (*stlSignalFunc)( stlInt32 );

/**
 * @brief Signal Extended Handler Function의 프로토타입
 */
typedef void (*stlSignalExFunc)( stlInt32, void*, void* );

/**
 * @defgroup stlSignalNumber Signal Number
 * @ingroup stlSignal
 * @{
 */
/**
 * @brief 플랫폼에서 제공되지 않는 Signal 번호.
 */
#define STL_SIGNAL_UNKNOWN   0x0FFFFFFF

/**
 * @brief ignore signal handler
 */
#define STL_SIGNAL_IGNORE   SIG_IGN

/**
 * @brief Trace trap
 */
#if defined( SIGTRAP ) || defined( DOXYGEN )
#define STL_SIGNAL_TRAP SIGTRAP
#else
#define STL_SIGNAL_TRAP STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Abort (abort function)
 */
#if defined( SIGABRT ) || defined( DOXYGEN )
#define STL_SIGNAL_ABRT SIGABRT

#else
#define STL_SIGNAL_ABRT STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Illegal instruction
 */
#if defined( SIGILL ) || defined( DOXYGEN )
#define STL_SIGNAL_ILL SIGILL
#else
#define STL_SIGNAL_ILL STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Floating-point exception
 */
#if defined( SIGFPE ) || defined( DOXYGEN )
#define STL_SIGNAL_FPE SIGFPE
#else
#define STL_SIGNAL_FPE STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Segmentation violation
 */
#if defined( SIGSEGV ) || defined( DOXYGEN )
#define STL_SIGNAL_SEGV SIGSEGV
#else
#define STL_SIGNAL_SEGV STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief BUS error
 */
#if defined( SIGBUS ) || defined( DOXYGEN )
#define STL_SIGNAL_BUS SIGBUS
#else
#define STL_SIGNAL_BUS STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Bad system call
 */
#if defined( SIGSYS ) || defined( DOXYGEN )
#define STL_SIGNAL_SYS SIGSYS
#else
#define STL_SIGNAL_SYS STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Continue
 */
#if defined( SIGCONT ) || defined( DOXYGEN )
#define STL_SIGNAL_CONT SIGCONT
#else
#define STL_SIGNAL_CONT STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Quit
 */
#if defined( SIGQUIT ) || defined( DOXYGEN )
#define STL_SIGNAL_QUIT SIGQUIT
#else
#define STL_SIGNAL_QUIT STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Termination
 */
#if defined( SIGTERM ) || defined( DOXYGEN )
#define STL_SIGNAL_TERM SIGTERM
#else
#define STL_SIGNAL_TERM STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Alarm clock
 */
#if defined( SIGALRM ) || defined( DOXYGEN )
#define STL_SIGNAL_ALRM SIGALRM
#else
#define STL_SIGNAL_ALRM STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Hangup
 */
#if defined( SIGHUP ) || defined( DOXYGEN )
#define STL_SIGNAL_HUP SIGHUP
#else
#define STL_SIGNAL_HUP STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Interrupt
 */
#if defined( SIGINT ) || defined( DOXYGEN )
#define STL_SIGNAL_INT SIGINT
#else
#define STL_SIGNAL_INT STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Kill, unblockable
 */
#if defined( SIGKILL ) || defined( DOXYGEN )
#define STL_SIGNAL_KILL SIGKILL
#else
#define STL_SIGNAL_KILL STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Broken pipe
 */
#if defined( SIGPIPE ) || defined( DOXYGEN )
#define STL_SIGNAL_PIPE SIGPIPE
#else
#define STL_SIGNAL_PIPE STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief IOT trap
 */
#if defined( SIGIOT ) || defined( DOXYGEN )
#define STL_SIGNAL_IOT SIGIOT
#else
#define STL_SIGNAL_IOT STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief User-defined signal 1
 */
#if defined( SIGUSR1 ) || defined( DOXYGEN )
#define STL_SIGNAL_USR1 SIGUSR1
#else
#define STL_SIGNAL_USR1 STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief User-defined signal 2
 */
#if defined( SIGUSR2 ) || defined( DOXYGEN )
#define STL_SIGNAL_USR2 SIGUSR2
#else
#define STL_SIGNAL_USR2 STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Stop, unblockable
 */
#if defined( SIGSTOP ) || defined( DOXYGEN )
#define STL_SIGNAL_STOP SIGSTOP
#else
#define STL_SIGNAL_STOP STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Stack fault
 */
#if defined( SIGSTKFLT ) || defined( DOXYGEN )
#define STL_SIGNAL_STKFLT SIGSTKFLT
#else
#define STL_SIGNAL_STKFLT STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Same as SIGCHLD
 */
#if defined( SIGCLD ) || defined( DOXYGEN )
#define STL_SIGNAL_CLD SIGCLD
#else
#define STL_SIGNAL_CLD STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Child status has changed
 */
#if defined( SIGCHLD ) || defined( DOXYGEN )
#define STL_SIGNAL_CHLD SIGCHLD
#else
#define STL_SIGNAL_CHLD STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Pollable event occurred
 */
#if defined( SIGPOLL ) || defined( DOXYGEN )
#define STL_SIGNAL_POLL SIGPOLL
#else
#define STL_SIGNAL_POLL STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Real-time event occurred
 */
#if defined( SIGRTMIN ) || defined( DOXYGEN )
#define STL_SIGNAL_RTMIN SIGRTMIN
#else
#define STL_SIGNAL_RTMIN STL_SIGNAL_UNKNOWN
#endif

/**
 * @brief Stop typed at terminal
 */
#if defined( SIGTSTP ) || defined( DOXYGEN )
#define STL_SIGNAL_TSTP SIGTSTP
#else
#define STL_SIGNAL_TSTP STL_SIGNAL_UNKNOWN
#endif

/** @} */
/** @} */

/**
 * @addtogroup stlRing
 * @{
 */

/**
 * @brief 연결할 객체 안에 링크 구조체를 선언해 사용한다.
 * @code
 *     struct MyElement
 *     {
 *         stlRingEntry   mLink;
 *         stlInt32       mFoo;
 *         stlChar      * mBar;
 *     }
 * @endcode
 */
typedef struct stlRingEntry
{
    void * mNext;
    void * mPrev;
} stlRingEntry;

/**
 * @brief Ring 관리를 위한 Ring Head
 */
typedef struct stlRingHead
{
    stlRingEntry mLink;
    stlUInt64    mLinkOffset;
} stlRingHead;

/** @} */

/**
 * @addtogroup stlDynamicAllocator
 * @{
 */

typedef struct stlDynamicAllocator
{

    stlRingHead    mChunkRing;     /**< 할당된 chunk들의 ring */
    stlInt64       mInitSize;      /**< Init size */
    stlInt64       mNextSize;      /**< Next size */
    stlUInt64      mTotalSize;     /**< 할당된 모든 chunk 메모리의 크기 */
} stlDynamicAllocator;

/** @} */


/**
 * @addtogroup stlThreadProc
 * @{
 */

/**
 * @brief Thread Handle 초기화 매크로
 */
#define STL_INIT_THREAD_HANDLE( aThreadHandle ) \
    {                                           \
        (aThreadHandle)->mHandle = -1;          \
        (aThreadHandle)->mProc.mProcID = -1;    \
    }

#if defined( STL_HAVE_CPUSET )

/**
 * @brief CPU set 타입 정의
 */
typedef cpu_set_t stlCpuSet;

/**
 * @brief CPU set의 크기
 */
#define STL_CPU_SET_SIZE  CPU_SETSIZE
/**
 * @brief CPU set에 특정 CPU 설정
 */
#define STL_CPU_SET       CPU_SET
/**
 * @brief CPU set OR CPU set
 */
#define STL_CPU_OR        CPU_OR
/**
 * @brief CPU set 초기화
 */
#define STL_CPU_ZERO      CPU_ZERO

#else

/**
 * @brief CPU set 타입 정의
 */
typedef stlInt32 stlCpuSet;

/**
 * @brief CPU set의 크기
 */
#define STL_CPU_SET_SIZE  0
/**
 * @brief CPU set에 특정 CPU 설정
 */
#define STL_CPU_SET( aCpu, aCpuSet ) 
/**
 * @brief CPU set OR CPU set
 */
#define STL_CPU_OR( aDestSet, aSrcSet1, aSrcSet2 )
/**
 * @brief CPU set 초기화
 */
#define STL_CPU_ZERO( aCpuSet )

#endif  /* STL_HAVE_CPUSET */

/**
 * @brief thread cancellation section으로 진입함.
 */
#define STL_ENTER_CANCELLATION_SECTION( aFunc, aArg )  pthread_cleanup_push( aFunc, aArg )
/**
 * @brief thread cancellation section을 떠남.
 */
#define STL_LEAVE_CANCELLATION_SECTION()               pthread_cleanup_pop( 0 )

/**
 * @brief A cancellation request is deferred until the thread next calls
 */
#define STL_THREAD_CANCEL_DEFERRED       PTHREAD_CANCEL_DEFERRED
/**
 * @brief The thread can be canceled at any time
 */
#define STL_THREAD_CANCEL_ASYNCHRONOUS   PTHREAD_CANCEL_ASYNCHRONOUS

/** @} */

/**
 * @addtogroup stlSemaphore
 * @{
 */

#define STL_MAX_SEM_NAME            7
#define STL_MAX_NAMED_SEM_NAME      15

typedef struct stlSemaphore
{
    stlChar       mName[STL_MAX_SEM_NAME+1]; /**< 세마포어 이름(Null Terminated String) */
    stlSemHandle  mHandle;
} stlSemaphore;

typedef struct stlNamedSemaphore
{
    stlChar             mName[STL_MAX_NAMED_SEM_NAME+1]; /**< 세마포어 이름(Null Terminated String) */
    stlNamedSemHandle * mHandle;
} stlNamedSemaphore;

typedef struct stlUnNamedSemaphore
{
    stlUnNamedSemHandle mHandle;
} stlUnNamedSemaphore;

typedef struct stlEventSemaphore
{
    stlUnNamedSemaphore mUnNamedSemaphore;
} stlEventSemaphore;

/** @} */

/**
 * @addtogroup stlTime
 * @{
 */

/** invalid stlTime value */
#define STL_INVALID_STL_TIME  ( 0 )

/** number of microseconds per second */
#define STL_USEC_PER_SEC STL_TIME_C(1000000)

/** @return stlTime as a second */
#define STL_GET_SEC_TIME(aTime) ((aTime) / STL_USEC_PER_SEC)

/** @return stlTime as a usec */
#define STL_GET_USEC_TIME(aTime) ((aTime) % STL_USEC_PER_SEC)

/** @return stlTime as a msec */
#define STL_GET_MSEC_TIME(aTime) (((aTime) / 1000) % 1000)

/** @return stlTime as a msec */
#define STL_GET_AS_MSEC_TIME(aTime) ((aTime) / 1000)

/** @return milliseconds as an stlTime */
#define STL_SET_MSEC_TIME(aMsec) ((stlTime)(aMsec) * 1000)

/** @return seconds as an stlTime */
#define STL_SET_SEC_TIME(aSec) ((stlTime)(aSec) * STL_USEC_PER_SEC)

/** @return a second and usec combination as an stlTime */
#define STL_SET_TIME(aSec, aUsec) ((stlTime)(aSec) * STL_USEC_PER_SEC   \
                                   + (stlTime)(aUsec))

/**
 * @brief 시간 정보를 표현하는 구조체
 * @remark ANSI의 struct tm과 다르게 마이크로초를 지원한다.
 */
typedef struct stlExpTime
{    
    stlInt32 mMicroSecond;      /**< mSecond에서 지간 마이크로초 */
    stlInt32 mSecond;           /**< (0-61) mMinute에서 지난 초 */
    stlInt32 mMinute;           /**< (0-59) mHour에서 지난 분 */
    stlInt32 mHour;             /**< (0-23) 자정에서부터 지난 시 */
    stlInt32 mDay;              /**< (1-31) 월의 날 */
    stlInt32 mMonth;            /**< (0-11) 년의 달 */
    stlInt32 mYear;             /**< 1900년에서부터 지난 년 */
    stlInt32 mWeekDay;          /**< (0-6) 일요일에서부터 지난 날 */    
    stlInt32 mYearDay;          /**< (0-365) 1월 1일에서부터 지난 날 */    
    stlInt32 mIsDaylightSaving; /**< 일광 절약 시간 (썸머타임) */
    stlInt32 mGMTOffset;        /**< UTC와 차이 초 */
} stlExpTime;

/** @} */

/**
 * @addtogroup stlSystem
 * @{
 */

typedef struct stlCpuInfo
{
    stlInt64  mCpuCount;
} stlCpuInfo;

#ifndef RLIMIT_CPU
#define RLIMIT_CPU    0
#endif

#ifndef RLIMIT_FSIZE
#define RLIMIT_FSIZE  1
#endif

#ifndef RLIMIT_DATA
#define RLIMIT_DATA   2
#endif

#ifndef RLIMIT_STACK
#define RLIMIT_STACK  3
#endif

#ifndef RLIMIT_CORE
#define RLIMIT_CORE   4
#endif

#ifndef RLIMIT_RSS
#define RLIMIT_RSS    5
#endif

#ifndef RLIMIT_NOFILE
#define RLIMIT_NOFILE 7
#endif

#ifndef RLIMIT_AS
#define RLIMIT_AS     9
#endif

/**
 * HP에서 RLIMIT_NPROC, RLIMIT_MEMLOCK 없어서 제거했음.
 * linux, hp, sun 공통 분만 적용함.
 */
typedef enum stlResourceLimitType
{
    STL_RESOURCE_LIMIT_CPU    = RLIMIT_CPU,     /* 초 단위의 CPU 시간 */
    STL_RESOURCE_LIMIT_FSIZE  = RLIMIT_FSIZE,   /* 최대 파일 크기 */
    STL_RESOURCE_LIMIT_DATA   = RLIMIT_DATA,    /* 최대 데이타 크기 */
    STL_RESOURCE_LIMIT_STACK  = RLIMIT_STACK,   /* 최대 스택 크기 */
    STL_RESOURCE_LIMIT_CORE   = RLIMIT_CORE,    /* 최대 코어 파일 크기 */
    STL_RESOURCE_LIMIT_RSS    = RLIMIT_RSS,     /* 최대 거주 집합 크기 */
    STL_RESOURCE_LIMIT_NOFILE = RLIMIT_NOFILE,  /* 최대 열 수 있는 파일의 수 */
    STL_RESOURCE_LIMIT_AS     = RLIMIT_AS       /* 주소 공간(가상 메모리) 제한값 */
} stlResourceLimitType;

typedef struct stlResourceLimit
{
    stlInt64  mCurrent;
    stlInt64  mMax;
} stlResourceLimit;


/** @} */

/**
 * @addtogroup stlIni
 * @{
 */

#define STL_MAX_INI_LINE              (1000)
#define STL_MAX_INI_OBJECT_NAME       STL_MAX_SQL_IDENTIFIER_LENGTH
#define STL_MAX_INI_PROPERTY_NAME     STL_MAX_SQL_IDENTIFIER_LENGTH
#define STL_MAX_INI_PROPERTY_VALUE    STL_MAX_SQL_IDENTIFIER_LENGTH

typedef struct stlIniProperty
{
    stlRingEntry mLink;
    stlChar      mName[STL_MAX_INI_PROPERTY_NAME + 1];
    stlChar      mValue[STL_MAX_INI_PROPERTY_NAME + 1];
} stlIniProperty;
    
typedef struct stlIniObject
{
    stlRingHead      mPropertyRing;
    stlRingEntry     mLink;
    stlChar          mName[STL_MAX_INI_OBJECT_NAME + 1];
} stlIniObject;

typedef struct stlIni
{
    stlRingHead    mObjectRing;
    stlAllocator   mAllocator;
    stlIniObject * mCurrentObject;
} stlIni;

typedef enum stlIniDataType
{
    STL_INI_DATA_TYPE_INT64 = 100,
    STL_INI_DATA_TYPE_STRING,
    STL_INI_DATA_TYPE_BOOLEAN
} stlIniDataType;

typedef struct stlIniFormat
{
    stlChar         * mOjbect;
    stlChar         * mName;
    stlIniDataType    mDataType;
    stlChar         * mDefaultValue;
    stlInt64          mMin;     /* string인 경우 자리수임 */
    stlInt64          mMax;     /* string인 경우 자리수임 */
    void            * mValue;
    stlSize           mValueSize;
} stlIniFormat;


/** @} */

/**
 * @addtogroup stlDigest
 * @{
 */

#define STL_CRC32_INIT_VALUE                (0)

/** @} */

#define  STL_ENV_DB_HOME                    STL_PRODUCT_NAME"_HOME"
#define  STL_ENV_DB_DATA                    STL_PRODUCT_NAME"_DATA"

/**
 * @addtogroup stlSpinLock
 * @{
 */
#define  STL_SPINLOCK_STATUS_INIT    0  /**< 아무도 latch를 획득하지 않았음 */
#define  STL_SPINLOCK_STATUS_LOCKED  1  /**< 누군가 래치를 획득하여 작업중임 */

#define  STL_INIT_SPIN_LOCK( aSpinLock ) { (aSpinLock) = STL_SPINLOCK_STATUS_INIT; }

/**
 * @brief SpinLock 구조체
 */
typedef stlUInt32 stlSpinLock;

/** @} */



/**
 * @addtogroup stlArrayAllocator
 * @{
 */

/**
 * @brief aAllocator로 첫번째 Element를 가져온다
 */
#define STL_ARRAY_GET_FIRST_ELEMENT( aAllocator )               ((stlArrayElement *)((stlChar *)(aAllocator) + STL_SIZEOF(stlArrayAllocator)))

/**
 * @brief aAllocator와 aArrayElement로 next Element를 가져온다
 */
#define STL_ARRAY_GET_NEXT_ELEMENT( aAllocator, aArrayElement ) ((stlArrayElement*)((stlChar *)(aArrayElement) + (aAllocator)->mElementSize))

/**
 * @brief aArrayElement로 Element body를 가져온다.
 */
#define STL_ARRAY_GET_BODY_FROM_HEADER( aArrayElement )        ((stlChar *)((stlUInt8 *)(aArrayElement) + STL_SIZEOF(stlArrayElement)))

/**
 * @brief body로 header위치를 가져온다.
 */
#define STL_ARRAY_GET_HEADER_FROM_BODY( aBody )                ((stlArrayElement *)((stlChar *)(aBody) - STL_SIZEOF(stlArrayElement)))


typedef struct stlArrayElement  stlArrayElement;

/**
 * @brief stlArrayAllocator의 element header 구조체.
 * @remark  element body는 runtime시 결정되기 때문에 동적으로 할당되어 element header뒤에 위치하게 된다.
 */
struct stlArrayElement
{
    /* 메모리를 줄이려면 아래 두 변수를 통합해도 될것 같음 */
    struct stlArrayElement * mNextFreeElement;    /**< 사용중이 아닌 다음 Element */
//    stlInt64                 mSize;               /**< 쓰여진 body size */
};

typedef stlStatus (*stlCheckCancellation)( stlBool         * aCancel,
                                           stlErrorStack   * aErrorStack );
/**
 * @brief 배열을 이용한 메모리 할당자의 header 구조체
 * @remark    mNextFreeElement는 할당 요청이 오면 넘겨줄 element를 가르키고 있는다.
 */
typedef struct stlArrayAllocator
{
    stlInt64             mTotalUsedBytes;     /**< array allocator의 총 byte */
    stlInt64             mElementSize;        /**< Element 하나의 크기 */
    stlInt64             mBodySize;           /**< 사용 가능한 Body 크기 */
    stlInt64             mTotalElementCount;  /**< Total Element 개수 */
    stlInt64             mUsedElementCount;   /**< Used Element 개수 */
    /* 아래 변수가 자주 변경됨으로 cpu cache만큼 padding을 주는게 좋을수 있음 */
    stlArrayElement    * mFirstFreeElement;   /**< 사용중이 아닌 First Element */
    stlSpinLock          mLock;
#ifdef __MEMORY_TEST__
    stlInt32             mTestMem;
#endif
} stlArrayAllocator;

/** @} */


/**
 * @addtogroup stlLogger
 * @{
 */

typedef struct stlLogger
{
    stlChar        mFileName[STL_MAX_FILE_NAME_LENGTH];
    stlChar        mFilePath[STL_MAX_FILE_PATH_LENGTH];
    stlSemaphore   mSemaphore;
    stlChar      * mLogMsg;
    stlBool        mNeedLock;
    stlInt32       mMaxFileSize;
    stlInt64       mLastFileNo;
} stlLogger;

/** @} */

/**
 * @addtogroup stlStaticHash
 * @{
 */

/**
 * @brief Static Hash 동시성 제어 방법
 */
#define STL_STATICHASH_MODE_NOLOCK      0 /**< 동시성 제어 안함 */
#define STL_STATICHASH_MODE_HASHLOCK    1 /**< Hash 전체를 하나의 latch로 동시성 제어함 */
#define STL_STATICHASH_MODE_BUCKETLOCK  2 /**< 각 Bucket 별로 latch를 할당받아 동시성 제어함 */

/**
 * @brief Static Hash 관리를 위한 Header
 * 바로 뒤에 mBucketCount 만큼의 stlStaticHashBucket의 배열이 있다
 */
typedef struct stlStaticHash
{
    stlUInt16        mBucketCount;
    stlChar          mLockMode;
    stlChar          mPad1;
    stlUInt16        mLinkOffset;
    stlUInt16        mKeyOffset;
    stlSpinLock    * mHashLock;
#if (STL_SIZEOF_VOIDP == 4 )
    void           * mAlign;
#endif
} stlStaticHash;

typedef struct stlStaticHashBucket
{
    stlRingHead   mList;
    stlSpinLock * mLock;
#if (STL_SIZEOF_VOIDP == 4 )
    void        * mAlign;
#endif
} stlStaticHashBucket;

typedef stlUInt32 (*stlStaticHashHashingFunc)(void * aKey, stlUInt32 aBucketCount);
typedef stlInt32  (*stlStaticHashCompareFunc)(void * aKeyA, void * aKeyB);

#define STL_STATICHASH_GET_NTH_BUCKET(hashPtr,Seq)                      \
    ((stlStaticHashBucket*)(((stlStaticHash*)hashPtr) + 1) + (Seq))

#define STL_DBCHASH_BUCKET_COUNT          (127)

/** @} */

/**
 * @addtogroup UDS name
 * @{
 */

/**
 * @brief 내부 process간 통신(UDS) name 정의
 *
 * @remark
 *  listener -> STL_BALANCER_FD_NAME -> balancer -> STL_DISPATCHER_FD_NAME -> gdispatcher
 *  listener <- STL_LISTENER_NAME    <- balancer <- STL_BALANCER_NAME      <- gdispatcher
 */
#define STL_LISTENER_NAME           "glsnr"             /**< listener에서 command및 fd전달에 대한 ack받는 UDS */
#define STL_BALANCER_FD_NAME        "gldblnr.fd"        /**< balancer에서 fd를 받을 UDS */
#define STL_BALANCER_NAME           "gldblnr"           /**< balancer에서 fd전달에 대한 ack받는 UDS */
#define STL_DISPATCHER_FD_NAME      "gdsptr.fd"         /**< dispatcher에서 fd를 받을 UDS */

/* fd전송시 fd에 대한 고유한 id size */
#define STL_FD_ID_SIZE              (16)


/** @} */

/** @} */

STL_END_DECLS

#endif /* _STLDEF_H_ */
