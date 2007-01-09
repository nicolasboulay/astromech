#include"video.hpp"
#include<iostream>

#include<stdio.h> //perror()
#include<fcntl.h> //O_RDONLY
#include <sys/ioctl.h> // system d'appel ioctl

#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <sys/mman.h> //mmap

using namespace cimg_library;

video_t::video_t(const std::string &  s)

{
  height = 120;//120
  width  = 160;//160
  framerate = 60;
  img[0]= new CImg<unsigned char>(width,height,1,3,100);
  img[1]= new CImg<unsigned char>(width,height,1,3,0);

  video_t::_open(s);
} 

video_t::~video_t()

{
  delete img[0];
  delete img[1];
  close(fd);
}

void video_t::_open(const string & s) 
{
  struct stat st; 

  if (-1 == stat (s.c_str(), &st)) {
    fprintf (stderr, "Cannot identify '%s': %d, %s\n",
	     s.c_str(), errno, strerror (errno));
    exit (EXIT_FAILURE);
  }

  if (!S_ISCHR (st.st_mode)) {
    fprintf (stderr, "%s is no device\n", s.c_str());
    exit (EXIT_FAILURE);
  }

  fd = open (s.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

  cout << "open device :" << s << endl;

  if (-1 == fd) {
    fprintf (stderr, "Cannot open '%s': %d, %s\n",
	     s.c_str(), errno, strerror (errno));
    exit (EXIT_FAILURE);
  }

  /* Get device capabilities */
  if (ioctl (fd, VIDIOCGCAP, &vcap) < 0) {
    perror ("VIDIOCGCAP");
    exit (EXIT_FAILURE);
  }

  printf ("Video Capture Device Name : %s\n", vcap.name);

  /* Mmap frame-buffer */
  framebuf = (unsigned char*)
    mmap(0, width * height * 3, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  print_parameter();
  if ((unsigned char *)-1 == (unsigned char *)framebuf) {
    perror ("mmap");
    exit (EXIT_FAILURE);
  }

  /* Get frame */
  mm.frame  = 0;
  mm.height = height;
  mm.width  = width;
  mm.format = VIDEO_PALETTE_YUV420P;
  if (ioctl(fd, VIDIOCMCAPTURE, &mm) < 0) {
    perror ("VIDIOCMCAPTURE");
    exit (EXIT_FAILURE);
  }

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

void video_t::print_parameter(void)
{
  printf("%ix%i %ifps\n", height, width, framerate);

}

CImg<unsigned char> & video_t::grab_frame(void) 
{
  mm.frame  = 0;
  mm.height = height;
  mm.width  = width;

  mm.format = VIDEO_PALETTE_YUV420P;

  /* Wait frame to be completed */
  if (ioctl(fd, VIDIOCSYNC, &mm.frame) < 0) {
    perror ("VIDIOCSYNC");
    exit (EXIT_FAILURE);
    }

  yuv420P_to_rgb24(framebuf,*img[0]);

  /* Get frame */
  if (ioctl(fd, VIDIOCMCAPTURE, &mm) < 0) {
    perror ("VIDIOCMCAPTURE");
    exit (EXIT_FAILURE);
  }

  return * img[0];
}


/* Go from RGB (red first) to 4:2:0 planar.
 * Note: this requires decimation of the U/V space by 2 in both directions
 * Also, a matrix multiply would be QUITE convenient...

   This is the matrix:
     (Y )   ( 77  150   29)   (R)
     (Cb) = (-43  -85  128) * (G)
     (Cr)   (128 -107  -21)   (B)
 */


//fonction de conversion yuv -> rgb pour affichage
void video_t::yuv420P_to_rgb24(
			       unsigned char * yuv, 
			       unsigned char * rgb, 
			       const unsigned width, 
			       const  unsigned height)
{
  unsigned int y,u,v,w,h;

 for(w=0;w<width;w=w+1)
   for(h=0; h<height ;h=h+1)
   {
     y=w+h*width;
     u=width*height+(w/2+ (h/2)*(width/2));
     v=u+(width/2)*(height/2);

     rgb[3*w+h*3*width]=abs(yuv[y] + 1.4075*(yuv[v]-128));
     rgb[1+3*w+h*3*width]=abs(yuv[y]- 0.344*(yuv[u]-128) - 0.7169*(yuv[v]-128));
     rgb[2+3*w+h*3*width]=abs(yuv[y]+ 1.779*(yuv[u]-128));

   }

}
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

inline unsigned int video_t::clip8(unsigned int i)
{
  if (i >= 255)
    return 255;
  return i;
}

inline unsigned char * video_t::YUV444toRGB888(unsigned int Y, 
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


/*
B = 1.164(Y - 16)                   + 2.018(U - 128)
G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
R = 1.164(Y - 16) + 1.596(V - 128)*/

//fonction de conversion yuv -> rgb pour affichage
void video_t::yuv420P_to_rgb24(unsigned char * yuv, 
			       CImg<unsigned char> & rgb)
{
  cimg_mapXY(rgb,w,h) 
    { 
      unsigned char YUV[3];
      yuv420p_get(yuv,YUV,w,h);
      unsigned char RGB[3];
      YUV444toRGB888(YUV[0],YUV[1],YUV[2],RGB);
      rgb(w,h,0)=RGB[0];
      rgb(w,h,1)=RGB[1];
      rgb(w,h,2)=RGB[2];
    }
}
