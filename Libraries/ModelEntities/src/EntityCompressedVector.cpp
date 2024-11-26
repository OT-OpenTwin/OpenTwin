
#include "EntityCompressedVector.h"
#include "EntityBinaryData.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityCompressedVector::EntityCompressedVector(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner),
	uncompressedLength(0),
	tolerance(0.0),
	maximumDataSize(5e6),
	valuesSize(0),
	countSize(0)
{
}

EntityCompressedVector::~EntityCompressedVector()
{
	compressedDataValues.clear();
	compressedDataCount.clear();
}

bool EntityCompressedVector::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityCompressedVector::StoreToDataBase(void)
{
	EntityBase::StoreToDataBase();
}

void EntityCompressedVector::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now add the actual compressed vector data

	auto arrayValues = bsoncxx::builder::basic::array();
	auto arrayCount  = bsoncxx::builder::basic::array();

	for (double v : compressedDataValues)
	{
		arrayValues.append(v);
	}

	for (double c : compressedDataCount)
	{
		arrayCount.append(c);
	}

	storage.append(
		bsoncxx::builder::basic::kvp("tolerance", tolerance),
		bsoncxx::builder::basic::kvp("uncompressedLength", uncompressedLength),
		bsoncxx::builder::basic::kvp("dataValuesSize", valuesSize),
		bsoncxx::builder::basic::kvp("dataCountSize", countSize),
		bsoncxx::builder::basic::kvp("dataValues", arrayValues),
		bsoncxx::builder::basic::kvp("dataCount", arrayCount)
	);
}

void EntityCompressedVector::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the compressed data

	tolerance          = doc_view["tolerance"].get_double();
	uncompressedLength = doc_view["uncompressedLength"].get_int64();

	valuesSize          = 0;
	countSize           = 0;

	try 
	{
		valuesSize          = doc_view["dataValuesSize"].get_int64();
		countSize           = doc_view["dataCountSize"].get_int64();
	}
	catch (std::exception)
	{
	}

	compressedDataValues.clear();
	compressedDataCount.clear();

	// Now load the data from the storage

	auto arrayValues = doc_view["dataValues"].get_array().value;
	size_t nArrayValues = std::distance(arrayValues.begin(), arrayValues.end());

	compressedDataValues.reserve(nArrayValues);

	auto iValues = arrayValues.begin();

	for (unsigned long index = 0; index < nArrayValues; index++)
	{
		compressedDataValues.push_back(iValues->get_double());
		iValues++;
	}

	auto arrayCount = doc_view["dataCount"].get_array().value;
	size_t nArrayCount = std::distance(arrayCount.begin(), arrayCount.end());

	compressedDataCount.reserve(nArrayCount);

	auto iCount = arrayCount.begin();

	for (unsigned long index = 0; index < nArrayCount; index++)
	{
		compressedDataCount.push_back(iCount->get_double());
		iCount++;
	}

	resetModified();
}

void EntityCompressedVector::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityCompressedVector::clearData(void)
{
	uncompressedLength = 0;
	compressedDataValues.clear();
	compressedDataCount.clear();

	valuesSize = 0;
	countSize  = 0;
}

void EntityCompressedVector::setTolerance(double tol)
{
	tolerance = tol;
}

double EntityCompressedVector::getTolerance(void)
{
	return tolerance;
}

void EntityCompressedVector::getValues(double *values, size_t length)
{
	if (length != uncompressedLength)
	{
		assert(0);
		return;
	}

	size_t dataIndex = 0;
	size_t valueIndex = 0;

	for (long long count : compressedDataCount)
	{
		if (count > 0)
		{
			// We have a series of raw data values written to the value vector

			for (long long index = 0; index < count; index++)
			{
				assert(valueIndex < uncompressedLength);

				values[valueIndex] = compressedDataValues[dataIndex];
				valueIndex++;
				dataIndex++;
			}
		}
		else
		{
			// We have a number of same entries in the value vector

			double value = compressedDataValues[dataIndex];
			dataIndex++;

			for (long long index = 0; index < abs(count); index++)
			{
				assert(valueIndex < uncompressedLength);

				values[valueIndex] = value;
				valueIndex++;
			}
		}
	}

	assert(valueIndex == uncompressedLength);
}

void EntityCompressedVector::setValues(const double *values, size_t length)
{
	// We iterate though the values array. If entries are the same (within the given tolerance), we count the number of identical entries and store
	// them as a negative value in the count array. The value itself is then stored only once. 
	// If the entries are different, we count how many different entries we have. The entries are then stored in the values array and the number of
	// different entries is stored in the count array as a positive value.

	clearData();

	uncompressedLength = length;

	// Determine the required storage amount in the vectors
	long long compressedDataValuesSize = 0, compressedDataCountSize = 0;
	compressData(values, length, false, compressedDataValuesSize, compressedDataCountSize);

	// Allocate the memory
	compressedDataValues.reserve(compressedDataValuesSize);
	compressedDataCount.reserve(compressedDataCountSize);

	// Now store the actual data
	compressedDataValuesSize = 0;
	compressedDataCountSize = 0;
	compressData(values, length, true, compressedDataValuesSize, compressedDataCountSize);

	valuesSize = compressedDataValues.size();
	countSize  = compressedDataCount.size();
}

