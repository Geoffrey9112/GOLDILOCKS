/*******************************************************************************
 * ztpfPathControl.c
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

#include <stl.h>
#include <goldilocks.h>
#include <ztpDef.h>
#include <ztpfPathControl.h>

/**
 * @file ztpfPathControl.c
 * @brief File/Path manipulation Routines
 */

/**
 * @addtogroup ztpfPathControl
 * @{
 */

/*
stlStatus ztpfSearchFile(stlChar         *aSourceFilePath,
                         stlChar         *aFileName,
                         stlChar         *aSearchFileName,
                         stlChar         *aSearchFilePath,
                         stlBool         *aIsFound,
                         stlErrorStack   *aErrorStack)
{
    stlBool             sFileExist = STL_FALSE;
    stlBool             sIsQuote = STL_FALSE;
    ztpIncludePath     *sIncludePath = &gIncludePath;
    stlChar             sArgPath[STL_MAX_FILE_PATH_LENGTH + 1];
    stlChar             sCurPath[STL_MAX_FILE_PATH_LENGTH + 1];
    stlInt32            sCurPathLen = STL_MAX_FILE_PATH_LENGTH + 1;

    if( aFileName[0] == '"' )
    {
        sIsQuote = STL_TRUE;
    }
    else
    {
        sIsQuote = STL_FALSE;
        STL_DASSERT( aFileName[0] == '<' );
    }

    / * Trim filename cover symbol : "", <> * /
    stlStrncpy( aSearchFileName, aFileName + 1, stlStrlen( aFileName ) - 1 );

    if( sIsQuote == STL_TRUE )
    {
        stlGetCurFilePath( sCurPath,
                           sCurPathLen,
                           aErrorStack );

        ztpfDirname( aSourceFilePath,
                     sArgPath );

        stlSnprintf( aSearchFilePath, STL_MAX_FILE_PATH_LENGTH + 1,
                     "%s%s%s%s%s",
                     sCurPath,
                     STL_PATH_SEPARATOR,
                     sArgPath,
                     STL_PATH_SEPARATOR,
                     aSearchFileName );

        if( stlExistFile( aSearchFilePath,
                          &sFileExist,
                          aErrorStack )
            == STL_SUCCESS )
        {
            if( sFileExist == STL_TRUE )
            {
                STL_THROW( RAMP_FILE_CHECK );
            }
        }
    }

    for( sIncludePath = &gIncludePath;
         sIncludePath != NULL;
         sIncludePath = sIncludePath->mNext )
    {
        stlSnprintf( aSearchFilePath, STL_MAX_FILE_PATH_LENGTH + 1,
                     "%s%s%s",
                     sIncludePath->mIncludePath,
                     STL_PATH_SEPARATOR,
                     aSearchFileName );

        if( stlExistFile( aSearchFilePath,
                          &sFileExist,
                          aErrorStack )
            == STL_SUCCESS )
        {
            if( sFileExist == STL_TRUE )
            {
                STL_THROW( RAMP_FILE_CHECK );
            }
        }
    }

    STL_RAMP( RAMP_FILE_CHECK );

    *aIsFound = sFileExist;

    return STL_SUCCESS;
}

stlStatus ztpfDirname( stlChar *aPath,
                       stlChar *aDirname )
{
    stlChar   *sPath = aPath;
    stlChar   *sSegment;
    stlInt32   sRemainLen;

    sSegment = stlStrstr( sPath, STL_PATH_SEPARATOR );
    while( sSegment != NULL )
    {
        sRemainLen = stlStrlen( sSegment );
        sPath = &sPath[stlStrlen(sPath) - sRemainLen + 1];
        sSegment = stlStrstr( sPath, STL_PATH_SEPARATOR );
    }

    stlStrncpy( aDirname, aPath, sPath - aPath );

    return STL_SUCCESS;
}
*/

/** @} */

