#ifndef CXXNET_LAYER_ACTIVATION_LAYER_INL_HPP_
#define CXXNET_LAYER_ACTIVATION_LAYER_INL_HPP_

#include <mshadow/tensor.h>
#include "./layer.h"
#include "./op.h"

namespace cxxnet {
namespace layer {

template<typename xpu,typename ForwardOp, typename BackOp>
class ActivationLayer : public ILayer<xpu>{
 public:
  virtual ~ActivationLayer(void) {}
  virtual void InitConnection(const std::vector<Node<xpu>*> &nodes_in,
                              const std::vector<Node<xpu>*> &nodes_out,
                              ConnectState<xpu> *p_cstate) {
    utils::Check(nodes_in.size() == 1 && nodes_out.size() == 1,
                 "ActivationLayer Layer only support 1-1 connection");
    nodes_out[0]->data.shape_ = nodes_in[0]->data.shape_;
  }
  virtual void Forward(bool is_train,
                       const std::vector<Node<xpu>*> &nodes_in,
                       const std::vector<Node<xpu>*> &nodes_out,
                       ConnectState<xpu> *p_cstate) {
    using namespace mshadow::expr;
    // InitConnection is already called, no need to check size again
    nodes_in[0]->data = F<ForwardOp>(nodes_in[0]->data);
    mshadow::Copy(nodes_out[0]->data, nodes_in[0]->data, nodes_out[0]->data.stream_);
  }
  virtual void Backprop(bool prop_grad,
                        const std::vector<Node<xpu>*> &nodes_in,
                        const std::vector<Node<xpu>*> &nodes_out,
                        ConnectState<xpu> *p_cstate) {
    using namespace mshadow::expr;
    if (prop_grad) {
      nodes_in[0]->data = F<BackOp>(nodes_in[0]->data) * nodes_out[0]->data;
    }
  }
};
}  // namespace layer
}  // namespace cxxnet
#endif  // LAYER_ACTIVATION_LAYER_INL_HPP_

