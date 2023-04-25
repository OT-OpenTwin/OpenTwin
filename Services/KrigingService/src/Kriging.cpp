#include "Kriging.h"

//	Constructors
Kriging::Kriging(const std::string scriptName) : IBase(scriptName) {
	std::cout << "initialize kriging object\n";
}
Kriging::Kriging(const std::string scriptName, KrigingParams params) : IBase(scriptName) {
	std::cout << "initialize Kriging with parameters\n";
	this->params = params;
}
Kriging::~Kriging() {
	std::cout << "destroying kriging object\n";
	Py_Finalize();
}

// Getters and Setters
DataSet Kriging::getDataSet() { return this->ds; }
void Kriging::setDataSet(DataSet ds) { this->ds = ds; }

KrigingParams Kriging::getParameters() { return this->params; }
void Kriging::setParameters(const KrigingParams params) { this->params = params; }

string Kriging::getModelStr() { return this->modelStr; }
void Kriging::setModelStr(const string model) { this->modelStr = model; }

//	Function Definitions
void Kriging::initializePython() {
	cout << "py_isinitialized: " << Py_IsInitialized() << endl;
	if (Py_IsInitialized() == 0) {
		//Py_SetPath(L"D:\\OpenTwin\\SimulationPlatform\\Deployment\\Python;D:\\OpenTwin\\SimulationPlatform\\Deployment\\Python\\Scripts;D:\\OpenTwin\\SimulationPlatform\\Deployment\\Python\\DLLs;D:\\OpenTwin\\SimulationPlatform\\Deployment\\Python\\lib;D:\\OpenTwin\\SimulationPlatform\\Deployment\\Python\\lib\\site-packages");

		// todo: can we automate to use 1st for build and 2nd for running through IDE???
		//Py_SetPythonHome(L"Python");
		Py_SetPythonHome(L"D:\\OpenTwin\\SimulationPlatform\\Deployment\\Python");

		/*wcout << Py_GetPath() << endl << endl;
		wcout << Py_GetPythonHome() << endl << endl;
		wcout << Py_GetProgramFullPath() << endl << endl;
		wcout << Py_GetProgramName() << endl << endl;*/

		Py_Initialize();

		cout << "py is initialized: " << Py_IsInitialized() << endl;
	}
}

int Kriging::predict() {
	initializePython();
	import_array();

	// Build the 2D array
	PyObject *pArgs, *pReturn, *pModule, *pFunc;
	PyArrayObject *np_arg;
	//PyArrayObject *np_ret;
	const int SIZE{ 10 };
	npy_intp dims[2]{ SIZE, SIZE };
	const int ND{ 2 };
	double(*c_arr)[SIZE] { new double[SIZE][SIZE] };
	//double* c_out;
	for (int i{}; i < SIZE; i++)
		for (int j{}; j < SIZE; j++)
			c_arr[i][j] = i * SIZE + j + 0.11 * i;

	np_arg = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNewFromData(ND, dims, NPY_DOUBLE,
		reinterpret_cast<void*>(c_arr)));

	// Calling array_tutorial from mymodule
	PyObject *pName = PyUnicode_FromString("run");
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (!pModule) {
		std::cout << "mymodule can not be imported" << endl;
		Py_DECREF(np_arg);
		delete[] c_arr;
		return 0;
	}
	pFunc = PyObject_GetAttrString(pModule, "test");
	if (!pFunc || !PyCallable_Check(pFunc)) {
		Py_DECREF(pModule);
		Py_XDECREF(pFunc);
		Py_DECREF(np_arg);
		delete[] c_arr;
		std::cout << "function is null or not callable" << endl;
		return 0;
	}
	pArgs = PyTuple_New(2);
	//PyTuple_SetItem(pArgs, 0, PyByteArray_FromStringAndSize(this->model, this->model.size);
	if (this->modelStr.empty()) {
		std::cout << "model not found" << endl;
	}
	else {
		//PyTuple_SetItem(pArgs, 0, this->pyobj);
		//PyTuple_SetItem(pArgs, 0, PyBytes_FromString(this->m));
		PyTuple_SetItem(pArgs, 0, PyBytes_FromString(this->modelStr.c_str()));
		PyTuple_SetItem(pArgs, 1, reinterpret_cast<PyObject*>(np_arg));
		pReturn = PyObject_CallObject(pFunc, pArgs);
		if (pReturn != NULL) {
			//printf("Result of call: %ld\n");
			Py_DECREF(pReturn);
		}
		else {
			Py_DECREF(pFunc);
			Py_DECREF(pModule);
			PyErr_Print();
			fprintf(stderr, "Call failed\n");
		}
	}

	Py_XDECREF(pFunc);
	Py_DECREF(pModule);

	return 0;
}

