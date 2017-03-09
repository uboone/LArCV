#ifndef __LARBYSIMAGEANA_CXX__
#define __LARBYSIMAGEANA_CXX__
#include "LArbysImageAna.h"



namespace larcv {

  static LArbysImageAnaProcessFactory __global_LArbysImageAnaProcessFactory__;

  LArbysImageAna::LArbysImageAna(const std::string name)
    : ProcessBase(name),
      _event_tree(nullptr),
      _vtx3d_tree(nullptr)
  {}
      
  void LArbysImageAna::configure(const PSet& cfg)
  {
    _combined_vertex_name = cfg.get<std::string>("CombinedVertexName");
    _combined_particle_offset = cfg.get<uint>("ParticleOffset");
  }
  
  void LArbysImageAna::ClearVertex() {
    _vtx2d_x_v.clear();
    _vtx2d_y_v.clear();
    _circle_x_v.clear();
    _circle_y_v.clear();
    _vtx2d_x_v.resize(3);
    _vtx2d_y_v.resize(3);
    _circle_x_v.resize(3);
    _circle_y_v.resize(3);
    _circle_xs_v.resize(3);
    _par_multi.clear();
    _par_multi.resize(3);
    _ntrack_par_v.clear();
    _nshower_par_v.clear();
    _ntrack_par_v.resize(3);
    _nshower_par_v.resize(3);
    std::fill(_ntrack_par_v.begin(), _ntrack_par_v.end(), 0);
    std::fill(_nshower_par_v.begin(), _nshower_par_v.end(), 0);
    
  }

  void LArbysImageAna::initialize()
  {
    
    _vtx3d_tree = new TTree("Vertex3DTree","");
    _vtx3d_tree->Branch("run"    ,&_run    , "run/i");
    _vtx3d_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _vtx3d_tree->Branch("event"  ,&_event  , "event/i");
    _vtx3d_tree->Branch("entry"  ,&_entry  , "entry/i");
    _vtx3d_tree->Branch("id",&_vtx3d_id, "id/i");
    _vtx3d_tree->Branch("type", &_vtx3d_type, "type/i");
    _vtx3d_tree->Branch("x", &_vtx3d_x, "x/D"  );
    _vtx3d_tree->Branch("y", &_vtx3d_y, "y/D"  );
    _vtx3d_tree->Branch("z", &_vtx3d_z, "z/D"  );
    _vtx3d_tree->Branch("vtx2d_x_v", &_vtx2d_x_v );
    _vtx3d_tree->Branch("vtx2d_y_v", &_vtx2d_y_v );
    _vtx3d_tree->Branch("cvtx2d_x_v",&_circle_x_v);
    _vtx3d_tree->Branch("cvtx2d_y_v",&_circle_y_v);
    _vtx3d_tree->Branch("cvtx2d_xs_v",&_circle_xs_v);
    _vtx3d_tree->Branch("multi_v",&_par_multi);
    _vtx3d_tree->Branch("ntrack_par_v",&_ntrack_par_v);
    _vtx3d_tree->Branch("nshower_par_v",&_nshower_par_v);


    _event_tree = new TTree("EventTree","");
    _event_tree->Branch("run"    ,&_run    , "run/i");
    _event_tree->Branch("subrun" ,&_subrun , "subrun/i");
    _event_tree->Branch("event"  ,&_event  , "event/i");
    _event_tree->Branch("entry"  ,&_entry  , "entry/i");
    _event_tree->Branch("Vertex3D_v",&_vertex3d_v);
    _event_tree->Branch("ParticleCluster_vvv",&_particle_cluster_vvv);
    _event_tree->Branch("TrackClusterCompound_vvv",&_track_compound_vvv);
    
  }

