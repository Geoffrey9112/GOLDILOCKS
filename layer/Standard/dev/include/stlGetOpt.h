/*******************************************************************************
 * stlGetOpt.h
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


#ifndef _STLGETOPT_H_
#define _STLGETOPT_H_ 1

/**
 * @file stlGetOpt.h
 * @brief Standard Layer Command Arguments Routines
 */

STL_BEGIN_DECLS


/**
 * @defgroup stlGetOpt Command Argument Parsing
 * @ingroup STL 
 * @{
 */

stlStatus stlInitGetOption(stlGetOpt             * aGetOpt,
                           stlInt32                aArgc,
                           const stlChar * const * aArgv,
                           stlErrorStack         * aErrorStack);
                           
stlStatus stlGetOption(stlGetOpt      * aGetOpt,
                       const stlChar  * aOptString,
                       stlChar        * aOptCh,
                       const stlChar ** aOptArg,
                       stlErrorStack  * aErrorStack);

stlStatus stlGetOptionLong(stlGetOpt        * aGetOpt,
                           stlGetOptOption  * aOption,
                           stlChar          * aOptCh,
                           const stlChar   ** aOptArg,
                           stlErrorStack    * aErrorStack);
/** @} */
    
STL_END_DECLS

#endif /* _STLGETOPT_H_ */
