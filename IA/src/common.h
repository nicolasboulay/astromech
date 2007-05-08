#ifndef COMMON_H
#define COMMON_H

//pic frame
#define TAILLE_TRAME 128
#define TAILLE_PAQUET_PIC_PC_1 20
#define TAILLE_PAQUET_PIC_PC_2 70
#define TAILLE_PAQUET_PIC_PC_3 32

#define TAILLE_PAQUET_PC_PIC_1 20
#define TAILLE_PAQUET_PC_PIC_2 76
#define TAILLE_PAQUET_PC_PIC_3 26


// Comportement nick name
#define COMPO_DUMMY1  0
#define COMPO_DEFAUT  1
#define COMPO_TEST    2
#define COMPO_ENDOFMATCH    3
#define COMPO_NAVIGATION    4
#define COMPO_NUMBER  5

// Referee
#define REFEREE_DEPLACEMENT 0
#define REFEREE_TOOLS       1
#define REFEREE_DISPLAY     2 
#define REFEREE_NAVIGATION  3 //recenter the pic navigation
#define REFEREE_GESTION     4
#define REFEREE_NUMBER      5

#define TRACE do{ printf("%s: %s:%i\n",__FILE__,__FUNCTION__, __LINE__); }while(0)

#endif
