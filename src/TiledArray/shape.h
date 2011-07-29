#ifndef TILEDARRAY_SHAPE_H__INCLUDED
#define TILEDARRAY_SHAPE_H__INCLUDED

#include <TiledArray/error.h>
#include <TiledArray/coordinate_system.h>
#include <TiledArray/range.h>
#include <TiledArray/versioned_pmap.h>
#include <TiledArray/contraction_tensor.h>
#include <TiledArray/permutation.h>
#include <TiledArray/tile.h>
#include <world/sharedptr.h>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/iterator_traits.hpp>
#include <typeinfo>

namespace TiledArray {

  // Forward declarations
  template <typename>
  class DenseShape;
  template <typename>
  class SparseShape;
  template <typename>
  class Shape;

  /// Shape of array tiles

  /// Stores information about the presence or absence of tiles in an array,
  /// and handles interprocess communication when the information is not locally
  /// available.
  /// \tparam CS The \c Shape coordinate system type
  /// \note This is an interface class only and cannot be constructed directly.
  /// Insted use DenseShape or SparseShape.
  template <typename CS>
  class Shape {
  private:
    typedef Shape<CS> Shape_;  ///< This type

    // assignment not allowed
    Shape();
    Shape_& operator=(const Shape_&);

  public:
    typedef CS coordinate_system;
    typedef typename coordinate_system::index index;
    typedef typename coordinate_system::ordinal_index ordinal_index;
    typedef Range<coordinate_system> range_type;  ///< Range object type
    typedef detail::VersionedPmap<ordinal_index> pmap_type; ///< The process map interface type
    typedef expressions::Tile<int, CS> array_type;

  protected:
    /// Shape constructor

    /// Shape base class constructor
    /// \param r The range for the shape
    /// \param m The process map for the shape
    Shape(const range_type& r, const pmap_type& m) : range_(r), pmap_(m) { }

    /// Copy constructor

    /// \param other The shape to be copied
    Shape(const Shape_& other) : range_(other.range_), pmap_(other.pmap_) { }

  public:

    /// Virtual destructor
    virtual ~Shape() { }

    /// Create a copy of this object

    /// \return A shared pointer to a copy of this object.
    virtual std::shared_ptr<Shape_> clone() const = 0;

    /// Type info accessor for derived class
    virtual const std::type_info& type() const = 0;

    /// Is shape data for key \c k stored locally.

    /// \param i The key to check
    /// \return \c true when shape data for the given tile is stored on this node,
    /// otherwise \c false.
    template <typename Index>
    bool is_local(const Index& i) const {
      TA_ASSERT(range_.includes(i));
      return this->local_data(range_.ord(i));
    }

    /// Probe for the presence of an element at key
    template <typename Index>
    bool probe(const Index& i) const {
      TA_ASSERT(this->is_local(i));
      return  this->local_probe(range_.ord(i));
    }

    /// Shape range accessor

    /// \return A reference to the range object associated with the shape.
    const range_type& range() const { return range_; }

    /// Construct a shape map

    /// \return A dense array that contains 1 where tiles exist in the shape and
    /// 0 where tiles do not exist in the shape.
    virtual array_type make_shape_map() const = 0;

  protected:

    typename range_type::volume_type volume() { return range_.volume(); }

    template <typename Index>
    ProcessID owner(const Index& i) const {
      return pmap_.owner(ord(i));
    }

  private:

    virtual bool local_data(const ordinal_index&) const { return true; }
    virtual bool local_probe(const ordinal_index&) const { return true; }

    const range_type& range_; ///< The range object associated with this shape.
    const pmap_type& pmap_;   ///< The process map for the shape.
  };

  /// Runtime type checking for dense shape

  /// \tparam T Coordinate system type of shape
  /// \param s The shape to check
  /// \return If shape is a \c DenseShape class return \c true , otherwise \c false .
  template <typename T>
  inline bool is_dense_shape(const T&) {
    return false;
  }

  /// Runtime type checking for dense shape

  /// \tparam CS Coordinate system type of shape
  /// \param s The shape to check
  /// \return If shape is a \c DenseShape class return \c true , otherwise \c false .
  template <typename CS>
  inline bool is_dense_shape(const Shape<CS>& s) {
    return s.type() == typeid(DenseShape<CS>);
  }

  /// Runtime type checking for dense shape