void Kriging::getModel() {
	PyObject *pReturn, *pModule, *pFunc;

	PyObject *pName = PyUnicode_FromString(this->scriptName.c_str());
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (!pModule) {
		std::cout << "mymodule can not be imported" << endl;
		return;
	}
	pFunc = PyObject_GetAttrString(pModule, "get_model");
	if (!pFunc || !PyCallable_Check(pFunc)) {
		Py_DECREF(pModule);
		Py_XDECREF(pFunc);
		std::cout << "function is null or not callable" << endl;
		return;
	}

	pReturn = PyObject_CallNoArgs(pFunc);

	if (pReturn != NULL) {
		PyObject* o = PyUnicode_AsUTF8String(pReturn);
		this->modelStr = PyBytes_AsString(o);
		//this->model = PyBytes_AsString(this->pyobj);
		printf("\nModel saved!!\n");
		Py_DECREF(pReturn);
	}
	else {
		Py_DECREF(pFunc);
		Py_DECREF(pModule);
		PyErr_Print();
		fprintf(stderr, "Call failed\n");
	}

	Py_XDECREF(pFunc);
	Py_DECREF(pModule);
}


int Kriging::setDataset(std::list<std::vector<double>> d)
{
	initializePython();
	import_array();

	// Build the 2D array
	PyObject *pArgs, *pReturn, *pModule, *pFunc;
	PyArrayObject *np_ret, *np_arg;
	const int SIZE{ 10 };
	npy_intp dims[2]{ SIZE, SIZE };
	const int ND{ 2 };
	double(*c_arr)[SIZE] { new double[SIZE][SIZE] };
	double(*c_arr_out)[SIZE] { new double[SIZE][SIZE] };
	//double* c_out;
	for (int i{}; i < SIZE; i++)
		for (int j{}; j < SIZE; j++)
			c_arr[i][j] = i * SIZE + j; // +0.1111 * i;

	np_arg = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNewFromData(ND, dims, NPY_DOUBLE,
		reinterpret_cast<void*>(c_arr)));

	// Calling array_tutorial from mymodule
	PyObject *pName = PyUnicode_FromString(this->scriptName.c_str());
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (!pModule) {
		std::cout << "mymodule can not be imported" << endl;
		Py_DECREF(np_arg);
		delete[] c_arr;
		return 1;
	}
	pFunc = PyObject_GetAttrString(pModule, "array_test");
	if (!pFunc || !PyCallable_Check(pFunc)) {
		Py_DECREF(pModule);
		Py_XDECREF(pFunc);
		Py_DECREF(np_arg);
		delete[] c_arr;
		std::cout << "function is null or not callable" << endl;
		return 1;
	}
	pArgs = PyTuple_New(1);
	PyTuple_SetItem(pArgs, 0, reinterpret_cast<PyObject*>(np_arg));
	pReturn = PyObject_CallObject(pFunc, pArgs);
	np_ret = reinterpret_cast<PyArrayObject*>(pReturn);
	if (PyArray_NDIM(np_ret) != ND) { // row[0] is returned
		std::cout << "Function returned with wrong dimension" << endl;
		Py_DECREF(pFunc);
		Py_DECREF(pModule);
		Py_DECREF(np_arg);
		Py_DECREF(np_ret);
		delete[] c_arr;
		return 1;
	}
	npy_intp len{ PyArray_SHAPE(np_ret)[0] };
	c_arr_out = reinterpret_cast<double(*)[SIZE]>(PyArray_DATA(np_ret));
	//c_out = reinterpret_cast<double*>(PyArray_DATA(np_ret));
	std::cout << "Printing output array" << endl << len << endl;
	for (int i{}; i < len; i++) {
		//std::cout << c_out[i] << ' ';

		for (int j{}; j < 3; j++)
			std::cout << c_arr_out[i][j] << '\t';
		std::cout << endl;
	}
	std::cout << endl;

	// Finalizing
	Py_DECREF(pFunc);
	Py_DECREF(pModule);
	Py_DECREF(np_arg);
	Py_DECREF(np_ret);
	delete[] c_arr;

	return 0;
}

