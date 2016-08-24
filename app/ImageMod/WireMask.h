/**
 * \file WireMask.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class WireMask
 *
 * @author kazuhiro
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __WIREMASK_H__
#define __WIREMASK_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class WireMask
     This module is used to mask column pixels that correspond to a set of specified wires.\n
  */
  class WireMask : public ProcessBase {

  public:
    
    /// Default constructor
    WireMask(const std::string name="WireMask");
    
    /// Default destructor
    ~WireMask(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  private:

    std::string _image_producer;  ///< Image to mask
    size_t      _plane_id;        ///< Plane ID (i.e. EventImage2D index number) to mask wires for
    std::vector<size_t> _wire_v;  ///< A list of wire numbers to be masked

  };

  /**
     \class larcv::WireMaskFactory
     \brief A concrete factory class for larcv::WireMask
  */
  class WireMaskProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    WireMaskProcessFactory() { ProcessFactory::get().add_factory("WireMask",this); }
    /// dtor
    ~WireMaskProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new WireMask(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

