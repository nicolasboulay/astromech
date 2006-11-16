#ifndef ModuleH
#define ModuleH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Systeme;
class TiXmlElement;
class Trace;
class Point2D;
class Simulation;

typedef vector<string> vectorString;

class Module
{
  protected: //accessible uniquement par héritage
    Systeme * systPere;
    Simulation *simuPere;
    Trace * trace;
    string src;
    string cl;
    string message;
    string nom;               //nom precis, ex : "moteur_droit"
    string nom_classe;        //ex : E_Moteur
    vectorString tabRelation; //nom precis des modules a lier
    int pasTemps_ms;
    ofstream log;
    Point2D * positionCourante;
    double * attitudeCourante_rad;
    
  public : // accessible partout
    Module(Systeme *s,Trace * t);
    virtual ~Module(void);
    string getNomModule(void);
    string getNomClasse(void);
    void setPasTemps(int pasDeTemps_ms);
    void setPosition(Point2D *positionCG);
    void setAttitude(double * attitude_rad);
    virtual void execute(int tempsCourant_ms);
    virtual void chargerXML(TiXmlElement* pModuleXML);
    virtual void connectModules(void);
};
#endif
