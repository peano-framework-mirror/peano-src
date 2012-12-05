#include "tarch/la/tests/TraitsTest.h"
#include "tarch/la/traits/EqualScalars.h"
#include "tarch/la/Vector.h"

#include "tarch/tests/TestCaseFactory.h"
registerTest(tarch::la::TraitsTest)

namespace tarch {
namespace la {

TraitsTest::TraitsTest ()
:
  TestCase("tarch::la::TraitsTest")
{}

void TraitsTest::run()
{
  testMethod (testEqualScalars);
}

//void TraitsTest::testIsEqual()
//{
//  bool equal = IsEqual<int,int>::value;
//  bool !Equal = ! IsEqual<int,double>::value;
//  validate (equal);
//  validate (!Equal);
//}

void TraitsTest:: testEqualScalars()
{
  bool equal = EqualScalars<Vector<3,double>,Vector<2,double> >::value;
  validate (equal);
  equal = EqualScalars<Vector<3,double>,DynamicVector<double> >::value;
  validate (equal);
  equal = EqualScalars<Vector<3,double>,Vector<3,int> >::value;
  validate (! equal);
  equal = EqualScalars<Vector<3,float>,DynamicVector<double> >::value;
  validate (! equal);
}

}} // namespace tarch, la
