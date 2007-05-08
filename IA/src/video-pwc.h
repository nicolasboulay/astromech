#ifndef VIDEO_PWC_CPP
#define VIDEO_PWC_CPP
#include "video.hpp"

using namespace std; 

class video_pwc_t : public video_t
{
public:
  video_pwc_t(const std::string &  s, int h, int w):video_t(s,h,w), fps(5) {};
  unsigned fps;
  unsigned agc;
  unsigned shutter_speed;
  int set_noise_filter(int i);
  int set_agc(int i);  
  virtual void print_parameter(void);
  int get_agc(int & i);
protected:
  virtual void video_windows_set();
};

#endif
