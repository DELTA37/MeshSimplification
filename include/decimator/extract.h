#include <Eigen/Dense>
#include <vector>
#include <utility>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Edge_collapse_visitor_base.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_and_length.h> 
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/property_map.h>
#include <CGAL/IO/read_ply_points.h>


namespace decimator {

constexpr size_t READ_BUFFER_CONSTANT = 100000;

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;

typedef CGAL::cpp11::tuple<Point, Vector> PN;
typedef CGAL::Nth_of_tuple_property_map<0, PN> Point_map;
typedef CGAL::Nth_of_tuple_property_map<1, PN> Normal_map;


std::vector<PN> extract(std::ifstream& in) {
  std::vector<PN> points;

  if (!in) {
    std::cerr << "unable read from file" << std::endl;
  }

  char read_buffer[READ_BUFFER_CONSTANT];
  in.rdbuf()->pubsetbuf(read_buffer, READ_BUFFER_CONSTANT);

  CGAL::read_ply_points_with_properties(
    in,
    std::back_inserter(points),
    CGAL::make_ply_point_reader(Point_map()),
    CGAL::make_ply_normal_reader(Normal_map())
  );

  for (std::size_t i = 0; i < 5; ++i) {
    const Point& p = get<0>(points[i]);
    const Vector& n = get<1>(points[i]);
    std::cout << "Point (" << p << ") with normal (" << n << ")" << std::endl;
  }

  return points;
}

} // decimator
