/*******************************************************************************
 * qllParameter.h
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

#ifndef _QLL_PARAMETER_H_
#define _QLL_PARAMETER_H_ 1

/**
 * @file qllParameter.h
 * @brief Bind Parameter Information
 */

stlInt32  qllGetParameterCount( qllStatement * aSQLStmt );

void qllSetBindIOType( qllStatement * aSQLStmt,
                       stlInt32       aBindIdx,
                       stlInt32       aBindNodePos,
                       knlBindType    aBindIO );

void qllGetParamInfo( qllStatement * aSQLStmt,
                      stlInt32     * aBindCnt,
                      knlBindType ** aBindIOArray );

#endif /* _QLL_TARGET_H_ */



