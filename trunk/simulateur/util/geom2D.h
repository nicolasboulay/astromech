#ifndef Geom2DH
#define Geom2DH
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define COTE_GAUCHE 1
#define COTE_DROIT 2

using namespace std;

class TiXmlElement;
class Point2D;
class Segment2D;
class Vecteur2D;
class Arc2D;
class Boite2D;
class WayPoint;
class Branche;

typedef vector<Segment2D *> vectorSegment2D;
typedef vector<WayPoint *> vectorWayPoint;

double max(double x, double y);
double min(double x, double y);
double square(double x);
void normalise0_360_100eme_deg(double & angle_deg);
void normalise0_2PI(double & angle_rad);
void normaliseMPI_PPI(double & angle_rad);
double valAbs(double & valeur);
double convertirAngle ( double angle_rad );
double rad2deg ( double angle_rad );
double deg2rad ( double angle_deg );
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
    static int nbCrees;
	static int nbPresents;
    Point2D * pt1;
    Point2D * pt2;
    
	Segment2D();
    Segment2D(TiXmlElement* pSeg);
    Segment2D(Point2D * pt1, Point2D * pt2);
    ~Segment2D(void);
    Segment2D * creeSegmentRT(Point2D * centreGraviteRT, double * att_rad);
    int CalculePointJonctionDilate(Segment2D * seg2,double dilatation_m,Point2D * pt1, Point2D * pt2);
    int TestIntersection(Segment2D * seg2,bool calculeIntersection,Point2D * ptInter);
    double DistancePoint(Point2D *point);
    void copySegment(Segment2D *seg);
	double getAngle(void);
	double getLongueur(void);
	char deletePoints;
};

class Vecteur2D
{

  protected:
  
  public:
    static int nbCrees;
	static int nbPresents;
    double x;
    double y;
  
  Vecteur2D(double x_i, double y_i);
  Vecteur2D(Segment2D *seg);
  Vecteur2D(Point2D *pt1, Point2D *pt2); // vecteur pt1 pt2
  ~Vecteur2D(void);
  double ProduitVectoriel(Vecteur2D *vect);
  double ProduitScalaire(Vecteur2D *vect);
  double angle(void);
  double norme(void);

};

/* Arc de cercle de centre pt, définit entre les angles AngleA et AngleB dans le sens trigonométrique (anti-horaire) */
class Arc2D
{
  friend class WayPoint;
  protected:

  public:
    static int nbCrees;
	static int nbPresents;
  Point2D * pt;
  double rayon;
  double angleA;
  double angleB;
  double longueur;
  
  Arc2D(Point2D *ptA, double rayon, double AngleA, double AngleB);
  Arc2D( WayPoint * wayPoint, double sens, double rayon);
  Arc2D(Point2D *ptO, Point2D *ptA, Point2D *ptB, double rayon);
  ~Arc2D(void);
  int TestIntersectionSegment ( Segment2D *seg, Point2D *ptInter1, Point2D *ptInter2 );
  int TestIntersectionArc ( Arc2D *arc2, Point2D *ptInter1, Point2D *ptInter2 );
  int CalculerSegmentsTangents ( Arc2D *other, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );
  void CalculerCheminLL ( Arc2D *other, Segment2D *segLL, Segment2D *segRR );
  void CalculerCheminLR ( Arc2D *other, Segment2D *segLR, Segment2D *segRL );
  /*int CalculerTrajectoires ( WayPoint *wp1, double rayon1, WayPoint *wp2, double rayon2, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL );*/
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
  int id;
  WayPoint(double x_m, double y_m, double cap_en_deg, double vitesse_m_par_s);
  WayPoint(TiXmlElement* pWP);
  ~WayPoint(void);
  WayPoint* operator = (WayPoint* wp);
  int cotePoint(Point2D *pt);
};

class Branche
{
  protected:
  
  public:
  WayPoint *wp1;
  WayPoint *wp2;
  double longueur;
  int type; // 0 : droit, 1 : courbe
  Arc2D *arc;
  
  Branche(Segment2D *seg);
  Branche(Arc2D *arc, WayPoint *wpA, WayPoint *wpB) ;
  ~Branche(void);
};

double calculerDistance ( double x1, double y1, double x2, double y2 );
double calculerAngle(double xA, double yA, double xB, double yB);
double produitVectoriel ( double x1, double y1, double x2, double y2 );
void calculerCercle ( double dtheta, double x1, double y1, double x2, double y2, double *x0, double *y0, double *R);
void calculerIntersectionCercles( double x1, double y1, double R1, double x2, double y2, double R2, double *xi1, double *yi1, double *xi2, double *yi2);
void calculerPositionTrigo(double xBalise, double yBalise, double xC1, double yC1, double R1, double xC2, double yC2, double R2, double *xP, double *yP);

class Vertex
{
  protected:
  
  public:
	WayPoint *wp;
	int id;
	double distDepart;

};

#endif
