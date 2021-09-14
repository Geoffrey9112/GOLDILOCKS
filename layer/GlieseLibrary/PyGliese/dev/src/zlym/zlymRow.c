/*******************************************************************************
 * zlymRow.c
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
 * @file zlymRow.c
 * @brief Python Row item for Gliese Python Database API
 */

/**
 * @addtogroup zlymRow
 * @{
 */

/**
 * @brief Internal
 */

#include <zlyDef.h>
#include <zlymModule.h>
#include <zlymRow.h>
#include <zlymPythonWrapper.h>

void FreeRowValues(Py_ssize_t cValues, PyObject** apValues)
{
    Py_ssize_t i = 0;

    if (apValues)
    {
        for(i = 0; i < cValues; i++)
            zlymPyXDecRef(apValues[i]);
        pyodbc_free(apValues);
    }
}

static void Row_dealloc(PyObject* o)
{
    // Note: Now that __newobj__ is available, our variables could be zero...

    zlyRow * self = (zlyRow *)o;

    zlymPyXDecRef(self->description);
    zlymPyXDecRef(self->map_name_to_index);
    FreeRowValues(self->cValues, self->apValues);
    PyObject_Del(self);
}


zlyRow* Row_New(PyObject    *description,
                PyObject    *map_name_to_index,
                Py_ssize_t   cValues,
                PyObject   **apValues)
{
    // Called by other modules to create rows.  Takes ownership of apValues.

#ifdef _MSC_VER
#pragma warning(disable : 4365)
#endif
    zlyRow *row = PyObject_NEW(zlyRow, &RowType);
#ifdef _MSC_VER
#pragma warning(default : 4365)
#endif

    if (row)
    {
        Py_INCREF(description);
        row->description = description;
        Py_INCREF(map_name_to_index);
        row->map_name_to_index = map_name_to_index;
        row->apValues          = apValues;
        row->cValues           = cValues;
    }
    else
    {
        FreeRowValues(cValues, apValues);
    }

    return row;
}


static PyObject* Row_getattro(PyObject* o, PyObject* name)
{
    // Called to handle 'row.colname'.

    zlyRow *self = (zlyRow *)o;

    PyObject* index = PyDict_GetItem(self->map_name_to_index, name);

    if (index)
    {
        Py_ssize_t i = PyNumber_AsSsize_t(index, 0);
        Py_INCREF(self->apValues[i]);
        return self->apValues[i];
    }

    return PyObject_GenericGetAttr(o, name);
}


static Py_ssize_t Row_length(PyObject* self)
{
    return ((zlyRow *)self)->cValues;
}


static stlInt32 Row_contains(PyObject* o, PyObject* el)
{
    // Implementation of contains.  The documentation is not good (non-existent?), so I copied the following from the
    // PySequence_Contains documentation: Return -1 if error; 1 if ob in seq; 0 if ob not in seq.

    zlyRow *self = (zlyRow *)o;
    Py_ssize_t i = 0;
    Py_ssize_t c;
    stlInt32 cmp = 0;

    for (i = 0, c = self->cValues ; cmp == 0 && i < c; ++i)
    {
        cmp = PyObject_RichCompareBool(el, self->apValues[i], Py_EQ);
    }

    return cmp;
}


static PyObject* Row_item(PyObject* o, Py_ssize_t i)
{
    // Apparently, negative indexes are handled by magic ;) -- they never make it here.

    zlyRow *self = (zlyRow *)o;

    if (i < 0 || i >= self->cValues)
    {
        PyErr_SetString(PyExc_IndexError, "tuple index out of range");
        return NULL;
    }

    Py_INCREF(self->apValues[i]);
    return self->apValues[i];
}


static stlInt32 Row_ass_item(PyObject* o, Py_ssize_t i, PyObject* v)
{
    // Implements row[i] = value.

    zlyRow *self = (zlyRow *)o;

    if (i < 0 || i >= self->cValues)
    {
        PyErr_SetString(PyExc_IndexError, "Row assignment index out of range");
        return -1;
    }

    zlymPyXDecRef(self->apValues[i]);
    Py_INCREF(v);
    self->apValues[i] = v;

    return 0;
}


static stlInt32 Row_setattro(PyObject* o, PyObject *name, PyObject* v)
{
    zlyRow *self = (zlyRow *)o;

    PyObject* index = PyDict_GetItem(self->map_name_to_index, name);

    if (index)
        return Row_ass_item(o, PyNumber_AsSsize_t(index, 0), v);

    return PyObject_GenericSetAttr(o, name, v);
}


