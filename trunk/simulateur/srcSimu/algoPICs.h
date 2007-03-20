#ifndef AlgoPICH
#define AlgoPICH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
class Algo;
class Trace;
class Systeme;
class ComSeriePC_PIC;
class Moteur;
class Codeuse;
class WayPoint;
class TramePC_PIC;
class TramePIC_PC;

class AlgoPIC : public Algo
{
  protected: //accessible uniquement par héritage
    ComSeriePC_PIC * comSerie;
    Codeuse * codeuseDroite;
    Codeuse * codeuseGauche;
    Moteur * moteurDroit;
    Moteur * moteurGauche;
    
    TramePC_PIC * trameRecue;
    TramePIC_PC * trameEnvoyee;
    
    signed int     s32_POS_X_ROBOT_um; //um = µm = microm
    signed int     s32_POS_Y_ROBOT_um;
    unsigned short u16_CAP_ROBOT_100eme_deg;
    unsigned short u16_VIT_ROBOT_mm_par_s;
    
    unsigned char  numeroWPenCours;
    signed int     s32_POS_X_WP_um;
    signed int     s32_POS_Y_WP_um;
    unsigned short u16_CAP_WP_100eme_deg;
    unsigned short u16_VIT_WP_mm_par_s;
    unsigned char  u8_CTRL_WP;
    
    float          u64_CARRE_DIST_ROBOT_WP;
    
    
    double vitesseConsigneCG_m_par_s;

    int sensRotationMoteurDroit;
    int sensRotationMoteurGauche;
    double rayonCercleIdealInitialWPenCours;
    double sommeEcartsAngulaire;
    bool wayPointConnu;
    
    
    //donnéesXML
    int nbPointsParTourMoteur;
    double rapportReduction;
    double rayonRoues_m;
    double ecartEntreRoues_m;
    double accelMax_m_par_s2;
    double decelMax_m_par_s2;
    double gainCorrectionAngulaire;
    double tempsMiseEnDecel_s;
    
  public : // accessible partout
    AlgoPIC(Systeme *s,Trace * t);
    ~AlgoPIC(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
    void miseAjourNavigation(const double & codeuseMoteurDroit,
                             const double & codeuseMoteurGauche,
			     const int    & sensRotationMoteurDroit,
		             const int    & sensRotationMoteurGauche);
    void boucleGuidage(double & consigneVitesseMoteurDroit_rad_s, 
		       double & consigneVitesseMoteurGauche_rad_s);
    void miseAjourWaypoint(void);
    void calculXcentreCercleDansRepereWaypoint(const WayPoint * wpEnCours, double & xCentreCercle);
					       
    float carreDistanceEuclidienne(float x1,
                                          float y1,
				          float x2,
				          float y2);
};
#endif

