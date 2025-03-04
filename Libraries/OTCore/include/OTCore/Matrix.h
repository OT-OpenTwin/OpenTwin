//! @file Matrix.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <stdint.h>

namespace ot {

    //! @brief A structure representing the index of a matrix element.
    //! This structure holds the x and y coordinates that refer to the position of a matrix element.
    struct MatrixIndex {
        using ValueType = uint64_t;

        MatrixIndex() : x(0), y(0) {};
        MatrixIndex(ValueType _x, ValueType _y) : x(_x), y(_y) {};

        ValueType x; //! @brief The x-coordinate of the matrix element.
        ValueType y; //! @brief The y-coordinate of the matrix element.
    };

    //! @brief A template class for a matrix.
    //! This class defines the basic functionality for a matrix, including methods to set and get data at specific positions in the matrix.
    //! The class supports both lvalue and rvalue references for setting data.
    //! @tparam T The data type to be stored in the matrix.
    template <typename T> class Matrix {
    public:
        //! @brief Virtual destructor to allow proper cleanup in derived classes.
        virtual ~Matrix() {};

        //! @brief Set data in the matrix at a specific location.
        //! The data is passed as an lvalue reference.
        //! @param _index Data index.
        //! @param _data Data to copy.
        virtual void setData(const MatrixIndex& _index, const T& _data) = 0;

        //! @brief Set data in the matrix at a specific location.
        //! The data is passed as an rvalue reference.
        //! @param _index Data index.
        //! @param _data Data to move.
        virtual void setData(const MatrixIndex& _index, T&& _data) = 0;

        //! @brief Retrieve a writeable data reference from the matrix.
        //! @param _index Data index.
        //! @return Writeable data reference.
        virtual T& getData(const MatrixIndex& _index) = 0;

        //! @brief Retrieve a const data reference from the matrix.
        //! @param _index Data index.
        //! @return Const data reference.
        virtual const T& getData(const MatrixIndex& _index) const = 0;
    };

} // namespace ot