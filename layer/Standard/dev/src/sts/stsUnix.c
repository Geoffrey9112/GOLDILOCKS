/*******************************************************************************
 * stsUnix.c
 *
 * Copyright (c) 2011, SUNJESOFT Inc.
 *
 *
 * IDENTIFICATION & REVISION
 *        $Id: stsUnix.c 3375 2012-01-26 11:42:41Z mkkim $
 *
 * NOTES
 *    
 *
 ******************************************************************************/

#include <stc.h>
#include <stlDef.h>
#include <ste.h>
#include <stlError.h>

#if STC_HAVE_UNISTD_H
#include <unistd.h>
#endif
#if STC_HAVE_SYS_PSTAT_H
#include <sys/pstat.h>
#endif
#if STC_HAVE_SYS_RESOURCE_H
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

stlStatus stsGetCpuInfo( stlCpuInfo    * aCpuInfo,
                         stlErrorStack * aErrorStack )
{
#ifdef STC_TARGET_OS_HPUX
    struct pst_dynamic psd;

    if( pstat_getdynamic(&psd, sizeof(psd), (size_t)1, 0) != -1 )
    {
        aCpuInfo->mCpuCount = (stlInt32)psd.psd_proc_cnt;
    }
    else
    {
        STL_THROW( RAMP_ERR_CPU_INFO );
    }
#else
#ifdef _SC_NPROCESSORS_ONLN
    stlInt32 sName;
    stlInt32 sReturn;

    sName = _SC_NPROCESSORS_ONLN;

    sReturn = sysconf( sName );

    STL_TRY_THROW( sReturn != -1, RAMP_ERR_CPU_INFO );

    aCpuInfo->mCpuCount = sReturn;
#else
    /* 
     * not supoort 
     */
    STL_ASSERT( 0 );
#endif
#endif
    
    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_CPU_INFO )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "stsGetCpuInfo : a queried option is not available",
                      aErrorStack );
        steSetSystemError( errno, aErrorStack );
    }
    
    STL_FINISH;

    return STL_FAILURE;
}

stlStatus stsGetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack )
{
    stlInt32        sResouce = aLimitType;
    struct rlimit   sResourceLimit;

    if( getrlimit( sResouce, &sResourceLimit ) != 0 )
    {
        /* 실패 처리 */
        STL_TRY_THROW( errno != EINVAL, RAMP_ERR_EINVAL );
        STL_TRY_THROW( errno != EPERM, RAMP_ERR_EPERM );
        STL_THROW( RAMP_ERR );
    }

    aLimit->mCurrent = sResourceLimit.rlim_cur;
    aLimit->mMax     = sResourceLimit.rlim_max;

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EINVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "stsGetResourceLimit : invalid argument",
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EPERM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ACCESS,
                      "stsGetResourceLimit : Permission denied",
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "stsGetResourceLimit : points outside the accessible address space",
                      aErrorStack );
    }

    STL_FINISH;

    return STL_FAILURE;
}


stlStatus stsSetResourceLimit( stlResourceLimitType    aLimitType,
                               stlResourceLimit      * aLimit,
                               stlErrorStack         * aErrorStack )
{
    stlInt32        sResouce = aLimitType;
    struct rlimit   sResourceLimit;

    sResourceLimit.rlim_cur = aLimit->mCurrent;
    sResourceLimit.rlim_max = aLimit->mMax;

    if( setrlimit( sResouce, &sResourceLimit ) != 0 )
    {
        /* 실패 처리 */
        STL_TRY_THROW( errno != EINVAL, RAMP_ERR_EINVAL );
        STL_TRY_THROW( errno != EPERM, RAMP_ERR_EPERM );
        STL_THROW( RAMP_ERR );
    }

    return STL_SUCCESS;

    STL_CATCH( RAMP_ERR_EINVAL )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "stsGetResourceLimit : invalid argument",
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR_EPERM )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_ACCESS,
                      "stsGetResourceLimit : Permission denied",
                      aErrorStack );
    }
    STL_CATCH( RAMP_ERR )
    {
        stlPushError( STL_ERROR_LEVEL_ABORT,
                      STL_ERRCODE_INVALID_ARGUMENT,
                      "stsGetResourceLimit : points outside the accessible address space",
                      aErrorStack );
    }
    STL_FINISH;

    return STL_FAILURE;
}

