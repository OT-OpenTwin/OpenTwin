#include "PythonStructValueComparison.h"

#include "PythonStructValueComparison.h"

// ── Getters ────────────────────────────────────────────────────────────────

static PyObject* PythonStructValueComparison_get_name(PythonStructValueComparison* self, void*)
{
    Py_INCREF(self->m_name);
    return self->m_name;
}

static PyObject* PythonStructValueComparison_get_comparator(PythonStructValueComparison* self, void*)
{
    Py_INCREF(self->m_comparator);
    return self->m_comparator;
}

static PyObject* PythonStructValueComparison_get_value(PythonStructValueComparison* self, void*)
{
    Py_INCREF(self->m_value);
    return self->m_value;
}

// ── GetSet table ───────────────────────────────────────────────────────────

static PyGetSetDef PythonStructValueComparison_getsetters[] = {
    {"name",       (getter)PythonStructValueComparison_get_name,       nullptr, "Name",       nullptr},
    {"comparator", (getter)PythonStructValueComparison_get_comparator, nullptr, "Comparator", nullptr},
    {"value",      (getter)PythonStructValueComparison_get_value,      nullptr, "Value",      nullptr},
    {NULL}
};

// ── Methods table ──────────────────────────────────────────────────────────

static PyMethodDef PythonStructValueComparison_methods[] = {
    {NULL}
};

// ── Lifecycle ──────────────────────────────────────────────────────────────

static void PythonStructValueComparison_dealloc(PythonStructValueComparison* self)
{
    Py_XDECREF(self->m_name);
    Py_XDECREF(self->m_comparator);
    Py_XDECREF(self->m_value);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* PythonStructValueComparison_repr(PythonStructValueComparison* self)
{
    return PyUnicode_FromFormat(
        "ValueComparison(name=%U, comparator=%U, value=%U)",
        self->m_name, self->m_comparator, self->m_value
    );
}

// ── Type object ────────────────────────────────────────────────────────────

PyTypeObject PythonStructValueComparisonType = { PyVarObject_HEAD_INIT(NULL, 0) };

// ── Registration ───────────────────────────────────────────────────────────

bool PythonStructValueComparison_registerType(PyObject* module)
{
    PythonStructValueComparisonType.tp_name = "OpenTwin.ValueComparison";
    PythonStructValueComparisonType.tp_doc = "Holds a named value comparison: name, comparator, and value.";
    PythonStructValueComparisonType.tp_basicsize = sizeof(PythonStructValueComparison);
    PythonStructValueComparisonType.tp_itemsize = 0;
    PythonStructValueComparisonType.tp_flags = Py_TPFLAGS_DEFAULT;
    PythonStructValueComparisonType.tp_new = PyType_GenericNew;
    PythonStructValueComparisonType.tp_dealloc = (destructor)PythonStructValueComparison_dealloc;
    PythonStructValueComparisonType.tp_repr = (reprfunc)PythonStructValueComparison_repr;
    PythonStructValueComparisonType.tp_getset = PythonStructValueComparison_getsetters;
    PythonStructValueComparisonType.tp_methods = PythonStructValueComparison_methods;

    if (PyType_Ready(&PythonStructValueComparisonType) < 0)
        return false;

    Py_INCREF(&PythonStructValueComparisonType);
    if (PyModule_AddObject(module, "ValueComparison", (PyObject*)&PythonStructValueComparisonType) < 0)
    {
        Py_DECREF(&PythonStructValueComparisonType);
        return false;
    }
    return true;
}

PyObject* PythonStructValueComparison_create(const ot::ValueComparisonDescription& _valueComparison)
{
    if (!PythonStructValueComparisonType.tp_alloc)
    {
        PyErr_SetString(PyExc_RuntimeError,
            "PythonStructValueComparisonType is not initialized — was registerType called?");
        return nullptr;
    }

    PythonStructValueComparison* obj = PyObject_New(PythonStructValueComparison, &PythonStructValueComparisonType);
    if (!obj) return nullptr;

    obj->m_name = PyUnicode_FromString(_valueComparison.getName().c_str());
    if (!obj->m_name) { Py_DECREF(obj); return nullptr; }

    obj->m_comparator = PyUnicode_FromString(_valueComparison.getComparator().c_str());
    if (!obj->m_comparator) { Py_DECREF(obj); return nullptr; }

    obj->m_value = PyUnicode_FromString(_valueComparison.getValue().c_str());
    if (!obj->m_value) { Py_DECREF(obj); return nullptr; }

    return (PyObject*)obj;
}