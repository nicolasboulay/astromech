#include "module.h"
#include "algo.h"
#include "algoPICs.h"
#include "systeme.h"
#include "trace.h"
#include "tinyxml.h"
#include "comSeriePC_PIC.h"
#include "senseur.h"
#include "codeuse.h"
#include "effecteur.h"
#include "moteur.h"
#include "geom2D.h"
#include "bit.h"
#include <math.h>
using namespace std;

# define RAYON_COURBURE_INFINIE 10000.0

AlgoPIC::AlgoPIC(Systeme *s,Trace * t):Algo(s,t)
{
  trace = t;
  src = "algoPICs.cpp";
  cl = "AlgoPIC";
  message = "debut";
  trace->print(src,cl,"AlgoPIC",message);
  numeroWPenCours = 0;
  vitesseConsigneCG_m_par_s = 0;
  
  trameEnvoyee = new TramePIC_PC();
  trameRecue = new TramePC_PIC();

  s32_POS_X_ROBOT_um = 0;
  s32_POS_Y_ROBOT_um = 0;
  u16_CAP_ROBOT_100eme_deg = 0;
  u16_VIT_ROBOT_mm_par_s = 0;
  sensRotationMoteurDroit = 1;
  sensRotationMoteurGauche = 1;
  rayonCercleIdealInitialWPenCours = 1;
  u64_CARRE_DIST_ROBOT_WP = 1E15;
  wayPointConnu = false;
  message = "fin";
  trace->print(src,cl,"AlgoPIC",message);
}

AlgoPIC::~AlgoPIC(void)
{
  message = "debut";
  trace->print(src,cl,"~AlgoPIC",message);
  
  message = "fin";
  trace->print(src,cl,"~AlgoPIC",message);
}

