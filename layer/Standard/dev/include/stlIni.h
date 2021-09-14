/*******************************************************************************
 * stlIni.h
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


#ifndef _STLINI_H_
#define _STLINI_H_ 1

/**
 * @file stlIni.h
 * @brief Standard Layer INI Routines
 */


STL_BEGIN_DECLS

/**
 * @defgroup stlIni Ini
 * @ingroup STL 
 * @{
 */

/*
 * INI FILE
 */
stlStatus stlOpenIni( stlIni        * aIni,
                      stlChar       * aFileName,
                      stlErrorStack * aErrorStack );

stlStatus stlCloseIni( stlIni        * aIni,
                       stlErrorStack * aErrorStack );

/*
 * OBJECT
 */
stlStatus stlFindIniObject( stlIni         * aIni,
                            stlChar        * aObjectName,
                            stlBool        * aFound,
                            stlIniObject  ** aObject,
                            stlErrorStack  * aErrorStack );

/*
 * PROPERTY
 */

stlStatus stlFindIniProperty( stlIniObject  * aObject,
                              stlChar       * aPropertyName,
                              stlChar       * aPropertyValue,
                              stlBool       * aFound,
                              stlErrorStack * aErrorStack );

/** @} */
    
STL_END_DECLS

#endif /* _STLINI_H_ */
