#include "TestStruct.h"


// PyVarObject_HEAD_INIT(NULL, 0) is a macro that expands to the first few mandatory fields of any Python object — the reference count, the type pointer, 
// and the variable size.Every PyTypeObject must start with these three values, and they cannot be zero - initialized with a plain = {} 
// because PyType_Ready() uses them internally before it fills in the rest.
PyTypeObject TestStructType = { PyVarObject_HEAD_INIT(NULL, 0) };

// Getter
static PyObject* TestStruct_get_name(TestStruct* self, void*)
{
    Py_INCREF(self->name);
    return self->name;
}

static PyObject* TestStruct_get_value(TestStruct* self, void*)
{
    return PyFloat_FromDouble(self->value);
}

static PyObject* TestStruct_get_count(TestStruct* self, void*)
{
    return PyLong_FromLong(self->count);
}

static void TestStruct_dealloc(TestStruct* self)
{
    Py_XDECREF(self->name);                      // Release name string — safe if NULL
    Py_TYPE(self)->tp_free((PyObject*)self);      // Free the struct allocation
}

static PyGetSetDef TestStruct_getsetters[] = {
    {"name",  (getter)TestStruct_get_name,  nullptr, "Port name",  nullptr},
    {"value", (getter)TestStruct_get_value, nullptr, "Port value", nullptr},
    {"count", (getter)TestStruct_get_count, nullptr, "Port count", nullptr},
    {NULL}
};

static PyObject* TestStruct_repr(TestStruct* self)
{
    PyObject* pyValue = PyFloat_FromDouble(self->value);
    PyObject* result = PyUnicode_FromFormat(
        "TestStruct(name=%U, value=%R, count=%d)",
        self->name, pyValue, self->count
    );
    Py_DECREF(pyValue);  // Release the temporary float object
    return result;
}

static PyMethodDef TestStruct_methods[] = {
    // {"method_name", (PyCFunction)fn, METH_NOARGS, "docstring"},
    {NULL}   // Sentinel — required even when empty
};

PyObject* TestStruct_create(const char* name, double value, int count)
{
    TestStruct* obj = PyObject_New(TestStruct, &TestStructType);
    if (!obj) return nullptr;

    obj->name = PyUnicode_FromString(name);  // Creates a new Python str
    obj->value = value;
    obj->count = count;
    return (PyObject*)obj;
}


bool TestStruct_registerType(PyObject* module)
{
    // Fill only the fields you care about
    TestStructType.tp_name = "OpenTwin.TestStruct";
    TestStructType.tp_doc = "A test struct.";
    TestStructType.tp_basicsize = sizeof(TestStruct);
    TestStructType.tp_flags = Py_TPFLAGS_DEFAULT;
    TestStructType.tp_new = PyType_GenericNew;
    TestStructType.tp_dealloc = (destructor)TestStruct_dealloc;
    TestStructType.tp_repr = (reprfunc)TestStruct_repr;
    TestStructType.tp_getset = TestStruct_getsetters;
    TestStructType.tp_methods = TestStruct_methods;

    if (PyType_Ready(&TestStructType) < 0)
        return false;

    Py_INCREF(&TestStructType);
    if (PyModule_AddObject(module, "TestStruct", (PyObject*)&TestStructType) < 0) {
        Py_DECREF(&TestStructType);
        return false;
    }
    return true;
}
