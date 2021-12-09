#pragma once

#include <array>
#include <numeric>
#include <functional>
#include <cmath>
#include <string>
#include <iostream>

namespace util {

    /**
     * Applies a binary function to elements of two objects piece by piece. The objects must
     * be iterable and should have the same size. If they have not the same size the function is applied as long
     * as the smaller one still have elements.
     * @tparam Container - a iterable object like an array or vector
     * @tparam BinOp - a binary function to apply
     * @param lhs - the first container
     * @param rhs - the second container
     * @param binOp - the binary function
     * @return a container containing the result
     */
    template<typename Container, typename BinOp>
    Container applyBinaryFunction(const Container &lhs, const Container &rhs, BinOp binOp) {
        Container ret = lhs;

        auto lhsIt = std::begin(lhs);
        auto lhsEnd = std::end(lhs);
        auto rhsIt = std::begin(rhs);
        auto rhsEnd = std::end(rhs);

        for (auto retIt = std::begin(ret); lhsIt != lhsEnd && rhsIt != rhsEnd; ++lhsIt, ++rhsIt, ++retIt) {
            *retIt = binOp(*lhsIt, *rhsIt);
        }
        return ret;
    }

    /**
     * Applies a binary function to elements of two objects piece by piece. The objects must
     * be iterable and should have the same size. If they have not the same size the function is applied as long
     * as the smaller one still have elements.
     * @tparam Container - a iterable object like an array or vector
     * @tparam Scalar - a scalar to use on each element
     * @tparam BinOp - a binary function to apply
     * @param lhs - the first container
     * @param scalar - a scalar to use on each element
     * @param binOp - the binary function
     * @return a container containing the result
     */
    template<typename Container, typename Scalar,typename BinOp>
    Container applyBinaryFunction(const Container &lhs, const Scalar &scalar, BinOp binOp) {
        Container ret = lhs;

        auto lhsIt = std::begin(lhs);
        auto lhsEnd = std::end(lhs);

        for (auto retIt = std::begin(ret); lhsIt != lhsEnd; ++lhsIt, ++retIt) {
            *retIt = binOp(*lhsIt, scalar);
        }
        return ret;
    }

    /**
     * Applies the Operation Minus to two Container piece by piece.
     * @example {1, 2, 3} - {1, 1, 1} = {0, 1, 2}
     * @tparam Container
     * @param lhs - minuend
     * @param rhs - subtrahend
     * @return the difference
     */
    template<typename Container>
    Container operator-(const Container &lhs, const Container &rhs) {
        return applyBinaryFunction(lhs, rhs, std::minus<>());
    }

    /**
    * Applies the Operation Plus to two Container piece by piece.
    * @example {1, 2, 3} + {1, 1, 1} = {2, 3, 4}
    * @tparam Container
    * @param lhs - addend
    * @param rhs - addend
    * @return the sum
    */
    template<typename Container>
    Container operator+(const Container &lhs, const Container &rhs) {
        return applyBinaryFunction(lhs, rhs, std::plus<>());
    }

    /**
    * Applies the Operation * to two Container piece by piece.
    * @example {1, 2, 3} * {2, 2, 2} = {2, 4, 6}
    * @tparam Container
    * @param lhs - multiplicand
    * @param rhs - multiplicand
    * @return the product
    */
    template<typename Container>
    Container operator*(const Container &lhs, const Container &rhs) {
        return applyBinaryFunction(lhs, rhs, std::multiplies<>());
    }

    /**
    * Applies the Operation / to two Container piece by piece.
    * @example {1, 2, 3} * {1, 2, 3} = {1, 1, 1}
    * @tparam Container
    * @param lhs - multiplicand
    * @param rhs - multiplicand
    * @return the product
    */
    template<typename Container>
    Container operator/(const Container &lhs, const Container &rhs) {
        return applyBinaryFunction(lhs, rhs, std::divides<>());
    }

    /**
    * Applies the Operation + to a Container and a Scalar.
    * @example {1, 2, 3} + 2 = {3, 4, 5}
    * @tparam Container
    * @tparam Scalar
    * @param lhs - addend
    * @param scalar - addend
    * @return a Container
    */
    template<typename Container, typename Scalar>
    Container operator+(const Container &lhs, const Scalar &scalar) {
        return applyBinaryFunction(lhs, scalar, std::plus<>());
    }

    /**
    * Applies the Operation - to a Container and a Scalar.
    * @example {1, 2, 3} - 2 = {-1, 0, 1}
    * @tparam Container
    * @tparam Scalar
    * @param lhs - minuend
    * @param scalar - subtrahend
    * @return a Container
    */
    template<typename Container, typename Scalar>
    Container operator-(const Container &lhs, const Scalar &scalar) {
        return applyBinaryFunction(lhs, scalar, std::minus<>());
    }

    /**
    * Applies the Operation - to a Container and a Scalar.
    * @example {1, 2, 3} * 2 = {2, 4, 6}
    * @tparam Container
    * @tparam Scalar
    * @param lhs - multiplicand
    * @param scalar - multiplicand
    * @return a Container
    */
    template<typename Container, typename Scalar>
    Container operator*(const Container &lhs, const Scalar &scalar) {
        return applyBinaryFunction(lhs, scalar, std::multiplies<>());
    }

    /**
     * Applies the Operation / to a Container and a Scalar.
     * @example {2, 4, 6} / 2 = {1, 2, 3}
     * @tparam Container
     * @tparam Scalar
     * @param lhs - the dividend
     * @param scalar - the divisor
     * @return a Container
     */
    template<typename Container, typename Scalar>
    Container operator/(const Container &lhs, const Scalar &scalar) {
        return applyBinaryFunction(lhs, scalar, std::divides<>());
    }

    /**
     * Applies the euclidean norm/ L2-norm to a Container (e.g. a vector)
     * @tparam Container - must be iterable
     * @param container - e.g. a vector
     * @return an double containing the L2 norm
     */
    template<typename Container>
    double euclideanNorm(const Container &container) {
        return std::sqrt(std::inner_product(std::begin(container), std::end(container), std::begin(container), 0.0));
    }

    /**
    * Returns the cross product of two cartesian vectors.
    * @tparam T - a number
    * @param lhs - left vector
    * @param rhs - right vector
    * @return cross product
    */
    template<typename T>
    std::array<T, 3> cross(const std::array<T, 3> &lhs, const std::array<T, 3> &rhs) {
        std::array<T, 3> result{};
        result[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
        result[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
        result[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
        return result;
    }

    /**
    * Returns the dot product of two cartesian vectors.
    * @tparam T - a number
    * @param lhs - left vector
    * @param rhs - right vector
    * @return dot product
    */
    template<typename T>
    T dot(const std::array<T, 3> &lhs, const std::array<T, 3> &rhs) {
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
    }

    /**
     * Operator << for an array of any size.
     * @tparam T - type of the array, must have an << operator overload
     * @tparam N - size of the array
     * @param os - the ostream
     * @param array - the array itself
     * @return ostream
     */
    template<typename T, size_t N>
    std::ostream &operator<<(std::ostream &os, const std::array<T, N> &array) {
        os << "[";
        auto it = array.begin();
        auto end = array.end() - 1;
        while (it != end) {
            os << *it << " ";
            ++it;
        }
        os << *it << "]";
        return os;
    }

    template <typename T>
    struct is_stdarray : std::false_type {};

    template <typename T, std::size_t N>
    struct is_stdarray<std::array<T, N>> : std::true_type {};

}