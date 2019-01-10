//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//
// Functions
//
#ifndef __CINT__
#pragma link C++ function larcv::as_th2d(const larcv::Image2D&, std::string)+;
#pragma link C++ function larcv::as_th2d_v(const std::vector<larcv::Image2D>&, std::string)+;
#pragma link C++ function larcv::as_tgraph( const larcv::Pixel2DCluster&, bool, larcv::ImageMetea* );
#pragma link C++ function larcv::as_contour_tgraph( const larcv::Pixel2DCluster&, bool, larcv::ImageMetea* );
#pragma link C++ function larcv::setBlueRedColorPalette();
#pragma link C++ function larcv::setRainbowColorPalette();
#pragma link C++ function larcv::setRedHeatColorPalette();
#pragma link C++ class larcv::ViewROOT+;
#endif
//ADD_NEW_CLASS ... do not change this line

#endif




