int Kriging::train()
{
	initializePython();
	import_array();

	params.printIt();

	std::array<long, 2> dataSize = this->ds.getDataSize();
	std::list<std::vector<double>> xData = this->ds.getxDataPoints();
	std::list<std::vector<double>> yData = this->ds.getyDataPoints();

	const size_t ROW_SIZE = xData.size();
	if (ROW_SIZE == 0) return 0;

	const size_t X_COL_SIZE = dataSize[0];
	const size_t Y_COL_SIZE = dataSize[1];

	const size_t COL_SIZE = X_COL_SIZE + Y_COL_SIZE;

	npy_intp dims[2]{ (npy_intp) ROW_SIZE, (npy_intp) COL_SIZE };
	const int ND{ 2 };

	double(*c_arr) { new double[ROW_SIZE * COL_SIZE] };

	int i = 0;
	std::list<std::vector<double>>::iterator it = yData.begin();
	for (const std::vector<double> & row : xData)
	{
		for (int j{}; j < row.size(); j++) {
			c_arr[i * COL_SIZE + j] = row.at(j);
			std::cout << c_arr[i * COL_SIZE + j] << "\t";
		}
		cout << "| ";

		const std::vector<double> yrow = *it;
		for (int j{}; j < yrow.size(); j++) {
			c_arr[i * COL_SIZE + X_COL_SIZE + j] = yrow.at(j);
			std::cout << c_arr[i * COL_SIZE + X_COL_SIZE + j] << "\t";
		}
		it = std::next(it, 1);
		std::cout << std::endl;

		i = i + 1;
	}


	PyArrayObject *np_arg;
	np_arg = reinterpret_cast<PyArrayObject*>(
		PyArray_SimpleNewFromData(ND, dims, NPY_DOUBLE, reinterpret_cast<void*>(c_arr))
		);


	PyObject *pName, *pModule, *pFunc;
	PyObject *pArgs, *pValue;

	Py_Initialize();
	pName = PyUnicode_DecodeFSDefault(this->scriptName.c_str());
	/* Error checking of pName left out */

	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule != NULL) {
		pFunc = PyObject_GetAttrString(pModule, "train");
		/* pFunc is a new reference */

		if (pFunc && PyCallable_Check(pFunc)) {
			pArgs = PyTuple_New(5);
			PyTuple_SetItem(pArgs, 0, PyFloat_FromDouble(params.theta));
			PyTuple_SetItem(pArgs, 1, PyFloat_FromDouble(params.nugget));
			PyTuple_SetItem(pArgs, 2, PyUnicode_FromString(params.corr.c_str()));
			PyTuple_SetItem(pArgs, 3, PyUnicode_FromString(params.poly.c_str()));
			PyTuple_SetItem(pArgs, 4, reinterpret_cast<PyObject*>(np_arg));

			pValue = PyObject_CallObject(pFunc, pArgs);
			Py_DECREF(pArgs);
			if (pValue != NULL) {
				printf("\n\n\nResult received\n\n");
				// ok, call the function
				double trError = 0, teError = 0, trAcc = 0, teAcc = 0;
				PyObject *pymodel;
				PyArg_ParseTuple(pValue, "d|d|d|d|O", &trError, &teError, &trAcc, &teAcc, &pymodel);

				if (pymodel) {
					std::cout << "model recieved";
					PyObject *o = PyUnicode_AsUTF8String(pymodel);
					this->modelStr = PyBytes_AsString(o);
				}
				std::cout << "\n" << trError << "\t" << teError << "\t" << trAcc << "\t" << teAcc;

				Py_DECREF(pValue);
			}
			else {
				Py_DECREF(pFunc);
				Py_DECREF(pModule);
				PyErr_Print();
				fprintf(stderr, "Call failed\n");
			}
		}
		else {
			if (PyErr_Occurred())
				PyErr_Print();
			//fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
		delete[] c_arr;
	}
	else {
		PyErr_Print();
		Py_DECREF(np_arg);
		delete[] c_arr;
		//fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
	}

	return 0;
}

