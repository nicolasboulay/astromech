#include "geom2D.h"
#include "tinyxml.h"
#include <math.h>


double max(double x, double y)
{
  if (x > y)
  {return x;}
  else
  {return y;}
}
double min(double x, double y)
{
  if (x > y)
  {return y;}
  else
  {return x;}
}

void normalise0_2PI(double & angle_rad)
{
  while (angle_rad < 0){angle_rad+=2*M_PI;}
  while (angle_rad >= 2*M_PI){angle_rad-=2*M_PI;}
}
void normaliseMPI_PPI(double & angle_rad)
{
  while (angle_rad < -M_PI){angle_rad+=2*M_PI;}
  while (angle_rad >= M_PI){angle_rad-=2*M_PI;}
}
/*************************************************************************/
/*                                  Point2D                              */
/*************************************************************************/
int Point2D::nbPtCrees = 0;
int Point2D::nbPtPresents = 0;

Point2D::Point2D(TiXmlElement* pPt)
{
  TiXmlElement* pElt;
  TiXmlHandle handlePt(0);
  handlePt = TiXmlHandle(pPt);
    
  pElt = handlePt.FirstChild("x").Element(); 
  istringstream issx( pElt->GetText() );
  issx >> x;
  cout << "x "<<issx.str() <<endl;
  pElt = handlePt.FirstChild("y").Element();
  istringstream issy( pElt->GetText() );
  issy >> y;
  cout << "y "<<issy.str() <<endl;
}
Point2D::Point2D(double a, double b)
{
  x = a;
  y = b;
  nbPtCrees++;
  nbPtPresents++;
  cout << "create Pt2D " <<this <<"nbPtCrees "<<nbPtCrees<< endl;
}
Point2D::Point2D(Point2D * pt)
{
  this->x = pt->x;
  this->y = pt->y;
  nbPtCrees++;
  nbPtPresents++;
  cout << "create Pt2D " <<this <<"nbPtCrees "<<nbPtCrees<<endl;
}
Point2D::~Point2D(void)
{
  nbPtPresents--;
  cout << "delete Pt2D " <<this <<"nbPtsPresents "<<nbPtPresents<< endl;
}
double Point2D::distanceAuPoint(Point2D * pt)
{
  return sqrt((x - pt->x)*(x - pt->x) + (y - pt->y)*(y - pt->y));
}

Point2D * Point2D::creePointRT(Point2D * centreGraviteRT, double * att_rad)
{
  double new_x,new_y;
  //translation
  new_x = centreGraviteRT->x + this->x;
  new_y = centreGraviteRT->y + this->y;
  //rotation
  new_x = cos(*att_rad)*new_x + sin(*att_rad)*new_y;
  new_y = -sin(*att_rad)*new_x + cos(*att_rad)*new_y;
  
  Point2D * newPoint2D = new Point2D(new_x,new_y);
  
  return newPoint2D;
}
void Point2D::calculePointDansNouveauRepere(Point2D * origineRepere,double orientationRepere_rad)
{
  double deltaX = origineRepere->x;
  double deltaY = origineRepere->y;
  double newX, newY;
  x -= deltaX;
  y -= deltaY;
  newX = cos(-orientationRepere_rad)*x + sin(-orientationRepere_rad)*y;
  newY = -sin(-orientationRepere_rad)*x + cos(-orientationRepere_rad)*y;
  x = newX;
  y = newY;
}
void Point2D::calculePointDansRepereTerrain(Point2D * origineRepere,double orientationRepere_rad)
{
  double deltaX = origineRepere->x;
  double deltaY = origineRepere->y;
  double newX, newY;
  newX = cos(orientationRepere_rad)*x + sin(orientationRepere_rad)*y;
  newY = -sin(orientationRepere_rad)*x + cos(orientationRepere_rad)*y;
  newX += deltaX;
  newY += deltaY;
  x = newX;
  y = newY;
}

double Point2D::calculeCap_rad(Point2D * ptDest)
{
  double cap,deltaX, deltaY;
  deltaX = ptDest->x - x;
  deltaY = ptDest->y - y;
  cap = 0;
  //recherche du quadrant
  if ((deltaX >= 0)&&(deltaY > 0))
  {
    cap = atan(deltaX/deltaY);
    return cap;
  }
  if ((deltaX >= 0)&&(deltaY < 0))
  {
    cap = atan(deltaX/deltaY) + M_PI;
    return cap;
  }
  if ((deltaX < 0)&&(deltaY < 0))
  {
    cap = atan(deltaX/deltaY) + M_PI;
    return cap;
  }
  if ((deltaX < 0)&&(deltaY > 0))
  {
    cap = atan(deltaX/deltaY) + 2*M_PI;
    return cap;
  }
  if (deltaY == 0.0)
  {
    if (deltaX > 0)
    {
      cap = M_PI/2;
      return cap;
    }
    if (deltaX < 0)
    {
      cap = -M_PI/2;
      return cap;
    }  
    if (deltaX == 0.0)
    {
      cap = 0.0;
      return cap; 
    }
  }
}
/*************************************************************************/
/*                                Segment2D                              */
/*************************************************************************/
Segment2D::Segment2D(TiXmlElement* pSeg)
{
  TiXmlElement* pPt;
  TiXmlHandle handleSegment(0);
  handleSegment = TiXmlHandle(pSeg);
  //chargement des points
  pPt = handleSegment.FirstChildElement("Point2D").Element();
  cout << "point1" <<endl;
  pt1 = new Point2D(pPt);
  pPt=pPt->NextSiblingElement("Point2D");
  cout << "point2" <<endl;
  pt2 = new Point2D(pPt);
}
Segment2D::Segment2D(Point2D * ptA, Point2D * ptB)
{
  pt1 = ptA;
  pt2 = ptB;
}

