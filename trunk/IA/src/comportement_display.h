#ifndef COMPORTEMENT_DISPLAY_H
#define COMPORTEMENT_DISPLAY_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>

//
//  Comportement de display
//   - permet d'afficher l'état du robot sur les diodes externes
//   - ne doit affecter que REFEREE_DISPLAY
//
class comportement_display_t : public comportement_t 
{
 public:
  comportement_display_t(int _name);
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					 internal_state_t & state);
};

#endif