void AlgoPIC::execute(int tempsCourant_ms)
{
  message = "debut";
  
  trace->print(src,cl,"execute",message);
  log <<  tempsCourant_ms<<"\t";
  cout <<"****** Execution PIC � t= "<<tempsCourant_ms<<"*************************************************************"<<endl;

  //actions � mener � chaque trame recue
  if (comSerie->tramePC_PIC_recue())
  {
    cout <<"trame PC_PIC_recue"<<endl;
    comSerie->getTramePC_PIC(trameRecue);
    cout << trameRecue->tramePC_PICToString();
    
    //Mise � jour de la nav si recalage par le PC
    if (isBitSet(trameRecue->u8_NAV_CTRL,BIT_UPDATE))
    {
      s32_POS_X_ROBOT_um = 1000*trameRecue->s16_NEW_POS_X_mm;
      s32_POS_Y_ROBOT_um = 1000*trameRecue->s16_NEW_POS_Y_mm;
      u16_CAP_ROBOT_100eme_deg = 100*trameRecue->u16_NEW_CAP_deg;
    }
    //Determination du waypoint a appliquer
    if (numeroWPenCours==trameRecue->u8_NUM_WP1)
    {
      s32_POS_X_WP_um = 1000*trameRecue->s16_POS_X_WP1_mm;
      s32_POS_Y_WP_um = 1000*trameRecue->s16_POS_Y_WP1_mm;
      u16_CAP_WP_100eme_deg = 100*trameRecue->u16_CAP_WP1_deg;
      u16_VIT_WP_mm_par_s = 10*trameRecue->u8_VIT_WP1_cm_par_s;
      u8_CTRL_WP   = trameRecue->u8_CTRL_WP1;
      wayPointConnu = true;
    }
    else if (numeroWPenCours==trameRecue->u8_NUM_WP2)
    {
      s32_POS_X_WP_um = 1000*trameRecue->s16_POS_X_WP2_mm;
      s32_POS_Y_WP_um = 1000*trameRecue->s16_POS_Y_WP2_mm;
      u16_CAP_WP_100eme_deg = 100*trameRecue->u16_CAP_WP2_deg;
      u16_VIT_WP_mm_par_s = 10*trameRecue->u8_VIT_WP2_cm_par_s;
      u8_CTRL_WP   = trameRecue->u8_CTRL_WP2;
      wayPointConnu = true;
    }
    else
    {
      //probl�me: le waypoint en cours ne correspond pas aux waypoints dans la trame
      cout << "ERREUR: n�WP1:"<<trameRecue->u8_NUM_WP1
           << " n�WP2:"<<trameRecue->u8_NUM_WP2
	   << " n�WPenCours:"<<numeroWPenCours<<endl;
      wayPointConnu = false;
    }
    
    //pr�paration d'une trame pour le PC
    trameEnvoyee->s16_POS_X_mm = s32_POS_X_ROBOT_um/1000;
    trameEnvoyee->s16_POS_Y_mm = s32_POS_Y_ROBOT_um/1000;
    trameEnvoyee->u16_CAP_deg  = u16_CAP_ROBOT_100eme_deg/100;
    trameEnvoyee->u8_VITESSE_cm_par_s = u16_VIT_ROBOT_mm_par_s/10;
    trameEnvoyee->u8_NUM_WP_CRT = numeroWPenCours;
    trameEnvoyee->u8_MUM_NEXT_WP = trameRecue->u8_NUM_WP2;
    comSerie->sendToPC(trameEnvoyee);
    cout << trameEnvoyee->tramePIC_PCToString();
  }
  else
  {
    cout <<"trame PC_PIC non recue"<<endl;
  }
  //lecture des codeuses
  double codeuseMoteurDroit = codeuseDroite->getNbImpulsions();
  double codeuseMoteurGauche = codeuseGauche->getNbImpulsions();
  
  //mise � jour navigation
  miseAjourNavigation(codeuseMoteurDroit,
                      codeuseMoteurGauche,
		      sensRotationMoteurDroit,
                      sensRotationMoteurGauche);
  
  double consigneVitesseMoteurDroit_rad_s = 0;
  double consigneVitesseMoteurGauche_rad_s = 0;
  
  if (wayPointConnu)
  {
    //incr�mentation du waypoint si le courant est atteint
    miseAjourWaypoint();

    //boucle de guidage pour vitesse CG non nulle (depend du WP)
    
    boucleGuidage(consigneVitesseMoteurDroit_rad_s, 
		  consigneVitesseMoteurGauche_rad_s);
  }
  else
  {
    log<<endl;
  }
  		
  if (consigneVitesseMoteurDroit_rad_s < 0)
  {
    sensRotationMoteurDroit = -1;
  }
  else
  {
    sensRotationMoteurDroit = 1;
  }
 
  if (consigneVitesseMoteurGauche_rad_s < 0)
  {
    sensRotationMoteurGauche = -1;
  }
  else
  {
    sensRotationMoteurGauche = 1;
  }
 
  //commande des moteurs
  moteurDroit->setConsigneVitesse(consigneVitesseMoteurDroit_rad_s);
  moteurGauche->setConsigneVitesse(consigneVitesseMoteurGauche_rad_s);
 
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void AlgoPIC::chargerXML(TiXmlElement* pModuleXML)
{
  message = "debut";
  trace->print(src,cl,"chargerXML",message);
  Algo::chargerXML(pModuleXML);

  TiXmlElement* pElt;
  TiXmlHandle handleModule(0);
  handleModule = TiXmlHandle(pModuleXML);
  
  pElt = handleModule.FirstChild("nbPointsParTourMoteur").Element();
  istringstream iss( pElt->GetText() );
  iss >> nbPointsParTourMoteur;
  message = "nbPointsParTourMoteur : " + iss.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("rapportReduction").Element();
  istringstream iss2( pElt->GetText() );
  iss2 >> rapportReduction;
  message = "rapportReduction : " + iss2.str();
  trace->print(src,cl,"chargerXML",message);

  pElt = handleModule.FirstChild("rayonRoues_m").Element();
  istringstream iss3( pElt->GetText() );
  iss3 >> rayonRoues_m;
  message = "rayonRoues_m : " + iss3.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("ecartEntreRoues_m").Element();
  istringstream iss4( pElt->GetText() );
  iss4 >> ecartEntreRoues_m;
  message = "ecartEntreRoues_m : " + iss4.str();
  trace->print(src,cl,"chargerXML",message);

  pElt = handleModule.FirstChild("accelMax_m_par_s2").Element();
  istringstream iss5( pElt->GetText() );
  iss5 >> accelMax_m_par_s2;
  message = "accelMax_m_par_s2 : " + iss5.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("decelMax_m_par_s2").Element();
  istringstream iss6( pElt->GetText() );
  iss6 >> decelMax_m_par_s2;
  message = "decelMax_m_par_s2 : " + iss6.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("gainCorrectionAngulaire").Element();
  istringstream iss7( pElt->GetText() );
  iss7 >> gainCorrectionAngulaire;
  message = "gainCorrectionAngulaire : " + iss7.str();
  trace->print(src,cl,"chargerXML",message);
  
  pElt = handleModule.FirstChild("tempsMiseEnDecel_s").Element();
  istringstream iss8( pElt->GetText() );
  iss8 >> tempsMiseEnDecel_s;
  message = "tempsMiseEnDecel_s : " + iss8.str();
  trace->print(src,cl,"chargerXML",message);

  log <<"t(s)"<<"\t"<<"xR(m)"<<"\t"<<"yR(m)"<<"\t"<<"capR(deg)"<<"\t"<<"vR(m/s)"<<"\t"<<"N� WP"<<"\t"<<"dist WP (m)"<<"\t"
      <<"xCentreCercle(m)"<<"\t"<<"yCentreCercle(m)"<<"\t"<<"directionWP(rad)"<<"\t"
      <<"xRobot/WP(m)"<<"\t"<<"yRobot/WP(m)"<<"\t"<<"Rayon(m)"<<"\t"<<"capRobot(rad)"<<"\t"
      <<"capCercleIdeal(rad)"<<"\t"<<"ecartAngulaire_rad(rad)"<<"\t"
      <<"consV_MDroit(rad/s)"<<"\t"<<"consV_MGauche(rad/s)"<<"\t"
      <<"vConsCGrad/s"<<"\t"<<"distCurvWP(m)"<<"\t"<<"distDecel(m)"<<endl;

  message = "fin";
  trace->print(src,cl,"chargerXML",message);
}

void AlgoPIC::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  
  string nomModule;
  vectorString::iterator it;
  it = tabRelation.begin();
  if (tabRelation.size() == 5)
  {
    nomModule = *it;
    comSerie = (ComSeriePC_PIC *)systPere->getPtrModule(nomModule);
    message = "connection � :" + nomModule;
    trace->print(src,cl,"connectModules",message);
    
    it++;
    
    nomModule = *it;
    codeuseDroite = (Codeuse *)systPere->getPtrModule(nomModule);
    message = "connection � :" + nomModule;
    trace->print(src,cl,"connectModules",message);
   
    it++;
	 
    nomModule = *it;
    codeuseGauche = (Codeuse *)systPere->getPtrModule(nomModule);
    message = "connection � :" + nomModule;
    trace->print(src,cl,"connectModules",message);
    
    it++;
	 
    nomModule = *it;
    moteurDroit = (Moteur *)systPere->getPtrModule(nomModule);
    message = "connection � :" + nomModule;
    trace->print(src,cl,"connectModules",message);
    
    it++;
	 
    nomModule = *it;
    moteurGauche = (Moteur *)systPere->getPtrModule(nomModule);
    message = "connection � :" + nomModule;
    trace->print(src,cl,"connectModules",message);
  }
  
  message = "fin";
  trace->print(src,cl,"connectModules",message);
}

