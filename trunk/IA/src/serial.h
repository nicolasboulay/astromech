#ifndef SERIAL_H
#define SERIAL_H
#include <QVector>
#include <termios.h>

#include "trame_binary.h"
#include "common.h"

class serial_t {
  int error;
  public :  
  serial_t(const char * device);
  ~serial_t();
  void exchange(QVector<unsigned char> & in, trame_binary_t & out);
 private:
  void find_trame(unsigned previous_bank);
  int fdr;
  int fdw;  
  struct termios oldtio,newtio;
  unsigned char buffer[2][TAILLE_TRAME];
  unsigned bank;
  unsigned index;
  QVector<unsigned char> frame;
};

#endif
