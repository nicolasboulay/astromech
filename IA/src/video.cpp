#include"video.h"
#include<iostream>

#include<stdio.h> //perror()
#include<fcntl.h> //O_RDONLY
#include <sys/ioctl.h> // system d'appel ioctl

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <sys/mman.h> //mmap

using namespace cimg_library;


video_t::video_t(const std::string &  s, int h=120, int w=160)

{
  open_device(s,h,w);
}

void video_t::open_device(const std::string &  s, int h=120, int w=160)
{
  file_name=s;
  framebuf=NULL;
  printf("Try to open webcam ...\n");
  while(!_open(fd,s)) {sleep(1);}
  getparam();  
  height = h;
  width  = w;
  //close(fd);
  setmmap();
  setgrabframe();
  print_parameter();
}

video_t::~video_t()
{
  close_device();
  printf("Close %s\n",file_name.c_str());
}

void video_t::close_device()
{
  munmap(framebuf, width * height * 3);
  close(fd);
  if(!framebuf) free(framebuf);
}

cimg_library::CImg<unsigned char> * video_t::factory_img()
{
   CImg<unsigned char> * image= new CImg<unsigned char>(width,height,1,3,0);

   return image;
}

void video_t::reset_cycle()
{ /*broken !*/
  //  wait_frame();
  munmap(framebuf, width * height * 3);
  if(close(fd)) perror("reset_cyle:");
  //_open(fd,file_name);
  // print_parameter();
}

void video_t::reopen()
{
  _open(fd,file_name);
  setmmap();
  setgrabframe();
  print_parameter();
}


