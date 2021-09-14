/*******************************************************************************
 * knpPropertyFile.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id$
 *
 * NOTES
 *        Property 관련 함수 원형
 *
 ******************************************************************************/

#ifndef _KNP_PROPERTY_FILE_H_
#define _KNP_PROPERTY_FILE_H_ 1

/**
 * @file knpPropertyManager.h
 * @brief Kernel Layer 의 Property 을 위한 내부 처리 루틴
 */

/**
 * @defgroup knpProperty Property 내부 처리 루틴
 * @ingroup KNP
 * @internal
 * @{
 */

                                       
/* File */
stlStatus knpProcessTextPropertyFile( stlChar  * aFileName, 
                                      knlEnv   * aKnlEnv ); 
                                         
stlStatus knpProcessBinaryPropertyFile( stlChar  * aFileName, 
                                        knlEnv   * aKnlEnv ); 

stlStatus knpCreateBinaryPropertyWithBlank( stlChar  * aFileName,
                                            knlEnv   * aKnlEnv );

stlStatus knpWritePropertyAtBinaryFile ( stlChar             * aFileName,
                                         knpPropertyContent  * aContent,
                                         void                * aValue,
                                         knlEnv              * aKnlEnv );

stlStatus knpRemovePropertyAtBinaryFile ( stlChar             * aFileName,
                                          knpPropertyContent  * aContent,
                                          knlEnv              * aKnlEnv );

stlStatus knpGetFileExist( stlChar  * aFileName,
                           stlBool  * aIsExist,
                           knlEnv   * aKnlEnv );
                           
stlStatus knpConvertPropertyTextToBinaryFile( stlChar  * aTFileName,  
                                              stlChar  * aBFileName,  
                                              knlEnv   * aKnlEnv );
                                              
stlStatus knpConvertPropertyBinaryToTextFile( stlChar  * abFileName,
                                              stlChar  * atFileName,  
                                              knlEnv   * aKnlEnv );
                                              
stlStatus knpConvertPropertyTextToBinaryFileByDefault( knlEnv  * aKnlEnv );

stlStatus knpConvertPropertyBinaryToTextFileByDefault( knlEnv  * aKnlEnv );
                                              
stlStatus knpDumpBinaryPropertyFile( stlChar  * aFileName,
                                     knlEnv   * aKnlEnv );                                              

/* for TEST */
        

#endif /* _KNP_PROPERTY_FILE_H_ */

/** @} */
