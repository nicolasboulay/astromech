#ifndef VIDEO_PWC_CPP
#define VIDEO_PWC_CPP
#include "video.h"
#include "/usr/src/linux/drivers/media/video/pwc/pwc-ioctl.h"
using namespace std; 

class video_pwc_t : public video_t
{
public:
  video_pwc_t() : video_t(), fps(5) {}
  video_pwc_t(const std::string &  s, int h, int w):video_t(s,h,w), fps(5) {};
  unsigned fps;
  int agc;
  int shutter_speed;
  int _compression;
  int set_noise_filter(int i);
  int set_agc(int i);  
  virtual void print_parameter(void);
  int get_agc(int & i);
  int set_shutter(int i);
  int awb(int mode);
  int awb();
  int compression();
  int compression(int c);
protected:
  virtual void video_windows_set();
};

#endif
