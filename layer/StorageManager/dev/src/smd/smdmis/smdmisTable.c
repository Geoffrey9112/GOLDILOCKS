/*******************************************************************************
 * smdmisTable.c
 *
 * Copyright (c) 2012, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: smdmisTable.c 6322 2012-11-13 05:16:47Z egon $
 *
 * NOTES
 *
 *
 ******************************************************************************/

/**
 * @file smdmisTable.c
 * @brief Storage Manager Layer Memory Instant Sort Index Table Routines
 */

#include <sml.h>
#include <smDef.h>
#include <smdDef.h>
#include <smg.h>
#include <sma.h>
#include <smd.h>
#include <smo.h>
#include <smsDef.h>
#include <smsManager.h>
#include <smdmisTable.h>

/**
 * @addtogroup smdmisTable
 * @{
 */

smdTableModule gSmdmisTableModule =
{
    NULL, /* CreateFunc */
    NULL, /* CreateIotFunc */
    NULL, /* CreateUndoFunc */
    smdmisTableDrop,
    NULL, /* CopyFunc */
    NULL, /* AddColumnFunc */
    NULL, /* DropColumnFunc */
    NULL, /* AlterTableAttrFunc */
    NULL, /* MergeTableFunc */
    NULL, /* CheckExistRowFunc */
    NULL, /* CreateForTruncateFunc */
    NULL, /* DropAgingFunc */
    NULL, /* InitializeFunc */
    NULL, /* FinalizeFunc */
    smdmisTableInsert,
    smdmisTableBlockInsert,
    smdmifTableUpdate,
    smdmifTableDelete,
    smdmisTableMerge,
    NULL, /* RowCountFunc */
    smdmifTableFetch,
    NULL, /* FetchWithRowidFunc */
    NULL, /* LockRowFunc */
    NULL, /* CompareKeyValueFunc */
    NULL, /* ExtractKeyValueFunc */
    NULL, /* ExtractValidKeyValueFunc */
    NULL, /* SortFunc */
    NULL, /* GetSlotBodyFunc */
    smdmisTableTruncate,
    smdmifTableSetFlag,
    smdmifTableCleanup,
    NULL,  /* BuildCacheFunc */
    NULL   /* Fetch4Index */
};