void EntityCompressedVector::setValues(const std::vector<double> &values)
{
	setValues(values.data(), values.size());
}

void EntityCompressedVector::getValues(std::vector<double> &values)
{
	values.clear();
	values.resize(uncompressedLength);

	getValues(values.data(), values.size());
}

void EntityCompressedVector::compressData(const double *values, size_t length, bool storeData, long long &compressedDataValuesSize, long long &compressedDataCountSize)
{
	long long lastDataCount = 0;

	for (size_t index = 0; index < length; index++)
	{
		if (index == length - 1)
		{
			// This is the very last entry -> it needs to be stored

			if (lastDataCount > 0)
			{
				lastDataCount++;

				compressedDataValuesSize++;

				if (storeData)
				{
					compressedDataValues.push_back(values[index]);
					compressedDataCount[compressedDataCountSize - 1] = lastDataCount;
				}
			}
			else 
			{
				compressedDataValuesSize++;
				compressedDataCountSize++;

				lastDataCount = 1;

				if (storeData)
				{
					compressedDataValues.push_back(values[index]);
					compressedDataCount.push_back(1);
				}
			}
		}
		else
		{
			size_t same = 1;
			do
			{
				if (fabs(values[index] - values[index + same]) > tolerance) break;
				same++;

			} while (same + index < length);

			// We now have "same" number of identical entries

			if (same > 1)
			{
				compressedDataValuesSize++;
				compressedDataCountSize++;

				lastDataCount = -same;

				if (storeData)
				{
					compressedDataValues.push_back(values[index]);
					compressedDataCount.push_back(-same);
				}

				index += same-1;
			}
			else
			{
				// The next entry is different from the current one. We need to store the new value and potentially increase the last value in the data count (if not negative)

				if (lastDataCount > 0)
				{
					lastDataCount++;

					compressedDataValuesSize++;

					if (storeData)
					{
						compressedDataValues.push_back(values[index]);
						compressedDataCount[compressedDataCountSize - 1] = lastDataCount;
					}
				}
				else 
				{
					compressedDataValuesSize++;
					compressedDataCountSize++;

					lastDataCount = 1;

					if (storeData)
					{
						compressedDataValues.push_back(values[index]);
						compressedDataCount.push_back(1);
					}
				}
			}
		}
	}
}

void EntityCompressedVector::setConstantValue(double value, long long dimension)
{
	assert(dimension > 0);

	clearData();

	uncompressedLength = dimension;

	compressedDataValues.reserve(1);
	compressedDataCount.reserve(1);

	compressedDataValues.push_back(value);
	compressedDataCount.push_back(-dimension);
}

bool EntityCompressedVector::multiply(std::vector<double> &values)
{
	// This method multiplies the current compressed vector component-wise to the given uncompressed vector

	if (uncompressedLength != values.size())
	{
		assert(0); // dimension does not match
		return false;
	}

	size_t dataIndex = 0;
	size_t valueIndex = 0;

	for (long long count : compressedDataCount)
	{
		if (count > 0)
		{
			// We have a series of raw data values written to the value vector

			for (long long index = 0; index < count; index++)
			{
				assert(valueIndex < uncompressedLength);

				values[valueIndex] *= compressedDataValues[dataIndex];
				valueIndex++;
				dataIndex++;
			}
		}
		else
		{
			// We have a number of same entries in the value vector

			double value = compressedDataValues[dataIndex];
			dataIndex++;

			for (long long index = 0; index < abs(count); index++)
			{
				assert(valueIndex < uncompressedLength);

				values[valueIndex] *= value;
				valueIndex++;
			}
		}
	}

	return true;
}

bool EntityCompressedVector::add(std::vector<double> &values)
{
	// This method adds the current compressed vector component-wise to the given uncompressed vector

	if (uncompressedLength != values.size())
	{
		assert(0); // dimension does not match
		return false;
	}

	size_t dataIndex = 0;
	size_t valueIndex = 0;

	for (long long count : compressedDataCount)
	{
		if (count > 0)
		{
			// We have a series of raw data values written to the value vector

			for (long long index = 0; index < count; index++)
			{
				assert(valueIndex < uncompressedLength);

				values[valueIndex] += compressedDataValues[dataIndex];
				valueIndex++;
				dataIndex++;
			}
		}
		else
		{
			// We have a number of same entries in the value vector

			double value = compressedDataValues[dataIndex];
			dataIndex++;

			for (long long index = 0; index < abs(count); index++)
			{
				assert(valueIndex < uncompressedLength);

				values[valueIndex] += value;
				valueIndex++;
			}
		}
	}

	return true;
}

