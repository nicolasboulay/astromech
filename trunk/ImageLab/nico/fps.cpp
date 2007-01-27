// Include CImg library file and use its main namespace
#include "CImg.h"
#include "video.hpp"
using namespace cimg_library;


int main(int argc,char **argv) {

  // Define program usage and read command line parameters
  //-------------------------------------------------------

  // Display program usage, when invoked from the command line with option '-h'.
  cimg_usage("Display a video from video 4 linux driver");

  // Read device filename from the command line (or set to /dev/video)"

  const char * video_i
    = cimg_option("-d","/dev/video","Input device");
  
  CImgDisplay 
    main_disp(160,120,"Test de vitesse de lecture d'image",1);
  CImg<unsigned char> image= CImg<>(160,120,3,1,0);

  video_t camera(video_i);
  
  const unsigned char white[3] = {255,255,255};
    
  // Enter event loop. This loop ends when one of the two display window is closed.
  while (!main_disp.is_closed) {
    
    // Handle display window resizing (if any)
    if (main_disp.is_resized) main_disp.resize();

    float fps = main_disp.frames_per_second();
    //draw_text(15,5,white,0,11,1,"%f fps ",fps)
    camera.grab_frame(image).display(main_disp);

    // Temporize event loop
    cimg::wait(1);
  }

  return 0;
}
