#ifndef TILEDARRAY_CONTRACTION_TENSOR_IMPL_H__INCLUDED
#define TILEDARRAY_CONTRACTION_TENSOR_IMPL_H__INCLUDED

#include <TiledArray/tensor_expression_impl.h>
#include <TiledArray/tensor.h>
#include <TiledArray/cyclic_pmap.h>
#include <TiledArray/mxm.h>
#include <world/shared_ptr.h>

namespace TiledArray {
  namespace expressions {

    template <typename Left, typename Right>
    class ContractionTensorImpl : public TensorExpressionImpl<DynamicTiledRange,
        Tensor<typename math::ContractionValue<typename Left::value_type::value_type,
        typename Right::value_type::value_type>::type, typename DynamicTiledRange::range_type> >
    {
    public:
      // Base class typedefs
      typedef TensorExpressionImpl<DynamicTiledRange,
          Tensor<typename math::ContractionValue<typename Left::value_type::value_type,
          typename Right::value_type::value_type>::type, typename DynamicTiledRange::range_type> >
          TensorExpressionImpl_;
      typedef typename TensorExpressionImpl_::TensorImplBase_ TensorImplBase_;
      typedef ContractionTensorImpl<Left, Right> ContractionTensorImpl_;

      typedef Left left_tensor_type; ///< The left tensor type
      typedef typename left_tensor_type::value_type left_value_type; /// The left tensor value type
      typedef Right right_tensor_type; ///< The right tensor type
      typedef typename right_tensor_type::value_type right_value_type; ///< The right tensor value type

      typedef typename TensorImplBase_::size_type size_type; ///< size type
      typedef typename TensorImplBase_::pmap_interface pmap_interface; ///< The process map interface type
      typedef typename TensorImplBase_::trange_type trange_type;
      typedef typename TensorImplBase_::range_type range_type;
      typedef typename TensorImplBase_::value_type value_type; ///< The result value type
      typedef typename TensorImplBase_::storage_type::const_iterator const_iterator; ///< Tensor const iterator
      typedef typename TensorImplBase_::storage_type::future const_reference;

    private:
      left_tensor_type left_; ///< The left argument tensor
      right_tensor_type right_; /// < The right argument tensor

      size_type left_inner_; ///< The number of inner indices in the left tensor argument
      size_type left_outer_; ///< The number of outer indices in the left tensor argument
      size_type right_inner_; ///< The number of inner indices in the right tensor argument
      size_type right_outer_; ///< The number of outer indices in the right tensor argument

    protected:
      const ProcessID rank_; ///< This process's rank
      const ProcessID size_; ///< Then number of processes
      size_type m_; ///< Number of element rows in the result and left matrix
      size_type n_; ///< Number of element columns in the result matrix and rows in the right argument matrix
      size_type k_; ///< Number of element columns in the left and right argument matrices
      size_type mk_; ///< Number of elements in left matrix
      size_type kn_; ///< Number of elements in right matrix
      size_type proc_cols_; ///< Number of columns in the result process map
      size_type proc_rows_; ///< Number of rows in the result process map
      size_type proc_size_; ///< Number of process in the process map. This may be
                         ///< less than the number of processes in world.
      ProcessID rank_row_; ///< This node's row in the process map
      ProcessID rank_col_; ///< This node's column in the process map
      size_type local_rows_; ///< The number of local element rows
      size_type local_cols_; ///< The number of local element columns
      size_type local_size_; ///< Number of local elements

    private:

      template <typename Pred>
      class InnerOuterPred {
      private:
        const expressions::VariableList& vars_;
        Pred pred_;

      public:
        InnerOuterPred(const expressions::VariableList& vars) : vars_(vars), pred_() { }

        bool operator()(const std::string& var) const { return pred_(std::find(vars_.begin(), vars_.end(), var), vars_.end()); }
      }; // class InnerOuterPred

      typedef InnerOuterPred<std::equal_to<expressions::VariableList::const_iterator> > OuterPred;
      typedef InnerOuterPred<std::not_equal_to<expressions::VariableList::const_iterator> > InnerPred;

      static expressions::VariableList contract_vars(const Left& left, const Right& right) {
        std::vector<std::string> vars;

        OuterPred left_pred(right.vars());
        for(expressions::VariableList::const_iterator it = left.vars().begin(); it != left.vars().end(); ++it)
          if(left_pred(*it))
            vars.push_back(*it);

        OuterPred right_pred(left.vars());
        for(expressions::VariableList::const_iterator it = right.vars().begin(); it != right.vars().end(); ++it)
          if(right_pred(*it))
            vars.push_back(*it);

        return expressions::VariableList(vars.begin(), vars.end());
      }

      static DynamicTiledRange contract_trange(const Left& left, const Right& right) {
        typename DynamicTiledRange::Ranges ranges;

        OuterPred left_pred(right.vars());
        for(expressions::VariableList::const_iterator it = left.vars().begin(); it != left.vars().end(); ++it)
          if(left_pred(*it))
            ranges.push_back(left.trange().data()[std::distance(left.vars().begin(), it)]);

        OuterPred right_pred(left.vars());
        for(expressions::VariableList::const_iterator it = right.vars().begin(); it != right.vars().end(); ++it)
          if(right_pred(*it))
            ranges.push_back(right.trange().data()[std::distance(right.vars().begin(), it)]);

        return DynamicTiledRange(ranges.begin(), ranges.end());
      }

    public:

      ContractionTensorImpl(const left_tensor_type& left, const right_tensor_type& right) :
          TensorExpressionImpl_(left.get_world(), contract_vars(left, right), contract_trange(left, right)),
          left_(left), right_(right),
          left_inner_(0ul), left_outer_(0ul), right_inner_(0ul), right_outer_(0ul),
          rank_(TensorImplBase_::get_world().rank()), size_(TensorImplBase_::get_world().size()),
          m_(1ul), n_(1ul), k_(1ul), mk_(1ul), kn_(1ul),
          proc_cols_(0ul), proc_rows_(0ul), proc_size_(0ul),
          rank_row_(-1), rank_col_(-1),
          local_rows_(0ul), local_cols_(0ul), local_size_(0ul)
      {
        InnerPred left_inner_pred(right_.vars());
        for(expressions::VariableList::const_iterator it = left_.vars().begin(); it != left_.vars().end(); ++it)
          if(left_inner_pred(*it)) {
            ++left_inner_;
            k_ *= left_.range().size()[std::distance(left_.vars().begin(), it)];
          }

        // Calculate the dimensions of the inner and outer fused tensors
        mk_ = left_.range().volume();
        kn_ = right_.range().volume();
        m_ = mk_ / k_;
        n_ = kn_ / k_;
        left_outer_ = left_.range().dim() - left_inner_;
        right_inner_ = left_inner_;
        right_outer_ = right_.range().dim() - right_inner_;

        // Caclulate the process map dimensions and size
        proc_cols_ = std::min(size_ / std::max(std::min<std::size_t>(std::sqrt(size_ * m_ / n_), size_), 1ul), n_);
        proc_rows_ = std::min(size_ / proc_cols_, m_);
        proc_size_ = proc_cols_ * proc_rows_;

        if(rank_ < proc_size_) {
          // Calculate this rank's row and column
          rank_row_ = rank_ / proc_cols_;
          rank_col_ = rank_ % proc_cols_;

          // Calculate the local tile dimensions and size
          local_rows_ = (m_ / proc_rows_) + (rank_row_ < (m_ % proc_rows_) ? 1 : 0);
          local_cols_ = (n_ / proc_cols_) + (rank_col_ < (n_ % proc_cols_) ? 1 : 0);
          local_size_ = local_rows_ * local_cols_;
        }
      }

      virtual ~ContractionTensorImpl() { }

      const left_tensor_type& left() const { return left_; }

      left_tensor_type& left() { return left_; }

      const right_tensor_type& right() const { return right_; }

      right_tensor_type& right() { return right_; }

    private:

      template <typename InIter>
      size_type product(InIter first, InIter last) const {
        size_type result = 1ul;

        for(; first != last; ++first)
          result *= *first;

        return result;
      }

    public:

      /// Contraction operation

      /// Contract \c left and \c right to \c result .
      /// \param[out] result The tensor that will store the result
      /// \param[in] left The left hand tensor argument
      /// \param[in] right The right hand tensor argument
      void contract(value_type& result, const left_value_type& left, const right_value_type& right) const {
        // Allocate the result tile if it is uninitialized
        if(result.empty()) {
          // Create the start and finish indices
          typename value_type::range_type::index start(left_outer_ + right_outer_);
          typename value_type::range_type::index finish(left_outer_ + right_outer_);

          // Copy the values from left and right ranges to start and finish indices
          std::copy(right.range().start().begin() + right_inner_, right.range().start().end(),
              std::copy(left.range().start().begin(), left.range().start().begin() + left_outer_, start.begin()));
          std::copy(right.range().finish().begin() + right_inner_, right.range().finish().end(),
              std::copy(left.range().finish().begin(), left.range().finish().begin() + left_outer_, finish.begin()));

          value_type(typename value_type::range_type(start, finish)).swap(result);
        }

        // Check that all tensors have been allocated at this point
        TA_ASSERT(!result.empty());
        TA_ASSERT(!left.empty());
        TA_ASSERT(!right.empty());

        // The assertions below varify that the argument and result tensors are coformal

        // Check that all tensors have the correct dimension sizes
        TA_ASSERT(result.range().dim() == (left_outer_ + right_outer_));
        TA_ASSERT(left.range().dim() == (left_outer_ + left_inner_));
        TA_ASSERT(right.range().dim() == (right_inner_ + right_outer_));

        // Check that the outer dimensions of left match the the corresponding dimesions in result
        TA_ASSERT(std::equal(left.range().start().begin(), left.range().start().begin() + left_outer_, result.range().start().begin()));
        TA_ASSERT(std::equal(left.range().finish().begin(), left.range().finish().begin() + left_outer_, result.range().finish().begin()));
        TA_ASSERT(std::equal(left.range().size().begin(), left.range().size().begin() + left_outer_, result.range().size().begin()));

        // Check that the outer dimensions of right match the the corresponding dimesions in result
        TA_ASSERT(std::equal(right.range().start().begin() + right_inner_, right.range().start().end(), result.range().start().begin() + left_outer_));
        TA_ASSERT(std::equal(right.range().finish().begin() + right_inner_, right.range().finish().end(), result.range().finish().begin() + left_outer_));
        TA_ASSERT(std::equal(right.range().size().begin() + right_inner_, right.range().size().end(), result.range().size().begin() + left_outer_));

        // Check that the  inner dimensions of left and right match
        TA_ASSERT(std::equal(left.range().start().begin() + left_outer_, left.range().start().end(), right.range().start().begin()));
        TA_ASSERT(std::equal(left.range().finish().begin() + left_outer_, left.range().finish().end(), right.range().finish().begin()));
        TA_ASSERT(std::equal(left.range().size().begin() + left_outer_, left.range().size().end(), right.range().size().begin()));

        // Calculate the fused tile dimension
        const size_type m = product(left.range().size().begin(), left.range().size().begin() + left_outer_);
        const size_type k = product(left.range().size().begin() + left_outer_, left.range().size().end());
        const size_type n = product(right.range().size().begin() + right_inner_, right.range().size().end());

        // Do the contraction
        TiledArray::detail::mxm(m, n, k, left.data(), right.data(), result.data());
      }

    private:

      virtual void eval_shape() {
        // Initialize the shape if this tensor is not dense
        if(! (left_.is_dense() && right_.is_dense())) {
          typedef Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> matrix_type;

          // Construct map objects for the shapes

          // Construct the left shape to its map
          matrix_type left_map(m_, k_);
          if(left_.is_dense())
            left_map.fill(1);
          else
            for(std::size_t i = 0; i < m_; ++i)
              for(std::size_t j = 0; j < k_; ++j)
                left_map(i, j) = (left_.get_shape()[i * k_ + j] ? 1 : 0);

          // Construct the right shape to its map
          matrix_type right_map(k_, n_);
          if(right_.is_dense())
            right_map.fill(1);
          else
            for(std::size_t i = 0; i < k_; ++i)
              for(std::size_t j = 0; j < n_; ++j)
                right_map(i, j) = (right_.get_shape()[i * n_ + j] ? 1 : 0);

          // Construct the new shape

          matrix_type res_map = left_map * right_map;

          // Update the shape
          const size_type size = TensorImplBase_::size();
          TiledArray::detail::Bitset<> s(size);
          for(std::size_t i = 0; i < m_; ++i)
            for(std::size_t j = 0; j < n_; ++j)
              if(res_map(i,j))
                s.set(i * m_ + j);
          TensorImplBase_::shape(s);
        }
      }

      static bool done(const bool left, const bool right) { return left && right; }

      virtual madness::Future<bool> eval_children(const expressions::VariableList& vars,
          const std::shared_ptr<pmap_interface>&)
      {

        std::vector<std::string> left_vars;
        std::vector<std::string> right_vars;

        // Construct the left variable list and right inner product variable list
        OuterPred left_outer_pred(right_.vars());
        for(expressions::VariableList::const_iterator it = left_.vars().begin(); it != left_.vars().end(); ++it)
          if(left_outer_pred(*it))
            left_vars.push_back(*it);
        for(expressions::VariableList::const_iterator it = left_.vars().begin(); it != left_.vars().end(); ++it)
          if(! left_outer_pred(*it)) {
            left_vars.push_back(*it);
            right_vars.push_back(*it);
          }

        // Construct the left argument process map
        std::shared_ptr<pmap_interface> left_pmap(new TiledArray::detail::CyclicPmap(
                    TensorImplBase_::get_world(), m_, k_, proc_rows_, proc_cols_));

        // Start the left tensor evaluation
        madness::Future<bool> left_done =
            left_.eval(expressions::VariableList(left_vars.begin(), left_vars.end()),
            left_pmap);

        // Finish constructing the right variable list with the outer product variables
        OuterPred right_outer_pred(left_.vars());
        for(expressions::VariableList::const_iterator it = right_.vars().begin(); it != right_.vars().end(); ++it)
          if(right_outer_pred(*it))
            right_vars.push_back(*it);


        // Construct the right argument process map
        std::shared_ptr<pmap_interface> right_pmap(new TiledArray::detail::CyclicPmap(
                    TensorImplBase_::get_world(), k_, n_, proc_rows_, proc_cols_));

        // Start the right tensor evaluation
        madness::Future<bool> right_done =
            right_.eval(expressions::VariableList(right_vars.begin(), right_vars.end()),
                right_pmap);

        // Wait for the evaluation of the left and right child tensors before returning
        // Note: This does not include evaluation of tiles, only structure.
        return TensorImplBase_::get_world().taskq.add(& ContractionTensorImpl_::done,
            left_done, right_done, madness::TaskAttributes::hipri());
      }
    }; // class ContractionAlgorithmBase

  } // namespace detail
}  // namespace TiledArray

#endif // TILEDARRAY_CONTRACTION_TENSOR_IMPL_H__INCLUDED
