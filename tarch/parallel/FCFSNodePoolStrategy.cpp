#include "tarch/parallel/FCFSNodePoolStrategy.h"


tarch::logging::Log tarch::parallel::FCFSNodePoolStrategy::_log( "tarch::parallel::FCFSNodePoolStrategy" );


tarch::parallel::FCFSNodePoolStrategy::FCFSNodePoolStrategy():
  NodePoolStrategy(),
  _tag(-1),
  _nodes() {
}


tarch::parallel::FCFSNodePoolStrategy::~FCFSNodePoolStrategy() {
}


void tarch::parallel::FCFSNodePoolStrategy::fillWorkerRequestQueue(RequestQueue& queue) {
  #ifdef Parallel
  assertion( _tag >= 0 );
  while ( tarch::parallel::messages::WorkerRequestMessage::isMessageInQueue(_tag, true) ) {
    tarch::parallel::messages::WorkerRequestMessage message;
    message.receive(MPI_ANY_SOURCE,_tag, true);
    queue.push_back( message );
  }
  #endif
}


void tarch::parallel::FCFSNodePoolStrategy::logQueue( const RequestQueue& queue ) const {
  #ifdef Parallel
  if (queue.empty()) {
	_log.debug( "logQueue()", "queue is empty" );
  }
  else {
    std::ostringstream msg;
    msg << "queue: ";

	  for (RequestQueue::const_iterator p = queue.begin(); p != queue.end(); p++ ) {
	    msg << p->getSenderRank() << ",";
	  }
	  _log.debug( "logQueue()", msg.str() );
  }
  #endif
}


tarch::parallel::messages::WorkerRequestMessage tarch::parallel::FCFSNodePoolStrategy::extractElementFromRequestQueue(RequestQueue& queue) {
  assertion( !queue.empty() );
  tarch::parallel::messages::WorkerRequestMessage result = queue.front();
  queue.pop_front();
  return result;
}


void tarch::parallel::FCFSNodePoolStrategy::addNode(const tarch::parallel::messages::RegisterAtNodePoolMessage& node) {
  #ifdef Parallel
  assertion( !isRegisteredNode(node.getSenderRank()) );

  logTraceInWith1Argument( "addNode(...)", node.getSenderRank() );
  NodePoolListEntry newEntry(
    node.getSenderRank(),
    tarch::parallel::StringTools::convert(node.getNodeName())
  );
  _nodes.push_back( newEntry ) ;
  _nodes.sort();
  logTraceOutWith1Argument( "addNode(...)", newEntry.toString() );
  #endif
}


void tarch::parallel::FCFSNodePoolStrategy::removeNode( int rank ) {
  assertion( isRegisteredNode(rank) );

  for (
    NodeContainer::iterator p = _nodes.begin();
    p != _nodes.end();
    p++
  ) {
    if ( p->getRank() == rank ) {
      #ifdef Debug
      _log.debug( "removeNode(int)", "remove entry " + p->toString() );
      #endif
      _nodes.erase(p);
      _nodes.sort();
      return;
    }
  }
}


bool tarch::parallel::FCFSNodePoolStrategy::hasIdleNode(int forMaster) const {
  return !_nodes.empty() &&
         _nodes.front().isIdle();
}


int tarch::parallel::FCFSNodePoolStrategy::removeNextIdleNode() {
  assertion1( !_nodes.empty(), _nodes.size() );
  assertion3( FCFSNodePoolStrategy::hasIdleNode(-1), _nodes.size(), _nodes.front().toString(), _nodes.begin()->isIdle() );

  int result = _nodes.front().getRank();
  _nodes.pop_front();
  return result;
}


int tarch::parallel::FCFSNodePoolStrategy::getNumberOfIdleNodes() const {
  int result = 0;
  NodeContainer::const_iterator p = _nodes.begin();
  while (p != _nodes.end()&& p->isIdle() ) {
	p++;
	result++;
  }
  return result;
}


void tarch::parallel::FCFSNodePoolStrategy::setNodeIdle( int rank ) {
  for (
    NodeContainer::iterator p = _nodes.begin();
    p != _nodes.end();
    p++
  ) {
    if ( p->getRank() == rank ) {
      p->deActivate();
    }
  }

  _nodes.sort();
}


bool tarch::parallel::FCFSNodePoolStrategy::isRegisteredNode(int rank) const {
  for (
    NodeContainer::const_iterator p = _nodes.begin();
    p != _nodes.end();
    p++
  ) {
    if ( p->getRank() == rank ) {
      return true;
    }
  }
  return false;
}


bool tarch::parallel::FCFSNodePoolStrategy::isIdleNode(int rank) const {
  assertion1( isRegisteredNode(rank), rank );
  for (
    NodeContainer::const_iterator p = _nodes.begin();
    p != _nodes.end();
    p++
  ) {
    if ( p->getRank() == rank && p->isIdle() ) {
      return true;
    }
  }
  return false;
}


void tarch::parallel::FCFSNodePoolStrategy::clearRegisteredNodes() {
  _nodes.clear();
}


int tarch::parallel::FCFSNodePoolStrategy::getNumberOfRegisteredNodes() const {
  return static_cast<int>( _nodes.size() );
}


std::string tarch::parallel::FCFSNodePoolStrategy::toString() const {
  std::ostringstream result;
  for (
    NodeContainer::const_iterator p = _nodes.begin();
    p != _nodes.end();
    p++
  ) {
	result << *p;
  }
  return result.str();
}


int tarch::parallel::FCFSNodePoolStrategy::reserveNode(int forMaster) {
  assertion1(hasIdleNode(forMaster),forMaster);

  NodePoolListEntry result = _nodes.front();
  _nodes.pop_front();

  logDebug( "getFreeNode(int)", "found free node: " << result );

  result.activate();
  _nodes.push_back(result);
  _nodes.sort();

  return result.getRank();
}


void tarch::parallel::FCFSNodePoolStrategy::setNodePoolTag(int tag) {
  _tag = tag;
}
