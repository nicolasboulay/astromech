#include "module.h"
#include "systeme.h"
#include "trace.h"
#include "tinyxml.h"
#include "geom2D.h"

using namespace std;

Module::Module(Systeme *s,Trace * t)
{
  trace = t;
  src = "module.cpp";
  cl = "Module";
  message = "debut";
  trace->print(src,cl,"Module",message);
  systPere = s;
  simuPere = s->getSimuPere();
  message = "fin";
  trace->print(src,cl,"Module",message);
}

Module::~Module(void)
{
  message = "debut";
  trace->print(src,cl,"~Module",message);
  log.close();
  message = "fin";
  trace->print(src,cl,"~Module",message);
}

string Module::getNomModule(void)
{
  return nom;
}

string Module::getNomClasse(void)
{
  return nom_classe;
}

void Module::setPasTemps(int pasDeTemps_ms)
{
  pasTemps_ms = pasDeTemps_ms;
}
    
void Module::setPosition(Point2D *positionCG)
{
  //en fait tous les positionCourante de tous les modules d'un systeme
  //pointent vers le meme Point2D
  //du coup, la mise à jour est automatique pour tous les modules
  positionCourante = positionCG;
}
void Module::setAttitude(double * attitude_rad)
{
  //de meme pour l'attitude
  attitudeCourante_rad = attitude_rad;
}
void Module::execute(int tempsCourant_ms)
{
  message = "debut";
  trace->print(src,cl,"execute",message);
  message = "fin";
  trace->print(src,cl,"execute",message);
}

void Module::chargerXML(TiXmlElement* pModuleXML)
{
    message = "debut";
    trace->print(src,cl,"chargerXML",message);
    TiXmlElement* pElt;
    TiXmlHandle handleModule(0);
    handleModule = TiXmlHandle(pModuleXML);
    
    pElt = handleModule.FirstChild("nom_classe").Element();
    nom_classe = pElt->GetText();
    message = "nom_classe : " + nom_classe;
    trace->print(src,cl,"chargerXML",message);
    
    pElt = handleModule.FirstChild("nom").Element();
    nom = pElt->GetText();
    message = "nom : " + nom;
    trace->print(src,cl,"chargerXML",message);
    
    pElt = handleModule.FirstChild( "relation" ).Element();
    if (pElt == NULL)
    {
      message = "pas de relations";
      trace->print(src,cl,"chargerXML",message);
    }
    else
    {
      for( pElt; pElt; pElt=pElt->NextSiblingElement("relation"))
      {
        string relation = pElt->GetText();
        tabRelation.push_back(relation);
        message = "relation : " + relation;
        trace->print(src,cl,"chargerXML",message);
      }
    }
    
    string nomFichierLog = systPere->getNomSysteme() +"_"+ nom + ".out";
    message = nomFichierLog;
    trace->print(src,cl,"chargerXML",message);
    log.open(nomFichierLog.c_str(),ofstream::out | ofstream::trunc);
    
    message = "fin";
    trace->print(src,cl,"chargerXML",message);
}

void Module::connectModules(void)
{
  message = "debut";
  trace->print(src,cl,"connectModules",message);
  message = "fin";
  trace->print(src,cl,"connectModules",message);
}
