#include "eye_processing.h"
#include "video_processing.h"
#include "common.h"

//////////////////////////////
//for debugging:
CImgDisplay * raw_display=NULL;
CImgDisplay * filt_display=NULL;
CImgDisplay * label_display=NULL;
//////////////////////////////

eye_processing_t::eye_processing_t() : h(120),w(160), before_match_picture(w,h,1,3)
{
  if(use_gui){
      raw_display   = new CImgDisplay(640,480,"raw !");
      filt_display   = new CImgDisplay(640,480,"Filtred");
      label_display = new CImgDisplay (640,480,"After processing",1);
    }

  before_match_picture.fill(0);
}

eye_processing_t::~eye_processing_t()
{
  if(use_gui){
    delete raw_display; 
    delete label_display;
  }
}

void eye_processing_t::init_device()
{
  h=120*1;w=160*1;
  before_match_picture.resize(w,h);
  camera.open_device(video_device_eye,h,w);
  camera.fps=5;;
  camera.commit();
  camera.set_agc(0);
  // de 0 à ~16000 rien de spécial ensuite (c'est sombre), cela la luminosité augmenter jusqu'à 65000
  camera.set_shutter(50000); 
  camera.awb(PWC_WB_INDOOR);
  camera.compression(0);
  image_cur= camera.factory_img();
}

void eye_processing_t::close_device()
{
  delete  image_cur;
  camera.close_device();
}

const unsigned char white[3] = {255,255,255};

void eye_processing_t::grab_frame()
{
  camera.grab_frame(*image_cur);
}

void eye_processing_t::take_before_match_picture()
{
  camera.grab_frame(before_match_picture);
}

void eye_processing_t::panier_processing()
{
  // CImg<float> cur(*image_cur);
  //CImg<float> avant(before_match_picture);
  //CImg<float> res(cur-avant);

  video_processing_t pro;

  CImg<unsigned char> res(image_cur->dimx(),image_cur->dimy(),1,3);
  CImg<unsigned char> labels(image_cur->dimx(),image_cur->dimy(),1,3);

  kind_of_diff(res,*image_cur,before_match_picture);  
  pro.seuillage(res,30,1);
  res.erode(2).dilate(2);


  int nb_object; 
  pro.labellise(res,labels,nb_object,1);

  printf("%i\n",nb_object);
  

  float fps=0;
  if(use_gui){
    // Handle display window resizing (if any)
    if (label_display->is_resized) label_display->resize();
    if (raw_display->is_resized) raw_display->resize();

    //display raw from the webcam
    fps = raw_display->frames_per_second();
    image_cur->draw_text(15,5,white,0,11,1,"%f fps !",fps).display(*raw_display);  
    image_cur->display(*raw_display);

    //display processing
    labels.display(*label_display);
    res.display(*filt_display);

    //simulate the button pressed
    if (raw_display->is_pressed(cimg::keyP)){
      take_before_match_picture();
    }
  
  }
}

inline unsigned char eye_processing_t::kind_of_diff(unsigned char a,
				  unsigned char b)
{
  int tmp_a = a;
  int tmp_b = b;
  int tmp_res;
  unsigned char res;
  tmp_res = tmp_a - tmp_b;

  if(tmp_res < 0)
    tmp_res=-tmp_res;
 
  res = tmp_res;
  return res; 
}

void eye_processing_t::kind_of_diff(CImg<unsigned char> & res,
				    CImg<unsigned char> & a,
				    CImg<unsigned char> & b)
{
  cimg_forXY(a,x,y)
    {
      unsigned short tmp = 
	(kind_of_diff(a(x,y,0),b(x,y,0))
	+ kind_of_diff(a(x,y,1),b(x,y,1)) +
	 kind_of_diff(a(x,y,2),b(x,y,2)))/3;
      res(x,y,0)=tmp;
      res(x,y,1)=tmp;
      res(x,y,2)=tmp;
    }
}

