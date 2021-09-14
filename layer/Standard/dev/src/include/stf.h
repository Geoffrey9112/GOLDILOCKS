/*******************************************************************************
 * stf.h
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


#ifndef _STF_H_
#define _STF_H_ 1

STL_BEGIN_DECLS

#define STF_DEFAULT_BUFSIZE   1024
#define STF_DIRECTIO_BUFSIZE   512


stlStatus stfGetFileInfo( stlFileInfo   * aFileInfo,
                          stlInt32        aWanted,
                          stlFile       * aFile,
                          stlErrorStack * aErrorStack );

stlStatus stfGetStat( stlFileInfo    * aFileInfo,
                      const stlChar  * aFileName,
                      stlInt32         aWanted,
                      stlErrorStack  * aErrorStack);

stlStatus stfGetCurFilePath( stlChar       * aPath,
                             stlInt32        aPathBufLen,
                             stlErrorStack * aErrorStack );

stlStatus stfOpen( stlFile        * aNewFile, 
                   const stlChar  * aFileName, 
                   stlInt32         aFlag, 
                   stlInt32         aPerm,
                   stlErrorStack  * aErrorStack );

stlStatus stfExist( const stlChar  * aFileName,
                    stlBool        * aExist,
                    stlErrorStack  * aErrorStack );

stlStatus stfClose( stlFile       * aFile,
                    stlErrorStack * aErrorStack );

stlStatus stfRemove( const stlChar * aPath,
                     stlErrorStack * aErrorStack );

stlStatus stfLink( const stlChar * aFromPath,
                   const stlChar * aToPath,
                   stlErrorStack * aErrorStack );

stlStatus stfSeek( stlFile        * aFile,
                   stlInt32         aWhere,
                   stlOffset      * aOffset,
                   stlErrorStack  * aErrorStack );

stlStatus stfTruncate( stlFile        * aFile,
                       stlOffset        aOffset,
                       stlErrorStack  * aErrorStack );

stlStatus stfTransferContents( const stlChar * aFromPath,
                               const stlChar * aToPath,
                               stlInt32        aFlags,
                               stlInt32        aToPerms,
                               stlChar       * aBuffer,
                               stlInt32        aBufferSize,
                               stlErrorStack * aErrorStack );

stlStatus stfRead( stlFile       * aFile,
                   void          * aBuffer,
                   stlSize         aBytes,
                   stlSize       * aReadBytes,
                   stlErrorStack * aErrorStack );

stlStatus stfWrite( stlFile       * aFile,
                    void          * aBuffer,
                    stlSize         aBytes,
                    stlSize       * aWrittenBytes,
                    stlErrorStack * aErrorStack );

stlStatus stfPutCharacter( stlChar         aCharacter,
                           stlFile       * aFile,
                           stlErrorStack * aErrorStack );

stlStatus stfGetCharacter( stlChar       * aCharacter,
                           stlFile       * aFile,
                           stlErrorStack * aErrorStack );

stlStatus stfPutString( const stlChar * aString,
                        stlFile       * aFile,
                        stlErrorStack * aErrorStack );

stlStatus stfGetString( stlChar       * aString,
                        stlInt32        aLength,
                        stlFile       * aFile,
                        stlErrorStack * aErrorStack );

stlStatus stfSync( stlFile       * aFile,
                   stlErrorStack * aErrorStack );

stlStatus stfDuplicate( stlFile        * aNewFile,
                        stlFile        * aOldFile,
                        stlInt32         aOption,
                        stlErrorStack  * aErrorStack );

stlStatus stfLock( stlFile        * aFile,
                   stlInt32         aType,
                   stlErrorStack  * aErrorStack );

stlStatus stfUnlock( stlFile        * aFile,
                     stlErrorStack  * aErrorStack );

stlStatus stfCreatePipe( stlFile        * aInFile,
                         stlFile        * aOutFile,
                         stlErrorStack  * aErrorStack );

stlStatus stfOpenStdin( stlFile       * aFile,
                        stlErrorStack * aErrorStack );

stlStatus stfOpenStdout( stlFile       * aFile,
                         stlErrorStack * aErrorStack );

stlStatus stfOpenStderr( stlFile       * aFile,
                         stlErrorStack * aErrorStack );

STL_END_DECLS

#endif /* _STF_H_ */
