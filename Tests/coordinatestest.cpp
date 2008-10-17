#include <iostream>
#include <coordinatestest.h>
#include <coordinates.h>
#include <permutation.h>

using namespace TiledArray;

struct ElementTag{};
struct TileTag{};  

void CoordinatesTest() {
  std::cout << "Coordinates Tests:" << std::endl;

  typedef LatticeCoordinate<int, 1, ElementTag> Point1;
  typedef LatticeCoordinate<int, 2, ElementTag> Point2;
  typedef LatticeCoordinate<int, 3, ElementTag> Point3;
  typedef LatticeCoordinate<int, 4, ElementTag> Point4;

  // Default constuctor
  Point1 p1(3);
  std::cout << "Point in 1-d space: " << p1 << std::endl;
  std::cout << "Tuple Tests:" << std::endl;
  
  // Test constructors.

  // Default constuctor
  Point1 p1D;
  std::cout << "1D Tuple with default constructor: " << p1D << std::endl;
  
  // Constructor with specified value for all elements
  Point2 p2D(1);
  std::cout << "2D Tuple with single value specified: " << p2D << std::endl;
  
  // Copy constructor test
  Point3 p3D(2);
  Point3 p3Dcopy(p3D);
  std::cout << "3D Tuple created with copy constructor: Original: " << p3D
      << " Copy: " << p3Dcopy << std::endl;
  
  // Multiple values specified
  int values1[] = { 1, 2, 3, 4 };
  Point4 p4D(values1);
  std::cout << "4D tuple with values specified for each element: " << p4D
      << std::endl;
  
  // Element accessor
  p4D[0] = 5;
  p4D[1] = 6;
  p4D[2] = 7;
  p4D[3] = 8;
  std::cout << "Tuple element accessor, set 4D tuple to (5, 6, 7, 8): "
      << p4D << std::endl;
  
  // Tuple iterator test
  std::cout << "Iteration tests: ";
  for (Point4::iterator it = p4D.begin(); it != p4D.end(); ++it) {
    std::cout << *it << ", ";
  }
  std::cout << std::endl;
  
  // Arithmatic tests
  std::cout << "Arithmatic Tests:" << std::endl;
  std::cout << "(2,2,2) + (1,1,1) = " << Point3(2) + Point3(1) << std::endl;
  std::cout << "(3,3,3) - (1,1,1) = " << Point3(3) - Point3(1) << std::endl;
  std::cout << "(1,1,1) - (3,3,3) = " << Point3(1) - Point3(3) << std::endl;
  std::cout << "-(4,4,4) = " << -(Point3(4)) << std::endl;
  std::cout << "(2,2,2) += (1,1,1) = " << (Point3(2) += Point3(1)) << std::endl;
  std::cout << "(3,3,3) -= (1,1,1) = " << (Point3(3) -= Point3(1)) << std::endl;

  // Comparison tests
  int values2[4] = {2, 3, 4, 5};
  int values3[4] = {4, 3, 2, 1};
  int values4[4] = {1, 2, 3, 5};
  int values5[4] = {1, 2, 3, 3};
  Point4 comp1(values1);
  Point4 comp2(values2);
  Point4 comp3(values3);
  Point4 comp4(values4);
  Point4 comp5(values5);

  std::cout << "Comparision Tests:" << std::endl;
  std::cout << "(1,2,3,4) < (2,3,4,5) = " << (comp1 < comp2) << std::endl;
  std::cout << "(1,2,3,4) > (2,3,4,5) = " << (comp1> comp2) << std::endl;
  std::cout << "(1,2,3,4) <= (2,3,4,5) = " << (comp1 <= comp2) << std::endl;
  std::cout << "(1,2,3,4) >= (2,3,4,5) = " << (comp1 >= comp2) << std::endl;
  std::cout << "(1,2,3,4) == (2,3,4,5) = " << (comp1 == comp2) << std::endl;
  std::cout << "(1,2,3,4) != (2,3,4,5) = " << (comp1 != comp2) << std::endl;

  std::cout << "(1,2,3,4) < (4,3,2,1) = " << (comp1 < comp3) << std::endl;
  std::cout << "(1,2,3,4) > (4,3,2,1) = " << (comp1> comp3) << std::endl;
  std::cout << "(1,2,3,4) <= (4,3,2,1) = " << (comp1 <= comp3) << std::endl;
  std::cout << "(1,2,3,4) >= (4,3,2,1) = " << (comp1 >= comp3) << std::endl;
  std::cout << "(1,2,3,4) == (4,3,2,1) = " << (comp1 == comp3) << std::endl;
  std::cout << "(1,2,3,4) != (4,3,2,1) = " << (comp1 != comp3) << std::endl;

  std::cout << "(1,2,3,4) == (1,2,3,4) = " << (comp1 == comp1) << std::endl;
  std::cout << "(1,2,3,4) == (1,2,3,5) = " << (comp1 == comp4) << std::endl;
  std::cout << "(1,2,3,4) == (1,2,3,3) = " << (comp1 == comp5) << std::endl;

  std::cout << "End Tuple Test" << std::endl << std::endl;
  
  // permutation
  Permutation<3> perm3 = Permutation<3>::unit();
  std::cout << "Permutation: " << perm3 << std::endl;
  Permutation<4>::Index _perm4[] = {1,2,3,4};
  Permutation<4> perm4(_perm4);
  std::cout << "Permutation: " << perm4 << std::endl;
      
}
  
