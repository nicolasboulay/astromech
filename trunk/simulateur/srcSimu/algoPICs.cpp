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
  numeroSequenceWP = -1;
  positionAttVit = new WayPoint(0,0,0,0);
  sensRotationMoteurDroit = 1;
  sensRotationMoteurGauche = 1;
  rayonCercleIdealInitialWPenCours = 1;

  message = "fin";
  trace->print(src,cl,"AlgoPIC",message);
}

AlgoPIC::~AlgoPIC(void)
{
  message = "debut";
  trace->print(src,cl,"~AlgoPIC",message);
  
  delete positionAttVit;
  
  message = "fin";
  trace->print(src,cl,"~AlgoPIC",message);
}

void AlgoPIC::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  log <<  tempsCourant_ms<<"\t";
  if (comSerie->tramePC_PIC_recue())
  {
    TramePC_PIC * trameRecue;
    trameRecue = comSerie->getTramePC_PIC();
    cout << "trameRecue "<<trameRecue<<endl;
    cout << "listeWP "<<&(trameRecue->tabWayPoint)<<endl;
    //lecture trame et sauvegarde des données en local
    if (this->numeroSequenceWP!=trameRecue->numeroSequenceWP)
    {
      this->numeroSequenceWP = trameRecue->numeroSequenceWP;
      //on efface la liste des wp precedente
      vectorWayPoint::iterator it;
      cout << "taille listeWPC "<<listeWPCourante.size()<<endl;;
      for (it = listeWPCourante.begin();it!=listeWPCourante.end();++it)
      {
        delete(*it);
      }
      listeWPCourante.clear();
      for (int indWP = 0;indWP < trameRecue->tabWayPoint.size();indWP++)
      {
        //recopie des wp dans la trame
        WayPoint * wp = new WayPoint(trameRecue->tabWayPoint[indWP]->pt->x,
                                     trameRecue->tabWayPoint[indWP]->pt->y,
				     trameRecue->tabWayPoint[indWP]->attitude_rad,
				     trameRecue->tabWayPoint[indWP]->vitesse_m_par_s);
        listeWPCourante.push_back(wp);
      }
    }  
    delete trameRecue;
    //traitements
    
    //création d'une trame pour le PC
    TramePIC_PC * trameEnvoyee = new TramePIC_PC();
    comSerie->sendToPC(trameEnvoyee);
  }
  //code temporaire
 /* vectorWayPoint listeWP;
  WayPoint * wp1 = new WayPoint(1,1,0,0.3);//coord (1,1) vit 0.3m/s
  WayPoint * wp2 = new WayPoint(2,2,1.57,0.5);//coord (2,2) vit 1m/s
  WayPoint * wp3 = new WayPoint(3,2,1.57,0.5);//coord (3,2) vit 1m/s
  WayPoint * wp4 = new WayPoint(3,0,4.71,0);//coord (3,0) vit 1m/s
  listeWP.push_back(wp1);
  listeWP.push_back(wp2);
  listeWP.push_back(wp3);
  listeWP.push_back(wp4);*/
  for (int indWP = 0;indWP < listeWPCourante.size();indWP++)
      {
  cout << listeWPCourante[indWP]->pt->x<<endl;
  cout << listeWPCourante[indWP]->pt->y<<endl;
  cout << listeWPCourante[indWP]->attitude_rad<<endl;
  cout << listeWPCourante[indWP]->vitesse_m_par_s<<endl;}
  //lecture des codeuses
  double codeuseMoteurDroit = codeuseDroite->getNbImpulsions();
  double codeuseMoteurGauche = codeuseGauche->getNbImpulsions();
  
  //mise à jour navigation
  miseAjourNavigation(codeuseMoteurDroit,
                      codeuseMoteurGauche,
		      sensRotationMoteurDroit,
                      sensRotationMoteurGauche);
  
  //incrémentation du waypoint si le courant est atteint
  miseAjourWaypoint(listeWPCourante);
  
  //boucle de guidage pour vitesse CG non nulle (depend du WP)
  double consigneVitesseMoteurDroit_rad_s = 0;
  double consigneVitesseMoteurGauche_rad_s = 0;
  boucleGuidage(listeWPCourante,
                consigneVitesseMoteurDroit_rad_s, 
		consigneVitesseMoteurGauche_rad_s);
		
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

  log <<"t(s)"<<"\t"<<"xR(m)"<<"\t"<<"yR(m)"<<"\t"<<"vR(m/s)"<<"\t"<<"N° WP"<<"\t"<<"dist WP (m)"<<"\t"
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
  double facteurConversion = 2*M_PI*rayonRoues_m/(nbPointsParTourMoteur*rapportReduction);//0.0000175;
  double deplacementRoueDroite = sensRotationMoteurDroit*facteurConversion*impCodeuseMoteurDroit;
  double deplacementRoueGauche = sensRotationMoteurGauche*facteurConversion*impCodeuseMoteurGauche;

  //double deplacementRoueDroite = facteurConversion*impCodeuseMoteurDroit;
  //double deplacementRoueGauche = facteurConversion*impCodeuseMoteurGauche;

  double deltaAvance_m = (deplacementRoueDroite + deplacementRoueGauche)/2;
  double new_pos_x_m = positionAttVit->pt->x + deltaAvance_m * sin(positionAttVit->attitude_rad);
  double new_pos_y_m = positionAttVit->pt->y + deltaAvance_m * cos(positionAttVit->attitude_rad);
  double new_att_rad = positionAttVit->attitude_rad + atan((deplacementRoueGauche - deplacementRoueDroite)/ecartEntreRoues_m);
  normalise0_2PI(new_att_rad);
  //mise à jour position
  positionAttVit->pt->x = new_pos_x_m;
  positionAttVit->pt->y = new_pos_y_m;
  positionAttVit->attitude_rad = new_att_rad;
  positionAttVit->vitesse_m_par_s = (deltaAvance_m/pasTemps_ms)*1000; //100Hz
  log << new_pos_x_m <<"\t"<< new_pos_y_m <<"\t"<<positionAttVit->vitesse_m_par_s <<"\t";
}