void AlgoPIC::miseAjourNavigation(const double & impCodeuseMoteurDroit,
                                  const double & impCodeuseMoteurGauche,
				  const int    & sensRotationMoteurDroit,
				  const int    & sensRotationMoteurGauche)
{
  //(2Pi*R)/(500*36) * NbImp = distance
  double facteurConversion = 2*M_PI*rayonRoues_m*1000000/(nbPointsParTourMoteur*rapportReduction);//17.5;
  double deplacementRoueDroite_um = sensRotationMoteurDroit*facteurConversion*impCodeuseMoteurDroit;
  double deplacementRoueGauche_um = sensRotationMoteurGauche*facteurConversion*impCodeuseMoteurGauche;
  cout<<"deplRD "<<deplacementRoueDroite_um<<" deplRG "<<deplacementRoueGauche_um<<endl;
  
  double deltaAvance_um = (deplacementRoueDroite_um + deplacementRoueGauche_um)/2;
  double d_cap_robot_100eme_deg = u16_CAP_ROBOT_100eme_deg;
  cout <<"u16_CAP_ROBOT_100eme_deg "<<u16_CAP_ROBOT_100eme_deg<<endl;
  double cap_robot_rad = (d_cap_robot_100eme_deg/100)*3.14/180;
  cout <<"capRobot_rad "<<cap_robot_rad<<endl;
  
  double new_pos_x_um = s32_POS_X_ROBOT_um + deltaAvance_um * sin(cap_robot_rad);
  double new_pos_y_um = s32_POS_Y_ROBOT_um + deltaAvance_um * cos(cap_robot_rad);
  double new_cap_100eme_deg = u16_CAP_ROBOT_100eme_deg + (100*180/3.14)*atan((deplacementRoueGauche_um - deplacementRoueDroite_um)/(1000000*ecartEntreRoues_m));
  cout <<"new_cap_100eme_deg "<<new_cap_100eme_deg<<endl;
  normalise0_360_100eme_deg(new_cap_100eme_deg);
  cout <<"new_cap_100eme_deg "<<new_cap_100eme_deg<<endl;
  //mise � jour position
  s32_POS_X_ROBOT_um = new_pos_x_um;
  s32_POS_Y_ROBOT_um = new_pos_y_um;
  u16_CAP_ROBOT_100eme_deg = new_cap_100eme_deg;
  u16_VIT_ROBOT_mm_par_s = deltaAvance_um/pasTemps_ms; 
  
  double pos_x_out_m = s32_POS_X_ROBOT_um/1E6;
  double pos_y_out_m = s32_POS_Y_ROBOT_um/1E6;
  double cap_out_deg = u16_CAP_ROBOT_100eme_deg/1E2;
  double vit_out_m_par_s = u16_VIT_ROBOT_mm_par_s/1E3;
  
  log << pos_x_out_m <<"\t"<< pos_y_out_m <<"\t"<<cap_out_deg <<"\t"<<vit_out_m_par_s<<"\t";
}

