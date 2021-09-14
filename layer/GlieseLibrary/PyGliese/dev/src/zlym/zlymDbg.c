/*******************************************************************************
 * zlymObject.c
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

#include <zlyDef.h>
#include <zlymDbg.h>

/**
 * @file zlymObject.c
 * @brief Python Object for Gliese Python Database API
 */

/**
 * @addtogroup zlymObject
 * @{
 */

/**
 * @brief Internal
 */

void PrintBytes(void* p, size_t len)
{
    size_t i = 0;
    unsigned char* pch = (unsigned char*)p;

    for(i = 0; i < len; i++)
        printf("%02x ", (stlUInt32)pch[i]);
    printf("\n");
}


#ifdef PYODBC_TRACE
void DebugTrace(const char* szFmt, ...)
{
    va_list marker;
    va_start(marker, szFmt);
    vprintf(szFmt, marker);
    va_end(marker);
}
#else
void DebugTrace(const char* szFmt, ...)
{
    UNUSED(NULL, szFmt);
}
#endif

#ifdef PYODBC_LEAK_CHECK

// THIS IS NOT THREAD SAFE: This is only designed for the single-threaded unit tests!

typedef struct Allocation
{
    const char* filename;
    int lineno;
    size_t len;
    void* pointer;
    int counter;
} Allocation;

static Allocation* allocs = 0;
static int bufsize = 0;
static int count = 0;
static int allocCounter = 0;

void* _pyodbc_malloc(const char* filename, int lineno, size_t len)
{
    void* p;

    p = malloc(len);
    STL_TRY( p != NULL );

    if (count == bufsize)
    {
        allocs = (Allocation*)realloc(allocs, (bufsize + 20) * sizeof(Allocation));
        STL_TRY_THROW( allocs != NULL, RAMP_ERR_REALLOC );

        bufsize += 20;
    }

    allocs[count].filename = filename;
    allocs[count].lineno   = lineno;
    allocs[count].len      = len;
    allocs[count].pointer  = p;
    allocs[count].counter  = allocCounter++;

    printf("malloc(%d): %s(%d) %d %p\n", allocs[count].counter, filename, lineno, (int)len, p);

    count += 1;

    return p;

    STL_CATH( RAMP_ERR_REALLOC )
    {
        // Yes we just lost the original pointer, but we don't care since everything is about to fail.  This is a
        // debug leak check, not a production malloc that needs to be robust in low memory.
        bufsize = 0;
        count   = 0;
    }
    STL_FINISH;

    return NULL;
}

void pyodbc_free(void* p)
{
    stlInt32 i = 0;

    STL_TRY( p != NULL );

    for(i = 0; i < count; i++)
    {
        if (allocs[i].pointer == p)
        {
            printf("free(%d): %s(%d) %d %p i=%d\n", allocs[i].counter, allocs[i].filename, allocs[i].lineno, (stlInt32)allocs[i].len, allocs[i].pointer, i);
            memmove(&allocs[i], &allocs[i + 1], sizeof(Allocation) * (count - i - 1));
            count -= 1;
            free(p);

            STL_THROW( RAMP_EXIT_FUNC );
        }
    }

    printf("FREE FAILED: %p\n", p);
    free(p);

    STL_RAMP( RAMP_EXIT_FUNC );

    return;

    STL_FINISH;

    return;
}

void pyodbc_leak_check()
{
    if (count == 0)
    {
        printf("NO LEAKS\n");
    }
    else
    {
        printf("********************************************************************************\n");
        printf("%d leaks\n", count);
        for (int i = 0; i < count; i++)
            printf("LEAK: %d %s(%d) len=%d\n", allocs[i].counter, allocs[i].filename, allocs[i].lineno, allocs[i].len);
    }
}

#endif

/**
 * @}
 */
