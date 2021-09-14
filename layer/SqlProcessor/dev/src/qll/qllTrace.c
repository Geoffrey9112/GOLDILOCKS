/*******************************************************************************
 * qllTrace.c
 *
 * Copyright (c) 2013, SUNJESOFT Inc.
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
 * @file qllTrace.c
 * @brief Query Process의 Trace Log
 */

#include <qll.h>
#include <qlDef.h>


/**
 * @addtogroup qllTrace
 * @{
 */


/**
 * @brief Trace Logger를 생성한다.
 * @param[in]   aEnv        Environment
 */
stlStatus qllTraceCreateLogger( qllEnv  * aEnv )
{
    STL_TRY( knlCreateProcessLogger( QLL_GET_TRACE_LOGGER(aEnv),
                                     QLL_TRACE_LOG_NAME_PREFIX,
                                     KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLL_SET_INIT_TRACE_LOGGER(aEnv);


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Trace Logger를 해제한다.
 * @param[in]   aEnv        Environment
 */
stlStatus qllTraceDestroyLogger( qllEnv  * aEnv )
{
    STL_TRY( knlDestroyProcessLogger( QLL_GET_TRACE_LOGGER(aEnv),
                                      KNL_ENV(aEnv) )
             == STL_SUCCESS );

    QLL_SET_FINI_TRACE_LOGGER(aEnv);


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief Trace Log의 Head 내용을 출력한다.
 * @param[in]   aTitle      Title
 * @param[in]   aEnv        Environment
 */
void qllTraceHead( stlChar  * aTitle,
                   qllEnv   * aEnv )
{
    knlLogMsgWithoutTimestamp(
        QLL_GET_TRACE_LOGGER(aEnv),
        KNL_ENV(aEnv),
        "============================== BEGIN OF %s  ==============================\n",
        aTitle );
}


/**
 * @brief Trace Log의 Tail 내용을 출력한다.
 * @param[in]   aTitle      Title
 * @param[in]   aEnv        Environment
 */
void qllTraceTail( stlChar  * aTitle,
                   qllEnv   * aEnv )
{
    knlLogMsgWithoutTimestamp(
        QLL_GET_TRACE_LOGGER(aEnv),
        KNL_ENV(aEnv),
        "============================== END OF %s  ==============================\n",
        aTitle );
}


/**
 * @brief Trace Log의 중간 Title을 출력한다.
 * @param[in]   aTitle      Title
 * @param[in]   aEnv        Environment
 */
void qllTraceMidTitle( stlChar  * aTitle,
                       qllEnv   * aEnv )
{
    knlLogMsgWithoutTimestamp(
        QLL_GET_TRACE_LOGGER(aEnv),
        KNL_ENV(aEnv),
        "\n"
        "*****************************************\n"
        "%s\n"
        "*****************************************\n",
        aTitle );
}


/**
 * @brief Trace Log의 Body 내용을 출력하고 줄바꿈을 한다.
 * @param[in]   aString     String
 * @param[in]   aEnv        Environment
 */
void qllTraceString( stlChar    * aString,
                     qllEnv     * aEnv )
{
    knlSimpleLogMsgWithoutTimestamp(
        QLL_GET_TRACE_LOGGER(aEnv),
        aString,
        KNL_ENV(aEnv) );
}


/**
 * @brief Trace Log의 Body 내용을 출력한다.
 * @param[in]   aRegionMem  Region Memory
 * @param[in]   aEnv        Environment
 * @param[in]   aFormat     Format
 * @param[in]   ...         Output Parameter
 */
stlStatus qllTraceBody( knlRegionMem    * aRegionMem,
                        qllEnv          * aEnv,
                        const stlChar   * aFormat,
                        ... )
{
    stlInt32      sStrLen;
    stlVarList    sVarArgList;
    stlChar     * sLogMsg       = NULL;

    /****************************************************************
     * Parameter Validation
     ****************************************************************/

    STL_PARAM_VALIDATE( aRegionMem != NULL, QLL_ERROR_STACK(aEnv) );


    /* String Size 설정 */
    stlVarStart( sVarArgList, aFormat );
    sStrLen = stlGetVprintfBufferSize( aFormat, sVarArgList );
    stlVarEnd( sVarArgList );

    /* Log Message 할당 */
    STL_TRY( knlAllocRegionMem( aRegionMem,
                                STL_SIZEOF( stlChar ) * (sStrLen + 1),
                                (void**) &sLogMsg,
                                KNL_ENV(aEnv) )
             == STL_SUCCESS );

    /* Log Message 복사 */
    stlVarStart( sVarArgList, aFormat );
    stlVsnprintf( sLogMsg,
                  sStrLen + 1,
                  aFormat,
                  sVarArgList );
    stlVarEnd( sVarArgList );

    /* Log Message 출력 */
    knlSimpleLogMsgWithoutTimestamp(
        QLL_GET_TRACE_LOGGER(aEnv),
        sLogMsg,
        KNL_ENV(aEnv) );


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} qllTrace */