int Kriging::predict(std::list<std::vector<double>> data)
{
	this->outputs.clear();
	this->outputs.append("predicting ...\n");
	if (this->modelStr.empty()) {
		std::cout << "model not found" << endl;
		this->outputs.append("model not found.\n");
		return 0;
	}

	initializePython();
	import_array();

	std::array<long, 2> dataSize = this->ds.getDataSize();

	const size_t ROW_SIZE = data.size();
	const size_t X_COL_SIZE = dataSize[0];
	const size_t Y_COL_SIZE = dataSize[1];

	const size_t COL_SIZE = X_COL_SIZE;

	if (ROW_SIZE == 0) return 0;

	npy_intp dims[2]{ (npy_intp) ROW_SIZE, (npy_intp) COL_SIZE };
	const int ND{ 2 };

	double(*c_arr) { new double[ROW_SIZE * COL_SIZE] };

	int i = 0;
	for (const std::vector<double> & row : data)
	{
		for (int j{}; j < row.size(); j++) {
			c_arr[i * COL_SIZE + j] = row.at(j);
			cout << c_arr[i * COL_SIZE + j] << "\t";
		}
		i = i + 1;
		cout << endl;
	}

	PyArrayObject *np_ret, *np_arg;
	np_arg = reinterpret_cast<PyArrayObject*>(
		PyArray_SimpleNewFromData(ND, dims, NPY_DOUBLE, reinterpret_cast<void*>(c_arr))
		);

	// Build the 2D array
	PyObject *pArgs, *pReturn, *pModule, *pFunc;

	// Calling array_tutorial from mymodule
	PyObject *pName;
	pName = PyUnicode_DecodeFSDefault(this->scriptName.c_str());
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (!pModule) {
		std::cout << "mymodule can not be imported" << endl;
		Py_DECREF(np_arg);
		delete[] c_arr;

		return 0;
	}
	pFunc = PyObject_GetAttrString(pModule, "predict");
	if (!pFunc || !PyCallable_Check(pFunc)) {
		Py_DECREF(pModule);
		Py_XDECREF(pFunc);
		Py_DECREF(np_arg);
		delete[] c_arr;

		std::cout << "function is null or not callable" << endl;
		return 0;
	}

	this->outputs.append("executing ...\n");

	pArgs = PyTuple_New(2);
	//PyTuple_SetItem(pArgs, 0, this->pyobj);
	PyTuple_SetItem(pArgs, 0, PyBytes_FromString(this->modelStr.c_str()));
	PyTuple_SetItem(pArgs, 1, reinterpret_cast<PyObject*>(np_arg));
	pReturn = PyObject_CallObject(pFunc, pArgs);
	Py_DECREF(pArgs);
	if (pReturn != NULL) {
		this->outputs.append("return value received.\n");

		np_ret = reinterpret_cast<PyArrayObject*>(pReturn);

		//if (PyArray_NDIM(np_ret) != ND) { // row[0] is returned
		//}

		this->outputs.append("parsing ...\n");

		const npy_intp lenx{ PyArray_SHAPE(np_ret)[0] };
		const npy_intp leny{ PyArray_SHAPE(np_ret)[1] };

		double(*c_arr_out) { new double[ROW_SIZE * Y_COL_SIZE] };
		c_arr_out = reinterpret_cast<double*>(PyArray_DATA(np_ret));

		std::cout << "Printing output array" << endl << lenx << "\t" << leny << endl;
		this->outputs.append("received predictions: (" + to_string(lenx) + ", " + to_string(leny) + ")\n");

		std::string op = "";

		int i = 0;
		for (int i{}; i < lenx; i++) {
			for (int j{}; j < leny; j++) {
				double val = c_arr_out[i*leny + j];
				std::cout << val << '\t';
				op.append(std::to_string(val)).append("\t");
				this->getDataSet().getyPredictionPoints().push_back(val);
			}
			op.append("\n");
			std::cout << endl;
		}
		std::cout << endl;
		this->outputs.assign(op);

		//for (int i{}; i < lenx; i++) {
		//	double val = c_arr_out[i*COL_SIZE];
		//	std::cout << val << "\t";
		//	//op.append(std::to_string(val)).append("\t");
		//	this->getDataSet().getyPredictionPoints().push_back(val);
		//}

		//this->outputs.assign(op);

		delete[] c_arr;
		delete[] c_arr_out;
	}
	else {
		PyErr_Print();
		fprintf(stderr, "Call failed\n");	
	}

	Py_XDECREF(pFunc);
	Py_DECREF(pModule);

	return 0;
}


