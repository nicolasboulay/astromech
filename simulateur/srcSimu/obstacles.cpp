#include "obstacles.h"
#include "tinyxml.h"
#include "geom2D.h"
#include "trace.h"
#include "conversion.h"

using namespace std;


/*Obstacles::Obstacles(string fichierObstacles,Trace * t)
{
  trace = t;
  src = "obstacles.cpp";
  cl = "Obstacles";
  message = "debut";
  trace->print(src,cl,"Obstacles",message);
  TiXmlDocument doc( fichierObstacles );
  if(!doc.LoadFile())
  {
    message = "Problème doc.LoadFile() "+ fichierObstacles;
    trace->print(src,cl,"Obstacles",message);
    exit(1);   
  }
  else
  {
    if(!chargerXML(doc))
    {
      message = "Problème après appel chargerXML ";
      trace->print(src,cl,"Obstacles",message);
      exit(1);
    }
  }
  message = "fin";
  trace->print(src,cl,"Obstacles",message);
}*/
Obstacles::Obstacles(vectorSegment2D tabSeg, Trace * t)
{
  Trace * trace;
  trace = t;
  src = "obstacles.cpp";
  cl = "Obstacles";
  message = "debut";
  trace->print(src,cl,"Obstacles",message);
  tabSegments = tabSeg;
  message = "fin";
  trace->print(src,cl,"Obstacles",message);
}
Obstacles::Obstacles(TiXmlElement* pObs,Trace * t)
{
  trace = t;
  src = "obstacles.cpp";
  cl = "Obstacles";
  message = "debut";
  trace->print(src,cl,"Obstacles",message);
  if(!chargerXML(pObs))
  {
    message = "Problème après appel chargerXML ";
    trace->print(src,cl,"Obstacles",message);
    exit(1);
  }
  message = "fin";
  trace->print(src,cl,"Obstacles",message);
}

Obstacles::~Obstacles(void)
{
  message = "debut";
  trace->print(src,cl,"~Obstacles",message);
  vectorSegment2D::iterator it;
  for (it = tabSegments.begin();it!=tabSegments.end();++it)
  {
    delete(*it);
  }
  message = "fin";
  trace->print(src,cl,"~Obstacles",message);
}

int Obstacles::chargerXML(TiXmlElement* pObs)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  
  TiXmlHandle handleObstacle(0);
  handleObstacle = TiXmlHandle(pObs);
  Point2D *pt1;
  Point2D *pt2;
  vectorpoint2D tabPointObstacle;
  
  //chargement des points
  TiXmlElement* pPt = handleObstacle.FirstChildElement("Point2D").Element();

  for( pPt; pPt; pPt=pPt->NextSiblingElement("Point2D"))
  {
    pt1 = new Point2D(pPt);
    tabPointObstacle.push_back(pt1);
  }
  
  //création des segments à partir des points
  for (int indicePt=0;indicePt<tabPointObstacle.size()-1;indicePt++)
  {
    pt2 = new Point2D(tabPointObstacle[indicePt+1]->x,tabPointObstacle[indicePt+1]->y);
    Segment2D *s2D = new Segment2D(tabPointObstacle[indicePt],pt2);
    tabSegments.push_back(s2D);
  }
  pt2 = new Point2D(tabPointObstacle[0]->x,tabPointObstacle[0]->y);
  Segment2D *s2D = new Segment2D(tabPointObstacle[tabPointObstacle.size()-1],pt2);
  tabSegments.push_back(s2D);
  
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
  return 1;
}

