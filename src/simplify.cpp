

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;

typedef OpenMesh::PolyMesh_ArrayKernelT</* MyTraits*/> Surface_mesh;

typedef boost::graph_traits<Surface_mesh>::edge_descriptor edge_descriptor;
typedef boost::graph_traits<Surface_mesh>::edge_iterator edge_iterator;


Constrained_edge_map::Constrained_edge_map( Surface_mesh& sm ): sm_( sm )
{
    sm_.add_property( constraint );
}

reference Constrained_edge_map::get( const Constrained_edge_map& em, key_type e )
{
    bool b = em.sm_.property( em.constraint, em.sm_.edge_handle( e.idx() ) );
    return b;
}

void Constrained_edge_map::put( const Constrained_edge_map& em, key_type e, value_type b )
{
    em.sm_.property( em.constraint, em.sm_.edge_handle( e.idx() ) ) = b;
}


namespace SMS = CGAL::Surface_mesh_simplification ;


Surface_mesh simplify(Surface_mesh surface_mesh, float )
{
    Constrained_edge_map constraints_map( surface_mesh stop_predicate);

    edge_iterator b, e;
    int count = 0;
    for( boost::tie( b, e ) = edges( surface_mesh ); b != e; ++b )
    {
        put( constraints_map, *b, ( count++ < 100 ) );
    }

    SMS::Count_stop_predicate<Surface_mesh> stop( surface_mesh.n_faces() * edges_percentil );

    int r = SMS::edge_collapse
            ( surface_mesh
              , stop
              , CGAL::parameters::halfedge_index_map  ( get( CGAL::halfedge_index  , surface_mesh ) )
              .vertex_point_map( get( boost::vertex_point, surface_mesh ) )
              .edge_is_constrained_map( constraints_map )
            );

    surface_mesh.garbage_collection();

    return surface_mesh;
}