void AlgoPIC::boucleGuidage(double & consigneVitesseMoteurDroit_rad_s, 
		            double & consigneVitesseMoteurGauche_rad_s)
{
  //on remet tout en metrique et degres temporairement
  cout <<"u16_VIT_WP_mm_par_s "<<u16_VIT_WP_mm_par_s<<endl;
  WayPoint * wpEnCours;
  double d_posx, d_posy, d_cap, d_vit;
  d_posx = s32_POS_X_WP_um;
  d_posy = s32_POS_Y_WP_um;
  d_cap  = u16_CAP_WP_100eme_deg;
  d_vit  = u16_VIT_WP_mm_par_s;
  wpEnCours = new WayPoint(d_posx/1000000, d_posy/1000000, d_cap/100, d_vit/1000);
   
  WayPoint * positionAttVit;
  d_posx = s32_POS_X_ROBOT_um;
  d_posy = s32_POS_Y_ROBOT_um;
  d_cap  = u16_CAP_ROBOT_100eme_deg;
  d_vit  = u16_VIT_ROBOT_mm_par_s;
  positionAttVit = new WayPoint(d_posx/1000000, d_posy/1000000, d_cap/100, d_vit/1000);

  Point2D * centreDuCercle = new Point2D(0,0);
  //calcul de la position courante dans le repere du WP en cours
  Point2D * ptCourant = new Point2D(positionAttVit->pt);
  ptCourant->calculePointDansNouveauRepere(wpEnCours->pt,wpEnCours->cap_deg*3.14/180);
    
    //calcul du centre du cercle ideal pour joindre le waypoint dans le repere du waypoint
    double xRobot,yRobot;
    xRobot = ptCourant->x;
    yRobot = ptCourant->y;
    delete ptCourant;
    double capCercleIdeal;
    double xCentreCercle;
    double rayonCercle;
    if (xRobot != 0) //cas ou le robot n'est pas pile dans l'axe du waypoint
    {
      xCentreCercle = (xRobot*xRobot+yRobot*yRobot)/(2*xRobot);
      if (xCentreCercle>=0)
      {
        rayonCercle = xCentreCercle;
      }
      else
      {
        rayonCercle = -xCentreCercle;
      }
      //Point2D * centreDuCercle = new Point2D(xCentreCercle,0);
      centreDuCercle->x = xCentreCercle;
      centreDuCercle->y = 0;
    	
      //calcul du centre du cercle dans le repere terrain
      centreDuCercle->calculePointDansRepereTerrain(wpEnCours->pt,wpEnCours->cap_deg*3.14/180);
      log << centreDuCercle->x <<"\t"<<centreDuCercle->y<<"\t";
      log <<  wpEnCours->cap_deg<<"\t"<<xRobot <<"\t"<< yRobot <<"\t"<< xCentreCercle<<"\t";

      //calcul du cap pt Robot / centre Cercle
      double capRobotCentre = positionAttVit->pt->calculeCap_rad(centreDuCercle);
      
      if (xRobot > 0)
      {
        capCercleIdeal = capRobotCentre + 3*M_PI/2;
      }
      else
      {
        capCercleIdeal = capRobotCentre + M_PI/2;
      }
    }
    else
    {
      //ligne droite pour rejoindre le wp
      capCercleIdeal = wpEnCours->cap_deg*3.14/180;
    }
    normalise0_2PI(capCercleIdeal);
    double ecartAngulaire_rad = capCercleIdeal - positionAttVit->cap_deg*3.14/180;
    normaliseMPI_PPI(ecartAngulaire_rad);
    
    //calcul de la vitesse du centre de gravit� du robot
    double distanceCurviligneJusqueWP = 0;
    double distanceDeceleration = 0;
    cout<<"vitesseConsigneCG_m_par_s "<<vitesseConsigneCG_m_par_s<<endl;
    cout<<"wpEnCours->vitesse_m_par_s "<<wpEnCours->vitesse_m_par_s<<endl;
    if (vitesseConsigneCG_m_par_s <= wpEnCours->vitesse_m_par_s)
    {
      //il faut acc�l�rer
      cout << "vc"<<positionAttVit->vitesse_m_par_s<<endl;
      cout << "vobj"<<wpEnCours->vitesse_m_par_s<<endl;
      
      double nouvelleVitesse_m_par_s = vitesseConsigneCG_m_par_s + (pasTemps_ms*accelMax_m_par_s2)/1000;
      if (nouvelleVitesse_m_par_s < wpEnCours->vitesse_m_par_s)
      {
        vitesseConsigneCG_m_par_s = nouvelleVitesse_m_par_s;
	cout << "vcg"<<vitesseConsigneCG_m_par_s<<endl;
      }
    }
    else
    {
      //il faut d�celerer le plus tard possible
      //calcul de la distance curviligne au WP courant
      double capCentreCercle_Robot = centreDuCercle->calculeCap_rad(positionAttVit->pt);
      double capCentreCercle_WP = centreDuCercle->calculeCap_rad(wpEnCours->pt);
      normaliseMPI_PPI(capCentreCercle_Robot);
      normaliseMPI_PPI(capCentreCercle_WP);
      double ecartAngulairePourRejoindreWP = capCentreCercle_WP - capCentreCercle_Robot;
      normalise0_2PI(ecartAngulairePourRejoindreWP);
      distanceCurviligneJusqueWP = ecartAngulairePourRejoindreWP * xCentreCercle;
      cout <<"capCentreCercle_Robot"<< capCentreCercle_Robot <<endl;
       cout <<"capCentreCercle_WP"<< capCentreCercle_WP <<endl;
       cout <<"centreDuCercleX"<<centreDuCercle->x<<endl;
       cout <<"centreDuCercleY"<<centreDuCercle->y<<endl;
       cout <<"wpX"<<wpEnCours->pt->x<<endl;
       cout <<"wpY"<<wpEnCours->pt->y<<endl;
      cout <<"ecartAngulairePourRejoindreWP"<< ecartAngulairePourRejoindreWP <<endl;
      
      //calcul de la distance de d�c�l�ration
      double deltaVitesseCourant = wpEnCours->vitesse_m_par_s - vitesseConsigneCG_m_par_s;
      distanceDeceleration = (deltaVitesseCourant*deltaVitesseCourant)/(2*decelMax_m_par_s2)
                             +vitesseConsigneCG_m_par_s*tempsMiseEnDecel_s;//terme correctif pour tenir compte de la diff�rence entre
			     //vitesse de consigne et vitesse r�alis�e
      
      //faut il d�celerer?
      if (distanceCurviligneJusqueWP <= distanceDeceleration)
      {
        vitesseConsigneCG_m_par_s = vitesseConsigneCG_m_par_s - (pasTemps_ms*decelMax_m_par_s2)/1000;
      }
    }
    
    double deltaV; 
    double vitesseConsigneCG_rad_par_s = vitesseConsigneCG_m_par_s*rapportReduction/rayonRoues_m;
    double valAbsEcartAngulaire_rad = ecartAngulaire_rad;
    
    if (ecartAngulaire_rad<0)
    {
      valAbsEcartAngulaire_rad = - ecartAngulaire_rad;
    }
      
    if ( rayonCercleIdealInitialWPenCours >= (ecartEntreRoues_m/2))
    {
      cout <<"sens rotation roues identique **** rayon cercle ideal "<<rayonCercleIdealInitialWPenCours<<endl;
      deltaV = vitesseConsigneCG_rad_par_s*(gainCorrectionAngulaire*valAbsEcartAngulaire_rad -1)/(gainCorrectionAngulaire*valAbsEcartAngulaire_rad + 1);
    }
    else
    {
      cout <<"sens rotation roues oppos� **** rayon cercle ideal "<<rayonCercleIdealInitialWPenCours<<endl;
      deltaV = vitesseConsigneCG_rad_par_s*(3*gainCorrectionAngulaire*valAbsEcartAngulaire_rad +1)/(gainCorrectionAngulaire*valAbsEcartAngulaire_rad + 1);     
    }
    cout <<"deltaV "<<deltaV<<endl;
    cout <<"vitesseConsigneCG_rad_par_s "<<vitesseConsigneCG_rad_par_s<<endl;
    if (ecartAngulaire_rad>0)
    {
      consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
      consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
    }
    else
    {
      consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
      consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
    }
    log <<  positionAttVit->cap_deg<<"\t"<<capCercleIdeal <<"\t"<< ecartAngulaire_rad <<"\t"
        << consigneVitesseMoteurDroit_rad_s<<"\t"<< consigneVitesseMoteurGauche_rad_s<<"\t"<<vitesseConsigneCG_rad_par_s<<"\t"
	<< distanceCurviligneJusqueWP <<"\t"<< distanceDeceleration <<"\t"<<sommeEcartsAngulaire<<endl;
    delete centreDuCercle;  
    delete wpEnCours;
    delete positionAttVit;
}

