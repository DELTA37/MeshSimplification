#include <yaply/YaPLY.hpp>
#include <Eigen/Dense>

#include <utility>

namespace decimator {
  
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;

typedef CGAL::cpp11::tuple<Point, Vector> PN;
typedef CGAL::Nth_of_tuple_property_map<0, PNCI> Point_map;
typedef CGAL::Nth_of_tuple_property_map<1, PNCI> Normal_map;


std::vector<PN> extract(std::ifstream& in) {
  std::vector<PN> points;

  if (!in) {
    std::cerr << "unable read from file" << std::endl;
  }
  CGAL::read_ply_points_with_properties(
    in,
    std::back_inserter(points),
    std::make_tuple(
      CGAL::make_ply_point_reader(Point_map()),
      CGAL::make_ply_normal_reader(Normal_map())
    )
  );

  for (std::size_t i = 0; i < points.size(); ++i) {
    const Point& p = get<0>(points[i]);
    const Vector& n = get<1>(points[i]);
    std::cout << "Point (" << p << ") with normal (" << n << std::endl;
  }

  return points;
}

} // decimator
