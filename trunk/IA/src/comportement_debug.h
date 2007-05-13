#ifndef COMPORTEMENT_DEBUG_H
#define COMPORTEMENT_DEBUG_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>

//
//  Comportement de debug
//   - fonctionnelle _uniquement_ avec le jack de départ
//   - transmet l'action sur les boutons vers des actions à définir
//   - permet la gestion de la "télécommande"
//
class comportement_debug_t : public comportement_t 
{
 public:
  comportement_debug_t(int _name);
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					 internal_state_t & state);
};

#endif