static PyObject* Row_repr(PyObject* o)
{
    PyObject  *pieces = NULL;
    PyObject  *piece = NULL;
    PyObject  *result = NULL;
    PyObject  *item = NULL;
    zlyRow    *self = (zlyRow *)o;
    Py_ssize_t offset = 0;
    Py_ssize_t i;
    Py_ssize_t length;
    TEXT_T    *buffer;

    if (self->cValues == 0)
        return PyString_FromString("()");

    pieces = PyTuple_New(self->cValues);
    STL_TRY( ZLY_IS_VALID_PYOBJECT(pieces) == STL_TRUE );

    length = 2 + (2 * (self->cValues-1)); // parens + ', ' separators

    for(i = 0; i < self->cValues; i++)
    {
        piece = PyObject_Repr(self->apValues[i]);
        STL_TRY( ZLY_IS_VALID_PYOBJECT(piece) == STL_TRUE );

        length += Text_Size(piece);

        PyTuple_SET_ITEM(pieces, i, piece);
    }

    if (self->cValues == 1)
    {
        // Need a trailing comma: (value,)
        length += 2;
    }

    result = Text_New(length);
    STL_TRY( ZLY_IS_VALID_PYOBJECT(result) == STL_TRUE );

    buffer = Text_Buffer(result);
    offset = 0;
    buffer[offset++] = '(';
    for( i = 0; i < self->cValues; i++ )
    {
        item = PyTuple_GET_ITEM(pieces, i);
        memcpy(&buffer[offset], Text_Buffer(item), Text_Size(item) * sizeof(TEXT_T));
        offset += Text_Size(item);

        if (i != self->cValues-1 || self->cValues == 1)
        {
            buffer[offset++] = ',';
            buffer[offset++] = ' ';
        }
    }
    buffer[offset++] = ')';

    I(offset == length);

    return result;

    STL_FINISH;

    return NULL;
}