Obstacles* Obstacles::dilateObstacle(double dilatation_m)
{
  vectorSegment2D tabSegDilate;
  vectorpoint2D tabPtDilate;
  Segment2D *segment1;
  Segment2D *segment2;
  Point2D *ptDilate1;
  Point2D *ptDilate2;
  int codeRetour = 0;
  
  message = "debut";
  trace->print(src,cl,"dilateObstacle",message);
  
  /* Parcourt de la liste des segments de l'obstacle */
  for (int IndiceSeg=0;IndiceSeg<tabSegments.size()-1;IndiceSeg++)
  {
    /* calcul des points dilatés et ajout à une liste des points dilatés */
    segment1 = tabSegments[IndiceSeg];
    segment2 = tabSegments[IndiceSeg+1];
    ptDilate1 = new Point2D(0,0);
    ptDilate2 = new Point2D(0,0);
    codeRetour = segment1->CalculePointJonctionDilate(segment2,dilatation_m,ptDilate1,ptDilate2);
    if (codeRetour == -1)
    {
      return NULL;
    }
    else
    {
      tabPtDilate.push_back(ptDilate1);
      if (codeRetour == 2)
      {
        tabPtDilate.push_back(ptDilate2);
      }
      else
      {
        delete ptDilate2;
        ptDilate2 = NULL;
      }
    }
  }
  segment1 = tabSegments[tabSegments.size()-1];
  segment2 = tabSegments[0];
  ptDilate1 = new Point2D(0,0);
  ptDilate2 = new Point2D(0,0);
  codeRetour = segment1->CalculePointJonctionDilate(segment2,dilatation_m,ptDilate1,ptDilate2);
  if (codeRetour == -1)
  {
    return NULL;
  }
  else
  {
    tabPtDilate.push_back(ptDilate1);
    if (codeRetour == 2)
    {
      tabPtDilate.push_back(ptDilate2);
    }
    else
    {
      delete ptDilate2;
      ptDilate2 = NULL;
    }
  }
  
  message = "nb de points dilatés : "+ int2String(tabPtDilate.size());
  trace->print(src,cl,"dilateObstacle",message);
  
  /* passage d'une liste de points à une liste de segments dilatés */
  tabSegDilate.clear();
  for (int indicePt=0;indicePt<tabPtDilate.size()-1;indicePt++)
  {
    Point2D *pt2 = new Point2D(tabPtDilate[indicePt+1]->x,tabPtDilate[indicePt+1]->y);
    Segment2D *s2D = new Segment2D(tabPtDilate[indicePt],pt2);
    tabSegDilate.push_back(s2D);
  }
  Point2D *pt2 = new Point2D(tabPtDilate[0]->x,tabPtDilate[0]->y);
  Segment2D *s2D = new Segment2D(tabPtDilate[tabPtDilate.size()-1],pt2);
  tabSegDilate.push_back(s2D);
  
  message = "nb de segments dilatés : "+ int2String(tabSegDilate.size());
  trace->print(src,cl,"dilateObstacle",message);
  
  /* création d'un nouvel Obstacles à partir de la liste de segments */
  Obstacles * obstacleDilate = new Obstacles(tabSegDilate,trace);
  
  message = "fin";
  trace->print(src,cl,"dilateObstacle",message);

  return obstacleDilate;
}

