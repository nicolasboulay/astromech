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

double distCurviligneAversB(double X_A,double Y_A, double X_B, double Y_B,double rayon);

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
    signed int     s32_X_CAP_ROBOT_um;
    signed int     s32_Y_CAP_ROBOT_um;
    unsigned short u16_VIT_ROBOT_mm_par_s;
    
    unsigned char  numeroWPenCours;
    signed int     s32_POS_X_WP_um;
    signed int     s32_POS_Y_WP_um;
    unsigned short u16_CAP_WP_100eme_deg;
    signed short   s16_XCAP_WP_mm;
    signed short   s16_YCAP_WP_mm;
    unsigned short u16_VIT_WP_mm_par_s;
    unsigned char  u8_CTRL_WP;
    
    float          u64_CARRE_DIST_ROBOT_WP;
    
    double qw;
    double qx;
    double qy;
    double qz;
    double posX;
    double posY;
    double cap;
    double vitesse;
    
    double vitesseConsigneCG_m_par_s;
    int vitesseConsigneRoue_mm_par_s;

    int sensRotationMoteurDroit;
    int sensRotationMoteurGauche;
    double rayonCercleIdealInitialWPenCours;
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
			     
    void miseAjourNavigationQuats(const double & codeuseMoteurDroit,
                             const double & codeuseMoteurGauche,
			     const int    & sensRotationMoteurDroit,
		             const int    & sensRotationMoteurGauche);
			     
    void boucleGuidageRotation(double & consigneVitesseMoteurDroit_rad_s, 
		               double & consigneVitesseMoteurGauche_rad_s);
    void boucleGuidage(double & consigneVitesseMoteurDroit_rad_s, 
		       double & consigneVitesseMoteurGauche_rad_s);
		       
    void boucleGuidageSansTrigo(double & consigneVitesseMoteurDroit_rad_s, 
		                double & consigneVitesseMoteurGauche_rad_s);
		       
    void miseAjourWaypoint(void);
					       
    float carreDistanceEuclidienne(float x1,
                                          float y1,
				          float x2,
				          float y2);
};
#endif

