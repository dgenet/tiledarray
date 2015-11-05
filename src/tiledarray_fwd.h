/*
 *  This file is a part of TiledArray.
 *  Copyright (C) 2014  Virginia Tech
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TILEDARRAY_FWD_H__INCLUDED
#define TILEDARRAY_FWD_H__INCLUDED

#include <complex>

namespace Eigen { // Eigen Alligned allocator for TA::Tensor
  template<class>
  class aligned_allocator;
} // namespace Eigen

namespace TiledArray {

  //TiledArray Policy
  class DensePolicy;
  class SparsePolicy;

  // TiledArray Tensors
  template<typename, typename>
  class Tensor;

  typedef Tensor<double, Eigen::aligned_allocator<double> > TensorD;
  typedef Tensor<int, Eigen::aligned_allocator<int> > TensorI;
  typedef Tensor<float, Eigen::aligned_allocator<float> > TensorF;
  typedef Tensor<long, Eigen::aligned_allocator<long> > TensorL;
  typedef Tensor<std::complex<double>, Eigen::aligned_allocator<std::complex<double> > > TensorZ;
  typedef Tensor<std::complex<float>, Eigen::aligned_allocator<std::complex<float> > > TensorC;

  // TiledArray Arrays
  template <typename, typename> class DistArray;

  // Dense Array Typedefs
  typedef DistArray<TensorD, DensePolicy> TArrayD;
  typedef DistArray<TensorI, DensePolicy> TArrayI;
  typedef DistArray<TensorF, DensePolicy> TArrayF;
  typedef DistArray<TensorL, DensePolicy> TArrayL;
  typedef DistArray<TensorZ, DensePolicy> TArrayZ;
  typedef DistArray<TensorC, DensePolicy> TArrayC;

  // Sparse Array Typedefs
  typedef DistArray<TensorD, SparsePolicy> TSpArrayD;
  typedef DistArray<TensorI, SparsePolicy> TSpArrayI;
  typedef DistArray<TensorF, SparsePolicy> TSpArrayF;
  typedef DistArray<TensorL, SparsePolicy> TSpArrayL;
  typedef DistArray<TensorZ, SparsePolicy> TSpArrayZ;
  typedef DistArray<TensorC, SparsePolicy> TSpArrayC;

  template <typename T, unsigned int, typename Tile = Tensor<T, Eigen::aligned_allocator<T> >, typename Policy = DensePolicy>
  using Array = DistArray<Tile, Policy>;

} // namespace TiledArray

#endif // TILEDARRAY_FWD_H__INCLUDED
