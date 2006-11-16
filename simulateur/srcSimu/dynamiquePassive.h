#ifndef DynamiquePassiveH
#define DynamiquePassiveH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Effecteur;
class Trace;
class Systeme;
class Contour;

class DynamiquePassive : public Effecteur
{
  protected: //accessible uniquement par héritage  
    Contour * contour;
    
  public : // accessible partout
    DynamiquePassive(Systeme *s,Trace * t);
    ~DynamiquePassive(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
};
#endif
