/*******************************************************************************
 * smgRedo.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smgRedo.c 4506 2012-05-09 02:13:24Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

/**
 * @file smgRedo.c
 * @brief Storage Manager Layer External Redo Internal Routines
 */

#include <stl.h>
#include <dtl.h>
#include <knl.h>
#include <scl.h>
#include <smlDef.h>
#include <smlSuppLogDef.h>
#include <smDef.h>

/**
 * @addtogroup smg
 * @{
 */

smrRedoVector gSmgRedoVectors[SML_SUPPL_LOG_MAX] =
{
    { /* SML_SUPPL_LOG_UPDATE */
        NULL,
        NULL,
        "SUPPL_LOG_UPDATE"
    },
    { /* SML_SUPPL_LOG_DELETE */
        NULL,
        NULL,
        "SUPPL_LOG_DELETE"
    },
    { /* SML_SUPPL_LOG_UPDATE_COLS */
        NULL,
        NULL,
        "SML_SUPPL_LOG_UPDATE_COLS"
    },
    { /* SML_SUPPL_LOG_UPDATE_BEFORE_COLS */
        NULL,
        NULL,
        "SML_SUPPL_LOG_UPDATE_BEFORE_COLS"
    },
    { /* SML_SUPPL_LOG_DDL */
        NULL,
        NULL,
        "SUPPL_LOG_DDL"
    },
    { /* SML_SUPPL_LOG_DDL_CLR */
        NULL,
        NULL,
        "SUPPL_LOG_DDL_CLR"
    }
};

/** @} */
