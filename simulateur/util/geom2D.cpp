#include "geom2D.h"
#include "tinyxml.h"
#include "bit.h"
#include "conversion.h"
#include <math.h>

//Attention ces define existent aussi dans comSeriePC_PIC.h mais 
//pour une raison inconnue je n'arrive pas à inclure ce .h
//bits pour u8_CTRL_WPx
#define BIT_WP_NUL 7
#define BIT_WP_NEXT 6
#define BIT_SENS_WP 5
#define BIT_ROT 4
#define BIT_SENS_ROT 3
#define BIT_PWM 2

#define TRUE 1
#define FALSE 0

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

/* Fonction "mettre au carré" */
double square(double x) {
   return x*x;
} /* fin square */

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

double valAbs(double & valeur)
{
  if (valeur >=0)
  {
    return valeur;
  }
  else
  {
    return -valeur;
  }
}

double convertirAngle ( double angle_rad )
{
  double angle_conv = M_PI/2 - angle_rad;
  normalise0_2PI(angle_conv);
  return angle_conv;
}

/* Fonction calcul de la distance angulaire entre deux angles normalisés [0,2*PI] */
double DistanceAngulaire ( double AngleA, double AngleB ) {
  
  if ( AngleB > AngleA ) {
    return AngleB - AngleA;
  } else {
    return 2*M_PI + AngleB - AngleA;
  }
  
} /* fin DistanceAngulaire */

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
  return 0.0;
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

