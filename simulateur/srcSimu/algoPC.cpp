#include "module.h"
#include "algo.h"
#include "algoPC.h"
#include "systeme.h"
#include "trace.h"
#include "tinyxml.h"
#include "comSeriePC_PIC.h"
#include "geom2D.h"
#include "bit.h"
#include "algoPlanifTraj.h"
#include "obstacles.h"
#include <math.h>

using namespace std;

AlgoPC::AlgoPC(Systeme *s,Trace * t):Algo(s,t)
{
  trace = t;
  src = "algoPC.cpp";
  cl = "AlgoPC";
  message = "debut";
  trace->print(src,cl,"AlgoPC",message);
  tempsDerniereExecution_ms = -100000;
  indiceListeEnCours = 0;
  u8_numeroAbsoluWP1 = 0;
  u8_numeroWP1_dansListe = 0;

  trameEnvoyee = new TramePC_PIC();
  trameRecue = new TramePIC_PC();

  algoPlanifTraj = new AlgoPlanifTrajectoire(1.0,//accelNormalMax
                                             3.0,//longueurTerrain_m
                                             2.1,//largeurTerrain_m
                                             0.2,t);//rayonRobot_m
  
  message = "fin";
  trace->print(src,cl,"AlgoPC",message);
}

AlgoPC::~AlgoPC(void)
{
  message = "debut";
  trace->print(src,cl,"~AlgoPC",message);
  delete(algoPlanifTraj);
  message = "fin";
  trace->print(src,cl,"~AlgoPC",message);
}