static PyObject* Row_richcompare(PyObject* olhs, PyObject* orhs, stlInt32 op)
{
    zlyRow *lhs;
    zlyRow *rhs;
    Py_ssize_t i = 0;
    Py_ssize_t c;

    if (!Row_Check(olhs) || !Row_Check(orhs))
    {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    lhs = (zlyRow *)olhs;
    rhs = (zlyRow *)orhs;

    if (lhs->cValues != rhs->cValues)
    {
        // Different sizes, so use the same rules as the tuple class.
        stlBool result;
        PyObject* p;

        switch (op)
        {
        case Py_EQ: result = (lhs->cValues == rhs->cValues); break;
        case Py_GE: result = (lhs->cValues >= rhs->cValues); break;
        case Py_GT: result = (lhs->cValues >  rhs->cValues); break;
        case Py_LE: result = (lhs->cValues <= rhs->cValues); break;
        case Py_LT: result = (lhs->cValues <  rhs->cValues); break;
        case Py_NE: result = (lhs->cValues != rhs->cValues); break;
        default:
            // Can't get here, but don't have a cross-compiler way to silence this.
            result = STL_FALSE;
        }
        p = result ? Py_True : Py_False;
        Py_INCREF(p);
        return p;
    }

    for(i = 0, c = lhs->cValues; i < c; i++)
        if (!PyObject_RichCompareBool(lhs->apValues[i], rhs->apValues[i], Py_EQ))
            return PyObject_RichCompare(lhs->apValues[i], rhs->apValues[i], op);

    // All items are equal.
    switch (op)
    {
    case Py_EQ:
    case Py_GE:
    case Py_LE:
        Py_RETURN_TRUE;

    case Py_GT:
    case Py_LT:
    case Py_NE:
        break;
    }

    Py_RETURN_FALSE;
}


static PyObject* Row_subscript(PyObject* o, PyObject* key)
{
    PyObject   *result = NULL;
    zlyRow     *row = (zlyRow *)o;

    if (PyIndex_Check(key))
    {
        Py_ssize_t i = PyNumber_AsSsize_t(key, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return 0;
        if (i < 0)
            i += row->cValues;

        if (i < 0 || i >= row->cValues)
            return PyErr_Format(PyExc_IndexError, "row index out of range index=%d len=%d", (stlInt32)i, (stlInt32)row->cValues);

        Py_INCREF(row->apValues[i]);
        return row->apValues[i];
    }

    if (PySlice_Check(key))
    {
        Py_ssize_t start, stop, step, slicelength;
        Py_ssize_t i = 0;
        Py_ssize_t index;

#if PY_VERSION_HEX >= 0x03020000
        if (PySlice_GetIndicesEx(key, row->cValues, &start, &stop, &step, &slicelength) < 0)
            return 0;
#else
        if (PySlice_GetIndicesEx((PySliceObject*)key, row->cValues, &start, &stop, &step, &slicelength) < 0)
            return 0;
#endif

        if (slicelength <= 0)
            return PyTuple_New(0);

        if (start == 0 && step == 1 && slicelength == row->cValues)
        {
            Py_INCREF(o);
            return o;
        }

        result = PyTuple_New(slicelength);
        STL_TRY( ZLY_IS_VALID_PYOBJECT(result) == STL_TRUE );

        for (i = 0, index = start; i < slicelength; i++, index += step)
        {
            PyTuple_SET_ITEM(result, i, row->apValues[index]);
            Py_INCREF(row->apValues[index]);
        }
        return result;
    }

    return PyErr_Format(PyExc_TypeError, "row indices must be integers, not %.200s", Py_TYPE(key)->tp_name);

    STL_FINISH;

    return NULL;
}


static PySequenceMethods row_as_sequence =
{
    Row_length,                 // sq_length
    0,                          // sq_concat
    0,                          // sq_repeat
    Row_item,                   // sq_item
    0,                          // was_sq_slice
    Row_ass_item,               // sq_ass_item
    0,                          // sq_ass_slice
    Row_contains,               // sq_contains
    NULL,                       // sq_inplace_concat
    NULL,                       // sq_inplace_repeat
};


static PyMappingMethods row_as_mapping =
{
    Row_length,                 // mp_length
    Row_subscript,              // mp_subscript
    0,                          // mp_ass_subscript
};


static char description_doc[] = "The Cursor.description sequence from the Cursor that created this row.";

static PyMemberDef Row_members[] =
{
    { "cursor_description", T_OBJECT_EX, STL_OFFSETOF(zlyRow, description), READONLY, description_doc },
    { NULL, 0, 0, 0, NULL }
};

static char row_doc[] =
    "Row objects are sequence objects that hold query results.\n"
    "\n"
    "They are similar to tuples in that they cannot be resized and new attributes\n"
    "cannot be added, but individual elements can be replaced.  This allows data to\n"
    "be \"fixed up\" after being fetched.  (For example, datetimes may be replaced by\n"
    "those with time zones attached.)\n"
    "\n"
    "  row[0] = row[0].replace(tzinfo=timezone)\n"
    "  print row[0]\n"
    "\n"
    "Additionally, individual values can be optionally be accessed or replaced by\n"
    "name.  Non-alphanumeric characters are replaced with an underscore.\n"
    "\n"
    "  cursor.execute(\"select customer_id, [Name With Spaces] from tmp\")\n"
    "  row = cursor.fetchone()\n"
    "  print row.customer_id, row.Name_With_Spaces\n"
    "\n"
    "If using this non-standard feature, it is often convenient to specifiy the name\n"
    "using the SQL 'as' keyword:\n"
    "\n"
    "  cursor.execute(\"select count(*) as total from tmp\")\n"
    "  row = cursor.fetchone()\n"
    "  print row.total";

PyTypeObject RowType =
{
    PyVarObject_HEAD_INIT(NULL, 0)
    "pygliese.Row",                                         // tp_name
    sizeof(zlyRow),                                         // tp_basicsize
    0,                                                      // tp_itemsize
    Row_dealloc,                                            // tp_dealloc
    0,                                                      // tp_print
    0,                                                      // tp_getattr
    0,                                                      // tp_setattr
    0,                                                      // tp_compare
    Row_repr,                                               // tp_repr
    0,                                                      // tp_as_number
    &row_as_sequence,                                       // tp_as_sequence
    &row_as_mapping,                                        // tp_as_mapping
    0,                                                      // tp_hash
    0,                                                      // tp_call
    0,                                                      // tp_str
    Row_getattro,                                           // tp_getattro
    Row_setattro,                                           // tp_setattro
    0,                                                      // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                                     // tp_flags
    row_doc,                                                // tp_doc
    0,                                                      // tp_traverse
    0,                                                      // tp_clear
    Row_richcompare,                                        // tp_richcompare
    0,                                                      // tp_weaklistoffset
    0,                                                      // tp_iter
    0,                                                      // tp_iternext
    0,                                                      // tp_methods
    Row_members,                                            // tp_members
    0,                                                      // tp_getset
    0,                                                      // tp_base
    0,                                                      // tp_dict
    0,                                                      // tp_descr_get
    0,                                                      // tp_descr_set
    0,                                                      // tp_dictoffset
    0,                                                      // tp_init
    0,                                                      // tp_alloc
    0,                                                      // tp_new
    0,                                                      // tp_free
    0,                                                      // tp_is_gc
    0,                                                      // tp_bases
    0,                                                      // tp_mro
    0,                                                      // tp_cache
    0,                                                      // tp_subclasses
    0,                                                      // tp_weaklist
    0,                          // tp_del
    0,                          // tp_version_tag
};

/**
 * @}
 */
