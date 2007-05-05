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
  vitesseConsigneRoue_mm_par_s = 0;
  
  trameEnvoyee = new TramePIC_PC();
  trameRecue = new TramePC_PIC();

  s32_POS_X_ROBOT_um = 0;
  s32_POS_Y_ROBOT_um = 0;
  u16_CAP_ROBOT_100eme_deg = 0;
  s16_XCAP_WP_mm = 0;
  s16_YCAP_WP_mm = 0;
  u16_VIT_ROBOT_mm_par_s = 0;
  s32_X_CAP_ROBOT_um = 0;
  s32_Y_CAP_ROBOT_um = 0;
  posX = 0;
  posY = 0;
  cap = 0;
  vitesse = 0;
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
  cout <<"****** Execution PIC à t= "<<tempsCourant_ms<<"*************************************************************"<<endl;

  //actions à mener à chaque trame recue
  if (comSerie->tramePC_PIC_recue())
  {
    cout <<"trame PC_PIC_recue"<<endl;
    comSerie->getTramePC_PIC(trameRecue);
    cout << trameRecue->tramePC_PICToString();
    
    //Mise à jour de la nav si recalage par le PC
    if (isBitSet(trameRecue->u8_NAV_CTRL,BIT_UPDATE))
    {
      s32_POS_X_ROBOT_um = 1000*trameRecue->s16_NEW_POS_X_mm;
      s32_POS_Y_ROBOT_um = 1000*trameRecue->s16_NEW_POS_Y_mm;
      u16_CAP_ROBOT_100eme_deg = 100*trameRecue->u16_NEW_CAP_deg;
      
      posX = trameRecue->s16_NEW_POS_X_mm/1000.0;
      posY = trameRecue->s16_NEW_POS_Y_mm/1000.0;
      cap = trameRecue->u16_NEW_CAP_deg;
      qw = cos((cap*3.14/180)/2);
      qx = 0.0;
      qy = 0.0;
      qz = -sin((cap*3.14/180)/2);
      
    }
    //Determination du waypoint a appliquer
    if (numeroWPenCours==trameRecue->u8_NUM_WP1)
    {
      s32_POS_X_WP_um = 1000*trameRecue->s16_POS_X_WP1_mm;
      s32_POS_Y_WP_um = 1000*trameRecue->s16_POS_Y_WP1_mm;
      u16_CAP_WP_100eme_deg = 100*trameRecue->u16_CAP_WP1_deg;
      s16_XCAP_WP_mm = trameRecue->s16_XCAP_WP1_mm;
      s16_YCAP_WP_mm = trameRecue->s16_YCAP_WP1_mm;
      u16_VIT_WP_mm_par_s = 10*trameRecue->u8_VIT_WP1_cm_par_s;
      u8_CTRL_WP   = trameRecue->u8_CTRL_WP1;
      wayPointConnu = true;
    }
    else if (numeroWPenCours==trameRecue->u8_NUM_WP2)
    {
      s32_POS_X_WP_um = 1000*trameRecue->s16_POS_X_WP2_mm;
      s32_POS_Y_WP_um = 1000*trameRecue->s16_POS_Y_WP2_mm;
      u16_CAP_WP_100eme_deg = 100*trameRecue->u16_CAP_WP2_deg;
      s16_XCAP_WP_mm = trameRecue->s16_XCAP_WP2_mm;
      s16_YCAP_WP_mm = trameRecue->s16_YCAP_WP2_mm;
      u16_VIT_WP_mm_par_s = 10*trameRecue->u8_VIT_WP2_cm_par_s;
      u8_CTRL_WP   = trameRecue->u8_CTRL_WP2;
      wayPointConnu = true;
    }
    else
    {
      //problème: le waypoint en cours ne correspond pas aux waypoints dans la trame
      cout << "ERREUR: n°WP1:"<<trameRecue->u8_NUM_WP1
           << " n°WP2:"<<trameRecue->u8_NUM_WP2
	   << " n°WPenCours:"<<numeroWPenCours<<endl;
      wayPointConnu = false;
    }
    
    //préparation d'une trame pour le PC
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
  
  //mise à jour navigation
  miseAjourNavigation(codeuseMoteurDroit,
                      codeuseMoteurGauche,
		      sensRotationMoteurDroit,
                      sensRotationMoteurGauche);
  
  miseAjourNavigationQuats(codeuseMoteurDroit,
                      codeuseMoteurGauche,
		      sensRotationMoteurDroit,
                      sensRotationMoteurGauche);
  
  
  double consigneVitesseMoteurDroit_rad_s = 0;
  double consigneVitesseMoteurGauche_rad_s = 0;
  
  if (wayPointConnu)
  {
    if (isBitSet(u8_CTRL_WP,BIT_WP_NUL))
    {
      log<<endl;
      consigneVitesseMoteurDroit_rad_s = 0;
      consigneVitesseMoteurGauche_rad_s = 0;
    }
    else
    {
      //incrémentation du waypoint si le courant est atteint
      miseAjourWaypoint();
      
      if (isBitSet(u8_CTRL_WP,BIT_ROT))
      {
        //rotation sur place
        boucleGuidageRotation(consigneVitesseMoteurDroit_rad_s, 
		            consigneVitesseMoteurGauche_rad_s);
      }
      else
      {
        //boucle de guidage pour vitesse CG non nulle (depend du WP)
        /*boucleGuidage(consigneVitesseMoteurDroit_rad_s, 
		      consigneVitesseMoteurGauche_rad_s);*/
		      
	boucleGuidageSansTrigo(consigneVitesseMoteurDroit_rad_s, 
		               consigneVitesseMoteurGauche_rad_s);
      }
    }
  }
  else
  {
    log<<endl;
    consigneVitesseMoteurDroit_rad_s = 0;
    consigneVitesseMoteurGauche_rad_s = 0;
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

  log <<"t(s)"<<"\t"<<"xR(m)"<<"\t"<<"yR(m)"<<"\t"<<"capR(deg)"<<"\t"<<"vR(m/s)"<<"\t"
      <<"xQ(m)"<<"\t"<<"yQ(m)"<<"\t"<<"capQ(deg)"<<"\t"
      <<"N° WP"<<"\t"<<"dist WP (m)"<<"\t"
      <<"xCentreCercle(m)"<<"\t"<<"yCentreCercle(m)"<<"\t"
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
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
    
    it++;
    
    nomModule = *it;
    codeuseDroite = (Codeuse *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
   
    it++;
	 
    nomModule = *it;
    codeuseGauche = (Codeuse *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
    
    it++;
	 
    nomModule = *it;
    moteurDroit = (Moteur *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
    trace->print(src,cl,"connectModules",message);
    
    it++;
	 
    nomModule = *it;
    moteurGauche = (Moteur *)systPere->getPtrModule(nomModule);
    message = "connection à :" + nomModule;
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
  //mise à jour position
  s32_POS_X_ROBOT_um = new_pos_x_um;
  s32_POS_Y_ROBOT_um = new_pos_y_um;
  u16_CAP_ROBOT_100eme_deg = new_cap_100eme_deg;
  deltaAvance_um = valAbs(deltaAvance_um);
  u16_VIT_ROBOT_mm_par_s = deltaAvance_um/pasTemps_ms; 
  cout<<"deltaAvance "<<deltaAvance_um<<endl;
  cout<<"pasTemps_ms "<<pasTemps_ms<<endl;
  cout<<"u16_VIT_ROBOT_mm_par_s "<<u16_VIT_ROBOT_mm_par_s<<endl;
  
  double pos_x_out_m = s32_POS_X_ROBOT_um/1E6;
  double pos_y_out_m = s32_POS_Y_ROBOT_um/1E6;
  double cap_out_deg = u16_CAP_ROBOT_100eme_deg/1E2;
  double vit_out_m_par_s = u16_VIT_ROBOT_mm_par_s/1E3;
  
  log << pos_x_out_m <<"\t"<< pos_y_out_m <<"\t"<<cap_out_deg <<"\t"<<vit_out_m_par_s<<"\t";
}

void AlgoPIC::miseAjourNavigationQuats(const double & impCodeuseMoteurDroit,
                             const double & impCodeuseMoteurGauche,
			     const int    & sensRotationMoteurDroit,
		             const int    & sensRotationMoteurGauche)
{
  //vitesse de rotation
  double vitesseRotDroite_rad_par_s = sensRotationMoteurDroit*impCodeuseMoteurDroit*2*M_PI/(nbPointsParTourMoteur*rapportReduction*(pasTemps_ms/1000.0));
  double vitesseRotGauche_rad_par_s = sensRotationMoteurGauche*impCodeuseMoteurGauche*2*M_PI/(nbPointsParTourMoteur*rapportReduction*(pasTemps_ms/1000.0));
  double vitesseRotationRobot_rad_par_s = (rayonRoues_m/ecartEntreRoues_m)*(vitesseRotGauche_rad_par_s-vitesseRotDroite_rad_par_s);
  
  //mise à jour du cap
  double new_cap_rad = cap*M_PI/180 + vitesseRotationRobot_rad_par_s*(pasTemps_ms/1000.0);
  
  //calcul quat "delta t"
  double qw_d,qz_d;
  if (vitesseRotationRobot_rad_par_s!=0)
  {
    qw_d = 1 - pow(vitesseRotationRobot_rad_par_s*(pasTemps_ms/1000.0),2)/8;
    qz_d = vitesseRotationRobot_rad_par_s*(pasTemps_ms/1000.0)/2;
  }
  else
  {
    qw_d = 1;
    qz_d = 0;
  }
  
  //transformation du repere robot vers repere absolu
  double qw_new,qx_new,qy_new,qz_new;
  qw_new = qw*qw_d - qz*qz_d;
  qx_new = qx*qw_d + qy*qz_d;
  qy_new = qy*qw_d - qx*qz_d;
  qz_new = qz*qw_d + qw*qz_d;
  
  qw = qw_new;
  qx = qx_new;
  qy = qy_new;
  qz = qz_new;
  
  //calcul de la nouvelle vitesse tangentielle dans les rep robot
  double vitTgRobot_m_par_s = (vitesseRotDroite_rad_par_s + vitesseRotGauche_rad_par_s)/2*rayonRoues_m;
  //calcul de la nouvelle acceleration normale dans les rep robot
  double accNRobot = rayonRoues_m*rayonRoues_m
                    *(vitesseRotGauche_rad_par_s*vitesseRotGauche_rad_par_s - vitesseRotDroite_rad_par_s*vitesseRotDroite_rad_par_s)
		    /(2*ecartEntreRoues_m);
  //calcul de la nouvelle acceleration tangentielle dans le rep robot
  double accTgRobot = (vitTgRobot_m_par_s - vitesse)/(pasTemps_ms/1000.0);
  
  //calcul de l'acceleration et vitesse dans le repere terrain
  double accX = (qw*qw + qx*qx - qy*qy - qz*qz)*accNRobot + 2*(qx*qy + qw*qz)*accTgRobot; 
  double accY = 2*(qx*qy -qw*qz)*accNRobot + (qw*qw - qx*qx + qy*qy - qz*qz)*accTgRobot;
  double vitX =                            2*(qx*qy + qw*qz)*vitTgRobot_m_par_s;
  double vitY =                           (qw*qw - qx*qx + qy*qy - qz*qz)*vitTgRobot_m_par_s;
  
  double xDep_m = vitX*(pasTemps_ms/1000.0) + 0.5*accX*(pasTemps_ms/1000.0)*(pasTemps_ms/1000.0);
  double yDep_m = vitY*(pasTemps_ms/1000.0) + 0.5*accY*(pasTemps_ms/1000.0)*(pasTemps_ms/1000.0);
  
  //calcul de la nouvelle position
  double newPosX = posX + xDep_m;
  double newPosY = posY + yDep_m;
  
  posX = newPosX;
  posY = newPosY;
  cap  = new_cap_rad*180/M_PI;
  vitesse = vitTgRobot_m_par_s;
  
  //mise à jour position
  s32_POS_X_ROBOT_um = posX*1000000;
  s32_POS_Y_ROBOT_um = posY*1000000;
  double cap100emedeg = cap*100;
  normalise0_360_100eme_deg(cap100emedeg);
  //s32_X_CAP_ROBOT_um = xDep_m*1000000.0;
  //s32_Y_CAP_ROBOT_um = yDep_m*1000000.0;
  s32_X_CAP_ROBOT_um = 2*(qx*qy + qw*qz)*1000000;
  s32_Y_CAP_ROBOT_um = (qw*qw + qx*qx - qy*qy - qz*qz)*1000000;
  
  u16_CAP_ROBOT_100eme_deg = cap100emedeg;
  u16_VIT_ROBOT_mm_par_s = vitTgRobot_m_par_s*1000;
  
  log << posX <<"\t"<< posY <<"\t"<<cap <<"\t";
}

void AlgoPIC::boucleGuidageRotation(double & consigneVitesseMoteurDroit_rad_s, 
		                    double & consigneVitesseMoteurGauche_rad_s)
{
  //Détermination des signes des vitesses droites et gauches
  //1: sens horaire 0:anti horaire
  int sensRotMoteurGauche = 0;
  int sensRotMoteurDroit  = 0;
  int vitesseConsigneAccelRoue_mm_par_s = 0;
  int vitesseConsignePlateauRoue_mm_par_s = 0;
  int vitesseConsigneDecelerationRoue_mm_par_s = 0;
  int ecartAngulaireCourant_100eme_deg = 0;
  vitesseConsigneCG_m_par_s = 0;
  
  if (isBitSet(u8_CTRL_WP,BIT_SENS_ROT))
  {
    sensRotMoteurGauche = 1;
    sensRotMoteurDroit  = -1;
  }
  else
  {
    sensRotMoteurGauche = -1;
    sensRotMoteurDroit  = 1;
  }
  //accélération
  vitesseConsigneAccelRoue_mm_par_s = vitesseConsigneRoue_mm_par_s+(pasTemps_ms*accelMax_m_par_s2);
  //plateau
  vitesseConsignePlateauRoue_mm_par_s = u16_VIT_WP_mm_par_s;
  //déceleration
  ecartAngulaireCourant_100eme_deg = u16_CAP_WP_100eme_deg - u16_CAP_ROBOT_100eme_deg;
  double d = ecartAngulaireCourant_100eme_deg;
  normalise0_360_100eme_deg(d);
  ecartAngulaireCourant_100eme_deg = valAbs(d);
  if (!isBitSet(u8_CTRL_WP,BIT_SENS_ROT))
  {
    ecartAngulaireCourant_100eme_deg = 36000-ecartAngulaireCourant_100eme_deg;
  }

  vitesseConsigneDecelerationRoue_mm_par_s = ecartAngulaireCourant_100eme_deg/10.0;
  
  vitesseConsigneRoue_mm_par_s = min(vitesseConsigneAccelRoue_mm_par_s, vitesseConsignePlateauRoue_mm_par_s);
  vitesseConsigneRoue_mm_par_s = min(vitesseConsigneRoue_mm_par_s, vitesseConsigneDecelerationRoue_mm_par_s);
  
  double vitesseConsigneRoue_rad_par_s = ((double)vitesseConsigneRoue_mm_par_s/1000.0)*rapportReduction/rayonRoues_m;
  
  consigneVitesseMoteurDroit_rad_s =  sensRotMoteurDroit*vitesseConsigneRoue_rad_par_s;
  consigneVitesseMoteurGauche_rad_s = sensRotMoteurGauche*vitesseConsigneRoue_rad_par_s;
  
  log <<"\t\t\t" 
  <<ecartAngulaireCourant_100eme_deg<<"\t"
  << consigneVitesseMoteurDroit_rad_s<<"\t"
  << consigneVitesseMoteurGauche_rad_s<<"\t"
  << vitesseConsigneRoue_rad_par_s<<"\t"
  << vitesseConsigneAccelRoue_mm_par_s<<"\t"
  << vitesseConsignePlateauRoue_mm_par_s<<"\t"
  << vitesseConsigneDecelerationRoue_mm_par_s<<"\t"<<endl;
}
void AlgoPIC::boucleGuidage(double & consigneVitesseMoteurDroit_rad_s, 
		                    double & consigneVitesseMoteurGauche_rad_s)
{
  //on remet tout en metrique et degres temporairement
  vitesseConsigneRoue_mm_par_s = 0;
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
  if (isBitSet(u8_CTRL_WP,BIT_SENS_WP)==false)
  {
    //marche arriere toute
    d_cap +=18000.0;
    normalise0_360_100eme_deg(d_cap);
  }
  positionAttVit = new WayPoint(d_posx/1000000, d_posy/1000000, d_cap/100, d_vit/1000);

  Point2D * centreDuCercle = new Point2D(0,0);
  //calcul de la position courante dans le repere du WP en cours
  Point2D * ptCourant = new Point2D(positionAttVit->pt);
  ptCourant->calculePointDansNouveauRepere(wpEnCours->pt,wpEnCours->cap_deg*M_PI/180);
    
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
      rayonCercleIdealInitialWPenCours = rayonCercle;
      
      //Point2D * centreDuCercle = new Point2D(xCentreCercle,0);
      centreDuCercle->x = xCentreCercle;
      centreDuCercle->y = 0;
    	
      //calcul du centre du cercle dans le repere terrain
      centreDuCercle->calculePointDansRepereTerrain(wpEnCours->pt,wpEnCours->cap_deg*M_PI/180);
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
      capCercleIdeal = wpEnCours->cap_deg*M_PI/180;
    }
    normalise0_2PI(capCercleIdeal);
    double ecartAngulaire_rad = capCercleIdeal - positionAttVit->cap_deg*M_PI/180;
    normaliseMPI_PPI(ecartAngulaire_rad);
    
    //calcul de la vitesse du centre de gravité du robot
    double distanceCurviligneJusqueWP = 0;
    double distanceDeceleration = 0;
    cout<<"vitesseConsigneCG_m_par_s "<<vitesseConsigneCG_m_par_s<<endl;
    cout<<"wpEnCours->vitesse_m_par_s "<<wpEnCours->vitesse_m_par_s<<endl;
    
    //calcul de la distance de décélération
    double deltaVitesseCourant = wpEnCours->vitesse_m_par_s - vitesseConsigneCG_m_par_s;
    distanceDeceleration = (deltaVitesseCourant*deltaVitesseCourant)/(2*decelMax_m_par_s2);
    
    //calcul de la distance curviligne jusqu'au prochain waypoint
    double capCentreCercle_Robot = centreDuCercle->calculeCap_rad(positionAttVit->pt);
    double capCentreCercle_WP = centreDuCercle->calculeCap_rad(wpEnCours->pt);
    normaliseMPI_PPI(capCentreCercle_Robot);
    normaliseMPI_PPI(capCentreCercle_WP);
    double ecartAngulairePourRejoindreWP;
    if (xCentreCercle >= 0)
    {
      ecartAngulairePourRejoindreWP = capCentreCercle_WP - capCentreCercle_Robot;
    }
    else
    {
        ecartAngulairePourRejoindreWP = capCentreCercle_Robot - capCentreCercle_WP;
    }
      
    normalise0_2PI(ecartAngulairePourRejoindreWP);
    distanceCurviligneJusqueWP = ecartAngulairePourRejoindreWP * xCentreCercle;
    distanceCurviligneJusqueWP = valAbs(distanceCurviligneJusqueWP);
    

    if (distanceCurviligneJusqueWP <= distanceDeceleration)
    {
      vitesseConsigneCG_m_par_s = vitesseConsigneCG_m_par_s - (pasTemps_ms*decelMax_m_par_s2)/1000;
    }
    else
    {
      if (vitesseConsigneCG_m_par_s < wpEnCours->vitesse_m_par_s)
      {
        //il faut accélérer sans dépasser Vmax
        cout << "vc"<<positionAttVit->vitesse_m_par_s<<endl;
        cout << "vobj"<<wpEnCours->vitesse_m_par_s<<endl;
      
        double nouvelleVitesse_m_par_s = vitesseConsigneCG_m_par_s + (pasTemps_ms*accelMax_m_par_s2)/1000;
	vitesseConsigneCG_m_par_s = min(nouvelleVitesse_m_par_s, wpEnCours->vitesse_m_par_s);
      }
    }
       
    double deltaV; 
    double vitesseConsigneCG_rad_par_s = vitesseConsigneCG_m_par_s*rapportReduction/rayonRoues_m;
    double valAbsEcartAngulaire_rad = ecartAngulaire_rad;
    
    if (ecartAngulaire_rad<0)
    {
      valAbsEcartAngulaire_rad = - ecartAngulaire_rad;
    }
      
    if ( rayonCercleIdealInitialWPenCours >= (ecartEntreRoues_m/2.0))
    {
      cout <<"sens rotation roues identique **** rayon cercle ideal "<<rayonCercleIdealInitialWPenCours<<endl;
      deltaV = vitesseConsigneCG_rad_par_s*(gainCorrectionAngulaire*valAbsEcartAngulaire_rad -1)/(gainCorrectionAngulaire*valAbsEcartAngulaire_rad + 1);
    }
    else
    {
      cout <<"sens rotation roues opposé **** rayon cercle ideal "<<rayonCercleIdealInitialWPenCours<<endl;
      deltaV = vitesseConsigneCG_rad_par_s*(3*gainCorrectionAngulaire*valAbsEcartAngulaire_rad +1)/(gainCorrectionAngulaire*valAbsEcartAngulaire_rad + 1);     
    }
    cout <<"deltaV "<<deltaV<<endl;
    cout <<"vitesseConsigneCG_rad_par_s "<<vitesseConsigneCG_rad_par_s<<endl;
    if (ecartAngulaire_rad>0)
    {
      if (rayonCercleIdealInitialWPenCours >= (ecartEntreRoues_m/2.0))
      {
        consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
        consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
      }
      else
      {
        if (xCentreCercle>=0)
	{
	  //roue gauche marche avant
	  consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
          consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
	}
	else
	{
	  //roue droite marche avant mais moins rapide
	  consigneVitesseMoteurGauche_rad_s = -0.0001;
          consigneVitesseMoteurDroit_rad_s = 2*vitesseConsigneCG_rad_par_s;
	}
      }
    }
    else
    {
      if (rayonCercleIdealInitialWPenCours >= (ecartEntreRoues_m/2.0))
      {
        consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
        consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
      }
      else
      {
        if (xCentreCercle>=0)
	{
	  //roue gauche marche avant mais moins rapide
	  consigneVitesseMoteurGauche_rad_s = 2*vitesseConsigneCG_rad_par_s;
          consigneVitesseMoteurDroit_rad_s = -0.0001;
	}
	else
	{
	  //roue droite marche avant
	  consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
          consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
	}
      }
    }
    
    if (isBitSet(u8_CTRL_WP,BIT_SENS_WP)==false)
    {
      //marche arriere toute
      double vd_temp, vgtemp;
      vd_temp = consigneVitesseMoteurDroit_rad_s;
      vgtemp  = consigneVitesseMoteurGauche_rad_s;
      consigneVitesseMoteurDroit_rad_s = -vgtemp;
      consigneVitesseMoteurGauche_rad_s= -vd_temp;
    }
    
    
    log <<  positionAttVit->cap_deg<<"\t"<<capCercleIdeal <<"\t"<< ecartAngulaire_rad <<"\t"
        << consigneVitesseMoteurDroit_rad_s<<"\t"<< consigneVitesseMoteurGauche_rad_s<<"\t"<<vitesseConsigneCG_rad_par_s<<"\t"
	<< distanceCurviligneJusqueWP <<"\t"<< distanceDeceleration <<endl;
    delete centreDuCercle;  
    delete wpEnCours;
    delete positionAttVit;
}

void AlgoPIC::boucleGuidageSansTrigo(double & consigneVitesseMoteurDroit_rad_s, 
		                     double & consigneVitesseMoteurGauche_rad_s)
{
  //1 calcul du centre du cercle ideal
  //1.1 calcul du rayon du cercle
  //1.2 calcul du signe du rayon
  //1.3 calcul coordonnées du centre 
  
  //2 calcul de l'ecart angulaire au cercle ideal
  //2.1 calcul du vecteur ideal
  //2.2 produit vectoriel vecteur ideal * vecteur robot
  //2.3 calcul des normes des vecteurs
  //2.4 calcul du sin de l'ecart angulaire
  
  //3 calcul des vitesses des roues droites et gauches
  //3.1 calcul de la vitesse du centre de gravité
  //3.1 calcul de deltaV
  //3.2 calcul des vitesses de rotation
  
  
  //1.1 calcul du rayon du cercle
  double X_vecWP, Y_vecWP, d_cap;
  d_cap = u16_CAP_WP_100eme_deg;
  //X_vecWP = sin((d_cap/100.0)*M_PI/180);
 // Y_vecWP = cos((d_cap/100.0)*M_PI/180);
  
  X_vecWP = s16_XCAP_WP_mm/1000.0;
  Y_vecWP = s16_YCAP_WP_mm/1000.0;
  
  double X_wp,Y_wp,X_r,Y_r;
  X_wp = s32_POS_X_WP_um/1000000.0;
  Y_wp = s32_POS_Y_WP_um/1000000.0;
  X_r = s32_POS_X_ROBOT_um/1000000.0;
  Y_r = s32_POS_Y_ROBOT_um/1000000.0;
  double X = (-Y_vecWP*X_wp + X_vecWP*Y_wp + Y_vecWP*X_r - X_vecWP*Y_r)/sqrt(X_vecWP*X_vecWP + Y_vecWP*Y_vecWP);
  X = valAbs(X);
  double Y = (X_vecWP*X_r + Y_vecWP*Y_r - X_vecWP*X_wp - Y_vecWP*Y_wp)/sqrt(X_vecWP*X_vecWP + Y_vecWP*Y_vecWP);
  Y = valAbs(Y);
  double rayonAbsolu = (X*X+Y*Y)/(2*X);
  
  //1.2 calcul du signe du rayon (utilisation signe produit vectoriel)
  double X_vecRobotWP, Y_vecRobotWP;
  X_vecRobotWP = X_wp - X_r;
  Y_vecRobotWP = Y_wp - Y_r;
  double normeProduitVec = X_vecWP*Y_vecRobotWP - X_vecRobotWP*Y_vecWP;
  double rayon;
  if (normeProduitVec<0)
  {
    rayon = -rayonAbsolu;
  }
  else
  {
    rayon = rayonAbsolu;
  }
  
  //1.3 calcul coordonnées du centre
  double xCentreCercle, yCentreCercle;
  xCentreCercle = X_wp + Y_vecWP*rayon;
  yCentreCercle = Y_wp - X_vecWP*rayon;
  
  cout <<"xCentreCercle "<<xCentreCercle<<endl;
  cout <<"yCentreCercle "<<yCentreCercle<<endl;
  cout <<"rayon "<<rayon<<endl;
  cout <<"X "<<X<<endl;
  cout <<"Y "<<Y<<endl;
  
  //2.1 calcul du vecteur ideal
  double X_vecCcercleRobot, Y_vecCcercleRobot, X_vecIdeal, Y_vecIdeal;
  X_vecCcercleRobot = X_r - xCentreCercle;
  Y_vecCcercleRobot = Y_r - yCentreCercle;
  if (X==0)
  {
    //robot pile dans l'axe du wp
    X_vecIdeal = X_vecWP;
    Y_vecIdeal = Y_vecWP;
  }
  else
  {
    if (rayon>0)
    {
      X_vecIdeal = Y_vecCcercleRobot;
      Y_vecIdeal = -X_vecCcercleRobot;
    }
    else
    {
      X_vecIdeal = -Y_vecCcercleRobot;
      Y_vecIdeal = X_vecCcercleRobot;
    }
  }
  
  //2.2 produit vectoriel vecteur ideal * vecteur robot
  double X_vecRobot, Y_vecRobot, prodVecIdealVecRobot;
  d_cap = u16_CAP_ROBOT_100eme_deg;
  
  X_vecRobot = s32_X_CAP_ROBOT_um;
  Y_vecRobot = s32_Y_CAP_ROBOT_um;
  
  //X_vecRobot = sin((d_cap/100)*M_PI/180);
  //Y_vecRobot = cos((d_cap/100)*M_PI/180);
  if (isBitSet(u8_CTRL_WP,BIT_SENS_WP)==false)
  {
    //marche arriere toute
    X_vecRobot = -X_vecRobot;
    Y_vecRobot = -Y_vecRobot;
  }

  prodVecIdealVecRobot = X_vecIdeal*Y_vecRobot - X_vecRobot*Y_vecIdeal;
  
  //2.3 calcul des normes des vecteurs
  double normeVecIdeal = sqrt(X_vecIdeal*X_vecIdeal + Y_vecIdeal*Y_vecIdeal);
  double normeVecRobot = sqrt(X_vecRobot*X_vecRobot + Y_vecRobot*Y_vecRobot);
  
  //2.4 calcul du sin de l'ecart angulaire
  double sinEcartAngulaire = prodVecIdealVecRobot/(normeVecIdeal*normeVecRobot);
 
  //3.1 calcul de la vitesse du centre de gravité
  
  //calcul de la distance de décélération
  double vitesseWP_m_par_s = u16_VIT_WP_mm_par_s/1000.0;
  double deltaVitesseCourant = vitesseWP_m_par_s - vitesseConsigneCG_m_par_s;
  double distanceDeceleration = (deltaVitesseCourant*deltaVitesseCourant)/(2*decelMax_m_par_s2);
    
  //calcul de la distance curviligne jusqu'au prochain waypoint
  double X_vecCcercleWP, Y_vecCcercleWP;
  X_vecCcercleWP = X_wp - xCentreCercle;
  Y_vecCcercleWP = Y_wp - yCentreCercle;
  double distanceCurviligneJusqueWP = distCurviligneAversB(X_vecCcercleWP,Y_vecCcercleWP,X_vecCcercleRobot,Y_vecCcercleRobot, rayon);

  if (distanceCurviligneJusqueWP <= distanceDeceleration)
  {
    vitesseConsigneCG_m_par_s = vitesseConsigneCG_m_par_s - (pasTemps_ms*decelMax_m_par_s2)/1000.0;
  }
  else
  {
    if (vitesseConsigneCG_m_par_s < vitesseWP_m_par_s)
    {
      //il faut accélérer sans dépasser Vmax   
      double nouvelleVitesse_m_par_s = vitesseConsigneCG_m_par_s + (pasTemps_ms*accelMax_m_par_s2)/1000;
      vitesseConsigneCG_m_par_s = min(nouvelleVitesse_m_par_s, vitesseWP_m_par_s);
    }
  }

  //3.1 calcul de deltaV
  double deltaV; 
  double vitesseConsigneCG_rad_par_s = vitesseConsigneCG_m_par_s*rapportReduction/rayonRoues_m;
  double valAbsSinEcartAngulaire = valAbs(sinEcartAngulaire);
      
  if ( rayonAbsolu >= (ecartEntreRoues_m/2.0))
  {
    deltaV = vitesseConsigneCG_rad_par_s*(gainCorrectionAngulaire*valAbsSinEcartAngulaire -1)/(gainCorrectionAngulaire*valAbsSinEcartAngulaire + 1);
  }
  else
  {
    deltaV = vitesseConsigneCG_rad_par_s*(3*gainCorrectionAngulaire*valAbsSinEcartAngulaire +1)/(gainCorrectionAngulaire*valAbsSinEcartAngulaire + 1);     
  }
  cout <<"deltaV "<<deltaV<<endl;
  cout <<"vitesseConsigneCG_rad_par_s "<<vitesseConsigneCG_rad_par_s<<endl;
  
  //3.2 calcul des vitesses de rotation 
  if (sinEcartAngulaire>0)
  {
    if (rayonAbsolu >= (ecartEntreRoues_m/2.0))
    {
      consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
      consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
    }
    else
    {
      if (xCentreCercle>=0)
      {
	//roue gauche marche avant
	consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
        consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
      }
      else
      {
	//roue droite marche avant mais moins rapide
	consigneVitesseMoteurGauche_rad_s = -0.0001;
        consigneVitesseMoteurDroit_rad_s = 2*vitesseConsigneCG_rad_par_s;
      }
    }
  }
  else
  {
    if (rayonAbsolu >= (ecartEntreRoues_m/2.0))
    {
      consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
      consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
    }
    else
    {
      if (xCentreCercle>=0)
      {
	//roue gauche marche avant mais moins rapide
	consigneVitesseMoteurGauche_rad_s = 2*vitesseConsigneCG_rad_par_s;
        consigneVitesseMoteurDroit_rad_s = -0.0001;
      }
      else
      {
	//roue droite marche avant
	consigneVitesseMoteurGauche_rad_s = vitesseConsigneCG_rad_par_s-deltaV;
        consigneVitesseMoteurDroit_rad_s = vitesseConsigneCG_rad_par_s+deltaV;
      }
    }
  }
    
  if (isBitSet(u8_CTRL_WP,BIT_SENS_WP)==false)
  {
      //marche arriere toute
      double vd_temp, vgtemp;
      vd_temp = consigneVitesseMoteurDroit_rad_s;
      vgtemp  = consigneVitesseMoteurGauche_rad_s;
      consigneVitesseMoteurDroit_rad_s = -vgtemp;
      consigneVitesseMoteurGauche_rad_s= -vd_temp;
  }
  
  double cap_deg = u16_CAP_WP_100eme_deg/100.0;
  double capCercleIdeal = atan2(X_vecIdeal,Y_vecIdeal);
  log << xCentreCercle <<"\t"
      << yCentreCercle <<"\t"
      << capCercleIdeal <<"\t"<< sinEcartAngulaire <<"\t"
      << consigneVitesseMoteurDroit_rad_s<<"\t"<< consigneVitesseMoteurGauche_rad_s<<"\t"<<vitesseConsigneCG_rad_par_s<<"\t"
      << distanceCurviligneJusqueWP <<"\t"<< distanceDeceleration <<endl;
}

double distCurviligneAversB(double X_A,double Y_A, double X_B, double Y_B, double rayon)
{
  double prodScalaire = X_A*X_B + Y_A*Y_B;
  double prodVectoriel = X_A*Y_B - X_B*Y_A;
  double quadrant = 0; //NO = 0, SO = 1, SE = 2, NE = 3
  double demiQuadrant = 0;
  double huitieme = 0;
  double distanceCurviligne;
  double rayonAbsolu = valAbs(rayon);
  
  cout<<"A:"<<X_A<<","<<Y_A<<" B:"<<X_B<<","<<Y_B<<endl;
  cout<<"Scalaire"<<prodScalaire<<endl;
  cout<<"Vectoriel"<<prodVectoriel<<endl;
  
  if (rayonAbsolu>10.0)
  {
    distanceCurviligne = sqrt((X_A-X_B)*(X_A-X_B)+(Y_A-Y_B)*(Y_A-Y_B));
  }
  else
  {
  double X_vecAvant,Y_vecAvant,X_vecApres,Y_vecApres;
  if (prodScalaire>=0)
  {
    //cos positif
    if (prodVectoriel>=0)
    {
      //sin positif
      quadrant = 0;
      X_vecAvant = X_A;
      Y_vecAvant = Y_A;
      X_vecApres = -Y_A;
      Y_vecApres = X_A;   
    }
    else
    {
      //sin negatif
      quadrant = 3;
      X_vecAvant = Y_A;
      Y_vecAvant = -X_A;
      X_vecApres = X_A;
      Y_vecApres = Y_A;
    }
  }
  else
  {
    //cos negatif
    if (prodVectoriel>=0)
    {
      //sin positif
      quadrant = 1;
      X_vecAvant = -Y_A;
      Y_vecAvant = X_A;
      X_vecApres = -X_A;
      Y_vecApres = -Y_A;
    }
    else
    {
      //sin negatif
      quadrant = 2;
      X_vecAvant = -X_A;
      Y_vecAvant = -Y_A;
      X_vecApres = Y_A;
      Y_vecApres = -X_A;
    } 
  }
  cout<<"quadrant "<<quadrant<<endl;
  
  double carreDistanceAvant = (X_vecAvant-X_B)*(X_vecAvant-X_B) + (Y_vecAvant-Y_B)*(Y_vecAvant-Y_B);
  double carreDistanceApres = (X_vecApres-X_B)*(X_vecApres-X_B) + (Y_vecApres-Y_B)*(Y_vecApres-Y_B);
  
  if (carreDistanceAvant>=carreDistanceApres)
  {
    demiQuadrant = 0.5;
  }
  
  huitieme = quadrant+demiQuadrant;
  cout<<"huitieme "<<huitieme<<endl;
  if (huitieme<0.5)
  {
    //entre 0 et 45°
    distanceCurviligne = sqrt(carreDistanceAvant); 
  }
  else
  {
    if (huitieme<1)
    {
      //entre 45° et 90°
      distanceCurviligne = rayonAbsolu*1.5707 - sqrt(carreDistanceApres);
    }
    else
    {
      if (huitieme<1.5)
      {
        //entre 90° et 135°
        distanceCurviligne = rayonAbsolu*1.5707 + sqrt(carreDistanceAvant);
      }
      else
      {
        if (huitieme<2.0)
        {
          //entre 135° et 180°
          distanceCurviligne = rayonAbsolu*3.1415 - sqrt(carreDistanceApres);
        }
        else
        {
          if (huitieme<2.5)
          {
            //entre 180° et 225°
            distanceCurviligne = rayonAbsolu*3.1415 + sqrt(carreDistanceAvant);
          }
          else
          {
            if (huitieme<3.0)
            {
              //entre 225° et 270°
              distanceCurviligne = rayonAbsolu*4.7123 - sqrt(carreDistanceApres);
            }
            else
            {
              if (huitieme<3.5)
              {
                //entre 270° et 315°
                distanceCurviligne = rayonAbsolu*4.7123 + sqrt(carreDistanceAvant);
              }
              else
              {
                //entre 315° et 360°
		distanceCurviligne = rayonAbsolu*6.2831 - sqrt(carreDistanceApres);
              }
            }
          }
        }
      }
    }
  }
  if (rayon<0)
  {
    distanceCurviligne = 6.2831*rayonAbsolu - distanceCurviligne;
  }
  }
  cout<<"distanceCurviligne "<<distanceCurviligne<<endl;
  return distanceCurviligne;
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
  bool transitionWP = false;
  
  if (isBitSet(u8_CTRL_WP,BIT_ROT))
  {
    double ecartAngulaireCourant_100eme_deg = u16_CAP_WP_100eme_deg - u16_CAP_ROBOT_100eme_deg;
  
    normalise0_360_100eme_deg(ecartAngulaireCourant_100eme_deg);
    ecartAngulaireCourant_100eme_deg = valAbs(ecartAngulaireCourant_100eme_deg);
    if (!isBitSet(u8_CTRL_WP,BIT_SENS_ROT))
    {
      ecartAngulaireCourant_100eme_deg = 36000-ecartAngulaireCourant_100eme_deg;
    }

    if (ecartAngulaireCourant_100eme_deg<=50)
    {	
      transitionWP = true;
      vitesseConsigneRoue_mm_par_s = 0;
    }
    int numeroWP_out = numeroWPenCours;
    log << numeroWP_out <<"\t"<< ecartAngulaireCourant_100eme_deg <<"\t";
  }
  else
  {
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
        transitionWP = true;
	u64_CARRE_DIST_ROBOT_WP = 1E15;
	//la dist au WP commence à augmenter: on transitionne
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
  
  if (transitionWP)
  {
        numeroWPenCours++;
	if (numeroWPenCours==trameRecue->u8_NUM_WP2)
	{
		s32_POS_X_WP_um = trameRecue->s16_POS_X_WP2_mm*1000;
		s32_POS_Y_WP_um = trameRecue->s16_POS_Y_WP2_mm*1000;
		u16_CAP_WP_100eme_deg = trameRecue->u16_CAP_WP2_deg*100;
		u16_VIT_WP_mm_par_s = trameRecue->u8_VIT_WP2_cm_par_s*10;
		u8_CTRL_WP   = trameRecue->u8_CTRL_WP2;
	}
	else
	{
	  //plus de waypoint : ARRET
	  setBit(BIT_WP_NUL,u8_CTRL_WP);
	}
  }
}
