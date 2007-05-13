#ifndef ROBOT_ADVERSE_H
#define ROBOT_ADVERSE_H
#include <QtGlobal>
#include <complex>
using namespace std;
// 
// C'est la class qui contient toutes les infos que l'on a sur le robot adverse
//

// A compléter en fonction de la forme des infos dispo et utile
// - cap relatif
// - estimation de distance
// - estimation de la présence dans un "secteur" (si on coupe le terrain en 9)

class robot_adverse_t {
 public:

  complex<double> position;
  float taille_m; //rayon du robot adverse
  float rayon_incertitude; // 

  robot_adverse_t() {}
};

#endif