Segment2D::~Segment2D(void)
{
  cout << "delete Seg2D" << endl;
  delete (pt1);
  delete (pt2);
}

Segment2D * Segment2D::creeSegmentRT(Point2D * centreGraviteRT, double * att_rad)
{
  Point2D *newpt1;
  Point2D *newpt2;
  newpt1 = this->pt1->creePointRT(centreGraviteRT,att_rad);
  newpt2 = this->pt2->creePointRT(centreGraviteRT,att_rad);
  Segment2D *newSegment;
  newSegment = new Segment2D(newpt1,newpt2);
  return newSegment;
}

/*************************************************************************/
/*                                  Boite2D                              */
/*************************************************************************/
Boite2D::Boite2D(Segment2D * seg2D)
{
  double pt1_x = seg2D->pt1->x;  
  double pt1_y = seg2D->pt1->y; 
  double pt2_x = seg2D->pt2->x;  
  double pt2_y = seg2D->pt2->y; 
  if (pt1_x <= pt2_x)
  {
    limGauche = pt1_x;
    limDroite = pt2_x;
  }
  else
  {
    limGauche = pt2_x;
    limDroite = pt1_x;
  }
  if (pt1_y <= pt2_y)
  {
    limBasse = pt1_y;
    limHaute = pt2_y;
  }
  else
  {
    limBasse = pt2_y;
    limHaute = pt1_y;
  }
}
Boite2D::Boite2D(Point2D * pt2D, double rayonEncombrement)
{
  limGauche = pt2D->x - rayonEncombrement;
  limDroite = pt2D->x + rayonEncombrement;
  limBasse = pt2D->y - rayonEncombrement;
  limHaute = pt2D->y + rayonEncombrement;
} 
Boite2D::Boite2D(vectorSegment2D contour)
{
  vectorSegment2D::iterator it;
  Segment2D *seg;
  limGauche = 100000;
  limDroite = -100000;
  limBasse = 100000;
  limHaute = -100000;
  for (it = contour.begin();it!=contour.end();++it)
  {
    seg = *it;
    limGauche = min (seg->pt1->x,limGauche);
    limGauche = min (seg->pt2->x,limGauche);
    limDroite = max (seg->pt1->x,limDroite);
    limDroite = max (seg->pt2->x,limDroite);
    limBasse = min (seg->pt1->y,limBasse);
    limBasse = min (seg->pt2->y,limBasse);
    limHaute = max (seg->pt1->y,limHaute);
    limHaute = max (seg->pt2->y,limHaute);
  }

}

Boite2D::~Boite2D(void)
{

}
bool Boite2D::IntersectionAvecBoite(Boite2D * boite)
{
  bool resultat = true;
  if(this->limGauche > boite->limDroite)
  {
    return false;
  }
  if(this->limDroite < boite->limGauche)
  {
    return false;
  }
  if(this->limHaute < boite->limBasse)
  {
    return false;
  }
  if(this->limBasse > boite->limHaute)
  {
    return false;
  } 
  return resultat; 
}
/*************************************************************************/
/*                                WayPoint                               */
/*************************************************************************/
WayPoint::WayPoint(double x_m, double y_m, double att_rad, double vitesse)
{
  pt = new Point2D(x_m,y_m);
  attitude_rad = att_rad;
  vitesse_m_par_s = vitesse;
}
WayPoint::WayPoint(TiXmlElement* pWP)
{
  TiXmlElement* pPt;
  TiXmlElement* pElt;
  TiXmlHandle handleWP(0);
  handleWP = TiXmlHandle(pWP);
  //chargement des points
  pPt = handleWP.FirstChildElement("Point2D").Element();
  this->pt = new Point2D(pPt);
  
  pElt = handleWP.FirstChild("cap_deg").Element(); 
  istringstream iss1( pElt->GetText() );
  iss1 >> this->attitude_rad;
  this->attitude_rad *=M_PI/180;
  cout << "attitude_deg "<<iss1.str() <<endl;
  pElt = handleWP.FirstChild("vit_m_par_s").Element();
  istringstream iss2( pElt->GetText() );
  iss2 >> this->vitesse_m_par_s;
  cout << "vitesse_m_par_s "<<iss2.str() <<endl;
}

WayPoint::~WayPoint(void)
{
  delete pt;
}
WayPoint* WayPoint::operator =(WayPoint* wp)
{
  if (wp == this) return this;
  this->pt = new Point2D(wp->pt->x,wp->pt->y);
  this->attitude_rad    = wp->attitude_rad;
  this->vitesse_m_par_s = wp->vitesse_m_par_s;
  return this;
}
