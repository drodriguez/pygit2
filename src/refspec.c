/*
 * Copyright 2010-2013 The pygit2 contributors
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * In addition to the permissions in the GNU General Public License,
 * the authors give you unlimited permission to link the compiled
 * version of this file into combinations with other programs,
 * and to distribute those combinations without any restriction
 * coming from the use of this file.  (The General Public License
 * restrictions do apply in other respects; for example, they cover
 * modification of the file, and distribution when not linked into
 * a combined executable.)
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>
#include "error.h"
#include "utils.h"
#include "types.h"
#include "refspec.h"

extern PyObject *GitError;
extern PyTypeObject RepositoryType;


PyDoc_STRVAR(Refspec_transform__doc__,
    "transform(name) -> str\n"
    "\n"
    "Transform a reference to its target following the refspec's rules.");

PyObject *
Refspec_transform(Refspec *self, PyObject *py_name)
{
    char *name = NULL;
    char *out = NULL;
    size_t outlen = 0;
    int err;
    PyObject *py_out = NULL;

    name = py_path_to_c_str(py_name);
    if (name == NULL)
        return NULL;

    /*
     * Create a buffer enough to hold the refspec part + the given name length
     * + terminator. Those are the calculations done internally by libgit2
     */

    outlen = strlen(git_refspec_dst(self->refspec)) + strlen(name) + 1;
    out = (char *) calloc(outlen, sizeof(char));
    err = git_refspec_transform(out, outlen, self->refspec, name);

    if (err != 0)
        return Error_set(err);

    py_out = to_unicode(out, NULL, NULL);
    free(name);
    return py_out;
}


PyDoc_STRVAR(Refspec_rtransform__doc__,
    "rtransform(name) -> str\n"
    "\n"
    "Transform a target reference to its source reference following the "
    "refspec's rules.");

PyObject *Refspec_rtransform(Refspec *self, PyObject *py_name)
{
    char *name = NULL;
    char *out = NULL;
    size_t outlen = 0;
    int err;
    PyObject *py_out = NULL;

    name = py_path_to_c_str(py_name);
    if (name == NULL)
        return NULL;

    /*
     * Create a buffer enough to hold the refspec part + the given name length
     * + terminator. Those are the calculations done internally by libgit2
     */

    outlen = strlen(git_refspec_src(self->refspec)) + strlen(name) + 1;
    out = (char *) calloc(outlen, sizeof(char));
    err = git_refspec_rtransform(out, outlen, self->refspec, name);

    if (err != 0)
        return Error_set(err);

    py_out = to_unicode(out, NULL, NULL);
    free(name);
    return py_out;
}


PyDoc_STRVAR(Refspec_dst_matches__doc__,
    "dst_matches(refname) -> str\n"
    "\n"
    "Check if a refspec's destination descriptor matches a reference.");

PyObject *Refspec_dst_matches(Refspec *self, PyObject *py_refname)
{
    char *refname = NULL;

    refname = py_path_to_c_str(py_refname);
    if (refname == NULL)
        return NULL;

    if (git_refspec_dst_matches(self->refspec, refname)) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}


PyDoc_STRVAR(Refspec_src_matches__doc__,
    "src_matches(refname) -> str\n"
    "\n"
    "Check if a refspec's source descriptor matches a reference.");

PyObject *Refspec_src_matches(Refspec *self, PyObject *py_refname)
{
    char *refname = NULL;

    refname = py_path_to_c_str(py_refname);
    if (refname == NULL)
        return NULL;

    if (git_refspec_src_matches(self->refspec, refname)) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}


PyDoc_STRVAR(Refspec_is_forced__doc__, "The force update setting of the refspec.");

PyObject *
Refspec_is_forced__get__(Refspec *self)
{
    if (git_refspec_force(self->refspec)) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}


PyDoc_STRVAR(Refspec_direction__doc__, "The direction of the refspec (fetch or push).");

PyObject *
Refspec_direction__get__(Refspec *self)
{
    return PyLong_FromLong((long) git_refspec_direction(self->refspec));
}


PyDoc_STRVAR(Refspec_destination__doc__, "The destination specifier of the refspec.");

PyObject *
Refspec_destination__get__(Refspec *self)
{
    const char *dst = git_refspec_dst(self->refspec);
    return to_unicode(dst, NULL, NULL);
}


PyDoc_STRVAR(Refspec_source__doc__, "The source specifier of the refspec.");

PyObject *
Refspec_source__get__(Refspec *self)
{
    const char *src = git_refspec_src(self->refspec);
    return to_unicode(src, NULL, NULL);
}


PyDoc_STRVAR(Refspec_string__doc__, "The complete reference specifier.");

PyObject *
Refspec_string__get__(Refspec *self)
{
    const char *string = git_refspec_string(self->refspec);
    return to_unicode(string, NULL, NULL);
}


static PyObject *
Refspec_str(Refspec *self)
{
    const char *string = git_refspec_string(self->refspec);
    return to_unicode(string, NULL, NULL);
}


PyMethodDef Refspec_methods[] = {
    METHOD(Refspec, transform, METH_O),
    METHOD(Refspec, rtransform, METH_O),
    METHOD(Refspec, dst_matches, METH_O),
    METHOD(Refspec, src_matches, METH_O),
    {NULL}
};

PyGetSetDef Refspec_getseters[] = {
    GETTER(Refspec, is_forced),
    GETTER(Refspec, direction),
    GETTER(Refspec, destination),
    GETTER(Refspec, source),
    GETTER(Refspec, string),
    {NULL}
};

PyDoc_STRVAR(Refspec__doc__, "Refspec object.");

PyTypeObject RefspecType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "_pygit2.Refspec",                         /* tp_name           */
    sizeof(Refspec),                           /* tp_basicsize      */
    0,                                         /* tp_itemsize       */
    0,                                         /* tp_dealloc        */
    0,                                         /* tp_print          */
    0,                                         /* tp_getattr        */
    0,                                         /* tp_setattr        */
    0,                                         /* tp_compare        */
    0,                                         /* tp_repr           */
    0,                                         /* tp_as_number      */
    0,                                         /* tp_as_sequence    */
    0,                                         /* tp_as_mapping     */
    0,                                         /* tp_hash           */
    0,                                         /* tp_call           */
    (reprfunc)Refspec_str,                     /* tp_str            */
    0,                                         /* tp_getattro       */
    0,                                         /* tp_setattro       */
    0,                                         /* tp_as_buffer      */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,  /* tp_flags          */
    Refspec__doc__,                            /* tp_doc            */
    0,                                         /* tp_traverse       */
    0,                                         /* tp_clear          */
    0,                                         /* tp_richcompare    */
    0,                                         /* tp_weaklistoffset */
    0,                                         /* tp_iter           */
    0,                                         /* tp_iternext       */
    Refspec_methods,                           /* tp_methods        */
    0,                                         /* tp_members        */
    Refspec_getseters,                         /* tp_getset         */
    0,                                         /* tp_base           */
    0,                                         /* tp_dict           */
    0,                                         /* tp_descr_get      */
    0,                                         /* tp_descr_set      */
    0,                                         /* tp_dictoffset     */
    0,                                         /* tp_init           */
    0,                                         /* tp_alloc          */
    0,                                         /* tp_new            */
};
