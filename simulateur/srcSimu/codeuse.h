#ifndef CodeuseH
#define CodeuseH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Senseur;
class Systeme;
class Trace;
class Moteur;

class Codeuse : public Senseur
{
  protected: //accessible uniquement par héritage
    int nbPointsParTour;
    double nbImpulsionComptees;
    
    Moteur * moteur;

  public : // accessible partout
    Codeuse(Systeme *s,Trace * t);
    ~Codeuse(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
    double getNbImpulsions(void);
};
#endif
