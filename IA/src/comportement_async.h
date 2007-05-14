#ifndef COMPORTEMENT_ASYNC_H
#define COMPORTEMENT_ASYNC_H

#include "trame_in.h"
#include "trame_out.h"
#include "internal_state.h"
#include "comportement.h"

#include <QVector>
#include <QThread>
#include <QObject>
#include <QMutex>

// comportement asynchrone
// - task_rt copy the input trame 
// -  task_async copy the previous copy 


class comportement_async_t : public comportement_t, public QThread 
{
    Q_OBJECT

 public:
  comportement_async_t(int _name) : comportement_t(_name), first_time(true) { }
  virtual comportement_result_t  execute(const trame_in_t & tin, 
					 internal_state_t & state);
  virtual ~comportement_async_t() {}
 protected:  
  QMutex mutex;
  trame_in_t tin_ext;
  internal_state_t state_ext;
  comportement_result_t result_ext;
  bool first_time;
};

#endif
