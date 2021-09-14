/*******************************************************************************
 * dtlGeneral.c
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
 * @file dtlGeneral.c
 * @brief DataType Layer General Routines
 */

#include <dtl.h>

extern stlErrorRecord   gDataTypeErrorTable[DTL_MAX_ERROR + 1];

/**
 * @addtogroup dtlGeneral
 * @{
 */



/**
 * @brief DataType Layer를 초기화한다.
 */
stlStatus dtlInitialize()
{
    stlErrorStack   sErrorStack;

    STL_INIT_ERROR_STACK( &sErrorStack );    
    
    if( stlGetProcessInitialized( STL_LAYER_DATATYPE ) == STL_FALSE )
    {
        /**
         * 하위 Layer 초기화
         */

        STL_TRY( stlInitialize() == STL_SUCCESS );

        /**
         * DataType Layer 에러 테이블을 등록한다.
         */
    
        stlRegisterErrorTable( STL_ERROR_MODULE_DATATYPE,
                               gDataTypeErrorTable );

        STL_TRY( dtlInitializeForInternal( &sErrorStack ) == STL_SUCCESS );

        stlSetProcessInitialized( STL_LAYER_DATATYPE );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief DataType Layer를 종료한다.
 */
stlStatus dtlTerminate()
{
    /* nothing to do */
    return STL_SUCCESS;
}


/**
 * @brief DataType Layer 내부 초기화 과정 수행.
 */
stlStatus dtlInitializeForInternal( stlErrorStack * aErrorStack )
{
    STL_TRY( dtlBuildNLSDateTimeFormatInfoForODBC( aErrorStack ) == STL_SUCCESS );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;    
}

/** @} */
