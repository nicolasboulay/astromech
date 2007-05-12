
#include <sys/ioctl.h>
#include "video-pwc.h"
#include <errno.h>

void video_pwc_t::video_windows_set()
{
   struct video_window cam_win;
  
  cam_win.width = width;
  cam_win.height = height;

  /* Set new framerate */
  cam_win.flags &= ~PWC_FPS_FRMASK;
  cam_win.flags |= (fps << PWC_FPS_SHIFT);

  if(ioctl(fd,VIDIOCSWIN,&cam_win) < 0){
    perror("video_pwc_t, video_window setting error");
  }

  if(ioctl(fd,VIDIOCGWIN,&cam_win) < 0){
    perror("video_t, video_window getting error");
  }
  fps = (cam_win.flags & PWC_FPS_FRMASK) >> PWC_FPS_SHIFT;
}

int video_pwc_t::set_noise_filter(int i)
{
  if(ioctl(fd,VIDIOCPWCSDYNNOISE,&i) < 0){
    perror("video_pwc_t, set_noise_filter setting error");
  }
  return errno;
}

int video_pwc_t::set_agc(int i)
{
  agc=i;
  if(ioctl(fd,VIDIOCPWCSAGC,&agc) < 0){
    perror("video_pwc_t, agc setting error");
  }
  return errno;
}

int video_pwc_t::get_agc(int & i)
{
  if(ioctl(fd,VIDIOCPWCSAGC,&agc) < 0){
    perror("video_pwc_t, agc setting error");
  }
  i=agc;
  return errno;
}

int video_pwc_t::awb(int mode){
  struct pwc_whitebalance _awb={0,0,0,0,0};
  _awb.mode=mode;
  if(ioctl(fd,VIDIOCPWCSAWB,&_awb) < 0){
    perror("video_pwc_t, awb getting error");
  }
  
  return awb();
}

int video_pwc_t::awb()
{
  struct pwc_whitebalance _awb={0,0,0,0,0};
  if(ioctl(fd,VIDIOCPWCGAWB,&_awb) < 0){
    perror("video_pwc_t, awb setting error");
  }
  return _awb.mode;
}

int video_pwc_t::compression(int c)
{
  
  if(ioctl(fd,VIDIOCPWCSCQUAL,&c) < 0){
    perror("video_pwc_t, compression setting error");
  }
  return compression();
}


int video_pwc_t::compression()
{
  if(ioctl(fd,VIDIOCPWCGCQUAL,&_compression) < 0){
    perror("video_pwc_t, compression getting error");
  }
  return _compression;
}


int video_pwc_t::set_shutter(int i)
{
  shutter_speed=i;
  if(ioctl(fd,VIDIOCPWCSSHUTTER,&shutter_speed) < 0){
    perror("video_pwc_t, shutter setting error");
  }
  i=shutter_speed;
  return errno;
}

void video_pwc_t::print_parameter()
{
  printf("%ix%i@%i palette=%i brightness=%i colour=%i contrast=%i whiteness=%i agc=%i shutter=%i qual=%i\n",
	 width,height,fps,palette,brightness,colour,contrast,whiteness,agc,shutter_speed,_compression);
}
