#include "tarch/parallel/NodePoolListEntry.h"
#include "tarch/Assertions.h"


#include <sstream>


tarch::parallel::NodePoolListEntry::NodePoolListEntry(int rank, const std::string& name):
  _rank(rank),
  _state(WORKING),
  _name(name) {
}


tarch::parallel::NodePoolListEntry::~NodePoolListEntry() {
}


std::ostream& operator<<( std::ostream& out, const tarch::parallel::NodePoolListEntry& entry ) {
  entry.toString(out);
  return out;
}


std::string tarch::parallel::NodePoolListEntry::getNodeName() const {
  return _name;
}


std::string tarch::parallel::NodePoolListEntry::toString() const {
  std::ostringstream out;
  toString(out);
  return out.str();
}


void tarch::parallel::NodePoolListEntry::toString(std::ostream& out) const {
  out << "(rank:" << _rank;
  switch (_state) {
    case IDLE:    out << ",state:idle";     break;
    case WORKING: out << ",state:working";  break;
  }
  out << ",name:" << _name << ")";
}


bool tarch::parallel::NodePoolListEntry::operator==( const NodePoolListEntry& than ) const {
  return _rank==than._rank;
}


void tarch::parallel::NodePoolListEntry::activate() {
  assertionEquals1( _state, IDLE, toString() );
  _state = WORKING;
}


void tarch::parallel::NodePoolListEntry::deActivate() {
  _state = IDLE;
}


int tarch::parallel::NodePoolListEntry::getRank() const {
  return _rank;
}


bool tarch::parallel::NodePoolListEntry::isIdle() const {
  return _state == IDLE;
}


bool tarch::parallel::NodePoolListEntry::operator<( const tarch::parallel::NodePoolListEntry& than ) const {
  return isIdle() && !than.isIdle();
}

