/*******************************************************************************
 * knpPropertyManager.h
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

#ifndef _KNP_PROPERTY_H_
#define _KNP_PROPERTY_H_ 1

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


stlStatus knpInitializePropertyInHeap( knlEnv  * aKnlEnv );
 
stlStatus knpInitializePropertyInShm( knlEnv  * aKnlEnv ); 

stlStatus knpInitializePropertyAtWarmup ( knlEnv * aKnlEnv );

stlStatus knpFinalizePropertyInHeap( knlEnv  * aKnlEnv );              

stlStatus knpInitializeEnvironment( knlEnv  * aKnlEnv );


/* Property Build Function */
stlStatus knpBuildPropertyWithDefault( knlEnv * aEnv );

stlStatus knpSetPropertyFromEnv( knlEnv  * aKnlEnv );

stlStatus knpSetPropertyFromFile( stlChar  * aFileName, 
                                  stlBool    aIsBinary,
                                  knlEnv   * aKnlEnv );


/* Property Environment Function */

stlUInt32 knpHashFunction( void      * aKey, 
                           stlUInt32   aBucketCount );
                           
stlInt32  knpHashCompareFunction( void * aKeyA, 
                                  void * aKeyB );
                                  

/* Property Management Function */
stlStatus knpRegistProperty( knpPropertyDesc  * aDesc,
                             knlEnv           * aKnlEnv );

stlStatus knpExistPropertyByName( stlChar   * aPropertyName,
                                  stlBool   * aIsExist,
                                  knlEnv    * aKnlEnv );

stlStatus knpAddPropertyValue( knpPropertyContent     * aContent, 
                               void                   * aValue,
                               knlPropertyModifyMode    aMode,
                               stlBool                  aIsDeferred,
                               knlEnv                 * aKnlEnv );

stlStatus knpAddPropertyFileValue( knpPropertyContent     * aContent, 
                                   void                   * aValue,
                                   knlPropertyModifyMode    aMode,
                                   knlEnv                 * aKnlEnv );

stlStatus knpAddPropertyValueAtFile( knpPropertyContent  * aContent,
                                     void                * aValue,
                                     knlEnv              * aKnlEnv );

stlStatus knpRemovePropertyValueAtFile( knpPropertyContent  * aContent,
                                        knlEnv              * aKnlEnv );


stlStatus knpAddPropertyInitValue( knpPropertyContent  * aContent, 
                                   void                * aValue,
                                   knlEnv              * aKnlEnv );

stlStatus knpVerifyPropertyValue( knpPropertyContent     * aContent, 
                                  void                   * aValue,
                                  knlEnv                 * aKnlEnv );
                                       
stlStatus knpVerifyPropertyAttribute( knpPropertyContent     * aContent, 
                                      knlPropertyModifyMode    aMode,
                                      stlBool                  aIsDeferred,
                                      knlPropertyModifyScope   aScope,
                                      knlEnv                 * aKnlEnv );

stlChar * knpGetHomeDir();

stlStatus knpConvertFromString ( knpPropertyContent  * aContent, 
                                 void                * aValue,
                                 void               ** aResult,
                                 knlEnv              * aKnlEnv );
                                 
stlStatus knpUpdateSessionEnv( knpPropertyContent     * aContent,
                               knlPropertyModifyMode    aMode,
                               stlBool                  aIsDeferred,
                               knlEnv                 * aKnlEnv );
                        
stlStatus knpInitPropertyValue( knpPropertyContent     * aContent,
                                knlPropertyModifyMode    aMode,
                                stlBool                  aIsDeferred,
                                knlPropertyModifyScope   aScope,
                                knlEnv                 * aKnlEnv );


stlStatus knpUpdatePropertyValue( knpPropertyContent      * aContent,
                                  void                    * aValue,
                                  knlPropertyModifyMode     aMode,
                                  stlBool                   aIsDeferred,
                                  knlPropertyModifyScope    aScope,
                                  knlEnv                  * aKnlEnv );

stlStatus knpUpdatePropertyValueUseStr( knpPropertyContent     * aContent,
                                        stlChar                * aValue,
                                        knlPropertyModifyMode    aMode,
                                        stlBool                  aIsDeferred,
                                        knlPropertyModifyScope   aScope,
                                        knlEnv                 * aKnlEnv );

stlStatus knpSetPropertyInitValue( knlEnv  * aKnlEnv );

                                 
stlStatus knpGetPropertyDomain ( knpPropertyContent  * aContent,
                                 knlPropertyDomain   * aType,
                                 knlEnv              * aKnlEnv );

stlStatus knpGetPropertyDataType ( knpPropertyContent  * aContent,
                                   knlPropertyDataType * aType,
                                   knlEnv              * aKnlEnv );
                                   
stlStatus knpGetPropertyBigIntUnit( knpPropertyContent     * aContent,
                                    knlPropertyBigintUnit  * aType,
                                    knlEnv                 * aKnlEnv );
                                    