/* radix sort를 위한 key value 조작 함수들 */
static void smdmisModifyKeyValNumeric( smdmisKeyRow       aKeyRow,
                                       stlInt64           aColLen,
                                       smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNull( smdmisKeyRow       aKeyRow,
                                    stlInt64           aColLen,
                                    smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNumericNull( smdmisKeyRow       aKeyRow,
                                           stlInt64           aColLen,
                                           smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNullFirst( smdmisKeyRow       aKeyRow,
                                         stlInt64           aColLen,
                                         smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNumericNullFirst( smdmisKeyRow       aKeyRow,
                                                stlInt64           aColLen,
                                                smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValDesc( smdmisKeyRow       aKeyRow,
                                    stlInt64           aColLen,
                                    smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNumericDesc( smdmisKeyRow       aKeyRow,
                                           stlInt64           aColLen,
                                           smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNullDesc( smdmisKeyRow       aKeyRow,
                                        stlInt64           aColLen,
                                        smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNumericNullDesc( smdmisKeyRow       aKeyRow,
                                               stlInt64           aColLen,
                                               smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNullFirstDesc( smdmisKeyRow       aKeyRow,
                                             stlInt64           aColLen,
                                             smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValNumericNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                    stlInt64           aColLen,
                                                    smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValSmallInt( smdmisKeyRow       aKeyRow,
                                        stlInt64           aColLen,
                                        smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValInteger( smdmisKeyRow       aKeyRow,
                                       stlInt64           aColLen,
                                       smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValBigInt( smdmisKeyRow       aKeyRow,
                                      stlInt64           aColLen,
                                      smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalYM( smdmisKeyRow       aKeyRow,
                                          stlInt64           aColLen,
                                          smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalDS( smdmisKeyRow       aKeyRow,
                                          stlInt64           aColLen,
                                          smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValSmallIntNull( smdmisKeyRow       aKeyRow,
                                            stlInt64           aColLen,
                                            smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntegerNull( smdmisKeyRow       aKeyRow,
                                           stlInt64           aColLen,
                                           smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValBigIntNull( smdmisKeyRow       aKeyRow,
                                          stlInt64           aColLen,
                                          smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalYMNull( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalDSNull( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValSmallIntNullFirst( smdmisKeyRow       aKeyRow,
                                                 stlInt64           aColLen,
                                                 smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntegerNullFirst( smdmisKeyRow       aKeyRow,
                                                stlInt64           aColLen,
                                                smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValBigIntNullFirst( smdmisKeyRow       aKeyRow,
                                               stlInt64           aColLen,
                                               smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalYMNullFirst( smdmisKeyRow       aKeyRow,
                                                   stlInt64           aColLen,
                                                   smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalDSNullFirst( smdmisKeyRow       aKeyRow,
                                                   stlInt64           aColLen,
                                                   smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValSmallIntDesc( smdmisKeyRow       aKeyRow,
                                            stlInt64           aColLen,
                                            smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntegerDesc( smdmisKeyRow       aKeyRow,
                                           stlInt64           aColLen,
                                           smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValBigIntDesc( smdmisKeyRow       aKeyRow,
                                          stlInt64           aColLen,
                                          smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalYMDesc( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalDSDesc( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValSmallIntNullDesc( smdmisKeyRow       aKeyRow,
                                                stlInt64           aColLen,
                                                smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntegerNullDesc( smdmisKeyRow       aKeyRow,
                                               stlInt64           aColLen,
                                               smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValBigIntNullDesc( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalYMNullDesc( smdmisKeyRow       aKeyRow,
                                                  stlInt64           aColLen,
                                                  smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalDSNullDesc( smdmisKeyRow       aKeyRow,
                                                  stlInt64           aColLen,
                                                  smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValSmallIntNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                     stlInt64           aColLen,
                                                     smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntegerNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                    stlInt64           aColLen,
                                                    smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValBigIntNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                   stlInt64           aColLen,
                                                   smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalYMNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                       stlInt64           aColLen,
                                                       smdmifDefColumn   *aKeyCol );

static void smdmisModifyKeyValIntervalDSNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                       stlInt64           aColLen,
                                                       smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValDesc( smdmisKeyRow       aKeyRow,
                                     smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValSmallInt( smdmisKeyRow       aKeyRow,
                                         smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValInteger( smdmisKeyRow       aKeyRow,
                                        smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValBigInt( smdmisKeyRow       aKeyRow,
                                       smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValIntervalYM( smdmisKeyRow       aKeyRow,
                                           smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValIntervalDS( smdmisKeyRow       aKeyRow,
                                           smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValSmallIntDesc( smdmisKeyRow       aKeyRow,
                                             smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValIntegerDesc( smdmisKeyRow       aKeyRow,
                                            smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValBigIntDesc( smdmisKeyRow       aKeyRow,
                                           smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValIntervalYMDesc( smdmisKeyRow       aKeyRow,
                                               smdmifDefColumn   *aKeyCol );

static void smdmisRecoverKeyValIntervalDSDesc( smdmisKeyRow       aKeyRow,
                                               smdmifDefColumn   *aKeyCol );

/* key 저장시, null order, not null 유무, desc 등을 위해 후처리하는 함수 백터 [asc|desc][not_null|null_last|null_first][type] */
smdmisModifyKeyValFunc  gSmdmisModifyKeyValFuncVector[2][3][DTL_TYPE_MAX] =
{
    {
        {   /* asc, not null */
            NULL,                             /* DTL_TYPE_BOOLEAN */
            smdmisModifyKeyValSmallInt,       /* DTL_TYPE_NATIVE_SMALLINT */
            smdmisModifyKeyValInteger,        /* DTL_TYPE_NATIVE_INTEGER */
            smdmisModifyKeyValBigInt,         /* DTL_TYPE_NATIVE_BIGINT */
            NULL,                             /* DTL_TYPE_NATIVE_REAL */
            NULL,                             /* DTL_TYPE_NATIVE_DOUBLE */
            smdmisModifyKeyValNumeric,        /* DTL_TYPE_FLOAT */            
            smdmisModifyKeyValNumeric,        /* DTL_TYPE_NUMBER */
            NULL,                             /* DTL_TYPE_DECIMAL */
            NULL,                             /* DTL_TYPE_CHAR */
            NULL,                             /* DTL_TYPE_VARCHAR */
            NULL,                             /* DTL_TYPE_LONGVARCHAR */
            NULL,                             /* DTL_TYPE_CLOB */
            NULL,                             /* DTL_TYPE_BINARY */
            NULL,                             /* DTL_TYPE_VARBINARY */
            NULL,                             /* DTL_TYPE_LONGVARBINARY */
            NULL,                             /* DTL_TYPE_BLOB */
            smdmisModifyKeyValBigInt,         /* DTL_TYPE_DATE */
            smdmisModifyKeyValBigInt,         /* DTL_TYPE_TIME */
            smdmisModifyKeyValBigInt,         /* DTL_TYPE_TIMESTAMP */
            smdmisModifyKeyValBigInt,         /* DTL_TYPE_TIME_WITH_TIME_ZONE */
            smdmisModifyKeyValBigInt,         /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
            smdmisModifyKeyValIntervalYM,     /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
            smdmisModifyKeyValIntervalDS,     /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
            NULL,                             /* DTL_TYPE_ROWID */
        },
        {   /* asc, null last */
            smdmisModifyKeyValNull,           /* DTL_TYPE_BOOLEAN */
            smdmisModifyKeyValSmallIntNull,   /* DTL_TYPE_NATIVE_SMALLINT */
            smdmisModifyKeyValIntegerNull,    /* DTL_TYPE_NATIVE_INTEGER */
            smdmisModifyKeyValBigIntNull,     /* DTL_TYPE_NATIVE_BIGINT */
            smdmisModifyKeyValNull,           /* DTL_TYPE_NATIVE_REAL */
            smdmisModifyKeyValNull,           /* DTL_TYPE_NATIVE_DOUBLE */
            smdmisModifyKeyValNumericNull,    /* DTL_TYPE_FLOAT */            
            smdmisModifyKeyValNumericNull,    /* DTL_TYPE_NUMBER */
            smdmisModifyKeyValNull,           /* DTL_TYPE_DECIMAL */
            smdmisModifyKeyValNull,           /* DTL_TYPE_CHAR */
            smdmisModifyKeyValNull,           /* DTL_TYPE_VARCHAR */
            smdmisModifyKeyValNull,           /* DTL_TYPE_LONGVARCHAR */
            smdmisModifyKeyValNull,           /* DTL_TYPE_CLOB */
            smdmisModifyKeyValNull,           /* DTL_TYPE_BINARY */
            smdmisModifyKeyValNull,           /* DTL_TYPE_VARBINARY */
            smdmisModifyKeyValNull,           /* DTL_TYPE_LONGVARBINARY */
            smdmisModifyKeyValNull,           /* DTL_TYPE_BLOB */
            smdmisModifyKeyValBigIntNull,     /* DTL_TYPE_DATE */
            smdmisModifyKeyValBigIntNull,     /* DTL_TYPE_TIME */
            smdmisModifyKeyValBigIntNull,     /* DTL_TYPE_TIMESTAMP */
            smdmisModifyKeyValBigIntNull,     /* DTL_TYPE_TIME_WITH_TIME_ZONE */
            smdmisModifyKeyValBigIntNull,     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
            smdmisModifyKeyValIntervalYMNull, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
            smdmisModifyKeyValIntervalDSNull, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
            smdmisModifyKeyValNull,           /* DTL_TYPE_ROWID */
        },
        {   /* asc, null first */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_BOOLEAN */
            smdmisModifyKeyValSmallIntNullFirst,   /* DTL_TYPE_NATIVE_SMALLINT */
            smdmisModifyKeyValIntegerNullFirst,    /* DTL_TYPE_NATIVE_INTEGER */
            smdmisModifyKeyValBigIntNullFirst,     /* DTL_TYPE_NATIVE_BIGINT */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_NATIVE_REAL */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_NATIVE_DOUBLE */
            smdmisModifyKeyValNumericNullFirst,    /* DTL_TYPE_FLOAT */            
            smdmisModifyKeyValNumericNullFirst,    /* DTL_TYPE_NUMBER */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_DECIMAL */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_CHAR */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_VARCHAR */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_LONGVARCHAR */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_CLOB */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_BINARY */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_VARBINARY */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_LONGVARBINARY */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_BLOB */
            smdmisModifyKeyValBigIntNullFirst,     /* DTL_TYPE_DATE */
            smdmisModifyKeyValBigIntNullFirst,     /* DTL_TYPE_TIME */
            smdmisModifyKeyValBigIntNullFirst,     /* DTL_TYPE_TIMESTAMP */
            smdmisModifyKeyValBigIntNullFirst,     /* DTL_TYPE_TIME_WITH_TIME_ZONE */
            smdmisModifyKeyValBigIntNullFirst,     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
            smdmisModifyKeyValIntervalYMNullFirst, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
            smdmisModifyKeyValIntervalDSNullFirst, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
            smdmisModifyKeyValNullFirst,           /* DTL_TYPE_ROWID */
        },
    },
    {
        {   /* desc, not null */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_BOOLEAN */
            smdmisModifyKeyValSmallIntDesc,   /* DTL_TYPE_NATIVE_SMALLINT */
            smdmisModifyKeyValIntegerDesc,    /* DTL_TYPE_NATIVE_INTEGER */
            smdmisModifyKeyValBigIntDesc,     /* DTL_TYPE_NATIVE_BIGINT */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_NATIVE_REAL */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_NATIVE_DOUBLE */
            smdmisModifyKeyValNumericDesc,    /* DTL_TYPE_FLOAT */            
            smdmisModifyKeyValNumericDesc,    /* DTL_TYPE_NUMBER */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_DECIMAL */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_CHAR */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_VARCHAR */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_LONGVARCHAR */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_CLOB */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_BINARY */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_VARBINARY */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_LONGVARBINARY */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_BLOB */
            smdmisModifyKeyValBigIntDesc,     /* DTL_TYPE_DATE */
            smdmisModifyKeyValBigIntDesc,     /* DTL_TYPE_TIME */
            smdmisModifyKeyValBigIntDesc,     /* DTL_TYPE_TIMESTAMP */
            smdmisModifyKeyValBigIntDesc,     /* DTL_TYPE_TIME_WITH_TIME_ZONE */
            smdmisModifyKeyValBigIntDesc,     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
            smdmisModifyKeyValIntervalYMDesc, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
            smdmisModifyKeyValIntervalDSDesc, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
            smdmisModifyKeyValDesc,           /* DTL_TYPE_ROWID */
        },
        {   /* desc, null last */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_BOOLEAN */
            smdmisModifyKeyValSmallIntNullDesc,   /* DTL_TYPE_NATIVE_SMALLINT */
            smdmisModifyKeyValIntegerNullDesc,    /* DTL_TYPE_NATIVE_INTEGER */
            smdmisModifyKeyValBigIntNullDesc,     /* DTL_TYPE_NATIVE_BIGINT */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_NATIVE_REAL */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_NATIVE_DOUBLE */
            smdmisModifyKeyValNumericNullDesc,    /* DTL_TYPE_FLOAT */            
            smdmisModifyKeyValNumericNullDesc,    /* DTL_TYPE_NUMBER */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_DECIMAL */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_CHAR */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_VARCHAR */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_LONGVARCHAR */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_CLOB */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_BINARY */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_VARBINARY */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_LONGVARBINARY */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_BLOB */
            smdmisModifyKeyValBigIntNullDesc,     /* DTL_TYPE_DATE */
            smdmisModifyKeyValBigIntNullDesc,     /* DTL_TYPE_TIME */
            smdmisModifyKeyValBigIntNullDesc,     /* DTL_TYPE_TIMESTAMP */
            smdmisModifyKeyValBigIntNullDesc,     /* DTL_TYPE_TIME_WITH_TIME_ZONE */
            smdmisModifyKeyValBigIntNullDesc,     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
            smdmisModifyKeyValIntervalYMNullDesc, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
            smdmisModifyKeyValIntervalDSNullDesc, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
            smdmisModifyKeyValNullDesc,           /* DTL_TYPE_ROWID */
        },
        {   /* desc, null first */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_BOOLEAN */
            smdmisModifyKeyValSmallIntNullFirstDesc,   /* DTL_TYPE_NATIVE_SMALLINT */
            smdmisModifyKeyValIntegerNullFirstDesc,    /* DTL_TYPE_NATIVE_INTEGER */
            smdmisModifyKeyValBigIntNullFirstDesc,     /* DTL_TYPE_NATIVE_BIGINT */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_NATIVE_REAL */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_NATIVE_DOUBLE */
            smdmisModifyKeyValNumericNullFirstDesc,    /* DTL_TYPE_FLOAT */            
            smdmisModifyKeyValNumericNullFirstDesc,    /* DTL_TYPE_NUMBER */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_DECIMAL */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_CHAR */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_VARCHAR */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_LONGVARCHAR */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_CLOB */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_BINARY */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_VARBINARY */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_LONGVARBINARY */
            smdmisModifyKeyValNullFirstDesc,           /* DTL_TYPE_BLOB */
            smdmisModifyKeyValBigIntNullFirstDesc,     /* DTL_TYPE_DATE */
            smdmisModifyKeyValBigIntNullFirstDesc,     /* DTL_TYPE_TIME */
            smdmisModifyKeyValBigIntNullFirstDesc,     /* DTL_TYPE_TIMESTAMP */
            smdmisModifyKeyValBigIntNullFirstDesc,     /* DTL_TYPE_TIME_WITH_TIME_ZONE */
            smdmisModifyKeyValBigIntNullFirstDesc,     /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
            smdmisModifyKeyValIntervalYMNullFirstDesc, /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
            smdmisModifyKeyValIntervalDSNullFirstDesc, /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
            smdmisModifyKeyValNullFirstDesc            /* DTL_TYPE_ROWID */
        }
    }
};

/* key 저장시, null order, not null 유무, desc 등을 위해 후처리하는 함수 백터 [asc|desc][type] */
smdmisRecoverKeyValFunc  gSmdmisRecoverKeyValFuncVector[2][DTL_TYPE_MAX] =
{
    {   /* asc */
        NULL,                             /* DTL_TYPE_BOOLEAN */
        smdmisRecoverKeyValSmallInt,      /* DTL_TYPE_NATIVE_SMALLINT */
        smdmisRecoverKeyValInteger,       /* DTL_TYPE_NATIVE_INTEGER */
        smdmisRecoverKeyValBigInt,        /* DTL_TYPE_NATIVE_BIGINT */
        NULL,                             /* DTL_TYPE_NATIVE_REAL */
        NULL,                             /* DTL_TYPE_NATIVE_DOUBLE */
        NULL,                             /* DTL_TYPE_FLOAT */        
        NULL,                             /* DTL_TYPE_NUMBER */
        NULL,                             /* DTL_TYPE_DECIMAL */
        NULL,                             /* DTL_TYPE_CHAR */
        NULL,                             /* DTL_TYPE_VARCHAR */
        NULL,                             /* DTL_TYPE_LONGVARCHAR */
        NULL,                             /* DTL_TYPE_CLOB */
        NULL,                             /* DTL_TYPE_BINARY */
        NULL,                             /* DTL_TYPE_VARBINARY */
        NULL,                             /* DTL_TYPE_LONGVARBINARY */
        NULL,                             /* DTL_TYPE_BLOB */
        smdmisRecoverKeyValBigInt,        /* DTL_TYPE_DATE */
        smdmisRecoverKeyValBigInt,        /* DTL_TYPE_TIME */
        smdmisRecoverKeyValBigInt,        /* DTL_TYPE_TIMESTAMP */
        smdmisRecoverKeyValBigInt,        /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        smdmisRecoverKeyValBigInt,        /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        smdmisRecoverKeyValIntervalYM,    /* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        smdmisRecoverKeyValIntervalDS,    /* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        NULL,                             /* DTL_TYPE_ROWID */
    },
    {   /* desc */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_BOOLEAN */
        smdmisRecoverKeyValSmallIntDesc,  /* DTL_TYPE_NATIVE_SMALLINT */
        smdmisRecoverKeyValIntegerDesc,   /* DTL_TYPE_NATIVE_INTEGER */
        smdmisRecoverKeyValBigIntDesc,    /* DTL_TYPE_NATIVE_BIGINT */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_NATIVE_REAL */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_NATIVE_DOUBLE */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_FLOAT */        
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_NUMBER */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_DECIMAL */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_CHAR */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_VARCHAR */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_LONGVARCHAR */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_CLOB */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_BINARY */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_VARBINARY */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_LONGVARBINARY */
        smdmisRecoverKeyValDesc,          /* DTL_TYPE_BLOB */
        smdmisRecoverKeyValBigIntDesc,    /* DTL_TYPE_DATE */
        smdmisRecoverKeyValBigIntDesc,    /* DTL_TYPE_TIME */
        smdmisRecoverKeyValBigIntDesc,    /* DTL_TYPE_TIMESTAMP */
        smdmisRecoverKeyValBigIntDesc,    /* DTL_TYPE_TIME_WITH_TIME_ZONE */
        smdmisRecoverKeyValBigIntDesc,    /* DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE */
        smdmisRecoverKeyValIntervalYMDesc,/* DTL_TYPE_INTERVAL_YEAR_TO_MONTH */
        smdmisRecoverKeyValIntervalDSDesc,/* DTL_TYPE_INTERVAL_DAY_TO_SECOND */
        smdmisRecoverKeyValDesc           /* DTL_TYPE_ROWID */
    }
};

static void smdmisSetCompareFunc( smdmisTableHeader *aHeader );

static stlStatus smdmisInitKeyColInfo( smdmisTableHeader  *aHeader,
                                       stlUInt16           aKeyColCount,
                                       knlValueBlockList  *aKeyColList,
                                       stlUInt8           *aKeyColFlags,
                                       smlEnv             *aEnv );

static stlBool smdmisLSDRadixIsBetter( smdmisTableHeader *aHeader,
                                       smlEnv            *aEnv );

static stlStatus smdmisInitKeyCols( smdmisTableHeader *aHeader,
                                    smdmifTableHeader *aBaseHeader,
                                    stlBool            aBottomUp,
                                    smlEnv            *aEnv );

static stlInt32 smdmisCalculateNodeCount( smdmisTableHeader *aHeader,
                                          smdmifTableHeader *aBaseHeader );

static stlInt32 smdmisCalculateKeyCount( stlInt32   aNodeSize,
                                         stlInt32   aNodeHeaderSize,
                                         stlInt32   aKeyRowLen,
                                         stlUInt16 *aFirstOffset );

static void smdmisIndexNodeInit( smdmisIndexNode *aNode,
                                 stlInt16         aLevel,
                                 stlInt32         aNodeSize,
                                 stlInt32         aKeyRowLen );

static void smdmisFreeIndexNode( smdmisTableHeader *aHeader,
                                 smdmisIndexNode   *aNode );

static void smdmisFreeRadixNode( smdmisTableHeader *aHeader,
                                 smdmisRadixNode   *aNode );

static stlStatus smdmisAllocIndexNode( smdmisTableHeader *aHeader,
                                       smdmisIndexNode  **aNewNode,
                                       smlEnv            *aEnv );

static stlStatus smdmisAllocRadixNode( smdmisTableHeader *aHeader,
                                       smdmisRadixNode  **aNewNode,
                                       smlEnv            *aEnv );

static stlStatus smdmisAllocKeyRow( smdmisTableHeader *aHeader,
                                    smdmisIndexPath   *aTraverseStack,
                                    smdmisIndexNode   *aNode,
                                    stlInt32           aIndex,
                                    smdmisKeyRow      *aKeyRow,
                                    smlEnv            *aEnv );

static stlStatus smdmisFindKey( smdmisTableHeader *aHeader,
                                stlInt32           aValueIdx,
                                knlValueBlockList *aCols,
                                smdmisKeyRow      *aFoundKeyRow,
                                smdmisIndexNode  **aFoundNode,
                                stlInt32          *aFoundPos,
                                smdmisIndexPath   *aTraverseStack,
                                smlEnv            *aEnv );

static inline void smdmisCompactNode( smdmisTableHeader *aHeader,
                                      smdmisIndexNode   *aNode );

static stlStatus smdmisSplitNode( smdmisTableHeader *aHeader,
                                  smdmisIndexPath   *aTraverseStack,
                                  smdmisIndexNode   *aNode,
                                  smdmisIndexNode  **aNextNode,
                                  smlEnv            *aEnv );

static stlStatus smdmisInsertKey( smdmisTableHeader *aHeader,
                                  smdmisIndexPath   *aTraverseStack,
                                  stlInt32           aValueIdx,
                                  knlValueBlockList *aCols,
                                  smdmisIndexNode   *aNode,
                                  stlInt32           aInsertPos,
                                  smdmifFixedRow     aRow,
                                  knlLogicalAddr     aRowAddr,
                                  smlEnv            *aEnv );

static stlStatus smdmisInitMergeEnv( smlStatement       *aStatement,
                                     smdmisMergeEnv     *aMergeEnv,
                                     stlInt32            aLeafRunCount,
                                     stlBool             aVolatileMode,
                                     smlEnv             *aEnv );

static void smdmisResetMergeRunLink( smdmisMergeEnv *aMergeEnv );

static inline smdmisKeyRow smdmisLeafRunCurKey( smdmisTableHeader *aHeader,
                                                stlBool            aVolatileMode,
                                                smdmisLeafRun     *aRun );

static stlStatus smdmisAddSortedRun( smdmisTableHeader   *aHeader,
                                     smdmisMergeEnv      *aMergeEnv,
                                     smdmisIndexNode     *aFirstNode,
                                     smdmisRunStackList **aSortedListPtr,
                                     smlEnv              *aEnv );

static stlStatus smdmisMerge( smdmisTableHeader   *aHeader,
                              smdmisMergeEnv      *aMergeEnv,
                              smdmisRunStackList **aSortedListPtr,
                              smlEnv              *aEnv );

static stlStatus smdmisMergeAll( smdmisTableHeader *aHeader,
                                 smdmisMergeEnv    *aMergeEnv,
                                 smlEnv            *aEnv );

static stlStatus smdmisSortByMSDRadix( smdmisTableHeader *aHeader,
                                       smdmifTableHeader *aBaseHeader,
                                       smdmisIndexNode  **aSortedNodes,
                                       smlEnv            *aEnv );

static stlStatus smdmisSortByLSDRadix( smdmisTableHeader *aHeader,
                                       smdmifTableHeader *aBaseHeader,
                                       smdmisIndexNode  **aSortedNodes,
                                       smlEnv            *aEnv );

static stlStatus smdmisMakeRadixEnv( smdmisTableHeader *aHeader, 
                                     smlEnv            *aEnv );

static stlStatus smdmisSortByMerge( smlStatement      *aStatement,
                                    smdmisTableHeader *aHeader,
                                    smdmifTableHeader *aBaseHeader,
                                    smdmisIndexNode  **aSortedNodes,
                                    smlEnv            *aEnv );

static stlStatus smdmisRadixSort( smdmisTableHeader *aHeader,
                                  smdmisRadixBucket *aBucket,
                                  stlUInt16          aBucketSize,
                                  smdmisRadixNode  **aRadixList,
                                  smdmisIndexNode  **aSortDone,
                                  smlEnv            *aEnv );

static stlStatus smdmisMakeNodeListFromBucket( smdmisTableHeader *aHeader,
                                               smdmisRadixBucket *aBucket,
                                               stlInt32           aBucketSize,
                                               smdmisRadixNode  **aResultNodeListFirst,
                                               smdmisRadixNode  **aResultNodeListLast,
                                               smdmisIndexNode  **aSortDone,
                                               smlEnv            *aEnv );

static void smdmisRadixNodeSort( smdmisTableHeader *aHeader,
                                 smdmisRadixNode   *aNode,
                                 stlUInt16          aRadixIndex );

static stlStatus smdmisRadixToIndex( smdmisTableHeader *aHeader,
                                     smdmisRadixNode   *aSourceNode,
                                     stlBool            aSetDup,
                                     smdmisIndexNode  **aTargetNode,
                                     smlEnv            *aEnv );

static stlStatus smdmisBuildInternalNodes( smdmisTableHeader *aHeader,
                                           smlEnv            *aEnv );

static stlStatus smdmisBuildLeafNodes( smdmisTableHeader *aHeader,
                                       smdmifTableHeader *aBaseHeader,
                                       smdmisIndexNode  **aLeafList,
                                       smlEnv            *aEnv );

static dtlCompareResult smdmisCompareOneInteger( smdmisTableHeader *aHeader,
                                                 stlChar           *aLeftRow,
                                                 stlChar           *aRightRow );

static dtlCompareResult smdmisCompareOneNumeric( smdmisTableHeader *aHeader,
                                                 stlChar           *aLeftRow,
                                                 stlChar           *aRightRow );

static dtlCompareResult smdmisCompareOneVarchar( smdmisTableHeader *aHeader,
                                                 stlChar           *aLeftRow,
                                                 stlChar           *aRightRow );

static dtlCompareResult smdmisCompareOneCol( smdmisTableHeader *aHeader,
                                             stlChar           *aLeftRow,
                                             stlChar           *aRightRow );

static dtlCompareResult smdmisCompareOneIntegerNotNull( smdmisTableHeader *aHeader,
                                                        stlChar           *aLeftRow,
                                                        stlChar           *aRightRow );

static dtlCompareResult smdmisCompareOneNumericNotNull( smdmisTableHeader *aHeader,
                                                        stlChar           *aLeftRow,
                                                        stlChar           *aRightRow );

static dtlCompareResult smdmisCompareOneVarcharNotNull( smdmisTableHeader *aHeader,
                                                        stlChar           *aLeftRow,
                                                        stlChar           *aRightRow );

static dtlCompareResult smdmisCompareOneColNotNull( smdmisTableHeader *aHeader,
                                                    stlChar           *aLeftRow,
                                                    stlChar           *aRightRow );

static dtlCompareResult smdmisCompareKeyByRadix( smdmisTableHeader *aHeader,
                                                 stlChar           *aLeftRow,
                                                 stlChar           *aRightRow );

static inline dtlCompareResult smdmisCompareKeyAndValue( smdmisTableHeader *aHeader,
                                                         stlChar           *aKeyRow,
                                                         stlInt32           aValueIdx,
                                                         knlValueBlockList *aCols );

static void smdmisSort( smdmisTableHeader *aHeader,
                        stlChar           *aRowBase,
                        smdmisSortItem    *aSortItems,
                        stlInt32           aRowCount );

static stlStatus smdmisVerifyTree( smdmisTableHeader *aHeader,
                                   smdmisIndexNode   *aNode );

static stlStatus smdmisVerifyNode( smdmisTableHeader *aHeader,
                                   smdmisIndexNode   *aNode );

static stlStatus smdmisVerifyOrderOfNode( smdmisTableHeader *aHeader,
                                          smdmisIndexNode   *aNode );

/**
 * @brief Sort Instant Table을 생성한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aTbsId 생성할 테이블이 저장될 테이블스페이스. temp tablespace여야 한다.
 * @param[in] aAttr sort instant table 속성 정의
 * @param[in] aBaseTableHandle 기존 인스턴트 테이블에 대해 secondary index sort를 만들 때 사용된다.
 * @param[in] aKeyColCount key column 개수
 * @param[in] aKeyColList key column 리스트
 * @param[in] aKeyColFlags key column info
 * @param[out] aRelationId 생성된 인스턴트 테이블 ID: 반환하지 않음
 * @param[out] aRelationHandle 생성된 인스턴트 테이블 핸들.
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmisTableCreate( smlStatement        *aStatement,
                             smlTbsId             aTbsId,
                             smlSortTableAttr    *aAttr,
                             void                *aBaseTableHandle,
                             stlUInt16            aKeyColCount,
                             knlValueBlockList   *aKeyColList,
                             stlUInt8            *aKeyColFlags,
                             stlInt64            *aRelationId,
                             void               **aRelationHandle,
                             smlEnv              *aEnv )
{
    smdmisTableHeader *sHeader = NULL;
    smdmifTableType    sSortType;
    stlInt32           sState = 0;

    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmisTableHeader ),
                                 (void**)&sHeader,
                                 aEnv ) == STL_SUCCESS );
    sState = 1;
    
    if( aAttr->mTopDown == STL_TRUE )
    {
        sSortType = SMDMIF_TABLE_TYPE_INDEX_SORT;
    }
    else if( aAttr->mVolatile == STL_TRUE )
    {
        sSortType = SMDMIF_TABLE_TYPE_VOLATILE_SORT;
    }
    else if( aAttr->mLeafOnly == STL_TRUE )
    {
        sSortType = SMDMIF_TABLE_TYPE_ORDERED_SORT;
    }
    else
    {
        sSortType = SMDMIF_TABLE_TYPE_MERGE_SORT;
    }

    STL_TRY( smdmifTableInitHeader( &sHeader->mBaseHeader,
                                    aTbsId,
                                    SML_MEMORY_INSTANT_SORT_TABLE,
                                    sSortType,
                                    STL_FALSE, /* scrollable 기능은 인덱스에서 구현하므로 base table은 무조건 false */
                                    STL_SIZEOF( smdmisTableHeader ),
                                    aEnv ) == STL_SUCCESS );

    sHeader->mAttr              = *aAttr;
    sHeader->mKeyCols           = NULL;
    sHeader->mColCombineMem1    = NULL;
    sHeader->mColCombineMem2    = NULL;
    sHeader->mRadixArray        = NULL;
    sHeader->mKeyColInfo        = NULL;
    sHeader->mKeyColCount       = 0;
    sHeader->mRadixIndexFence   = 0;
    sHeader->mRootNode          = KNL_LOGICAL_ADDR_NULL;
    sHeader->mFirstLeafNode     = KNL_LOGICAL_ADDR_NULL;
    sHeader->mUnorderedLeafList = KNL_LOGICAL_ADDR_NULL;
    sHeader->mFreeNode          = KNL_LOGICAL_ADDR_NULL;
    sHeader->mKeyRowLen         = 0;
    sHeader->mCurNodeBlock      = KNL_LOGICAL_ADDR_NULL;
    sHeader->mNodeSize          = 0;
    sHeader->mCurBlockFreeSize  = 0;

    STL_TRY( smdAddHandleToSession( aStatement,
                                    (smeRelationHeader*)sHeader,
                                    aEnv ) == STL_SUCCESS );
    sState = 2;

    smdmisInitKeyColInfo( sHeader,
                          aKeyColCount,
                          aKeyColList,
                          aKeyColFlags,
                          aEnv );

    if( aBaseTableHandle != NULL )
    {
        /* [coverage]
         * 윈도우 query가 되기 전에는 secondary index를 만들 일이 없다.
         */
        /* 기존 인스턴트 테이블에 대해 secondary index를 빌드한다.
           bottom-up build를 수행한다. */
        STL_TRY( smdmisTableBuildIndex( aStatement,
                                        sHeader,
                                        (smdmifTableHeader *)aBaseTableHandle,
                                        aEnv ) == STL_SUCCESS );
    }

    *aRelationHandle = sHeader;

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 2:
            (void) smdmisTableDrop( aStatement, (void*)sHeader, aEnv );
            break;
        case 1:
            (void) smgFreeSessShmMem( sHeader, aEnv );
        default:
            break;
    }

    return STL_FAILURE;
}

static void smdmisSetCompareFunc( smdmisTableHeader *aHeader )
{
    aHeader->mCompareKey = smdmisCompareKey;

    if( (aHeader->mKeyColCount == 1) && (aHeader->mKeyColInfo[0].mAscending == STL_TRUE) )
    {
        aHeader->mFirstKeyOffset = aHeader->mKeyCols[0].mOffset;

        if( aHeader->mKeyCols[0].mIsFixedPart == STL_TRUE )
        {
            if( aHeader->mKeyColInfo[0].mNotNull == STL_TRUE )
            {
                /* [coverage]
                 * 현재 mNotNull이 세팅되는 경우는 in-key range 탈 때밖에 없는데,
                 * 이 경우 top-down으로 빌드되기 때문에 이 코드를 탈 일이 없다.
                 * 하지만 추후 다른 질의 처리에서 mNotNull 조건이 올 수 있으므로 놔둔다.
                 */
                if( aHeader->mKeyCols[0].mDataType == DTL_TYPE_NATIVE_INTEGER )
                {
                    aHeader->mCompareKey = smdmisCompareOneIntegerNotNull;
                }
                else if( ( aHeader->mKeyCols[0].mDataType == DTL_TYPE_FLOAT ) ||
                         ( aHeader->mKeyCols[0].mDataType == DTL_TYPE_NUMBER ) )
                {
                    aHeader->mCompareKey = smdmisCompareOneNumericNotNull;
                }
                else if( aHeader->mKeyCols[0].mDataType == DTL_TYPE_VARCHAR )
                {
                    aHeader->mCompareKey = smdmisCompareOneVarcharNotNull;
                }
                else
                {
                    aHeader->mCompareKey = smdmisCompareOneColNotNull;
                }
            }
            else
            {
                if( aHeader->mKeyCols[0].mDataType == DTL_TYPE_NATIVE_INTEGER )
                {
                    aHeader->mCompareKey = smdmisCompareOneInteger;
                }
                else if( ( aHeader->mKeyCols[0].mDataType == DTL_TYPE_FLOAT ) ||
                         ( aHeader->mKeyCols[0].mDataType == DTL_TYPE_NUMBER ) )
                {
                    aHeader->mCompareKey = smdmisCompareOneNumeric;
                }
                else if( aHeader->mKeyCols[0].mDataType == DTL_TYPE_VARCHAR )
                {
                    aHeader->mCompareKey = smdmisCompareOneVarchar;
                }
                else
                {
                    aHeader->mCompareKey = smdmisCompareOneCol;
                }
            }
        }
    }
}

static stlStatus smdmisInitKeyColInfo( smdmisTableHeader  *aHeader,
                                       stlUInt16           aKeyColCount,
                                       knlValueBlockList  *aKeyColList,
                                       stlUInt8           *aKeyColFlags,
                                       smlEnv             *aEnv )
{
    dtlDataType sDataType;
    stlInt32    i;

    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmisKeyColumn ) * aKeyColCount,
                                     (void**)&aHeader->mKeyColInfo,
                                     aEnv ) == STL_SUCCESS );

    aHeader->mKeyColCount = aKeyColCount;
    for( i = 0; i < aKeyColCount; i++ )
    {
        sDataType = KNL_GET_BLOCK_DB_TYPE( aKeyColList );
        aHeader->mKeyColInfo[i].mColOrder        = KNL_GET_BLOCK_COLUMN_ORDER( aKeyColList );
        aHeader->mKeyColInfo[i].mAscending       = ( aKeyColFlags[i] & DTL_KEYCOLUMN_INDEX_ORDER_MASK ) ==
                                                   DTL_KEYCOLUMN_INDEX_ORDER_ASC ? STL_TRUE : STL_FALSE;
        aHeader->mKeyColInfo[i].mNotNull         = ( aKeyColFlags[i] & DTL_KEYCOLUMN_INDEX_NULLABLE_MASK ) ==
                                                   DTL_KEYCOLUMN_INDEX_NULLABLE_FALSE ? STL_TRUE : STL_FALSE;
        aHeader->mKeyColInfo[i].mNullOrderLast   = ( aKeyColFlags[i] & DTL_KEYCOLUMN_INDEX_NULL_ORDER_MASK ) ==
                                                   DTL_KEYCOLUMN_INDEX_NULL_ORDER_LAST ? STL_TRUE : STL_FALSE;
        aHeader->mKeyColInfo[i].mPhysicalCompare = dtlPhysicalCompareFuncList[sDataType][sDataType];
        aHeader->mKeyColInfo[i].mModifyKeyVal    = NULL;
        aHeader->mKeyColInfo[i].mRecoverKeyVal   = NULL;

        aKeyColList = aKeyColList->mNext;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블을 drop한다. 메모리 자원을 모두 해제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmisTableDrop( smlStatement *aStatement,
                           void         *aRelationHandle,
                           smlEnv       *aEnv )
{
    smdmisTableHeader *sHeader = (smdmisTableHeader *)aRelationHandle;

    STL_TRY( smdmifTableFiniHeader( &sHeader->mBaseHeader,
                                    aEnv ) == STL_SUCCESS );

    if( sHeader->mRadixArray != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHeader->mRadixArray,
                                    aEnv ) == STL_SUCCESS );
        sHeader->mRadixArray = NULL;
    }

    if( sHeader->mColCombineMem1 != NULL )
    {
        STL_TRY( knlFreeLongVaryingMem( sHeader->mColCombineMem1,
                                        KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sHeader->mColCombineMem1 = NULL;
    }

    if( sHeader->mColCombineMem2 != NULL )
    {
        STL_TRY( knlFreeLongVaryingMem( sHeader->mColCombineMem2,
                                        KNL_ENV(aEnv) )
                 == STL_SUCCESS );
        sHeader->mColCombineMem2 = NULL;
    }

    if( sHeader->mKeyColInfo != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHeader->mKeyColInfo,
                                    aEnv ) == STL_SUCCESS );
        sHeader->mKeyColInfo = NULL;
    }

    if( sHeader->mKeyCols != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHeader->mKeyCols,
                                    aEnv ) == STL_SUCCESS );
        sHeader->mKeyCols = NULL;
    }

    STL_TRY( smdRemoveHandleFromSession( aStatement,
                                         (smeRelationHeader*)sHeader,
                                         aEnv ) == STL_SUCCESS );

    STL_TRY( smgFreeSessShmMem( sHeader,
                                aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisInitKeyCols( smdmisTableHeader *aHeader,
                                    smdmifTableHeader *aBaseHeader,
                                    stlBool            aBottomUp,
                                    smlEnv            *aEnv )
{
    stlInt32            i;
    stlInt32            sKeyRowLen;
    stlInt32            sColLen;
    smdmifDefColumn    *sCol;
    stlBool             sHasVarCol = STL_FALSE;
    stlInt32            sFixedKeyMaxLen;
    stlInt32            sKeyCountPerNode;
    stlInt32            sNodeCountPerBlock;
    stlInt32            sNodeSize;

    /**
     * 단계1: mKeyColInfo 공간을 할당한다.
     */
    STL_TRY( smgAllocSessShmMem( STL_SIZEOF( smdmifDefColumn ) * aHeader->mKeyColCount,
                                 (void**)&aHeader->mKeyCols,
                                 aEnv ) == STL_SUCCESS );

    /**
     * 단계2: key column의 fixed/var type 결정하고, offset을 설정한다.
     */
    sFixedKeyMaxLen = (stlInt32)knlGetPropertyBigIntValueByID( KNL_PROPERTY_INST_TABLE_FIXED_COL_MAX_LEN,
                                                               KNL_ENV( aEnv ) );

    sKeyRowLen = SMDMIS_KEY_ROW_HEADER_SIZE + STL_SIZEOF(knlLogicalAddr);

    for( i = 0; i < aHeader->mKeyColCount; i++ )
    {
        sCol = &aBaseHeader->mCols[aHeader->mKeyColInfo[i].mColOrder];

        aHeader->mKeyCols[i].mDataType = sCol->mDataType;
        if( aBottomUp == STL_TRUE )
        {
            sColLen = sCol->mMaxActualLen;
        }
        else
        {
            sColLen = sCol->mAllocSize;
        }
        aHeader->mKeyCols[i].mMaxActualLen = sColLen;
        aHeader->mKeyCols[i].mAllocSize = sColLen == 0 ? sCol->mAllocSize : sColLen;
        aHeader->mKeyCols[i].mIsFixedPart = sColLen <= sFixedKeyMaxLen ? STL_TRUE : STL_FALSE;

        STL_DASSERT( (sCol->mDataType != DTL_TYPE_LONGVARCHAR) &&
                     (sCol->mDataType != DTL_TYPE_LONGVARBINARY) );

        if( aHeader->mAttr.mLeafOnly == STL_TRUE )
        {
            aHeader->mKeyCols[i].mHasNull = sCol->mHasNull;
        }
        else
        {
            /* internal node를 만들겠다는 의미는 나중에 range scan을 할 것이라는 의미이므로,
               table에는 null이 없더라도 null과 비교를 할 수 있기 때문에
               null이 있다고 세팅한다. */
            aHeader->mKeyCols[i].mHasNull = STL_TRUE;
        }

        if( aHeader->mKeyCols[i].mIsFixedPart == STL_FALSE )
        {
            /* key column이 var part이면 var row ptr을 저장할 공간을 마련한다. */
            aHeader->mKeyCols[i].mOffset = sKeyRowLen;
            sKeyRowLen += STL_SIZEOF( knlLogicalAddr );
            sHasVarCol = STL_TRUE;
        }
    }

    /**
     * 단계3: key column 중 fixed part 컬럼들에 대해 offset을 설정한다.
     */
    for( i = 0; i < aHeader->mKeyColCount; i++ )
    {
        if( aHeader->mKeyCols[i].mIsFixedPart == STL_TRUE )
        {
            /* null을 하나라도 가진 컬럼이면 null 유무를 저장할 한 바이트를 (column offset 바로 앞에) 추가한다. */
            if( (aHeader->mKeyCols[i].mHasNull == STL_TRUE) &&
                (aHeader->mKeyCols[i].mDataType != DTL_TYPE_FLOAT) &&
                (aHeader->mKeyCols[i].mDataType != DTL_TYPE_NUMBER) )
            {
                /* null 데이터가 오면 offset - 1 위치에 null 값을 표시한다.
                   col len이 2바이트 align되므로 null 바이트를 위해 2바이트가 추가될 수도 있다.
                   한 바이트가 추가되던, 2바이트가 추가되던 offset - 1 위치에 null 유무를 표시한다.
                   top-down 빌드시에는 mHasNull이 세팅이 안되어 있으므로 null 바이트를 추가하지 않는다.
                   그리고, numeric은 sort 성능을 위해서 null 바이트를 추가하지 않는데,
                   numeric의 경우엔 첫바이트에 null을 위한 값을 할당할 수 있기 때문에 null 바이트가 필요없다. */
                sKeyRowLen++;
            }

            /* key row header를 뺀 위치가 align 맞아야 한다. */
            aHeader->mKeyCols[i].mOffset = STL_ALIGN( sKeyRowLen - SMDMIS_KEY_ROW_HEADER_SIZE, SMDMIS_KEY_COL_ALIGN ) +
                                           SMDMIS_KEY_ROW_HEADER_SIZE;

            sKeyRowLen = aHeader->mKeyCols[i].mOffset + SMDMIS_KEY_COL_SIZE(aHeader->mKeyCols[i].mAllocSize);
        }
    }

    STL_TRY_THROW( sKeyRowLen <= STL_UINT16_MAX, RAMP_ERR_KEY_ROW_TOO_LARGE );

    aHeader->mKeyRowLen = STL_ALIGN( sKeyRowLen, STL_SIZEOF(knlLogicalAddr) );

    sNodeCountPerBlock = knlGetPropertyBigIntValueByID( KNL_PROPERTY_INST_TABLE_NODE_COUNT_PER_BLOCK,
                                                        KNL_ENV( aEnv ) );

    sNodeSize = STL_ALIGN( SMDMIS_TABLE_USABLE_BLOCK_SIZE(aHeader) / sNodeCountPerBlock - 8, 8 );

    sKeyCountPerNode = smdmisCalculateKeyCount( sNodeSize,
                                                STL_OFFSETOF( smdmisIndexNode, mSortItems ),
                                                aHeader->mKeyRowLen,
                                                NULL );

    /* 최소한 한 노드에 key가 두개는 들어갈 수 있어야 한다. */
    while( sKeyCountPerNode < 2 )
    {
        /* fixed row가 너무 길어 한 노드안에 2개 이상 들어갈 수 없을 경우
           node size를 재조정한다. */

        /* 노드 크기를 늘려서 다시 구한다. */
        sNodeCountPerBlock = sNodeCountPerBlock / 2;

        /* 노드가 offset(2바이트)의 제한때문에 65536바이트보다 클 수 없다.
           노드가 64K일 때 블럭하나에 4개가 만들어질 수 있으므로
           블럭하나당 최소 4개 이상의 노드는 만들어져야 한다.
           이 값은 INST_TABLE_INDEX_NODE_COUNT_PER_BLOCK 프로퍼티의 min 값이기도 하다. */
        STL_TRY_THROW( sNodeCountPerBlock >= 4, RAMP_ERR_KEY_ROW_TOO_LARGE );

        sNodeSize = STL_ALIGN( SMDMIS_TABLE_USABLE_BLOCK_SIZE(aHeader) / sNodeCountPerBlock - 8, 8 );

        sKeyCountPerNode = smdmisCalculateKeyCount( sNodeSize,
                                                    STL_OFFSETOF( smdmisIndexNode, mSortItems ),
                                                    aHeader->mKeyRowLen,
                                                    NULL );
    }

    /* node size를 설정한다. */
    aHeader->mNodeSize = sNodeSize;

    /**
     * 단계4: combine memory를 할당한다.
     */
    if( sHasVarCol == STL_TRUE )
    {
        STL_TRY( knlAllocLongVaryingMem( aBaseHeader->mLargestColSize,
                                         (void**)&aHeader->mColCombineMem1,
                                         KNL_ENV(aEnv) )
                 == STL_SUCCESS );

        STL_TRY( knlAllocLongVaryingMem( aBaseHeader->mLargestColSize,
                                         (void**)&aHeader->mColCombineMem2,
                                         KNL_ENV(aEnv) )
                 == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_KEY_ROW_TOO_LARGE )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SORT_TABLE_KEY_ROW_TOO_LARGE,
                      NULL,
                      KNL_ERROR_STACK( aEnv ),
                      gPhaseString[KNL_STARTUP_PHASE_MOUNT] );
    }

    STL_FINISH;

    return STL_FAILURE;
}

static stlInt32 smdmisCalculateNodeCount( smdmisTableHeader *aHeader,
                                          smdmifTableHeader *aBaseHeader )
{
    stlInt32 sKeyCountPerNode;

    sKeyCountPerNode = smdmisCalculateKeyCount( aHeader->mNodeSize,
                                                STL_OFFSETOF( smdmisIndexNode, mSortItems ),
                                                aHeader->mKeyRowLen,
                                                NULL );

    return ( SMDMIF_TABLE_GET_TOTAL_ROW_COUNT(aBaseHeader) + sKeyCountPerNode - 1 ) / 
           sKeyCountPerNode; 
}

static stlInt32 smdmisCalculateKeyCount( stlInt32   aNodeSize,
                                         stlInt32   aNodeHeaderSize,
                                         stlInt32   aKeyRowLen,
                                         stlUInt16 *aFirstOffset )
{
    stlInt32  sOffsetLen;
    stlInt32  sEstimatedMaxKeys;
    stlUInt16 sFirstOffset = 0;

    sOffsetLen = STL_SIZEOF( smdmisSortItem );
    sEstimatedMaxKeys = ( aNodeSize - aNodeHeaderSize ) / ( sOffsetLen + aKeyRowLen );

    for( ; sEstimatedMaxKeys > 0; sEstimatedMaxKeys-- )
    {
        sFirstOffset = STL_ALIGN( aNodeHeaderSize + sOffsetLen * sEstimatedMaxKeys + SMDMIS_KEY_ROW_HEADER_SIZE,
                                  STL_SIZEOF(knlLogicalAddr) ) - SMDMIS_KEY_ROW_HEADER_SIZE;

        if( sFirstOffset + aKeyRowLen * sEstimatedMaxKeys <= aNodeSize )
        {
            break;
        }
    }

    if( aFirstOffset != NULL )
    {
        *aFirstOffset = sFirstOffset;
    }

    return sEstimatedMaxKeys;
}

static void smdmisIndexNodeInit( smdmisIndexNode *aNode,
                                 stlInt16         aLevel,
                                 stlInt32         aNodeSize,
                                 stlInt32         aKeyRowLen )
{
    aNode->mMaxKeyCount = smdmisCalculateKeyCount( aNodeSize,
                                                   STL_OFFSETOF( smdmisIndexNode, mSortItems ),
                                                   aKeyRowLen,
                                                   &aNode->mFirstKeyOffset );

    aNode->mNext           = KNL_LOGICAL_ADDR_NULL;
    aNode->mLeftMost       = KNL_LOGICAL_ADDR_NULL;
    aNode->mLevel          = aLevel;

    SMDMIS_NODE_CLEAR( aNode );
}

static void smdmisRadixNodeInit( smdmisRadixNode *aNode,
                                 stlUInt16        aUsedRadixIndex,
                                 stlInt32         aNodeSize,
                                 stlInt32         aKeyRowLen )
{
    aNode->mNextOverflow = NULL;
    aNode->mNextGreater = NULL;
    aNode->mUsedRadixIndex = aUsedRadixIndex;
    aNode->mMaxKeyCount = smdmisCalculateKeyCount( aNodeSize,
                                                   STL_OFFSETOF( smdmisRadixNode, mSortItems ),
                                                   aKeyRowLen,
                                                   &aNode->mFirstKeyOffset );
    SMDMIS_NODE_CLEAR( aNode );
}

static void smdmisFreeIndexNode( smdmisTableHeader *aHeader,
                                 smdmisIndexNode   *aNode )
{
    aNode->mNext = aHeader->mFreeNode;
    aHeader->mFreeNode = aNode->mMyAddr;
}

static void smdmisFreeRadixNode( smdmisTableHeader *aHeader,
                                 smdmisRadixNode   *aNode )
{
    aNode->mNextOverflow = (smdmisRadixNode *)aHeader->mFreeNode;
    aHeader->mFreeNode = aNode->mMyAddr;
}

static stlStatus smdmisAllocIndexNode( smdmisTableHeader *aHeader,
                                       smdmisIndexNode  **aNewNode,
                                       smlEnv            *aEnv )
{
    stlChar   *sBlock = SMDMIS_TABLE_CUR_NODE_BLOCK( aHeader );

    if( aHeader->mFreeNode != KNL_LOGICAL_ADDR_NULL )
    {
        *aNewNode = SMDMIS_TO_NODE_PTR( aHeader->mFreeNode );
        aHeader->mFreeNode = (*aNewNode)->mNext;
    }
    else
    {
        if( sBlock == NULL || aHeader->mCurBlockFreeSize < aHeader->mNodeSize )
        {
            STL_TRY( smdmifTableAllocBlock( &aHeader->mBaseHeader,
                                            SMDMIF_DEF_BLOCK_INDEX_NODE,
                                            (void **)&sBlock,
                                            aEnv ) == STL_SUCCESS );

            aHeader->mCurNodeBlock = SMDMIF_DEF_BLOCK_ADDR( sBlock );
            aHeader->mCurBlockFreeSize = SMDMIS_TABLE_USABLE_BLOCK_SIZE( aHeader );
        }

        *aNewNode = (smdmisIndexNode *)(sBlock + aHeader->mBaseHeader.mBlockSize - aHeader->mCurBlockFreeSize);
        (*aNewNode)->mMyAddr = aHeader->mCurNodeBlock + aHeader->mBaseHeader.mBlockSize - aHeader->mCurBlockFreeSize;

        aHeader->mCurBlockFreeSize -= aHeader->mNodeSize;

        SMDMIF_DEF_BLOCK_ITEM_COUNT(sBlock)++;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisAllocRadixNode( smdmisTableHeader *aHeader,
                                       smdmisRadixNode  **aNewNode,
                                       smlEnv            *aEnv )
{
    stlChar         *sBlock = SMDMIS_TABLE_CUR_NODE_BLOCK( aHeader );
    smdmisIndexNode *sIndexNode;

    if( aHeader->mFreeNode != KNL_LOGICAL_ADDR_NULL )
    {
        *aNewNode = SMDMIS_TO_RADIX_NODE_PTR( aHeader->mFreeNode );
        sIndexNode = (smdmisIndexNode *)(*aNewNode);
        aHeader->mFreeNode = sIndexNode->mNext;
    }
    else
    {
        if( sBlock == NULL || aHeader->mCurBlockFreeSize < aHeader->mNodeSize )
        {
            STL_TRY( smdmifTableAllocBlock( &aHeader->mBaseHeader,
                                            SMDMIF_DEF_BLOCK_INDEX_NODE,
                                            (void **)&sBlock,
                                            aEnv ) == STL_SUCCESS );

            aHeader->mCurNodeBlock = SMDMIF_DEF_BLOCK_ADDR( sBlock );
            aHeader->mCurBlockFreeSize = SMDMIS_TABLE_USABLE_BLOCK_SIZE( aHeader );
        }

        *aNewNode = (smdmisRadixNode *)(sBlock + aHeader->mBaseHeader.mBlockSize - aHeader->mCurBlockFreeSize);
        (*aNewNode)->mMyAddr = aHeader->mCurNodeBlock + aHeader->mBaseHeader.mBlockSize - aHeader->mCurBlockFreeSize;

        aHeader->mCurBlockFreeSize -= aHeader->mNodeSize;

        SMDMIF_DEF_BLOCK_ITEM_COUNT(sBlock)++;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisAllocKeyRow( smdmisTableHeader *aHeader,
                                    smdmisIndexPath   *aTraverseStack,
                                    smdmisIndexNode   *aNode,
                                    stlInt32           aIndex,
                                    smdmisKeyRow      *aKeyRow,
                                    smlEnv            *aEnv )
{
    stlInt32          i;
    smdmisIndexNode  *sNextNode;

    if( SMDMIS_NODE_IS_FULL(aNode) == STL_TRUE )
    {
        STL_TRY( smdmisSplitNode( aHeader,
                                  aTraverseStack,
                                  aNode,
                                  &sNextNode,
                                  aEnv ) == STL_SUCCESS );

        if( aIndex > aNode->mKeyCount )
        {
            aIndex -= aNode->mKeyCount;
            aNode = sNextNode;
        }
    }

    for( i = aNode->mKeyCount; i > aIndex; i-- )
    {
        aNode->mSortItems[i] = aNode->mSortItems[i-1];
    }

    SMDMIS_NODE_ALLOC_KEY_AT( aNode,
                              i,
                              *aKeyRow,
                              aHeader->mKeyRowLen );

    SMDMIS_KEY_ROW_INIT_HEADER( *aKeyRow );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisFindKey( smdmisTableHeader *aHeader,
                                stlInt32           aValueIdx,
                                knlValueBlockList *aCols,
                                smdmisKeyRow      *aFoundKeyRow,
                                smdmisIndexNode  **aFoundNode,
                                stlInt32          *aFoundPos,
                                smdmisIndexPath   *aTraverseStack,
                                smlEnv            *aEnv )
{
    smdmisKeyRow     sKeyRow = NULL; /* node에 row가 하나도 없으면 NULL row이다. */
    smdmisIndexNode *sNode;
    dtlCompareResult sCom = DTL_COMPARISON_EQUAL;
    stlInt32         sMin;
    stlInt32         sMax;
    stlInt32         sMid = 0; /* 최초 row가 insert될 때 0번 위치에 저장된다. */

    if( aHeader->mRootNode == KNL_LOGICAL_ADDR_NULL )
    {
        STL_TRY( smdmisAllocIndexNode( aHeader,
                                       &sNode,
                                       aEnv ) == STL_SUCCESS );

        /* 최초 노드 생성: leaf 노드가 된다. */
        smdmisIndexNodeInit( sNode,
                             0, /* this is leaf node */
                             aHeader->mNodeSize,
                             aHeader->mKeyRowLen );

        aHeader->mRootNode      = sNode->mMyAddr;
        aHeader->mFirstLeafNode = sNode->mMyAddr;
    }
    else
    {
        sNode = SMDMIS_TO_NODE_PTR( aHeader->mRootNode );
    }

    STL_TRY_THROW( sNode->mLevel < SMDMIS_INDEX_DEPTH_MAX,
                   RAMP_ERR_INDEX_DEPTH_OVERFLOW );

    /* root node는 parent node가 없으므로 left most로 traverse했다고 생각할 수 있다.
       이 값은 실제로 root가 새로 생성될 때 insert position으로 사용된다. */
    SMDMIS_INDEX_PATH_SET( &aTraverseStack[sNode->mLevel], NULL, -1 );

    while( STL_TRUE )
    {
        sMin = 0;
        sMax = sNode->mKeyCount - 1;

        while( sMin <= sMax )
        {
            sMid = (sMin + sMax) >> 1;
            sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sMid );

            STL_DASSERT( sKeyRow != NULL );

            sCom = smdmisCompareKeyAndValue( aHeader,
                                             sKeyRow,
                                             aValueIdx,
                                             aCols );

            if( sCom == DTL_COMPARISON_LESS )
            {
                sMin = sMid + 1;
            }
            else if( sCom == DTL_COMPARISON_GREATER )
            {
                sMax = sMid - 1;
            }
            else
            {
                break;
            }
        }

        if( sCom == DTL_COMPARISON_GREATER )
        {
            sMid--;
        }

        STL_DASSERT( sMid >= -1 );

        if( sNode->mLevel == 0 )
        {
            break;
        }

        /*
         * leaf node가 아닌 경우이다. child node로 이동한다.
         */

        /* save traverse path */
        SMDMIS_INDEX_PATH_SET( &aTraverseStack[sNode->mLevel-1], sNode, sMid );

        if( sMid < 0 )
        {
            sNode = SMDMIS_INDEX_NODE_LEFTMOST_NODE( sNode );
        }
        else
        {
            sNode = SMDMIS_TO_NODE_PTR( SMDMIS_KEY_ROW_GET_CHILD( SMDMIS_NODE_GET_KEY(sNode, sMid) ) );
        }
    }

    *aFoundNode = sNode;
    *aFoundPos = sMid + 1; /* insert position은 sMid 바로 다음이다. */

    if( sCom == DTL_COMPARISON_EQUAL )
    {
        *aFoundKeyRow = sKeyRow;
    }
    else
    {
        *aFoundKeyRow = NULL;
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_INDEX_DEPTH_OVERFLOW )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      SML_ERRCODE_SORT_OVERFLOW,
                      NULL,
                      KNL_ERROR_STACK( aEnv ) );
    }

    STL_FINISH;

    return STL_FAILURE;
}

static inline void smdmisCompactNode( smdmisTableHeader *aHeader,
                                      smdmisIndexNode   *aNode )
{
    stlInt32          i, j;
    stlInt32          sLimitOffset;

    i = 0;
    j = aNode->mKeyCount;
    sLimitOffset = SMDMIS_NODE_CUR_KEY_OFFSET( aNode, aHeader->mKeyRowLen );

    while( i < aNode->mKeyCount )
    {
        while( ( i < aNode->mKeyCount ) && ( aNode->mSortItems[i] < sLimitOffset ) )
        {
            i++;
        }
        if( i == aNode->mKeyCount )
        {
            break;
        }

        while( ( j < aNode->mMaxKeyCount ) && ( aNode->mSortItems[j] >= sLimitOffset ) )
        {
            j++;
        }
        /* j == aNode->mMaxKeyCount가 되려면 이미 위쪽 조건 (i == aNode->mKeyCount)를 만족해서
           while을 빠져 나갔어야 했다. */
        STL_DASSERT( j < aNode->mMaxKeyCount );

        stlMemcpy( SMDMIS_NODE_GET_KEY( aNode, j ),
                   SMDMIS_NODE_GET_KEY( aNode, i ),
                   aHeader->mKeyRowLen );

        aNode->mSortItems[i] = aNode->mSortItems[j];

        i++;
        j++;
    }
}

static stlStatus smdmisSplitNode( smdmisTableHeader *aHeader,
                                  smdmisIndexPath   *aTraverseStack,
                                  smdmisIndexNode   *aNode,
                                  smdmisIndexNode  **aNextNode,
                                  smlEnv            *aEnv )
{
    smdmisIndexNode  *sNextNode;
    smdmisIndexNode  *sParent;
    stlInt32          sMoveCount;
    smdmisKeyRow      sKeyRow;
    smdmisKeyRow      sTargetKeyRow;
    stlInt32          sTargetOffset;
    stlInt32          i;
    smdmisIndexPath  *sPath;

    /* split을 하기 위해 노드가 가득 차야 한다. */
    STL_DASSERT( aNode->mKeyCount == aNode->mMaxKeyCount );

    STL_TRY( smdmisAllocIndexNode( aHeader,
                                   &sNextNode,
                                   aEnv ) == STL_SUCCESS );

    smdmisIndexNodeInit( sNextNode,
                         aNode->mLevel,
                         aHeader->mNodeSize,
                         aHeader->mKeyRowLen );

    /**
     * link sibling
     */
    SMDMIS_INDEX_NODE_ADD_TO_LIST( aNode,
                                   sNextNode );

    /**
     * key 분배 
     */
    sMoveCount = aNode->mKeyCount >> 1;
    sTargetOffset = sNextNode->mFirstKeyOffset;
    for( i = 0; i < sMoveCount; i++ )
    {
        sNextNode->mSortItems[i] = sTargetOffset;

        sKeyRow = SMDMIS_NODE_GET_KEY( aNode, aNode->mKeyCount - sMoveCount + i );
        sTargetKeyRow = SMDMIS_NODE_GET_KEY( sNextNode, i );

        stlMemcpy( sTargetKeyRow, sKeyRow, aHeader->mKeyRowLen );

        sTargetOffset += aHeader->mKeyRowLen;
    }
    aNode->mKeyCount -= sMoveCount;
    sNextNode->mKeyCount = sMoveCount;

    /**
     * 기존 노드의 compaction
     */
    smdmisCompactNode( aHeader,
                       aNode );

    /**
     * parent 처리
     */
    sPath = &aTraverseStack[aNode->mLevel];

    sParent = SMDMIS_INDEX_PATH_PARENT( sPath );
    if( sParent == NULL )
    {
        STL_TRY( smdmisAllocIndexNode( aHeader,
                                       &sParent,
                                       aEnv ) == STL_SUCCESS );

        smdmisIndexNodeInit( sParent,
                             aNode->mLevel + 1,
                             aHeader->mNodeSize,
                             aHeader->mKeyRowLen );

        /* left most를 세팅한다. */
        SMDMIS_INDEX_NODE_SET_LEFTMOST( sParent, aNode );

        /* 새로운 root를 세팅한다. */
        aHeader->mRootNode = sParent->mMyAddr;
    }

    /**
     * parent에 key propagation
     */
    STL_TRY( smdmisAllocKeyRow( aHeader,
                                aTraverseStack,
                                sParent,
                                SMDMIS_INDEX_PATH_POS( sPath ) + 1, /* 탐색한 노드 바로 다음에 insert한다. */
                                &sTargetKeyRow,
                                aEnv ) == STL_SUCCESS );

    sKeyRow = SMDMIS_NODE_GET_KEY( sNextNode, 0 );

    stlMemcpy( sTargetKeyRow, sKeyRow, aHeader->mKeyRowLen );

    SMDMIS_KEY_ROW_WRITE_CHILD_ADDR( sTargetKeyRow, sNextNode->mMyAddr );

    *aNextNode = sNextNode;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisInsertKey( smdmisTableHeader *aHeader,
                                  smdmisIndexPath   *aTraverseStack,
                                  stlInt32           aValueIdx,
                                  knlValueBlockList *aCols,
                                  smdmisIndexNode   *aNode,
                                  stlInt32           aInsertPos,
                                  smdmifFixedRow     aRow,
                                  knlLogicalAddr     aRowAddr,
                                  smlEnv            *aEnv )
{
    smdmisKeyRow     sKeyRow;
    stlChar         *sColPos;
    stlInt32         sColLen;
    stlInt32         i;
    smdmifDefColumn *sCol;

    STL_TRY( smdmisAllocKeyRow( aHeader,
                                aTraverseStack,
                                aNode,
                                aInsertPos,
                                &sKeyRow,
                                aEnv ) == STL_SUCCESS );

    SMDMIS_KEY_ROW_WRITE_CHILD_ADDR( sKeyRow, aRowAddr );

    for( i = 0; i < aHeader->mKeyColCount; i++ )
    {
        sCol = &aHeader->mKeyCols[i];

        if( sCol->mIsFixedPart == STL_TRUE )
        {
            sColLen = KNL_GET_BLOCK_DATA_LENGTH( aCols, aValueIdx );
            sColPos = KNL_GET_BLOCK_DATA_PTR( aCols, aValueIdx );

            SMDMIS_KEY_ROW_WRITE_COL( sKeyRow,
                                      sCol,
                                      sColPos,
                                      sColLen );
        }
        else
        {
            /* var column인 경우 fixed row로부터 var col ptr을 복사한다. */
            SMDMIS_KEY_ROW_SET_VAR_COL( sKeyRow,
                                        sCol,
                                        SMDMIF_FIXED_ROW_GET_VAR_COL_ADDR( aRow,
                                          &aHeader->mBaseHeader.mCols[aHeader->mKeyColInfo[i].mColOrder] ) );
        }

        aCols = aCols->mNext;
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE; 
}

/**
 * @brief 기존 레코드가 존재하면 합병하고, 없다면 삽입한다.
 * @param[in] aStatement Statement 구조체
 * @param[in] aRelationHandle Insert할 테이블의 Relation Handle
 * @param[in] aInsertCols 삽입될 Record 들의 컬럼값 리스트의 배열
 * @param[in] aMergeRecordInfo 합병에 필요한 정보들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmisTableMerge( smlStatement       * aStatement,
                            void               * aRelationHandle,
                            knlValueBlockList  * aInsertCols,
                            smlMergeRecordInfo * aMergeRecordInfo,
                            smlEnv             * aEnv )
{
    /* [coverage]
     * group sort 기능이 생기면 이 함수가 사용될 것이다.
     * 현재로서는 아직 불일 일이 없다.
     */
    smdmisTableHeader *sHeader = (smdmisTableHeader *)aRelationHandle;
    stlInt32           sBlockIdx;
    smdmisKeyRow       sKeyRow = NULL;
    smdmisIndexNode   *sNode = NULL;
    stlInt32           sInsertPos = 0;
    smdmisIndexPath    sTraverseStack[SMDMIS_INDEX_DEPTH_MAX];
    stlBool            sFetched;
    stlBool            sVersionConflict;
    stlBool            sSkipped;
    knlLogicalAddr     sRowAddr;

    if( sHeader->mBaseHeader.mCols == NULL )
    {
        STL_TRY( smdmifTableInitColumnInfo( &sHeader->mBaseHeader,
                                            aInsertCols,
                                            aEnv ) == STL_SUCCESS );
    }

    STL_DASSERT( sHeader->mAttr.mTopDown == STL_TRUE );
    STL_DASSERT( sHeader->mAttr.mDistinct == STL_TRUE );
    
    /* top-down insert시에는 insert하기 전에 key column 정보가 먼저 세팅되어 있어야 한다. */
    if( sHeader->mKeyCols == NULL )
    {
        STL_TRY( smdmisInitKeyCols( sHeader,
                                    &sHeader->mBaseHeader,
                                    STL_FALSE,
                                    aEnv ) == STL_SUCCESS );
    }

    for( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
         sBlockIdx < KNL_GET_BLOCK_USED_CNT( aInsertCols );
         sBlockIdx++ )
    {
        STL_TRY( smdmisFindKey( sHeader,
                                sBlockIdx,
                                aInsertCols,
                                &sKeyRow,
                                &sNode,
                                &sInsertPos,
                                sTraverseStack,
                                aEnv ) == STL_SUCCESS );
            
        if( sKeyRow == NULL )
        {
            STL_TRY( smoAssignAggrFunc( aMergeRecordInfo->mAggrFuncInfoList,
                                        sBlockIdx,
                                        aEnv )
                     == STL_SUCCESS );
    
            /*
             * 해당 키가 없거나 unique table이 아니므로 insert를 수행한다.
             * table에 먼저 row를 insert한 후 key를 삽입한다.
             */
            STL_TRY( smdmifTableInsertInternal( &sHeader->mBaseHeader,
                                                aInsertCols,
                                                NULL,  /* aUniqueViolation */
                                                sBlockIdx,
                                                (smlRid *)&sRowAddr,
                                                aEnv ) == STL_SUCCESS );

            STL_TRY( smdmisInsertKey( sHeader,
                                      sTraverseStack,
                                      sBlockIdx,
                                      aInsertCols,
                                      sNode,
                                      sInsertPos,
                                      SMDMIF_GET_ROW_BY_ROW_ADDR( sRowAddr ),
                                      sRowAddr,
                                      aEnv ) == STL_SUCCESS );
        }
        else
        {
            STL_TRY( smdmifTableFetchOneRow( (smdmifTableHeader*)sHeader,
                                             SMDMIS_KEY_ROW_GET_FIXED_ROW( sKeyRow ),
                                             NULL,  /* key physical filter */
                                             NULL,  /* key logical filter */
                                             NULL,  /* key fetch columns */
                                             NULL,  /* row physical filter */
                                             NULL,  /* row filter evaluation info */
                                             aMergeRecordInfo->mReadColList,
                                             sBlockIdx,
                                             &sFetched,
                                             aEnv ) == STL_SUCCESS );
            
            STL_TRY( smoBuildAggrFunc( aMergeRecordInfo->mAggrFuncInfoList,
                                       sBlockIdx,
                                       aEnv )
                     == STL_SUCCESS );

            sRowAddr = SMDMIS_KEY_ROW_GET_FIXED_ROW_ADDR( sKeyRow );
            STL_TRY( smdmifTableUpdate( aStatement,
                                        aRelationHandle,
                                        (smlRid *)sRowAddr,
                                        0,    /* aRowScn */
                                        sBlockIdx,
                                        aMergeRecordInfo->mUpdateColList,
                                        NULL, /* aBeforeCols */
                                        NULL, /* aPrimaryKey */
                                        &sVersionConflict,
                                        &sSkipped,
                                        aEnv )
                     == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}


/**
 * @brief 주어진 relation에 새 row를 삽입한다
 * @param[in] aStatement 사용중인 statement
 * @param[in] aRelationHandle row를 삽입할 Relation의 handle
 * @param[in] aValueIdx 삽입할 row의 value block내에서의 순번
 * @param[in] aInsertCols 삽입할 row의 value block
 * @param[out] aUniqueViolation 삽입할 row가 unique check에 걸려 insert되지 않았는지의 여부
 * @param[out] aRowRid 삽입한 row의 위치
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmisTableInsert( smlStatement      *aStatement,
                             void              *aRelationHandle,
                             stlInt32           aValueIdx,
                             knlValueBlockList *aInsertCols,
                             knlValueBlockList *aUniqueViolation,
                             smlRid            *aRowRid,
                             smlEnv            *aEnv )
{
    smdmisTableHeader *sHeader = (smdmisTableHeader *)aRelationHandle;
    smdmisKeyRow       sKeyRow = NULL;
    smdmisIndexNode   *sNode = NULL;
    stlInt32           sInsertPos = 0;
    smdmisIndexPath    sTraverseStack[SMDMIS_INDEX_DEPTH_MAX];
    dtlDataValue      *sDataValue = NULL;
    knlLogicalAddr     sRowAddr;
    stlInt32           i;
    knlValueBlockList *sInsertCol;

    if( sHeader->mBaseHeader.mCols == NULL )
    {
        STL_TRY( smdmifTableInitColumnInfo( &sHeader->mBaseHeader,
                                            aInsertCols,
                                            aEnv ) == STL_SUCCESS );
    }

    /*
     * 성능을 위해 key column 중에 null이 하나라도 있으면
     * insert를 하지 않는 기능이 필요하다.
     */
    if( sHeader->mAttr.mNullExcluded == STL_TRUE )
    {
        sInsertCol = aInsertCols;
        for( i = 0; i < sHeader->mKeyColCount; i++ )
        {
            if( KNL_GET_BLOCK_DATA_LENGTH( sInsertCol, aValueIdx ) == 0 )
            {
                STL_THROW( RAMP_FINISH );
            }

            sInsertCol = sInsertCol->mNext;
        }
    }

    if( sHeader->mAttr.mTopDown == STL_FALSE )
    {
        STL_TRY( smdmifTableInsertInternal( &sHeader->mBaseHeader,
                                            aInsertCols,
                                            aUniqueViolation,
                                            aValueIdx,
                                            aRowRid,
                                            aEnv ) == STL_SUCCESS );
    }
    else
    {
        /* top-down insert시에는 insert하기 전에 key column 정보가 먼저 세팅되어 있어야 한다. */
        if( sHeader->mKeyCols == NULL )
        {
            STL_TRY( smdmisInitKeyCols( sHeader,
                                        &sHeader->mBaseHeader,
                                        STL_FALSE,
                                        aEnv ) == STL_SUCCESS );
        }

        STL_TRY( smdmisFindKey( sHeader,
                                aValueIdx,
                                aInsertCols,
                                &sKeyRow,
                                &sNode,
                                &sInsertPos,
                                sTraverseStack,
                                aEnv ) == STL_SUCCESS );

        if( sKeyRow == NULL || sHeader->mAttr.mDistinct == STL_FALSE )
        {
            /*
             * 해당 키가 없거나 unique table이 아니므로 insert를 수행한다.
             * table에 먼저 row를 insert한 후 key를 삽입한다.
             */
            STL_TRY( smdmifTableInsertInternal( &sHeader->mBaseHeader,
                                                aInsertCols,
                                                aUniqueViolation,
                                                aValueIdx,
                                                aRowRid,
                                                aEnv ) == STL_SUCCESS );

            SMDMIF_RID_TO_ROW_ADDR( &sRowAddr, aRowRid );

            STL_TRY( smdmisInsertKey( sHeader,
                                      sTraverseStack,
                                      aValueIdx,
                                      aInsertCols,
                                      sNode,
                                      sInsertPos,
                                      SMDMIF_GET_ROW_BY_ROW_ADDR( sRowAddr ),
                                      sRowAddr,
                                      aEnv ) == STL_SUCCESS );

            if( sHeader->mAttr.mDistinct == STL_TRUE )
            {
                if( aUniqueViolation != NULL )
                {
                    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aUniqueViolation, aValueIdx );
                    DTL_BOOLEAN( sDataValue ) = STL_FALSE;
                    DTL_SET_BOOLEAN_LENGTH( sDataValue );
                }
            }
        }
        else
        {
            /* 해당 키가 있고 unique table이므로 기존 row의 rid만 세팅한다. */
            STL_WRITE_INT64( aRowRid, SMDMIS_KEY_ROW_GET_FIXED_ROW_ADDR_PTR( sKeyRow ));
                
            if( aUniqueViolation != NULL )
            {
                sDataValue = KNL_GET_BLOCK_DATA_VALUE( aUniqueViolation, aValueIdx );
                DTL_BOOLEAN( sDataValue ) = STL_TRUE;
                DTL_SET_BOOLEAN_LENGTH( sDataValue );
            }
        }
    }

    STL_RAMP( RAMP_FINISH );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief 인스턴트 테이블에 레코드를 삽입한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in] aInsertCols 삽입될 레코드들의 컬럼값 리스트
 * @param[out] aUniqueViolation 삽입할 row가 unique check에 걸려 insert되지 않았는지의 여부
 * @param[in] aRowBlock 삽입된 레코드의 RID 배열 (SCN은 N/A)
 * @param[in] aEnv Environment 구조체
 */
stlStatus smdmisTableBlockInsert( smlStatement      *aStatement,
                                  void              *aRelationHandle,
                                  knlValueBlockList *aInsertCols,
                                  knlValueBlockList *aUniqueViolation,
                                  smlRowBlock       *aRowBlock,
                                  smlEnv            *aEnv )
{
    smdmisTableHeader *sHeader = (smdmisTableHeader *)aRelationHandle;
    stlInt32           sBlockIdx;
    smdmisKeyRow       sKeyRow = NULL;
    smdmisIndexNode   *sNode = NULL;
    stlInt32           sInsertPos = 0;
    smdmisIndexPath    sTraverseStack[SMDMIS_INDEX_DEPTH_MAX];
    dtlDataValue      *sDataValue = NULL;
    knlLogicalAddr     sRowAddr;
    stlInt32           i;
    knlValueBlockList *sInsertCol;
    stlBool            sInsert;

    if( sHeader->mBaseHeader.mCols == NULL )
    {
        STL_TRY( smdmifTableInitColumnInfo( &sHeader->mBaseHeader,
                                            aInsertCols,
                                            aEnv ) == STL_SUCCESS );
    }

    if( sHeader->mAttr.mTopDown == STL_FALSE )
    {
        for( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
             sBlockIdx < KNL_GET_BLOCK_USED_CNT( aInsertCols );
             sBlockIdx++ )
        {
            /*
             * 성능을 위해 key column 중에 null이 하나라도 있으면
             * insert를 하지 않는 기능이 필요하다.
             */
            sInsert = STL_TRUE;
            if( sHeader->mAttr.mNullExcluded == STL_TRUE )
            {
                /* [coverage]
                 * top-down이 아닌 경우 null excluded가 세팅돼서 오는 경우는
                 * 현재까진 없는 듯 하다.
                 * 하지만 top-down인 케이스로 테스트가 되었기 때문에 별 문제는 없을 듯
                 */
                sInsertCol = aInsertCols;
                for( i = 0; i < sHeader->mKeyColCount; i++ )
                {
                    if( KNL_GET_BLOCK_DATA_LENGTH( sInsertCol, sBlockIdx ) == 0 )
                    {
                        sInsert = STL_FALSE;
                        break;
                    }

                    sInsertCol = sInsertCol->mNext;
                }   
            }

            if( sInsert == STL_FALSE )
            {
                continue;
            }

            STL_TRY( smdmifTableInsertInternal( &sHeader->mBaseHeader,
                                                aInsertCols,
                                                aUniqueViolation,
                                                sBlockIdx,
                                                &aRowBlock->mRidBlock[sBlockIdx],
                                                aEnv ) == STL_SUCCESS );
        }
    }
    else
    {
        /* top-down insert시에는 insert하기 전에 key column 정보가 먼저 세팅되어 있어야 한다. */
        if( sHeader->mKeyCols == NULL )
        {
            STL_TRY( smdmisInitKeyCols( sHeader,
                                        &sHeader->mBaseHeader,
                                        STL_FALSE,
                                        aEnv ) == STL_SUCCESS );
        }

        for( sBlockIdx = KNL_GET_BLOCK_SKIP_CNT( aInsertCols );
             sBlockIdx < KNL_GET_BLOCK_USED_CNT( aInsertCols );
             sBlockIdx++ )
        {
            /*
             * 성능을 위해 key column 중에 null이 하나라도 있으면
             * insert를 하지 않는 기능이 필요하다.
             */
            sInsert = STL_TRUE;
            if( sHeader->mAttr.mNullExcluded == STL_TRUE )
            {
                sInsertCol = aInsertCols;
                for( i = 0; i < sHeader->mKeyColCount; i++ )
                {
                    if( KNL_GET_BLOCK_DATA_LENGTH( sInsertCol, sBlockIdx ) == 0 )
                    {
                        sInsert = STL_FALSE;
                        break;
                    }

                    sInsertCol = sInsertCol->mNext;
                }
            }

            if( sInsert == STL_FALSE )
            {
                continue;
            }

            STL_TRY( smdmisFindKey( sHeader,
                                    sBlockIdx,
                                    aInsertCols,
                                    &sKeyRow,
                                    &sNode,
                                    &sInsertPos,
                                    sTraverseStack,
                                    aEnv ) == STL_SUCCESS );
            
            if( sKeyRow == NULL || sHeader->mAttr.mDistinct == STL_FALSE )
            {
                /*
                 * 해당 키가 없거나 unique table이 아니므로 insert를 수행한다.
                 * table에 먼저 row를 insert한 후 key를 삽입한다.
                 */
                STL_TRY( smdmifTableInsertInternal( &sHeader->mBaseHeader,
                                                    aInsertCols,
                                                    aUniqueViolation,
                                                    sBlockIdx,
                                                    &aRowBlock->mRidBlock[sBlockIdx],
                                                    aEnv ) == STL_SUCCESS );

                SMDMIF_RID_TO_ROW_ADDR( &sRowAddr, &aRowBlock->mRidBlock[sBlockIdx] );

                STL_TRY( smdmisInsertKey( sHeader,
                                          sTraverseStack,
                                          sBlockIdx,
                                          aInsertCols,
                                          sNode,
                                          sInsertPos,
                                          SMDMIF_GET_ROW_BY_ROW_ADDR( sRowAddr ),
                                          sRowAddr,
                                          aEnv ) == STL_SUCCESS );

                if( sHeader->mAttr.mDistinct == STL_TRUE )
                {
                    if( aUniqueViolation != NULL )
                    {
                        sDataValue = KNL_GET_BLOCK_DATA_VALUE( aUniqueViolation, sBlockIdx );
                        DTL_BOOLEAN( sDataValue ) = STL_FALSE;
                        DTL_SET_BOOLEAN_LENGTH( sDataValue );
                    }
                }
            }
            else
            {
                /* 해당 키가 있고 unique table이므로 기존 row의 rid만 세팅한다. */
                STL_WRITE_INT64( &aRowBlock->mRidBlock[sBlockIdx], SMDMIS_KEY_ROW_GET_FIXED_ROW_ADDR_PTR( sKeyRow ) );
                
                if( aUniqueViolation != NULL )
                {
                    sDataValue = KNL_GET_BLOCK_DATA_VALUE( aUniqueViolation, sBlockIdx );
                    DTL_BOOLEAN( sDataValue ) = STL_TRUE;
                    DTL_SET_BOOLEAN_LENGTH( sDataValue );
                }
            }
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisInitMergeEnv( smlStatement       *aStatement,
                                     smdmisMergeEnv     *aMergeEnv,
                                     stlInt32            aLeafRunCount,
                                     stlBool             aVolatileMode,
                                     smlEnv             *aEnv )
{
    stlInt32        sLevel = 0;
    stlInt32        sRemain = 0;
    stlInt32        sTotalRunCount;
    stlInt32        i;
    smdmisLeafRun  *sRun;

    aMergeEnv->mLevelCount[sLevel] = aLeafRunCount;
    sTotalRunCount = aLeafRunCount;
    while( (aMergeEnv->mLevelCount[sLevel] > 1) || (sRemain > 0) )
    {
        sLevel++;
        STL_ASSERT( sLevel < SMDMIS_MERGE_LEVEL_MAX );

        aMergeEnv->mLevelCount[sLevel] = ( aMergeEnv->mLevelCount[sLevel - 1] + sRemain ) >> 1;
        sRemain = ( aMergeEnv->mLevelCount[sLevel - 1] + sRemain ) & 0x00000001;

        sTotalRunCount += aMergeEnv->mLevelCount[sLevel];
    }

    if( aVolatileMode == STL_TRUE )
    {
        /* volatile mode에서는 shared memory(iterator와 같은 메모리)에 run들을 할당해야 한다. */
        STL_TRY( smaAllocRegionMem( aStatement,
                                    SMDMIS_MERGE_RUN_SIZE * sTotalRunCount,
                                    (void**)&aMergeEnv->mLeafRunList,
                                    aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    SMDMIS_MERGE_RUN_SIZE * sTotalRunCount,
                                    (void**)&aMergeEnv->mLeafRunList,
                                    KNL_ENV( aEnv ) ) == STL_SUCCESS );
    }

    for( i = 0; i < aLeafRunCount; i++ )
    {
        sRun = &aMergeEnv->mLeafRunList[i];
        SMDMIS_LEAF_RUN_INIT( sRun );
    }

    aMergeEnv->mRoot         = (smdmisMergeRun *)(&aMergeEnv->mLeafRunList[sTotalRunCount - 1]);
    aMergeEnv->mMergeRunList = (smdmisMergeRun *)(&aMergeEnv->mLeafRunList[aLeafRunCount]);
    aMergeEnv->mLeafCount    = aLeafRunCount;
    aMergeEnv->mMergeCount   = sTotalRunCount - aLeafRunCount;
    aMergeEnv->mVolatileMode = aVolatileMode;
    aMergeEnv->mSortedList   = NULL;
    aMergeEnv->mLevel        = sLevel;

    smdmisResetMergeRunLink( aMergeEnv );

    SMDMIS_MERGE_ENV_CLEAR( aMergeEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static void smdmisResetMergeRunLink( smdmisMergeEnv *aMergeEnv )
{
    stlInt32        i, j;
    stlInt32        sCur;
    stlInt32        sChild;
    stlInt32        sLevelHead;
    stlInt32        sRemainIndex = 0;
    smdmisMergeRun *sMergeRun;

    sCur = aMergeEnv->mLeafCount;
    sChild = 0;
    for( i = 1; i <= aMergeEnv->mLevel; i++ )
    {
        sLevelHead = sCur;
        for( j = 0; j < aMergeEnv->mLevelCount[i]; j++ )
        {
            sMergeRun = (smdmisMergeRun *)(&aMergeEnv->mLeafRunList[sCur]);

            SMDMIS_MERGE_RUN_INIT( sMergeRun );

            SMDMIS_MERGE_RUN_SET_LEFT( sMergeRun, (smdmisMergeRun *)(&aMergeEnv->mLeafRunList[sChild]) );
            sChild++;
            if( sChild == sLevelHead )
            {
                STL_DASSERT( sRemainIndex > 0 );
                SMDMIS_MERGE_RUN_SET_RIGHT( sMergeRun, (smdmisMergeRun *)(&aMergeEnv->mLeafRunList[sRemainIndex]) );
            }
            else
            {
                STL_DASSERT( sChild < sLevelHead );
                SMDMIS_MERGE_RUN_SET_RIGHT( sMergeRun, (smdmisMergeRun *)(&aMergeEnv->mLeafRunList[sChild]) );
                sChild++;
            }
            sCur++;
        }
        if( sChild < sLevelHead )
        {
            sRemainIndex = sChild;
            sChild++;
        }
    }
}

static inline smdmisKeyRow smdmisLeafRunCurKey( smdmisTableHeader *aHeader,
                                                stlBool            aVolatileMode,
                                                smdmisLeafRun     *aRun )
{
    smdmisKeyRow     sRet = NULL;
    smdmisIndexNode *sNode;

    while( aRun->mNode != NULL )
    {
        if( SMDMIS_LEAF_RUN_IS_END( aRun ) == STL_FALSE )
        {
            sRet = SMDMIS_LEAF_RUN_CUR_KEY( aRun );
            break;
        }

        if( aVolatileMode == STL_TRUE )
        {
            /* volatile mode일 땐, next를 쫓아가면 안된다. 하나의 노드만 fetch해야 한다.
               next는 leaf들의 list이기 때문이다. */
            break;
        }

        sNode = aRun->mNode;
        aRun->mNode = SMDMIS_TO_NODE_PTR( aRun->mNode->mNext );

        smdmisFreeIndexNode( aHeader,
                             sNode );

        SMDMIS_LEAF_RUN_RESET( aRun );
    }

    return sRet;
}

smdmisKeyRow smdmisTableGetNext( smdmisTableHeader *aHeader,
                                 smdmisMergeEnv    *aMergeEnv )
{
    smdmisKeyRow     sRet;
    smdmisLeafRun   *sMin;
    smdmisMergeRun  *sRun;
    smdmisLeafRun   *sLeftLess;
    smdmisLeafRun   *sRightLess;
    smdmisKeyRow     sLeftKey;
    smdmisKeyRow     sRightKey;
    dtlCompareResult sComResult;

    sMin = aMergeEnv->mRoot->mLess;
    if( SMDMIS_LEAF_RUN_IS_END( sMin ) == STL_FALSE )
    {
        sRet = SMDMIS_LEAF_RUN_CUR_KEY( sMin );
    }
    else
    {
        sRet = smdmisLeafRunCurKey( aHeader, aMergeEnv->mVolatileMode, sMin );

        STL_TRY_THROW( sRet != NULL, RAMP_FINISH );
    }

    SMDMIS_LEAF_RUN_NEXT( sMin );

    if( ( SMDMIS_LEAF_RUN_IS_END( sMin ) == STL_FALSE ) &&
        ( SMDMIS_KEY_ROW_IS_DUP( SMDMIS_LEAF_RUN_CUR_KEY( sMin ) ) == STL_TRUE ) )
    {
        /* 다음 row가 dup key이면 (현재 row의 key 값과 같다면 ) mLess의 propagation이 필요없다.
           특허감이다! */
        STL_THROW( RAMP_FINISH );
    }

    sRun = sMin->mParent;
    while( sRun != NULL )
    {
        sLeftLess = sRun->mLeft->mLess;
        sRightLess = sRun->mRight->mLess;

        if( SMDMIS_LEAF_RUN_IS_END( sLeftLess ) == STL_FALSE )
        {
            sLeftKey = SMDMIS_LEAF_RUN_CUR_KEY( sLeftLess );
        }
        else
        {
            sLeftKey = smdmisLeafRunCurKey( aHeader, aMergeEnv->mVolatileMode, sLeftLess );

            if( sLeftKey == NULL )
            {
                /* left에 더이상 키가 없으면, root의 자식으로 자기대신 right를 매단다. */
                if( sRun->mParent != NULL )
                {
                    if( sRun->mParent->mLeft == sRun )
                    {
                        SMDMIS_MERGE_RUN_SET_LEFT( sRun->mParent, sRun->mRight );
                    }
                    else
                    {
                        SMDMIS_MERGE_RUN_SET_RIGHT( sRun->mParent, sRun->mRight );
                    }
                }
                else
                {
                    sRun->mLess = sRightLess;
                }
                sRun = sRun->mParent;
                continue;
            }
        }

        if( SMDMIS_LEAF_RUN_IS_END( sRightLess ) == STL_FALSE )
        {
            sRightKey = SMDMIS_LEAF_RUN_CUR_KEY( sRightLess );
        }
        else
        {
            sRightKey = smdmisLeafRunCurKey( aHeader, aMergeEnv->mVolatileMode, sRightLess );

            if( sRightKey == NULL )
            {
                /* right에 더이상 키가 없으면, root의 자식으로 자기대신 left를 매단다. */
                if( sRun->mParent != NULL )
                {
                    if( sRun->mParent->mLeft == sRun )
                    {
                        SMDMIS_MERGE_RUN_SET_LEFT( sRun->mParent, sRun->mLeft );
                    }
                    else
                    {
                        SMDMIS_MERGE_RUN_SET_RIGHT( sRun->mParent, sRun->mLeft );
                    }
                }
                else
                {
                    sRun->mLess = sLeftLess;
                }
                sRun = sRun->mParent;
                continue;
            }
        }

        if( SMDMIS_KEY_ROW_IS_DUP( sLeftKey ) == STL_TRUE )
        {
            sRun->mLess = sLeftLess;
        }
        else if( SMDMIS_KEY_ROW_IS_DUP( sRightKey ) == STL_TRUE )
        {
            sRun->mLess = sRightLess;
        }
        else
        {
            sComResult = aHeader->mCompareKey( aHeader,
                                               sLeftKey,
                                               sRightKey );

            if( sComResult == DTL_COMPARISON_LESS )
            {
                sRun->mLess = sLeftLess;
            }
            else if( sComResult == DTL_COMPARISON_EQUAL )
            {
                sRun->mLess = sLeftLess;
                SMDMIS_KEY_ROW_SET_DUP( sRightKey );
            }
            else
            {
                sRun->mLess = sRightLess;
            }
        }
        sRun = sRun->mParent;
    }

    STL_RAMP( RAMP_FINISH );

    return sRet;
}

void smdmisTableReadyToMerge( smdmisTableHeader *aHeader,
                              smdmisMergeEnv    *aMergeEnv )
{
    stlInt32          i;
    smdmisLeafRun    *sLeftLess;
    smdmisLeafRun    *sRightLess;
    smdmisKeyRow      sLeftKey;
    smdmisKeyRow      sRightKey;
    dtlCompareResult  sComResult;

    /* 0 ~ mLeafCount가 leaf run들이지만, curLeafIndex까지 add되었다. */
    for( i = 0; i < aMergeEnv->mCurLeafIndex; i++ )
    {
        SMDMIS_LEAF_RUN_RESET( &aMergeEnv->mLeafRunList[i] );
    }

    for( i = 0; i < aMergeEnv->mMergeCount; i++ )
    {
        sLeftLess = aMergeEnv->mMergeRunList[i].mLeft->mLess;
        sLeftKey = smdmisLeafRunCurKey( aHeader, aMergeEnv->mVolatileMode, sLeftLess );

        sRightLess = aMergeEnv->mMergeRunList[i].mRight->mLess;
        sRightKey = smdmisLeafRunCurKey( aHeader, aMergeEnv->mVolatileMode, sRightLess );

        if( sLeftKey == NULL )
        {
            aMergeEnv->mMergeRunList[i].mLess = sRightLess;
        }
        else if( sRightKey == NULL )
        {
            aMergeEnv->mMergeRunList[i].mLess = sLeftLess;
        }
        else
        {
            sComResult = aHeader->mCompareKey( aHeader,
                                               sLeftKey,
                                               sRightKey );

            if( sComResult == DTL_COMPARISON_LESS )
            {
                aMergeEnv->mMergeRunList[i].mLess = sLeftLess;
            }
            else if( sComResult == DTL_COMPARISON_EQUAL )
            {
                aMergeEnv->mMergeRunList[i].mLess = sLeftLess;
                SMDMIS_KEY_ROW_SET_DUP( sRightKey );
            }
            else
            {
                aMergeEnv->mMergeRunList[i].mLess = sRightLess;
            }
        }
    }
}

static void smdmisAddNodeToMergeEnv( smdmisTableHeader  *aHeader,
                                     smdmisMergeEnv     *aMergeEnv,
                                     smdmisIndexNode    *aNode )
{
    smdmisIndexNode *sLastNode = SMDMIS_MERGE_ENV_GET_LAST_NODE( aMergeEnv );
    smdmisKeyRow     sLastMaxKey;
    smdmisKeyRow     sCurMinKey;
    dtlCompareResult sCompResult;
    stlBool          sNodeAdded = STL_FALSE;

    if( aNode->mNext == KNL_LOGICAL_ADDR_NULL )
    {
        if( sLastNode != NULL )
        {
            /* 이전 노드의 max key 값과 현재 노드의 min key를 비교하여
               prev->max <= cur->min이면 이미 두 노드간에 정렬이 된 것이기 때문에
               prev의 next에 cur를 매단다. merge시 비용을 줄인다. */
            STL_DASSERT( sLastNode->mKeyCount > 0 );
            sLastMaxKey = SMDMIS_NODE_GET_KEY( sLastNode, sLastNode->mKeyCount - 1 );

            STL_DASSERT( aNode->mKeyCount > 0 );
            sCurMinKey = SMDMIS_NODE_GET_KEY( aNode, 0 );

            sCompResult = aHeader->mCompareKey( aHeader,
                                                sLastMaxKey,
                                                sCurMinKey );

            if( sCompResult != DTL_COMPARISON_GREATER )
            {
                /* 두 노드간 크기 순서를 정할 수 있으므로
                   mergeEnv에 매달지 않고 lastNode의 next에 단다. */
                SMDMIS_INDEX_NODE_ADD_TO_LIST( sLastNode, aNode );
                sNodeAdded = STL_TRUE;

                if( sCompResult == DTL_COMPARISON_EQUAL )
                {
                    /* last->max = cur->min 이므로, cur->min의 dup를 세팅한다. */
                    SMDMIS_KEY_ROW_SET_DUP( sCurMinKey );
                }
            }
        }

        SMDMIS_MERGE_ENV_SET_LAST_NODE( aMergeEnv, aNode );
    }
    else
    {
        /* aNode가 단독 node가 아니라 리스트일 경우엔 last node를 이용한 최적화를 하지 않는다.
           최적화를 하려면 last를 구하기 위해 aNode의 리스트를 탐색해야 하는 비용이 발생하고,
           또한 aNode가 리스트로 merge env에 adde되는 경우는 최적화 적용이 안될 가능성이 많기 때문이다.*/
    }

    if( sNodeAdded == STL_FALSE )
    {
        SMDMIS_MERGE_ENV_ADD_NODE( aMergeEnv, aNode );
    }
}

static stlStatus smdmisAddSortedRun( smdmisTableHeader   *aHeader,
                                     smdmisMergeEnv      *aMergeEnv,
                                     smdmisIndexNode     *aFirstNode,
                                     smdmisRunStackList **aSortedListPtr,
                                     smlEnv              *aEnv )
{
    smdmisRunStackList *sSortedList = *aSortedListPtr;
    stlInt32            i;

    /* merge env는 비어있어야 한다. */
    STL_DASSERT( SMDMIS_MERGE_ENV_IS_EMPTY( aMergeEnv ) == STL_TRUE );

    if( sSortedList == NULL )
    {
        /* sort list가 없으므로 할당받는다. */
        STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                    STL_OFFSETOF( smdmisRunStackList, mItems ) +
                                        STL_SIZEOF( smdmisIndexNode * ) * aMergeEnv->mLeafCount,
                                    (void**)&sSortedList,
                                    KNL_ENV( aEnv ) ) == STL_SUCCESS );

        SMDMIS_RUN_STACK_LIST_INIT( sSortedList, aMergeEnv->mLeafCount );

        *aSortedListPtr = sSortedList;
    }
    else if( SMDMIS_RUN_STACK_LIST_IS_FULL( sSortedList ) == STL_TRUE )
    {
        for( i = 0; i < sSortedList->mItemCount; i++ )
        {
            smdmisAddNodeToMergeEnv( aHeader,
                                     aMergeEnv,
                                     sSortedList->mItems[i] );
        }

        STL_DASSERT( SMDMIS_MERGE_ENV_IS_FULL( aMergeEnv ) == STL_TRUE );

        STL_TRY( smdmisMerge( aHeader,
                              aMergeEnv,
                              &sSortedList->mNext,
                              aEnv ) == STL_SUCCESS );

        SMDMIS_RUN_STACK_LIST_CLEAR( sSortedList );
    }

    SMDMIS_RUN_STACK_LIST_ADD_RUN( sSortedList,
                                   aFirstNode );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisMerge( smdmisTableHeader   *aHeader,
                              smdmisMergeEnv       *aMergeEnv,
                              smdmisRunStackList  **aSortedListPtr,
                              smlEnv               *aEnv )
{
    smdmisIndexNode *sFirstNode = NULL;
    smdmisIndexNode *sNode = NULL;
    smdmisIndexNode *sNewNode = NULL;
    smdmisKeyRow     sKeyRow;

    if( SMDMIS_MERGE_ENV_IS_EMPTY(aMergeEnv) == STL_TRUE )
    {
        /* merge할 run들이 하나도 없다. */
        STL_THROW( RAMP_NOTHING_TO_MERGE );
    }

    if( SMDMIS_MERGE_ENV_HAS_ONLY_ONE_SORTED_RUN(aMergeEnv) == STL_TRUE )
    {
        /* merge할 run이 딱 하나 있다. 이 경우 merge 과정 없이, node list만 sorted list에 매단다. */
        sFirstNode = SMDMIS_MERGE_ENV_FIRST_NODE(aMergeEnv);

        STL_THROW( RAMP_MERGE_DONE );
    }

    /**
     * merge phase
     */
    smdmisTableReadyToMerge( aHeader, aMergeEnv );

    sKeyRow = smdmisTableGetNext( aHeader, aMergeEnv );
    while( sKeyRow != NULL )
    {
        /*
         * insert key
         */
        if( (sNode == NULL) || (SMDMIS_NODE_IS_FULL(sNode) == STL_TRUE) )
        {
            STL_TRY( smdmisAllocIndexNode( aHeader,
                                           &sNewNode,
                                           aEnv ) == STL_SUCCESS );

            smdmisIndexNodeInit( sNewNode,
                                 0, /* this is leaf node */
                                 aHeader->mNodeSize,
                                 aHeader->mKeyRowLen );

            if( sNode == NULL )
            {
                sFirstNode = sNewNode;
            }
            else
            {
                SMDMIS_INDEX_NODE_ADD_TO_LIST( sNode, sNewNode );
            }
            sNode = sNewNode;
        }

        SMDMIS_NODE_APPEND_KEY( sNode, sKeyRow, aHeader->mKeyRowLen );

        sKeyRow = smdmisTableGetNext( aHeader, aMergeEnv );
    }

    STL_RAMP( RAMP_MERGE_DONE );

    /**
     * merge env를 reset한다. mCurLeafIndex를 제자리에 놓는다.
     */
    SMDMIS_MERGE_ENV_CLEAR( aMergeEnv );

    /* merge env의 merge run의 link를 reset한다. */
    smdmisResetMergeRunLink( aMergeEnv );

    /**
     * add sort run phase: merge결과 얻은 sort run을 추가한다.
     */
    STL_TRY( smdmisAddSortedRun( aHeader,
                                 aMergeEnv,
                                 sFirstNode,
                                 aSortedListPtr,
                                 aEnv ) == STL_SUCCESS );

    STL_RAMP( RAMP_NOTHING_TO_MERGE );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisMergeAll( smdmisTableHeader *aHeader,
                                 smdmisMergeEnv    *aMergeEnv,
                                 smlEnv            *aEnv )
{
    stlInt32            i;
    smdmisRunStackList *sSortedList = aMergeEnv->mSortedList;

    while( sSortedList != NULL )
    {
        for( i = 0; i < sSortedList->mItemCount; i++ )
        {
            if( SMDMIS_MERGE_ENV_IS_FULL( aMergeEnv ) == STL_TRUE )
            {
                STL_TRY( smdmisMerge( aHeader,
                                      aMergeEnv,
                                      &sSortedList->mNext,
                                      aEnv ) == STL_SUCCESS );
            }

            smdmisAddNodeToMergeEnv( aHeader,
                                     aMergeEnv,
                                     sSortedList->mItems[i] );
        }

        SMDMIS_RUN_STACK_LIST_CLEAR( sSortedList );

        sSortedList = sSortedList->mNext;
    }

    /* 최종 sorted run들을 merge한다. */
    STL_TRY( smdmisMerge( aHeader,
                          aMergeEnv,
                          &aMergeEnv->mSortedList,
                          aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisBuildInternalNodes( smdmisTableHeader *aHeader,
                                           smlEnv            *aEnv )
{
    smdmisIndexNode *sFirstNode;
    smdmisIndexNode *sFirstParentNode;
    smdmisIndexNode *sNode;
    smdmisIndexNode *sParentNode;
    smdmisIndexNode *sNewNode;
    smdmisKeyRow     sSourceKey;
    smdmisKeyRow     sTargetKey;
    stlInt32         sLevel = 1;

    sFirstNode = SMDMIS_TO_NODE_PTR( aHeader->mFirstLeafNode );
    sFirstParentNode = sFirstNode;

    while( sFirstNode->mNext != KNL_LOGICAL_ADDR_NULL )
    {
        /* only one child, this is the root */

        STL_TRY( smdmisAllocIndexNode( aHeader,
                                       &sNewNode,
                                       aEnv ) == STL_SUCCESS );

        smdmisIndexNodeInit( sNewNode,
                             sLevel, /* this is leaf node */
                             aHeader->mNodeSize,
                             aHeader->mKeyRowLen );

        sFirstParentNode = sNewNode;

        SMDMIS_INDEX_NODE_SET_LEFTMOST( sFirstParentNode, sFirstNode );

        sNode = SMDMIS_INDEX_NODE_GET_NEXT( sFirstNode );
        sParentNode = sFirstParentNode;

        while( sNode != NULL )
        {
            STL_DASSERT( sNode->mKeyCount > 0 );

            sSourceKey = SMDMIS_NODE_GET_KEY( sNode, 0 );

            if( SMDMIS_NODE_IS_FULL( sParentNode ) == STL_TRUE )
            {
                STL_TRY( smdmisAllocIndexNode( aHeader,
                                               &sNewNode,
                                               aEnv ) == STL_SUCCESS );

                smdmisIndexNodeInit( sNewNode,
                                     sLevel, /* this is leaf node */
                                     aHeader->mNodeSize,
                                     aHeader->mKeyRowLen );

                SMDMIS_INDEX_NODE_ADD_TO_LIST( sParentNode,
                                               sNewNode );

                sParentNode = sNewNode;
            }

            SMDMIS_NODE_ALLOC_KEY( sParentNode, sTargetKey, aHeader->mKeyRowLen );

            stlMemcpy( sTargetKey, sSourceKey, aHeader->mKeyRowLen );

            SMDMIS_KEY_ROW_WRITE_CHILD_ADDR( sTargetKey, sNode->mMyAddr );

            sNode = SMDMIS_INDEX_NODE_GET_NEXT( sNode );
        }

        sFirstNode = sFirstParentNode;
        sLevel++;
    }

    aHeader->mRootNode = sFirstParentNode->mMyAddr;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief flat하게 insert된 테이블을 기반으로 volatile sort table 형태로 사용할 수 있는 환경을 만든다.
 * @param[in] aStatement Statement Pointer
 * @param[in] aHeader Index를 만들 sort table header
 * @param[out] aMergeEnv sort된 key들을 fetchNext를 통해 얻을 수 있는 핸들.
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmisTableBuildMergeEnv( smlStatement       *aStatement,
                                    smdmisTableHeader  *aHeader,
                                    smdmisMergeEnv    **aMergeEnv,
                                    smlEnv             *aEnv )
{
    smdmisIndexNode *sLeafList;
    smdmisIndexNode *sNode;

    STL_TRY( smaAllocRegionMem( aStatement,
                                STL_SIZEOF( smdmisMergeEnv ),
                                (void**)aMergeEnv,
                                aEnv ) == STL_SUCCESS );

    STL_TRY( smdmisInitMergeEnv( aStatement,
                                 *aMergeEnv,
                                 smdmisCalculateNodeCount( aHeader, &aHeader->mBaseHeader ),
                                 STL_TRUE, /* aVolatileMode */
                                 aEnv ) == STL_SUCCESS );

    if( aHeader->mUnorderedLeafList == KNL_LOGICAL_ADDR_NULL )
    {
        STL_TRY( smdmisBuildLeafNodes( aHeader,
                                       &aHeader->mBaseHeader,
                                       &sLeafList,
                                       aEnv ) == STL_SUCCESS );

        aHeader->mUnorderedLeafList = sLeafList->mMyAddr;
    }
    else
    {
        sLeafList = SMDMIS_TO_NODE_PTR( aHeader->mUnorderedLeafList );
    }

    /* leaf들을 merge environment에 매단다. */
    while( sLeafList != NULL )
    {
        SMDMIS_INDEX_NODE_REMOVE_FIRST( sLeafList, sNode );

        /* merge env의 leaf run들을 leaf node들만큼 만들었기 때문에 가득찰 수가 없다. */
        STL_DASSERT( SMDMIS_MERGE_ENV_IS_FULL( *aMergeEnv ) == STL_FALSE );

        smdmisAddNodeToMergeEnv( aHeader,
                                 *aMergeEnv,
                                 sNode );
    }

    /* merge할 준비를 한다. */
    smdmisTableReadyToMerge( aHeader, *aMergeEnv );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

/**
 * @brief aBaseHeader 테이블을 기반으로 index를 빌드한다. leaf만 만들수도, index tree를 만들 수도 있다.
 * @param[in] aStatement Statement Pointer
 * @param[in] aHeader Index를 만들 sort table header
 * @param[in] aBaseHeader row가 저장된 base table. 외부 테이블일 수도 있고, 자기자신 테이블일 수도 있다.
 * @param[in] aEnv Storage Manager Environment
 */
stlStatus smdmisTableBuildIndex( smlStatement      *aStatement,
                                 smdmisTableHeader *aHeader,
                                 smdmifTableHeader *aBaseHeader,
                                 smlEnv            *aEnv )
{
    smdmisIndexNode *sSortedNodes = NULL;
    stlInt32         i;
    stlInt64         sSortMethod;

    if( aHeader->mKeyCols == NULL )
    {
        /* Build Index는 한 sort instant 테이블에 대해 여러번 불릴 수 있다.
         * mKeyCols는 테이블이 만들어질 때 구성되고 drop될 때 free되어야 하나
         * 이 정보는 insert시 내려오는 컬럼 정보를 바탕으로 구성되기 때문에
         * 데이터가 insert된 후에야 구성될 수 있다.
         * 이 이유로 create시점에 구성하지 못하고 insert나 build index시 구성되는데
         * 이 때문에 여러번 구성되는 것을 방지할 필요가 있다.
         * 해서 mKeyCols가 NULL일 때만 구성하도록 한다.
         * 해제는 truncate가 아니라 drop때 해야 한다.
         */
        STL_TRY( smdmisInitKeyCols( aHeader,
                                    aBaseHeader,
                                    STL_TRUE,
                                    aEnv ) == STL_SUCCESS );
    }

    /* 한건도 insert되지 않았으면 build 과정을 거치지 않는다. */
    STL_TRY_THROW( SMDMIF_TABLE_FIRST_INSERTED( aBaseHeader ) == STL_TRUE, RAMP_FINISH );

    sSortMethod = knlGetPropertyBigIntValueByID( KNL_PROPERTY_INST_TABLE_SORT_METHOD,
                                                 KNL_ENV( aEnv ) );

    if( sSortMethod != SMDMIS_SORT_METHOD_KEY_COMP )
    {
        for( i = 0; i < aHeader->mKeyColCount; i++ )
        {
            if( (aHeader->mKeyCols[i].mDataType == DTL_TYPE_NATIVE_REAL) ||
                (aHeader->mKeyCols[i].mDataType == DTL_TYPE_NATIVE_DOUBLE) ||
                (aHeader->mKeyCols[i].mDataType == DTL_TYPE_VARBINARY) ||
                (aHeader->mKeyCols[i].mIsFixedPart == STL_FALSE ) )
            {
                sSortMethod = SMDMIS_SORT_METHOD_KEY_COMP;
                break;
            }
        }

        if( sSortMethod == SMDMIS_SORT_METHOD_AUTO )
        {
            if( smdmisLSDRadixIsBetter( aHeader,
                                        aEnv ) == STL_TRUE )
            {
                sSortMethod = SMDMIS_SORT_METHOD_LSD_RADIX;
            }
            else
            {
                sSortMethod = SMDMIS_SORT_METHOD_MSD_RADIX;
            }
        }
    }

    if( sSortMethod == SMDMIS_SORT_METHOD_MSD_RADIX )
    {
        aHeader->mCompareKey = smdmisCompareKeyByRadix;

        STL_TRY( smdmisSortByMSDRadix( aHeader,
                                       aBaseHeader,
                                       &sSortedNodes,
                                       aEnv ) == STL_SUCCESS );
    }
    else if( sSortMethod == SMDMIS_SORT_METHOD_LSD_RADIX )
    {
        aHeader->mCompareKey = smdmisCompareKeyByRadix;

        STL_TRY( smdmisSortByLSDRadix( aHeader,
                                       aBaseHeader,
                                       &sSortedNodes,
                                       aEnv ) == STL_SUCCESS );
    }
    else
    {
        STL_DASSERT( sSortMethod == SMDMIS_SORT_METHOD_KEY_COMP );

        /* compare function을 설정한다. */
        smdmisSetCompareFunc( aHeader );

        STL_TRY( smdmisSortByMerge( aStatement,
                                    aHeader,
                                    aBaseHeader,
                                    &sSortedNodes,
                                    aEnv ) == STL_SUCCESS );
    }

    /* sort된 node 리스트를 firstLeafNode에 매단다. */
    aHeader->mFirstLeafNode = sSortedNodes->mMyAddr;

    if( aHeader->mAttr.mLeafOnly == STL_FALSE )
    {
        STL_TRY( smdmisBuildInternalNodes( aHeader,
                                           aEnv ) == STL_SUCCESS );
    }

    STL_RAMP( RAMP_FINISH );

    STL_DASSERT( smdmisTableVerify( aHeader ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlBool smdmisLSDRadixIsBetter( smdmisTableHeader *aHeader,
                                       smlEnv            *aEnv )
{
    if( (aHeader->mKeyColCount == 1) &&
        (aHeader->mKeyRowLen <= knlGetPropertyBigIntValueByID(
              KNL_PROPERTY_INST_TABLE_MAX_KEY_LEN_FOR_LSD_RADIX,
              KNL_ENV( aEnv ) ) ) )
    {
        return STL_TRUE;
    }
    return STL_FALSE;
}

static stlStatus smdmisSortByMSDRadix( smdmisTableHeader *aHeader,
                                       smdmifTableHeader *aBaseHeader,
                                       smdmisIndexNode  **aSortedNodes,
                                       smlEnv            *aEnv )
{
    stlChar              *sColValue;
    stlInt64              sColLen;
    void                 *sScanFixedPart;
    stlInt32              sScanRowIndex;
    smdmifFixedRow        sScanRow;
    knlLogicalAddr        sScanRowAddr;
    smdmisRadixNode      *sNewNode;
    smdmisRadixNode      *sNodeListFirst;
    smdmisRadixNode      *sNodeListLast;
    smdmisIndexNode      *sIndexNode;
    smdmisKeyRow          sKeyRow;
    smdmifDefColumn      *sKeyCol;
    smdmifDefColumn      *sRowCol;
    stlInt32              i;
    smdmisRadixBucket     sBucket[SMDMIS_RADIX_RANGE + 1];
    stlUInt8             *sRadixPos;
    stlUInt8              sRadixValue;
    stlBool               sEmptyTable = STL_TRUE;
    stlChar              *sKeyColPos;
    smdmisKeyRow          sTempKeyAlloced;
    smdmisKeyRow          sTempKey;
    knlRegionMark         sOpMemMark;
    stlInt32              sState = 0;

    KNL_INIT_REGION_MARK( &sOpMemMark );
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sOpMemMark,
                               KNL_ENV( aEnv ) ) == STL_SUCCESS );

    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                aHeader->mKeyRowLen + STL_SIZEOF(knlLogicalAddr),
                                (void**)&sTempKeyAlloced,
                                KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* sTempKey는 SMDMIS_KEY_ROW_HEADER_SIZE를 더한 위치가 align이 맞아야 한다.*/
    sTempKey = sTempKeyAlloced + STL_SIZEOF(knlLogicalAddr) - SMDMIS_KEY_ROW_HEADER_SIZE;

    STL_TRY( smdmisMakeRadixEnv( aHeader,
                                 aEnv ) == STL_SUCCESS );

    sRadixPos = (stlUInt8 *)(sTempKey + aHeader->mRadixArray[0]);

    /* radix sort를 위한 bucket을 초기화한다. */
    for( i = 0; i <= SMDMIS_RADIX_RANGE; i++)
    {
        sBucket[i].mFirst = NULL;
        sBucket[i].mLast = NULL;
    }

    sScanFixedPart = SMDMIF_TABLE_GET_FIRST_FIXED_PART(aBaseHeader);
    sScanRowIndex = 0;

    while( STL_TRUE )
    {
        /**
         * base table에 다음 row를 구한다.
         */
        sScanRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sScanFixedPart, sScanRowIndex );
        while( sScanRowAddr == KNL_LOGICAL_ADDR_NULL )
        {
            if( sScanFixedPart == NULL )
            {
                break;
            }
            sScanFixedPart = SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART( sScanFixedPart );
            sScanRowIndex = 0;
            sScanRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sScanFixedPart, sScanRowIndex );
        }

        if( sScanRowAddr == KNL_LOGICAL_ADDR_NULL )
        {
            /* 더이상 row가 없다. */
            break;
        }

        sScanRow = KNL_TO_PHYSICAL_ADDR( sScanRowAddr );
        sScanRowIndex++;

        /* column value에 0-pad를 위해 row 자체를 0으로 초기화한다. */
        SMDMIS_KEY_ROW_INIT( sTempKey, aHeader->mKeyRowLen );

        /*
         * key row를 write한다.
         */
        SMDMIS_KEY_ROW_WRITE_CHILD_ADDR( sTempKey, sScanRowAddr );

        for( i = 0; i < aHeader->mKeyColCount; i++ )
        {
            sKeyCol = &aHeader->mKeyCols[i];
            sRowCol = &aBaseHeader->mCols[aHeader->mKeyColInfo[i].mColOrder];
            sKeyColPos = sTempKey + SMDMIS_KEY_COL_OFFSET_VALUE( sKeyCol );

            STL_DASSERT( sKeyCol->mIsFixedPart == STL_TRUE );

            smdmifTableGetColValueAndLen( sScanRow,
                                          sRowCol,
                                          /* var part이고 쪼개져 저장되어 있으면 바로 여기에 카피가 된다. */
                                          sKeyColPos,
                                          &sColValue,
                                          &sColLen );

            if( sColValue != sKeyColPos )
            {
                /* sColValue가 sKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE( sKeyCol )와 같다면 이미 카피가 되었다. */
                SMDMIS_KEY_ROW_WRITE_COL( sTempKey,
                                          sKeyCol,
                                          sColValue,
                                          sColLen );
            }

            if( aHeader->mKeyColInfo[i].mModifyKeyVal != NULL )
            {
                aHeader->mKeyColInfo[i].mModifyKeyVal( sTempKey,
                                                       sColLen,
                                                       sKeyCol );
            }
        }

        sRadixValue = *sRadixPos;

        /**
         * 키를 입력할 node 및 위치 확보
         */
        if( (sBucket[sRadixValue].mLast == NULL) ||
            (SMDMIS_NODE_IS_FULL(sBucket[sRadixValue].mLast) == STL_TRUE) )
        {
            STL_TRY( smdmisAllocRadixNode( aHeader,
                                           &sNewNode,
                                           aEnv ) == STL_SUCCESS );

            smdmisRadixNodeInit( sNewNode,
                                 0, /* MSD radix index */
                                 aHeader->mNodeSize,
                                 aHeader->mKeyRowLen );

            if( sBucket[sRadixValue].mLast == NULL )
            {
                sBucket[sRadixValue].mFirst = sNewNode;
            }
            else
            {
                sBucket[sRadixValue].mLast->mNextOverflow = sNewNode;
            }

            sBucket[sRadixValue].mLast = sNewNode;
        }

        SMDMIS_NODE_ALLOC_KEY( sBucket[sRadixValue].mLast, sKeyRow, aHeader->mKeyRowLen );

        stlMemcpy( sKeyRow, sTempKey, aHeader->mKeyRowLen );

        sEmptyTable = STL_FALSE;
    }

    if( sEmptyTable == STL_FALSE )
    {
        /* 첫 인덱스 노드를 하나 생성해, radix sort 함수에 넘긴다.
           compact가 이루어지면 이 index node에 채워진다. */
        STL_TRY( smdmisAllocIndexNode( aHeader,
                                       &sIndexNode,
                                       aEnv ) == STL_SUCCESS );

        smdmisIndexNodeInit( sIndexNode,
                             0, /* this is leaf node */
                             aHeader->mNodeSize,
                             aHeader->mKeyRowLen );

        *aSortedNodes = sIndexNode;

        STL_TRY( smdmisMakeNodeListFromBucket( aHeader,
                                               sBucket,
                                               SMDMIS_RADIX_RANGE + 1,
                                               &sNodeListFirst,
                                               &sNodeListLast, 
                                               &sIndexNode,
                                               aEnv ) == STL_SUCCESS );

        STL_TRY( smdmisRadixSort( aHeader,
                                  sBucket,
                                  SMDMIS_RADIX_RANGE + 1,
                                  &sNodeListFirst,
                                  &sIndexNode,
                                  aEnv ) == STL_SUCCESS );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sOpMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sOpMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            KNL_ENV( aEnv ) );
            break;
    }

    return STL_FAILURE;
}

stlStatus smdmisTableInvalidateRadix( smdmisTableHeader *aHeader,
                                      smlEnv            *aEnv )
{
    smdmisIndexNode *sNode = SMDMIS_TO_NODE_PTR( aHeader->mRootNode );
    smdmisIndexNode *sFirstChild;
    smdmisKeyColumn *sKeyCol;
    stlInt32         i, j;

    /* 이 함수는 반드시 internal node까지 모두 빌드된 후에 불려야 한다. */
    STL_DASSERT( sNode != NULL );

    while( sNode != NULL )
    {
        sFirstChild = SMDMIS_INDEX_NODE_LEFTMOST_NODE( sNode );
        while( sNode != NULL )
        {
            for( i = 0; i < sNode->mKeyCount; i++ )
            {
                for( j = 0; j < aHeader->mKeyColCount; j++ )
                {
                    sKeyCol = &aHeader->mKeyColInfo[j];
                    if( sKeyCol->mRecoverKeyVal != NULL )
                    {
                        sKeyCol->mRecoverKeyVal( SMDMIS_NODE_GET_KEY( sNode, i ),
                                                 &aHeader->mKeyCols[j] );
                    }
                }
            }
            sNode = SMDMIS_INDEX_NODE_GET_NEXT( sNode );
        }

        sNode = sFirstChild;
    }

    /* compare function을 physical compare function으로 세팅한다. */
    smdmisSetCompareFunc( aHeader );

    /* SMDMIS_TABLE_IS_RADIX_SORTED가 FALSE가 나오게 하기 위해 mRadixArray 정보를 제거한다. */
    STL_TRY( smgFreeSessShmMem( aHeader->mRadixArray,
                                aEnv ) == STL_SUCCESS );
    aHeader->mRadixArray = NULL;

    STL_DASSERT( smdmisTableVerify( aHeader ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}
                                      
static stlStatus smdmisMakeRadixEnv( smdmisTableHeader *aHeader,
                                     smlEnv            *aEnv )
{
    stlInt32 i;
    stlInt32 j;
    stlInt32 sColStart;
    stlInt32 sColEnd;
    stlInt32 sRadixIndex = 0;

    STL_TRY( smgAllocSessShmMem( aHeader->mKeyRowLen * STL_SIZEOF( stlUInt16 ),
                                 (void**)&aHeader->mRadixArray,
                                 aEnv ) == STL_SUCCESS );

    for( i = 0; i < aHeader->mKeyColCount; i++ )
    {
        if( (aHeader->mKeyCols[i].mHasNull == STL_TRUE) &&
            (aHeader->mKeyCols[i].mDataType != DTL_TYPE_FLOAT) &&
            (aHeader->mKeyCols[i].mDataType != DTL_TYPE_NUMBER) )
        {
            /* null이 하나라도 있다. null 바이트가 하나 추가되고, radix sort시
               이 바이트가 MSD가 된다.
               다만, numeric의 경우 첫 바이트에 null을 위한 값을 따로 할당하기 때문에
               null을 위한 바이트를 추가하지 않는다. */
            aHeader->mRadixArray[sRadixIndex++] = SMDMIS_KEY_COL_OFFSET_NULL_VALUE(&(aHeader->mKeyCols[i]));
        }

        if( (aHeader->mKeyCols[i].mDataType == DTL_TYPE_TIME_WITH_TIME_ZONE) ||
            (aHeader->mKeyCols[i].mDataType == DTL_TYPE_TIMESTAMP_WITH_TIME_ZONE) )
        {
            /* timezone을 가진 타입들은 timezone 영역은 radix sort에서 제외한다. */
            sColStart = 0;
            sColEnd = aHeader->mKeyCols[i].mAllocSize - STL_SIZEOF( stlInt32 );
        }
        else if( aHeader->mKeyCols[i].mDataType == DTL_TYPE_INTERVAL_YEAR_TO_MONTH )
        {
            /* interval 계열은 indicator 맴버를 제외한다. */
            sColStart = 0;
            sColEnd = aHeader->mKeyCols[i].mAllocSize - STL_SIZEOF( stlInt32 );
        }
        else if( aHeader->mKeyCols[i].mDataType == DTL_TYPE_INTERVAL_DAY_TO_SECOND )
        {
            /* interval 계열은 indicator 맴버를 제외한다. */

            /* day 부터 radix array를 구성한다. */
            sColStart = STL_SIZEOF( dtlTimeOffset );
            sColEnd = STL_SIZEOF( dtlTimeOffset ) + STL_SIZEOF( stlInt32 );
            
            for( j = sColStart; j < sColEnd; j++ )
            {
                aHeader->mRadixArray[sRadixIndex++] = SMDMIS_KEY_COL_OFFSET_VALUE(&(aHeader->mKeyCols[i])) + j;
            }

            /* time 부터 radix array를 구성한다. */
            sColStart = 0;
            sColEnd = STL_SIZEOF( dtlTimeOffset );
        }
        else
        {
            /* native 소수 계열들은 radix sort가 불가능하다. */
            STL_DASSERT( aHeader->mKeyCols[i].mDataType != DTL_TYPE_NATIVE_REAL );
            STL_DASSERT( aHeader->mKeyCols[i].mDataType != DTL_TYPE_NATIVE_DOUBLE );

            /* 대부분의 타입들은 MSD radix sort를 그대로 수행할 수 있다. */
            sColStart = 0;
            sColEnd = aHeader->mKeyCols[i].mAllocSize;
        }

        for( j = sColStart; j < sColEnd; j++ )
        {
            aHeader->mRadixArray[sRadixIndex++] = SMDMIS_KEY_COL_OFFSET_VALUE(&(aHeader->mKeyCols[i])) + j;
        }

        aHeader->mKeyColInfo[i].mModifyKeyVal = 
            gSmdmisModifyKeyValFuncVector[aHeader->mKeyColInfo[i].mAscending == STL_TRUE ? 0 : 1]
                                         [aHeader->mKeyCols[i].mHasNull == STL_FALSE ? 0 :
                                             (aHeader->mKeyColInfo[i].mNullOrderLast == STL_TRUE ? 1 : 2)]
                                         [aHeader->mKeyCols[i].mDataType];

        aHeader->mKeyColInfo[i].mRecoverKeyVal =
            gSmdmisRecoverKeyValFuncVector[aHeader->mKeyColInfo[i].mAscending == STL_TRUE ? 0 : 1]
                                          [aHeader->mKeyCols[i].mDataType];
    }

    aHeader->mRadixIndexFence = sRadixIndex;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static void smdmisModifyKeyValNumeric( smdmisKeyRow       aKeyRow,
                                       stlInt64           aColLen,
                                       smdmifDefColumn   *aKeyCol )
{
    stlUInt8 *sPos;
    stlUInt8 *sPosFence;

    /* 음수 처리 */
    if( aColLen < aKeyCol->mAllocSize )
    {
        sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);

        if( *sPos < SMDMIS_NUMERIC_MINUS_SIGN )
        {
            /* 음수일 경우엔 0-pad가 아니라 ff-pad를 붙여야 한다.
               -101과 -100을 비교할 때를 생각해보면 된다. */
            sPosFence = sPos + aKeyCol->mAllocSize;
            for( sPos = sPos + aColLen;
                 sPos < sPosFence;
                 sPos++ )
            {
                *sPos = SMDMIS_NUMERIC_MINUS_PAD;
            }
        }
    }
}

static void smdmisModifyKeyValNull( smdmisKeyRow       aKeyRow,
                                    stlInt64           aColLen,
                                    smdmifDefColumn   *aKeyCol )
{
    /* NULL 처리 */
    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }   
    else
    {   
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );
    }   
}

static void smdmisModifyKeyValNumericNull( smdmisKeyRow       aKeyRow,
                                           stlInt64           aColLen,
                                           smdmifDefColumn   *aKeyCol )
{
    stlUInt8 *sPos;
    stlUInt8 *sPosFence;

    /* NULL 처리 */
    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NUMERIC_NULL( aKeyRow, aKeyCol );
    }
    else if( aColLen < aKeyCol->mAllocSize )
    {
        /* 음수 처리 */
        sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);

        if( *sPos < SMDMIS_NUMERIC_MINUS_SIGN )
        {
            /* 음수일 경우엔 0-pad가 아니라 ff-pad를 붙여야 한다.
               -101과 -100을 비교할 때를 생각해보면 된다. */
            sPosFence = sPos + aKeyCol->mAllocSize;
            for( sPos = sPos + aColLen;
                 sPos < sPosFence;
                 sPos++ )
            {
                *sPos = SMDMIS_NUMERIC_MINUS_PAD;
            }
        }
    }
}

static void smdmisModifyKeyValNullFirst( smdmisKeyRow       aKeyRow,
                                         stlInt64           aColLen,
                                         smdmifDefColumn   *aKeyCol )
{
    /* NULL 처리 */
    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );
    }
}

static void smdmisModifyKeyValNumericNullFirst( smdmisKeyRow       aKeyRow,
                                                stlInt64           aColLen,
                                                smdmifDefColumn   *aKeyCol )
{
    stlUInt8 *sPos;
    stlUInt8 *sPosFence;

    /* NULL 처리 */
    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NUMERIC_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else if( aColLen < aKeyCol->mAllocSize )
    {
        /* 음수 처리 */
        sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);

        if( *sPos < SMDMIS_NUMERIC_MINUS_SIGN )
        {
            /* 음수일 경우엔 0-pad가 아니라 ff-pad를 붙여야 한다.
               -101과 -100을 비교할 때를 생각해보면 된다. */
            sPosFence = sPos + aKeyCol->mAllocSize;
            for( sPos = sPos + aColLen;
                 sPos < sPosFence;
                 sPos++ )
            {
                *sPos = SMDMIS_NUMERIC_MINUS_PAD;
            }
        }
    }
}

static void smdmisModifyKeyValDesc( smdmisKeyRow       aKeyRow,
                                    stlInt64           aColLen,
                                    smdmifDefColumn   *aKeyCol )
{
    stlInt32  i;
    stlUInt8 *sPos;

    /* descending 처리: radix sort를 위해 키 값의 각 radix를 255-value로 치환한다. */
    for( i = 0, sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);
         i < aKeyCol->mAllocSize;
         i++, sPos++ )
    {   
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }   
}

static void smdmisRecoverKeyValDesc( smdmisKeyRow       aKeyRow,
                                     smdmifDefColumn   *aKeyCol )
{
    stlInt32  i;
    stlUInt8 *sPos;

    /* descending 처리에 대한 복구 */
    for( i = 0, sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);
         i < aKeyCol->mAllocSize;
         i++, sPos++ )
    {  
        /* 다시 한번 255-value 하면 원래 값으로 복구된다. */
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
}

static void smdmisModifyKeyValNumericDesc( smdmisKeyRow       aKeyRow,
                                           stlInt64           aColLen,
                                           smdmifDefColumn   *aKeyCol )
{
    stlInt32  i;
    stlUInt8 *sPos;
    stlUInt8 *sPosFence;

    if( aColLen < aKeyCol->mAllocSize )
    {
        sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);

        if( *sPos < SMDMIS_NUMERIC_MINUS_SIGN )
        {
            /* 음수일 경우엔 0-pad가 아니라 ff-pad를 붙여야 한다.
               -101과 -100을 비교할 때를 생각해보면 된다. */
            sPosFence = sPos + aKeyCol->mAllocSize;
            for( sPos = sPos + aColLen;
                 sPos < sPosFence;
                 sPos++ )
            {
                *sPos = SMDMIS_NUMERIC_MINUS_PAD;
            }
        }
    }

    /* descending 처리: radix sort를 위해 키 값의 각 radix를 255-value로 치환한다. */
    for( i = 0, sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);
         i < aKeyCol->mAllocSize;
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
}

static void smdmisModifyKeyValNullDesc( smdmisKeyRow       aKeyRow,
                                        stlInt64           aColLen,
                                        smdmifDefColumn   *aKeyCol )
{
    stlInt32  i;
    stlUInt8 *sPos;

    /* NULL 처리 */
    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        /* descending 처리: radix sort를 위해 키 값의 각 radix를 255-value로 치환한다. */
        for( i = 0, sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);
             i < aKeyCol->mAllocSize;
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }
    }
}

static void smdmisModifyKeyValNullFirstDesc( smdmisKeyRow       aKeyRow,
                                             stlInt64           aColLen,
                                             smdmifDefColumn   *aKeyCol )
{
    stlInt32  i;
    stlUInt8 *sPos;

    /* NULL 처리 */
    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        /* descending 처리: radix sort를 위해 키 값의 각 radix를 255-value로 치환한다. */
        for( i = 0, sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);
             i < aKeyCol->mAllocSize;
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }
    }
}

static void smdmisModifyKeyValNumericNullDesc( smdmisKeyRow       aKeyRow,
                                               stlInt64           aColLen,
                                               smdmifDefColumn   *aKeyCol )
{
    stlInt32  i;
    stlUInt8 *sPos;
    stlUInt8 *sPosFence;

    /* NULL 처리 */
    if( aColLen == 0 ) 
    {   
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NUMERIC_NULL( aKeyRow, aKeyCol );
    }   
    else
    {   
        /* 음수 처리 */
        if( aColLen < aKeyCol->mAllocSize )
        {
            sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);

            if( *sPos < SMDMIS_NUMERIC_MINUS_SIGN )
            {
                /* 음수일 경우엔 0-pad가 아니라 ff-pad를 붙여야 한다.
                   -101과 -100을 비교할 때를 생각해보면 된다. */
                sPosFence = sPos + aKeyCol->mAllocSize;
                for( sPos = sPos + aColLen;
                     sPos < sPosFence;
                     sPos++ )
                {
                    *sPos = SMDMIS_NUMERIC_MINUS_PAD;
                }
            }
        }

        /* descending 처리: radix sort를 위해 키 값의 각 radix를 255-value로 치환한다. */
        for( i = 0, sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);
             i < aKeyCol->mAllocSize;
             i++, sPos++ )
        {   
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }   
    }
}

static void smdmisModifyKeyValNumericNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                    stlInt64           aColLen,
                                                    smdmifDefColumn   *aKeyCol )
{
    stlInt32  i;
    stlUInt8 *sPos;
    stlUInt8 *sPosFence;

    /* NULL 처리 */
    if( aColLen == 0 )
    {  
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NUMERIC_NULL_FIRST( aKeyRow, aKeyCol );
    }  
    else
    {
        /* 음수 처리 */
        if( aColLen < aKeyCol->mAllocSize )
        {
            sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);

            if( *sPos < SMDMIS_NUMERIC_MINUS_SIGN )
            {
                /* 음수일 경우엔 0-pad가 아니라 ff-pad를 붙여야 한다.
                   -101과 -100을 비교할 때를 생각해보면 된다. */
                sPosFence = sPos + aKeyCol->mAllocSize;
                for( sPos = sPos + aColLen;
                     sPos < sPosFence;
                     sPos++ )
                {
                    *sPos = SMDMIS_NUMERIC_MINUS_PAD;
                }
            }
        }

        /* descending 처리: radix sort를 위해 키 값의 각 radix를 255-value로 치환한다. */
        for( i = 0, sPos = (stlUInt8 *)aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol);
             i < aKeyCol->mAllocSize;
             i++, sPos++ )
        {  
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }  
    }
}

static void smdmisModifyKeyValSmallInt( smdmisKeyRow       aKeyRow,
                                        stlInt64           aColLen,
                                        smdmifDefColumn   *aKeyCol )
{
    stlUInt16  sValue;
    stlUInt16 *sSourceVal = (stlUInt16 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));

    STL_WRITE_INT16( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_2BYTE;
#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT16( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[1];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[0];
#endif
}

static void smdmisRecoverKeyValSmallInt( smdmisKeyRow       aKeyRow,
                                         smdmifDefColumn   *aKeyCol )
{
    stlUInt16  sValue;
    stlUInt16 *sSourceVal = (stlUInt16 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT16( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[0];
#endif

    sValue -= SMDMIS_KEY_COMPLEMENT_2BYTE;
    STL_WRITE_INT16( sSourceVal, &sValue );
}

static void smdmisModifyKeyValInteger( smdmisKeyRow       aKeyRow,
                                       stlInt64           aColLen,
                                       smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));

    STL_WRITE_INT32( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
    ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
    ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
}

static void smdmisRecoverKeyValInteger( smdmisKeyRow       aKeyRow,
                                        smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[3];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[2];
    ((stlChar *)&sValue)[2] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[3] = ((stlChar *)(sSourceVal))[0];
#endif

    sValue -= SMDMIS_KEY_COMPLEMENT_4BYTE;
    STL_WRITE_INT32( sSourceVal, &sValue );
}

static void smdmisModifyKeyValBigInt( smdmisKeyRow       aKeyRow,
                                      stlInt64           aColLen,
                                      smdmifDefColumn   *aKeyCol )
{
    stlUInt64  sValue;
    stlUInt64 *sSourceVal = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));

    STL_WRITE_INT64( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_8BYTE;
#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT64( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[7];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[6];
    ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[5];
    ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[4];
    ((stlChar *)sSourceVal)[4] = ((stlChar *)(&sValue))[3];
    ((stlChar *)sSourceVal)[5] = ((stlChar *)(&sValue))[2];
    ((stlChar *)sSourceVal)[6] = ((stlChar *)(&sValue))[1];
    ((stlChar *)sSourceVal)[7] = ((stlChar *)(&sValue))[0];
#endif
}

static void smdmisRecoverKeyValBigInt( smdmisKeyRow       aKeyRow,
                                       smdmifDefColumn   *aKeyCol )
{                                     
    stlUInt64  sValue;
    stlUInt64 *sSourceVal = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT64( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[7];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[6];
    ((stlChar *)&sValue)[2] = ((stlChar *)(sSourceVal))[5];
    ((stlChar *)&sValue)[3] = ((stlChar *)(sSourceVal))[4];
    ((stlChar *)&sValue)[4] = ((stlChar *)(sSourceVal))[3];
    ((stlChar *)&sValue)[5] = ((stlChar *)(sSourceVal))[2];
    ((stlChar *)&sValue)[6] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[7] = ((stlChar *)(sSourceVal))[0];
#endif

    sValue -= SMDMIS_KEY_COMPLEMENT_8BYTE;
    STL_WRITE_INT64( sSourceVal, &sValue );
}

static void smdmisModifyKeyValIntervalYM( smdmisKeyRow       aKeyRow,
                                          stlInt64           aColLen,
                                          smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalYearToMonthType, mMonth));

    STL_WRITE_INT32( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
    ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
    ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
}

static void smdmisRecoverKeyValIntervalYM( smdmisKeyRow       aKeyRow,
                                           smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalYearToMonthType, mMonth));

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[3];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[2];
    ((stlChar *)&sValue)[2] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[3] = ((stlChar *)(sSourceVal))[0];
#endif

    sValue -= SMDMIS_KEY_COMPLEMENT_4BYTE;
    STL_WRITE_INT32( sSourceVal, &sValue );
}

static void smdmisModifyKeyValIntervalDS( smdmisKeyRow       aKeyRow,
                                          stlInt64           aColLen,
                                          smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt64  sValue2;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mDay));
    stlUInt64 *sSourceVal2 = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mTime));
                                  
    STL_WRITE_INT32( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
    ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];  
    ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];  
#endif

    STL_WRITE_INT64( &sValue2, sSourceVal2 );
    sValue2 += SMDMIS_KEY_COMPLEMENT_8BYTE;
#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT64( sSourceVal2, &sValue2 );
#else
    ((stlChar *)sSourceVal2)[0] = ((stlChar *)(&sValue2))[7];
    ((stlChar *)sSourceVal2)[1] = ((stlChar *)(&sValue2))[6];
    ((stlChar *)sSourceVal2)[2] = ((stlChar *)(&sValue2))[5];
    ((stlChar *)sSourceVal2)[3] = ((stlChar *)(&sValue2))[4];
    ((stlChar *)sSourceVal2)[4] = ((stlChar *)(&sValue2))[3];
    ((stlChar *)sSourceVal2)[5] = ((stlChar *)(&sValue2))[2];
    ((stlChar *)sSourceVal2)[6] = ((stlChar *)(&sValue2))[1];
    ((stlChar *)sSourceVal2)[7] = ((stlChar *)(&sValue2))[0];
#endif
}

static void smdmisRecoverKeyValIntervalDS( smdmisKeyRow       aKeyRow,
                                           smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt64  sValue2;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mDay));
    stlUInt64 *sSourceVal2 = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mTime));

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[3];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[2];
    ((stlChar *)&sValue)[2] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[3] = ((stlChar *)(sSourceVal))[0];
#endif

    sValue -= SMDMIS_KEY_COMPLEMENT_4BYTE;
    STL_WRITE_INT32( sSourceVal, &sValue );

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT64( &sValue2, sSourceVal2 );
#else
    ((stlChar *)&sValue2)[0] = ((stlChar *)(sSourceVal2))[7];
    ((stlChar *)&sValue2)[1] = ((stlChar *)(sSourceVal2))[6];
    ((stlChar *)&sValue2)[2] = ((stlChar *)(sSourceVal2))[5];
    ((stlChar *)&sValue2)[3] = ((stlChar *)(sSourceVal2))[4];
    ((stlChar *)&sValue2)[4] = ((stlChar *)(sSourceVal2))[3];
    ((stlChar *)&sValue2)[5] = ((stlChar *)(sSourceVal2))[2];
    ((stlChar *)&sValue2)[6] = ((stlChar *)(sSourceVal2))[1];
    ((stlChar *)&sValue2)[7] = ((stlChar *)(sSourceVal2))[0];
#endif

    sValue2 -= SMDMIS_KEY_COMPLEMENT_8BYTE;
    STL_WRITE_INT64( sSourceVal2, &sValue2 );
}

static void smdmisModifyKeyValSmallIntNull( smdmisKeyRow       aKeyRow,
                                            stlInt64           aColLen,
                                            smdmifDefColumn   *aKeyCol )
{
    stlUInt16  sValue;
    stlUInt16 *sSourceVal;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt16 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT16( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_2BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT16( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntegerNull( smdmisKeyRow       aKeyRow,
                                           stlInt64           aColLen,
                                           smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValBigIntNull( smdmisKeyRow       aKeyRow,
                                          stlInt64           aColLen,
                                          smdmifDefColumn   *aKeyCol )
{
    stlUInt64  sValue;
    stlUInt64 *sSourceVal;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT64( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_8BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT64( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[7];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[6];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[5];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[4];
        ((stlChar *)sSourceVal)[4] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[5] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[6] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[7] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntervalYMNull( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalYearToMonthType, mMonth));
        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntervalDSNull( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt64  sValue2;
    stlUInt32 *sSourceVal;
    stlUInt64 *sSourceVal2;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mDay));
        sSourceVal2 = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mTime));

        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif

        STL_WRITE_INT64( &sValue2, sSourceVal2 );
        sValue2 += SMDMIS_KEY_COMPLEMENT_8BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT64( sSourceVal2, &sValue2 );
#else
        ((stlChar *)sSourceVal2)[0] = ((stlChar *)(&sValue2))[7];
        ((stlChar *)sSourceVal2)[1] = ((stlChar *)(&sValue2))[6];
        ((stlChar *)sSourceVal2)[2] = ((stlChar *)(&sValue2))[5];
        ((stlChar *)sSourceVal2)[3] = ((stlChar *)(&sValue2))[4];
        ((stlChar *)sSourceVal2)[4] = ((stlChar *)(&sValue2))[3];
        ((stlChar *)sSourceVal2)[5] = ((stlChar *)(&sValue2))[2];
        ((stlChar *)sSourceVal2)[6] = ((stlChar *)(&sValue2))[1];
        ((stlChar *)sSourceVal2)[7] = ((stlChar *)(&sValue2))[0];
#endif
    }
}

static void smdmisModifyKeyValSmallIntNullFirst( smdmisKeyRow       aKeyRow,
                                                 stlInt64           aColLen,
                                                 smdmifDefColumn   *aKeyCol )
{
    stlUInt16  sValue;
    stlUInt16 *sSourceVal;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt16 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT16( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_2BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT16( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntegerNullFirst( smdmisKeyRow       aKeyRow,
                                                stlInt64           aColLen,
                                                smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValBigIntNullFirst( smdmisKeyRow       aKeyRow,
                                               stlInt64           aColLen,
                                               smdmifDefColumn   *aKeyCol )
{
    stlUInt64  sValue;
    stlUInt64 *sSourceVal;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT64( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_8BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT64( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[7];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[6];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[5];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[4];
        ((stlChar *)sSourceVal)[4] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[5] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[6] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[7] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntervalYMNullFirst( smdmisKeyRow       aKeyRow,
                                                   stlInt64           aColLen,
                                                   smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalYearToMonthType, mMonth));
        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntervalDSNullFirst( smdmisKeyRow       aKeyRow,
                                                   stlInt64           aColLen,
                                                   smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt64  sValue2;
    stlUInt32 *sSourceVal;
    stlUInt64 *sSourceVal2;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mDay));
        sSourceVal2 = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mTime));

        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif

        STL_WRITE_INT64( &sValue2, sSourceVal2 );
        sValue2 += SMDMIS_KEY_COMPLEMENT_8BYTE;
#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT64( sSourceVal2, &sValue2 );
#else
        ((stlChar *)sSourceVal2)[0] = ((stlChar *)(&sValue2))[7];
        ((stlChar *)sSourceVal2)[1] = ((stlChar *)(&sValue2))[6];
        ((stlChar *)sSourceVal2)[2] = ((stlChar *)(&sValue2))[5];
        ((stlChar *)sSourceVal2)[3] = ((stlChar *)(&sValue2))[4];
        ((stlChar *)sSourceVal2)[4] = ((stlChar *)(&sValue2))[3];
        ((stlChar *)sSourceVal2)[5] = ((stlChar *)(&sValue2))[2];
        ((stlChar *)sSourceVal2)[6] = ((stlChar *)(&sValue2))[1];
        ((stlChar *)sSourceVal2)[7] = ((stlChar *)(&sValue2))[0];
#endif
    }
}

static void smdmisModifyKeyValSmallIntDesc( smdmisKeyRow       aKeyRow,
                                            stlInt64           aColLen,
                                            smdmifDefColumn   *aKeyCol )
{
    stlUInt16  sValue;
    stlUInt16 *sSourceVal = (stlUInt16 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
    stlInt32   i;
    stlUInt8  *sPos;

    STL_WRITE_INT16( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_2BYTE;

    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt16 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT16( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[1];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[0];
#endif
}

static void smdmisRecoverKeyValSmallIntDesc( smdmisKeyRow       aKeyRow,
                                             smdmifDefColumn   *aKeyCol )
{
    stlUInt16  sValue;
    stlUInt16 *sSourceVal = (stlUInt16 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
    stlInt32   i;
    stlUInt8  *sPos;

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT16( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[0];
#endif
    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt16 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
    sValue -= SMDMIS_KEY_COMPLEMENT_2BYTE;
    STL_WRITE_INT16( sSourceVal, &sValue );
}

static void smdmisModifyKeyValIntegerDesc( smdmisKeyRow       aKeyRow,
                                           stlInt64           aColLen,
                                           smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
    stlInt32   i;
    stlUInt8  *sPos;

    STL_WRITE_INT32( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt32 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
    ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
    ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
}

static void smdmisRecoverKeyValIntegerDesc( smdmisKeyRow       aKeyRow,
                                            smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
    stlInt32   i;
    stlUInt8  *sPos;

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[3];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[2];
    ((stlChar *)&sValue)[2] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[3] = ((stlChar *)(sSourceVal))[0];
#endif
    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt32 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
    sValue -= SMDMIS_KEY_COMPLEMENT_4BYTE;
    STL_WRITE_INT32( sSourceVal, &sValue );
}

static void smdmisModifyKeyValBigIntDesc( smdmisKeyRow       aKeyRow,
                                          stlInt64           aColLen,
                                          smdmifDefColumn   *aKeyCol )
{
    stlUInt64  sValue;
    stlUInt64 *sSourceVal = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
    stlInt32   i;
    stlUInt8  *sPos;

    STL_WRITE_INT64( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_8BYTE;

    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt64 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT64( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[7];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[6];
    ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[5];
    ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[4];
    ((stlChar *)sSourceVal)[4] = ((stlChar *)(&sValue))[3];
    ((stlChar *)sSourceVal)[5] = ((stlChar *)(&sValue))[2];
    ((stlChar *)sSourceVal)[6] = ((stlChar *)(&sValue))[1];
    ((stlChar *)sSourceVal)[7] = ((stlChar *)(&sValue))[0];
#endif
}

static void smdmisRecoverKeyValBigIntDesc( smdmisKeyRow       aKeyRow,
                                           smdmifDefColumn   *aKeyCol )
{   
    stlUInt64  sValue;
    stlUInt64 *sSourceVal = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
    stlInt32   i;
    stlUInt8  *sPos;

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT64( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[7];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[6];
    ((stlChar *)&sValue)[2] = ((stlChar *)(sSourceVal))[5];
    ((stlChar *)&sValue)[3] = ((stlChar *)(sSourceVal))[4];
    ((stlChar *)&sValue)[4] = ((stlChar *)(sSourceVal))[3];
    ((stlChar *)&sValue)[5] = ((stlChar *)(sSourceVal))[2];
    ((stlChar *)&sValue)[6] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[7] = ((stlChar *)(sSourceVal))[0];
#endif
    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt64 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
    sValue -= SMDMIS_KEY_COMPLEMENT_8BYTE;
    STL_WRITE_INT64( sSourceVal, &sValue );
}

static void smdmisModifyKeyValIntervalYMDesc( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalYearToMonthType, mMonth));
    stlInt32   i;
    stlUInt8  *sPos;

    STL_WRITE_INT32( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt32 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
    ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
    ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
}

static void smdmisRecoverKeyValIntervalYMDesc( smdmisKeyRow       aKeyRow,
                                               smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalYearToMonthType, mMonth));
    stlInt32   i;
    stlUInt8  *sPos;

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[3];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[2];
    ((stlChar *)&sValue)[2] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[3] = ((stlChar *)(sSourceVal))[0];
#endif
    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt32 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
    sValue -= SMDMIS_KEY_COMPLEMENT_4BYTE;
    STL_WRITE_INT32( sSourceVal, &sValue );
}

static void smdmisModifyKeyValIntervalDSDesc( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt64  sValue2;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mDay));
    stlUInt64 *sSourceVal2 = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mTime));
    stlInt32   i;
    stlUInt8  *sPos;

    STL_WRITE_INT32( &sValue, sSourceVal );
    sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt32 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( sSourceVal, &sValue );
#else
    ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
    ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
    ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];      
    ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif

    STL_WRITE_INT64( &sValue2, sSourceVal2 );
    sValue2 += SMDMIS_KEY_COMPLEMENT_8BYTE;

    for( i = 0, sPos = (stlUInt8 *)(&sValue2);
         i < STL_SIZEOF( stlUInt64 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT64( sSourceVal2, &sValue2 );
#else
    ((stlChar *)sSourceVal2)[0] = ((stlChar *)(&sValue2))[7];
    ((stlChar *)sSourceVal2)[1] = ((stlChar *)(&sValue2))[6];
    ((stlChar *)sSourceVal2)[2] = ((stlChar *)(&sValue2))[5];
    ((stlChar *)sSourceVal2)[3] = ((stlChar *)(&sValue2))[4];
    ((stlChar *)sSourceVal2)[4] = ((stlChar *)(&sValue2))[3];
    ((stlChar *)sSourceVal2)[5] = ((stlChar *)(&sValue2))[2];
    ((stlChar *)sSourceVal2)[6] = ((stlChar *)(&sValue2))[1];
    ((stlChar *)sSourceVal2)[7] = ((stlChar *)(&sValue2))[0];
#endif
}

static void smdmisRecoverKeyValIntervalDSDesc( smdmisKeyRow       aKeyRow,
                                               smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt64  sValue2;
    stlUInt32 *sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mDay));
    stlUInt64 *sSourceVal2 = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mTime));
    stlInt32   i;
    stlUInt8  *sPos;

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT32( &sValue, sSourceVal );
#else
    ((stlChar *)&sValue)[0] = ((stlChar *)(sSourceVal))[3];
    ((stlChar *)&sValue)[1] = ((stlChar *)(sSourceVal))[2];
    ((stlChar *)&sValue)[2] = ((stlChar *)(sSourceVal))[1];
    ((stlChar *)&sValue)[3] = ((stlChar *)(sSourceVal))[0];
#endif
    for( i = 0, sPos = (stlUInt8 *)(&sValue);
         i < STL_SIZEOF( stlUInt32 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
    sValue -= SMDMIS_KEY_COMPLEMENT_4BYTE;
    STL_WRITE_INT32( sSourceVal, &sValue );

#ifdef STL_IS_BIGENDIAN
    STL_WRITE_INT64( &sValue2, sSourceVal2 );
#else
    ((stlChar *)&sValue2)[0] = ((stlChar *)(sSourceVal2))[7];
    ((stlChar *)&sValue2)[1] = ((stlChar *)(sSourceVal2))[6];
    ((stlChar *)&sValue2)[2] = ((stlChar *)(sSourceVal2))[5];
    ((stlChar *)&sValue2)[3] = ((stlChar *)(sSourceVal2))[4];
    ((stlChar *)&sValue2)[4] = ((stlChar *)(sSourceVal2))[3];
    ((stlChar *)&sValue2)[5] = ((stlChar *)(sSourceVal2))[2];
    ((stlChar *)&sValue2)[6] = ((stlChar *)(sSourceVal2))[1];
    ((stlChar *)&sValue2)[7] = ((stlChar *)(sSourceVal2))[0];
#endif
    for( i = 0, sPos = (stlUInt8 *)(&sValue2);
         i < STL_SIZEOF( stlUInt64 );
         i++, sPos++ )
    {
        *sPos = SMDMIS_RADIX_RANGE - *sPos;
    }
    sValue2 -= SMDMIS_KEY_COMPLEMENT_8BYTE;
    STL_WRITE_INT64( sSourceVal2, &sValue2 );
}

static void smdmisModifyKeyValSmallIntNullDesc( smdmisKeyRow       aKeyRow,
                                                stlInt64           aColLen,
                                                smdmifDefColumn   *aKeyCol )
{
    stlUInt16  sValue;
    stlUInt16 *sSourceVal;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt16 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT16( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_2BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt16 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT16( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntegerNullDesc( smdmisKeyRow       aKeyRow,
                                               stlInt64           aColLen,
                                               smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt32 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValBigIntNullDesc( smdmisKeyRow       aKeyRow,
                                              stlInt64           aColLen,
                                              smdmifDefColumn   *aKeyCol )
{
    stlUInt64  sValue;
    stlUInt64 *sSourceVal;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT64( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_8BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt64 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT64( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[7];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[6];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[5];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[4];
        ((stlChar *)sSourceVal)[4] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[5] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[6] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[7] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntervalYMNullDesc( smdmisKeyRow       aKeyRow,
                                                  stlInt64           aColLen,
                                                  smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalYearToMonthType, mMonth));
        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt32 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntervalDSNullDesc( smdmisKeyRow       aKeyRow,
                                                  stlInt64           aColLen,
                                                  smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt64  sValue2;
    stlUInt32 *sSourceVal;
    stlUInt64 *sSourceVal2;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mDay));
        sSourceVal2 = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mTime));

        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt32 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];      
#endif

        STL_WRITE_INT64( &sValue2, sSourceVal2 );
        sValue2 += SMDMIS_KEY_COMPLEMENT_8BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue2);
             i < STL_SIZEOF( stlUInt64 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT64( sSourceVal2, &sValue2 );
#else
        ((stlChar *)sSourceVal2)[0] = ((stlChar *)(&sValue2))[7];
        ((stlChar *)sSourceVal2)[1] = ((stlChar *)(&sValue2))[6];
        ((stlChar *)sSourceVal2)[2] = ((stlChar *)(&sValue2))[5];
        ((stlChar *)sSourceVal2)[3] = ((stlChar *)(&sValue2))[4];
        ((stlChar *)sSourceVal2)[4] = ((stlChar *)(&sValue2))[3];
        ((stlChar *)sSourceVal2)[5] = ((stlChar *)(&sValue2))[2];
        ((stlChar *)sSourceVal2)[6] = ((stlChar *)(&sValue2))[1];
        ((stlChar *)sSourceVal2)[7] = ((stlChar *)(&sValue2))[0];
#endif
    }
}

static void smdmisModifyKeyValSmallIntNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                     stlInt64           aColLen,
                                                     smdmifDefColumn   *aKeyCol )
{
    stlUInt16  sValue;
    stlUInt16 *sSourceVal;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt16 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT16( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_2BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt16 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT16( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntegerNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                    stlInt64           aColLen,
                                                    smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt32 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValBigIntNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                   stlInt64           aColLen,
                                                   smdmifDefColumn   *aKeyCol )
{
    stlUInt64  sValue;
    stlUInt64 *sSourceVal;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol));
        STL_WRITE_INT64( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_8BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt64 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT64( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[7];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[6];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[5];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[4];
        ((stlChar *)sSourceVal)[4] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[5] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[6] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[7] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntervalYMNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                       stlInt64           aColLen,
                                                       smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt32 *sSourceVal;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalYearToMonthType, mMonth));
        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt32 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif
    }
}

static void smdmisModifyKeyValIntervalDSNullFirstDesc( smdmisKeyRow       aKeyRow,
                                                       stlInt64           aColLen,
                                                       smdmifDefColumn   *aKeyCol )
{
    stlUInt32  sValue;
    stlUInt64  sValue2;
    stlUInt32 *sSourceVal;
    stlUInt64 *sSourceVal2;
    stlInt32   i;
    stlUInt8  *sPos;

    if( aColLen == 0 )
    {
        /* null 값을 세팅한다. */
        SMDMIS_KEY_ROW_SET_NULL_FIRST( aKeyRow, aKeyCol );
    }
    else
    {
        SMDMIS_KEY_ROW_SET_NOT_NULL_FIRST( aKeyRow, aKeyCol );

        sSourceVal = (stlUInt32 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mDay));
        sSourceVal2 = (stlUInt64 *)(aKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE(aKeyCol) + STL_OFFSETOF(dtlIntervalDayToSecondType, mTime));

        STL_WRITE_INT32( &sValue, sSourceVal );
        sValue += SMDMIS_KEY_COMPLEMENT_4BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue);
             i < STL_SIZEOF( stlUInt32 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT32( sSourceVal, &sValue );
#else
        ((stlChar *)sSourceVal)[0] = ((stlChar *)(&sValue))[3];
        ((stlChar *)sSourceVal)[1] = ((stlChar *)(&sValue))[2];
        ((stlChar *)sSourceVal)[2] = ((stlChar *)(&sValue))[1];
        ((stlChar *)sSourceVal)[3] = ((stlChar *)(&sValue))[0];
#endif

        STL_WRITE_INT64( &sValue2, sSourceVal2 );
        sValue2 += SMDMIS_KEY_COMPLEMENT_8BYTE;

        for( i = 0, sPos = (stlUInt8 *)(&sValue2);
             i < STL_SIZEOF( stlUInt64 );
             i++, sPos++ )
        {
            *sPos = SMDMIS_RADIX_RANGE - *sPos;
        }

#ifdef STL_IS_BIGENDIAN
        STL_WRITE_INT64( sSourceVal2, &sValue2 );
#else
        ((stlChar *)sSourceVal2)[0] = ((stlChar *)(&sValue2))[7];
        ((stlChar *)sSourceVal2)[1] = ((stlChar *)(&sValue2))[6];
        ((stlChar *)sSourceVal2)[2] = ((stlChar *)(&sValue2))[5];
        ((stlChar *)sSourceVal2)[3] = ((stlChar *)(&sValue2))[4];
        ((stlChar *)sSourceVal2)[4] = ((stlChar *)(&sValue2))[3];
        ((stlChar *)sSourceVal2)[5] = ((stlChar *)(&sValue2))[2];
        ((stlChar *)sSourceVal2)[6] = ((stlChar *)(&sValue2))[1];
        ((stlChar *)sSourceVal2)[7] = ((stlChar *)(&sValue2))[0];
#endif
    }
}

/**
 * aInputNodeList는 정렬되지 않은 단방향 node 리스트(mNextOverflow로 연결)이다.
 * 이 함수는 이 node 리스트의 키들을 정렬된 단방향 node 리스트(mNextGreater로 연결)로 만들어준다.
 * 정렬된 리스트는 aSortedNodeList로 반환된다.
 */
static stlStatus smdmisRadixSort( smdmisTableHeader *aHeader,
                                  smdmisRadixBucket *aBucket,
                                  stlUInt16          aBucketSize,
                                  smdmisRadixNode  **aRadixList,
                                  smdmisIndexNode  **aSortDone,
                                  smlEnv            *aEnv )
{
    /**
      용어 정리
       - group: 하나의 mNextOverflow로 연결된 노드들. 정렬되어 있지 않음
                group들 간에는 서로 크기 비교가 가능하기 때문에 mNextGreater로 연결된다.
       - bucket: radix(지수)로 분류하기 위한 array의 한 칸을 의미. 한 버킷에는 한 그룹이 저장된다.
     */
    smdmisRadixNode   *sNode;              /* 한 그룹내에서 overflow 리스트를 따라가며 탐색할 때 사용 */
    smdmisRadixNode   *sTempNode;          /* nextOverflow로 이동할 때 이전 노드를 free하기 위해 임시로 사용되는 변수 */
    smdmisRadixNode   *sNextGreaterNode;   /* 현재 노드(sNode)의 mNextGreater를 따놓은 변수 */
    smdmisRadixNode   *sRadixSortedFirst;  /* 한 그룹을 radix sort 했을 때, greater 리스트의 맨 처음을 가리킴 */
    smdmisRadixNode   *sRadixSortedLast;   /* 한 그룹을 radix sort 했을 때, greater 리스트의 맨 마지막을 가리킴 */
    smdmisRadixNode   *sNewNode;           /* 새로 node를 할당할 때 사용되는 변수 */
    stlUInt16          sKeyIndex = 0;
    stlUInt16          sRadixIndex;
    smdmisKeyRow       sKeyRow;
    stlUInt16          sRadix;
    stlUInt8           sRadixValue;

    sNode = *aRadixList;
    while( sNode != NULL )
    {
        sNextGreaterNode = sNode->mNextGreater;

        /* overflow를 가진 노드를 만났다.
           그룹에 대한 radix sort를 수행한다. */
        sRadixIndex = sNode->mUsedRadixIndex + 1;
        sRadix = aHeader->mRadixArray[sRadixIndex];
        sKeyIndex = 0;
        SMDMIS_RADIX_NODE_GET_NEXT_KEY( aHeader, sNode, sTempNode, sKeyIndex, sKeyRow );
        while( sKeyRow != NULL )
        {
            sRadixValue = SMDMIS_KEY_ROW_GET_RADIX( sKeyRow, sRadix );

            if( (aBucket[sRadixValue].mLast == NULL) ||
                (SMDMIS_NODE_IS_FULL(aBucket[sRadixValue].mLast) == STL_TRUE) )
            {
                STL_TRY( smdmisAllocRadixNode( aHeader,
                                               &sNewNode,
                                               aEnv ) == STL_SUCCESS );

                smdmisRadixNodeInit( sNewNode,
                                     sRadixIndex,
                                     aHeader->mNodeSize,
                                     aHeader->mKeyRowLen );

                if( aBucket[sRadixValue].mLast == NULL )
                {
                    aBucket[sRadixValue].mFirst = sNewNode;
                }
                else
                {
                    aBucket[sRadixValue].mLast->mNextOverflow = sNewNode;
                }

                aBucket[sRadixValue].mLast = sNewNode;
            }

            SMDMIS_NODE_APPEND_KEY( aBucket[sRadixValue].mLast, sKeyRow, aHeader->mKeyRowLen );

            SMDMIS_RADIX_NODE_GET_NEXT_KEY( aHeader, sNode, sTempNode, sKeyIndex, sKeyRow );
        }

        STL_TRY( smdmisMakeNodeListFromBucket( aHeader,
                                               aBucket,
                                               aBucketSize,
                                               &sRadixSortedFirst,
                                               &sRadixSortedLast,
                                               aSortDone,
                                               aEnv ) == STL_SUCCESS );

        if( sRadixSortedLast == NULL )
        {
            /* bucket의 노드들이 모두 sort 완료되어 index node로 옮겨졌다.
               리스트의 다음 노드들에 대해 단일 노드이거나 radix sort를 완료한
               노드들을 처리한다. */

            sNode = sNextGreaterNode;
            while( sNode != NULL )
            {
                if( SMDMIS_RADIX_NODE_SORT_DONE( sNode, aHeader->mRadixIndexFence ) == STL_TRUE )
                {
                    /* 이미 radix sort를 끝낸 노드이다. overflow가 달려 있더라도,
                       모두 같은 값들이다. */
                    sNextGreaterNode = sNode->mNextGreater;

                    STL_TRY( smdmisRadixToIndex( aHeader,
                                                 sNode,
                                                 STL_TRUE, /* set dup */
                                                 aSortDone,
                                                 aEnv ) == STL_SUCCESS );

                    sNode = sNextGreaterNode;
                    continue;
                }
                else if( SMDMIS_RADIX_NODE_INPLACE_SORTABLE( sNode ) == STL_TRUE )
                {
                    sNextGreaterNode = sNode->mNextGreater;

                    smdmisRadixNodeSort( aHeader,
                                         sNode,
                                         sNode->mUsedRadixIndex + 1 );

                    STL_TRY( smdmisRadixToIndex( aHeader,
                                                 sNode,
                                                 STL_FALSE, /* set dup */
                                                 aSortDone,
                                                 aEnv ) == STL_SUCCESS );

                    sNode = sNextGreaterNode;
                    continue;
                }

                break;
            }
        }
        else
        {
            /* 새로 만들어진 리스트를 이전 리스트와 연결한다. */
            sRadixSortedLast->mNextGreater = sNextGreaterNode;
            sNode = sRadixSortedFirst;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisMakeNodeListFromBucket( smdmisTableHeader *aHeader,
                                               smdmisRadixBucket *aBucket,
                                               stlInt32           aBucketSize,
                                               smdmisRadixNode  **aResultNodeListFirst,
                                               smdmisRadixNode  **aResultNodeListLast,
                                               smdmisIndexNode  **aSortDone,
                                               smlEnv            *aEnv )
{
    stlInt32 i;
    stlBool  sCompact = STL_TRUE;

    *aResultNodeListFirst = NULL;
    *aResultNodeListLast = NULL;

    /* bucket의 노드들을 nextGreater 리스트로 묶고, bucket을 비운다. */
    for( i = 0; i < aBucketSize; i++ )
    {
        if( aBucket[i].mFirst != NULL )
        {
            if( sCompact == STL_TRUE )
            {
                if( SMDMIS_RADIX_NODE_SORT_DONE( aBucket[i].mFirst, aHeader->mRadixIndexFence ) == STL_TRUE )
                {
                    /* 이미 radix sort를 끝낸 노드이다. overflow가 달려 있더라도,
                       모두 같은 값들이다. */
                    STL_TRY( smdmisRadixToIndex( aHeader,
                                                 aBucket[i].mFirst,
                                                 STL_TRUE, /* set dup */
                                                 aSortDone,
                                                 aEnv ) == STL_SUCCESS );
                }
                else if( SMDMIS_RADIX_NODE_INPLACE_SORTABLE( aBucket[i].mFirst ) == STL_TRUE )
                {
                    smdmisRadixNodeSort( aHeader,
                                         aBucket[i].mFirst,
                                         aBucket[i].mFirst->mUsedRadixIndex + 1 );

                    STL_TRY( smdmisRadixToIndex( aHeader,
                                                 aBucket[i].mFirst,
                                                 STL_FALSE, /* set dup */
                                                 aSortDone,
                                                 aEnv ) == STL_SUCCESS );
                }
                else
                {
                    sCompact = STL_FALSE;
                }
            }

            if( sCompact == STL_FALSE )
            {
                if( *aResultNodeListFirst == NULL )
                {
                    *aResultNodeListFirst = aBucket[i].mFirst;
                }
                else
                {
                    (*aResultNodeListLast)->mNextGreater = aBucket[i].mFirst;
                }
                *aResultNodeListLast = aBucket[i].mFirst;
            }

            aBucket[i].mFirst = NULL;
            aBucket[i].mLast = NULL;
        }
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static void smdmisRadixNodeSort( smdmisTableHeader *aHeader,
                                 smdmisRadixNode   *aNode,
                                 stlUInt16          aRadixIndex )
{
    smdmisSortItem  *sStackList[SMDMIS_QSORT_STACK_SIZE];
    stlInt32         sStackElemCount[SMDMIS_QSORT_STACK_SIZE];
    stlUInt16        sStackRadixIndex[SMDMIS_QSORT_STACK_SIZE];
    stlInt32         sStackPos = -1;

    stlChar         *sRowBase = (stlChar *)aNode;

    smdmisSortItem  *sList;
    stlInt32         sElemCount;
    stlUInt16        sRadixIndex;
    stlUInt16        sLocalRadixIndex;

    stlInt32         i;
    stlInt32         j;
    smdmisSortItem   sSave;

    stlInt32         sMid;
    stlInt32         sCurLeft;
    smdmisSortItem   sPivotVal;
    stlInt32         sRightEqual;
    stlInt32         sLeftSmall;
    stlInt32         sSmallLen;
    stlInt32         sEqualLen;
    stlInt32         sBigLen;
    stlInt32         sMinLen;
    dtlCompareResult sCompResult;

    stlUInt8         sRadix1;
    stlUInt8         sRadix2;

    /* Terminate on small subfiles */
    sList = aNode->mSortItems;
    sElemCount = aNode->mKeyCount;
    sRadixIndex = aRadixIndex;

    STL_TRY_THROW( sElemCount > 1, RAMP_FINISH );

    while( STL_TRUE )
    {
        if( sRadixIndex == aHeader->mRadixIndexFence )
        {
            /* 모든 element가 같은 값임. 두번째 element부터 dup를 세팅한다.  */
            for( i = 1; i < sElemCount; i++ )
            {
                SMDMIS_KEY_ROW_SET_DUP( sRowBase + sList[i] );
            }

            STL_THROW( RAMP_NEXT_STACK );
        }

        STL_DASSERT( sRadixIndex < aHeader->mRadixIndexFence );

        if( sElemCount < SMDMIS_QSORT_CUTOFF )
        {
            /**
             * element 개수가 SMDMIS_QSORT_CUTOFF보다 작을 경우
             * quick sort보다 insertion sort가 더 효율적이므로
             * insertion sort로 처리하도록 한다.
             */
            for( i = 1; i <= sElemCount - 1; i++ )
            {
                SMDMIS_COMPARE_KEY( sRowBase + sList[i - 1],
                                    sRowBase + sList[i],
                                    sRadix1,
                                    sRadix2,
                                    aHeader->mRadixArray[sRadixIndex],
                                    sCompResult );

                if( sCompResult == DTL_COMPARISON_LESS )
                {
                    continue;
                }
                else if( sCompResult == DTL_COMPARISON_EQUAL )
                {
                    sLocalRadixIndex = sRadixIndex + 1;
                    while( (sCompResult == DTL_COMPARISON_EQUAL) && (sLocalRadixIndex < aHeader->mRadixIndexFence ) )
                    {
                        SMDMIS_COMPARE_KEY( sRowBase + sList[i - 1],
                                            sRowBase + sList[i],
                                            sRadix1,
                                            sRadix2,
                                            aHeader->mRadixArray[sLocalRadixIndex],
                                            sCompResult );

                        sLocalRadixIndex++;
                    }

                    if( sCompResult == DTL_COMPARISON_LESS )
                    {
                        continue;
                    }
                    else if( sCompResult == DTL_COMPARISON_EQUAL )
                    {
                        SMDMIS_KEY_ROW_SET_DUP( sRowBase + sList[i] );
                        continue;
                    }
                }

                sSave = sList[i];
                sList[i] = sList[i - 1];

                for( j = i - 1; j > 0; j-- )
                {
                    SMDMIS_COMPARE_KEY( sRowBase + sList[j - 1],
                                        sRowBase + sSave,
                                        sRadix1,
                                        sRadix2,
                                        aHeader->mRadixArray[sRadixIndex],
                                        sCompResult );

                    if( sCompResult == DTL_COMPARISON_LESS )
                    {
                        break;
                    }
                    else if( sCompResult == DTL_COMPARISON_EQUAL )
                    {
                        sLocalRadixIndex = sRadixIndex + 1;
                        while( (sCompResult == DTL_COMPARISON_EQUAL) && (sLocalRadixIndex < aHeader->mRadixIndexFence ) )
                        {
                            SMDMIS_COMPARE_KEY( sRowBase + sList[j - 1],
                                                sRowBase + sSave,
                                                sRadix1,
                                                sRadix2,
                                                aHeader->mRadixArray[sLocalRadixIndex],
                                                sCompResult );

                            sLocalRadixIndex++;
                        }

                        if( sCompResult == DTL_COMPARISON_LESS )
                        {
                            break; 
                        }
                        else if( sCompResult == DTL_COMPARISON_EQUAL )
                        {
                            SMDMIS_KEY_ROW_SET_DUP( sRowBase + sSave );
                            break;
                        }
                    }

                    sList[j] = sList[j - 1];
                }

                sList[j] = sSave;
            }

            STL_THROW( RAMP_NEXT_STACK );
        }

        /**
         * ascending으로 정렬된 데이터가 들어오는 경우
         * first value를 pivot value로 설정하게 되면
         * 데이터 개수만큼 loop를 돌아야 하는 경우가 발생한다.
         * 이는 최악의 성능을 가져오게 되는데, 이를 방지하기 위하여
         * first value, mid value, last value들에 대하여
         * 정렬상 중간값을 pivot value로 한다.
         */
        /* Find the median of three values in list, use it as the pivot */
        sMid = sElemCount / 2;
        SMDMIS_COMPARE_KEY( sRowBase + sList[0],
                            sRowBase + sList[sMid],
                            sRadix1,
                            sRadix2,
                            aHeader->mRadixArray[sRadixIndex],
                            sCompResult );

        if( sCompResult > 0 )
        {
            SMDMIS_SWAP( sList[0], sList[sMid], sSave );
        }

        SMDMIS_COMPARE_KEY( sRowBase + sList[0],
                            sRowBase + sList[sElemCount - 1],
                            sRadix1,
                            sRadix2,
                            aHeader->mRadixArray[sRadixIndex],
                            sCompResult );

        if( sCompResult > 0 )
        {
            SMDMIS_SWAP( sList[0], sList[sElemCount - 1], sSave );
        }

        SMDMIS_COMPARE_KEY( sRowBase + sList[sElemCount - 1],
                            sRowBase + sList[sMid],
                            sRadix1,
                            sRadix2,
                            aHeader->mRadixArray[sRadixIndex],
                            sCompResult );

        if( sCompResult > 0 )
        {
            SMDMIS_SWAP( sList[sMid], sList[sElemCount - 1], sSave );
        }

        sPivotVal   = sList[sElemCount - 1];
        sCurLeft    = 0;
        sLeftSmall  = 0;
        sRightEqual = sElemCount - 1;

        /**
         * 3way Quick Sort를 수행한다.
         */
        /* Three way partition <,==,> */
        while( (sCurLeft < sElemCount - 1) && (sCurLeft < sRightEqual) )
        {
            SMDMIS_COMPARE_KEY( sRowBase + sList[sCurLeft],
                                sRowBase + sPivotVal,
                                sRadix1,
                                sRadix2,
                                aHeader->mRadixArray[sRadixIndex],
                                sCompResult );

            if( sCompResult < 0 )
            {
                if( sCurLeft != sLeftSmall )
                {
                    SMDMIS_SWAP( sList[sCurLeft], sList[sLeftSmall], sSave );
                }
                sCurLeft++;
                sLeftSmall++;
            }
            else if( sCompResult == 0 )
            {
                sRightEqual--;
                SMDMIS_SWAP( sList[sCurLeft], sList[sRightEqual], sSave );
            }
            else
            {
                sCurLeft++;
            }
        }

        sEqualLen = sElemCount - sRightEqual;
        if( sEqualLen == sElemCount )
        {
            sRadixIndex++;
            continue;
        }
        else
        {
            /**
             * pivot value와 동일한 값을 가장 오른쪽에 채워놓았다.
             * 여기서는 이를 pivot value보다 작은값과 큰값 사이로
             * 옮기는 작업을 수행한다.
             */
            sSmallLen = sLeftSmall;
            sBigLen = sRightEqual - sLeftSmall;
            sMinLen = STL_MIN( sBigLen, sEqualLen );
            sCurLeft = sLeftSmall + sEqualLen;  /* big 시작 위치 */

            for( i = 0; i < sMinLen; i++ )
            {
                SMDMIS_SWAP( sList[sLeftSmall + i], sList[sElemCount - 1 - i], sSave );
            }


            if( sSmallLen < sBigLen )
            {
                if( sBigLen < sEqualLen )
                {
                    /* equal part > big part > small part 순이다. equal, big을 스택에 차례대로 넣는다.*/
                    if( sEqualLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = &sList[sLeftSmall];
                        sStackElemCount[sStackPos]  = sEqualLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex + 1;
                    }
                    if( sBigLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = &sList[sCurLeft];
                        sStackElemCount[sStackPos]  = sBigLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex;
                    }
                    if( sSmallLen > 1 )
                    {
                        sElemCount = sLeftSmall;
                        continue;
                    }
                }
                else if( sSmallLen < sEqualLen )
                {
                    /* big part > equal part > small part 순이다. big, equal을 스택에 차례대로 넣는다.*/
                    if( sBigLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = &sList[sCurLeft];
                        sStackElemCount[sStackPos]  = sBigLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex;
                    }
                    if( sEqualLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = &sList[sLeftSmall];
                        sStackElemCount[sStackPos]  = sEqualLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex + 1;
                    }
                    if( sSmallLen > 1 )
                    {
                        sElemCount = sLeftSmall;
                        continue;
                    }
                }
                else
                {
                    /* big part > small part > equal part 순이다. big, small을 스택에 차례대로 넣는다.*/
                    if( sBigLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = &sList[sCurLeft];
                        sStackElemCount[sStackPos]  = sBigLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex;
                    }
                    if( sSmallLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = sList;
                        sStackElemCount[sStackPos]  = sSmallLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex;
                    }
                    if( sEqualLen > 1 )
                    {
                        sList = &sList[sLeftSmall];
                        sElemCount = sEqualLen;
                        sRadixIndex = sRadixIndex + 1;
                        continue;
                    }
                }
            }
            else
            {
                if( sSmallLen < sEqualLen )
                {
                    /* equal part > small part > big part 순이다. equal, small을 스택에 차례대로 넣는다.*/
                    if( sEqualLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = &sList[sLeftSmall];
                        sStackElemCount[sStackPos]  = sEqualLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex + 1;
                    }
                    if( sSmallLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = sList;
                        sStackElemCount[sStackPos]  = sSmallLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex;
                    }
                    if( sBigLen > 1 )
                    {
                        sList = &sList[sCurLeft];
                        sElemCount = sBigLen;
                        continue;
                    }
                }
                else if( sBigLen < sEqualLen )
                {
                    /* small part > equal part > big part 순이다. small, equal을 스택에 차례대로 넣는다.*/
                    if( sSmallLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = sList;
                        sStackElemCount[sStackPos]  = sSmallLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex;
                    }
                    if( sEqualLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = &sList[sLeftSmall];
                        sStackElemCount[sStackPos]  = sEqualLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex + 1;
                    }
                    if( sBigLen > 1 )
                    {
                        sList = &sList[sCurLeft];
                        sElemCount = sBigLen;
                        continue;
                    }
                }
                else
                {
                    /* small part > big part > equal part 순이다. small, big을 스택에 차례대로 넣는다.*/
                    if( sSmallLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = sList;
                        sStackElemCount[sStackPos]  = sSmallLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex;
                    }
                    if( sBigLen > 1 )
                    {
                        STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                        sStackPos++;
                        sStackList[sStackPos]       = &sList[sCurLeft];
                        sStackElemCount[sStackPos]  = sBigLen;
                        sStackRadixIndex[sStackPos] = sRadixIndex;
                    }
                    if( sEqualLen > 1 )
                    {
                        sList = &sList[sLeftSmall];
                        sElemCount = sEqualLen;
                        sRadixIndex = sRadixIndex + 1;
                        continue;
                    }
                }
            }
        }

        STL_RAMP( RAMP_NEXT_STACK );

        /**
         * stack에 저장된 list 및 element count를 가져온다.
         */
        if( sStackPos < 0 )
        {
            break;
        }
        else
        {
            sList       = sStackList[sStackPos];
            sElemCount  = sStackElemCount[sStackPos];
            sRadixIndex = sStackRadixIndex[sStackPos];
            sStackPos--;
        }
    }

    STL_RAMP( RAMP_FINISH );
}

static stlStatus smdmisRadixToIndex( smdmisTableHeader *aHeader,
                                     smdmisRadixNode   *aSourceNode,
                                     stlBool            aSetDup,
                                     smdmisIndexNode  **aTargetNode,
                                     smlEnv            *aEnv )
{
    smdmisRadixNode *sNode = aSourceNode;
    smdmisIndexNode *sTargetNode = *aTargetNode;
    smdmisRadixNode *sTempNode;
    smdmisIndexNode *sNewNode;
    smdmisKeyRow     sKeyRow;
    stlInt32         sKeyIndex = 0;
    stlInt32         sKeyIndexFence;
    stlBool          sFirstDup = STL_FALSE;

    STL_DASSERT( sTargetNode != NULL );

    while( sNode != NULL )
    {
        if( SMDMIS_NODE_IS_FULL(sTargetNode) == STL_TRUE )
        {
            STL_TRY( smdmisAllocIndexNode( aHeader,
                                           &sNewNode,
                                           aEnv ) == STL_SUCCESS );

            smdmisIndexNodeInit( sNewNode,
                                 0, /* this is leaf node */
                                 aHeader->mNodeSize,
                                 aHeader->mKeyRowLen );

            SMDMIS_INDEX_NODE_ADD_TO_LIST( sTargetNode, sNewNode );

            sTargetNode = sNewNode;
        }

        sKeyIndexFence = STL_MIN( sTargetNode->mMaxKeyCount - sTargetNode->mKeyCount,
                                  sNode->mKeyCount - sKeyIndex ) + sKeyIndex;

        if( aSetDup == STL_TRUE )
        {
            if( sFirstDup == STL_FALSE )
            {
                /* 첫번째 key는 dup을 세팅하지 않는다. */
                sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sKeyIndex );
                SMDMIS_NODE_APPEND_KEY( sTargetNode, sKeyRow, aHeader->mKeyRowLen );
                sKeyIndex++;
                sFirstDup = STL_TRUE;
            }
            for( ; sKeyIndex < sKeyIndexFence; sKeyIndex++ )
            {
                sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sKeyIndex );
                SMDMIS_KEY_ROW_SET_DUP( sKeyRow );
                SMDMIS_NODE_APPEND_KEY( sTargetNode, sKeyRow, aHeader->mKeyRowLen );
            }
        }
        else
        {
            for( ; sKeyIndex < sKeyIndexFence; sKeyIndex++ )
            {
                sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sKeyIndex );
                SMDMIS_NODE_APPEND_KEY( sTargetNode, sKeyRow, aHeader->mKeyRowLen );
            }
        }

        if( sKeyIndex == sNode->mKeyCount )
        {
            sTempNode = sNode->mNextOverflow;

            smdmisFreeRadixNode( aHeader,
                                 sNode );

            sNode = sTempNode;
            sKeyIndex = 0;
        }
    }

    *aTargetNode = sTargetNode;

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static stlStatus smdmisSortByLSDRadix( smdmisTableHeader *aHeader,
                                       smdmifTableHeader *aBaseHeader,
                                       smdmisIndexNode  **aSortedNodes,
                                       smlEnv            *aEnv )
{
    smdmisIndexBucket  sRadixTableEntry[2][SMDMIS_RADIX_RANGE + 1];
    stlChar           *sColValue;
    stlInt64           sColLen;
    void              *sScanFixedPart;
    knlLogicalAddr     sScanRowAddr;
    stlInt32           sScanRowIndex;
    smdmifFixedRow     sScanRow;
    smdmisIndexNode   *sNode;
    smdmisIndexNode   *sNewNode;
    smdmisIndexNode   *sTargetNode;
    smdmisKeyRow       sKeyRow;
    smdmifDefColumn   *sKeyCol;
    smdmifDefColumn   *sRowCol;
    stlInt32           i, j;
    stlUInt8          *sRadixPos;
    stlInt32           sRadixIndex;
    stlUInt8           sRadixValue;
    stlInt16           sKeyIndex;
    stlInt32           sSourceBucket = 0;
    stlInt32           sTargetBucket = 1;
    stlInt32           sTemp;
    stlInt32           sState = 0;
    smdmisKeyRow       sTempKeyAlloced;
    smdmisKeyRow       sTempKey;
    knlRegionMark      sOpMemMark;
    stlChar           *sKeyColPos;
    smdmisKeyRow       sLeftKeyRow;
    smdmisKeyRow       sRightKeyRow;

    KNL_INIT_REGION_MARK( &sOpMemMark );
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sOpMemMark,
                               KNL_ENV( aEnv ) ) == STL_SUCCESS );

    sState = 1;

    STL_TRY( knlAllocRegionMem( &aEnv->mOperationHeapMem,
                                aHeader->mKeyRowLen + STL_SIZEOF(knlLogicalAddr),
                                (void**)&sTempKeyAlloced,
                                KNL_ENV( aEnv ) ) == STL_SUCCESS );

    /* sTempKey는 SMDMIS_KEY_ROW_HEADER_SIZE를 더한 위치가 align이 맞아야 한다.*/
    sTempKey = sTempKeyAlloced + STL_SIZEOF(knlLogicalAddr) - SMDMIS_KEY_ROW_HEADER_SIZE;

    STL_TRY( smdmisMakeRadixEnv( aHeader,
                                 aEnv ) == STL_SUCCESS );

    sRadixIndex = aHeader->mRadixIndexFence - 1;
    sRadixPos = (stlUInt8 *)(sTempKey + aHeader->mRadixArray[sRadixIndex]);

    *aSortedNodes = NULL;

    for( i = 0; i <= SMDMIS_RADIX_RANGE; i++ )
    {
        sRadixTableEntry[0][i].mFirst = NULL;
        sRadixTableEntry[0][i].mLast = NULL;
        sRadixTableEntry[1][i].mFirst = NULL;
        sRadixTableEntry[1][i].mLast = NULL;
    }

    sScanFixedPart = SMDMIF_TABLE_GET_FIRST_FIXED_PART(aBaseHeader);
    sScanRowIndex = 0;
    sNode = NULL;

    while( STL_TRUE )
    {
        /**
         * base table에 다음 row를 구한다.
         */
        sScanRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sScanFixedPart, sScanRowIndex );
        while( sScanRowAddr == KNL_LOGICAL_ADDR_NULL )
        {
            if( sScanFixedPart == NULL )
            {
                break;
            }
            sScanFixedPart = SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART( sScanFixedPart );
            sScanRowIndex = 0;
            sScanRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sScanFixedPart, sScanRowIndex );
        }
        if( sScanRowAddr == KNL_LOGICAL_ADDR_NULL )
        {
            /* 더이상 row가 없다. */
            break;
        }
        sScanRow = KNL_TO_PHYSICAL_ADDR( sScanRowAddr );
        sScanRowIndex++;

        /* column value에 0-pad를 위해 row 자체를 0으로 초기화한다. */
        SMDMIS_KEY_ROW_INIT( sTempKey, aHeader->mKeyRowLen );

        /*
         * key row를 write한다.
         */
        SMDMIS_KEY_ROW_WRITE_CHILD_ADDR( sTempKey, sScanRowAddr );

        for( i = 0; i < aHeader->mKeyColCount; i++ )
        {
            sKeyCol = &aHeader->mKeyCols[i];
            sRowCol = &aBaseHeader->mCols[aHeader->mKeyColInfo[i].mColOrder];
            sKeyColPos = sTempKey + SMDMIS_KEY_COL_OFFSET_VALUE( sKeyCol );

            STL_DASSERT( sKeyCol->mIsFixedPart == STL_TRUE );

            smdmifTableGetColValueAndLen( sScanRow,
                                          sRowCol,
                                          /* var part이고 쪼개져 저장되어 있으면 바로 여기에 카피가 된다. */
                                          sKeyColPos,
                                          &sColValue,
                                          &sColLen );

            if( sColValue != sKeyColPos )
            {
                /* sColValue가 sKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE( sKeyCol )와 같다면 이미 카피가 되었다. */
                SMDMIS_KEY_ROW_WRITE_COL( sTempKey,
                                          sKeyCol,
                                          sColValue,
                                          sColLen );
            }

            if( aHeader->mKeyColInfo[i].mModifyKeyVal != NULL )
            {
                aHeader->mKeyColInfo[i].mModifyKeyVal( sTempKey,
                                                       sColLen,
                                                       sKeyCol );
            }
        }

        sRadixValue = *sRadixPos;

        sNode = sRadixTableEntry[sSourceBucket][sRadixValue].mLast;

        /**
         * 키를 입력할 node 및 위치 확보
         */
        if( (sNode == NULL) || (SMDMIS_NODE_IS_FULL(sNode) == STL_TRUE) )
        {
            STL_TRY( smdmisAllocIndexNode( aHeader,
                                           &sNewNode,
                                           aEnv ) == STL_SUCCESS );

            smdmisIndexNodeInit( sNewNode,
                                 0, /* this is leaf node */
                                 aHeader->mNodeSize,
                                 aHeader->mKeyRowLen );

            if( sNode == NULL )
            {
                sRadixTableEntry[sSourceBucket][sRadixValue].mFirst = sNewNode;
            }
            else
            {
                SMDMIS_INDEX_NODE_ADD_TO_LIST( sNode, sNewNode );
            }

            sNode = sNewNode;
            sRadixTableEntry[sSourceBucket][sRadixValue].mLast = sNode;
        }

        SMDMIS_NODE_ALLOC_KEY( sNode, sKeyRow, aHeader->mKeyRowLen );

        stlMemcpy( sKeyRow, sTempKey, aHeader->mKeyRowLen );
    }

    for( sRadixIndex--; sRadixIndex >= 0; sRadixIndex-- )
    {
        for( j = 0; j <= SMDMIS_RADIX_RANGE; j++ )
        {
            sNode = sRadixTableEntry[sSourceBucket][j].mFirst;
            sKeyIndex = 0;
            while( sNode != NULL )
            {
                if( sKeyIndex >= sNode->mKeyCount )
                {
                    sNewNode = SMDMIS_INDEX_NODE_GET_NEXT( sNode );
                    smdmisFreeIndexNode( aHeader,
                                         sNode );
                    sNode = sNewNode;
                    sKeyIndex = 0;
                    continue;
                }
                sKeyRow = SMDMIS_NODE_GET_KEY( sNode, sKeyIndex );
                sKeyIndex++;

                sRadixValue = *((stlUInt8 *)(sKeyRow + aHeader->mRadixArray[sRadixIndex]));

                sTargetNode = sRadixTableEntry[sTargetBucket][sRadixValue].mLast;
                if( (sTargetNode == NULL) || (SMDMIS_NODE_IS_FULL(sTargetNode) == STL_TRUE) )
                {
                    STL_TRY( smdmisAllocIndexNode( aHeader,
                                                   &sNewNode,
                                                   aEnv ) == STL_SUCCESS );
                    smdmisIndexNodeInit( sNewNode,
                                         0, /* this is leaf node */
                                         aHeader->mNodeSize,
                                         aHeader->mKeyRowLen );
                    if( sTargetNode == NULL )
                    {
                        sRadixTableEntry[sTargetBucket][sRadixValue].mFirst = sNewNode;
                    }
                    else
                    {
                        SMDMIS_INDEX_NODE_ADD_TO_LIST( sTargetNode, sNewNode );
                    }
                    sTargetNode = sNewNode;
                    sRadixTableEntry[sTargetBucket][sRadixValue].mLast = sTargetNode;
                }
                SMDMIS_NODE_APPEND_KEY( sTargetNode, sKeyRow, aHeader->mKeyRowLen );
            }

            sRadixTableEntry[sSourceBucket][j].mFirst = NULL;
            sRadixTableEntry[sSourceBucket][j].mLast = NULL;
        }

        sTemp = sSourceBucket;
        sSourceBucket = sTargetBucket;
        sTargetBucket = sTemp;
    }

    sNewNode = NULL;
    for( i = 0; i <= SMDMIS_RADIX_RANGE ; i++ )
    {
        sNode = sRadixTableEntry[sSourceBucket][i].mFirst;

        if( sNode == NULL )
        {
            continue;
        }

        if( sNewNode == NULL )
        {
            *aSortedNodes = sNode;
        }
        else
        {
            sNewNode->mNext = sNode->mMyAddr;
        }

        sNewNode = sRadixTableEntry[sSourceBucket][i].mLast;
    }

    /* LSD sort시에는 dup 설정을 하지 않으므로, sort가 완료된 후
       키 검사를 통해 dup 설정을 한다. */
    sNode = *aSortedNodes;
    sLeftKeyRow = NULL;
    while( sNode != NULL )
    {
        STL_DASSERT( sNode->mKeyCount > 0 );

        if( sLeftKeyRow != NULL )
        {
            sRightKeyRow = SMDMIS_NODE_GET_KEY( sNode, 0 );

            if( aHeader->mCompareKey( aHeader,
                                      sLeftKeyRow,
                                      sRightKeyRow ) == DTL_COMPARISON_EQUAL )
            {
                SMDMIS_KEY_ROW_SET_DUP( sRightKeyRow );
            }
        }

        for( i = 0; i < sNode->mKeyCount - 1; i++ )
        {
            sLeftKeyRow = SMDMIS_NODE_GET_KEY( sNode, i );
            sRightKeyRow = SMDMIS_NODE_GET_KEY( sNode, i + 1 );

            if( aHeader->mCompareKey( aHeader,
                                      sLeftKeyRow,
                                      sRightKeyRow ) == DTL_COMPARISON_EQUAL )
            {
                SMDMIS_KEY_ROW_SET_DUP( sRightKeyRow );
            }
        }

        sLeftKeyRow = SMDMIS_NODE_GET_KEY( sNode, sNode->mKeyCount - 1 );
        sNode = SMDMIS_INDEX_NODE_GET_NEXT( sNode );
    }

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sOpMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sOpMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            KNL_ENV( aEnv ) );
            break;
    }

    return STL_FAILURE;
}

static stlStatus smdmisSortByMerge( smlStatement      *aStatement,
                                    smdmisTableHeader *aHeader,
                                    smdmifTableHeader *aBaseHeader,
                                    smdmisIndexNode  **aSortedNodes,
                                    smlEnv            *aEnv )
{
    knlRegionMark      sOpMemMark;
    stlInt32           sState = 0;
    smdmisMergeEnv     sMergeEnv;
    stlInt32           sLeafCount;
    smdmisIndexNode   *sLeafList;
    smdmisIndexNode   *sNode;

    /**
     * merge를 위해 merge environment를 생성, 초기화한다.
     */
    KNL_INIT_REGION_MARK( &sOpMemMark );
    STL_TRY( knlMarkRegionMem( &aEnv->mOperationHeapMem,
                               &sOpMemMark,
                               KNL_ENV( aEnv ) ) == STL_SUCCESS );

    sState = 1;

    sLeafCount = STL_MIN( smdmisCalculateNodeCount( aHeader, aBaseHeader ),
                          knlGetPropertyBigIntValueByID( KNL_PROPERTY_INST_TABLE_SORT_RUN_COUNT,
                                                         KNL_ENV( aEnv ) ) );

    STL_TRY( smdmisInitMergeEnv( aStatement,
                                 &sMergeEnv,
                                 sLeafCount,
                                 STL_FALSE, /* aVolatileMode. stable table이므로 false를 준다. */
                                 aEnv ) == STL_SUCCESS );

    /**
     * base table의 row들로부터 key를 추출하여 leaf node들을 생성한다.
     */
    STL_TRY( smdmisBuildLeafNodes( aHeader,
                                   aBaseHeader,
                                   &sLeafList,
                                   aEnv ) == STL_SUCCESS );

    while( sLeafList != NULL )
    {
        SMDMIS_INDEX_NODE_REMOVE_FIRST( sLeafList, sNode );

        if( SMDMIS_MERGE_ENV_IS_FULL( &sMergeEnv ) == STL_TRUE )
        {
            STL_TRY( smdmisMerge( aHeader,
                                  &sMergeEnv,
                                  &sMergeEnv.mSortedList,
                                  aEnv ) == STL_SUCCESS );
        }

        /**
         * add aNode to current merge run
         */
        smdmisAddNodeToMergeEnv( aHeader,
                                 &sMergeEnv,
                                 sNode );
    }

    STL_TRY( smdmisMergeAll( aHeader,
                             &sMergeEnv,
                             aEnv ) == STL_SUCCESS );

    STL_DASSERT( (sMergeEnv.mSortedList != NULL) &&
                 (sMergeEnv.mSortedList->mItemCount == 1) );

    *aSortedNodes = sMergeEnv.mSortedList->mItems[0];

    sState = 0;
    STL_TRY( knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                       &sOpMemMark,
                                       STL_FALSE, /* aFreeChunk */
                                       KNL_ENV( aEnv ) ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    switch( sState )
    {
        case 1:
            (void)knlFreeUnmarkedRegionMem( &aEnv->mOperationHeapMem,
                                            &sOpMemMark,
                                            STL_FALSE, /* aFreeChunk */
                                            KNL_ENV( aEnv ) );
            break;
    }

    return STL_FAILURE;
}

static stlStatus smdmisBuildLeafNodes( smdmisTableHeader *aHeader,
                                       smdmifTableHeader *aBaseHeader,
                                       smdmisIndexNode  **aLeafList,
                                       smlEnv            *aEnv )
{
    stlChar           *sColValue;
    stlInt64           sColLen;
    void              *sScanFixedPart;
    stlInt32           sScanRowIndex;
    smdmifFixedRow     sScanRow;
    knlLogicalAddr     sScanRowAddr;
    smdmisIndexNode   *sNode;
    smdmisIndexNode   *sNewNode;
    smdmisKeyRow       sKeyRow;
    smdmifDefColumn   *sKeyCol;
    smdmifDefColumn   *sRowCol;
    stlInt32           i;
    stlChar           *sKeyColPos;

    *aLeafList = NULL;

    sScanFixedPart = SMDMIF_TABLE_GET_FIRST_FIXED_PART(aBaseHeader);
    sScanRowIndex = 0;
    sNode = NULL;

    while( STL_TRUE )
    {
        /**
         * base table에 다음 row를 구한다.
         */
        sScanRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sScanFixedPart, sScanRowIndex );
        while( sScanRowAddr == KNL_LOGICAL_ADDR_NULL )
        {
            if( sScanFixedPart == NULL )
            {
                break;
            }
            sScanFixedPart = SMDMIF_FIXED_PART_GET_NEXT_FIXED_PART( sScanFixedPart );
            sScanRowIndex = 0;
            sScanRowAddr = SMDMIF_FIXED_PART_GET_ROW_ADDR( sScanFixedPart, sScanRowIndex );
        }

        if( sScanRowAddr == KNL_LOGICAL_ADDR_NULL )
        {
            /* 더이상 row가 없다. */
            break;
        }

        sScanRow = KNL_TO_PHYSICAL_ADDR( sScanRowAddr );
        sScanRowIndex++;

        /**
         * 키를 입력할 node 및 위치 확보
         */
        if( (sNode == NULL) || (SMDMIS_NODE_IS_FULL(sNode) == STL_TRUE) )
        {
            if( sNode != NULL )
            {
                /* sNode가 가득차면 sort를 수행한다. */
                smdmisSort( aHeader,
                            (stlChar *)sNode,
                            sNode->mSortItems,
                            sNode->mKeyCount );

                STL_DASSERT( smdmisVerifyOrderOfNode( aHeader,
                                                      sNode ) == STL_SUCCESS );
            }

            STL_TRY( smdmisAllocIndexNode( aHeader,
                                           &sNewNode,
                                           aEnv ) == STL_SUCCESS );

            smdmisIndexNodeInit( sNewNode,
                                 0, /* this is leaf node */
                                 aHeader->mNodeSize,
                                 aHeader->mKeyRowLen );

            if( sNode == NULL )
            {
                *aLeafList = sNewNode;
            }
            else
            {
                SMDMIS_INDEX_NODE_ADD_TO_LIST( sNode, sNewNode );
            }

            sNode = sNewNode;
        }

        SMDMIS_NODE_ALLOC_KEY( sNode, sKeyRow, aHeader->mKeyRowLen );

        SMDMIS_KEY_ROW_INIT_HEADER( sKeyRow );

        /*
         * key row를 write한다.
         */
        SMDMIS_KEY_ROW_WRITE_CHILD_ADDR( sKeyRow, sScanRowAddr );

        for( i = 0; i < aHeader->mKeyColCount; i++ )
        {
            sKeyCol = &aHeader->mKeyCols[i];
            sRowCol = &aBaseHeader->mCols[aHeader->mKeyColInfo[i].mColOrder];
            sKeyColPos = sKeyRow + SMDMIS_KEY_COL_OFFSET_VALUE( sKeyCol );

            if( sKeyCol->mIsFixedPart == STL_TRUE )
            {
                smdmifTableGetColValueAndLen( sScanRow,
                                              sRowCol,
                                              sKeyColPos,
                                              &sColValue,
                                              &sColLen );

                if( sColValue != sKeyColPos )
                {
                    SMDMIS_KEY_ROW_WRITE_COL( sKeyRow,
                                              sKeyCol,
                                              sColValue,
                                              (stlUInt16)sColLen );
                }
            }
            else
            {
                /* var column인 경우 fixed row로부터 var col ptr을 복사한다. */
                SMDMIS_KEY_ROW_SET_VAR_COL( sKeyRow,
                                            sKeyCol,
                                            SMDMIF_FIXED_ROW_GET_VAR_COL_ADDR( sScanRow, sRowCol ) );
            }
        }
    }

    if( sNode != NULL )
    {
        /* 마지막 node를 sort한다. */
        smdmisSort( aHeader,
                    (stlChar *)sNode,
                    sNode->mSortItems,
                    sNode->mKeyCount );

        STL_DASSERT( smdmisVerifyOrderOfNode( aHeader,
                                              sNode ) == STL_SUCCESS );
    }

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

static void smdmisSort( smdmisTableHeader *aHeader,
                        stlChar           *aRowBase,
                        smdmisSortItem    *aSortItems,
                        stlInt32           aRowCount )
{
    smdmisSortItem  *sStackList[SMDMIS_QSORT_STACK_SIZE];
    stlInt32         sStackElemCount[SMDMIS_QSORT_STACK_SIZE];
    stlInt32         sStackPos = -1;

    smdmisSortItem  *sList;
    stlInt32         sElemCount;

    stlInt32         i;
    stlInt32         j;
    smdmisSortItem   sSave;

    stlInt32         sMid;
    stlInt32         sCurLeft;
    smdmisSortItem   sPivotVal;
    stlInt32         sRet;
    stlInt32         sRightEqual;
    stlInt32         sLeftSmall;
    stlUInt32        sBigLen;
    stlUInt32        sEqualLen;
    stlUInt32        sMinLen;
    stlChar         *sCompRow1;
    stlChar         *sCompRow2;
    dtlCompareResult sCompResult;

    /* Terminate on small subfiles */
    sList = aSortItems;
    sElemCount = aRowCount;
    while( STL_TRUE )
    {
        if( sElemCount < SMDMIS_QSORT_CUTOFF )
        {
            /**
             * element 개수가 SMDMIS_QSORT_CUTOFF보다 작을 경우
             * quick sort보다 insertion sort가 더 효율적이므로
             * insertion sort로 처리하도록 한다.
             */
            for( i = 1; i <= sElemCount - 1; i++ )
            {
                sCompRow1 = aRowBase + sList[i - 1];
                sCompRow2 = aRowBase + sList[i];
                sCompResult = aHeader->mCompareKey( aHeader,
                                                    sCompRow1,
                                                    sCompRow2 );

                if( sCompResult == DTL_COMPARISON_LESS )
                {
                    continue;
                }
                else if( sCompResult == DTL_COMPARISON_EQUAL )
                {
                    SMDMIS_KEY_ROW_SET_DUP( sCompRow2 );
                    continue;
                }

                sSave = sList[i];
                sList[i] = sList[i - 1];
                sCompRow2 = aRowBase + sSave;

                for( j = i - 1; j > 0; j-- )
                {
                    sCompRow1 = aRowBase + sList[j - 1];
                    sCompResult = aHeader->mCompareKey( aHeader,
                                                        sCompRow1,
                                                        sCompRow2 );

                    if( sCompResult == DTL_COMPARISON_LESS )
                    {
                        break;
                    }
                    else if( sCompResult == DTL_COMPARISON_EQUAL )
                    {
                        SMDMIS_KEY_ROW_SET_DUP( sCompRow2 );
                        break;
                    }

                    sList[j] = sList[j - 1];
                }

                sList[j] = sSave;
            }
        }
        else
        {
            /**
             * ascending으로 정렬된 데이터가 들어오는 경우
             * first value를 pivot value로 설정하게 되면
             * 데이터 개수만큼 loop를 돌아야 하는 경우가 발생한다.
             * 이는 최악의 성능을 가져오게 되는데, 이를 방지하기 위하여
             * first value, mid value, last value들에 대하여
             * 정렬상 중간값을 pivot value로 한다.
             */
            /* Find the median of three values in list, use it as the pivot */
            sMid = sElemCount / 2;
            if( aHeader->mCompareKey( aHeader,
                                      aRowBase + sList[0],
                                      aRowBase + sList[sMid] ) > 0 )
            {
                SMDMIS_SWAP( sList[0], sList[sMid], sSave );
            }

            if( aHeader->mCompareKey( aHeader,
                                      aRowBase + sList[0],
                                      aRowBase + sList[sElemCount - 1] ) > 0 )
            {
                SMDMIS_SWAP( sList[0], sList[sElemCount - 1], sSave );
            }

            if( aHeader->mCompareKey( aHeader,
                                      aRowBase + sList[sElemCount - 1],
                                      aRowBase + sList[sMid] ) > 0 )
            {
                SMDMIS_SWAP( sList[sMid], sList[sElemCount - 1], sSave );
            }

            sPivotVal   = sList[sElemCount - 1];
            sCurLeft    = 0;
            sLeftSmall  = 0;
            sRightEqual = sElemCount - 1;

            /**
             * 3way Quick Sort를 수행한다.
             */
            /* Three way partition <,==,> */
            while( (sCurLeft < sElemCount - 1) && (sCurLeft < sRightEqual) )
            {
                sRet = aHeader->mCompareKey( aHeader,
                                             aRowBase + sList[sCurLeft],
                                             aRowBase + sPivotVal );
                if( sRet < 0 )
                {
                    if( sCurLeft != sLeftSmall )
                    {
                        SMDMIS_SWAP( sList[sCurLeft], sList[sLeftSmall], sSave );
                    }
                    sCurLeft++;
                    sLeftSmall++;
                }
                else if( sRet == 0 )
                {
                    sRightEqual--;
                    SMDMIS_SWAP( sList[sCurLeft], sList[sRightEqual], sSave );
                    /* 같은 값을 만나면 pivot이 아닌 쪽에 setDup을 호출한다.
                       pivot이 같은 값중 맨 앞에 오기 때문이다. */
                    SMDMIS_KEY_ROW_SET_DUP( aRowBase + sList[sRightEqual] );
                }
                else
                {
                    sCurLeft++;
                }
            }

            sEqualLen = sElemCount - sRightEqual;
            if( sEqualLen == sElemCount )
            {
                /* 모든 element가 같은 값임 */
                /* dup 세팅 안된 마지막 element가 맨 앞에 와야 함: bugKeyDuplicity.ts.sql에 의해 테스트 */
                SMDMIS_SWAP( sList[0], sList[sElemCount - 1], sSave );
            }
            else
            {
                /**
                 * pivot value와 동일한 값을 가장 오른쪽에 채워놓았다.
                 * 여기서는 이를 pivot value보다 작은값과 큰값 사이로
                 * 옮기는 작업을 수행한다.
                 */
                sBigLen = sRightEqual - sLeftSmall;
                sMinLen = STL_MIN( sBigLen, sEqualLen );

                /* sMinLen이 0일 경우 pivot보다 큰 값이 없다는 뜻인데,
                   이럴경우에도 pivot을 맨 앞으로 옮기기 위해 (dup 세팅 안된 것이 맨 앞에 와야 함)
                   sMinLen을 1로 바꾼다. bugKeyDuplicity.ts.sql에 예제가 있음 */
                sMinLen = STL_MAX( sMinLen, 1 );
                for( i = 0; i < sMinLen; i++ )
                {
                    /* pivot이 같은 값들 중 맨 앞으로 가게 되어 있다.*/
                    SMDMIS_SWAP( sList[sLeftSmall + i], sList[sElemCount - 1 - i], sSave );
                }

                sCurLeft = sLeftSmall + sEqualLen;

                /**
                 * pivot value를 기준으로
                 * element count가 많은 쪽을 stack에 저장하고,
                 * element count가 적은 쪽에 대해 quick sort를 계속 수행한다.
                 * 반대로 할 경우 stack의 사용량이 더 많아지므로 주의한다.
                 */
                STL_DASSERT( sStackPos + 1 < SMDMIS_QSORT_STACK_SIZE );
                if( sLeftSmall < (sElemCount - sCurLeft) )
                {
                    /* pivot을 기준으로 left의 elem count가 적은 경우 */
                    sStackPos++;
                    sStackList[sStackPos]       = &sList[sCurLeft];
                    sStackElemCount[sStackPos]  = sElemCount - sCurLeft;

                    sElemCount = sLeftSmall;
                    continue;
                }
                else
                {
                    /* pivot을 기준으로 right의 elem count가 적은 경우 */
                    sStackPos++;
                    sStackList[sStackPos]       = sList;
                    sStackElemCount[sStackPos]  = sLeftSmall;

                    sList = &sList[sCurLeft];
                    sElemCount = sElemCount - sCurLeft;
                    continue;
                }
            }
        }

        /**
         * stack에 저장된 list 및 element count를 가져온다.
         */
        if( sStackPos < 0 )
        {
            break;
        }
        else
        {
            sList       = sStackList[sStackPos];
            sElemCount  = sStackElemCount[sStackPos];
            sStackPos--;
        }
    }
}

static dtlCompareResult smdmisCompareOneInteger( smdmisTableHeader *aHeader,
                                                 stlChar           *aLeftRow,
                                                 stlChar           *aRightRow )
{
    dtlCompareResult    sRet = DTL_COMPARISON_EQUAL;
    stlChar            *sLeftColValue;
    stlChar            *sRightColValue;
    smdmisKeyColLength  sLeftColLen;
    smdmisKeyColLength  sRightColLen;
    smdmisKeyCol        sLeftKeyCol;
    smdmisKeyCol        sRightKeyCol;
    stlInt32            sLeftVal;
    stlInt32            sRightVal;

    sLeftKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aLeftRow, aHeader->mFirstKeyOffset );
    sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sLeftKeyCol );

    sRightKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aRightRow, aHeader->mFirstKeyOffset );
    sRightColLen   = SMDMIS_KEY_COL_GET_LEN( sRightKeyCol );

    if( sLeftColLen == 0 )
    {
        if( sRightColLen == 0 )
        {
            sRet = DTL_COMPARISON_EQUAL;
        }
        else if( aHeader->mKeyColInfo[0].mNullOrderLast == STL_TRUE )
        {
            sRet = DTL_COMPARISON_GREATER;
        }
        else
        {
            sRet = DTL_COMPARISON_LESS;
        }
    }
    else if( sRightColLen == 0 )
    {
        if( aHeader->mKeyColInfo[0].mNullOrderLast == STL_TRUE )
        {
            sRet = DTL_COMPARISON_LESS;
        }
        else
        {
            sRet = DTL_COMPARISON_GREATER;
        }
    }
    else
    {
        sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sLeftKeyCol );
        sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sRightKeyCol );

        DTL_GET_NATIVE_INTEGER_FROM_PTR( sLeftColValue, sLeftVal );
        DTL_GET_NATIVE_INTEGER_FROM_PTR( sRightColValue, sRightVal );

        sRet = ( sLeftVal > sRightVal ) - ( sLeftVal < sRightVal );
    }

    return sRet;
}

static dtlCompareResult smdmisCompareOneIntegerNotNull( smdmisTableHeader *aHeader,
                                                        stlChar           *aLeftRow,
                                                        stlChar           *aRightRow )
{
    stlChar            *sLeftColValue;
    stlChar            *sRightColValue;
    smdmisKeyCol        sLeftKeyCol;
    smdmisKeyCol        sRightKeyCol;
    stlInt32            sLeftVal;
    stlInt32            sRightVal;

    sLeftKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aLeftRow, aHeader->mFirstKeyOffset );
    sRightKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aRightRow, aHeader->mFirstKeyOffset );

    sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sLeftKeyCol );
    sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sRightKeyCol );

    DTL_GET_NATIVE_INTEGER_FROM_PTR( sLeftColValue, sLeftVal );
    DTL_GET_NATIVE_INTEGER_FROM_PTR( sRightColValue, sRightVal );

    return ( sLeftVal > sRightVal ) - ( sLeftVal < sRightVal );
}

static dtlCompareResult smdmisCompareOneNumeric( smdmisTableHeader *aHeader,
                                                 stlChar           *aLeftRow,
                                                 stlChar           *aRightRow )
{
    dtlCompareResult    sRet = DTL_COMPARISON_EQUAL;
    stlChar            *sLeftColValue;
    stlChar            *sRightColValue;
    smdmisKeyColLength  sLeftColLen;
    smdmisKeyColLength  sRightColLen;
    smdmisKeyCol        sLeftKeyCol;
    smdmisKeyCol        sRightKeyCol;

    sLeftKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aLeftRow, aHeader->mFirstKeyOffset );
    sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sLeftKeyCol );

    sRightKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aRightRow, aHeader->mFirstKeyOffset );
    sRightColLen   = SMDMIS_KEY_COL_GET_LEN( sRightKeyCol );

    if( sLeftColLen == 0 )
    {
        if( sRightColLen == 0 )
        {
            sRet = DTL_COMPARISON_EQUAL;
        }
        else if( aHeader->mKeyColInfo[0].mNullOrderLast == STL_TRUE )
        {
            sRet = DTL_COMPARISON_GREATER;
        }
        else
        {
            sRet = DTL_COMPARISON_LESS;
        }
    }
    else if( sRightColLen == 0 )
    {
        if( aHeader->mKeyColInfo[0].mNullOrderLast == STL_TRUE )
        {
            sRet = DTL_COMPARISON_LESS;
        }
        else
        {
            sRet = DTL_COMPARISON_GREATER;
        }
    }
    else
    {
        sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sLeftKeyCol );
        sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sRightKeyCol );

        DTL_COMPARE_NUMERIC( sRet,
                             sRet, /* unused param */
                             sLeftColValue,
                             sLeftColLen,
                             sRightColValue,
                             sRightColLen );
    }

    return sRet;
}

static dtlCompareResult smdmisCompareOneNumericNotNull( smdmisTableHeader *aHeader,
                                                        stlChar           *aLeftRow,
                                                        stlChar           *aRightRow )
{
    dtlCompareResult    sRet = DTL_COMPARISON_EQUAL;
    stlChar            *sLeftColValue;
    stlChar            *sRightColValue;
    smdmisKeyColLength  sLeftColLen;
    smdmisKeyColLength  sRightColLen;
    smdmisKeyCol        sLeftKeyCol;
    smdmisKeyCol        sRightKeyCol;

    sLeftKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aLeftRow, aHeader->mFirstKeyOffset );
    sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sLeftKeyCol );

    sRightKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aRightRow, aHeader->mFirstKeyOffset );
    sRightColLen   = SMDMIS_KEY_COL_GET_LEN( sRightKeyCol );

    sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sLeftKeyCol );
    sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sRightKeyCol );

    DTL_COMPARE_NUMERIC( sRet,
                         sRet, /* unused param */
                         sLeftColValue,
                         sLeftColLen,
                         sRightColValue,
                         sRightColLen );

    return sRet;
}