void AlgoPIC::boucleGuidage(vectorWayPoint listeWP,
                double & consigneVitesseMoteurDroit_rad_s, 
		double & consigneVitesseMoteurGauche_rad_s)
{
  WayPoint * wpEnCours;
  if (numeroWPenCours < listeWP.size())
  {
    wpEnCours = listeWP[numeroWPenCours];
 
    Point2D * centreDuCercle = new Point2D(0,0);
    //calcul de la position courante dans le repere du WP en cours
    Point2D * ptCourant = new Point2D(positionAttVit->pt);
    ptCourant->calculePointDansNouveauRepere(wpEnCours->pt,wpEnCours->attitude_rad);
    
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
      centreDuCercle->calculePointDansRepereTerrain(wpEnCours->pt,wpEnCours->attitude_rad);
      log << centreDuCercle->x <<"\t"<<centreDuCercle->y<<"\t";
      log <<  wpEnCours->attitude_rad<<"\t"<<xRobot <<"\t"<< yRobot <<"\t"<< xCentreCercle<<"\t";

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
      capCercleIdeal = wpEnCours->attitude_rad;
    }
    normalise0_2PI(capCercleIdeal);
    double ecartAngulaire_rad = capCercleIdeal - positionAttVit->attitude_rad;
    normaliseMPI_PPI(ecartAngulaire_rad);
    
    //calcul de la vitesse du centre de gravité du robot
    double distanceCurviligneJusqueWP = 0;
    double distanceDeceleration = 0;
    if (vitesseConsigneCG_m_par_s <= wpEnCours->vitesse_m_par_s)
    {
      //il faut accélérer
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
      //il faut décelerer le plus tard possible
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
      
      //calcul de la distance de décélération
      double deltaVitesseCourant = wpEnCours->vitesse_m_par_s - vitesseConsigneCG_m_par_s;
      distanceDeceleration = (deltaVitesseCourant*deltaVitesseCourant)/(2*decelMax_m_par_s2)
                             +vitesseConsigneCG_m_par_s*tempsMiseEnDecel_s;//terme correctif pour tenir compte de la différence entre
			     //vitesse de consigne et vitesse réalisée
      
      //faut il décelerer?
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
      deltaV = vitesseConsigneCG_rad_par_s*(gainCorrectionAngulaire*valAbsEcartAngulaire_rad -1)/(gainCorrectionAngulaire*valAbsEcartAngulaire_rad + 1);
    }
    else
    {
      deltaV = vitesseConsigneCG_rad_par_s*(3*gainCorrectionAngulaire*valAbsEcartAngulaire_rad +1)/(gainCorrectionAngulaire*valAbsEcartAngulaire_rad + 1);
    }
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
    log <<  positionAttVit->attitude_rad<<"\t"<<capCercleIdeal <<"\t"<< ecartAngulaire_rad <<"\t"
        << consigneVitesseMoteurDroit_rad_s<<"\t"<< consigneVitesseMoteurGauche_rad_s<<"\t"<<vitesseConsigneCG_rad_par_s<<"\t"
	<< distanceCurviligneJusqueWP <<"\t"<< distanceDeceleration <<"\t"<<sommeEcartsAngulaire<<endl;
    delete centreDuCercle;
  }
  else
  {
    //plus de WP, il faut arreter le robot
    consigneVitesseMoteurDroit_rad_s = 0;
    consigneVitesseMoteurGauche_rad_s = 0;
    log <<endl;
  }
  
}
void AlgoPIC::miseAjourWaypoint(vectorWayPoint listeWP)
{
  WayPoint * wpEnCours;
  if (numeroWPenCours < listeWP.size())
  {
    wpEnCours = listeWP[numeroWPenCours];
    double distanceRobot_WPenCours = positionAttVit->pt->distanceAuPoint(wpEnCours->pt);
    
    //OU critere de transition sur la distance normale mais attention au waypoint d'entrée de jeu franchi dans ce cas (mais avec un angle incorrect)
    
    
    if (distanceRobot_WPenCours < 0.01)
    {
        numeroWPenCours++;
	if (numeroWPenCours < listeWP.size())
        {
	   wpEnCours = listeWP[numeroWPenCours];
	   double xCentreCercle;
	   calculXcentreCercleDansRepereWaypoint(*wpEnCours,
                                                 xCentreCercle);
		
		/*
		//calcul de la position courante dans le repere du WP en cours
		Point2D * ptCourant = new Point2D(positionAttVit->pt);
		ptCourant->calculePointDansNouveauRepere(wpEnCours->pt,wpEnCours->attitude_rad);
    
		//calcul du centre du cercle ideal pour joindre le waypoint dans le repere du waypoint
		double xRobot,yRobot;
		xRobot = ptCourant->x;
		yRobot = ptCourant->y;
		delete ptCourant;
		
		if (xRobot != 0) //cas ou le robot n'est pas pile dans l'axe du waypoint
		{
		xCentreCercle = (xRobot*xRobot+yRobot*yRobot)/(2*xRobot);*/
		if (xCentreCercle>=0)
		{
			rayonCercleIdealInitialWPenCours = xCentreCercle;
		}
		else
		{
			rayonCercleIdealInitialWPenCours = -xCentreCercle;
		}
		/*}
		else
		{
		rayonCercleIdealInitialWPenCours = 100000;
		}*/
      }
    }
        
    log << numeroWPenCours <<"\t"<< distanceRobot_WPenCours <<"\t";
  }
}

void AlgoPIC::calculXcentreCercleDansRepereWaypoint(const WayPoint & wpEnCours,
                                                    double & xCentreCercle)
{
  //calcul de la position courante dans le repere du WP en cours
  Point2D * ptCourant = new Point2D(positionAttVit->pt);
  ptCourant->calculePointDansNouveauRepere(wpEnCours.pt,wpEnCours.attitude_rad);
    
  //calcul du centre du cercle ideal pour joindre le waypoint dans le repere du waypoint
  double xRobot,yRobot;
  xRobot = ptCourant->x;
  yRobot = ptCourant->y;
  delete ptCourant;
  if (xRobot != 0) //cas ou le robot n'est pas pile dans l'axe du waypoint
  {
    xCentreCercle = (xRobot*xRobot+yRobot*yRobot)/(2*xRobot); 
  }
  else
  {
    xCentreCercle = RAYON_COURBURE_INFINIE;
  }
}

