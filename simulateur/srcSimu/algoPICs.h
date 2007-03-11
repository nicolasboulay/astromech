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
typedef vector<WayPoint *> vectorWayPoint;

class AlgoPIC : public Algo
{
  protected: //accessible uniquement par héritage
    ComSeriePC_PIC * comSerie;
    Codeuse * codeuseDroite;
    Codeuse * codeuseGauche;
    Moteur * moteurDroit;
    Moteur * moteurGauche;
    WayPoint * positionAttVit;
    int numeroWPenCours;
    double vitesseConsigneCG_m_par_s;
    int numeroSequenceWP;
    vectorWayPoint listeWPCourante;
    int sensRotationMoteurDroit;
    int sensRotationMoteurGauche;
    double rayonCercleIdealInitialWPenCours;
    double sommeEcartsAngulaire;
    
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
    void boucleGuidage(vectorWayPoint listeWP,
                double & consigneVitesseMoteurDroit_rad_s, 
		double & consigneVitesseMoteurGauche_rad_s);
    void miseAjourWaypoint(vectorWayPoint listeWP);
    void calculXcentreCercleDansRepereWaypoint(const WayPoint & wpEnCours,
                                               double & xCentreCercle);
};
#endif