Obstacles* Obstacles::autoFusion(void)
{
  vectorSegment2D tabSegFusionne;
  vectorpoint2D tabPtFusionne;
  vectorpoint2D tabPtInterPourSegCrt; //regroupe tous les pts d'intersection pour le segment courant
  vectorInt     tabIdSegInterctantSegCrt;
  
  message = "debut";
  trace->print(src,cl,"autoFusion",message);
  Segment2D *segCrt;
  Segment2D *segSec;
  bool calculIntersection = true;
  int codeRetour = -1;
  Point2D *ptInter = new Point2D(0,0);
  
  tabPtFusionne.clear();
  for (int indexSegCrt=0;indexSegCrt<tabSegments.size()-2;indexSegCrt++)
  {
    segCrt = tabSegments[indexSegCrt];
    tabPtInterPourSegCrt.clear();
    tabIdSegInterctantSegCrt.clear();
    for (int indexSegSec=indexSegCrt+2;indexSegSec<tabSegments.size();indexSegSec++ )
    {
      if ((indexSegCrt!=0)||(indexSegSec!=tabSegments.size()-1))
      {
        segSec = tabSegments[indexSegSec];
        codeRetour = segCrt->TestIntersection(segSec,calculIntersection,ptInter);
 
        if (codeRetour == 1)
        {
          //intersection entre le segment segCrt et segSec
	  message = "intersection trouvee :" + double2String(ptInter->x)+ " "+ double2String(ptInter->y);
          trace->print(src,cl,"autoFusion",message);	  
	  Point2D *ptInterAconserver = new Point2D(ptInter->x,ptInter->y);
	  tabPtInterPourSegCrt.push_back(ptInterAconserver);
	  tabIdSegInterctantSegCrt.push_back(indexSegSec);
        }
      }
    }
    
    if (tabPtInterPourSegCrt.size()>0)
    {
      //Il y a eu au moins une intersection
      //On cherche la plus proche du pt1 du segCrt
      int indicePtInterlePlusProche = 0;
      double distancePtInterPt1segCrt = 0;
      double distancePtInterLaPlusFaible = 10000000;
      for (int indicePtInter = 0;indicePtInter<tabPtInterPourSegCrt.size();indicePtInter++)
      {
        distancePtInterPt1segCrt = segCrt->pt1->distanceAuPoint(tabPtInterPourSegCrt[indicePtInter]);
	if (distancePtInterLaPlusFaible>distancePtInterPt1segCrt)
	{
	  distancePtInterLaPlusFaible = distancePtInterPt1segCrt;
	  indicePtInterlePlusProche = indicePtInter;
	}
      }
      
      
      // on ajoute le pt1 du segCrt et le point d'intersection
      // si ils sont différents
      Point2D * pt1SegCrt = new Point2D(segCrt->pt1->x,segCrt->pt1->y);
      Point2D * ptInterAajouter = new Point2D(tabPtInterPourSegCrt[indicePtInterlePlusProche]->x,tabPtInterPourSegCrt[indicePtInterlePlusProche]->y);

      if (!((pt1SegCrt->x == ptInterAajouter->x)&&(pt1SegCrt->y == ptInterAajouter->y)))
      {
        tabPtFusionne.push_back(pt1SegCrt);
      }
      tabPtFusionne.push_back(ptInterAajouter);
      
      message = "intersection ajout du pt1 :" + double2String(pt1SegCrt->x)+ " "+ double2String(pt1SegCrt->y);
      trace->print(src,cl,"autoFusion",message);
      message = "intersection ajout du pt intersection :" + double2String(ptInterAajouter->x)+ " "+ double2String(ptInterAajouter->y);
      trace->print(src,cl,"autoFusion",message);
      
      // on efface les points d'intersection
      for (int indicePtInter = 0;indicePtInter<tabPtInterPourSegCrt.size();indicePtInter++)
      {
        delete tabPtInterPourSegCrt[indicePtInter];
      }
      tabPtInterPourSegCrt.clear();
      // on poursuit la fusion à partir du segement rencontré
      indexSegCrt = tabIdSegInterctantSegCrt[indicePtInterlePlusProche];
      
      //Le point d'intersection doit devenir le Pt1 du segment rencontré
      tabSegments[indexSegCrt]->pt1->x = ptInterAajouter->x; 
      tabSegments[indexSegCrt]->pt1->y = ptInterAajouter->y;
    }
    else
    {
      //pas d'intersection. On ajoute le Pt1 du seg crt
      Point2D * pt1SegCrt = new Point2D(segCrt->pt1->x,segCrt->pt1->y);
      tabPtFusionne.push_back(pt1SegCrt);
      
      message = "ajout du pt1 :" + double2String(pt1SegCrt->x)+ " "+ double2String(pt1SegCrt->y);
      trace->print(src,cl,"autoFusion",message);
    }
  }
  //Il faut également ajouter les derniers points pour refermer le contour
  Segment2D * avantDernierSeg = tabSegments[tabSegments.size()-2];
  Point2D * pt1AvantDernierSeg = new Point2D(avantDernierSeg->pt1->x,avantDernierSeg->pt1->y);
  tabPtFusionne.push_back(pt1AvantDernierSeg);
  
  Segment2D * dernierSeg = tabSegments[tabSegments.size()-1];
  Point2D * pt1DernierSeg = new Point2D(dernierSeg->pt1->x,dernierSeg->pt1->y);
  tabPtFusionne.push_back(pt1DernierSeg);
  
  // la tabPtFusionne doit etre transforme en tabSegFusionne
  /* passage d'une liste de points à une liste de segments */
  tabSegFusionne.clear();
  for (int indicePt=0;indicePt<tabPtFusionne.size()-1;indicePt++)
  {
    Point2D *pt2 = new Point2D(tabPtFusionne[indicePt+1]->x,tabPtFusionne[indicePt+1]->y);
    Segment2D *s2D = new Segment2D(tabPtFusionne[indicePt],pt2);
    tabSegFusionne.push_back(s2D);
  }
  Point2D *pt2 = new Point2D(tabPtFusionne[0]->x,tabPtFusionne[0]->y);
  Segment2D *s2D = new Segment2D(tabPtFusionne[tabPtFusionne.size()-1],pt2);
  tabSegFusionne.push_back(s2D);
  // création d'un nouvel Obstacles à partir de la liste de segments
  Obstacles * obstacleFusionne = new Obstacles(tabSegFusionne,trace);
  
  message = "fin";
  trace->print(src,cl,"autoFusion",message);
  
  return obstacleFusionne;
}
int Obstacles::FusionneAvecObstacle(Obstacles* obs)
{
  //si l'obstacle this et obs sont fusionnes en this, retourne 1
  //0 si pas de contact entre this et obs
  int codeRetour;
  int indexSegFinal;
  Segment2D *segCrt;
  Segment2D *segSec;
  bool ptDansPolygone;
  
  message = "debut";
  trace->print(src,cl,"FusionneAvecObstacle",message);
/*  
  //il faut dans un premier temps s'assurer que le point de départ de "this" est bien à l'extérieur
  //du polygone obs. Si ce n'est pas le cas, il faut tester le point suivant.
  for (int indexSegCrt=0;indexSegCrt<tabSegments.size();indexSegCrt++)
  {
    ptDansPolygone = obs->pointInPolygon(tabSegments[indexSegCrt]->pt1);
    if (ptDansPolygone == false)
    { 
      break;
    }
  }
  
  if ((indexSegCrt == tabSegments.size()-1)&&(ptDansPolygone == true))
  {
    //tous les points de this sont dans obs
    //on arrete les frais ici
    return 0;
  }
  
  //En commençant à indexSegCrt, on va suivre le contour exterieur des 2 polygones jusqu'à retrouver le 
  //point de départ.
  segCrt = tabSegments[indexSegCrt];
  indexSegFinal = (tabSegments.size()+indexSegCrt-1)%(tabSegments.size())
  while (indexSegCrt != indexSegFinal)
  {
  
  }
  
  for (int indexSegCrt=0;indexSegCrt<tabSegments.size();indexSegCrt++)
  {
    segCrt = tabSegments[indexSegCrt];
    for (int indexSegObs=0;indexSegObs<obs->tabSegments.size();indexSegObs++ )
    {
      segSec = obs->tabSegments[indexSegObs];
      codeRetour = segCrt->TestIntersection(segSec,calculIntersection,ptInter);
      
      if (codeRetour == 1)
      {
        //intersection entre le segment segCrt et segSec
	message = "intersection trouvee :" + double2String(ptInter->x)+ " "+ double2String(ptInter->y);
        trace->print(src,cl,"FusionneAvecObstacle",message);	  
	Point2D *ptInterAconserver = new Point2D(ptInter->x,ptInter->y);
	tabPtInterPourSegCrt.push_back(ptInterAconserver);
	tabIdSegInterctantSegCrt.push_back(indexSegSec);
      }	
    }
  }*/
  
  message = "fin";
  trace->print(src,cl,"FusionneAvecObstacle",message);
}
void Obstacles::dumpObstacle(void)
{ 
  message = "début du Dump";
  trace->print(src,cl,"dumpObstacle",message);
 
  message = "nb segments : " + int2String(tabSegments.size());
  trace->print(src,cl,"dumpObstacle",message);
  
  Segment2D *segCrt;
  for (int indexSegCrt=0;indexSegCrt<tabSegments.size();indexSegCrt++)
  {
   segCrt = tabSegments[indexSegCrt];
   message = "(" + double2String(segCrt->pt1->x) +","+double2String(segCrt->pt1->y)+") ("+double2String(segCrt->pt2->x) +","+double2String(segCrt->pt2->y)+")";
   trace->print(src,cl,"dumpObstacle",message);
  }
  message = "fin du Dump";
  trace->print(src,cl,"dumpObstacle",message);
}

