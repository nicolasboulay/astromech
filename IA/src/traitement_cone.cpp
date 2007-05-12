#include"traitement_cone.h"
#include "CImg.h"
#include <QVector>
#include <iostream>
#include "common.h"
#include <cmath>
using namespace cimg_library;

traitement_cone_t::traitement_cone_t() //: camera("/dev/video",480/4,640/4)
{
  // /!\ ce n'est pas le thread de traitement ici.
}

//for debugging:
  CImgDisplay * label_disp;

void traitement_cone_t::run(){
  
 video_pwc_t camera("/dev/video",480,640);

 //for debuging purpose:
 CImgDisplay * main_disp=NULL;
 CImgDisplay * raw_disp=NULL;
 
 if(use_gui){
   main_disp  = new CImgDisplay (640,480,"Webcam cone !",1);

   raw_disp   = new CImgDisplay(640,480,"raw !",1);
   label_disp = new CImgDisplay (640,480,"label !",1);
 }
  const unsigned char white[3] = {255,255,255};
  
  camera.fps=5;;
  camera.commit();
  camera.set_agc(0);
  camera.set_shutter(10000); // de 0 à ~16000 rien de spécial ensuite, cela la luminosité augmenter jusqu'à 32000
  camera.awb(PWC_WB_INDOOR);
  camera.compression(3);

  CImg<unsigned char> * image= camera.factory_img();

  // Enter event loop. This loop ends when one of the two display window is closed.
  forever {

    
    // Handle display window resizing (if any)
    float fps=0;
    if(use_gui){
      if (main_disp->is_resized) main_disp->resize();
      if (raw_disp->is_resized) raw_disp->resize();
    
      fps = main_disp->frames_per_second();
    }
    //
    camera.grab_frame(*image);
    if(use_gui){
      image->display(*raw_disp);
    }
    //seuillage(*image);
    QVector<complex<double> > beacons(6+1,0.0); // beacons 3,4,5,6 leds
    seuillage(*image,220);
    find_the_4_beacons(*image,beacons);
    if(use_gui){
      image->draw_text(15,5,white,0,11,1,"%f fps !",fps).display(*main_disp);
    }
      camera.getparam();
      camera.print_parameter();
   
    // Temporize event loop
    cimg::wait(1);
  }

 if(use_gui){
   delete main_disp;

   delete raw_disp; 
   delete label_disp;
 }

}

//!!!! seuil sur le rouge !!!
CImg<unsigned char> & traitement_cone_t::seuillage(CImg<unsigned char>&  img, unsigned char seuil)
{
  
  cimg_forXY(img,x,y)
    {
      if(img(x,y,0)< seuil) img(x,y,0) =0;
      img(x,y,1)=0;
      img(x,y,2)=0;
    }
  return img;
}


// Labellise img. The object are separate by black fields (0)
// labels : 0 <- back fields
// labels : 1 <- none proccessed pixel
// labels : n <- objects
CImg<unsigned char> & traitement_cone_t::labellise(CImg<unsigned char>&  img, 
						   CImg<unsigned char>&  labels, int & nb_object,
						   int channel = 0)
{
  labels.fill(1);
  unsigned char label=2;
   cimg_for_insideXY(img,x,y,1){     
     if((img(x,y,channel) != 0)){ // object
       if(labels(x,y) == 1){ // pas encore labelisé
	 labellise_a_pixel(labels,img,x,y,label,channel);
	 label+=1;
	 if(label == 255) goto error;
       }
     } else { // pas un objet
       labels(x,y)=0;
     }
   }
 
   nb_object=label-2;
   return img;
 error:
   nb_object=255;
   return img;
}

void traitement_cone_t::labellise_a_pixel(CImg<unsigned char>&  labelling,
					  CImg<unsigned char>&  img,
					  int x, int y, unsigned char label,int channel=0)
{
  // x,y doivent être valide
  if(
     (x >= 0) && (y >= 0) && 
     (x < img.dimx()) &&
     (y < img.dimy())
     )
    {
      if(labelling(x,y) == 1){ // si non labelisé
	if(img(x,y,channel) != 0){   // si non back field
	  labelling(x,y)=label; // on labelise
	  //et on traite les voisins
	  labellise_a_pixel(labelling,img,x,y-1,label);
	  labellise_a_pixel(labelling,img,x-1,y,label);
	  labellise_a_pixel(labelling,img,x+1,y,label);
	  labellise_a_pixel(labelling,img,x,y+1,label);

	  labellise_a_pixel(labelling,img,x-1,y-1,label);
	  labellise_a_pixel(labelling,img,x+1,y-1,label);
	  labellise_a_pixel(labelling,img,x+1,y+1,label);
	  labellise_a_pixel(labelling,img,x-1,y+1,label);

	} else {
	  //sinon c'est du back field
	  labelling(x,y)=0;
	}
      }
    }
}

