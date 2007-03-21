#ifndef Geom2DH
#define Geom2DH
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
class TiXmlElement;
class Segment2D;
typedef vector<Segment2D *> vectorSegment2D;

double max(double x, double y);
double min(double x, double y);
double square(double x);
void normalise0_360_100eme_deg(double & angle_deg);
void normalise0_2PI(double & angle_rad);
void normaliseMPI_PPI(double & angle_rad);
double DistanceAngulaire(double AngleA, double AngleB);

class Point2D
{
  friend class Segment2D;
  friend class Boite2D;
  friend class WayPoint;
  protected: //accessible uniquement par héritage
    
    
  public : // accessible partout
  static int nbPtCrees;
  static int nbPtPresents; 
  double x;
    double y;
    Point2D(TiXmlElement* pPt);
    Point2D(double x, double y);
    Point2D(Point2D * pt);
    ~Point2D(void);
    double getX(void){return x;};
    double getY(void){return y;};
    void setXY(double X, double Y){x=X;y=Y;};
    double distanceAuPoint(Point2D * pt);
    Point2D * creePointRT(Point2D * centreGraviteRT, double * att_rad);
    void calculePointDansNouveauRepere(Point2D * origineRepere,double orientationRepere_rad);
    void calculePointDansRepereTerrain(Point2D * origineRepere,double orientationRepere_rad);
    double calculeCap_rad(Point2D * ptDest);
};

class Segment2D
{
  friend class Boite2D;
  protected: //accessible uniquement par héritage
        
  public : // accessible partout
  Point2D * pt1;
    Point2D * pt2;

    Segment2D(TiXmlElement* pSeg);
    Segment2D(Point2D * pt1, Point2D * pt2);
    ~Segment2D(void);
    Segment2D * creeSegmentRT(Point2D * centreGraviteRT, double * att_rad);
    int CalculePointJonctionDilate(Segment2D * seg2,double dilatation_m,Point2D * pt1, Point2D * pt2);
    int TestIntersection(Segment2D * seg2,bool calculeIntersection,Point2D * ptInter);
    double DistancePoint(Point2D *point);
};

class Vecteur2D
{

  protected:
  
  public:
  double x;
  double y;
  
  Vecteur2D(double x_i, double y_i);
  Vecteur2D(Segment2D *seg);
  Vecteur2D(Point2D *pt1, Point2D *pt2);
  ~Vecteur2D(void);
  double ProduitVectoriel(Vecteur2D *vect);
  double ProduitScalaire(Vecteur2D *vect);
  double angle(void);
  double norme(void);

};

/* Arc de cercle de centre pt, définit entre les angles AngleA et AngleB dans le sens trigonométrique (anti-horaire) */
class Arc2D
{
  protected:

  public:
  Point2D * pt;
  double rayon;
  double angleA;
  double angleB;
  double longueur;
  
  Arc2D(Point2D *ptA, double rayon, double AngleA, double AngleB);
  ~Arc2D(void);
  int TestIntersectionSegment ( Segment2D *seg, Point2D *ptInter1, Point2D *ptInter2 );
  int CalculerSegmentsTangents ( Arc2D *other, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );
  void CalculerCheminLL ( Arc2D *other, Segment2D *segLL, Segment2D *segRR );
  void CalculerCheminLR ( Arc2D *other, Segment2D *segLR, Segment2D *segRL );
};

class Boite2D
{
  protected: //accessible uniquement par héritage
    
    
  public : // accessible partout
  double limHaute;
    double limBasse;
    double limGauche;
    double limDroite;
    
    Boite2D(Segment2D * seg2D);
    Boite2D(Point2D * pt2D, double rayonEncombrement); 
    Boite2D(vectorSegment2D contour);
    ~Boite2D(void);
    bool IntersectionAvecBoite(Boite2D * boite);
};
class WayPoint
{
  protected: //accessible uniquement par héritage
     
  public : // accessible partout
  Point2D * pt;
  double cap_deg;
  double vitesse_m_par_s;
  unsigned char controlByte;
  WayPoint(double x_m, double y_m, double cap_en_deg, double vitesse_m_par_s);
  WayPoint(TiXmlElement* pWP);
  ~WayPoint(void);
  WayPoint* operator = (WayPoint* wp);

};
#endif