static dtlCompareResult smdmisCompareOneVarchar( smdmisTableHeader *aHeader,
                                                 stlChar           *aLeftRow,
                                                 stlChar           *aRightRow )
{
    dtlCompareResult    sRet = DTL_COMPARISON_EQUAL;
    stlChar            *sLeftColValue;
    stlChar            *sRightColValue;
    smdmisKeyColLength  sLeftColLen;
    smdmisKeyColLength  sRightColLen;
    smdmisKeyCol        sLeftKeyCol;
    smdmisKeyCol        sRightKeyCol;

    sLeftKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aLeftRow, aHeader->mFirstKeyOffset );
    sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sLeftKeyCol );

    sRightKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aRightRow, aHeader->mFirstKeyOffset );
    sRightColLen   = SMDMIS_KEY_COL_GET_LEN( sRightKeyCol );

    if( sLeftColLen == 0 )
    {
        if( sRightColLen == 0 )
        {
            sRet = DTL_COMPARISON_EQUAL;
        }
        else if( aHeader->mKeyColInfo[0].mNullOrderLast == STL_TRUE )
        {
            sRet = DTL_COMPARISON_GREATER;
        }
        else
        {
            sRet = DTL_COMPARISON_LESS;
        }
    }
    else if( sRightColLen == 0 )
    {
        if( aHeader->mKeyColInfo[0].mNullOrderLast == STL_TRUE )
        {
            sRet = DTL_COMPARISON_LESS;
        }
        else
        {
            sRet = DTL_COMPARISON_GREATER;
        }
    }
    else
    {
        sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sLeftKeyCol );
        sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sRightKeyCol );

        DTL_COMPARE_VAR_CHAR( sRet,
                              sRet, /* unused param */
                              sLeftColValue,
                              sLeftColLen,
                              sRightColValue,
                              sRightColLen );
    }

    return sRet;
}