  bool LArbysImageAna::process(IOManager& mgr)
  {
        
    LARCV_DEBUG() << "process" << std::endl;

    /// get the data manager
    const auto& data_mgr  = _mgr_ptr->DataManager();
    const auto& ass_man   = data_mgr.AssManager();
    
    
    /// unique event keys
    const auto& event_id = mgr.event_id();
    _run    = (uint) event_id.run();
    _subrun = (uint) event_id.subrun();
    _event  = (uint) event_id.event();
    _entry =  (uint) mgr.current_entry();

    /// get the track estimate data
    const auto vtx3d_array = (larocv::data::Vertex3DArray*)
      data_mgr.Data(data_mgr.ID(_combined_vertex_name), 0);
    
    const auto& vtx_cluster_v = vtx3d_array->as_vector();
    
    _n_vtx3d = (uint) vtx_cluster_v.size();


    _vertex3d_v.clear();
    _vertex3d_v.resize(vtx_cluster_v.size(),nullptr);

    _particle_cluster_vvv.clear();
    _particle_cluster_vvv.resize(vtx_cluster_v.size());

    _track_compound_vvv.clear();
    _track_compound_vvv.resize(vtx_cluster_v.size());
    
    for(uint vtx_id=0;vtx_id<_n_vtx3d;++vtx_id) { 

      auto& particle_cluster_vv = _particle_cluster_vvv[vtx_id];
      auto& track_compound_vv = _track_compound_vvv[vtx_id];
      
      // clear vertex
      ClearVertex();
	
      // set the vertex index number
      _vtx3d_id=vtx_id;
      
      // get this 3D vertex
      const auto& vtx3d = vtx_cluster_v[vtx_id];
      _vertex3d_v[vtx_id] = &vtx3d;
      
      // set the vertex type
      _vtx3d_type = (uint) vtx3d.type;

      // set the 3D coordinates
      _vtx3d_x = vtx3d.x;
      _vtx3d_y = vtx3d.y;
      _vtx3d_z = vtx3d.z;
      
      // set the number of planes this vertex was reconstructed from
      _vtx3d_n_planes = (uint)vtx3d.num_planes;


      particle_cluster_vv.resize(3);
      track_compound_vv.resize(3);
      
      for(uint plane_id=0; plane_id<3;  ++plane_id) {
	auto& particle_cluster_v = particle_cluster_vv[plane_id];
	auto& track_compound_v = track_compound_vv[plane_id];
	
	auto track_particle_cluster_id = data_mgr.ID(_combined_vertex_name);
	// query the vertex type it's 0 (time vtx) or 1 (wire vtx)
	if (_vtx3d_type < 2) {
	  // store circle vertex information
	  const auto& circle_vtx   = vtx3d.cvtx2d_v.at(plane_id);
	  const auto& circle_vtx_c = circle_vtx.center;
	  auto& circle_x  = _circle_x_v [plane_id];
	  auto& circle_y  = _circle_y_v [plane_id];
	  auto& circle_xs = _circle_xs_v[plane_id];
	  circle_x = circle_vtx_c.x;
	  circle_y = circle_vtx_c.y;
	  circle_xs = (uint) circle_vtx.xs_v.size();
	}
	
	auto& vtx2d_x = _vtx2d_x_v[plane_id];
	auto& vtx2d_y = _vtx2d_y_v[plane_id];

	// store the 2D vertex information for this plane
	vtx2d_x = vtx3d.cvtx2d_v[plane_id].center.x;
	vtx2d_y = vtx3d.cvtx2d_v[plane_id].center.y;

	//get the particle cluster array
	const auto par_array = (larocv::data::ParticleClusterArray*)
	  data_mgr.Data(track_particle_cluster_id, plane_id+_combined_particle_offset);

	//get the compound array
	const auto comp_array = (larocv::data::TrackClusterCompoundArray*)
	  data_mgr.Data(track_particle_cluster_id, plane_id+_combined_particle_offset+3);
	
	auto par_ass_idx_v = ass_man.GetManyAss(vtx3d,par_array->ID());

	_par_multi[plane_id] = (uint)par_ass_idx_v.size();

	particle_cluster_v.resize(par_ass_idx_v.size(),nullptr);
	track_compound_v.resize(par_ass_idx_v.size(),nullptr);

	for(size_t ass_id=0;ass_id<par_ass_idx_v.size();++ass_id) {
	  auto ass_idx = par_ass_idx_v[ass_id];
	  if (ass_idx==kINVALID_SIZE) throw larbys("Invalid vertex->particle association detected");
	  const auto& par = par_array->as_vector()[ass_idx];
	  particle_cluster_v[ass_id] = &par;
	  if (par.type==larocv::data::ParticleType_t::kTrack) _ntrack_par_v[plane_id]++;
	  if (par.type==larocv::data::ParticleType_t::kShower)_nshower_par_v[plane_id]++;
	  else LARCV_WARNING() << "Unknown particle found" << std::endl;
	  
	  auto comp_ass_id = ass_man.GetOneAss(par,comp_array->ID());
	  if (comp_ass_id==kINVALID_SIZE) continue;
	  const auto& comp = comp_array->as_vector()[comp_ass_id];
	  track_compound_v[ass_id] = &comp;
	}
	
	
      } // end plane
      _vtx3d_tree->Fill();
    } // end loop over vtx
    _event_tree->Fill();
    
    return true;
  }

  void LArbysImageAna::finalize()
  {
    _event_tree->Write();
    _vtx3d_tree->Write();
  }

}
#endif