#if 0
  // Constructor with specified value for all elements
  TiledArray::Tuple<2> tuple2D(1);
  std::cout << "2D Tuple with single value specified: " << tuple2D << std::endl;
  
  // Copy constructor test
  TiledArray::Tuple<3> tuple3D(2);
  TiledArray::Tuple<3> tuple3Dcopy(tuple3D);
  std::cout << "3D Tuple created with copy constructor: Original: " << tuple3D
      << " Copy: " << tuple3Dcopy << std::endl;
  
  // Multiple values specified
  int values1[4] = { 1, 2, 3, 4 };
  TiledArray::Tuple<4> tuple4D(values1);
  std::cout << "4D tuple with values specified for each element: " << tuple4D
      << std::endl;
  
  // Element accessor
  tuple4D[0] = 5;
  tuple4D[1] = 6;
  tuple4D[2] = 7;
  tuple4D[3] = 8;
  std::cout << "Tuple element accessor, set 4D tuple to (5, 6, 7, 8): "
      << tuple4D << std::endl;
  
  // Tuple iterator test
  std::cout << "Iteration tests: ";
  for (TiledArray::Tuple<4>::iterator it = tuple4D.begin(); it != tuple4D.end(); ++it) {
    std::cout << *it << ", ";
  }
  std::cout << std::endl;
  
  // Arithmatic tests
  std::cout << "Arithmatic Tests:" << std::endl;
  std::cout << "(2,2,2) + (1,1,1) = " << TiledArray::Tuple<3>(2) + TiledArray::Tuple<3>(1) << std::endl;
  std::cout << "(3,3,3) - (1,1,1) = " << TiledArray::Tuple<3>(3) - TiledArray::Tuple<3>(1) << std::endl;
  std::cout << "(1,1,1) - (3,3,3) = " << TiledArray::Tuple<3>(1) - TiledArray::Tuple<3>(3) << std::endl;
  std::cout << "-(4,4,4) = " << -(TiledArray::Tuple<3>(4)) << std::endl;
  std::cout << "(2,2,2) * (3,3,3) = " << TiledArray::Tuple<3>(2) * TiledArray::Tuple<3>(3) << std::endl;
  std::cout << "(3,3,3) / (2,2,2) = " << TiledArray::Tuple<3>(3) / TiledArray::Tuple<3>(2) << std::endl;
  std::cout << "(6,6,6) / (2,2,2) = " << TiledArray::Tuple<3>(6) / TiledArray::Tuple<3>(2) << std::endl;
  std::cout << "(7,7,7) % (2,2,2) = " << TiledArray::Tuple<3>(7) % TiledArray::Tuple<3>(2) << std::endl;
  std::cout << "(2,2,2) += (1,1,1) = " << (TiledArray::Tuple<3>(2) += TiledArray::Tuple<3>(1)) << std::endl;
  std::cout << "(3,3,3) -= (1,1,1) = " << (TiledArray::Tuple<3>(3) -= TiledArray::Tuple<3>(1)) << std::endl;

  // Comparison tests
      int values2[4] = {2, 3, 4, 5};
      int values3[4] = {4, 3, 2, 1};
      int values4[4] = {1, 2, 3, 5};
      int values5[4] = {1, 2, 3, 3};
      TiledArray::Tuple<4> comp1(values1);
      TiledArray::Tuple<4> comp2(values2);
      TiledArray::Tuple<4> comp3(values3);
      TiledArray::Tuple<4> comp4(values4);
      TiledArray::Tuple<4> comp5(values5);

      std::cout << "Comparision Tests:" << std::endl;
      std::cout << "(1,2,3,4) < (2,3,4,5) = " << (comp1 < comp2) << std::endl;
      std::cout << "(1,2,3,4) > (2,3,4,5) = " << (comp1> comp2) << std::endl;
      std::cout << "(1,2,3,4) <= (2,3,4,5) = " << (comp1 <= comp2) << std::endl;
      std::cout << "(1,2,3,4) >= (2,3,4,5) = " << (comp1 >= comp2) << std::endl;
      std::cout << "(1,2,3,4) == (2,3,4,5) = " << (comp1 == comp2) << std::endl;
      std::cout << "(1,2,3,4) != (2,3,4,5) = " << (comp1 != comp2) << std::endl;

      std::cout << "(1,2,3,4) < (4,3,2,1) = " << (comp1 < comp3) << std::endl;
      std::cout << "(1,2,3,4) > (4,3,2,1) = " << (comp1> comp3) << std::endl;
      std::cout << "(1,2,3,4) <= (4,3,2,1) = " << (comp1 <= comp3) << std::endl;
      std::cout << "(1,2,3,4) >= (4,3,2,1) = " << (comp1 >= comp3) << std::endl;
      std::cout << "(1,2,3,4) == (4,3,2,1) = " << (comp1 == comp3) << std::endl;
      std::cout << "(1,2,3,4) != (4,3,2,1) = " << (comp1 != comp3) << std::endl;

      std::cout << "(1,2,3,4) == (1,2,3,4) = " << (comp1 == comp1) << std::endl;
      std::cout << "(1,2,3,4) == (1,2,3,5) = " << (comp1 == comp4) << std::endl;
      std::cout << "(1,2,3,4) == (1,2,3,3) = " << (comp1 == comp5) << std::endl;

      // Permutation tests
      int values6[4] = {1,3,0,2};
      TiledArray::Tuple<4> randPerm(values6);
      std::cout << "Permutation Tests:" << std::endl;
      std::cout << "Initial: " << comp1 << std::endl;
      std::cout << "(1,2,3,4).Permute() = " << comp1.permute() << std::endl;
      std::cout << "(2,3,4,1).ReversePermute() = " << comp1.reverse_permute() << std::endl;
      std::cout << "(5,6,7,8).Permute((1,3,0,2)) = " << tuple4D.permute(randPerm) << std::endl;

      std::cout << "End Tuple Test" << std::endl << std::endl;
#endif

