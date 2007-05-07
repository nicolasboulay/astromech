#ifndef TASK_RT_H
#define TASK_RT_H
#include <QThread>
#include <QVector>
#include <QObject>
#include <string.h>
#include "trame_out.h"
#include "trame_in.h"
#include "internal_state.h"

// Main task.
// This run in a thread to leave the main thread for gui
//  

class task_rt_t : public QThread
{    
  Q_OBJECT

public:
  void run();
  void run2();
  task_rt_t(const char* _device) {strcpy(device,_device);}

signals:
  void newTrameOut(QVector<unsigned char> tab);
  void newTrameIn(QVector<unsigned char> tab);
  void newTreeOut(const trame_out_t & out);
  void newTreeIn(const trame_in_t & in);
  void newTreeState(const internal_state_t & state);

  void newTrameC(int i, int j,int val);
private:
  char device[32];
  int j;

};

#endif
