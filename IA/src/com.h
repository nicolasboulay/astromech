#ifndef COM_H
#define COM_H
#include <QVector>
#include <termios.h>

#include "trame_binary.h"
#include "common.h"

class com_t {
  public : 
    com_t(const char * device) : serial(device) {}

    int exchange(trame_in_t & in, trame_out_t & out);
  private :
    serial_t serial;
  trame_binary_t bin_in;
  trame_binary_t bin_out;
};

#endif