int Kriging::testing() {
	initializePython();
	import_array();
	std::array<long, 2> dataSize = this->ds.getDataSize();
	std::list<std::vector<double>> xData = this->ds.getxDataPoints();
	std::list<std::vector<double>> yData = this->ds.getyDataPoints();

	const size_t ROW_SIZE = xData.size();
	const size_t X_COL_SIZE = dataSize[0];
	const size_t Y_COL_SIZE = dataSize[1];

	const size_t COL_SIZE = X_COL_SIZE + Y_COL_SIZE;

	npy_intp dims[2]{ (npy_intp) ROW_SIZE, (npy_intp) COL_SIZE };
	const int ND{ 2 };

	double(*c_arr) { new double[ROW_SIZE * COL_SIZE] };
	double(*c_arr_out) { new double[ROW_SIZE * COL_SIZE] };

	if (ROW_SIZE == 0) return 0;

	int i = 0;
	for (const std::vector<double> & row : xData)
	{
		for (int j{}; j < row.size(); j++) {
			c_arr[i * COL_SIZE + j] = row.at(j);
		}
		i = i + 1;
	}

	// adding y values;
	i = 0;
	for (const std::vector<double> & row : yData)
	{
		for (int j{}; j < row.size(); j++) {
			c_arr[i * COL_SIZE + X_COL_SIZE + j] = row.at(j);
		}
		i = i + 1;
	}

	//cout << "\n\nconverting dataset for transfer: \n";
	//for (int i{}; i < row_size * col_size; i++) {
	//	cout << c_arr[i] << "\t";
	//}

	PyObject *pArgs, *pReturn, *pModule, *pFunc;
	PyArrayObject *np_ret, *np_arg;
	np_arg = reinterpret_cast<PyArrayObject*>(
		PyArray_SimpleNewFromData(ND, dims, NPY_DOUBLE, reinterpret_cast<void*>(c_arr))
		);

	// Calling array_tutorial from mymodule
	PyObject *pName = PyUnicode_FromString("run");
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (!pModule) {
		std::cout << "mymodule can not be imported" << endl;
		Py_DECREF(np_arg);
		//delete[] c_arr;
		return 1;
	}
	pFunc = PyObject_GetAttrString(pModule, "array_test");
	if (!pFunc || !PyCallable_Check(pFunc)) {
		Py_DECREF(pModule);
		Py_XDECREF(pFunc);
		Py_DECREF(np_arg);
		//delete[] c_arr;
		std::cout << "function is null or not callable" << endl;
		return 1;
	}
	pArgs = PyTuple_New(1);
	PyTuple_SetItem(pArgs, 0, reinterpret_cast<PyObject*>(np_arg));
	pReturn = PyObject_CallObject(pFunc, pArgs);
	np_ret = reinterpret_cast<PyArrayObject*>(pReturn);
	if (PyArray_NDIM(np_ret) != ND) { // row[0] is returned
		std::cout << "Function returned with wrong dimension" << endl;
		Py_DECREF(pFunc);
		Py_DECREF(pModule);
		Py_DECREF(np_arg);
		Py_DECREF(np_ret);
		//delete[] c_arr;
		return 1;
	}
	npy_intp len{ PyArray_SHAPE(np_ret)[0] };
	const npy_intp leny{ PyArray_SHAPE(np_ret)[1] };
	c_arr_out = reinterpret_cast<double(*)>(PyArray_DATA(np_ret));
	std::cout << "Printing output array" << endl << len << "\t" << leny << endl;

	//for (int i{}; i < len * leny; i++) {
	//	std::cout << c_arr_out[i] << '\t';
	//}
	//std::cout << endl << endl;

	for (int i{}; i < len; i++) {
		for (int j{}; j < leny; j++) {
			std::cout << c_arr_out[i*leny + j] << '\t';
		}
		std::cout << endl;
	}
	std::cout << endl;

	// Finalizing
	Py_DECREF(pFunc);
	Py_DECREF(pModule);
	Py_DECREF(np_arg);
	Py_DECREF(np_ret);
	delete[] c_arr;
	delete[] c_arr_out;

	return 0;
}