static dtlCompareResult smdmisCompareOneVarcharNotNull( smdmisTableHeader *aHeader,
                                                        stlChar           *aLeftRow,
                                                        stlChar           *aRightRow )
{
    dtlCompareResult    sRet = DTL_COMPARISON_EQUAL;
    stlChar            *sLeftColValue;
    stlChar            *sRightColValue;
    smdmisKeyColLength  sLeftColLen;
    smdmisKeyColLength  sRightColLen;
    smdmisKeyCol        sLeftKeyCol;
    smdmisKeyCol        sRightKeyCol;

    sLeftKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aLeftRow, aHeader->mFirstKeyOffset );
    sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sLeftKeyCol );

    sRightKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aRightRow, aHeader->mFirstKeyOffset );
    sRightColLen   = SMDMIS_KEY_COL_GET_LEN( sRightKeyCol );

    sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sLeftKeyCol );
    sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sRightKeyCol );

    DTL_COMPARE_VAR_CHAR( sRet,
                          sRet, /* unused param */
                          sLeftColValue,
                          sLeftColLen,
                          sRightColValue,
                          sRightColLen );

    return sRet;
}

static dtlCompareResult smdmisCompareOneCol( smdmisTableHeader *aHeader,
                                             stlChar           *aLeftRow,
                                             stlChar           *aRightRow )
{
    dtlCompareResult    sRet = DTL_COMPARISON_EQUAL;
    stlChar            *sLeftColValue;
    stlChar            *sRightColValue;
    smdmisKeyColLength  sLeftColLen;
    smdmisKeyColLength  sRightColLen;
    smdmisKeyCol        sLeftKeyCol;
    smdmisKeyCol        sRightKeyCol;
    dtlCompArg          sCompArg;

    sLeftKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aLeftRow, aHeader->mFirstKeyOffset );
    sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sLeftKeyCol );

    sRightKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aRightRow, aHeader->mFirstKeyOffset );
    sRightColLen   = SMDMIS_KEY_COL_GET_LEN( sRightKeyCol );

    if( sLeftColLen == 0 )
    {
        if( sRightColLen == 0 )
        {
            sRet = DTL_COMPARISON_EQUAL;
        }
        else if( aHeader->mKeyColInfo[0].mNullOrderLast == STL_TRUE )
        {
            sRet = DTL_COMPARISON_GREATER;
        }
        else
        {
            sRet = DTL_COMPARISON_LESS;
        }
    }
    else if( sRightColLen == 0 )
    {
        if( aHeader->mKeyColInfo[0].mNullOrderLast == STL_TRUE )
        {
            sRet = DTL_COMPARISON_LESS;
        }
        else
        {
            sRet = DTL_COMPARISON_GREATER;
        }
    }
    else
    {
        sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sLeftKeyCol );
        sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sRightKeyCol );

        DTL_TYPED_COMPARE( aHeader->mKeyColInfo[0].mPhysicalCompare,
                           sRet,
                           sCompArg,
                           sLeftColValue,
                           sLeftColLen,
                           sRightColValue,
                           sRightColLen );
    }

    return sRet;
}

