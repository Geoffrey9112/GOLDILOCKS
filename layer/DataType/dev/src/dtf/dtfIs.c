/*******************************************************************************
 * dtfIs.c
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
 * @file dtfIs.c
 * @brief dtfIs Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>

/**
 * @addtogroup dtfIs IS
 * @ingroup dtf
 * @remark
 * <BR>  IS의 boolean 연산은 아래와 같다.
 * <BR>  그러나, IS(dtfIs) 함수로 들어오는 input argument로는 TRUE와 FALSE만 들어오고,
 * <BR>  UNKNOWN(null)인 경우는 상위 단계에서 처리된다. 
 * <BR>
 * <table>
 *   <tr> <td> IS      </td> <td> TRUE  </td> <td> FALSE </td> <td> UNKNOWN </td> </tr>
 *   <tr> <td> TRUE    </td> <td> TRUE  </td> <td> FALSE </td> <td> FALSE   </td> </tr>
 *   <tr> <td> FALSE   </td> <td> FALSE </td> <td> TRUE  </td> <td> FALSE   </td> </tr>
 *   <tr> <td> UNKNOWN </td> <td> FALSE </td> <td> FALSE </td> <td> TRUE    </td> </tr>
 * </table>  
 * 
 * @internal
 * @{
 */

/**
 * @brief boolean value의 IS 연산
 *
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfIs( stlUInt16        aInputArgumentCnt,
                 dtlValueEntry  * aInputArgument,
                 void           * aResultValue,
                 void           * aEnv )
{
    dtlDataValue  * sValue1;
    dtlDataValue  * sValue2;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt == 2, (stlErrorStack *)aEnv );

    sValue1 = aInputArgument[0].mValue.mDataValue;
    sValue2 = aInputArgument[1].mValue.mDataValue;
    sResultValue = (dtlDataValue *)aResultValue;

    DTF_CHECK_DTL_VALUE( sValue1, DTL_TYPE_BOOLEAN, (stlErrorStack *)aEnv );
    DTF_CHECK_DTL_VALUE( sValue2, DTL_TYPE_BOOLEAN, (stlErrorStack *)aEnv );

    /* sResultValue는 length 체크를 하지 않는다. */
    DTF_CHECK_DTL_RESULT( sResultValue, DTL_TYPE_BOOLEAN, (stlErrorStack *)aEnv );

    /* IS 연산을 한다. */
    DTF_BOOLEAN( sResultValue ) = ( DTF_BOOLEAN( sValue1 ) ==  DTF_BOOLEAN( sValue2 ) );
    DTL_SET_BOOLEAN_LENGTH( sResultValue );
    
    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
