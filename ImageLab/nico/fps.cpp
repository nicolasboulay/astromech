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
    = cimg_option("-i","img/parrot_original.ppm","Input image");

  // Load an image, transform it to a color image (if necessary)
  CImg<unsigned char> image[2];

  image[0] = CImg<>(file_i).normalize(0,255).resize(-100,-100,1,3);
  image[1] = CImg<>(file_i).normalize(0,128).resize(-100,-100,1,3);

  
  // Create two display window, one for the image, the other for the color profile.
  CImgDisplay 
    main_disp(image[0],"Test de vitesse de lecture d'image",0);

  const unsigned char
    red  [3] = {255,  0,  0},
    green[3] = {  0,255,  0},
    blue [3] = {  0,  0,255},
    white[3] = {255,255,255},
    grey [3] = {128,128,128};

    
    int i=0;
  // Enter event loop. This loop ends when one of the two display window is closed.
  while (!main_disp.is_closed) {

    
    // Handle display window resizing (if any)
    if (main_disp.is_resized) main_disp.resize();

    main_disp << image[(i++)%2];

    float fps = main_disp.frames_per_second();

    CImg<unsigned char>(main_disp.dimx(),main_disp.dimy(),1,3,0)
      .draw_text(15,5,white,0,11,1,"%f fps ",fps)
      .display(main_disp);


    // Temporize event loop
    cimg::wait(1);
  }

  return 0;
}