stlStatus knpConvertUnitToValue( knpPropertyContent  * aContent,
                                 stlChar             * aInStr,
                                 stlInt64            * aValue,
                                 knlEnv              * aKnlEnv );
                                   
stlStatus knpCopyPropertyValueToSessionEnv ( knlSessionEnv * aSessionEnv,
                                             knlEnv        * aKnlEnv );


stlStatus knpGetPropertyHandleByName( stlChar    * aPropertyName,
                                      void      ** aPropertyHandle,
                                      knlEnv     * aKnlEnv );

stlStatus knpGetPropertyHandleByID( knlPropertyID      aPropertyID, 
                                    void            ** aPropertyHandle, 
                                    knlEnv           * aKnlEnv );

stlStatus knpGetPropertyIDByName( stlChar        * aPropertyName,
                                  knlPropertyID  * aPropertyID, 
                                  knlEnv         * aKnlEnv );
                                  
stlStatus knpGetPropertyNameByID ( knlPropertyID    aPropertyID,
                                   stlChar        * aPropertyName,
                                   knlEnv         * aKnlEnv );

stlStatus knpGetPropertyValue ( knpPropertyContent   * aContent,
                                void                 * aValue,
                                knlEnv               * aKnlEnv );
                                
stlBool knpGetPropertyBoolValue ( knlPropertyID    aPropertyID,
                                  knlEnv         * aKnlEnv );
                                  
stlInt64 knpGetPropertyBigIntValue ( knlPropertyID    aPropertyID,
                                     knlEnv         * aKnlEnv );

stlChar * knpGetPropertyStringValue ( knlPropertyID    aPropertyID,
                                      knlEnv         * aKnlEnv );

stlBool knpGetPropertyBoolValueAtShm( knlPropertyID aPropertyID );

stlInt64 knpGetPropertyBigIntValueAtShm( knlPropertyID aPropertyID );

stlBool knpGetPropertyBoolValue4SessEnv( knlPropertyID   aPropertyID,
                                         knlSessionEnv * aSessionEnv );

stlInt64 knpGetPropertyBigIntValue4SessEnv( knlPropertyID   aPropertyID,
                                            knlSessionEnv * aSessionEnv );

stlStatus knpGetPropertyInitValue ( knpPropertyContent  * aContent,
                                    void                * aValue,
                                    knlEnv              * aKnlEnv );

stlStatus knpGetPropertyFileValue ( knpPropertyContent  * aContent,
                                    void                * aValue,
                                    knlEnv              * aKnlEnv );
        
stlStatus knpGetPropertyValueLen( knpPropertyContent  * aContent,
                                  stlUInt32           * aValue, 
                                  knlEnv              * aKnlEnv );
                               
stlStatus knpGetPropertyValueMaxLen( knpPropertyContent  * aContent,
                                     stlUInt32           * aValue, 
                                     knlEnv              * aKnlEnv );
                               
stlStatus knpGetPropertySysModType( knpPropertyContent     * aContent,
                                    knlPropertyModifyType  * aType,
                                    knlEnv                 * aKnlEnv );
                                
stlStatus knpGetPropertySesModType( knpPropertyContent  * aContent,
                                    stlBool             * aType,
                                    knlEnv              * aKnlEnv );
                                
stlStatus knpGetPropertyModifiable ( knpPropertyContent  * aContent,
                                     stlBool             * aType,
                                     knlEnv              * aKnlEnv );
                                     
stlStatus knpGetPropertyCheckRange ( knpPropertyContent  * aContent,
                                     stlBool             * aType,
                                     knlEnv              * aKnlEnv );

stlStatus knpGetPropertyCheckIncrease ( knpPropertyContent  * aContent,
                                        stlBool             * aType,
                                        knlEnv              * aKnlEnv );
                                     
stlStatus knpParseVarcharPath ( stlChar   * aValue,
                                stlChar   * aRetValue,
                                knlEnv    * aKnlEnv );
                                     
/* for Fixed Table */
stlStatus openFxPropertyCallback( void   * aStmt,
                                  void   * aDumpObjHandle,
                                  void   * aPathCtrl,
                                  knlEnv * aEnv );
                                  
stlStatus closeFxPropertyCallback( void   * aDumpObjHandle,
                                   void   * aPathCtrl,
                                   knlEnv * aEnv );
                                   
stlStatus buildRecordFxPropertyCallback( void              * aDumpObjHandle,
                                         void              * aPathCtrl,
                                         knlValueBlockList * aValueList,
                                         stlInt32            aBlockIdx,
                                         stlBool           * aTupleExist,
                                         knlEnv            * aKnlEnv );

stlStatus buildRecordFxSPropertyCallback( void              * aDumpObjHandle,
                                          void              * aPathCtrl,
                                          knlValueBlockList * aValueList,
                                          stlInt32            aBlockIdx,
                                          stlBool           * aTupleExist,
                                          knlEnv            * aKnlEnv );

/** @} */

#endif /* _KNP_PROPERTY_H_ */
