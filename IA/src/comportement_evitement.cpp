// Qt things
#include <QTextStream>
#include <QVector>

// std things
#include <cassert>
#include <complex>

//global IA things
#include "common.h"

// input/output/state of IA 
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"

// helper for manipulating mecanics of the robot
#include "barillet.h"


#include "comportement_evitement.h"


 comportement_evitement_t::comportement_evitement_t(int _name) : comportement_t(_name) 
{
    QTextStream qout(stdout);
    output.name("evitement comportement");
    qout << "init comportement_evitement:" << _name << endl;
}

#define SEUIL_US 100

comportement_result_t 
comportement_evitement_t::execute(const trame_in_t & tin,  
			     internal_state_t & state) 
{

  for(int n=0;n<REFEREE_NUMBER;n++){
    output.isActive[n]=false;
  }
  
  // Détection des obstacles
  if(tin.mesure_us_1 < 0 ) { // CALIBRAGE distance ultra-son

  } else {
  	return true;
  }
  
  // Estimation de la distance
  
  // Modélisation de l'obstacle (position, dimension)
  
  // Confirmation de sa position dans le champ de jeu

  // Envoyer un waypoint d'arrêt immédiat

  return output;
};
