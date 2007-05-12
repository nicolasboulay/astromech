#ifndef COMPORTEMENT_NAVIGATION_H
#define COMPORTEMENT_NAVIGATION_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>
#include "traitement_cone.h"

//
//  Class de navigation absolue utilisant la camera 360° 
//   - recale également la navigation interne au pic
//   - nice to have: prendre la main pour recaler sur bordure en cas de 
//     impossiblité de faire la triangulation
class comportement_navigation_t : public comportement_t 
{
 public:
  comportement_navigation_t(int _name);
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					 internal_state_t & state);
 private:
  traitement_cone_t traitement_cone;

};

#endif