void AlgoPC::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  
  if ((tempsCourant_ms - tempsDerniereExecution_ms) >= 1000/(frequenceExecution_Hz))
  {
    cout <<"****** Execution boucle PC à t= "<<tempsCourant_ms<<"*************************************************************"<<endl;
    if (comSerie->tramePIC_PC_recue())
    {
      cout<<"trame PIC PC recue"<<endl;
      comSerie->getTramePIC_PC(trameRecue);
      cout<<trameRecue->tramePIC_PCToString();
      //lecture trame et sauvegarde des données en local
    }
    
    //traitements
    //code temporaire: recalage de la navigation à t=0
    if (tempsCourant_ms==0)
    {
      setBit(BIT_UPDATE,trameEnvoyee->u8_NAV_CTRL);
      trameEnvoyee->s16_NEW_POS_X_mm = 300;
      trameEnvoyee->s16_NEW_POS_Y_mm = 300;
      trameEnvoyee->u16_NEW_CAP_deg  = 0;
      trameEnvoyee->s16_QUAT_W = 0;
      trameEnvoyee->s16_QUAT_Z = 0;
    }
    else
    {
      clearBit(BIT_UPDATE,trameEnvoyee->u8_NAV_CTRL);
    }
    
    
    //code temporaire: creation d'obstacles
    //et waypointDepart et arrivee
    Obstacle *obs1 = new Obstacle(1.0,2.0,0.5); //x,y,r
    Obstacle *obs2 = new Obstacle(2.0,1.0,0.3); //x,y,r
    vectorObstacles tableDesObstacles;
    tableDesObstacles.push_back(obs1);
    tableDesObstacles.push_back(obs2);
    
    WayPoint wayPointDepart(0.0,0.0,0.0,0.0);
    wayPointDepart.pt->x = (double)trameRecue->s16_POS_X_mm/1000.0;
    wayPointDepart.pt->y = (double)trameRecue->s16_POS_Y_mm/1000.0;;
    wayPointDepart.cap_deg = trameRecue->u16_CAP_deg;
    wayPointDepart.vitesse_m_par_s = (double)trameRecue->u8_VITESSE_cm_par_s/100;
    
    WayPoint wayPointArrivee(0.0,0.0,0.0,0.0);
    wayPointArrivee.pt->x = 1.8;
    wayPointArrivee.pt->y = 2.5;
    wayPointArrivee.cap_deg = 20.0;
    wayPointArrivee.vitesse_m_par_s = 0.3;
    
    algoPlanifTraj->calculeTrajectoire(//IN
                                       tableDesObstacles,
                                       &wayPointDepart,
			               &wayPointArrivee,
			               //OUT
			               listeWP);
    
    
    //temporaire: une seule traj est envoyee composee des waypoints contenus dans le .xml    
    gereFluxWayPoints(listeWP,0);
    //envoi d'une trame
    comSerie->sendToPIC(trameEnvoyee);
    cout<<trameEnvoyee->tramePC_PICToString();
    
    tempsDerniereExecution_ms = tempsCourant_ms;
    log << tempsCourant_ms << endl;
  }
  
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void AlgoPC::gereFluxWayPoints(vectorWayPoint listeWPCourante, int indiceListe)
{
  unsigned char u8_numeroWayPointEnCoursPIC;
  bool dernierWayPoint = false;
  bool dernierWayPointDepasse = false;
  u8_numeroWayPointEnCoursPIC = trameRecue->u8_NUM_WP_CRT;

  if (indiceListe==indiceListeEnCours)
  {
    //vérification du n° de waypoint en cours
    cout<<"u8_numeroWayPointEnCoursPIC "<<(int)u8_numeroWayPointEnCoursPIC<<endl;
    cout<<"u8_numeroAbsoluWP1 "<<(int)u8_numeroAbsoluWP1<<endl;
    if (u8_numeroWayPointEnCoursPIC==u8_numeroAbsoluWP1)
    {
      //renvoi du meme waypoint
    }
    else
    {
      //le pic vient de transitioner de waypoint
      u8_numeroAbsoluWP1++;
      u8_numeroWP1_dansListe++; 
    }
    
    if (u8_numeroWP1_dansListe==listeWPCourante.size()-1)
    {
      //on ne transmet que le dernier WP dans la "case" WP1
      dernierWayPoint = true;
    }
    if (u8_numeroWP1_dansListe>listeWPCourante.size()-1)
    {
      dernierWayPointDepasse = true;
      //envoi du WP nul aux "cases" WP1 et WP2
    }

    if (dernierWayPointDepasse==false)
    {
      double cap_rad = listeWPCourante[u8_numeroWP1_dansListe]->cap_deg;
      cap_rad *=M_PI/180;
      trameEnvoyee->u8_NUM_WP1          = u8_numeroAbsoluWP1;
      trameEnvoyee->s16_POS_X_WP1_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe]->pt->x;
      trameEnvoyee->s16_POS_Y_WP1_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe]->pt->y;
      trameEnvoyee->u16_CAP_WP1_deg     = listeWPCourante[u8_numeroWP1_dansListe]->cap_deg;
      trameEnvoyee->s16_XCAP_WP1_mm     = 1000*sin(cap_rad);
      trameEnvoyee->s16_YCAP_WP1_mm     = 1000*cos(cap_rad);
      trameEnvoyee->u8_VIT_WP1_cm_par_s = 100*listeWPCourante[u8_numeroWP1_dansListe]->vitesse_m_par_s;
      trameEnvoyee->u8_CTRL_WP1         = listeWPCourante[u8_numeroWP1_dansListe]->controlByte;
    
    
    if (dernierWayPoint==false)
    {
      double cap_rad = listeWPCourante[u8_numeroWP1_dansListe+1]->cap_deg;
      cap_rad *=M_PI/180;
      trameEnvoyee->u8_NUM_WP2           = u8_numeroAbsoluWP1+1;
      trameEnvoyee->s16_POS_X_WP2_mm     = 1000*listeWPCourante[u8_numeroWP1_dansListe+1]->pt->x;
      trameEnvoyee->s16_POS_Y_WP2_mm     = 1000*listeWPCourante[u8_numeroWP1_dansListe+1]->pt->y;
      trameEnvoyee->u16_CAP_WP2_deg      = listeWPCourante[u8_numeroWP1_dansListe+1]->cap_deg;
      trameEnvoyee->s16_XCAP_WP2_mm      = 1000*sin(cap_rad);
      trameEnvoyee->s16_YCAP_WP2_mm      = 1000*cos(cap_rad);
      trameEnvoyee->u8_VIT_WP2_cm_par_s  = 100*listeWPCourante[u8_numeroWP1_dansListe+1]->vitesse_m_par_s;
      trameEnvoyee->u8_CTRL_WP2          = listeWPCourante[u8_numeroWP1_dansListe+1]->controlByte;
    }
    else
    {
      double cap_rad = listeWPCourante[u8_numeroWP1_dansListe]->cap_deg;
      cap_rad *=M_PI/180;
      trameEnvoyee->u8_NUM_WP2          = u8_numeroAbsoluWP1;
      trameEnvoyee->s16_POS_X_WP2_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe]->pt->x;
      trameEnvoyee->s16_POS_Y_WP2_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe]->pt->y;
      trameEnvoyee->u16_CAP_WP2_deg     = listeWPCourante[u8_numeroWP1_dansListe]->cap_deg;
      trameEnvoyee->s16_XCAP_WP2_mm     = 1000*sin(cap_rad);
      trameEnvoyee->s16_YCAP_WP2_mm     = 1000*cos(cap_rad);
      trameEnvoyee->u8_VIT_WP2_cm_par_s = 100*listeWPCourante[u8_numeroWP1_dansListe]->vitesse_m_par_s;
      trameEnvoyee->u8_CTRL_WP2         = listeWPCourante[u8_numeroWP1_dansListe]->controlByte;
    }
    }
    if (dernierWayPointDepasse)
    {
      cout<<"###########dernierWayPointDepasse#####################################"<<endl;
    }
  }
  else
  {
    //remplacement immédiat du waypoint 1
    u8_numeroWP1_dansListe = 0;
    indiceListeEnCours = indiceListe;
    double cap_rad = listeWPCourante[u8_numeroWP1_dansListe]->cap_deg;
    cap_rad *=M_PI/180;
    trameEnvoyee->u8_NUM_WP1          = u8_numeroAbsoluWP1;
    trameEnvoyee->s16_POS_X_WP1_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe]->pt->x;
    trameEnvoyee->s16_POS_Y_WP1_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe]->pt->y;
    trameEnvoyee->u16_CAP_WP1_deg     = listeWPCourante[u8_numeroWP1_dansListe]->cap_deg;
    trameEnvoyee->s16_XCAP_WP1_mm     = 1000*sin(cap_rad);
    trameEnvoyee->s16_YCAP_WP1_mm     = 1000*cos(cap_rad);
    trameEnvoyee->u8_VIT_WP1_cm_par_s = 100*listeWPCourante[u8_numeroWP1_dansListe]->vitesse_m_par_s;
    trameEnvoyee->u8_CTRL_WP1         = listeWPCourante[u8_numeroWP1_dansListe]->controlByte;
    
    if (listeWPCourante.size()>=2)
    {
      //positionnement du waypoint 2 à suivre
      double cap_rad = listeWPCourante[u8_numeroWP1_dansListe+1]->cap_deg;
      cap_rad *=M_PI/180;
      trameEnvoyee->u8_NUM_WP2          = u8_numeroAbsoluWP1+1;
      trameEnvoyee->s16_POS_X_WP2_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe+1]->pt->x;
      trameEnvoyee->s16_POS_Y_WP2_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe+1]->pt->y;
      trameEnvoyee->u16_CAP_WP2_deg     = listeWPCourante[u8_numeroWP1_dansListe+1]->cap_deg;
      trameEnvoyee->s16_XCAP_WP2_mm     = 1000*sin(cap_rad);
      trameEnvoyee->s16_YCAP_WP2_mm     = 1000*cos(cap_rad);
      trameEnvoyee->u8_VIT_WP2_cm_par_s = 100*listeWPCourante[u8_numeroWP1_dansListe+1]->vitesse_m_par_s;
      trameEnvoyee->u8_CTRL_WP2         = listeWPCourante[u8_numeroWP1_dansListe+1]->controlByte;
    }
    else
    {
      double cap_rad = listeWPCourante[u8_numeroWP1_dansListe]->cap_deg;
      cap_rad *=M_PI/180;
      trameEnvoyee->u8_NUM_WP2          = u8_numeroAbsoluWP1;
      trameEnvoyee->s16_POS_X_WP2_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe]->pt->x;
      trameEnvoyee->s16_POS_Y_WP2_mm    = 1000*listeWPCourante[u8_numeroWP1_dansListe]->pt->y;
      trameEnvoyee->u16_CAP_WP2_deg     = listeWPCourante[u8_numeroWP1_dansListe]->cap_deg;
      trameEnvoyee->s16_XCAP_WP2_mm     = 1000*sin(cap_rad);
      trameEnvoyee->s16_YCAP_WP2_mm     = 1000*cos(cap_rad);
      trameEnvoyee->u8_VIT_WP2_cm_par_s = 100*listeWPCourante[u8_numeroWP1_dansListe]->vitesse_m_par_s;
      trameEnvoyee->u8_CTRL_WP2         = listeWPCourante[u8_numeroWP1_dansListe]->controlByte;
    }
  }
}

