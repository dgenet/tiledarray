#include <tiledarray/tensor/tensor.h>
#include "irregular_tiled_matrix_wrapper.h"

void *tilearray_future_get_tile(void *f)
{
    void *res;
    typedef TiledArray::Tensor<double, Eigen::aligned_allocator<double> > static_tile_type;
    typedef madness::Future<static_tile_type> static_future_type;
    std::cout << "tilearray_future_get_tile: static casts done" << std::endl;
    static_future_type *F = reinterpret_cast <static_future_type*>(f);
    std::cout << "tilearray_future_get_tile: reintrepret cast done" << std::endl;
    static_tile_type T = F->get();
    std::cout << "tilearray_future_get_tile: get done" << std::endl;
    res = T.data();
    std::cout << "tilearray_future_get_tile: data() resolved" << std::endl;
    return res;
}
