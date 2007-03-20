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
void normalise0_360_100eme_deg(double & angle_deg)
{
  while (angle_deg < 0){angle_deg+=36000;}
  while (angle_deg >= 36000){angle_deg-=36000;}
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
  //cout << "x "<<issx.str() <<endl;
  pElt = handlePt.FirstChild("y").Element();
  istringstream issy( pElt->GetText() );
  issy >> y;
  //cout << "y "<<issy.str() <<endl;
}
Point2D::Point2D(double a, double b)
{
  x = a;
  y = b;
  nbPtCrees++;
  nbPtPresents++;
  //cout << "create Pt2D " <<this <<"nbPtCrees "<<nbPtCrees<< endl;
}
Point2D::Point2D(Point2D * pt)
{
  this->x = pt->x;
  this->y = pt->y;
  nbPtCrees++;
  nbPtPresents++;
  //cout << "create Pt2D " <<this <<"nbPtCrees "<<nbPtCrees<<endl;
}
Point2D::~Point2D(void)
{
  nbPtPresents--;
  //cout << "delete Pt2D " <<this <<"nbPtsPresents "<<nbPtPresents<< endl;
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
  //cout << "delete Seg2D" << endl;
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
int Segment2D::CalculePointJonctionDilate(Segment2D * seg2, double dilatation_m,Point2D * ptDilate1, Point2D * ptDilate2)
{ 

  if ((seg2!=NULL)&&(ptDilate1!=NULL)&&(ptDilate2!=NULL))
  {
    Point2D *pt1 = this->pt1;
    Point2D *pt2 = this->pt2;
    Point2D *pt3 = seg2->pt2;
    double capPt2VersPt1 = pt2->calculeCap_rad(pt1);
    double capPt2VersPt3 = pt2->calculeCap_rad(pt3);
    double angleExterieur_rad = capPt2VersPt3 - capPt2VersPt1;
    normalise0_2PI(angleExterieur_rad);
    double distancePt2PtDilate;
    double capDilatation_rad;
    if (angleExterieur_rad <= 3*M_PI/2)
    {
      /* dilatation : un seul point suffit*/
      distancePt2PtDilate = dilatation_m/sin(angleExterieur_rad/2);
      capDilatation_rad = capPt2VersPt1 + angleExterieur_rad/2;
      normalise0_2PI(capDilatation_rad);
      ptDilate1->x = pt2->x + distancePt2PtDilate*sin(capDilatation_rad);
      ptDilate1->y = pt2->y + distancePt2PtDilate*cos(capDilatation_rad);
      return 1;
    }
    else
    {
      /* dilatation : 2 pts nécessaires */
      distancePt2PtDilate = dilatation_m/cos((angleExterieur_rad-M_PI)/4);
      capDilatation_rad = capPt2VersPt1 + (angleExterieur_rad+M_PI)/4;
      normalise0_2PI(capDilatation_rad);
      ptDilate1->x = pt2->x + distancePt2PtDilate*sin(capDilatation_rad);
      ptDilate1->y = pt2->y + distancePt2PtDilate*cos(capDilatation_rad);
      
      //capDilatation_rad += (angleExterieur_rad+M_PI)/2;
      capDilatation_rad = capPt2VersPt3 - (angleExterieur_rad+M_PI)/4;
      normalise0_2PI(capDilatation_rad);
      ptDilate2->x = pt2->x + distancePt2PtDilate*sin(capDilatation_rad);
      ptDilate2->y = pt2->y + distancePt2PtDilate*cos(capDilatation_rad);
      return 2;
    }
  }
  else
  {
    return -1;
  }
}
int Segment2D::TestIntersection(Segment2D * seg2,bool calculeIntersection,Point2D * ptInter)
{
  //retourne 0 si pas intersection, 1 si intersection, -1 si erreur.
  Point2D *pt1;
  Point2D *pt2;
  Point2D *pt3;
  Point2D *pt4;
  
  pt1 = this->pt1;
  pt2 = this->pt2;
  pt3 = seg2->pt1;
  pt4 = seg2->pt2;
  
  ptInter->x = 0.0;
  ptInter->y = 0.0;
  
  //test de rejet rapide (vérif de l'intersection des boites englobantes
  if(!(   max(pt1->x,pt2->x)>=min(pt3->x,pt4->x)
       && max(pt3->x,pt4->x)>=min(pt1->x,pt2->x)
       && max(pt1->y,pt2->y)>=min(pt3->y,pt4->y)
       && max(pt3->y,pt4->y)>=min(pt1->y,pt2->y)
       ))
  {
    return 0;
  }
  
  //test complet car les boites intersectent
  double z1 = (pt3->x - pt1->x)*(pt2->y - pt1->y) - (pt3->y - pt1->y)*(pt2->x - pt1->x);
  double z2 = (pt4->x - pt1->x)*(pt2->y - pt1->y) - (pt4->y - pt1->y)*(pt2->x - pt1->x);
  double z3 = (pt1->x - pt3->x)*(pt4->y - pt3->y) - (pt1->y - pt3->y)*(pt4->x - pt3->x);
  double z4 = (pt2->x - pt3->x)*(pt4->y - pt3->y) - (pt2->y - pt3->y)*(pt4->x - pt3->x);
  
  int s1 = 0;
  int s2 = 0;
  int s3 = 0;
  int s4 = 0;
  
  if (z1 < 0){s1 = -1;} else if (z1>0) {s1 = 1;} else {s1 = 0;}
  if (z2 < 0){s2 = -1;} else if (z2>0) {s2 = 1;} else {s2 = 0;}
  if (z3 < 0){s3 = -1;} else if (z3>0) {s3 = 1;} else {s3 = 0;}
  if (z4 < 0){s4 = -1;} else if (z4>0) {s4 = 1;} else {s4 = 0;}
  
  if ((s1*s2<=0)&&(s3*s4<=0))
  {
    if (calculeIntersection)
    {
      if ((s1==0)&&(s2==0)&&(s3==0)&&(s4==0))
      {
        //infinité de points d'intersection
	//on choisit le point4
	ptInter->x = pt4->x;
	ptInter->y = pt4->y;
      }
      else
      {
        //on doit renseigner le point d'intersection
        //resolution du systeme A1.x+B1.y=C1
        //                      A2.x+B2.y=C2
        double A1 = pt1->y - pt2->y;
        double B1 = pt2->x - pt1->x;
        double A2 = pt3->y - pt4->y;
        double B2 = pt4->x - pt3->x;
        double C1 = pt1->x*A1 + pt1->y*B1;
        double C2 = pt3->x*A2 + pt3->y*B2;
        double invDeterminant = 1/(A1*B2-B1*A2);
      
        //reste à résoudre (X Y) = invA * (C1 C2)
        ptInter->x = invDeterminant * ( C1*B2 - B1*C2);
        ptInter->y = invDeterminant * (-A2*C1 + A1*C2);
      }
     /* if (ptInter->x != ptInter->x)
      {
        cout <<"z1"<<z1<<"z2"<<z2<<"z3"<<z3<<"z4"<<z4<<endl;
	cout <<"A1"<<A1<<"B1"<<B1<<"A2"<<A2<<"B2"<<B2<<"C1"<<C1<<"C2"<<C2<<endl;
	cout <<"pt1 "<<pt1->x<<" "<<pt1->y<<endl;
	cout <<"pt2 "<<pt2->x<<" "<<pt2->y<<endl;
	cout <<"pt3 "<<pt3->x<<" "<<pt3->y<<endl;
	cout <<"pt4 "<<pt4->x<<" "<<pt4->y<<endl;
      }*/
      
    }
    return 1;
  }
  else
  {
    return 0;
  }
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
WayPoint::WayPoint(double x_m, double y_m, double cap_en_deg, double vitesse)
{
  pt = new Point2D(x_m,y_m);
  cap_deg = cap_en_deg;
  vitesse_m_par_s = vitesse;
  controlByte = 0;
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
  iss1 >> this->cap_deg;
  cout << "cap_deg "<<iss1.str() <<endl;
  pElt = handleWP.FirstChild("vit_m_par_s").Element();
  istringstream iss2( pElt->GetText() );
  iss2 >> this->vitesse_m_par_s;
  cout << "vitesse_m_par_s "<<iss2.str() <<endl;
  controlByte = 0;
}

WayPoint::~WayPoint(void)
{
  delete pt;
}
WayPoint* WayPoint::operator =(WayPoint* wp)
{
  if (wp == this) return this;
  this->pt = new Point2D(wp->pt->x,wp->pt->y);
  this->cap_deg    = wp->cap_deg;
  this->vitesse_m_par_s = wp->vitesse_m_par_s;
  return this;
}