static dtlCompareResult smdmisCompareOneColNotNull( smdmisTableHeader *aHeader,
                                                    stlChar           *aLeftRow,
                                                    stlChar           *aRightRow )
{
    dtlCompareResult    sRet = DTL_COMPARISON_EQUAL;
    stlChar            *sLeftColValue;
    stlChar            *sRightColValue;
    smdmisKeyColLength  sLeftColLen;
    smdmisKeyColLength  sRightColLen;
    smdmisKeyCol        sLeftKeyCol;
    smdmisKeyCol        sRightKeyCol;
    dtlCompArg          sCompArg;

    sLeftKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aLeftRow, aHeader->mFirstKeyOffset );
    sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sLeftKeyCol );

    sRightKeyCol   = SMDMIS_KEY_ROW_GET_COL_BY_OFFSET( aRightRow, aHeader->mFirstKeyOffset );
    sRightColLen   = SMDMIS_KEY_COL_GET_LEN( sRightKeyCol );

    sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sLeftKeyCol );
    sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sRightKeyCol );

    DTL_TYPED_COMPARE( aHeader->mKeyColInfo[0].mPhysicalCompare,
                       sRet,
                       sCompArg,
                       sLeftColValue,
                       sLeftColLen,
                       sRightColValue,
                       sRightColLen );

    return sRet;
}