void Segment2D::copySegment( Segment2D *seg ) {
   this->pt1->x = seg->pt1->x;
   this->pt1->y = seg->pt1->y;
   this->pt2->x = seg->pt2->x;
   this->pt2->y = seg->pt2->y;
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

// Calcul la distance entre le point C et la droite sur laquelle se trouve le segment AB
double Segment2D::DistancePoint ( Point2D *point ) {

  Vecteur2D *vectAB = new Vecteur2D(this);
  Vecteur2D *vectAC = new Vecteur2D(point->x - this->pt1->x, point->y - this->pt1->y);
  
  double produitVectoriel = vectAB->ProduitVectoriel(vectAC);
  double normeAB = vectAB->norme();
  if ( normeAB <= 0 ) {
    return -1;
  } else {
    return produitVectoriel / vectAB->norme();
  }
  
}

/*************************************************************************/
/*                                Vecteur                                */
/*************************************************************************/
Vecteur2D::Vecteur2D(double x_i, double y_i) {
  x = x_i;
  y = y_i;
}

Vecteur2D::Vecteur2D(Segment2D *seg) {
  x = seg->pt2->x - seg->pt1->x;
  y = seg->pt2->y - seg->pt1->y;
}

Vecteur2D::Vecteur2D(Point2D *pt1, Point2D *pt2 ) {
  x = pt2->x - pt1->x;
  y = pt2->y - pt1->y;
}

Vecteur2D::~Vecteur2D(void) {

}

// Calcule le produit vectoriel projeté sur Z : this ^ vect
double Vecteur2D::ProduitVectoriel ( Vecteur2D *vect ) {
  return this->x * vect->y - this->y * vect->x;
}

// Calcule le produit scalaire this . vect
double Vecteur2D::ProduitScalaire ( Vecteur2D *vect ) {
  return this->x * vect->x + this->y * vect->y;
}

double Vecteur2D::angle (void) {
  double angle = atan2(y,x);
  normalise0_2PI(angle);
  return angle;
}

double Vecteur2D::norme(void) {
  return sqrt(square(x) + square(y));
}

/*************************************************************************/
/*                                Arc2D                                  */
/*************************************************************************/
Arc2D::Arc2D(Point2D * ptA, double rayonA, double thetaA, double thetaB)
{
  pt = ptA;
  rayon = rayonA;
  angleA = thetaA;
  angleB = thetaB;
  longueur = rayon * DistanceAngulaire(angleB, angleA);
}

// 1 = left, 2 = right
Arc2D::Arc2D(WayPoint *WP, double sens, double rayonA)
{
  rayon = rayonA;
  angleA = 0;
  angleB = 2 * M_PI;
  pt = new Point2D(0,0);
  pt->x = WP->pt->x + rayon * cos(WP->cap_deg * M_PI/180 + sens * M_PI/2);
  pt->y = WP->pt->y + rayon * sin(WP->cap_deg * M_PI/180 + sens * M_PI/2);
}

Arc2D::Arc2D(Point2D *ptO, Point2D *ptA, Point2D *ptB, double rayonA)
{
  pt = ptO;
  rayon = rayonA;
  Vecteur2D *vecOA = new Vecteur2D(ptO, ptA);
  angleA = vecOA->angle();
  Vecteur2D *vecOB = new Vecteur2D(ptO, ptB);
  angleB = vecOB->angle();
  longueur = rayon * DistanceAngulaire(angleB, angleA);
}

Arc2D::~Arc2D(void)
{
  //cout << "delete Arc2D" << endl;
  delete (pt);
}

// Tester la collision avec un segment
// retourne 1 si collision, 0 sinon
int Arc2D::TestIntersectionSegment ( Segment2D *seg, Point2D *ptInter1, Point2D *ptInter2 ) {

  // Si distance supérieure entre le segment et le centre supérieure au rayon de l'arc de cercle
  if ( seg->DistancePoint(pt) > rayon ) {
    //printf("Trop eloigne du centre de l'arc\n");
    return 0;
  }
    
  // Sinon, il va falloir calculer les points d'intersection
  // On note 0 le centre de l'arc de cercle, A et B les extrêmités du segment
  Vecteur2D *vectAB = new Vecteur2D(seg);
  Vecteur2D *vectOA = new Vecteur2D(pt, seg->pt1);
  
  // On note P le point courant sur le segment sAB
  // On a vAP = k vAB (vecteurs)
  // Il faut |vOP| = R (longueurs)
  // vOP = vOA + vAP = vOA + k.vAB
  // (OA->x + k.AB->x)^2 + (OA->y + k.AB->y)^2 - R^2 = 0
  // a.k^2 + b.k + c = 0
  // avec : a = (AB->x + AB->y)^2
  //        b = 2*(OA->x * AB->x + OA->y + AB->y)
  //        c = OA->x^2 + OA->y^2 - R^2
  //
  // On obtient sqrt(d) = sqrt(b^2 - 4.a.c)
  // d étant forcément supérieur à 0 (tests rapides initiaux), on a forcément deux solutions
  
  double a = square(vectAB->x) + square(vectAB->y);
  double b = 2*(vectOA->x * vectAB->x + vectOA->y * vectAB->y);
  double c = square(vectOA->x) + square(vectOA->y) - square(rayon);
  double d = square(b) - 4 * a * c;
  
  if ( d < 0 ) { // impossible, mais bon...
    //printf("Impossible\n");
    return FALSE;
  }
  
  double k1 = (-b - sqrt(d)) / ( 2 * a );
  double k2 = (-b + sqrt(d)) / ( 2 * a );

  //printf("k1:%f k2:%f\n", k1, k2);
  
  // Si les points d'intersection sont en dehors du segment, c fini
  if ( (k1 < 0 || k1 > 1) && (k2 < 0 || k2 > 1) ) {
    //printf("Aucun\n");
	return 0;
  }
  //printf("Peut etre\n");
  // Sinon, il va falloir vérifier avec l'arc de cercle
  
  // Construction des deux points d'intersection potentiels Arc-Segment
 
  int NbPoints = 0;
  
  // On regarde si le premier point appartient à l'arc de cercle
  if ( k1 > 0 && k1 < 1 ) {

    Point2D *ptInter = new Point2D(seg->pt1->x + k1 * (seg->pt2->x - seg->pt1->x), seg->pt1->y + k1 * (seg->pt2->y - seg->pt1->y));
    Vecteur2D *vecInter = new Vecteur2D(pt, ptInter);
    double theta = vecInter->angle();
    
	//printf("Theta : %f, [%f %f] [%f %f] [%f %f]\n", theta, ptInter->x, ptInter->y, seg->pt1->x, seg->pt1->y, seg->pt2->x, seg->pt2->y);
	
    // Si l'angle du point est dans le secteur balayé par l'arc de cercle, contact !
    if ( DistanceAngulaire(angleA, theta) < DistanceAngulaire(angleA, angleB) ) {
      NbPoints++;
	  ptInter1->x = ptInter->x;
	  ptInter1->y = ptInter->y;
    }
  
  }
    
  // On regarde si le deuxième point appartient à l'arc de cercle
  if ( k2 > 0 && k2 < 1 ) {

    Point2D *ptInter = new Point2D(seg->pt1->x + k2 * (seg->pt2->x - seg->pt1->x), seg->pt1->y + k2 * (seg->pt2->y - seg->pt1->y));
    Vecteur2D *vecInter = new Vecteur2D(pt, ptInter);
    double theta = vecInter->angle();
    
	//printf("Theta : %f, [%f %f]\n", theta, ptInter->x, ptInter->y);
	
    // Si l'angle du point est dans le secteur balayé par l'arc de cercle, contact !
    if ( DistanceAngulaire(angleA, theta) < DistanceAngulaire(angleA, angleB) ) {
		NbPoints++;
		if ( NbPoints == 1 ) {
			ptInter1->x = ptInter->x;
			ptInter1->y = ptInter->y;
		} else {
			ptInter2->x = ptInter->x;
			ptInter2->y = ptInter->y;
		}
		
    }
  
  }
  
  return NbPoints;
}

// Calcul des 4 segments tangents entre deux cercles
int Arc2D::CalculerSegmentsTangents ( Arc2D *other, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL ) {
    
  // On note A et B les centres respectifs des cercles this et other
  
  // Y aura-t-il 0, 2 ou 4 segments ?
  // 0 segment si un cercle est inclus dans l'autre
  Vecteur2D *vectAB = new Vecteur2D(this->pt, other->pt);
  double distAB = vectAB->norme();
  double R1 = this->rayon;
  double R2 = other->rayon;
  
  if ( ( R1 + distAB < R2 ) || ( R2 + distAB < R1 ) ) {
    return 0;
  }
  
  int NbSegments = ( distAB < R1 + R2 ) ? 2 : 4;
  
  // Calcul des points du chemin LL
  this->CalculerCheminLL ( other, segLL, segRR );
  
  if ( NbSegments == 4 ) {
    this->CalculerCheminLR ( other, segLR, segRL );
  }
  
  return NbSegments;
  
}

// Calcule des segments tangents extérieurement à deux cercles
void Arc2D::CalculerCheminLL ( Arc2D *other, Segment2D *segLL, Segment2D *segRR ) {

  // On note O1 et O2 les centres des cercles
  // On note P1 et P2 les points du segment
  
  Vecteur2D *vectO1O2 = new Vecteur2D(this->pt, other->pt);
  
  // Angle de la droite D1 passant par les centres des cercles
  double angleO1O2 = vectO1O2->angle();
  
  // Ecart d'angle entre la droite D1 et de la droite tangente D2
  double delta = asin( ( other->rayon - this->rayon ) / vectO1O2->norme() );
  
  // Calcul des angles dans le repère du centre des cercles
  double angle1 = angleO1O2 - delta - M_PI/2;
  double angle2 = angleO1O2 + delta + M_PI/2;

  // Calcule des coordonnees des points
  // Point adjacent au premier cercle, sens gauche (sens anti-horaire)
  segLL->pt1->x = this->pt->x + this->rayon * cos(angle1);
  segLL->pt1->y = this->pt->y + this->rayon * sin(angle1);
  
  // Point adjacent au premier cercle, sens doite (sens horaire)
  segRR->pt1->x = this->pt->x + this->rayon * cos(angle2);
  segRR->pt1->y = this->pt->y + this->rayon * sin(angle2);
  
  // Point adjacent au second cercle, sens gauche (sens anti-horaire)
  segLL->pt2->x = other->pt->x + other->rayon * cos(angle1);
  segLL->pt2->y = other->pt->y + other->rayon * sin(angle1);
  
  // Point adjacent au second cercle, sens doite (sens horaire)
  segRR->pt2->x = other->pt->x + other->rayon * cos(angle2);
  segRR->pt2->y = other->pt->y + other->rayon * sin(angle2);
  
}

// Calcule des segments tangents croisés à deux cercles
void Arc2D::CalculerCheminLR ( Arc2D *other, Segment2D *segLR, Segment2D *segRL ) {

  // On note O1 et O2 les centres des cercles
  // On note P1 et P2 les points du segment
  
  Vecteur2D *vectO1O2 = new Vecteur2D(this->pt, other->pt);
  
  // Angle de la droite D1 passant par les centres des cercles
  double angleO1O2 = vectO1O2->angle();
  
  // Ecart d'angle entre la droite D1 et de la droite tangente D2
  double delta = asin( ( other->rayon + this->rayon ) / vectO1O2->norme() );
  
  // Calcul des angles dans le repère du centre des cercles
  double angle1R = angleO1O2 - delta + M_PI/2;
  double angle1L = angleO1O2 + delta - M_PI/2;
  double angle2L = angleO1O2 - delta - M_PI/2;
  double angle2R = angleO1O2 + delta + M_PI/2;

  // Calcule des coordonnees des points
  // Point adjacent au premier cercle, sens gauche (sens anti-horaire)
  segLR->pt1->x = this->pt->x + this->rayon * cos(angle1L);
  segLR->pt1->y = this->pt->y + this->rayon * sin(angle1L);
  
  // Point adjacent au premier cercle, sens doite (sens horaire)
  segRL->pt1->x = this->pt->x + this->rayon * cos(angle1R);
  segRL->pt1->y = this->pt->y + this->rayon * sin(angle1R);
  
  // Point adjacent au second cercle, sens gauche (sens anti-horaire)
  segLR->pt2->x = other->pt->x + other->rayon * cos(angle2R);
  segLR->pt2->y = other->pt->y + other->rayon * sin(angle2R);
  
  // Point adjacent au second cercle, sens doite (sens horaire)
  segRL->pt2->x = other->pt->x + other->rayon * cos(angle2L);
  segRL->pt2->y = other->pt->y + other->rayon * sin(angle2L);
  
}

/*
int Arc2D::CalculerTrajectoires ( WayPoint *wp1, double rayon1, WayPoint *wp2, double rayon2, Segment2D *segLL, Segment2D *segRR, Segment2D *segLR, Segment2D *segRL ) {
  Arc2D *arc1L = new Arc2D(wp1,  1, rayon1);
  Arc2D *arc1R = new Arc2D(wp1, -1, rayon1);
  Arc2D *arc2L = new Arc2D(wp2,  1, rayon2);
  Arc2D *arc2R = new Arc2D(wp2, -1, rayon2);
  
  // Initilialisation
  Segment2D *segVide  = new Segment2D(new Point2D(0,0), new Point2D(0,0));
  Segment2D *segUtile = new Segment2D(new Point2D(0,0), new Point2D(0,0));
  
  // SegmentLL
  if ( 2 <= arc1L->CalculerSegmentsTangents( arc2L, segUtile, segVide, segVide, segVide) ) {
    segLL = segUtile;
  }
  
  // SegmentRR
  if ( 2 <= arc1R->CalculerSegmentsTangents( arc2R, segVide, segUtile, segVide, segVide) ) {
    segRR = segUtile;
  }
  
  // SegmentLR
  if ( 4 <= arc1L->CalculerSegmentsTangents( arc2R, segVide, segVide, segUtile, segVide) ) {
    segLR = segUtile;
  }
  
  // SegmentRL
  if ( 4 <= arc1R->CalculerSegmentsTangents( arc2L, segVide, segVide, segVide, segUtile) ) {
    segRL = segUtile;
  }
  
  return 0;
  
}
*/

// Tester la collision avec un cercle

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
  this->cap_deg = string2Double (pElt->GetText());
  cout << "cap_deg "<<this->cap_deg <<endl;
  
  pElt = handleWP.FirstChild("vit_m_par_s").Element();
  istringstream iss2( pElt->GetText() );
  iss2 >> this->vitesse_m_par_s;
  cout << "vitesse_m_par_s "<<iss2.str() <<endl;
  
  pElt = handleWP.FirstChild("sensMAv").Element();
  int sensMAv = string2Int(pElt->GetText());
  pElt = handleWP.FirstChild("rot").Element();
  int rot = string2Int(pElt->GetText());
  pElt = handleWP.FirstChild("sensRot").Element();
  int sensRot = string2Int(pElt->GetText());
  cout << "sensMAv "<<sensMAv <<endl;
  cout << "rot "<<rot <<endl;
  cout << "sensRot "<<sensRot <<endl;
  
  controlByte = 0;
  if (sensMAv==1)
    setBit(BIT_SENS_WP,controlByte);
  if (rot==1)
    setBit(BIT_ROT,controlByte);
  if (sensRot==1)
    setBit(BIT_SENS_ROT,controlByte);
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
/*************************************************************************/
/*                                Leg                               */
/*************************************************************************/
#define BRANCHE_DROITE 1
#define BRANCHE_COURBE 2

Branche::Branche(Segment2D *seg)
{
  Vecteur2D *vec = new Vecteur2D(seg);
  double route = vec->angle();
  wp1 = new WayPoint(seg->pt1->x, seg->pt1->y, route*180/M_PI, 0);
  wp2 = new WayPoint(seg->pt2->x, seg->pt2->y, route*180/M_PI, 0);
  longueur = vec->norme();
  type = BRANCHE_DROITE;
}
Branche::Branche(Arc2D *arcA, WayPoint *wpA, WayPoint *wpB) 
{
  wp1 = wpA;
  wp2 = wpB;
  Vecteur2D *vec1 = new Vecteur2D(arcA->pt, wp1->pt);
  Vecteur2D *vec2 = new Vecteur2D(arcA->pt, wp2->pt);
  double angle1 = vec1->angle();
  double angle2 = vec2->angle();
  arc = new Arc2D(arcA->pt, arcA->rayon, angle1, angle2);
  type = BRANCHE_COURBE;
  longueur = arc->rayon * DistanceAngulaire(vec1->angle(), vec2->angle());
}
Branche::~Branche(void)
{
  delete wp1;
  delete wp2;
}

/*************************************************************************/
/*                                Navigation absolue                               */
/*************************************************************************/

// Calcule la distance (norme euclidienne) entre les points A (x1,y1) et B (x2, y2)
double calculerDistance ( double x1, double y1, double x2, double y2 ) {
	return sqrt(square(x2-x1) + square(y2-y1));
}

double calculerAngle(double xA, double yA, double xB, double yB) {
	
	double angle = atan2(yB-yA, xB-xA);
	if ( angle < 0 ) {
		return angle+2*M_PI;
	} else {
		return angle;
	}
}

// Calcule le produit vectoriel entre les vecteurs V1 (x1, y1) et V2 (x2, y2) projeté sur z
double produitVectoriel ( double x1, double y1, double x2, double y2 ) {
	return x1*y2 - x2*y1;
}

// Calculer le cercle passant par les points A (x1,y1) et B (x2,y2) et le point où se trouve le robot, position depuis laquelle il voit l'écart angulaire entre A et B égal à dtheta.
// Le rayon du cercle est sauvegardé dans *R, la position de son centre en *x0 et *y0.
void calculerCercle ( double dtheta, double x1, double y1, double x2, double y2, double *x0, double *y0, double *R) {
	
	// Distance entre A et B
	double longueur = sqrt(square(x2-x1)+square(y2-y1));
	
	// Calcul du rayon du cercle
	double l = longueur/(2*sin(dtheta));
	*R = valAbs(l);
	
	// Saturation du rayon de virage à 1000 km (éviter les divisions par zéro)
	if ( *R > 1e9 ) {
		//printf("Saturation de R de %f à %f\n", *R, 1e9);
		*R = 1e9;
	}
	
	// Les deux équations du cercle passant par A et B
	// (xA-x)^2 + (yA-y)^2 = R^2  (1)
	// (xB-x)^2 + (yB-y)^2 = R^2  (2)
	// (1)-(2) sous la forme y = A * x + B  (3)
	double A = - (x2-x1)/(y2-y1);
	double B = ( square(y2) - square(y1) + square(x2) - square(x1) ) / ( 2*(y2-y1) );
	
	//printf("A:%f B:%f\n", A, B);
	
	// On remplace y (3) dans l'équation (1)
	// (1) avec (3) sous la forme Cx^2 + Dx + E = 0
	double C = 1+square(A);
	double D = -2*x1 - 2*A*(y1-B);
	double E = square(x1) + square(y1-B) - square(*R);
	
	//printf("C:%f D:%f E:%f\n", C, D, E);
	
	// Résolution de l'équation en x^2 : discriminant et racines du polynomes
	double delta = square(D) - 4*C*E;
	//printf("delta:%f\n", delta);
	
	// On obtient deux centres possibles pour le cercle
	double xr1 = (-D + sqrt(delta))/(2*C);
	double xr2 = (-D - sqrt(delta))/(2*C);
	
	double yr1 = A * xr1 + B;
	double yr2 = A * xr2 + B;
	
	//printf("C1 : %f %f\n", xr1, yr1);
	//printf("C2 : %f %f\n", xr2, yr2);
	
	// Calcul du produit vectoriel AB x AO1 (premier centre)
	double ProdVec = produitVectoriel(x2-x1, y2-y1, xr1-x1, yr1-y1);
	// De quel signe devrait être le produit vectoriel (côté du cercle par rapport à la position du robot)
	double signePrevu = ( dtheta < M_PI && dtheta > M_PI/2 ) ? -1 : 1;
	
	//printf("PV:%f PVprev:%f\n", ProdVec, signePrevu);
	
	// Sélection du côté du cercle en fonction du signe du produit vectoriel
	if ( ProdVec * signePrevu > 0 ) { // Même signe, donc on prend le premier point
		*x0 = xr1;
		*y0 = yr1;
	} else { // C'est le deuxième point qui est le bon
		*x0 = xr2;
		*y0 = yr2;
	}
	
}

// Calcule de l'intersection entre les deux cercles C1 de centre (x1, y1) et de rayon R1 et C2 de centre (x2, y2) et de rayon R2
// Les deux intersections seront notées (xi1, yi1) et (xi2, yi2)
void calculerIntersectionCercles( double x1, double y1, double R1, double x2, double y2, double R2, double *xi1, double *yi1, double *xi2, double *yi2) {

	//printf("Cercles : %f %f %f ; %f %f %f\n", x1, y1, R1, x2, y2, R2);

	// Sélection de la méthode, soit on prend y=Ax+B, soit x=Ay+B, permet de gérer les points alignés sur l'axe X ou l'axe Y
	double y21 = y2-y1;
	double x21 = x2-x1;
	if ( valAbs(y21) > valAbs(x21) ) {
		
		// équation du cercle 1 - équation du cercle 2 sous la forme yi = A xi + B (3)
		double A = - (x1-x2)/(y1-y2);
		double B = (square(x1) + square(y1) - square(x2) - square(y2) - square(R1) + square(R2)) / ( 2*(y1-y2));
		
		// (3) reportée dans (1) exprimé sous la forme C xi^2 + D xi + E = 0
		double C = 1 + square(A);
		double D = -2*x1 + 2*A*(B-y1);
		double E = square(B-y1) - square(R1) + square(x1);
		
		double delta = square(D) - 4*C*E;
		
		// Solutions
		*xi1 = ( -D - sqrt(delta) ) / (2*C);
		*xi2 = ( -D + sqrt(delta) ) / (2*C);
		
		*yi1 = A * (*xi1) + B;
		*yi2 = A * (*xi2) + B;
		
	} else {
	
		// équation du cercle 1 - équation du cercle 2 sous la forme xi = A yi + B
		double A = - (y1-y2)/(x1-x2);
		double B = (square(x1) + square(y1) - square(x2) - square(y2) - square(R1) + square(R2)) / ( 2*(x1-x2));
		
		// (3) reportée dans (1) exprimé sous la forme C xi^2 + D xi + E = 0
		double C = 1 + square(A);
		double D = -2*y1 + 2*A*(B-x1);
		double E = square(B-x1) - square(R1) + square(y1);
		
		double delta = square(D) - 4*C*E;
		
		// Solutions
		*yi1 = ( -D - sqrt(delta) ) / (2*C);
		*yi2 = ( -D + sqrt(delta) ) / (2*C);
		
		*xi1 = A * (*yi1) + B;
		*xi2 = A * (*yi2) + B;
		
	}
	
}

// Calcule la position d'intersection valable sur les deux calculées : une des deux est la position de la balise présente sur les deux cercles, l'autre est celle du robot
void calculerPositionTrigo(double xBalise, double yBalise, double xC1, double yC1, double R1, double xC2, double yC2, double R2, double *xP, double *yP) {
	
	double xi1, yi1, xi2, yi2;
	calculerIntersectionCercles(xC1, yC1, R1, xC2, yC2, R2, &xi1, &yi1, &xi2, &yi2);

	//printf("Intersections: %f %f ; %f %f\n", xi1, yi1, xi2, yi2);
	
	// Calcule les deux distances par rapport à la balise
	double distBaliseI1 = calculerDistance(xBalise, yBalise, xi1, yi1);
	double distBaliseI2 = calculerDistance(xBalise, yBalise, xi2, yi2);
	
	// Le point qui a la distance la plus faible par rapport à la balise correspond à l'intersection qui ne nous intéresse pas
	if ( distBaliseI1 < distBaliseI2 ) {
		*xP = xi2;
		*yP = yi2;
	} else {
		*xP = xi1;
		*yP = yi1;
	}

}