void Obstacles::dumpObstacleFormatExcel(void)
{
  message = "début du Dump";
  trace->print(src,cl,"dumpObstacleFormatExcel",message);
 
  message = "nb segments : " + int2String(tabSegments.size());
  trace->print(src,cl,"dumpObstacleFormatExcel",message);
  
  Segment2D *segCrt;
  for (int indexSegCrt=0;indexSegCrt<tabSegments.size();indexSegCrt++)
  {
   segCrt = tabSegments[indexSegCrt];
   message = "\t\t\t"+double2String(segCrt->pt1->x) +"\t"+double2String(segCrt->pt1->y);
   trace->print(src,cl,"dumpObstacleFormatExcel",message);
  }
  message = "fin du Dump";
  trace->print(src,cl,"dumpObstacleFormatExcel",message);
}
















//  Globals which should be set before calling this function:
//
//  int    polySides  =  how many corners the polygon has
//  float  polyX[]    =  horizontal coordinates of corners
//  float  polyY[]    =  vertical coordinates of corners
//  float  x, y       =  point to be tested
//
//  (Globals are used in this example for purposes of speed.
//  Change as desired.)
//
//  The function will return TRUE if the point x,y is inside the
//  polygon, or FALSE if it is not. If the point x,y is exactly on
//  the edge of the polygon, then the function may return TRUE or
//  FALSE.
//
//  Note that division by zero is avoided because the division is
//  protected by the "if" clause which surrounds it.
//http://www.alienryderflex.com/polygon/
bool Obstacles::pointInPolygon(Point2D *ptATester) 
{
  bool  oddNODES=false ;
  int j = 0;
  for (int i=0; i<tabSegments.size(); i++) 
  {
    j++;
    if (j==tabSegments.size()) 
    {
      j=0;
    }
    if((tabSegments[i]->pt1->y < ptATester->y && tabSegments[j]->pt1->y >= ptATester->y)
    || (tabSegments[j]->pt1->y < ptATester->y && tabSegments[i]->pt1->y >= ptATester->y)) 
    {
      if (tabSegments[i]->pt1->x+(ptATester->y-tabSegments[i]->pt1->y)/(tabSegments[j]->pt1->y-tabSegments[i]->pt1->y )*(tabSegments[j]->pt1->x-tabSegments[i]->pt1->x )<ptATester->x)
      {
        oddNODES=!oddNODES; 
      }
    }
  }
  return oddNODES;
}
