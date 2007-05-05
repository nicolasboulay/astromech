#ifndef VIDEO_CPP
#define VIDEO_CPP
#include "CImg.h"
#include <string>
#include <linux/types.h> // miss inside videodev2 !!
#include <linux/videodev.h> //drivers video4linux 1.0 deprecated
#include <linux/videodev2.h> //drivers video4linux 2.0
using namespace std; 

class video_t{
public:
  video_t(const std::string &  s, int h, int w);
  ~video_t();
  int set (int width_, int height_, int framerate_,
       int brightness_,  int colour_, int contrast_,
       int whiteness_, int palette_, int agc_);
  void commit();
  void getparam();
  void reopen();
  void print_parameter(void);
  void setmmap();
  void wait_frame();
  void setgrabframe();
  void resetmmap();
  void reset_cycle();
  cimg_library::CImg<unsigned char> * factory_img();
  cimg_library::CImg<unsigned char> & grab_frame(cimg_library::CImg<unsigned char> & img); 
  unsigned int width;
  unsigned int height;
  int framerate;
  int brightness;
  int colour;
  int contrast;
  int whiteness;
  int palette;
  int agc;

private:
  string file_name;
  int fd;

  // V4L 1.0
  unsigned char *framebuf;
  struct video_capability vcap;
  struct video_channel    vc;
  struct video_mmap       mm;
  cimg_library::CImg<unsigned char> * img[2];
  void _open(const string & s);
  inline int clip8(int i);
  inline unsigned char * YUV444toRGB888(unsigned int Y,
					      unsigned int U, 
					      unsigned int V,
					      unsigned char ret[3]);
  inline unsigned char * YUV444toRGB888_true(unsigned int Y, 
						     unsigned int U, 
						     unsigned int V,
					     unsigned char ret[3]);
  inline unsigned char * yuv420p_get(
					   unsigned char * yuv,
					   unsigned char ret[3],
					   unsigned w,
					   unsigned h);
  void yuv420P_to_rgb24(unsigned char * yuv, 
			unsigned char * rgb,
			const unsigned width,
			const  unsigned height);
  void yuv420P_to_rgb24(unsigned char * yuv, 
			cimg_library::CImg<unsigned char> & rgb);
};

#endif
