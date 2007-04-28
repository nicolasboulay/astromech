#ifndef COMMON_H
#define COMMON_H

#define TAILLE_TRAME 128
#define TAILLE_PAQUET_PIC_PC_1 26
#define TAILLE_PAQUET_PIC_PC_2 62
#define TAILLE_PAQUET_PIC_PC_3 34

#define TAILLE_PAQUET_PC_PIC_1 28
#define TAILLE_PAQUET_PC_PIC_2 58
#define TAILLE_PAQUET_PC_PIC_3 36



#define TRACE do{ printf("%s: %s:%i\n",__FILE__,__FUNCTION__, __LINE__); }while(0)

#endif
