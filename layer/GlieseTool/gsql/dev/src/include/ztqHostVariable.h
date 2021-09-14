/*******************************************************************************
 * ztqHostVariable.h
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


#ifndef _ZTQHOSTVARIABLE_H_
#define _ZTQHOSTVARIABLE_H_ 1

/**
 * @file ztqHostVariable.h
 * @brief Host Variable Definition
 */

/**
 * @defgroup ztqHostVariable Host Variable
 * @ingroup ztq
 * @{
 */

stlStatus ztqAssignHostVariable( ztqCmdExecAssignTree * aParseTree,
                                 stlChar              * aCommandString,
                                 stlAllocator         * aAllocator,
                                 stlErrorStack        * aErrorStack );

stlStatus ztqDeclareHostVariable( stlChar       * aVariableName,
                                  ztqDataType   * aDataType,
                                  stlErrorStack * aErrorStack );

stlStatus ztqPrintAllHostVariable( stlAllocator  * aAllocator,
                                   stlErrorStack * aErrorStack );

stlStatus ztqPrintHostVariable( stlChar       * aVariableName,
                                stlAllocator  * aAllocator,
                                stlErrorStack * aErrorStack );

stlStatus ztqFindHostVariable( stlChar          * aVariableName,
                               ztqHostVariable ** aHostVariable,
                               stlErrorStack    * aErrorStack );

stlStatus ztqGetHostVariable( stlChar          * aVariableName,
                              ztqHostVariable ** aHostVariable,
                              stlErrorStack    * aErrorStack );

stlStatus ztqCloneHostVarList( ztqHostVariable  * aSrcHostVarList,
                               ztqHostVariable ** aDestHostVarList,
                               stlErrorStack    * aErrorStack );

stlStatus ztqAllocSysHostVariable( stlErrorStack * aErrorStack );

stlStatus ztqDestHostVariables( stlErrorStack * aErrorStack );

stlStatus ztqAssignSysHostVariable( stlChar       * aVariableName,
                                    stlChar       * aValue,
                                    stlErrorStack * aErrorStack );

/** @} */

#endif /* _ZTQHOSTVARIABLE_H_ */
