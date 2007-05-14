#ifndef COMPORTEMENT_EVITEMENT_H
#define COMPORTEMENT_EVITEMENT_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>
#include "barillet.h"

// class d'évitement le but est d'éviter l'adversaire
// en utilisant les ultra-son.

//
//  Garder le même WP mais partir en marche arrière pendant 5s.
//

class comportement_evitement_t : public comportement_t 
{
  barillet_t barillet;
 public:
  comportement_evitement_t(int _name);
  virtual comportement_result_t  execute(trame_in_t & tin, 
					 internal_state_t & state);
};

#endif
