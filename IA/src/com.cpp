#include"com.h"

int  com_t::exchange(trame_in_t & in, trame_out_t & out)
{
 
  out.serialise(bin_out);
  bin_out.gen_valid_pc_pic_paquet();
  bin_out.gen_valid_trame();
  serial.exchange(bin_in,bin_out);
  if(bin_in.validation()){
    return -1;
  }
  in.deserialise(bin_in);
  
  return 0;
}