float AlgoPIC::carreDistanceEuclidienne(float x1,
                                               float y1,
				               float x2,
				               float y2)
{
  return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}
void AlgoPIC::miseAjourWaypoint(void)
{
  //attention au cas des waypoint "changement de cap" � traiter plus tard
  float u64_carreDistanceCouranteRobot_WP;
  u64_carreDistanceCouranteRobot_WP = carreDistanceEuclidienne(s32_POS_X_WP_um,
                                                               s32_POS_Y_WP_um,
						               s32_POS_X_ROBOT_um,
                                                               s32_POS_Y_ROBOT_um);
  cout <<u64_carreDistanceCouranteRobot_WP<<endl;
  float carre5cm_um=50000.0*50000.0;
  cout <<carre5cm_um<<endl;
  if (u64_carreDistanceCouranteRobot_WP < carre5cm_um) //si distWP<5cm
  {
    cout <<"distWP<5cm "<<u64_carreDistanceCouranteRobot_WP<<endl;
    if (u64_carreDistanceCouranteRobot_WP > u64_CARRE_DIST_ROBOT_WP)
    {
      //la dist au WP commence � augmenter: on transitionne
      numeroWPenCours++;
      u64_CARRE_DIST_ROBOT_WP = 1E15;
      if (numeroWPenCours==trameRecue->u8_NUM_WP2)
      {
        s32_POS_X_WP_um = trameRecue->s16_POS_X_WP2_mm*1000;
        s32_POS_Y_WP_um = trameRecue->s16_POS_Y_WP2_mm*1000;
        u16_CAP_WP_100eme_deg = trameRecue->u16_CAP_WP2_deg*100;
        u16_VIT_WP_mm_par_s = trameRecue->u8_VIT_WP2_cm_par_s*10;
        u8_CTRL_WP   = trameRecue->u8_CTRL_WP2;
	
	WayPoint * wpEnCours;
        double d_posx, d_posy, d_cap, d_vit;
        d_posx = s32_POS_X_WP_um;
        d_posy = s32_POS_Y_WP_um;
        d_cap  = u16_CAP_WP_100eme_deg;
        d_vit  = u16_VIT_WP_mm_par_s;
        wpEnCours = new WayPoint(d_posx/1000000, d_posy/1000000, d_cap/100, d_vit/1000);
	double xCentreCercle;
	cout<<"avant"<<endl;
	calculXcentreCercleDansRepereWaypoint(wpEnCours,xCentreCercle);
	cout<<"apres"<<endl;	
	if (xCentreCercle>=0)
	{
	  rayonCercleIdealInitialWPenCours = xCentreCercle;
	}
	else
	{
	  rayonCercleIdealInitialWPenCours = -xCentreCercle;
	}
	delete wpEnCours;
      }
      else
      {
        //plus de waypoint : ARRET
      }
    }
    else
    {
      u64_CARRE_DIST_ROBOT_WP = u64_carreDistanceCouranteRobot_WP;
      cout <<"u64_CARRE_DIST_ROBOT_WP "<<u64_CARRE_DIST_ROBOT_WP<<endl;
    }
  }
    
  double distanceAuWPCourant = sqrt(u64_carreDistanceCouranteRobot_WP)/1E6;     
  int numeroWP_out = numeroWPenCours;
  log << numeroWP_out <<"\t"<< distanceAuWPCourant <<"\t";
}


void AlgoPIC::calculXcentreCercleDansRepereWaypoint(const WayPoint * wpEnCours,
                                                    double & xCentreCercle)
{
  //calcul de la position courante dans le repere du WP en cours
  double d_posx,d_posy;
  d_posx = s32_POS_X_ROBOT_um;
  d_posy = s32_POS_Y_ROBOT_um;
  cout<<"avant pt"<<endl;
  Point2D * ptCourant = new Point2D(d_posx/1000000,d_posy/1000000);
  ptCourant->calculePointDansNouveauRepere(wpEnCours->pt,wpEnCours->cap_deg*3.14/180);
  cout<<"apres pt"<<endl;  
  //calcul du centre du cercle ideal pour joindre le waypoint dans le repere du waypoint
  double xRobot,yRobot;
  xRobot = ptCourant->x;
  yRobot = ptCourant->y;

  if (xRobot != 0) //cas ou le robot n'est pas pile dans l'axe du waypoint
  {
    xCentreCercle = (xRobot*xRobot+yRobot*yRobot)/(2*xRobot); 
  }
  else
  {
    xCentreCercle = RAYON_COURBURE_INFINIE;
  }
  delete ptCourant;
}

