#ifndef EYE_PROCESSING_H
#define EYE_PROCESSING_H

#include "video-pwc.h"
#include "CImg.h"

using namespace cimg_library;

class eye_processing_t 
{
 public:
  eye_processing_t();
  ~eye_processing_t();
  void init_device();
  void close_device();
  void grab_frame();
  void take_before_match_picture();
  void panier_processing();
 protected:
  int h;
  int w;
  CImg<unsigned char> before_match_picture; 
  CImg<unsigned char> * image_cur;
  video_pwc_t camera;
  void kind_of_diff(CImg<unsigned char> & res,
		    CImg<unsigned char> & a,
		    CImg<unsigned char> & b);
  inline unsigned char kind_of_diff(unsigned char a,
				    unsigned char b);
};

#endif