dtlCompareResult smdmisCompareKey( smdmisTableHeader *aHeader,
                                   stlChar           *aLeftRow,
                                   stlChar           *aRightRow )
{
    dtlCompareResult sRet = DTL_COMPARISON_EQUAL;
    stlChar         *sLeftColValue;
    stlChar         *sRightColValue;
    stlInt64         sLeftColLen;
    stlInt64         sRightColLen;
    stlInt32         i;
    smdmisKeyCol     sKeyCol;
    smdmifVarCol     sVarCol;
    dtlCompArg       sCompArg;

    for( i = 0; i < aHeader->mKeyColCount; i++ )
    {
        if( aHeader->mKeyCols[i].mIsFixedPart == STL_TRUE )
        {
            sKeyCol       = SMDMIS_KEY_ROW_GET_COL( aLeftRow, &aHeader->mKeyCols[i] );
            sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sKeyCol );
            sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sKeyCol );

            sKeyCol        = SMDMIS_KEY_ROW_GET_COL( aRightRow, &aHeader->mKeyCols[i] );
            sRightColValue = SMDMIS_KEY_COL_GET_VALUE( sKeyCol );
            sRightColLen   = SMDMIS_KEY_COL_GET_LEN( sKeyCol );
        }
        else
        {
            sVarCol = SMDMIS_KEY_ROW_GET_VAR_COL( aLeftRow, &aHeader->mKeyCols[i] );
            if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
            {
                /* var col이 쪼개져 있는 경우 aTempMem에 복사한다. */
                smdmifVarPartReadCol( sVarCol,
                                      aHeader->mColCombineMem1,
                                      &sLeftColLen );
                sLeftColValue = aHeader->mColCombineMem1;
            }
            else
            {
                sLeftColValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                sLeftColLen   = SMDMIF_VAR_COL_GET_LEN( sVarCol );
            }

            sVarCol = SMDMIS_KEY_ROW_GET_VAR_COL( aRightRow, &aHeader->mKeyCols[i] );
            if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
            {
                /* var col이 쪼개져 있는 경우 aTempMem에 복사한다. */
                smdmifVarPartReadCol( sVarCol,
                                      aHeader->mColCombineMem2,
                                      &sRightColLen );
                sRightColValue = aHeader->mColCombineMem2;
            }
            else
            {
                sRightColValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                sRightColLen   = SMDMIF_VAR_COL_GET_LEN( sVarCol );
            }
        }

        if( sLeftColLen == 0 )
        {
            if( sRightColLen == 0 )
            {
                sRet = DTL_COMPARISON_EQUAL;
            }
            else if( aHeader->mKeyColInfo[i].mNullOrderLast == STL_TRUE )
            {
                sRet = DTL_COMPARISON_GREATER;
            }
            else
            {
                sRet = DTL_COMPARISON_LESS;
            }
        }
        else if( sRightColLen == 0 )
        {
            if( aHeader->mKeyColInfo[i].mNullOrderLast == STL_TRUE )
            {
                sRet = DTL_COMPARISON_LESS;
            }
            else
            {
                sRet = DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            DTL_TYPED_COMPARE( aHeader->mKeyColInfo[i].mPhysicalCompare,
                               sRet,
                               sCompArg,
                               sLeftColValue,
                               sLeftColLen,
                               sRightColValue,
                               sRightColLen );

            if( aHeader->mKeyColInfo[i].mAscending == STL_FALSE )
            {
                if( sRet == DTL_COMPARISON_LESS )
                {
                    sRet = DTL_COMPARISON_GREATER;
                }
                else if( sRet == DTL_COMPARISON_GREATER )
                {
                    sRet = DTL_COMPARISON_LESS;
                }
            }
        }

        if( sRet != DTL_COMPARISON_EQUAL )
        {
            break;
        }
    }

    return sRet;
}

