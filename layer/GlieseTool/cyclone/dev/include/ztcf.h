/*******************************************************************************
 * ztcf.h
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id $
 *
 * NOTES
 *    
 *
 ******************************************************************************/


#ifndef _ZTCF_H_
#define _ZTCF_H_ 1

/**
 * @file ztcf.h
 * @brief GlieseTool Cyclone File Routines
 */


/**
 * @defgroup ztcf Cyclone Configure File Routines
 * @{
 */

#ifndef YY_EXTRA_TYPE
#define YY_EXTRA_TYPE ztcConfigureParam *
#endif
                                  
/**
 * For Parser
 */
stlStatus ztcfReadConfigure( stlErrorStack * aErrorStack );

stlStatus ztcfIdentifyGroup( stlErrorStack * aErrorStack );

stlInt32  ztcfConfigurelex_init( void ** );

stlInt32  ztcfConfigurelex_destroy( void * );

stlInt32  ztcfConfigureparse( );

void      ztcfConfigureset_extra( YY_EXTRA_TYPE, void * );

void      ztcfAddGroupItem( ztcConfigureParam * aParam,
                            stlChar           * aGroupName );

void      ztcfAddTableInfo( ztcConfigureParam * aParam, 
                            stlChar           * aSchemaName,
                            stlChar           * aTableName );

void      ztcfAddApplyInfo( ztcConfigureParam * aParam,
                            stlChar           * aMasterSchema,
                            stlChar           * aMasterTable,
                            stlChar           * aSlaveSchema,
                            stlChar           * aSlaveTable );

void      ztcfSetUserIdToGlobalArea( ztcConfigureParam * aParam, 
                                     stlChar           * aUserId );
                        
void      ztcfSetUserIdToGroupArea( ztcConfigureParam * aParam, 
                                    stlChar           * aUserId );

void      ztcfSetUserPwToGlobalArea( ztcConfigureParam * aParam, 
                                     stlChar           * aUserPw );
                        
void      ztcfSetUserPwToGroupArea( ztcConfigureParam * aParam, 
                                    stlChar           * aUserPw );

void      ztcfSetPort( ztcConfigureParam * aParam, 
                       stlChar           * aPort );

void      ztcfSetHostIpToGlobalArea( ztcConfigureParam * aParam, 
                                     stlChar           * aHostIp );

void      ztcfSetHostIpToGroupArea( ztcConfigureParam * aParam, 
                                    stlChar           * aHostIp );
                         
void      ztcfSetHostPortToGlobalArea( ztcConfigureParam * aParam, 
                                       stlChar           * aPort );
                           
void      ztcfSetHostPortToGroupArea( ztcConfigureParam * aParam, 
                                      stlChar           * aPort );

void      ztcfSetDsnToGlobalArea( ztcConfigureParam * aParam, 
                                  stlChar           * aDsn );
                           
void      ztcfSetDsnToGroupArea( ztcConfigureParam * aParam, 
                                 stlChar           * aDsn );

void      ztcfSetMasterIp( ztcConfigureParam * aParam, 
                           stlChar           * aMasterIp );
                           
void      ztcfSetTransSortAreaSize( ztcConfigureParam * aParam, 
                                    stlChar           * aSize );

void      ztcfSetApplierCount( ztcConfigureParam * aParam, 
                               stlChar           * aSize );

void      ztcfSetApplyCommitSize( ztcConfigureParam * aParam, 
                                  stlChar           * aSize );

void      ztcfSetApplyArraySize( ztcConfigureParam * aParam, 
                                 stlChar           * aSize );

void      ztcfSetReadLogBlockCount( ztcConfigureParam * aParam, 
                                    stlChar           * aCount );

void      ztcfSetCommChunkCount( ztcConfigureParam * aParam, 
                                 stlChar           * aCount );

void      ztcfSetPropagateMode( ztcConfigureParam * aParam, 
                                stlChar           * aMode );

void      ztcfSetUserLogFilePath( ztcConfigureParam * aParam, 
                                  stlChar           * aLogPath );

void      ztcfSetTransFilePath( ztcConfigureParam * aParam, 
                                stlChar           * aPath );

void      ztcfSetGiveupInterval( ztcConfigureParam * aParam, 
                                 stlChar           * aInterval );

void      ztcfSetGoldilocksListenPort( ztcConfigureParam * aParam, 
                                  stlChar           * aPort );

void      ztcfSetSyncherCount( ztcConfigureParam * aParam, 
                               stlChar           * aCount );

void      ztcfSetSyncArraySize( ztcConfigureParam * aParam, 
                                stlChar           * aSize );

void      ztcfSetUpdateApplyMode( ztcConfigureParam * aParam, 
                                  stlChar           * aMode );

/** @} */

#endif /* _ZTCF_H_ */