bool AlgoPC::listeWayPointsTerminee(vectorWayPoint listeWPCourante)
{
  if (u8_numeroWP1_dansListe>listeWPCourante.size()-1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void AlgoPC::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Algo::chargerXML(pModuleXML);
      
  TiXmlElement* pElt;
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);
  
  pElt = handleModule.FirstChild("frequenceExecution_Hz").Element();
  istringstream iss( pElt->GetText() );
  iss >> frequenceExecution_Hz;
  message = "frequenceExecution_Hz : " + iss.str();
  trace->print(src,cl,"chargerXML",message);
  
  //chargement des segments
  TiXmlElement* pWP = handleModule.FirstChildElement("wayPoint").Element();
  for( pWP; pWP; pWP=pWP->NextSiblingElement("wayPoint"))
  {
    WayPoint * wp = new WayPoint(pWP);
    listeWP.push_back(wp);
  }
  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void AlgoPC::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  string nomModule;
  vectorString::iterator it;
  it = tabRelation.begin();
  if (tabRelation.size() == 1)
  {
    nomModule = *it;
    comSerie = (ComSeriePC_PIC *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
  }
  
  message = "fin";
  trace->print(src,cl,"connectModules",message);
}

void AlgoPC::calculerPosition( double & angleGisement1, double & angleGisement2, double & angleGisement3, double & capRobot, double & posX, double & posY )
{
  // Conversion des angles de gisement en angle trigo et en radians
  double angleGisement1_t = 2*M_PI - angleGisement1*M_PI/180;
  double angleGisement2_t = 2*M_PI - angleGisement2*M_PI/180;
  double angleGisement3_t = 2*M_PI - angleGisement3*M_PI/180;
  
  double dt12 = DistanceAngulaire(angleGisement1_t, angleGisement2_t);
  double dt23 = DistanceAngulaire(angleGisement2_t, angleGisement3_t);
  double dt31 = DistanceAngulaire(angleGisement3_t, angleGisement1_t);
  
  // calcul des cercles d'intersection
  double xC1, yC1, R1;
  double xC2, yC2, R2;
  double xC3, yC3, R3;
  
  calculerCercle(dt12, POS_X_BALISE_1, POS_Y_BALISE_1, POS_X_BALISE_2, POS_Y_BALISE_2, &xC1, &yC1, &R1);
  //printf("Rayon : %f\n", R1);
  //printf("Centre: %f %f\n\n", xC1, yC1);
  
  calculerCercle(dt23, POS_X_BALISE_2, POS_Y_BALISE_2, POS_X_BALISE_3, POS_Y_BALISE_3, &xC2, &yC2, &R2);
  //printf("Rayon : %f\n", R2);
  //printf("Centre: %f %f\n\n", xC2, yC2);
  
  calculerCercle(dt31, POS_X_BALISE_3, POS_Y_BALISE_3, POS_X_BALISE_1, POS_Y_BALISE_1, &xC3, &yC3, &R3);
  //printf("Rayon : %f\n", R3);
  //printf("Centre: %f %f\n\n", xC3, yC3);
  
  // calcul des intersections des cercles
  
  double xP1, yP1, xP2, yP2, xP3, yP3;
  calculerPositionTrigo(POS_X_BALISE_1, POS_Y_BALISE_1, xC3, yC3, R3, xC1, yC1, R1, &xP1, &yP1);
  calculerPositionTrigo(POS_X_BALISE_2, POS_Y_BALISE_2, xC1, yC1, R1, xC2, yC2, R2, &xP2, &yP2);
  calculerPositionTrigo(POS_X_BALISE_3, POS_Y_BALISE_3, xC2, yC2, R2, xC3, yC3, R3, &xP3, &yP3);

  //printf("Position : %f %f [%f]\n", xP1, yP1, calculerDistance(xReel, yReel, xP1, yP1));
  //printf("Position : %f %f [%f]\n", xP2, yP2, calculerDistance(xReel, yReel, xP2, yP2));
  //printf("Position : %f %f [%f]\n", xP3, yP3, calculerDistance(xReel, yReel, xP3, yP3));
  
  posX = (xP1 + xP2 + xP3)/3;
  posY = (yP1 + yP2 + yP3)/3;
  
  // Calcul du relèvement de la balise 1
  double angleRelevement1_t = calculerAngle(posX, posY, POS_X_BALISE_1, POS_Y_BALISE_1);
  double capRobot_t = angleRelevement1_t - angleGisement1_t;
  
  // Conversion du cap du robot en angle conventionnel et en degrés
  capRobot = convertirAngle(capRobot_t)*180/M_PI;
  
}

void AlgoPC::calculerGisements( double & angleGisement1, double & angleGisement2, double & angleGisement3, double & capRobot, double & posX, double & posY )
{

  // Calcul de la position angulaire des balises en angles trigo
  double angleRelevement1_t = calculerAngle(posX, posY, POS_X_BALISE_1, POS_Y_BALISE_1);
  double angleRelevement2_t = calculerAngle(posX, posY, POS_X_BALISE_2, POS_Y_BALISE_2);
  double angleRelevement3_t = calculerAngle(posX, posY, POS_X_BALISE_3, POS_Y_BALISE_3);
  
  // Conversion du cap du robot en angle trigo
  double capRobot_t = convertirAngle(capRobot)*M_PI/180;
  
  // Calcul des angles de gisement en angle trigo
  double angleGisement1_t = DistanceAngulaire(capRobot_t, angleRelevement1_t);
  double angleGisement2_t = DistanceAngulaire(capRobot_t, angleRelevement2_t);
  double angleGisement3_t = DistanceAngulaire(capRobot_t, angleRelevement3_t);
    
  // Conversion en angles conventionnels et en degrés
  angleGisement1 = (2*M_PI - angleGisement1_t)*180/M_PI;
  angleGisement2 = (2*M_PI - angleGisement2_t)*180/M_PI;
  angleGisement3 = (2*M_PI - angleGisement3_t)*180/M_PI;
  

}
