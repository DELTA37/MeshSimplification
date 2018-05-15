#include <iostream>
#include <fstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include <CGAL/boost/graph/graph_traits_PolyMesh_ArrayKernelT.h>

#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_length_cost.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_placement.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_simplification/Edge_collapse_visitor_base.h>

namespace decimator {

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point_3;
typedef CGAL::Surface_mesh<Point_3> Surface_mesh; 

typedef boost::graph_traits<Surface_mesh>::edge_descriptor edge_descriptor;
typedef boost::graph_traits<Surface_mesh>::edge_iterator edge_iterator;
typedef boost::graph_traits<Surface_mesh>::halfedge_descriptor halfedge_descriptor ;
typedef boost::graph_traits<Surface_mesh>::vertex_descriptor vertex_descriptor;

namespace SMS = CGAL::Surface_mesh_simplification ;
typedef SMS::Edge_profile<Surface_mesh> Profile ;

struct Stats {
  Stats() : collected(0), 
            processed(0), 
            collapsed(0), 
            non_collapsable(0), 
            cost_uncomputable(0), 
            placement_uncomputable(0)
  {} 
  
  std::size_t collected;
  std::size_t processed;
  std::size_t collapsed;
  std::size_t non_collapsable;
  std::size_t cost_uncomputable;
  std::size_t placement_uncomputable;
};

struct CustomVisitor : SMS::Edge_collapse_visitor_base<Surface_mesh> {
  CustomVisitor(Stats* s) : stats(s) {} 
  void OnCollected(Profile const&, boost::optional<double> const&) {
    ++ stats->collected;
    std::cerr << "\rEdges collected: " << stats->collected << std::flush;
  }                
  
  void OnSelected(Profile const&, boost::optional<double> cost, std::size_t initial , std::size_t current) {
    ++stats->processed;
    if (!cost) {
      ++stats->cost_uncomputable;
    }
      
    if (current == initial) {
      std::cerr << "\n" << std::flush;
    }
    std::cerr << "\r" << current << std::flush ;
  }                
  
  void OnCollapsing(Profile const& , boost::optional<Point> placement) {
    if ( !placement )
      ++stats->placement_uncomputable;
  }                
  
  void OnNonCollapsable(Profile const&) {
    ++stats->non_collapsable;
  }                
  
  void OnCollapsed(Profile const&, vertex_descriptor) {
    ++stats->collapsed;
  }                
  
  Stats* stats;
};

Surface_mesh simplify(Surface_mesh surface_mesh, float stop_edges_percentil);

} // decimator
