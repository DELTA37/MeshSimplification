#include <fstream>
#include <cstdlib>

namespace decimator {

template<typename Polyhedron>
void visualize(Polyhedron const& ply) {
  ofstream out("tmp.off");
  out << ply; 
  std::system("tmp.off");
}

} // decimator
