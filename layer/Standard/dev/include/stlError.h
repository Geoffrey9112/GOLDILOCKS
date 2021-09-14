/*******************************************************************************
 * stlError.h
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


#ifndef _STLERROR_H_
#define _STLERROR_H_ 1

/**
 * @file stlError.h
 * @brief Standard Layer Error Routines
 */

extern stlFatalHandler gStlFatalHandler;

STL_BEGIN_DECLS

/**
 * @defgroup stlError Error
 * @ingroup STL 
 * @{
 */

void stlRegisterErrorTable( stlErrorModule    aErrorModule,
                            stlErrorRecord  * aErrorTable );

void stlPushError( stlErrorLevel     aErrorLevel,
                   stlUInt32         aErrorCode,
                   stlChar         * aDetailErrorMessage,
                   stlErrorStack   * aErrorStack,
                   ... );

void stlPushFatalErrorCallstack( void            * aSigInfo,
                                 void            * aContext,
                                 stlErrorStack   * aErrorStack );

stlStatus stlHookFatalHandler( stlFatalHandler   aNewHandler,
                               stlFatalHandler * aOldHandler );

void stlAssert( const stlChar * aFormat, ... );

stlErrorData * stlPopError( stlErrorStack * aErrorStack );
stlErrorData * stlGetLastErrorData( stlErrorStack * aErrorStack );
void stlPushErrorData( stlErrorData  * aErrorData,
                       stlErrorStack * aErrorStack );
void stlAppendErrors( stlErrorStack * aDestErrorStack,
                      stlErrorStack * aSrcErrorStack );
stlInt32 stlGetErrorStackDepth( stlErrorStack * aErrorStack );
stlInt32 stlGetFailureErrorCount( stlErrorStack * aErrorStack );

stlUInt32 stlToExternalErrorCode( stlUInt32 aInternalErrorCode );
stlUInt32 stlGetExternalErrorCode( stlErrorData * aErrorData );
void stlMakeSqlState( stlUInt32 aExternalErrorCode, stlChar * aSqlState );

stlUInt32 stlGetLastErrorCode( stlErrorStack * aErrorStack );
stlUInt32 stlGetLastSystemErrorCode( stlErrorStack * aErrorStack );
stlChar * stlGetLastErrorMessage( stlErrorStack * aErrorStack );
stlChar * stlGetLastDetailErrorMessage( stlErrorStack * aErrorStack );
void stlSetLastDetailErrorMessage( stlErrorStack   * aErrorStack,
                                   stlChar         * aDetailErrorMessage );

void stlTestPrintErrorStack( stlErrorStack * aErrorStack );
void stlPrintFirstErrorCallstack( stlErrorStack * aErrorStack );
void stlPrintFirstErrorCallstackToStr( stlErrorStack * aErrorStack,
                                       stlChar       * aBuffer,
                                       stlInt32        aBufferSize );

stlErrorData * stlGetFirstErrorData( stlErrorStack * aErrorStack,
                                     stlInt16      * aErrorIdx  );
stlErrorData * stlGetNextErrorData( stlErrorStack * aErrorStack,
                                    stlInt16      * aErrorIdx  );

stlBool stlCheckErrorCode( stlErrorStack  * aErrorStack,
                           stlUInt32        aInternalErrorCode );

stlBool stlHasWarningError( stlErrorStack * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLERROR_H_ */
