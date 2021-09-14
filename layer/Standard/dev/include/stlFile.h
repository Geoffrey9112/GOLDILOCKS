/*******************************************************************************
 * stlFile.h
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


#ifndef _STLFILE_H_
#define _STLFILE_H_ 1

/**
 * @file stlFile.h
 * @brief Standard Layer File Routines
 */


STL_BEGIN_DECLS


/**
 * @defgroup stlFile File
 * @ingroup STL 
 * @{
 */
stlStatus stlGetFileStatByHandle( stlFileInfo   * aFileInfo,
                                  stlInt32        aWanted,
                                  stlFile       * aFile,
                                  stlErrorStack * aErrorStack );

stlStatus stlGetFileStatByName( stlFileInfo    * aFileInfo,
                                const stlChar  * aFileName,
                                stlInt32         aWanted,
                                stlErrorStack  * aErrorStack );

stlStatus stlOpenFile( stlFile        * aNewFile, 
                       const stlChar  * aFileName, 
                       stlInt32         aFlag, 
                       stlInt32         aPerm,
                       stlErrorStack  * aErrorStack );

stlStatus stlExistFile( const stlChar  * aFileName,
                        stlBool        * aExist,
                        stlErrorStack  * aErrorStack );

stlStatus stlCloseFile( stlFile       * aFile,
                        stlErrorStack * aErrorStack );

stlStatus stlRemoveFile( const stlChar * aPath,
                         stlErrorStack * aErrorStack );

stlStatus stlLinkFile( const stlChar * aFromPath,
                       const stlChar * aToPath,
                       stlErrorStack * aErrorStack );

stlStatus stlSeekFile( stlFile        * aFile,
                       stlInt32         aWhere,
                       stlOffset      * aOffset,
                       stlErrorStack  * aErrorStack );
    
stlStatus stlTruncateFile( stlFile        * aFile,
                           stlOffset        aOffset,
                           stlErrorStack  * aErrorStack );
    
stlStatus stlCopyFile( const stlChar * aFromPath,
                       const stlChar * aToPath,
                       stlInt32        aFlags,
                       stlInt32        aPerms,
                       stlChar       * aBuffer,
                       stlInt32        aBufferSize,
                       stlErrorStack * aErrorStack );

stlStatus stlAppendFile( const stlChar * aFromPath,
                         const stlChar * aToPath,
                         stlInt32        aPerms,
                         stlErrorStack * aErrorStack );
    
stlStatus stlReadFile( stlFile       * aFile,
                       void          * aBuffer,
                       stlSize         aBytes,
                       stlSize       * aReadBytes,
                       stlErrorStack * aErrorStack );
    
stlStatus stlWriteFile( stlFile       * aFile,
                        void          * aBuffer,
                        stlSize         aBytes,
                        stlSize       * aWrittenBytes,
                        stlErrorStack * aErrorStack );
    
stlStatus stlPutCharacterFile( stlChar         aCharacter,
                               stlFile       * aFile,
                               stlErrorStack * aErrorStack );

stlStatus stlGetCharacterFile( stlChar       * aCharacter,
                               stlFile       * aFile,
                               stlErrorStack * aErrorStack );

stlStatus stlPutStringFile( const stlChar * aString,
                            stlFile       * aFile,
                            stlErrorStack * aErrorStack );

stlStatus stlGetStringFile( stlChar       * aString,
                            stlInt32        aLength,
                            stlFile       * aFile,
                            stlErrorStack * aErrorStack );
stlStatus stlSyncFile( stlFile       * aFile,
                       stlErrorStack * aErrorStack );

stlStatus stlDuplicateFile( stlFile        * aNewFile,
                            stlFile        * aOldFile,
                            stlInt32         aOption,
                            stlErrorStack  * aErrorStack );

stlStatus stlLockFile( stlFile        * aFile,
                       stlInt32         aType,
                       stlErrorStack  * aErrorStack );

stlStatus stlUnlockFile( stlFile        * aFile,
                         stlErrorStack  * aErrorStack );

stlStatus stlCreatePipe( stlFile        * aInFile,
                         stlFile        * aOutFile,
                         stlErrorStack  * aErrorStack );

stlStatus stlOpenStdinFile( stlFile        * aFile,
                            stlErrorStack  * aErrorStack );

stlStatus stlOpenStdoutFile( stlFile        * aFile,
                             stlErrorStack  * aErrorStack );

stlStatus stlOpenStderrFile( stlFile        * aFile,
                             stlErrorStack  * aErrorStack );

stlStatus stlMergeAbsFilePath( stlChar       * aRootPath,
                               stlChar       * aAddPath,
                               stlChar       * aNewPath,
                               stlInt32        aNewPathBufferLen,
                               stlInt32      * aNewPathLen,
                               stlErrorStack * aErrorStack );

stlStatus stlGetCurFilePath( stlChar       * aPath,
                             stlInt32        aPathBufLen,
                             stlErrorStack * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLFILE_H_ */
