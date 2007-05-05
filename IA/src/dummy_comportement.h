#ifndef DUMMY_COMPORTEMENT_H
#define DUMMY_COMPORTEMENT_H
#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>


// class de test
class dummy_comportement_t : public comportement_t {
 public:
  dummy_comportement_t(int _name) : comportement_t(_name) {
    QTextStream qout(stdout);
      output.name("dummy_comportement");
      qout << "init dummy_comportement_t:" << _name << endl;
  }
  virtual comportement_result_t  execute(trame_in_t tin, 
					  internal_state_t state) 
    {
      QTextStream qout(stdout);
      for(int n=0;n<REFEREE_NUMBER;n++){
	output.isActive[n]=false;
      }

      return output;
    };
};

#endif
