/*******************************************************************************
 * goldilocksesql.h
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

#ifndef _GOLDILOCKSESQL_H_
#define _GOLDILOCKSESQL_H_ 1

/**
 * @file goldilocksesql.h
 * @brief Gliese Precompiler of Embedded SQL for C Header file.
 */

#include <stdlib.h>
#include <goldilocks.h>

struct sqlca;

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

typedef struct sqlhv_t
{
    int    io_type;
    int    data_type;
    int    arr_size;
    int    el_size;
    int    buflen;
    int    precision;
    int    scale;
    void * value;
    void * indicator;
    int    ind_type;
    int    ind_el_size;
} sqlhv_t;

typedef struct sqldynhv_t
{
    int       isdesc;         /* is descriptor */
    char    * descname;       /* descriptor name */
    int       hvc;            /* host varible count */
    sqlhv_t * dynhv;          /* dynamic host variable */
} sqldynhv_t;

typedef struct sqlcurprops_t
{
    int    org_type;
    int    cur_type;
    int    sensitivity;
    int    scrollability;
    int    holdability;
    int    updatability;
    int    returnability;
} sqlcurprops_t;



typedef struct sqlcursor_t
{
    char           * sqlcname;             /* SQL cursor name */
    char           * sqldynstmt;           /* dynamic statement name */
    sqlcurprops_t  * sqlcurprops;          /* SQL cursor property */
    sqldynhv_t     * dynusing;             /* dynamic using parameter */
} sqlcursor_t;

typedef struct sqlfetch_t
{
    char   *  sqlcname;             /* SQL cursor name */
    int       sqlfo;                /* cursor fetch orientation */
    sqlhv_t * offsethv;             /* fetch offset host variable */
    int       sqlfph;               /* cursor fetch offset(high part) */
    int       sqlfpl;               /* cursor fetch offset(low part) */
} sqlfetch_t;

typedef struct sqlargs_t
{
    int            sqltype;              /* SQL statement type (INSERT, UPDATE, ... ) */
    int            iters;                /* Iteration count */
    char         * conn;                 /* Connection name */
    struct sqlca * sql_ca;               /* SQL Communication Area */
    char         * sql_state;            /* SQLSTATE */
    char         * sqlstmt;              /* SQL string */
    char         * sqlfn;                /* file name */
    int            sqlln;                /* line number */
    sqlcursor_t  * sqlcursor;            /* cursor information */
    sqlfetch_t   * sqlfetch;             /* fetch information */

    int            atomic;               /* Atomic Insert flag */
    int            unsafenull;           /* --unsafe-null option used */

    int            hvc;                  /* host variable count */
    sqlhv_t      * sqlhv;                /* host variable array */
    sqldynhv_t   * dynusing;             /* dynamic using parameter */
    sqldynhv_t   * dyninto;              /* dynamic into parameter */
} sqlargs_t;

#ifdef __cplusplus
extern "C" {
#endif

/* External function prototype */
int  GOLDILOCKSESQL_Initialize();
int  GOLDILOCKSESQL_Connect( void        * aSqlContext,
                        sqlargs_t   * aSqlArgs,
                        char        * aConnName,
                        char        * aUserName,
                        char        * aPassword );
int  GOLDILOCKSESQL_Disconnect( void       * aSqlContext,
                           sqlargs_t  * aSqlArgs,
                           char       * aConnObject,
                           int          aIsAll );
int  GOLDILOCKSESQL_AllocContext(void      ** aSqlContext,
                            sqlargs_t  * aSqlArgs );
int  GOLDILOCKSESQL_FreeContext( void      ** aSqlContext,
                            sqlargs_t  * aSqlArgs );
int  GOLDILOCKSESQL_EndTran( void       * aSqlContext,
                        sqlargs_t  * aSqlArgs,
                        int          aIsCommit,
                        int          aIsRelease );
int  GOLDILOCKSESQL_Execute( void       * aSqlContext,
                        sqlargs_t  * aSqlArgs );
int  GOLDILOCKSESQL_SetAutoCommit( void       * aSqlContext,
                              sqlargs_t  * aSqlArgs,
                              int          aIsOn );

/***************************************************************
 * Cursor
 ***************************************************************/

int  GOLDILOCKSESQL_DeclareCursor( void       * aSqlContext,
                              sqlargs_t  * aSqlArgs );
int  GOLDILOCKSESQL_OpenCursor( void       * aSqlContext,
                           sqlargs_t  * aSqlArgs );
int  GOLDILOCKSESQL_FetchCursor( void       * aSqlContext,
                            sqlargs_t  * aSqlArgs );
int  GOLDILOCKSESQL_CloseCursor( void       * aSqlContext,
                            sqlargs_t  * aSqlArgs );
int  GOLDILOCKSESQL_PositionedCursorDML( void       * aSqlContext,
                                    sqlargs_t  * aSqlArgs );

/***************************************************************
 * Dynamic SQL
 ***************************************************************/

int GOLDILOCKSESQL_DynamicExecuteImmediate( void      * aSqlContext,
                                       sqlargs_t * aSqlArgs );

int GOLDILOCKSESQL_DynamicPrepare( void      * aSqlContext,
                              sqlargs_t * aSqlArgs,
                              char      * aStmtName );

int GOLDILOCKSESQL_DynamicExecute( void      * aSqlContext,
                              sqlargs_t * aSqlArgs,
                              char      * aStmtName );

#ifdef __cplusplus
}
#endif

#endif /* _GOLDILOCKSESQL_H_ */
