#ifndef ChassisH
#define ChassisH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Effecteur;
class Trace;
class Systeme;
class Moteur;

class Chassis : public Effecteur
{
  protected: //accessible uniquement par héritage
    double rapportReduction;
    double rayonRoueDroite_m;
    double rayonRoueGauche_m;
    double ecartEntreRoues_m;
    double glissementRoueDroite_mm_par_m;
    double glissementRoueGauche_mm_par_m;
    
    Moteur * moteurDroit;
    Moteur * moteurGauche;
    
  public : // accessible partout
    Chassis(Systeme *s,Trace * t);
    ~Chassis(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
};
#endif
