#include <decimator/simplify.h>

namespace decimator {


Surface_mesh simplify(Surface_mesh surface_mesh, float edges_percentil) {
  if (!CGAL::is_triangle_mesh(surface_mesh)){
    std::cerr << "Input geometry is not triangulated." << std::endl;
    return surface_mesh;
  }

  SMS::Count_ratio_stop_predicate<Surface_mesh> stop(0.1);
 
  Stats stats;
  
  CustomVisitor vis(&stats);
  int r = SMS::edge_collapse(surface_mesh, stop, 
    CGAL::parameters::get_cost(SMS::Edge_length_cost<Surface_mesh>())
    .get_placement(SMS::Midpoint_placement<Surface_mesh>())
    .visitor(vis)
  );

  std::cout << "\nEdges collected: "  << stats.collected
            << "\nEdges proccessed: " << stats.processed
            << "\nEdges collapsed: "  << stats.collapsed
            << std::endl
            << "\nEdges not collapsed due to topological constraints: "  << stats.non_collapsable
            << "\nEdge not collapsed due to cost computation constraints: "  << stats.cost_uncomputable 
            << "\nEdge not collapsed due to placement computation constraints: " << stats.placement_uncomputable 
            << std::endl ; 
            
  std::cout << "\nFinished...\n" << r << " edges removed.\n" 
            << surface_mesh.number_of_edges() << " final edges.\n";

  surface_mesh.collect_garbage();
  return surface_mesh;
}

} // decimator
