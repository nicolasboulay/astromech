#include "/usr/src/linux/drivers/media/video/pwc/pwc-ioctl.h"
#include <sys/ioctl.h>
#include "video-pwc.hpp"
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

void video_pwc_t::print_parameter()
{
  printf("%ix%i palette=%i brightness=%i colour=%i contrast=%i whiteness=%i agc=%i shutter speed=%i\n",
	 width,height,palette,brightness,colour,contrast,whiteness,agc,shutter_speed);
}
