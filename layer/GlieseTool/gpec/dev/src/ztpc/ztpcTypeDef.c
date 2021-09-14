/*******************************************************************************
 * ztpcTypeDef.c
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

#include <dtl.h>
#include <goldilocks.h>
#include <ztpDef.h>
#include <ztpcTypeDef.h>
#include <ztpuFile.h>

/**
 * @file ztpcTypeDef.c
 * @brief Gliese Embedded SQL in C precompiler common functions on Type Definitions
 */

/**
 * @addtogroup ztpcTypeDef
 * @{
 */

/************************************************************************
 * Type Definitions
 ************************************************************************/

stlStatus ztpcAddTypeDefToSymTab(ztpSymTabHeader   *aSymTab,
                                 ztpCVariable      *aTypeDefSymbolList)
{
    ztpCVariable   *sLast;

    if( aSymTab->mTypedefListHead == NULL )
    {
        aSymTab->mTypedefListHead = aTypeDefSymbolList;
    }
    else
    {
        sLast = aSymTab->mTypedefListHead;

        while( sLast->mNext != NULL )
        {
            sLast = sLast->mNext;
        }

        sLast->mNext = aTypeDefSymbolList;
    }

    return STL_SUCCESS;
}

stlStatus ztpcFindTypeDefInSymTab(stlChar          *aSymName,
                                  ztpCVariable    **aTypeDefSymbol)
{
    ztpSymTabHeader  *sSymTab = NULL;
    ztpCVariable     *sSymEntry = NULL;

    *aTypeDefSymbol = NULL;
    sSymTab = gCurrSymTab;
    while(sSymTab != NULL)
    {
        {
            /* Debug */
            /*
            stlPrintf( "Find Typedef : SymTab [%p], Name : [%s]\n",
                       sSymTab, aSymName );
            */
        }
        for( sSymEntry = sSymTab->mTypedefListHead;
             sSymEntry != NULL;
             sSymEntry = sSymEntry->mNext )
        {
            /* Debug */
            /*
            stlPrintf( "    SymTabEntry : [%s], Avail : %d\n",
                       sSymEntry->mName,
                       sSymEntry->mIsAvailHostVar );
            */
            if(stlStrncmp(sSymEntry->mName,
                          aSymName,
                          STL_MAX_SQL_IDENTIFIER_LENGTH) == 0)
            {
                *aTypeDefSymbol = sSymEntry;
                STL_THROW(EXIT_FUNCTION);
            }
        }

        sSymTab = sSymTab->mPrevDepth;
    }

    STL_RAMP(EXIT_FUNCTION);
    STL_TRY(*aTypeDefSymbol != NULL);

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/** @} */
