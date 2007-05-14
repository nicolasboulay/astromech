#ifndef VIDEO_PROCESSING_H
#define VIDEO_PROCESSING_H
#include "CImg.h"
#include <iostream>
#include <QVector>
#include <complex>

class video_processing_t 
{
 public:

  inline CImg<unsigned char> & seuillage(CImg<unsigned char>&  img, 
				  unsigned char seuil, int channel);
  inline CImg<unsigned char> & labellise(CImg<unsigned char>&  img, 
				  CImg<unsigned char>&  labels, int & nb_object,
				  int channel);
  inline void labellise_a_pixel(CImg<unsigned char>&  labelling,
				CImg<unsigned char>&  img,
				int x, int y, unsigned char label,int channel);
  inline void  
  give_barycentre(const CImg<unsigned char>&  labels, 
		  int nb_object,
		  QVector<complex<double> > & barys, 
		  QVector<int> & weight );
};


inline CImg<unsigned char> & video_processing_t::seuillage(CImg<unsigned char>&  img, 
							   unsigned char seuil, 
							   int channel)
{
  
  cimg_forXY(img,x,y)
    {
      if(img(x,y,channel)< seuil) img(x,y,channel) =0;
      if(img(x,y,(channel+1)%3)< seuil) img(x,y,(channel+1)%3) =0;
      if(img(x,y,(channel+2)%3)< seuil) img(x,y,(channel+2)%3) =0;
    }
  return img;
}


// Labellise img. The object are separate by black fields (0)
// labels : 0 <- back fields
// labels : 1 <- none proccessed pixel
// labels : n <- objects
inline CImg<unsigned char> & 
video_processing_t::labellise(CImg<unsigned char>&  img, 
			      CImg<unsigned char>&  labels, 
			      int & nb_object,
			      int channel)
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

inline void video_processing_t::labellise_a_pixel(CImg<unsigned char>&  labelling,
						  CImg<unsigned char>&  img,
						  int x, int y, unsigned char label,
						  int channel)
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
	  labellise_a_pixel(labelling,img,x,y-1,label,channel);
	  labellise_a_pixel(labelling,img,x-1,y,label,channel);
	  labellise_a_pixel(labelling,img,x+1,y,label,channel);
	  labellise_a_pixel(labelling,img,x,y+1,label,channel);

	  labellise_a_pixel(labelling,img,x-1,y-1,label,channel);
	  labellise_a_pixel(labelling,img,x+1,y-1,label,channel);
	  labellise_a_pixel(labelling,img,x+1,y+1,label,channel);
	  labellise_a_pixel(labelling,img,x-1,y+1,label,channel);

	} else {
	  //sinon c'est du back field
	  labelling(x,y)=0;
	}
      }
    }
}

inline void  
video_processing_t::give_barycentre(const CImg<unsigned char>&  labels, 
				    int nb_object,
				    QVector<complex<double> > & barys, 
				    QVector<int> & weight )
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
    //    cout << barys[i] << " ";
  }
  //cout << endl;
} 

#endif
