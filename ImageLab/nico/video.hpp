#ifndef VIDEO_CPP
#define VIDEO_CPP
#include <string>

using namespace std; 

class video_t{
public:
  video_t(const string &  s);
  video_t(void) : file_name(NULL), fd(NULL) {};
  int set (int width_, int height_, int framerate_,
       int brightness_,  int colour_, int contrast_,
       int whiteness_, int palette_, int agc_);
  void commit();
  void fetch(void);

  int width;
  int height;
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
  void _open(const string & s);
};

#endif