// beacon return angle in °
CImg<unsigned char> & traitement_cone_t::find_the_4_beacons(CImg<unsigned char>&  img, 
							    QVector<complex<double> > & beacons)
{
  CImg<unsigned char> labels(img.dimx(),img.dimy(),1,3);
  
  int nb_object=0;
  ////
  //
  labellise(img,labels,nb_object,0);
  //
  ////

  QVector<complex<double> > barys(nb_object+2,0.0);
  QVector<int> weight(nb_object+2,0);

  /////
  //
  give_barycentre(labels, nb_object, barys,weight);
  //
  ////

  ////
  //
  find_alignement(barys,weight, beacons);
  //
  ///

  //////////////////////////////////////////////////////
  //
  // display for debugging
  //
  if(use_gui){
  
    for(int i=2;i<barys.size();i++){
      complex<double> c=barys[i];
      labels((int)c.real(),(int)c.imag(),1)=weight[i];
    }
  
    const unsigned char white[3] = {255,255,255};

    int center_x=labels.dimx()/2;
    int center_y=labels.dimy()/2;

    labels.normalize(0,255)
      .draw_line(center_x,center_y,
		 (int)beacons[3].real()+center_x,(int)beacons[3].imag()+center_y,white)
      .draw_text(15,5,white,0,11,1,"%i obj !",nb_object).display(*label_disp);
  }
  ////////////////////////////////////////////////////////
  return img;
}


void  traitement_cone_t::give_barycentre(CImg<unsigned char>&  labels, int nb_object,
		     QVector<complex<double> > & barys, QVector<int> & weight )
{
  weight.resize(nb_object+2);
  barys.resize(nb_object+2);
  complex<double> J(0.0,1.0);


  cimg_for_insideXY(labels,x,y,1){
    //cout << nb_object <<" " << (int)labels(x,y) << " " <<(double)x+(double)y*J << endl;
    int index =labels(x,y);
      barys[index] +=(double)x+(double)y*J;
      weight[index]+=1;
  }  

  for(int i=0;i<barys.size();i++){
    barys[i]/=weight[i];
    cout << barys[i] << " ";
  }
  cout << endl;
} 

const double PI = 3.14159265358979323846;

 void  traitement_cone_t::find_alignement(QVector<complex<double> > & barys, 
 					 QVector<int> & weight,
					  QVector<complex<double> >& beacons)
{
  complex<double> res;
  double delta=5;
  for(int i=2;i<barys.size();i++){
    for(int j=i+1;j<barys.size();j++){
      for(int k=j+1;k<barys.size();k++){
// 	printf("(%i:%i:%i) %f %f %f / %f %f %f\n",i,j,k,
// 	       fmod(arg(barys[i]-barys[j]),PI),
// 	       fmod(arg(barys[i]-barys[k]),PI),
// 	       fmod(arg(barys[j]-barys[k]),PI),
// 	       abs(barys[i]-barys[j]),
// 	       abs(barys[i]-barys[k]),
// 	       abs(barys[j]-barys[k])
// 	       );

	//0.1 rad == 8° 
	double diff = fmod(arg(barys[i]-barys[j]),PI) 
	  - fmod(arg(barys[i]-barys[k]),PI);
	if(diff < 0.1) {
	  if(diff < delta){
	    delta=diff;
	    res=largest_difference(barys[i]-barys[j],
				   barys[j]-barys[k],
				   barys[i]-barys[k]);
	  }
	}
      }
      //      cout << endl; 
    }
  }

  beacons[3]=res;
  return;
}

complex<double> 
traitement_cone_t::largest_difference(complex<double> a, 
				      complex<double> b, 
				      complex<double> c)
{
  complex<double> c1=a-b;
  complex<double> c2=b-c;
  complex<double> c3=a-c;
  complex<double> res;
  double n1=abs(c1);
  double n2=abs(c2);
  double n3=abs(c3);

  res = n1;
  if(n1 < n2){
    res = c2; 
  }
  if(n2 <n3 ){
    res=c3;
  }
  return res;
}
