#ifndef COMPORTEMENT_EYE_H
#define COMPORTEMENT_EYE_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement_async.h"
#include "comportement.h"
#include "QObject"

//
//  Class eye pour les traitements de la camera avant
//   - trouver le panier au début de match
//   - trouver une cible en cours de match
class comportement_eye_t : public comportement_async_t 
{ 
  Q_OBJECT 
 public:
  comportement_eye_t(int _name);
/*   virtual comportement_result_t  execute(const trame_in_t & tin,  */
/* 					 internal_state_t & state); */
 private:
  
  void run();
};

#endif
