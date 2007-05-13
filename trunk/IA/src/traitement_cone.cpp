#include"traitement_cone.h"
#include "CImg.h"
#include <QVector>
#include <QList>
#include <iostream>
#include "common.h"
#include <cmath>
#include <cassert>
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
    QVector<complex<double> > beacons(255,0.0); // beacons 3,4,5,6 leds
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
    const unsigned char white[3] = {255,255,255};
    const unsigned char red[3] = {255,0,0};
    const unsigned char blue[3] = {0,0,255};
    const unsigned char green[3] = {0,255,0};
  
    for(int i=2;i<barys.size();i++){
      complex<double> c=barys[i];
      labels((int)c.real(),(int)c.imag(),1)=255;
      labels.draw_text((int)c.real(),(int)c.imag(),white,0,11,1,"%i",i);
    }
  

    int center_x=labels.dimx()/2;
    int center_y=labels.dimy()/2;

    labels.normalize(0,255)
      .draw_line(center_x,center_y,
		 (int)beacons[3].real()+center_x,(int)beacons[3].imag()+center_y,white)
      .draw_line(center_x,center_y,
		 (int)beacons[4].real()+center_x,(int)beacons[4].imag()+center_y,red)
      .draw_line(center_x,center_y,
		 (int)beacons[5].real()+center_x,(int)beacons[5].imag()+center_y,blue)
      .draw_line(center_x,center_y,
		 (int)beacons[6].real()+center_x,(int)beacons[6].imag()+center_y,green)
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

 void  traitement_cone_t::find_alignement(QVector<complex<double> > & all_barys, 
 					 QVector<int> & weight,
					  QVector<complex<double> >& beacons)
{
  complex<double> res;
  //double delta=5;
  QVector<complex<double> >  barys;

  //
  // elimination des taches trop grosses ou trop petite.
  //
  for(int i=2;i<weight.size();i++){
    //if(weight[i] > 1 && weight[i] < 50) //DEBUG pour utiliser les halogènes
      {
      barys.append(all_barys[i]);
    }
  }  
  
  //
  // Trouve les points colinéaires 
  //
  QList<QVector<int> > colin;
  QVector<bool> crible(barys.size(),false);

  for(int i=2;i<barys.size();i++){
    if(crible.at(i)) continue;      
    for(int j=i+1;j<barys.size();j++) {
      if(crible.at(j)) continue;      
      QVector<int> tmp(2,0);
      tmp[0]=i;
      tmp[1]=j;      
      for(int k=j+1;k<barys.size();k++) {
	//0.1 rad == 8° 
	double diff = fabs(fmod(arg(barys[i]-barys[j]),PI) 
	  - fmod(arg(barys[i]-barys[k]),PI));
	//colinéaire ?
	printf(" colinéarité: %i %i %i -> %f\n",i,j,k,diff);
	if(diff < 0.06) {
	  tmp.append(k);
	  crible[k]=true;
	}
      }
      if(tmp.size() !=2){
	crible[tmp.at(0)]=true;
	crible[tmp.at(1)]=true;
	colin.append(tmp);

	printf(" colin!");
	for(int jj=0;jj<tmp.size();jj++){
	  printf(" %i ",tmp.at(jj));
        }
        printf("\n");
      }
    }
  }
  cout << endl;
  //
  // Reconnaissance des balises 
  //
  colinear_vector_to_beacon(colin, barys,beacons);

//    for(int i=0;i<colin.size();i++){
//      int index = colin[i].size();
//      if (index > 6) index =6;
//      beacons[index]=largest_difference(colin[i],barys);
//    }

//    if(use_gui){
//      for(int i=0;i<colin.size();i++){
//        int index = colin[i].size();
//        printf("b %i: ",index);
//        for(int j=0;j<colin[i].size();j++){
// 	 printf("%i ",colin.at(i).at(j));
//        }
//        printf("\n");
//      }
//    }

  return;
}

// récupère une liste de point aligné et rend la place des balises
// En cas de plusieurs valeurs possibles, choix en fonction de l'écartement des points
//

void
traitement_cone_t::colinear_vector_to_beacon(const QList<QVector<int> > & colin,
					     const QVector<complex<double> > & barys,
					     QVector<complex<double> >& beacons)
{  

  double n_min; 
  double n_max;
  complex<double> diff_min;
  complex<double> diff_max;

  for(int i=0;i<colin.size();i++){
    // gestion du iéme alignement
    min_max_norm_of_colinear_vector(colin[i],barys,n_min,n_max,diff_min,diff_max);

    double ratio = n_max/n_min;
    double intpart;
    double fractpart = modf (ratio , &intpart);
    int nb_points = colin[i].size();

    cout << "identification [" << i << "]:" << n_min << "/"<< diff_max << " "  
	 << n_max << "/"<<diff_max << "("
	 << ratio << ":" << intpart << ":" << fractpart << ")" 
	 << endl; 

    if(fabs(ratio+(1-nb_points)) < 0.2){  // Guess: margin ?  
      // test si intpart est proche de colin[i]. size() ?
      beacons[(int) intpart+1]=diff_max;
      printf("validé beacon[%i]: %f (%i) %f\n", (int)intpart+1, fractpart, colin[i].size()
	     ,ratio+(1-nb_points));
    }
  }
  
  // for(int i=0;i<colin.size();i++){
//      int index = colin[i].size();
//      if (index > 6) index =6;
//      beacons[index]=largest_difference(colin[i],barys);
//    }

    if(use_gui){
      for(int i=0;i<colin.size();i++){
        int index = colin[i].size();
        printf("b %i: ",index);
        for(int j=0;j<colin[i].size();j++){
 	 printf("%i ",colin.at(i).at(j));
        }
        printf("\n");
      }
    }

}

void traitement_cone_t::min_max_norm_of_colinear_vector(const QVector<int> & index,
							const QVector<complex<double> >  & barys,
							double & _min, double & _max,
							complex<double> & c_min,complex<double> & c_max)
{

  double n_min=1000.0;
  double n_max=0.0;
  complex<double> v_min=0.0;
  complex<double> v_max=0.0;

  // récupération de la plus grande et de la plus petit norme
  for(int i=0;i<index.size();i++){
    for(int j=i+1;j<index.size();j++){
      complex<double> c=barys.at(index.at(i))-barys.at(index.at(j));
      double n = abs(c);
      if(n > n_max){
	v_max = c;
	n_max = n; 
      }
      if(n < n_min){
	v_min = c;
	n_min = n; 
      }
    }
  }

  _min = n_min;
  _max = n_max;
  c_min=v_min;
  c_max=v_max;
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

complex<double> 
traitement_cone_t::largest_difference(QVector<int> index,
				      QVector<complex<double> >  barys)
{
  double delta=0.0;
  complex<double> res=0.0;

  for(int i=0;i<index.size();i++){
    for(int j=i+1;j<index.size();j++){
      complex<double> c=barys.at(index.at(i))-barys.at(index.at(j));
      double n =abs(c);
      if(n > delta){
	res=c;
	delta=n;
      }
    }
  }

  return res;
}
