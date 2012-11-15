#include "peano/grid/nodes/Constants.h"


std::string peano::grid::nodes::counterToString( int counter ) {
  if (counter < CounterPersistentNode) {
    return "invalid(smaller-zero)";
  }
  if (counter==CounterPersistentNode) {
    return "persistent-node";
  }
  if (counter<CounterHangingNode) {
    return "persistent-node(already-updated-in-4powd-pattern)";
  }
  if (counter==CounterHangingNode) {
    return "hanging-node";
  }
  if (counter<CounterNewNode) {
    return "hanging-node(already-updated-in-4powd-pattern)";
  }
  if (counter==CounterNewNode) {
    return "new-node";
  }
  if (counter<CounterPersistentNodeDelete) {
    return "new-node(already-updated-in-4powd-pattern)";
  }
  if (counter==CounterPersistentNodeDelete) {
    return "persistent-node-delete";
  }
  if (counter<CounterNodeWithoutLifecycle) {
    return "persistent-node-delete(already-updated-in-4powd-pattern)";
  }
  if (counter==CounterNodeWithoutLifecycle) {
    return "node-without-lifecycle";
  }
  if (counter<CounterNodeWithoutLifecycle+TWO_POWER_D) {
    return "node-without-lifecycle(already-updated-in-4powd-pattern)";
  }
  return "undef";
}
