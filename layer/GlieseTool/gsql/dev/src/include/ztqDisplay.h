/*******************************************************************************
 * ztqDisplay.h
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


#ifndef _ZTQDISPLAY_H_
#define _ZTQDISPLAY_H_ 1

/**
 * @file ztqDisplay.h
 * @brief Display  Definition
 */

/**
 * @defgroup ztqDisplay Display
 * @ingroup ztq
 * @{
 */

stlStatus ztqPrintTableHeader( ztqRowBuffer    * aRowBuffer,
                               ztqTargetColumn * aTargetColumn );

stlStatus ztqPrintRowString( ztqRowBuffer    * aRowBuffer,
                             ztqTargetColumn * aTargetColumn,
                             stlInt32          aLineIdx,
                             stlAllocator    * aAllocator,
                             stlErrorStack   * aErrorStack );

stlStatus ztqPrintColumnLines( ztqRowBuffer      * aRowBuffer,
                               ztqTargetColumn   * aTargetColumn,
                               stlInt32            aLineIdx,
                               stlChar         *** aColumnLinePtrArray,
                               stlInt32            aStartColumnIdx,
                               stlInt32            aEndColumnIdx,
                               stlErrorStack     * aErrorStack );

stlStatus ztqDisableColoredText( stlErrorStack * aErrorStack );
stlStatus ztqEnableColoredText( stlErrorStack * aErrorStack );

stlStatus ztqPrintAnswerMessage( ztqStatementType   aStmtType,
                                 ztqObjectType      aObjectType,
                                 stlInt32           aRowCount,
                                 stlErrorStack    * aErrorStack );

stlStatus ztqPrintDmlAnswerMessage( ztqStatementType   aStmtType,
                                    stlInt32           aRowCount,
                                    stlErrorStack    * aErrorStack );

stlStatus ztqPrintTranAnswerMessage( stlBool         aIsCommit,
                                     stlErrorStack * aErrorStack );

stlStatus ztqPrintPreparedAnswerMessage( stlErrorStack * aErrorStack );

stlStatus ztqPrintEtcAnswerMessage( ztqStatementType   aStmtType,
                                    ztqObjectType      aObjectType,
                                    stlErrorStack    * aErrorStack );

void ztqClearElapsedTime();
stlStatus ztqPrintElapsedTime( stlErrorStack * aErrorStack );

stlStatus ztqPrintCommandHelp( stlErrorStack * aErrorStack );

void ztqPrintErrorStack( stlErrorStack * aErrorStack );
void ztqPrintErrorCallstack( stlErrorStack * aErrorStack );

stlInt32 ztqEchoPrintf( const stlChar *aFormat, ... );
stlInt32 ztqPrintf( const stlChar *aFormat, ... );
stlInt32 ztqPaintf( stlPaintAttr * aAttr, const stlChar *aFormat, ... );

void ztqFreeSpoolBuffer( );
stlStatus ztqAllocSpoolBuffer( stlSize         aSize,
                               stlErrorStack * aErrorStack );

stlInt32 ztqWriteSpoolPrintf( const stlChar *aFormat, ... );
stlStatus ztqWriteSpoolFile( stlInt32       aStringLength,
                             const stlChar *aFormat,
                             va_list        aVarArgList );

stlStatus ztqSpoolCommand( stlChar       *aSpoolFilePath,
                           ztqSpoolFlag   aSpoolFlag,
                           stlAllocator  *aAllocator,
                           stlErrorStack *aErrorStack );

stlStatus ztqSpoolBegin( stlChar       *aSpoolFilePath,
                         ztqSpoolFlag   aSpoolFlag,
                         stlAllocator  *aAllocator,
                         stlErrorStack *aErrorStack );

stlStatus ztqSpoolEnd( stlChar       *aSpoolFilePath,
                       ztqSpoolFlag   aSpoolFlag,
                       stlAllocator  *aAllocator,
                       stlErrorStack *aErrorStack );

stlStatus ztqSpoolStatus( stlChar       *aSpoolFilePath,
                          ztqSpoolFlag   aSpoolFlag,
                          stlAllocator  *aAllocator,
                          stlErrorStack *aErrorStack );

/** @} */

#endif /* _ZTQDISPLAY_H_ */
