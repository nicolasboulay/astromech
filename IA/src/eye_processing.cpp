#include "eye_processing.h"
#include "video_processing.h"
#include "common.h"
#include <cmath>

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
  camera.set_shutter(52000); 
  camera.awb(PWC_WB_INDOOR);
  camera.compression(0);
  image_cur= camera.factory_img();
}

void eye_processing_t::close_device()
{
  delete  image_cur;
  camera.close_device();
}

const unsigned char white[3] = {250,250,250};

void eye_processing_t::grab_frame()
{
  camera.grab_frame(*image_cur);
}

void eye_processing_t::take_before_match_picture()
{
  camera.grab_frame(before_match_picture);
}

void eye_processing_t::panier_processing(QVector<float> & proba_panier)
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

  QVector<complex<double> > barys(nb_object+2,0.0);
  QVector<int> weight(nb_object+2,0);

  label_to_panier(labels,nb_object,proba_panier, barys, weight );
  
  float fps=0;
  if(use_gui){
    // Handle display window resizing (if any)
    if (label_display->is_resized) label_display->resize();
    if (raw_display->is_resized) raw_display->resize();

    //display raw from the webcam
    fps = raw_display->frames_per_second();
    image_cur->draw_text(15,5,white,0,11,1,"%f fps !",fps).display(*raw_display);  
    image_cur->display(*raw_display);

    res.display(*filt_display);

    //display processing    
    for(int i=2;i<barys.size();i++){
      complex<double> c=barys[i];
      labels((int)c.real(),(int)c.imag(),1)=255;
      labels.draw_text((int)c.real(),(int)c.imag(),white,0,11,1,"%i",i);
    }
    
    labels.display(*label_display);


    //simulate the button pressed
    if (raw_display->is_pressed(cimg::keyP)){
      take_before_match_picture();
    }
  
  }

}

inline void eye_processing_t::label_to_panier(const CImg<unsigned char> & labels, int nb_object,
					      QVector<float> & proba_panier,
					      QVector<complex<double> > & barys,
					      QVector<int> & weight )
{
  video_processing_t pro;
  pro.give_barycentre(labels, nb_object, barys, weight);
  
  //TODO déterminer les zones interrescantes en fonction des tests

//   for(int i=2;i<barys.size();i++){
//     if(barys[i].real())
//   }
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


void eye_processing_t::target_processing()
{
  video_processing_t pro;

  CImg<unsigned char> res(*image_cur);

  CImg<unsigned char> labels(image_cur->dimx(),image_cur->dimy(),1,3);


//   int x=image_cur->dimx()/2;
//   int y=(image_cur->dimy()*3)/4;
//   QVector<unsigned char> color(3,0);

//   extract_floor_color( before_match_picture, color,
// 		       x-30, y-15,
// 		       x+30, y+15);
//   soustract_color(res,color);


  pro.seuillage(res,30,1);
  // jaune des cannettes [65:68]
  //keep_one_color(res,340.0/360.0,10.0/360.0);
  //res.erode(2).dilate(2);

  derivateY(res);
  int nb_object;
 
  pro.labellise(res,labels,nb_object,1);

  QVector<complex<double> > barys(nb_object+2,0.0);
  QVector<int> weight(nb_object+2,0);
  pro.give_barycentre(labels, nb_object, barys, weight);
  
  //  res.draw_rectangle(x+30,y-15,x-30,y+15,white,0.4);
  
  float fps=0;
  if(use_gui){
    // Handle display window resizing (if any)
    if (label_display->is_resized) label_display->resize();
    if (raw_display->is_resized) raw_display->resize();

    //display raw from the webcam
    fps = raw_display->frames_per_second();
    image_cur->draw_text(15,5,white,0,11,1,"%f fps !",fps).display(*raw_display);  
    image_cur->display(*raw_display);

    res.display(*filt_display);

    //display processing    
    for(int i=2;i<barys.size();i++){
      complex<double> c=barys[i];
      labels((int)c.real(),(int)c.imag(),1)=255;
      labels.draw_text((int)c.real(),(int)c.imag(),white,0,11,1,"%i",i);
    }
    
    labels.display(*label_display);

    //simulate the button pressed
    if (raw_display->is_pressed(cimg::keyP)){
      take_before_match_picture();
      printf("p!\n");
    }
  
  }

}

// enlever dans l'image la couleur de moyenne du bas de l'image de départ
void eye_processing_t::extract_floor_color( const CImg<unsigned char> & img, 
			  QVector<unsigned char> & color,
			  int x1, int y1,
			  int x2, int y2)
{

  int i=0;
  int c0=0,c1=0,c2=0;
  for(int x = x1; x !=x2; x++)
    for(int y = y1; y !=y2; y++){

      i++;
      c0 +=img(x,y,0);
      c1 +=img(x,y,1);
      c2 +=img(x,y,2);
    }

  color[0] =c0/i;
  color[1] =c1/i;
  color[2] =c2/i;
  
  printf("%i:%i %i:%i -> [%i;%i;%i]\n",x1,y1,x2,y2, color[0],color[1],color[2]);
}

inline unsigned char eye_processing_t::abs_diff(unsigned char a,
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


inline void eye_processing_t::soustract_color(CImg<unsigned char> & img,
				       QVector<unsigned char> & color)
{
  cimg_forXY(img,x,y){
    if(abs_diff(img(x,y,0),color.at(0)) < 20)
      if(abs_diff(img(x,y,1),color.at(1))<20)
	if(abs_diff(img(x,y,2),color.at(2)) < 20){
	  img(x,y,0)=0;
	  img(x,y,1)=0;
	  img(x,y,2)=0;
	}
    
  }
}

inline void eye_processing_t::keep_one_color(CImg<unsigned char> & in,
					     float color,
					     float seuil)
{
  CImg<float> img(in);

  img.RGBtoHSV();

  cimg_forXY(img,x,y){
    printf("%f %f %f\n",img(x,y,0),img(x,y,1),img(x,y,2));
    //test du hue
    if(fabs(img(x,y,0)-color) > seuil){
      in(x,y,0)=0;
      in(x,y,1)=0;
      in(x,y,2)=0;
    }
  }
    
  
}


inline void eye_processing_t::derivateY(CImg<unsigned char> & in)

{
  CImg<unsigned char> img(in);

  for(int x = 0;x < img.dimx();x++)
    for(int y = img.dimy()-2 ;y !=0;y--){
  
//     in(x,y,0) = abs_diff(img(x,y,0),img(x,y+1,0)); 
//     in(x,y,1) = abs_diff(img(x,y,1),img(x,y+1,1)); 
//     in(x,y,2) = abs_diff(img(x,y,2),img(x,y+1,2)); 

      int tmp = abs_diff(img(x,y,0),img(x,y+1,0))
	+ abs_diff(img(x,y,1),img(x,y+1,1))
	+ abs_diff(img(x,y,2),img(x,y+1,2)); 
      if(tmp > 100){
	in(x,y,0)=255;
	break; // fini pour cette colonne
      }
    
  }
  
}
