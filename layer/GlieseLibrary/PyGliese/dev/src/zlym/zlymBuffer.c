/*******************************************************************************
 * zlymBuffer.c
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
 * @file zlymBuffer.c
 * @brief Python Parameter's buffer for Gliese Python Database API
 */

/**
 * @addtogroup zlymBuffer
 * @{
 */

/**
 * @brief Internal
 */

#include <zlyDef.h>

#if PY_MAJOR_VERSION < 3

#include <zlymBuffer.h>
#include <zlymModule.h>

Py_ssize_t
PyBuffer_GetMemory(PyObject* buffer, const stlChar** pp)
{
#if PY_VERSION_HEX >= 0x02050000
    stlChar* pT = 0;
#else
    const stlChar* pT = 0;
#endif
    Py_ssize_t cb;

    PyBufferProcs* procs = Py_TYPE(buffer)->tp_as_buffer;

    // Can't access the memory directly because the buffer object doesn't support it.
    STL_TRY( (procs != NULL) &&
             (PyType_HasFeature(Py_TYPE(buffer), Py_TPFLAGS_HAVE_GETCHARBUFFER) != 0) );

    // Can't access the memory directly because there is more than one segment.
    STL_TRY( procs->bf_getsegcount(buffer, 0) == 1 );

    cb = procs->bf_getcharbuffer(buffer, 0, &pT);

    if (pp)
        *pp = pT;

    return cb;

    STL_FINISH;

    return -1;
}

Py_ssize_t
PyBuffer_Size(PyObject* self)
{
    Py_ssize_t total_len = 0;

    STL_TRY_THROW( PyBuffer_Check(self) != 0,
                   RAMP_ERR_NOT_BUFFER );

    Py_TYPE(self)->tp_as_buffer->bf_getsegcount(self, &total_len);

    return total_len;

    STL_CATCH( RAMP_ERR_NOT_BUFFER )
    {
        PyErr_SetString(PyExc_TypeError, "Not a buffer!");
    }
    STL_FINISH;

    return 0;
}

void zlymInitBufSegIterator(zlyBufSegIterator *aIterator, PyObject* _pBuffer)
{
    PyBufferProcs* procs;

    procs = Py_TYPE(_pBuffer)->tp_as_buffer;

    aIterator->pBuffer   = _pBuffer;
    aIterator->iSegment  = 0;
    aIterator->cSegments = procs->bf_getsegcount(aIterator->pBuffer, 0);
}

stlBool zlymGetNextBufSegIterator(zlyBufSegIterator *aIterator, stlUInt8** pb, SQLLEN *cb)
{
    PyBufferProcs* procs;

    if (aIterator->iSegment >= aIterator->cSegments)
        return STL_FALSE;

    procs = Py_TYPE(aIterator->pBuffer)->tp_as_buffer;
    *cb = procs->bf_getreadbuffer(aIterator->pBuffer, aIterator->iSegment ++, (void**)pb);
    return STL_TRUE;
}

#endif

/**
 * @}
 */