int Kriging::dsTest() {
	import_array();
	std::array<long, 2> dataSize = this->ds.getDataSize();
	std::list<std::vector<double>> xData = this->ds.getxDataPoints();
	std::list<std::vector<double>> yData = this->ds.getyDataPoints();

	const size_t ROW_SIZE = xData.size();
	const size_t X_COL_SIZE = dataSize[0];
	const size_t Y_COL_SIZE = dataSize[1];

	const size_t COL_SIZE = X_COL_SIZE + Y_COL_SIZE;

	npy_intp dims[2]{ (npy_intp) ROW_SIZE, (npy_intp) COL_SIZE };
	const int ND{ 2 };

	double(*c_arr) { new double[ROW_SIZE * COL_SIZE] };
	double(*c_arr_out) { new double[ROW_SIZE * COL_SIZE] };

	if (ROW_SIZE == 0) return 0;

	int i = 0;
	for (const std::vector<double> & row : xData)
	{
		for (int j{}; j < row.size(); j++) {
			c_arr[i * COL_SIZE + j] = row.at(j);
		}
		i = i + 1;
	}

	// adding y values;
	i = 0;
	for (const std::vector<double> & row : yData)
	{
		for (int j{}; j < row.size(); j++) {
			c_arr[i * COL_SIZE + X_COL_SIZE + j] = row.at(j);
		}
		i = i + 1;
	}

	/*cout << "\n\nconverting dataset for transfer: \n";
	for (int i{}; i < ROW_SIZE * COL_SIZE; i++) {
		cout << c_arr[i] << "\t";
	}*/

	PyObject *pArgs, *pReturn, *pModule, *pFunc;
	PyArrayObject *np_ret, *np_arg;
	np_arg = reinterpret_cast<PyArrayObject*>(
		PyArray_SimpleNewFromData(ND, dims, NPY_DOUBLE, reinterpret_cast<void*>(c_arr))
		);

	// Calling array_tutorial from mymodule
	PyObject *pName = PyUnicode_FromString("run");
	pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if (!pModule) {
		std::cout << "mymodule can not be imported" << endl;
		Py_DECREF(np_arg);
		delete[] c_arr;
		return 1;
	}
	pFunc = PyObject_GetAttrString(pModule, "array_test");
	if (!pFunc || !PyCallable_Check(pFunc)) {
		Py_DECREF(pModule);
		Py_XDECREF(pFunc);
		Py_DECREF(np_arg);
		delete[] c_arr;
		std::cout << "function is null or not callable" << endl;
		return 1;
	}
	pArgs = PyTuple_New(1);
	PyTuple_SetItem(pArgs, 0, reinterpret_cast<PyObject*>(np_arg));
	pReturn = PyObject_CallObject(pFunc, pArgs);
	np_ret = reinterpret_cast<PyArrayObject*>(pReturn);
	if (PyArray_NDIM(np_ret) != ND) { // row[0] is returned
		std::cout << "Function returned with wrong dimension" << endl;
		Py_DECREF(pFunc);
		Py_DECREF(pModule);
		Py_DECREF(np_arg);
		Py_DECREF(np_ret);
		delete[] c_arr;
		return 1;
	}
	npy_intp len{ PyArray_SHAPE(np_ret)[0] };
	const npy_intp leny{ PyArray_SHAPE(np_ret)[1] };

	c_arr_out = reinterpret_cast<double(*)>(PyArray_DATA(np_ret));

	std::cout << "Printing output array" << endl << len << "\t" << leny << endl;

	//for (int i{}; i < len * leny; i++) {
	//	std::cout << c_arr_out[i] << '\t';
	//}
	//std::cout << endl << endl;

	for (int i{}; i < len; i++) {
		for (int j{}; j < leny; j++) {
			std::cout << c_arr_out[i*leny + j] << '\t';
		}
		std::cout << endl;
	}

	std::cout << endl;

	// Finalizing
	Py_DECREF(pFunc);
	Py_DECREF(pModule);
	Py_DECREF(np_arg);
	Py_DECREF(np_ret);

	/*delete[] c_arr;
	delete[] c_arr_out;*/

	return 0;
}