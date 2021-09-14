/*******************************************************************************
 * dtfNot.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *
 ******************************************************************************/

/**
 * @file dtfNot.c
 * @brief dtfNot Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>

/**
 * @addtogroup dtfNot NOT
 * @ingroup dtf
 * @remark
 * <BR>  NOT(dtfNot) 함수로 들어오는 input argument로는 TRUE와 FALSE만 들어오고,
 * <BR>  UNKNOWN(null)인 경우는 상위 단계에서 처리된다. 
 * @internal
 * @{
 */

/**
 * @brief boolean value의 NOT 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfNot( stlUInt16        aInputArgumentCnt,
                  dtlValueEntry  * aInputArgument,
                  void           * aResultValue,
                  void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 1, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_BOOLEAN, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_BOOLEAN, (stlErrorStack *)aEnv );

    /* NOT 연산을 한다. */
    DTF_BOOLEAN( sResultValue ) = ( DTF_BOOLEAN( sValue1 ) == STL_FALSE );
    DTL_SET_BOOLEAN_LENGTH( sResultValue );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}



/** @} */
