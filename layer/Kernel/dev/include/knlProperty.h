/*******************************************************************************
 * knlProperty.h
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


#ifndef _KNLPROPERTY_H_
#define _KNLPROPERTY_H_ 1

/**
 * @file knlProperty.h
 * @brief Kernel Layer Property Routines
 */

/**
 * @defgroup knlProperty Property
 * @ingroup knExternal
 * @{
 */

stlStatus knlInitializePropertyInHeap ( knlEnv *aKnlEnv );
stlStatus knlFinalizePropertyInHeap( knlEnv * aKnlEnv );
stlStatus knlInitializePropertyInShm ( knlEnv *aKnlEnv );
stlStatus knlInitializePropertyAtWarmup ( knlEnv * aKnlEnv );


stlStatus knlGetPropertyIDByName( stlChar        * aPropertyName,
                                  knlPropertyID  * aPropertyID, 
                                  knlEnv         * aKnlEnv );

stlStatus knlGetPropertyNameByID ( knlPropertyID    aPropertyID,
                                   stlChar        * aPropertyName,
                                   knlEnv         * aKnlEnv );

stlStatus knlExistPropertyByName ( stlChar        * aPropertyName,
                                   stlBool        * aIsExist,
                                   knlEnv         * aKnlEnv );

stlStatus knlCopyPropertyValueToSessionEnv ( knlSessionEnv * aSessionEnv,
                                             knlEnv        * aKnlEnv );


/* Modifiable Function */
stlStatus knlUpdatePropertyValueByName( stlChar                * aPropertyName,
                                        void                   * aValue, 
                                        knlPropertyModifyMode    aMode,
                                        stlBool                  aIsDeferred,
                                        knlPropertyModifyScope   aScope,
                                        knlEnv                 * aKnlEnv );
                                              
stlStatus knlUpdatePropertyValueByID( knlPropertyID            aPropertyID,
                                      void                   * aValue, 
                                      knlPropertyModifyMode    aMode,
                                      stlBool                  aIsDeferred,
                                      knlPropertyModifyScope   aScope,
                                      knlEnv                 * aKnlEnv );
                                   
stlStatus knlInitPropertyValueByName( stlChar                * aPropertyName,
                                      knlPropertyModifyMode    aMode,
                                      stlBool                  aIsDeferred,
                                      knlPropertyModifyScope   aScope,
                                      knlEnv                 * aKnlEnv );
                                                                       
stlStatus knlInitPropertyValueByID( knlPropertyID            aPropertyID,
                                    knlPropertyModifyMode    aMode,
                                    stlBool                  aIsDeferred,
                                    knlPropertyModifyScope   aScope,
                                    knlEnv                 * aKnlEnv );    

stlStatus knlRemovePropertyValueByName( stlChar * aPropertyName,
                                        knlEnv  * aKnlEnv );
                                                                       
stlStatus knlRemovePropertyValueByID( knlPropertyID   aPropertyID,
                                      knlEnv        * aKnlEnv );    
                                 

                                
/* Get Function by ID */

stlStatus knlGetPropertyValueByID ( knlPropertyID    aPropertyID,
                                    void           * aValue,
                                    knlEnv         * aKnlEnv );

stlBool knlGetPropertyBoolValueByID ( knlPropertyID    aPropertyID,
                                      knlEnv         * aKnlEnv );

stlInt64 knlGetPropertyBigIntValueByID ( knlPropertyID    aPropertyID,
                                         knlEnv         * aKnlEnv );

stlChar * knlGetPropertyStringValueByID ( knlPropertyID    aPropertyID,
                                          knlEnv         * aKnlEnv );

stlBool knlGetPropertyBoolValueAtShmByID( knlPropertyID aPropertyID );

stlInt64 knlGetPropertyBigIntValueAtShmByID ( knlPropertyID aPropertyID );

stlBool knlGetPropertyBoolValue4SessEnvById ( knlPropertyID   aPropertyID,
                                              knlSessionEnv * aSessionEnv );

stlInt64 knlGetPropertyBigIntValue4SessEnvById ( knlPropertyID   aPropertyID,
                                                 knlSessionEnv * aSessionEnv );

stlStatus knlGetPropertyValueMaxLenByID ( knlPropertyID    aPropertyID,
                                          stlUInt32      * aValue, 
                                          knlEnv         * aKnlEnv );

stlStatus knlGetPropertySysModTypeByID ( knlPropertyID            aPropertyID,
                                         knlPropertyModifyType  * aType,
                                         knlEnv                 * aKnlEnv );
                                    
stlStatus knlGetPropertySesModTypeByID ( knlPropertyID    aPropertyID,
                                         stlBool        * aType,
                                         knlEnv         * aKnlEnv );
                                    
stlStatus knlGetPropertyModifiableByID ( knlPropertyID    aPropertyID,
                                         stlBool        * aType,
                                         knlEnv         * aKnlEnv );
                                         
stlStatus knlGetPropertyDataTypeByID ( knlPropertyID    aPropertyID,
                                       knlPropertyDataType * aType,
                                       knlEnv              * aKnlEnv );
                                       
stlStatus knlConvertUnitToValue( knlPropertyID         aPropertyID,
                                 stlChar             * aInStr,
                                 stlInt64            * aValue,
                                 knlEnv              * aKnlEnv );

/* Get Function by Name */
                                
stlStatus knlGetPropertyValueByName ( stlChar    * aPropertyName,
                                      void       * aValue,
                                      knlEnv     * aKnlEnv );
                                
stlStatus knlGetPropertyValueMaxLenByName ( stlChar    * aPropertyName,
                                            stlUInt32  * aValue, 
                                            knlEnv     * aKnlEnv );
                                   
stlStatus knlGetPropertySysModTypeByName ( stlChar                * aPropertyName,
                                           knlPropertyModifyType  * aType,
                                           knlEnv                 * aKnlEnv );
                                    
stlStatus knlGetPropertySesModTypeByName ( stlChar  * aPropertyName,
                                           stlBool  * aType,
                                           knlEnv   * aKnlEnv );
                                    
stlStatus knlGetPropertyModifiableByName ( stlChar  * aPropertyName,
                                           stlBool  * aType,
                                           knlEnv   * aKnlEnv );

stlStatus knlGetPropertyDataTypeByName ( stlChar             * aPropertyName,
                                         knlPropertyDataType * aType,
                                         knlEnv              * aKnlEnv );

/* Other Function */
stlStatus knlConvertPropertyTextToBinaryFile( stlChar  * aTFileName,  
                                              stlChar  * aBFileName,  
                                              knlEnv   * aKnlEnv );
                                           
stlStatus knlConvertPropertyBinaryToTextFile( stlChar  * aTFileName,  
                                              stlChar  * aBFileName,  
                                              knlEnv   * aKnlEnv );

stlStatus knlConvertPropertyBinaryToTextFileByDefault( knlEnv  * aKnlEnv );

stlStatus knlConvertPropertyTextToBinaryFileByDefault( knlEnv  * aKnlEnv );

stlStatus knlDumpBinaryPropertyFile( stlChar * aFileName,
                                     knlEnv  * aKnlEnv );

/* for TEST */
stlStatus knlTestInitializePropertyFile( knlEnv  * aKnlEnv );
                                           
/** @} */
    
#endif /* _KNLPROPERTY_H_ */
