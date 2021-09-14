/*******************************************************************************
 * dtfAnd.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: dtfAnd.c 1389 2011-07-12 02:23:18Z ehpark $
 *
 * NOTES
 *
 ******************************************************************************/

/**
 * @file dtfAnd.c
 * @brief dtfAnd Function DataType Library Layer
 */

#include <dtl.h>
#include <dtlDef.h>
#include <dtDef.h>

/**
 * @addtogroup dtfAnd And
 * @ingroup dtf
 * @remark
 * <BR>  AND의 boolean 연산은 아래와 같다.
 * <BR>  그러나, AND(dtfAnd) 함수로 들어오는 input argument로는 TRUE와 FALSE만 들어오고,
 * <BR>  UNKNOWN(null)인 경우는 상위 단계에서 처리된다. 
 * <BR>
 * <table>
 *   <tr> <td> AND     </td> <td> TRUE    </td> <td> FALSE </td> <td> UNKNOWN </td> </tr>
 *   <tr> <td> TRUE    </td> <td> TRUE    </td> <td> FALSE </td> <td> UNKNOWN </td> </tr>
 *   <tr> <td> FALSE   </td> <td> FALSE   </td> <td> FALSE </td> <td> FALSE   </td> </tr>
 *   <tr> <td> UNKNOWN </td> <td> UNKNOWN </td> <td> FALSE </td> <td> UNKNOWN </td> </tr>
 * </table>  
 * 
 * @internal
 * @{
 */

/**
 * @brief boolean value의 AND 연산
 * @param[in]  aInputArgumentCnt input argument count
 * @param[in]  aInputArgument    input argument
 * @param[out] aResultValue      연산 결과 (결과타입 BOOLEAN)
 * @param[out] aEnv              environment (stlErrorStack)
 */
stlStatus dtfAnd( stlUInt16        aInputArgumentCnt,
                  dtlValueEntry  * aInputArgument,
                  void           * aResultValue,
                  void           * aEnv )
{
    stlInt32        i;
    dtlDataValue  * sValue;
    dtlDataValue  * sResultValue;

    DTL_PARAM_VALIDATE( aInputArgumentCnt >= 2, (stlErrorStack *)aEnv );

    sResultValue = (dtlDataValue *)aResultValue;

    /* Result Value를 TRUE로 초기화 */
    DTF_BOOLEAN( sResultValue ) = STL_TRUE;
    DTL_SET_BOOLEAN_LENGTH( sResultValue );

    for( i = 0; i < aInputArgumentCnt; i++ )
    {
        sValue = aInputArgument[i].mValue.mDataValue;
        STL_DASSERT( sValue->mType == DTL_TYPE_BOOLEAN );

        if( DTL_IS_NULL( sValue ) == STL_TRUE )
        {
            DTL_SET_NULL( sResultValue );
            break;
        }

        /* Input Argument가 FALSE가 올 수 없음 */
        STL_DASSERT( DTF_BOOLEAN( sValue ) != STL_FALSE );
    }


    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/** @} */
