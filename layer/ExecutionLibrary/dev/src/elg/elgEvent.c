/*******************************************************************************
 * elgEvent.c
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

/**
 * @file elgEvent.c
 * @brief Execution Library Layer Event
 */


#include <ell.h>
#include <elDef.h>
#include <eldt.h>
#include <eldc.h>

#include <eldDictionary.h>

/**
 * @addtogroup elgEvent
 * @{
 */

knlEventTable gElgEventTable[] =
{
    {
        "aging dictionary cache",
        eldAgingDictMem
    }
};

/** @} elgEvent */
