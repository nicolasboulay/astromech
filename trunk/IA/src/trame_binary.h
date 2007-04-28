#ifndef TRAME_BINARY_H
#define TRAME_BINARY_H
#include <QVector>
#include "common.h"

// Representation binaire d'une trame

class trame_binary_t : public QVector<unsigned char>
{
  public :
  trame_binary_t(unsigned n=TAILLE_TRAME) : QVector<unsigned char>(n,0),num_trame(0) {};
  int validation();
  void clear();
  void test();
  void gen_valid_trame();
  void gen_valid_pic_pc_paquet();
  void gen_valid_pc_pic_paquet();
  void print();
  void print_from_pic();
  int validation_trame();

  int validation_paquet_pc_pic1();
  void inc_num_trame() {num_trame++;};
 private:

  int give_pic1_offset();
  int give_pic1_size();
  int give_pic2_offset();
  int give_pic2_size();
  int give_pic3_offset();
  int give_pic3_size();

  int validation_paquet(unsigned h, unsigned offset, unsigned size);
  int validation_paquet_pic1();
  int validation_paquet_pic2();
  int validation_paquet_pic3();
  void gen_valid_paquet(unsigned h, unsigned offset, unsigned size);

  unsigned num_trame;
};
#endif
