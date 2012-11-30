#include "peano/parallel/loadbalancing/OracleForOnePhase.h"
#include "tarch/Assertions.h"
#include "tarch/parallel/Node.h"


peano::parallel::loadbalancing::LoadBalancingFlag peano::parallel::loadbalancing::getLoadBalancingFlag( int value ) {
  switch (value) {
    case Continue:
      return Continue;
    case Join:
      return Join;
    case ForkOnce:
      return ForkOnce;
    case ForkGreedy:
      return ForkGreedy;
    default:
      assertion3( false, "fork type not supported", value, tarch::parallel::Node::getInstance().getRank() );
      return Continue;
  }
}


std::string peano::parallel::loadbalancing::toString(const LoadBalancingFlag& flag) {
  switch (flag) {
    case Continue:
      return "Continue";
    case Join:
      return "Join";
    case ForkOnce:
      return "ForkOnce";
    case ForkGreedy:
      return "ForkGreedy";
    default:
      return "<default>";
  }
}
