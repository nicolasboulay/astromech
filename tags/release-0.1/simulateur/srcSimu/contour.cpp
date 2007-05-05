#include "module.h"
#include "systeme.h"
#include "senseur.h"
#include "contour.h"
#include "trace.h"
#include "tinyxml.h"
#include "geom2D.h"
#include "simulation.h"
#include <math.h>
using namespace std;

Contour::Contour(Systeme *s,Trace * t):Senseur(s,t)
{
  trace = t;
  src = "contour.cpp";
  cl = "Contour";
  message = "debut";
  trace->print(src,cl,"Contour",message);
    
  message = "fin";
  trace->print(src,cl,"Contour",message);
}

Contour::~Contour(void)
{
  message = "debut";
  trace->print(src,cl,"~Contour",message);
  vectorSegment2D::iterator it;
  for (it = tabSegments_Contour.begin();it!=tabSegments_Contour.end();++it)
  {
    delete(*it);
  }

  message = "fin";
  trace->print(src,cl,"~Contour",message);
}

void Contour::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  vectorSegment2D contourReferentielTerrain;
  
  //initialiser la liste des collisions avec des systemes et des obstacles
  tabPointContacts_Obstacles.clear();
  tabPointContacts_Systemes.clear();
  tabSystemesEnContact.clear();
  
  //calcul de la position absolue des segments du contour
  vectorSegment2D::iterator it;
  Segment2D *seg;
  Segment2D *segRT;
  for (it = tabSegments_Contour.begin();it!=tabSegments_Contour.end();++it)
  {
    seg = *it;
    segRT = seg->creeSegmentRT(positionCourante,attitudeCourante_rad);
    contourReferentielTerrain.push_back(segRT);
  }

  //calcul de la boite associée au contour
  Boite2D * boiteContourRT = new Boite2D(contourReferentielTerrain);

  //recherche dans la liste des systemes si une boite intersecte la presente boite
  vectorSysteme tabSystemes = simuPere->getTabSystemes();
  vectorSysteme::iterator itSys;
  Systeme * sys;
  for (itSys = tabSystemes.begin();itSys!=tabSystemes.end();++itSys)
  {
    sys = *itSys;
    Contour * pContourSys;
    pContourSys = sys->getPt_Contour();
    if (pContourSys != this)
    {
      vectorSegment2D contourSystCourantReferentielTerrain;
      vectorSegment2D contourSystCourant = pContourSys->tabSegments_Contour;
      for (it = contourSystCourant.begin();it!=contourSystCourant.end();++it)
      {
        seg = *it;
        segRT = seg->creeSegmentRT(sys->getPositionCourante(),sys->getAttitudeCourante_rad());
        contourSystCourantReferentielTerrain.push_back(segRT);
      }
      Boite2D * boiteContourCourantRT = new Boite2D(contourSystCourantReferentielTerrain);
	  
      if (boiteContourRT->IntersectionAvecBoite(boiteContourCourantRT))
      {
        log << tempsCourant_ms <<"\t"<< "intersection" <<endl;
	intersection = true;
      }
      else
      {
        log << tempsCourant_ms <<"\t"<< " pas intersection" <<endl;
	intersection = false;
      }
      delete boiteContourCourantRT;
      for (it = contourSystCourantReferentielTerrain.begin();it!=contourSystCourantReferentielTerrain.end();++it)
      {
        segRT = *it;
        delete segRT;
      }
    }
  }

  //recherche plus precise de l'intersection  
  
  //recherche si contact avec un obstacle
  
  //recherche plus precise de l'intersection
  
  //Pour chaque intersection avec un systeme different  
       //recherche du point de contact
  
  delete boiteContourRT;
  for (it = contourReferentielTerrain.begin();it!=contourReferentielTerrain.end();++it)
  {
    segRT = *it;
    delete segRT;
  }
  message = "fin";
  trace->print(src,cl,"execute",message);
}


void Contour::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Senseur::chargerXML(pModuleXML);
  
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);

  //chargement des segments
  TiXmlElement* pSeg = handleModule.FirstChildElement("Segment2D").Element();
  for( pSeg; pSeg; pSeg=pSeg->NextSiblingElement("Segment2D"))
  {
    Segment2D * s2D = new Segment2D(pSeg);
    message = "segment";
    trace->print(src,cl,"chargerXML",message);
    tabSegments_Contour.push_back(s2D);
  }
    
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void Contour::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  message = "fin";
  trace->print(src,cl,"connectModules",message);
}
bool Contour::intersectionEnCours(void)
{
  return intersection;
}
