#ifndef COMPORTEMENT_THREAD_H
#define COMPORTEMENT_THREAD_H

#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"
#include <QVector>
#include <QThread>
#include <QObject>

// comportement asynchrone
class comportement_thread_t : public comportement_t, public QThread 
{
    Q_OBJECT

 public:
  comportement_thread_t(int _name) : comportement_t(_name) { }
  virtual comportement_result_t & execute(trame_in_t tin, 
					  internal_state_t state) {return output;};
};

#endif
