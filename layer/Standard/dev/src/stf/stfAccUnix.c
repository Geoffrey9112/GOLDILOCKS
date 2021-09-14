/*******************************************************************************
 * stfAccUnix.c
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

#include <stc.h>
#include <stlDef.h>
#include <stfUnix.h>

mode_t stfUnixPerms2Mode( stlInt32 aPerms )
{
    mode_t sMode = 0;

    if( aPerms & STL_FPERM_USETID )
    {
        sMode |= S_ISUID;
    }
    if( aPerms & STL_FPERM_UREAD )
    {
        sMode |= S_IRUSR;
    }
    if( aPerms & STL_FPERM_UWRITE )
    {
        sMode |= S_IWUSR;
    }
    if( aPerms & STL_FPERM_UEXECUTE )
    {
        sMode |= S_IXUSR;
    }
    if( aPerms & STL_FPERM_GSETID )
    {
        sMode |= S_ISGID;
    }
    if( aPerms & STL_FPERM_GREAD )
    {
        sMode |= S_IRGRP;
    }
    if( aPerms & STL_FPERM_GWRITE )
    {
        sMode |= S_IWGRP;
    }
    if( aPerms & STL_FPERM_GEXECUTE )
    {
        sMode |= S_IXGRP;
    }

#ifdef S_ISVTX
    if( aPerms & STL_FPERM_WSTICKY )
    {
        sMode |= S_ISVTX;
    }
#endif
    if( aPerms & STL_FPERM_WREAD )
    {
        sMode |= S_IROTH;
    }
    if( aPerms & STL_FPERM_WWRITE )
    {
        sMode |= S_IWOTH;
    }
    if( aPerms & STL_FPERM_WEXECUTE )
    {
        sMode |= S_IXOTH;
    }

    return sMode;
}

stlInt32 stfUnixMode2Perms( mode_t aMode )
{
    stlInt32 sPerms = 0;

    if( aMode & S_ISUID )
    {
        sPerms |= STL_FPERM_USETID;
    }
    if( aMode & S_IRUSR )
    {
        sPerms |= STL_FPERM_UREAD;
    }
    if( aMode & S_IWUSR )
    {
        sPerms |= STL_FPERM_UWRITE;
    }
    if( aMode & S_IXUSR )
    {
        sPerms |= STL_FPERM_UEXECUTE;
    }

    if( aMode & S_ISGID )
    {
        sPerms |= STL_FPERM_GSETID;
    }
    if( aMode & S_IRGRP )
    {
        sPerms |= STL_FPERM_GREAD;
    }
    if( aMode & S_IWGRP )
    {
        sPerms |= STL_FPERM_GWRITE;
    }
    if( aMode & S_IXGRP )
    {
        sPerms |= STL_FPERM_GEXECUTE;
    }

#ifdef S_ISVTX
    if( aMode & S_ISVTX )
    {
        sPerms |= STL_FPERM_WSTICKY;
    }
#endif
    if( aMode & S_IROTH )
    {
        sPerms |= STL_FPERM_WREAD;
    }
    if( aMode & S_IWOTH )
    {
        sPerms |= STL_FPERM_WWRITE;
    }
    if( aMode & S_IXOTH )
    {
        sPerms |= STL_FPERM_WEXECUTE;
    }

    return sPerms;
}

