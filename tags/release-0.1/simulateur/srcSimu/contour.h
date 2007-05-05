#ifndef ContourH
#define ContourH

#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Senseur;
class Systeme;
class Trace;
class Point2D;
class Segment2D;

typedef vector<Segment2D *> vectorSegment2D;
typedef vector<Point2D *> vectorPoint2D;
typedef vector<Systeme *> vectorSysteme;

class Contour : public Senseur
{
  protected: //accessible uniquement par héritage
    vectorSegment2D tabSegments_Contour; //contour de l'objet
    
    vectorPoint2D tabPointContacts_Obstacles;
    vectorPoint2D tabPointContacts_Systemes;
    vectorSysteme tabSystemesEnContact;
    
    bool intersection;

  public : // accessible partout
    Contour(Systeme *s,Trace * t);
    ~Contour(void);
    void execute(int tempsCourant_ms);
    void chargerXML(TiXmlElement* pModuleXML);
    void connectModules(void);
    bool intersectionEnCours(void);
};
#endif
