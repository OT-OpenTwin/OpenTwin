#include "FixtureNumpyAPI.h"
#include "OTCore/Variable.h"


int initiateNumpy()
{
	import_array1(0);
	return 0;
}

TEST_F(FixtureNumpyAPI, Array_1D_Access)
{
	initiateNumpy();
	// Create an array of float values
	double expected = 3.;
	npy_float64 data[] = { 1.0, 2.0, expected, 4.0, 5.5 };
	npy_intp dims[] = { 5 };  // Number of elements in the array

	// Create a NumPy array from the existing data
	PyObject* numpy_array = PyArray_SimpleNewFromData(1, dims, NPY_FLOAT64, data);

	// Access an element from the array (e.g., the third element)
	double third_element = *(double*)PyArray_GETPTR1((PyArrayObject*)numpy_array, 2);

	ASSERT_DOUBLE_EQ(third_element,expected);
	// Print the entire array and the third element
	
	// Clean up
	Py_DECREF(numpy_array);
}

TEST_F(FixtureNumpyAPI, Array_2D_Access)
{
	initiateNumpy();
	double expected = 7.7;
	npy_float64 data[] = { 1.0, 2.0, 3., 4.0, 5.5, 6., expected,8.8,9.9 };
	npy_intp dims[] = { 3,3,3 };

	// Create a NumPy array from the existing data
	PyObject* numpy_array = PyArray_SimpleNewFromData(3, dims, NPY_FLOAT64, data);

	// Access an element from the array (e.g., the third element)
	double third_element = *(double*)PyArray_GETPTR2((PyArrayObject*)numpy_array, 0,2);

	ASSERT_DOUBLE_EQ(third_element, expected);
	// Print the entire array and the third element

	// Clean up
	Py_DECREF(numpy_array);
}