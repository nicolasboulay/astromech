#include<trame_binary.h>

int trame_binary_t::give_pic1_offset()
{
  return 4;
}

int trame_binary_t::give_pic1_size()
{
  return this->at(give_pic1_offset()+1);
}

int trame_binary_t::give_pic2_offset()
{
  int pic1_offset = give_pic1_offset();
  int pic1_size   = give_pic1_size();

  return pic1_offset + pic1_size;
}

int trame_binary_t::give_pic2_size()
{
  return this->at(give_pic2_offset()+1);
}

int trame_binary_t::give_pic3_offset()
{
  int pic2_offset = give_pic2_offset();
  int pic2_size   = give_pic2_size();

  return pic2_offset + pic2_size;
}

int trame_binary_t::give_pic3_size()
{
  return this->at(give_pic3_offset()+1);
}


int trame_binary_t::validation_trame()
{
  unsigned xor1=0;
  unsigned xor2=0;

  unsigned magic= 
    (this->at(0) << 3*8) |
    (this->at(1) << 2*8) |
    (this->at(2) << 1*8) |
    (this->at(3));

  if(magic!=0x1ACFFC1d)
    return -1;
  TRACE;
  //XOR trame
  for(unsigned i=0; i < TAILLE_TRAME-2;i+=2){
    xor1^=this->at(i);
    xor2^=this->at(i+1);
  }
  printf("xor1=%x xor2=%x [-2]=%x [-1]=%x", xor1,xor2,
	 this->at(TAILLE_TRAME-2), this->at(TAILLE_TRAME-1));
  if((xor1!=this->at(TAILLE_TRAME-2))||
     (xor2!=this->at(TAILLE_TRAME-1)))
    return -1;
  TRACE;
  return 0;
}
int trame_binary_t::validation_paquet(unsigned h, unsigned offset, unsigned size)
{
  unsigned add=0;

  if(this->at(offset)!=h) 
    return -1;

  //add paquet
  for(unsigned i=offset; i < (offset+size-2);i+=2){
    add+=(this->at(i) << 8) | this->at(i+1);
  }
  printf("validation paquet add=%x add1=%x add2=%x [-2]=%x [-1]=%x size=%i offset=%i\n", 
	add,((add >> 8)& 0xff), ((add & 0xff)),this->at(offset+ size-2), this->at(offset+ size-1),
	 size, offset);
  if((((add >> 8)& 0xff )!=this->at(offset+ size-2))||
     ((add & 0xff)!=this->at(offset+ size-1)))
    return -1;

  return 0;
}

int trame_binary_t::validation_paquet_pic1()
{
  return validation_paquet(0x0,give_pic1_offset(),give_pic1_size());
}

int trame_binary_t::validation_paquet_pic2()
{
  return validation_paquet(0x2,give_pic2_offset(),give_pic2_size());
}

int trame_binary_t::validation_paquet_pic3()
{
  return validation_paquet(0x4,give_pic3_offset(),give_pic3_size());
}

int trame_binary_t::validation_paquet_pc_pic1()
{
  return validation_paquet(0x0,4,TAILLE_PAQUET_PC_PIC_1);
}


int trame_binary_t::validation()
{
  if(validation_trame() && 
     validation_paquet_pic1() && 
     validation_paquet_pic2() && 
     validation_paquet_pic3())
    return -1;
  
  return 0;
}

void trame_binary_t::clear()
{
  (*this)[0]=0x1a;
  (*this)[1]=0xcf;
  (*this)[2]=0xfc;
  (*this)[3]=0x1d;

  (*this)[TAILLE_TRAME-2]=0x34;
  (*this)[TAILLE_TRAME-1]=0x35;
}

void trame_binary_t::test()
{

  for(int i=0;i<TAILLE_TRAME;i++){
    (*this)[i]=':';
  }

  (*this)[0]='B';
  (*this)[1]='U';
  (*this)[2]='F';
  (*this)[3]=' ';

  (*this)[4]='\r';
  (*this)[5]='\n';

  (*this)[TAILLE_TRAME-5]='E';
  (*this)[TAILLE_TRAME-4]='N';
  (*this)[TAILLE_TRAME-3]='D';
  (*this)[TAILLE_TRAME-2]='\r';
  (*this)[TAILLE_TRAME-1]='\n';
}

void trame_binary_t::gen_valid_trame()
{
  unsigned xor1=0;
  unsigned xor2=0;
  (*this)[0]=0x1a;
  (*this)[1]=0xcf;
  (*this)[2]=0xfc;
  (*this)[3]=0x1d;

  for(unsigned i=0; i < TAILLE_TRAME-2;i+=2){
    xor1^=this->at(i);
    xor2^=this->at(i+1);
  }

  (*this)[TAILLE_TRAME-2]=xor1;
  (*this)[TAILLE_TRAME-1]=xor2;

  
}

void trame_binary_t::gen_valid_paquet(unsigned h, unsigned offset, unsigned size)
{
  unsigned add=0;

  (*this)[offset]=h;
  (*this)[offset+1]=size;
  (*this)[offset+2]=num_trame;

  for(unsigned i=offset; i < (offset+size-2);i+=2){
    add+=(this->at(i) << 8) | this->at(i+1);
  }
  (*this)[offset+size-2]= (add >> 8)& 0xff;
  (*this)[offset+size-1]= (add & 0xff);
}

void trame_binary_t::gen_valid_pic_pc_paquet()
{
  gen_valid_paquet(0,4,TAILLE_PAQUET_PIC_PC_1);
  gen_valid_paquet(2,4+TAILLE_PAQUET_PIC_PC_1,TAILLE_PAQUET_PIC_PC_2);
  gen_valid_paquet(4,4+TAILLE_PAQUET_PIC_PC_1+TAILLE_PAQUET_PIC_PC_2,
		   TAILLE_PAQUET_PIC_PC_1);
}

void trame_binary_t::gen_valid_pc_pic_paquet()
{
  gen_valid_paquet(0,4,TAILLE_PAQUET_PC_PIC_1);
  gen_valid_paquet(2,4+TAILLE_PAQUET_PC_PIC_1,TAILLE_PAQUET_PC_PIC_2);
  gen_valid_paquet(4,4+TAILLE_PAQUET_PC_PIC_1+TAILLE_PAQUET_PC_PIC_2,
		   TAILLE_PAQUET_PC_PIC_1);
}

void trame_binary_t::print()
{
  for(int i=0;i<this->count();i++){
    printf("%.2x ", (*this)[i]);
  }
  printf("\n");
}

void trame_binary_t::print_from_pic()
{
  int i,prev;
  for(i=0;i<4;i++){
    printf("%.2x ", (*this)[i]);
  }
  printf("\n");
  prev=i;
  for(;i<TAILLE_PAQUET_PIC_PC_1+prev;i++){
    printf("%.2x ", (*this)[i]);
  }
  printf("\n");
  prev=i;
  for(;i<TAILLE_PAQUET_PIC_PC_2+prev;i++){
    printf("%.2x ", (*this)[i]);
  }
  printf("\n");
  prev=i;
  for(;i<TAILLE_PAQUET_PIC_PC_3+prev;i++){
    printf("%.2x ", (*this)[i]);
  }
  printf("\n");
  prev=i;
  for(;i<2+prev;i++){
    printf("%.2x ", (*this)[i]);
  }
  printf("\n");



}
