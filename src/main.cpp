#include <yaply/YaPLY.hpp>
#include <iostream>
#include <Eigen/Dense>
#include <boost/program_options.hpp>

#include <decimation/visual.h>

#include <iostream>
#include <utility>
#include <vector>
#include <fstream>

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


namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help")
    ("filename", po::value<std::string>(), "set filename")
  ;
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  std::string&& fname = vm["filename"].as<std::string>();
  std::ifstream in(fname);
  auto&& points = extract(in);
  return 0;
}


