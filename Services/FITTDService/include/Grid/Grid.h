#pragma once
#include <memory>

#include "Grid/GridIteratorVolume.h"
#include "Grid/GridIteratorComponent.h"
#include "Grid/GridIteratorComponentSubspace.h"
#include "SystemDependencies/SystemDependentDefines.h"
#include "SystemDependencies/Allignments.h"
#include "Grid/DimensionPadding.h"
#include "Grid/IntrinsicMasks.h"


template<class T>
class Grid
{
public:
	Grid(index_t dofX, index_t dofY, index_t dofZ, Alignment _alignment)
		:degreesOfFreedomX(dofX), degreesOfFreedomY(dofY), degreesOfFreedomZ(dofZ),
		degreesOfFreedomTotal(dofX*dofY*dofZ), dimensionPedding(_alignment, dofX)
	{
		auto iterX = new GridIteratorComponent(0, degreesOfFreedomX - 1, 1);
		auto iterY = new GridIteratorComponent(0, degreesOfFreedomY - 1, dimensionPedding.GetPaddedDegreesOfFreedom());
		auto iterZ = new GridIteratorComponent(0, degreesOfFreedomZ - 1, dimensionPedding.GetPaddedDegreesOfFreedom() * degreesOfFreedomY);

		_volumeIterator.reset(new GridIteratorVolume(iterX, iterY, iterZ, degreesOfFreedomTotal));
	};
	Grid(Grid & other) = delete;
	Grid & operator=(const Grid & other) = delete;
	~Grid()
	{
	};

	index_t GetDegreesOfFreedomNumberInX() const { return degreesOfFreedomX; };
	index_t GetDegreesOfFreedomNumberInY() const { return degreesOfFreedomY; };
	index_t GetDegreesOfFreedomNumberInZ() const { return degreesOfFreedomZ; };
	index_t GetDegreesOfFreedomNumberTotal() const { return degreesOfFreedomTotal; };
	index_t GetDegreesOfFreedomNumberTotalPadded() const { return  dimensionPedding.GetPaddedDegreesOfFreedom() * degreesOfFreedomY * degreesOfFreedomZ; };

	index_t GetPlusOneXStep() const { return 1; };
	index_t GetPlusOneYStep() const { return dimensionPedding.GetPaddedDegreesOfFreedom(); };
	index_t GetPlusOneZStep() const { return dimensionPedding.GetPaddedDegreesOfFreedom() * degreesOfFreedomY; };

	GridIteratorVolume * GetGridVolumeIterator() { return _volumeIterator.get(); };

	const Alignment GetAlignment() const { return dimensionPedding.GetAlignment(); };
	const int GetLengthOfIntrinsicVector() const { return dimensionPedding.GetLengthOfIntrinsicVector(); };
	const index_t GetNumberOfIntrinsicVectors() const { return dimensionPedding.GetNumberOfIntrinsicVectors(); };
	const index_t GetPaddedDimension() const { return dimensionPedding.GetPaddedDegreesOfFreedom(); };
	const IntrinsicMasks * GetIntrinsicMasks() const { return dimensionPedding.GetIntrinsicMasks(); };

	inline index_t GetIndexToCoordinate(index_t x, index_t y, index_t z) const;
	inline index_t GetIndexToCoordinateNotPadded(index_t x, index_t y, index_t z) const;

protected:
	index_t degreesOfFreedomX = 0;
	index_t degreesOfFreedomY = 0;
	index_t degreesOfFreedomZ = 0;
	index_t degreesOfFreedomTotal = 0;

	DimensionPadding<T> dimensionPedding;

private:
	std::unique_ptr<GridIteratorVolume> _volumeIterator;

};

template  <class T>
inline index_t Grid<T>::GetIndexToCoordinate(index_t x, index_t y, index_t z) const
{
	return x + dimensionPedding.GetPaddedDegreesOfFreedom() * y + degreesOfFreedomY * dimensionPedding.GetPaddedDegreesOfFreedom() * z;
}

template<class T>
inline index_t Grid<T>::GetIndexToCoordinateNotPadded(index_t x, index_t y, index_t z) const
{
	return x + degreesOfFreedomX * y + degreesOfFreedomY * degreesOfFreedomX * z;
}