#include <fstream>
#include <cstdlib>

namespace decimator {

template<typename Polyhedron>
void visualize(Polyhedron const& ply) {
  std::ofstream out("tmp.off");
  out << ply; 
  std::system("geomview tmp.off");
}


} // decimator
