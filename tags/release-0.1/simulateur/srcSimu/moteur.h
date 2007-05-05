#ifndef MoteurH
#define MoteurH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Effecteur;
class Trace;
class Systeme;

class Moteur : public Effecteur
{
  protected: //accessible uniquement par héritage
    double coeffRedErreur;
    double coeffSatAccelRot_rad_par_s;
    double consigneVitesse_rad_par_s;
    double deltaRot_rad;
    double deltaRotPrec_rad;
    
    Contour * contour;
    
  public : // accessible partout
    Moteur(Systeme *s,Trace * t);
    ~Moteur(void);
    double getDeltaRot_rad(void);
    void setConsigneVitesse(double vitesse_rad_par_s);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
};
#endif
