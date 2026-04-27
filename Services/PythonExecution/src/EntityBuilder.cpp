#include "EntityBuilder.h"

// ── Type object ────────────────────────────────────────────────────────────

PyTypeObject EntityBuilderType = { PyVarObject_HEAD_INIT(NULL, 0) };


// ── Getters ────────────────────────────────────────────────────────────────

static PyObject* EntityBuilder_get_name(EntityBuilder* self, void*)
{
    Py_INCREF(self->m_name);
    return self->m_name;
}

static PyObject* EntityBuilder_get_entityType(EntityBuilder* self, void*)
{
    Py_INCREF(self->m_entityType);
    return self->m_entityType;
}

static PyObject* EntityBuilder_get_data(EntityBuilder* self, void*)
{
    Py_INCREF(self->m_data);
    return self->m_data;
}

// ── Setters ────────────────────────────────────────────────────────────────
// Builder pattern — setters return the object itself for chaining (see below)

static int EntityBuilder_set_name(EntityBuilder* self, PyObject* value, void*)
{
    if (!PyUnicode_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "name must be a string");
        return -1;  // -1 signals failure for setters
    }
    Py_XDECREF(self->m_name);  // Release old value
    Py_INCREF(value);
    self->m_name = value;
    return 0;  // 0 signals success
}

static int EntityBuilder_set_entityType(EntityBuilder* self, PyObject* value, void*)
{
    if (!PyUnicode_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "entityType must be a string");
        return -1;
    }
    Py_XDECREF(self->m_entityType);
    Py_INCREF(value);
    self->m_entityType = value;
    return 0;
}

static int EntityBuilder_set_data(EntityBuilder* self, PyObject* value, void*)
{
    // Accept dict or None — reject everything else
    if (value != Py_None && !PyDict_Check(value))
    {
        PyErr_SetString(PyExc_TypeError, "data must be a dict or None");
        return -1;
    }
    Py_XDECREF(self->m_data);
    Py_INCREF(value);
    self->m_data = value;
    return 0;
}

// ── GetSet table ───────────────────────────────────────────────────────────

static PyGetSetDef EntityBuilder_getsetters[] = {
    {"name",       (getter)EntityBuilder_get_name,       (setter)EntityBuilder_set_name,       "Entity name",       nullptr},
    {"entityType", (getter)EntityBuilder_get_entityType, (setter)EntityBuilder_set_entityType, "Entity type",       nullptr},
    {"data",       (getter)EntityBuilder_get_data,       (setter)EntityBuilder_set_data,       "Entity data",       nullptr},
    {NULL}
};

// ── Build method ───────────────────────────────────────────────────────────

std::map<std::string, std::string> externalToInternalTypes = { {"Curve","EntityResult1DCurve"}, {"Plot","EntityResult1DPlot"}, {"3DResults","EntityResult3D"} };

static PyObject* EntityBuilder_build(EntityBuilder* self, PyObject* Py_UNUSED(ignored))
{
    const char* name = PyUnicode_AsUTF8(self->m_name);
    const char* entityType = PyUnicode_AsUTF8(self->m_entityType);

    if (!name || !entityType)
    {
        PyErr_SetString(PyExc_ValueError, "name and entityType must be set before calling build()");
        return nullptr;
    }

    // Extract individual keys from the data dict if present
    //const char* resolution = nullptr;
    //if (self->m_data && self->m_data != Py_None)
    //{
    //    PyObject* pyResolution = PyDict_GetItemString(self->m_data, "resolution");
    //    if (pyResolution)
    //        resolution = PyUnicode_AsUTF8(pyResolution);  // if stored as string
    //}

    //bool success = YourCppBuilder::build(name, entityType, resolution);
    //if (!success)
    //{
    //    PyErr_Format(PyExc_RuntimeError,
    //        "build() failed for entity '%s' of type '%s'", name, entityType);
    //    return nullptr;
    //}

    Py_RETURN_NONE;
}

// ── Methods table ──────────────────────────────────────────────────────────

static PyMethodDef EntityBuilder_methods[] = {
    {"build", (PyCFunction)EntityBuilder_build, METH_NOARGS, "Build the entity in OpenTwin."},
    {NULL}
};

// ── Lifecycle ──────────────────────────────────────────────────────────────

static void EntityBuilder_dealloc(EntityBuilder* self)
{
    Py_XDECREF(self->m_name);
    Py_XDECREF(self->m_entityType);
    Py_XDECREF(self->m_data);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* EntityBuilder_repr(EntityBuilder* self)
{
    return PyUnicode_FromFormat(
        "EntityDescription(name=%U, entityType=%U, data=%R)",
        self->m_name, self->m_entityType, self->m_data
    );
}



static int EntityBuilder_init(EntityBuilder* self, PyObject* args, PyObject* kwargs)
{
    const char* name = nullptr;
    const char* entityType = nullptr;
    PyObject* data = Py_None;  // PyObject* instead of const char*

    static const char* kwlist[] = { "name", "entityType", "data", nullptr };

    // "ss|O" — two required strings, one optional Python object
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ss|O",
        const_cast<char**>(kwlist),
        &name, &entityType, &data))
        return -1;

    // Validate the optional data argument
    if (data != Py_None && !PyDict_Check(data))
    {
        PyErr_SetString(PyExc_TypeError, "data must be a dict or None");
        return -1;
    }

    Py_XDECREF(self->m_name);
    Py_XDECREF(self->m_entityType);
    Py_XDECREF(self->m_data);

    self->m_name = PyUnicode_FromString(name);
    self->m_entityType = PyUnicode_FromString(entityType);
    self->m_data = data;
    Py_INCREF(self->m_data);

    return 0;
}


// ── Registration ───────────────────────────────────────────────────────────

bool EntityBuilder_registerType(PyObject* module)
{
    EntityBuilderType.tp_name = "OpenTwin.EntityBuilder";
    EntityBuilderType.tp_doc = "Builder for OpenTwin entities.";
    EntityBuilderType.tp_basicsize = sizeof(EntityBuilder);
    EntityBuilderType.tp_itemsize = 0;
    EntityBuilderType.tp_flags = Py_TPFLAGS_DEFAULT;
    EntityBuilderType.tp_new = PyType_GenericNew;
    EntityBuilderType.tp_dealloc = (destructor)EntityBuilder_dealloc;
    EntityBuilderType.tp_repr = (reprfunc)EntityBuilder_repr;
    EntityBuilderType.tp_getset = EntityBuilder_getsetters;
    EntityBuilderType.tp_methods = EntityBuilder_methods;
    EntityBuilderType.tp_init = (initproc)EntityBuilder_init;
    if (PyType_Ready(&EntityBuilderType) < 0)
        return false;

    Py_INCREF(&EntityBuilderType);
    if (PyModule_AddObject(module, "EntityBuilder", (PyObject*)&EntityBuilderType) < 0)
    {
        Py_DECREF(&EntityBuilderType);
        return false;
    }
    return true;
}