bool video_t::_open(int & _fd, const string & s) 
{
  struct stat st; 

  if (-1 == stat (s.c_str(), &st)) {
    //fprintf (stderr, "Cannot identify '%s': %d, %s\n",
	     //     s.c_str(), errno, strerror (errno));
    //exit (EXIT_FAILURE);
    return false;
  }

  if (!S_ISCHR (st.st_mode)) {
    fprintf (stderr, "%s is no device\n", s.c_str());
    //exit (EXIT_FAILURE);
    return false;
  }

  _fd = open (s.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

  cout << "open device :" << s << endl;

  if (-1 == _fd) {
    fprintf (stderr, "Cannot open '%s': %d, %s\n",
	     s.c_str(), errno, strerror (errno));
    //exit (EXIT_FAILURE);
    return false;
  }
  struct video_capability vcap;

  if(!get_video_capability(_fd,vcap))
    return false;

  printf ("Video Capture Device Name : %s\n", vcap.name);
  printf ("  %ix%i to %ix%i type=%i\n", 
	  vcap.minwidth,vcap.minheight,vcap.maxwidth,vcap.maxheight,vcap.type);
  return true;
}

bool video_t::get_video_capability(int file, struct video_capability & vcap)
{
  /* Get device capabilities */
  if (ioctl (file, VIDIOCGCAP, &vcap) < 0) {
    perror ("VIDIOCGCAP");
    return false;
  }
  return true;
}

const char * video_t::which_one_is_the_good_one(const char * name, const char * dev1,const char * dev2)
{
  const char * ret=NULL;
  int fd1;
  if(!_open(fd1,dev1)) {
    fprintf(stderr,"%s:",dev1);
    perror("open");
  }
  int fd2;
  if(!_open(fd2,dev2)) {
    fprintf(stderr,"%s:",dev2);
    perror("open");
  }

  struct video_capability vcap1;
  get_video_capability(fd1,vcap1);
  if(!strcmp(name,vcap1.name)){
    ret= dev1;
  }

  struct video_capability vcap2;
  get_video_capability(fd2,vcap2);
  if(!strcmp(name,vcap2.name)){
    ret= dev2;
  }

  printf("%s is in %s.\n", name,ret);

  close(fd1);
  close(fd2);
  return ret;
}
 

void video_t::setmmap()
{
  const unsigned abs_maxx=640;
  const unsigned abs_maxy=480;
  /* Mmap frame-buffer */
  if(!framebuf)  munmap(framebuf, width * height * 3);

  framebuf = (unsigned char*)
    mmap(0, (abs_maxx * abs_maxy * 3)/2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
  if ((unsigned char *)-1 == (unsigned char *)framebuf) {
    perror ("mmap");
    exit (EXIT_FAILURE);
  }
}

void video_t::setgrabframe()
{
  /* Get frame */
  mm.frame  = 0;
  mm.height = height;
  mm.width  = width;
  mm.format = palette;
  if (ioctl(fd, VIDIOCMCAPTURE, &(mm.frame)) < 0) {
    perror ("VIDIOCMCAPTURE");
    exit (EXIT_FAILURE);
  }
}

void video_t::wait_frame()
{
  mm.frame  = 0;
  mm.height = height;
  mm.width  = width;

  mm.format = palette;

  /* Wait frame to be completed */
  if (ioctl(fd, VIDIOCSYNC, &mm) < 0) {
    perror ("VIDIOCSYNC");
    exit (EXIT_FAILURE);
    }
}

/*to change size setting*/
void video_t::resetmmap()
{
  wait_frame();
  setmmap();
}

int video_t::set (int width_, int height_, int framerate_,
	 int brightness_,  int colour_, int contrast_,
	 int whiteness_, int palette_, int agc_)
{
  width  = width_ ;
  height =  height_;
  framerate =  framerate_;
  brightness=   brightness_;
  colour =  colour_;
  contrast=  contrast_;
  whiteness=  whiteness_;
  palette=  palette_;
  agc=  agc_;
  
  return 1;
}

void video_t::video_windows_set()
{
   struct video_window cam_win;
  
  cam_win.width = width;
  cam_win.height = height;
  if(ioctl(fd,VIDIOCSWIN,&cam_win) < 0){
    perror("video_t, video_window setting error");
  }
}

void video_t::commit()
{
  video_windows_set();
  struct video_picture cam_control;
  cam_control.palette = palette;
  cam_control.brightness = brightness;
  cam_control.colour = colour;
  cam_control.contrast = contrast;
  cam_control.whiteness = whiteness;
  if(ioctl (fd, VIDIOCSPICT, &cam_control)<0){
    perror("video_t, video_picture setting error");
  }
  resetmmap();
  setgrabframe();
  getparam();
}

void video_t::print_parameter()
{
  printf("%ix%i palette=%i brightness=%i colour=%i contrast=%i whiteness=%i\n",
	 width,height,palette,brightness,colour,contrast,whiteness);
}

void video_t::getparam()
{
  struct video_window cam_win;
  
  if(ioctl(fd,VIDIOCGWIN,&cam_win) < 0){
    perror("video_t, video_window getting error");
  }
  width  = cam_win.width;
  height = cam_win.height;

  struct video_picture cam_control;
  if(ioctl (fd, VIDIOCGPICT, &cam_control)<0){
    perror("video_t, video_picture getting error");
  }
  palette    = cam_control.palette;
  brightness = cam_control.brightness;
  colour     = cam_control.colour;
  contrast   = cam_control.contrast;
  whiteness  = cam_control.whiteness;
}

CImg<unsigned char> & video_t::grab_frame(CImg<unsigned char> & img) 
{

  wait_frame();
  yuv420P_to_rgb24(framebuf,img);
  setgrabframe();

  return img;
}

/*
  stupid convertion mess

*/

/*

C = Y - 16
D = U - 128
E = V - 128

Using the previous coefficients and noting that clip() denotes clipping 
a value to the range of 0 to 255, the following formulas provide the 
conversion from YUV to RGB:

R = clip(( 298 * C           + 409 * E + 128) >> 8)
G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
B = clip(( 298 * C + 516 * D           + 128) >> 8)
*/

inline int video_t::clip8(int i)
{
  if (i >= 255){
    return 255;
  } else if (i<=0){
    return 0;
  }
  return i;
}

/*
  Real formule for yuv -> rgb conversion

  "the defined range for Y is [16,235] (220 steps) and the valid ranges for Cr 
  and Cb are [16,239] (235 steps) These are normalized ranges and when the 
  data is used, 16 is added to Y and 128 is added from Cr and Cb to de-normalize 
  them. The reason for this is that control and colorburst information is stored 
  in the components along with the luminence and chromiance data which is why 
  the full range of [0,255] is not used. There is no "rumor" about this being 
  correct. It is a defined standard for YCrCb video. I wouldn't be surprised if 
  certain PC programs are using a variant where the entire range of [0,255] is 
  used but technically it is not YCrCb." http://www.fourcc.org/fccyvrgb.php

  Y     [16,235] for [0.0:1.0]
  U&V   [16,239] for [-0.5;+0.5]
*/

inline unsigned char * video_t::YUV444toRGB888_true(unsigned int Y, 
						     unsigned int U, 
						     unsigned int V,
						     unsigned char ret[3])
{
  int C, D, E;
  C = Y - 16;
  D = U - 128;
  E = V - 128;

  ret[0] = clip8(( 298 * C           + 409 * E + 128) >> 8);
  ret[1] = clip8(( 298 * C - 100 * D - 208 * E + 128) >> 8);
  ret[2] = clip8(( 298 * C + 516 * D           + 128) >> 8);

  return ret;
}

/*
  but webcam use Y[0;244] 
*/
inline unsigned char * video_t::YUV444toRGB888(unsigned int Y, 
						     unsigned int U, 
						     unsigned int V,
						     unsigned char ret[3])
{
  int C, D, E;
  C = Y;
  D = U - 128;
  E = V - 128;

  ret[0] = clip8(( 266 * C           + 409 * E + 128) >> 8);
  ret[1] = clip8(( 266 * C - 100 * D - 208 * E + 128) >> 8);
  ret[2] = clip8(( 266 * C + 516 * D           + 128) >> 8);

  return ret;
}

inline unsigned char * video_t::yuv420p_get(
					  unsigned char * yuv,
					  unsigned char ret[3],
					  unsigned w,
					  unsigned h)

{
  unsigned YArraySize = height * width;
  ret[0] = yuv[h * width + w];
  ret[1] = yuv[(h/2) * (width/2) + w/2 + YArraySize];
  ret[2] = yuv[(h/2) * (width/2) + w/2 + YArraySize + YArraySize/4];
  
  return ret;
}



//fonction de conversion yuv -> rgb pour affichage
void video_t::yuv420P_to_rgb24(unsigned char * yuv, 
			       CImg<unsigned char> & rgb)
{
  cimg_mapXY(rgb,w,h) 
    { 
      fsync(2);
      unsigned char YUV[3];
      yuv420p_get(yuv,YUV,w,h);
      unsigned char RGB[3];
      YUV444toRGB888(YUV[0],YUV[1],YUV[2],RGB);
      rgb(w,h,0)=RGB[0];
      rgb(w,h,1)=RGB[1];
      rgb(w,h,2)=RGB[2];

    }
}