  /// \tparam CS Coordinate system type of shape
  /// \param s The shape to check
  /// \return If shape is a \c DenseShape class return \c true , otherwise \c false .
  template <typename CS>
  inline bool is_dense_shape(const DenseShape<CS>&) {
    return true;
  }

  /// Runtime type checking for sparse shape

  /// \tparam CS Coordinate system type of shape
  /// \param s The shape to check
  /// \return If shape is a \c SparseShape class return \c true , otherwise \c false .
  template <typename T>
  inline bool is_sparse_shape(const T&) {
    return false;
  }

  /// Runtime type checking for sparse shape

  /// \tparam CS Coordinate system type of shape
  /// \param s The shape to check
  /// \return If shape is a \c SparseShape class return \c true , otherwise \c false .
  template <typename CS>
  inline bool is_sparse_shape(const Shape<CS>& s) {
    return s.type() == typeid(SparseShape<CS>);
  }

  /// Runtime type checking for sparse shape

  /// \tparam CS Coordinate system type of shape
  /// \param s The shape to check
  /// \return If shape is a \c SparseShape class return \c true , otherwise \c false .
  template <typename CS>
  inline bool is_sparse_shape(const SparseShape<CS>&) {
    return true;
  }

  /// Create a copy of the shape

  /// The new shape has the same tiles as the original but may refer to different
  /// process maps and range object.
  /// \tparam CS The shape coordiante system type
  /// \param world The world where the shape exists
  /// \param range The range of the result shape
  /// \param pmap The range process map
  /// \param other The shape to be copied
  template <typename CS>
  Shape<CS>* shape_copy(madness::World& world, const typename Shape<CS>::range_type& range,
      const typename Shape<CS>::pmap_type& pmap, const Shape<CS>& other)
  {
    TA_ASSERT(other.range() == range);

    Shape<CS>* result = NULL;

    if(is_dense_shape(other))
      result = new DenseShape<CS>(range, pmap);
    else
      result = new SparseShape<CS>(world, range, pmap,
          static_cast<const SparseShape<CS>&>(other));

    return result;
  }

  /// Create a copy of the shape

  /// The new shape has the same tiles as the original but may refer to different
  /// process maps and range object.
  /// \tparam CS The shape coordiante system type
  /// \param world The world where the shape exists
  /// \param range The range of the result shape
  /// \param pmap The range process map
  /// \param other The shape to be copied
  template <typename CS>
  Shape<CS>* shape_permute(madness::World& world, const typename Shape<CS>::range_type& range,
      const typename Shape<CS>::pmap_type& pmap, const Permutation<CS::dim>& p, const Shape<CS>& other)
  {
    TA_ASSERT(other.range() == range);

    Shape<CS>* result = NULL;

    if(is_dense_shape(other))
      result = new DenseShape<CS>(range, pmap);
    else
      result = new SparseShape<CS>(world, range, pmap, p ^ other.make_shape_map());

    return result;
  }

  template <typename CS>
  Shape<CS>* shape_union(madness::World& world, const typename Shape<CS>::range_type& range,
      const typename Shape<CS>::pmap_type& pmap, const Shape<CS>& left,
      const Shape<CS>& right)
  {
    TA_ASSERT(range == left.range());
    TA_ASSERT(range == right.range());

    Shape<CS>* result = NULL;

    if(is_dense_shape(left) || is_dense_shape(right))
      result = new DenseShape<CS>(range, pmap);
    else
      result = new SparseShape<CS>(world, range, pmap,
          static_cast<const SparseShape<CS>&>(left),
          static_cast<const SparseShape<CS>&>(right));

    return result;
  }

  template <typename ResCS, typename I, typename LeftCS, typename RightCS>
  Shape<ResCS>* shape_contract(madness::World& world, const typename Shape<ResCS>::range_type& range,
      const typename Shape<ResCS>::pmap_type& pmap, const std::shared_ptr<math::Contraction<I> >& cont,
      const Shape<LeftCS>& left, const Shape<RightCS>& right)
  {
    Shape<ResCS>* result = NULL;

    if(is_dense_shape(left) && is_dense_shape(right))
      result = new DenseShape<ResCS>(range, pmap);
    else {
      expressions::ContractionTensor<typename Shape<LeftCS>::array_type,
        typename Shape<RightCS>::array_type> contracted(left.make_shape_map(),
        right.make_shape_map(), cont);

      result = new SparseShape<ResCS>(world, range, pmap, contracted);
    }

    return result;
  }

} // namespace TiledArray

#endif // TILEDARRAY_SHAPE_H__INCLUDED
