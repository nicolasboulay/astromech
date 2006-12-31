#include"video.hpp"
#include<iostream>

#include<stdio.h> //perror()
#include<fcntl.h> //O_RDONLY
//#include </usr/src/linux/drivers/usb/media/pwc/pwc-ioctl.h>
#include <linux/videodev.h> //drivers video4linux
#include <sys/ioctl.h> // system d'appel ioctl


//#include<stdlib.h> //open()
#include <sys/types.h>
#include <sys/stat.h>

//using namespace std;

video_t::video_t(const string &  s)
{
  video_t::_open(s);
} 

void video_t::_open(const string & s) 
{
  fd = open(s.c_str(), O_RDONLY);
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

void video_t::commit()
{
  struct video_window cam_win;
  
  cam_win.width = width;
  cam_win.height = height;
  //cam_win.flags = framerate << PWC_FPS_SHIFT; 
  if(ioctl(fd,VIDIOCSWIN,&cam_win) < 0){
    perror("video_t, video_window setting error");
  }

  struct video_picture cam_control;
  cam_control.palette = palette;
  cam_control.brightness = brightness;
  //  cam_control.hue = hue;
  cam_control.colour = colour;
  cam_control.contrast = contrast;
  cam_control.whiteness = whiteness;
  if(ioctl (fd, VIDIOCSPICT, &cam_control)<0){
    perror("video_t, video_picture setting error");
  }

  //if(ioctl(fd, VIDIOCPWCSAGC, &agc)<0){
  //  perror("video_t, agc setting error");
  //}
}

void video_t::fetch()
{
  struct video_window cam_win;
  
  if(ioctl(fd,VIDIOCGWIN,&cam_win) < 0){
    perror("video_t, video_window getting error");
  }
  width  = cam_win.width;
  height = cam_win.height;
  //framerate = cam_win.flags >> PWC_FPS_SHIFT; 


  struct video_picture cam_control;
  if(ioctl (fd, VIDIOCGPICT, &cam_control)<0){
    perror("video_t, video_picture getting error");
  }
  palette    =cam_control.palette;
  brightness =cam_control.brightness;
  //  cam_control.hue = hue;
  colour = cam_control.colour;
  contrast = cam_control.contrast;
  whiteness = cam_control.whiteness;

  //if(ioctl(fd, VIDIOCPWCGAGC, &agc)<0){
  //  perror("video_t, agc getting error");
  //}
}
