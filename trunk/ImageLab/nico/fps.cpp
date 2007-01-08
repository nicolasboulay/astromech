// Include CImg library file and use its main namespace
#include "CImg.h"
#include "video.hpp"
using namespace cimg_library;

int main(int argc,char **argv) {

  // Define program usage and read command line parameters
  //-------------------------------------------------------

  // Display program usage, when invoked from the command line with option '-h'.
  cimg_usage("View the color profile of an image along the X axis");

  // Read image filename from the command line (or set it to 
  //  "img/parrot_original.ppm" if option '-i' is not provided).
  const char* file_i 
    = cimg_option("-i","../capture.bmp","Input image");

  const char * video_i
    = cimg_option("-d","/dev/video","Input device");

  // Load an image, transform it to a color image (if necessary)
  CImg<unsigned char> image= CImg<>(file_i).normalize(0,128);

  
  // Create two display window, one for the image, the other for the color profile.
  CImgDisplay 
    main_disp(image,"Test de vitesse de lecture d'image",0);

  const unsigned char
    red  [3] = {255,  0,  0},
    green[3] = {  0,255,  0},
    blue [3] = {  0,  0,255},
    white[3] = {255,255,255},
    grey [3] = {128,128,128};

    video_t camera(video_i);
    
    int i=0;
  // Enter event loop. This loop ends when one of the two display window is closed.
  while (!main_disp.is_closed) {

    
    // Handle display window resizing (if any)
    if (main_disp.is_resized) main_disp.resize();

    //main_disp << image[(i++)%2];
    //camera.grab_frame().display(main_disp);
    float fps = main_disp.frames_per_second();
    camera.grab_frame()
      .draw_text(15,5,white,0,11,1,"%f fps ",fps)
      .display(main_disp);


    // Temporize event loop
    cimg::wait(1);
  }

  return 0;
}
