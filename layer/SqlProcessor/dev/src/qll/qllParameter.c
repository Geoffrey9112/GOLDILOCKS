/*******************************************************************************
 * qllParameter.c
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
 * @file qllParameter.c
 * @brief Paramter Basic Information for ODBC, JDBC
 */

#include <qll.h>
#include <qlDef.h>

/**
 * @addtogroup qllParameter
 * @{
 */

stlInt32 qllGetParameterCount( qllStatement * aSQLStmt )
{
    return aSQLStmt->mBindCount;
}


/**
 * @brief Bind Parameter 의 I/O type 을 설정한다.
 * @param[in]  aSQLStmt       SQL Statement
 * @param[in]  aBindIdx       Bind Node Idx(0-base)
 * @param[in]  aBindNodePos   Bind Node Position in SQL
 * @param[in]  aBindIO        Bind I/O type
 */
void qllSetBindIOType( qllStatement * aSQLStmt,
                       stlInt32       aBindIdx,
                       stlInt32       aBindNodePos,
                       knlBindType    aBindIO )
{
    qlvInitPlan * sInitPlan = (qlvInitPlan *) qllGetInitPlan( aSQLStmt );
    
    /**
     * Parameter Validation
     */

    STL_DASSERT( sInitPlan != NULL );
    STL_DASSERT( sInitPlan->mBindCount > aBindIdx );

    sInitPlan->mBindIOType[aBindIdx]  = aBindIO;
    sInitPlan->mBindNodePos[aBindIdx] = aBindNodePos;
}

void qllGetParamInfo( qllStatement * aSQLStmt,
                      stlInt32     * aBindCnt,
                      knlBindType ** aBindIOArray )
{
    qlvInitPlan * sInitPlan = NULL;
    
    /**
     * Parameter Validation
     */

    STL_DASSERT( aSQLStmt != NULL );
    STL_DASSERT( aBindCnt != NULL );
    STL_DASSERT( aBindIOArray != NULL );

    /**
     * Output 설정
     */

    if ( aSQLStmt->mBindCount > 0 )
    {
        *aBindCnt     = aSQLStmt->mBindCount;
        sInitPlan = (qlvInitPlan *) qllGetInitPlan( aSQLStmt );

        STL_DASSERT( sInitPlan->mBindCount == aSQLStmt->mBindCount );
        STL_DASSERT( sInitPlan->mBindIOType != NULL );
        
        *aBindIOArray = sInitPlan->mBindIOType;
    }
    else
    {
        *aBindCnt     = aSQLStmt->mBindCount;
        *aBindIOArray = NULL;
    }
}


/** @} qllParameter */
