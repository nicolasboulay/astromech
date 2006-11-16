#ifndef GuidageConstantH
#define GuidageConstantH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
class Algo;
class Trace;
class Systeme;
class Moteur;

class GuidageConstant : public Algo
{
  protected: //accessible uniquement par héritage
    double vitConsigneMoteurDroit_rad_s; 
    double vitConsigneMoteurGauche_rad_s; 
    Moteur * moteurDroit;
    Moteur * moteurGauche;
    
  public : // accessible partout
    GuidageConstant(Systeme *s,Trace * t);
    ~GuidageConstant(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
};
#endif
