#ifndef TRAITEMENT_CONE_H
#define TRAITEMENT_CONE_H
#include <QThread>
#include <QMutex>
#include "video-pwc.h"
#include <complex>
using namespace cimg_library;

class traitement_cone_t  : public QThread
{
  Q_OBJECT 
  
 public:
  void run();
  traitement_cone_t();
 private:

  CImg<unsigned char> & seuillage(CImg<unsigned char>&  image, unsigned char seuil);
  CImg<unsigned char> & find_the_4_beacons(CImg<unsigned char>&  img, 
					   QVector<complex<double> > &beacons);
  void labellise_a_pixel(CImg<unsigned char>&  labelling,
			 CImg<unsigned char>&  img,
			 int x, int y, unsigned char label,int channel);
  CImg<unsigned char> & labellise(CImg<unsigned char>&  img, 
				  CImg<unsigned char>&  labels, int & nb_object,
				  int channel);

void give_barycentre(CImg<unsigned char>&  labels, int nb_object,
		     QVector<complex<double> > & barys,  QVector<int> & weight);
 void  find_alignement(QVector<complex<double> > & barys, 
		       QVector<int> & weight,QVector<complex<double> >& beacons);
complex<double>  largest_difference(complex<double> a, 
				    complex<double> b, 
				    complex<double> c);
 complex<double> largest_difference(QVector<int> index,
				    QVector<complex<double> >  barys);


 void colinear_vector_to_beacon(const QList<QVector<int> > & colin,
				const QVector<complex<double> > & barys,
				QVector<complex<double> >& beacons);

 void min_max_norm_of_colinear_vector(const QVector<int> & index,
				      const QVector<complex<double> >  & barys,
				      double & _min, double & _max,
				      complex<double> & c_min,complex<double> & c_max);

  QMutex mutex;
  //video_t camera;
};


#endif