static dtlCompareResult smdmisCompareKeyByRadix( smdmisTableHeader *aHeader,
                                                 stlChar           *aLeftRow,
                                                 stlChar           *aRightRow )
{
    stlInt32         i;
    stlUInt16        sRadix;
    stlUInt8         sLeftRadixValue;
    stlUInt8         sRightRadixValue;
    dtlCompareResult sResult = DTL_COMPARISON_EQUAL;

    for( i = 0; i < aHeader->mRadixIndexFence; i++ )
    {
        sRadix = aHeader->mRadixArray[i];

        sLeftRadixValue = SMDMIS_KEY_ROW_GET_RADIX( aLeftRow, sRadix );
        sRightRadixValue = SMDMIS_KEY_ROW_GET_RADIX( aRightRow, sRadix );

        if( sLeftRadixValue < sRightRadixValue )
        {
            sResult = DTL_COMPARISON_LESS;
            break;
        }
        else if( sLeftRadixValue > sRightRadixValue )
        {
            sResult = DTL_COMPARISON_GREATER;
            break;
        }
    }

    return sResult;
}

static inline dtlCompareResult smdmisCompareKeyAndValue( smdmisTableHeader *aHeader,
                                                         stlChar           *aKeyRow,
                                                         stlInt32           aValueIdx,
                                                         knlValueBlockList *aCols )
{
    stlInt32         i;
    stlChar         *sLeftColValue;
    stlChar         *sRightColValue;
    stlInt64         sLeftColLen;
    stlInt64         sRightColLen;
    dtlCompareResult sRet = DTL_COMPARISON_EQUAL;
    smdmisKeyCol     sKeyCol;
    smdmifVarCol     sVarCol;
    dtlCompArg       sCompArg;

    for( i = 0; i < aHeader->mKeyColCount; i++ )
    {
        if( aHeader->mKeyCols[i].mIsFixedPart == STL_TRUE )
        {
            sKeyCol       = SMDMIS_KEY_ROW_GET_COL( aKeyRow, &aHeader->mKeyCols[i] );
            sLeftColValue = SMDMIS_KEY_COL_GET_VALUE( sKeyCol );
            sLeftColLen   = SMDMIS_KEY_COL_GET_LEN( sKeyCol );
        }
        else
        {
            sVarCol = SMDMIS_KEY_ROW_GET_VAR_COL( aKeyRow, &aHeader->mKeyCols[i] );
            if( SMDMIF_VAR_COL_HAS_NEXT( sVarCol ) == STL_TRUE )
            {
                /* var col이 쪼개져 있는 경우 aTempMem에 복사한다. */
                smdmifVarPartReadCol( sVarCol,
                                      aHeader->mColCombineMem1,
                                      &sLeftColLen );
                sLeftColValue = aHeader->mColCombineMem1;
            }
            else
            {
                sLeftColValue = SMDMIF_VAR_COL_VALUE_POS( sVarCol );
                sLeftColLen   = SMDMIF_VAR_COL_GET_LEN( sVarCol );
            }
        }

        sRightColLen   = KNL_GET_BLOCK_DATA_LENGTH( aCols, aValueIdx );
        sRightColValue = KNL_GET_BLOCK_DATA_PTR( aCols, aValueIdx );

        if( sLeftColLen == 0 )
        {
            if( sRightColLen == 0 )
            {
                sRet = DTL_COMPARISON_EQUAL;
            }
            else if( aHeader->mKeyColInfo[i].mNullOrderLast == STL_TRUE )
            {
                sRet = DTL_COMPARISON_GREATER;
            }
            else
            {
                sRet = DTL_COMPARISON_LESS;
            }
        }
        else if( sRightColLen == 0 )
        {
            if( aHeader->mKeyColInfo[i].mNullOrderLast == STL_TRUE )
            {
                sRet = DTL_COMPARISON_LESS;
            }
            else
            {
                sRet = DTL_COMPARISON_GREATER;
            }
        }
        else
        {
            DTL_TYPED_COMPARE( aHeader->mKeyColInfo[i].mPhysicalCompare,
                               sRet,
                               sCompArg,
                               sLeftColValue,
                               sLeftColLen,
                               sRightColValue,
                               sRightColLen );

            if( aHeader->mKeyColInfo[i].mAscending == STL_FALSE )
            {
                if( sRet == DTL_COMPARISON_LESS )
                {
                    sRet = DTL_COMPARISON_GREATER;
                }
                else if( sRet == DTL_COMPARISON_GREATER )
                {
                    sRet = DTL_COMPARISON_LESS;
                }
            }
        }

        if( sRet != DTL_COMPARISON_EQUAL )
        {
            break;
        }

        aCols = aCols->mNext;
    }

    return sRet;
}

