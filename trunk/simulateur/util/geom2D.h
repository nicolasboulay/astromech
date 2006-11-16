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
void normalise0_2PI(double & angle_rad);
void normaliseMPI_PPI(double & angle_rad);

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
  double attitude_rad;
  double vitesse_m_par_s;
  WayPoint(double x_m, double y_m, double att_rad, double vitesse_m_par_s);
  WayPoint(TiXmlElement* pWP);
  ~WayPoint(void);
  WayPoint* operator = (WayPoint* wp);

};
#endif
