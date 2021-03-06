//#include <eigen3/Eigen/Dense>
#include <vector>
#include <utility>
#include <array>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh_simplification/edge_collapse.h>
#include <CGAL/Surface_mesh_simplification/Edge_collapse_visitor_base.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Count_ratio_stop_predicate.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Midpoint_and_length.h> 
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/property_map.h>
#include <CGAL/IO/read_ply_points.h>
#include <CGAL/Homogeneous.h>
#include <CGAL/Polyhedron_traits_with_normals_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>

#include "decimator/rply.h"

namespace decimator {

constexpr size_t READ_BUFFER_CONSTANT = 100000;
typedef CGAL::Simple_cartesian<double> Kernel;
//typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;

typedef CGAL::Surface_mesh<Point> Mesh;
typedef std::array<double, 3> Vertex;
typedef std::array<Mesh::Vertex_index, 3> Facet;

typedef CGAL::cpp11::tuple<Point, Vector> PN;
typedef CGAL::Nth_of_tuple_property_map<0, PN> Point_map;
typedef CGAL::Nth_of_tuple_property_map<1, PN> Normal_map;


struct MeshVertexAdder :  public std::iterator<
                            std::output_iterator_tag,
                            Point,
                            Point,
                            Point,
                            Point
                          > {
private:
  Mesh& m;

public:
  MeshVertexAdder(Mesh& input_mesh) : m(input_mesh) {}
  MeshVertexAdder& operator++() { return *this; }
  MeshVertexAdder& operator++(int) { return *this; }
  MeshVertexAdder& operator*() { return *this; }
  MeshVertexAdder& operator=(Point const& p) {
    m.add_vertex(p);
    return *this;
  }
};

struct MeshFacetAdder :  public std::iterator<
                            std::output_iterator_tag,
                            Point,
                            Point,
                            Point,
                            Point
                          > {
private:
  Mesh& m;

public:
  MeshFacetAdder(Mesh& input_mesh) : m(input_mesh) {}
  MeshFacetAdder& operator++() {return *this; }
  MeshFacetAdder& operator++(int) {return *this; }
  MeshFacetAdder& operator*() { return *this; }
  MeshFacetAdder& operator=(Facet const& f) {
    m.add_face(std::get<0>(f), std::get<1>(f), std::get<2>(f));
    return *this;
  }
};


static int vertex_cb(p_ply_argument argument) {
  long ind;
  Vertex* point;
  Mesh* m;
  std::pair<Mesh*, Vertex*>* point_args;

  ply_get_argument_user_data(argument, reinterpret_cast<void**>(&point_args), &ind);

  m = point_args->first;
  point = point_args->second;

  (*point)[ind] = ply_get_argument_value(argument);
  //std::cout << (*point)[0] << std::endl << (*point)[1] << std::endl << (*point)[2] << std::endl;
  if (ind == 2) {
    m->add_vertex(Point((*point)[0], (*point)[1], (*point)[2]));
  }
  return 1;
}

static int face_cb(p_ply_argument argument) {
  long ind;
  Facet* facet;
  Mesh* m;

  std::pair<Mesh*, Facet*>* facet_args;
  ply_get_argument_user_data(argument, reinterpret_cast<void**>(&facet_args), NULL);
  ply_get_argument_property(argument, NULL, NULL, &ind);

  m = facet_args->first;
  facet = facet_args->second;

  (*facet)[ind] = (Mesh::Vertex_index)ply_get_argument_value(argument);
  if (ind == 2) {
    m->add_face((*facet)[0], (*facet)[1], (*facet)[2]);
  }
  return 1;
}


Mesh extract(char const* fname) {
  long nvertices, ntriangles;
  p_ply ply = ply_open(fname, NULL, 0, NULL);
  ply_read_header(ply);

  Mesh m;
  Vertex point;
  Facet facet;
  
  std::pair<Mesh*, Vertex*> point_args(&m, &point);
  std::pair<Mesh*, Facet*> facet_args(&m, &facet);

  nvertices = ply_set_read_cb(ply, "vertex", "x", vertex_cb, &point_args, 0);
              ply_set_read_cb(ply, "vertex", "y", vertex_cb, &point_args, 1);
              ply_set_read_cb(ply, "vertex", "z", vertex_cb, &point_args, 2);

  ntriangles = ply_set_read_cb(ply, "face", "vertex_indices", face_cb, &facet_args, 0);
  ply_read(ply);
  ply_close(ply); 

  return m;
}

void store(const Mesh &m, char const* fname)
{
  p_ply ply = ply_create(fname, PLY_ASCII, NULL, 0, NULL);

  ply_add_element(ply, "vertex", m.number_of_vertices());
  ply_add_property(ply, "x", PLY_FLOAT, PLY_FLOAT, PLY_FLOAT);
  ply_add_property(ply, "y", PLY_FLOAT, PLY_FLOAT, PLY_FLOAT);
  ply_add_property(ply, "z", PLY_FLOAT, PLY_FLOAT, PLY_FLOAT);
  //ply_add_property(ply, "nx", PLY_FLOAT, NULL, NULL);
  //ply_add_property(ply, "ny", PLY_FLOAT, NULL, NULL);
  //ply_add_property(ply, "nz", PLY_FLOAT, NULL, NULL);

  ply_add_element(ply, "face", m.number_of_faces());
  ply_add_list_property(ply, "vertex_indices", PLY_UCHAR, PLY_INT);

  ply_write_header(ply);

  for (auto it = m.vertices().begin(); it != m.vertices().end(); it++) {
    ply_write(ply, m.point(*it)[0]);
    ply_write(ply, m.point(*it)[1]);
    ply_write(ply, m.point(*it)[2]);
  }

  for (auto it = m.faces().begin(); it != m.faces().end(); it++) {
    ply_write(ply, 3);
    auto halfedge = m.halfedge(*it);
    ply_write(ply, m.target(halfedge));
    halfedge = m.next(halfedge);
    ply_write(ply, m.target(halfedge));
    halfedge = m.next(halfedge);
    ply_write(ply, m.target(halfedge));
  }

  ply_close(ply);
}

} // decimator