/**
 * @brief 인스턴트 테이블을 truncate한다. 사용한 instant memory와 extent들을 모두 해제한다.
 * @param[in] aStatement Statement 객체
 * @param[in] aRelationHandle 인스턴트 테이블 핸들
 * @param[in,out] aEnv Environment 구조체
 */
stlStatus smdmisTableTruncate( smlStatement *aStatement,
                               void         *aRelationHandle,
                               smlEnv       *aEnv )
{
    smdmisTableHeader *sHeader = (smdmisTableHeader *)aRelationHandle;

    /*
     * smdmisTableHeader 맴버들 초기화
     */
    sHeader->mRootNode          = KNL_LOGICAL_ADDR_NULL;
    sHeader->mFirstLeafNode     = KNL_LOGICAL_ADDR_NULL;
    sHeader->mUnorderedLeafList = KNL_LOGICAL_ADDR_NULL;
    sHeader->mFreeNode          = KNL_LOGICAL_ADDR_NULL;
    sHeader->mCurNodeBlock      = KNL_LOGICAL_ADDR_NULL;
    sHeader->mCurBlockFreeSize  = 0;
    sHeader->mRadixIndexFence   = 0;
    sHeader->mCompareKey        = NULL;
    sHeader->mFirstKeyOffset    = 0;

    if( sHeader->mRadixArray != NULL )
    {
        STL_TRY( smgFreeSessShmMem( sHeader->mRadixArray,
                                    aEnv ) == STL_SUCCESS );

        sHeader->mRadixArray = NULL;
    }

    STL_TRY( smdmifTableTruncate( aStatement,
                                  aRelationHandle,
                                  aEnv ) == STL_SUCCESS );

    return STL_SUCCESS;

    STL_FINISH;

    return STL_FAILURE;
}

stlStatus smdmisTableVerify( smdmisTableHeader *aHeader )
{
    smdmisKeyColumn *sKeyCol;
    stlInt32         i;
    smdmisIndexNode *sNode;

    /*
     * 기본 flat instant table 체크
     */
    STL_ASSERT( smdmifTableVerify( &aHeader->mBaseHeader ) == STL_SUCCESS );

    /*
     * key column 체크
     */
    STL_DASSERT( aHeader->mKeyColCount <= aHeader->mBaseHeader.mColCount );

    for( i = 0; i < aHeader->mKeyColCount; i++ )
    {
        sKeyCol = &aHeader->mKeyColInfo[i];
        STL_ASSERT( sKeyCol->mColOrder < aHeader->mBaseHeader.mColCount );
        STL_ASSERT( sKeyCol->mAscending == STL_TRUE || sKeyCol->mAscending == STL_FALSE );
        STL_ASSERT( sKeyCol->mNullOrderLast == STL_TRUE || sKeyCol->mNullOrderLast == STL_FALSE );
        STL_ASSERT( sKeyCol->mPhysicalCompare != NULL );
    }

    /*
     * root node 체크
     */
    if( SMDMIS_TO_NODE_PTR( aHeader->mRootNode ) != NULL )
    {
        STL_ASSERT( smdmisVerifyTree( aHeader,
                                      SMDMIS_TO_NODE_PTR( aHeader->mRootNode ) ) == STL_SUCCESS );
    }
    else
    {
        sNode = SMDMIS_TO_NODE_PTR( aHeader->mFirstLeafNode );

        while( sNode != NULL )
        {
            STL_ASSERT( smdmisVerifyNode( aHeader,
                                          sNode ) == STL_SUCCESS );

            sNode = SMDMIS_INDEX_NODE_GET_NEXT( sNode );
        }
    }

    /*
     * cur node block 체크
     */
    return STL_SUCCESS;
}

static stlStatus smdmisVerifyTree( smdmisTableHeader *aHeader,
                                   smdmisIndexNode   *aNode )
{
    smdmisKeyRow     sKeyRow;
    stlInt32         i;
    smdmisIndexNode *sChildNode;
    smdmisKeyRow     sChildKeyRow;

    STL_ASSERT( smdmisVerifyNode( aHeader,
                                  aNode ) == STL_SUCCESS );

    if( aNode->mLevel > 0 )
    {
        if( SMDMIS_INDEX_NODE_LEFTMOST_NODE( aNode ) != NULL )
        {
            STL_ASSERT( smdmisVerifyTree( aHeader,
                                          SMDMIS_INDEX_NODE_LEFTMOST_NODE( aNode ) ) == STL_SUCCESS );
        }

        for( i = 0; i < aNode->mKeyCount; i++ )
        {
            sKeyRow = SMDMIS_NODE_GET_KEY( aNode, i );
            sChildNode = SMDMIS_TO_NODE_PTR(SMDMIS_KEY_ROW_GET_CHILD(sKeyRow));

            /*
             * sChildNode의 모든 키는 aNode의 i번째 키와 i+1번째 키 사이어야 한다.
             * 즉, i_key(aNode) <= minKey(sChildNode) <= maxKey(sChildNode) <= i+1_key(aNode) 를 
             * 만족해야 한다.
             */
            sChildKeyRow = SMDMIS_NODE_GET_KEY( sChildNode, 0 );

            STL_ASSERT( aHeader->mCompareKey( aHeader,
                                              sKeyRow,
                                              sChildKeyRow ) != DTL_COMPARISON_GREATER );

            if( i < aNode->mKeyCount - 1 )
            {
                sKeyRow = SMDMIS_NODE_GET_KEY( aNode, i + 1 );
                sChildKeyRow = SMDMIS_NODE_GET_KEY( sChildNode, sChildNode->mKeyCount - 1 );

                STL_ASSERT( aHeader->mCompareKey( aHeader,
                                                  sChildKeyRow,
                                                  sKeyRow ) != DTL_COMPARISON_GREATER );
            }

            STL_ASSERT( smdmisVerifyTree( aHeader,
                                          sChildNode ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;
}

static stlStatus smdmisVerifyNode( smdmisTableHeader *aHeader,
                                   smdmisIndexNode   *aNode )
{
    smdmisKeyRow     sKeyRow;
    smdmifFixedRow   sFixedRow;
    stlInt32         i;

    STL_ASSERT( aNode->mKeyCount <= aNode->mMaxKeyCount );

    /*
     * verify order
     */
    STL_ASSERT( smdmisVerifyOrderOfNode( aHeader,
                                         aNode ) == STL_SUCCESS );

    if( aNode->mLevel == 0 )
    {
        for( i = 0; i < aNode->mKeyCount; i++ )
        {
            sKeyRow = SMDMIS_NODE_GET_KEY( aNode, i );
            sFixedRow = SMDMIS_KEY_ROW_GET_FIXED_ROW( sKeyRow );

            STL_ASSERT( smdmifFixedRowVerify( sFixedRow,
                                              aHeader->mBaseHeader.mCols,
                                              aHeader->mBaseHeader.mColCount,
                                              smdmifVarPartVerifyCol ) == STL_SUCCESS );
        }
    }

    return STL_SUCCESS;
}

static stlStatus smdmisVerifyOrderOfNode( smdmisTableHeader *aHeader,
                                          smdmisIndexNode   *aNode )
{
    smdmisKeyRow     sLeftKeyRow;
    smdmisKeyRow     sRightKeyRow;
    stlInt32         i;
    dtlCompareResult sCom;
    stlBool          sEqual = STL_FALSE;
    stlBool          sDupCheck = (aNode->mLevel == 0 ? STL_TRUE : STL_FALSE);

    for( i = 0; i < aNode->mKeyCount - 1; i++ )
    {
        sLeftKeyRow = SMDMIS_NODE_GET_KEY( aNode, i );
        sRightKeyRow = SMDMIS_NODE_GET_KEY( aNode, i + 1 );

        sCom = aHeader->mCompareKey( aHeader,
                                     sLeftKeyRow,
                                     sRightKeyRow );

        if( sCom == DTL_COMPARISON_EQUAL )
        {
            if( sDupCheck == STL_TRUE )
            {
                if( sEqual == STL_FALSE )
                {
                    if( i > 0 )
                    {
                        STL_ASSERT( SMDMIS_KEY_ROW_IS_DUP( sLeftKeyRow ) == STL_FALSE );
                    }
                    STL_ASSERT( SMDMIS_KEY_ROW_IS_DUP( sRightKeyRow ) == STL_TRUE );
                    sEqual = STL_TRUE;
                }
                else
                {
                    STL_ASSERT( SMDMIS_KEY_ROW_IS_DUP( sLeftKeyRow ) == STL_TRUE );
                    STL_ASSERT( SMDMIS_KEY_ROW_IS_DUP( sRightKeyRow ) == STL_TRUE );
                }
            }
        }
        else
        {
            STL_ASSERT( sCom == DTL_COMPARISON_LESS );

            if( sDupCheck == STL_TRUE )
            {
                STL_ASSERT( SMDMIS_KEY_ROW_IS_DUP( sRightKeyRow ) == STL_FALSE );
            }

            sEqual = STL_FALSE;
        }
    }

    return STL_SUCCESS;
}

/** @} */

