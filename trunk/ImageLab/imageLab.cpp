/*-------------------------------------------------------------------------------

  File        : CImg_demo.cpp

  Description : A multi-part demo demonstrating some of the CImg capabilities

  Copyright  : David Tschumperle - http://www.greyc.ensicaen.fr/~dtschump/
  
  This software is governed by the CeCILL-C license under French law and
  abiding by the rules of distribution of free software.  You can  use, 
  modify and/ or redistribute the software under the terms of the CeCILL-C
  license as circulated by CEA, CNRS and INRIA at the following URL
  "http://www.cecill.info". 
  
  As a counterpart to the access to the source code and  rights to copy,
  modify and redistribute granted by the license, users are provided only
  with a limited warranty  and the software's author,  the holder of the
  economic rights,  and the successive licensors  have only  limited
  liability. 
  
  In this respect, the user's attention is drawn to the risks associated
  with loading,  using,  modifying and/or developing or reproducing the
  software by the user in light of its specific status of free software,
  that may mean  that it is complicated to manipulate,  and  that  also
  therefore means  that it is reserved for developers  and  experienced
  professionals having in-depth computer knowledge. Users are therefore
  encouraged to load and test the software's suitability as regards their
  requirements in conditions enabling the security of their systems and/or 
  data to be ensured and,  more generally, to use and operate it in the 
  same conditions as regards security. 
  
  The fact that you are presently reading this means that you have had
  knowledge of the CeCILL-C license and that you accept its terms.
  
  ------------------------------------------------------------------------------*/

#include "CImg.h"
#include <string>
#include <iostream> 
#include <fstream>
#include <sstream>
using namespace cimg_library;
using namespace std;

#define PAS_D_ERREUR 1
#define ERREUR 0
#define NB_COULEURS 4
enum nomCouleur{JAUNE,ROUGE,BLEU,VERT};
struct couleur{
enum nomCouleur nom;
int minHue;
int maxhue;
int minSat;
int maxSat;
int minLight;
int maxLight;
};

int chargerFichierParam(string i_nomFichier,
                        bool & s_modeWebcam,
                        string & s_nomFichierImage,
                        couleur  s_tabCouleur[NB_COULEURS])
{
  int codeRetour = PAS_D_ERREUR;
  ifstream fichierParamIfStream (i_nomFichier.c_str(),ios::in );
  if(!fichierParamIfStream)
  {
    cout <<"le fichier n'existe pas"<<endl;
    codeRetour = ERREUR;
  }
  else
  {
    bool fichierImageLab = false;
    string ligne;
    int indice;
    
    while (!fichierParamIfStream.eof ())
    {
      getline (fichierParamIfStream,ligne);
      cout << ligne <<endl;
      if (ligne.find("ImageLab")!=-1)
      {
        fichierImageLab = true;
	cout <<"ImageLab trouve"<<endl;
      }

      if ((ligne.find("modeWebcam=")!=-1)&&(fichierImageLab))
      {
        indice = ligne.find("oui");
	if (indice == -1)
	{
	  s_modeWebcam = false;
	}
	else
	{
	  s_modeWebcam = true;
	}
      }
      
      if ((ligne.find("nomFichierImage=")!=-1)&&(fichierImageLab))
      {
        indice = ligne.find("=");
	s_nomFichierImage = ligne.substr(indice+1,ligne.size()-indice);
	cout << "image :"<<s_nomFichierImage<<endl;
      }
      
      if ((ligne.find("couleur=")!=-1)&&(fichierImageLab))
      {
        string nomCouleur;
        for(int i=0;i<NB_COULEURS;i++)
	{
	  indice = ligne.find("=");
	  nomCouleur=ligne.substr(indice+1,ligne.size()-indice);
	  cout << nomCouleur <<endl;
	}
      }
    }

    fichierParamIfStream.close ();
  }
  return codeRetour;
}

/*---------------------------

  Main procedure

  --------------------------*/
int main(int argc,char **argv) {
  
  bool modeWebcam;
  string nomFichierImage;
  couleur tabCouleur[NB_COULEURS];

  // Display info about the CImg Library configuration
  //--------------------------------------------------
  cimg_usage("ImageLab.\n\nUsage : imageLap fichierParam");
  cimg::info();
  
  // Chargement fichier param (utilisation de tinyxml)
  string nomFichierParam;
  if (argc < 2)
  {
    cout << "Preciser le fichier de parametre.\n\n"<<endl;
    std::exit(0);
  }
  else
  {
    nomFichierParam = argv[1];
    cout << "Fichier à charger: "<<nomFichierParam<<endl;
    if (chargerFichierParam(nomFichierParam,
                            modeWebcam,
			    nomFichierImage,
			    tabCouleur)==ERREUR)
    {
      cout << "Echec appel chargerFichierParam sur : "<<nomFichierParam<<endl;
    }
  }
  
   
  // Demo selection menu
  //---------------------
  const unsigned char white[3] = {255,255,255}, red[3]    = {120,50,80},
		     yellow[3] = {200,155,0},   green[3]  = {30,200,70};
  float t=0, rx=(float)(2*cimg::crand()), ry=(float)(2*cimg::crand());
  int demo_number,y0=2*13;
  int nbItemMenu = 10;
  bool stopflag = false;
  
  /*back width=1, height=2, depth=1, #channels=3, default value for pixels=10*/
  /*img non initialisee*/
  CImg<unsigned char> back(1,2,1,3,10), img;
  /*le pixel du bas du channel bleu vaut 235*/
  back(0,1,2) = 235;
  /*resize width=320,height=370,depth inchangé, #channels inchangé, type d'interpo: linear*/
  /*va permettre d'obtenir un menu avec un dégradé vertical du noir vers le bleu*/
  /*accès à l'image "bleu"*/
  /*ajout d'un bruit sur le bleu de puissance 10 et type uniforme et ajoute un plasma par defaut*/
  //back.resize(320,370,1,3,3).get_shared_channel(2).noise(10,1).draw_plasma();
  back.resize(280,260,1,3,3);
  /*dessine un rectangle sur toute la largeur de la fenetre d'epaisseur 12 en rouge et une opacite par defaut*/
  back.draw_rectangle(0,y0,back.dimx()-1,y0+12,red).
    draw_text(20,y0,white,0,13,1,
	      "=> CImg %g Menu ImageLab  : \n\n"
	      "- Affiche image naturelle\n"
	      "- Affiche décomposition HSV\n"
	      "- Affiche image binarisée par couleur\n"
	      "- Affiche formes reconnues\n"
	      "- Affiche vue de dessus sans traitement\n"
	      "- Affiche vue de dessus après reconnaissance\n"
	      "- Balance des blancs\n"
	      "- Calibration\n"
	      "- Recharger paramètres\n"
	      "- Sauver paramètres\n",
	      cimg_version);

  /*création d'un display à partir de back, le titre, type de normalization 0, events geres 3, fullscreen false,
  visible = true*/
  CImgDisplay disp(back,"ImageLab",0,3,false,true);
  disp.move((CImgDisplay::screen_dimx()-disp.window_width)/2,
	    (CImgDisplay::screen_dimy()-disp.window_height)/2);
  disp.move(0,0);
  img=back; back*=0.1;
  for(y0+=2*13; !stopflag; ) 
  {
    while(!(disp.button&1) && !stopflag) 
    {
      if (disp.is_closed || disp.key == cimg::keyQ || disp.key==cimg::keyESC) stopflag=true;
      img*=0.9; img+=back;
      int y = disp.mouse_y;
      if (y>=y0 && y<y0+nbItemMenu*13) 
      {
        y = (y/13)*13+7;
        for (int yy=y-7; yy<=y+6; yy++) img.draw_rectangle(0,yy,0,1,img.dimx()-1,yy,0,1,(unsigned char)(130-15*cimg::abs(yy-y)));
        img.draw_triangle(2,y-4,2,y+4,8,y,yellow).draw_triangle(img.dimx()-2,y-4,img.dimx()-2,y+4,img.dimx()-8,y,yellow);
      }
      disp.resize(disp,false).display(img).wait(25);
    }
    disp.button=0;
    if (!stopflag) demo_number = (disp.mouse_y-y0)/13; else break;
    
    // Define colors used to plot the profile, and a hatch to draw the line
      unsigned long hatch=0xF0F0F0F0;
      const unsigned char
     	red  [3] = {255,  0,  0},
     	green[3] = {  0,255,  0},
     	blue [3] = {  0,  0,255},
     	white[3] = {255,255,255},
     	grey [3] = {128,128,128};
    
    switch(demo_number) {
    case 0: 
    {
      // Affichage image naturelle et courbe RGB
      //------------------------
      CImg<unsigned char> image;
      
      if (modeWebcam== false)
      {
        cout << "ouverture " <<nomFichierImage<<endl;
	image = CImg<unsigned char>(nomFichierImage.c_str()).normalize(0,255);
      }
      else
      {
        //capture image webcam
      }
      // Create two display window, one for the image, the other for the color profile.
      CImgDisplay
      main_disp(image,"Color image (Try to move mouse pointer over)",0),
      draw_disp(500,400,"Color profile of the X-axis",0);
      
      
      // Enter event loop. This loop ends when one of the two display window is closed.
      while (!main_disp.is_closed && !draw_disp.is_closed) 
      {
        // Handle display window resizing (if any)
        if (main_disp.is_resized) main_disp.resize().display(image);
        draw_disp.resize();

        if (main_disp.mouse_x>=0 && main_disp.mouse_y>=0) 
	{ // Mouse pointer is over the image     
 	  const int
	    xm = main_disp.mouse_x,			// X-coordinate of the mouse pointer over the image
	    ym = main_disp.mouse_y,			// Y-coordinate of the mouse pointer over the image
	    xl = xm*draw_disp.dimx()/main_disp.dimx(),  // Corresponding X-coordinate of the hatched line
	    x = xm*image.dimx()/main_disp.dimx(),	// Corresponding X-coordinate of the pointed pixel in the image
	    y = ym*image.dimy()/main_disp.dimy();	// Corresponding Y-coordinate of the pointex pixel in the image

 	  // Retrieve color component values at pixel (x,y)
 	  const unsigned int
	    val_red   = image(x,y,0),
	    val_green = image(x,y,1),
	    val_blue  = image(x,y,2);

 	 // Create and display the image of the intensity profile
 	 CImg<unsigned char>(draw_disp.dimx(),draw_disp.dimy(),1,3,0).
 	   draw_axis(0,image.dimx()-1,255,0,grey).
 	   draw_graph(image.get_shared_line(y,0,0),red,0,255,0).
 	   draw_graph(image.get_shared_line(y,0,1),green,0,255,0).
 	   draw_graph(image.get_shared_line(y,0,2),blue,0,255,0).
 	   draw_text(15,5,white,0,11,1,"Pixel (%d,%d)={%d %d %d}",main_disp.mouse_x,main_disp.mouse_y,val_red,val_green,val_blue).
 	   draw_line(xl,0,xl,draw_disp.dimy()-1,white,hatch=cimg::rol(hatch),0.5).
 	   display(draw_disp);
	} 
	else
	{
          // else display a text in the profile display window.
          CImg<unsigned char>(draw_disp.dimx(),draw_disp.dimy()).fill(0).
          draw_text("Mouse pointer is outside the image",draw_disp.dimx()/2-110,draw_disp.dimy()/2-5,white).display(draw_disp);
	}
        // Temporize event loop
        cimg::wait(20);
      }
    } break;
    
    case 1: {
      // Affichage courbe HSV
      //------------------------
      CImg<float> image;
      CImg<float> imageH;
      CImg<float> imageS;
      CImg<float> imageV;
      
      if (modeWebcam== false)
      {
        cout << "ouverture " <<nomFichierImage<<endl;
	image = CImg<unsigned char>(nomFichierImage.c_str());
      }
      else
      {
        //capture image webcam
      }
      
      image.RGBtoHSV();
      /*image contient maintenant H [0,2*PI],S[0,1], V[0,1]*/
      /* on va maintenant normaliser chaque composante avant affichage*/
      imageH = image.get_channel(0);
      imageS = image.get_channel(1);
      imageV = image.get_channel(2);
      
      const CImgStats statH(imageH,false);
      const CImgStats statS(imageS,false);
      const CImgStats statV(imageV,false);
      
      /*statH.print("Hue");
      statS.print("Sat");
      statV.print("V");*/
      
      imageH.normalize(statH.min*255/(2*cimg::PI),statH.max*255/(2*cimg::PI));
      imageS.normalize(statS.min*255,statS.max*255);
      imageV.normalize(statV.min*255,statV.max*255);
      /*les 3 images contiennent maintenant H [0,255],S[0,255], V[0,255]*/
      
      // Create two display window, one for the image, the other for the color profile.
      CImgDisplay
      main_dispH(imageH,"Hue image",0),
      main_dispS(imageS,"Saturation image",0),
      main_dispV(imageV,"Value image",0),
      draw_disp(400,300,"Color profile of the X-axis",0);
    
      int xm,ym,xl,x,y;
      bool pointerOnImage=false;
      
      // Enter event loop. This loop ends when one of the two display window is closed.
      while (!main_dispH.is_closed &&
     	     !main_dispS.is_closed &&
     	     !main_dispV.is_closed &&
     	     !draw_disp.is_closed)
      {

     	// Handle display window resizing (if any)
     	if (main_dispH.is_resized) main_dispH.resize().display(imageH);
     	if (main_dispS.is_resized) main_dispS.resize().display(imageS);
     	if (main_dispV.is_resized) main_dispV.resize().display(imageV);
     	draw_disp.resize();

        pointerOnImage = false;

     	if (main_dispH.mouse_x>=0 && main_dispH.mouse_y>=0)
     	{ // Mouse pointer is over the image Hue
     	  xm = main_dispH.mouse_x,		     // X-coordinate of the mouse pointer over the image
     	  ym = main_dispH.mouse_y,		     // Y-coordinate of the mouse pointer over the image
     	  xl = xm*draw_disp.dimx()/main_dispH.dimx(),  // Corresponding X-coordinate of the hatched line
     	  x = xm*imageH.dimx()/main_dispH.dimx(),     // Corresponding X-coordinate of the pointed pixel in the image
     	  y = ym*imageH.dimy()/main_dispH.dimy();     // Corresponding Y-coordinate of the pointex pixel in the image
	  pointerOnImage = true;
     	}
     	else if (main_dispS.mouse_x>=0 && main_dispS.mouse_y>=0)
     	{
     	  xm = main_dispS.mouse_x,		     // X-coordinate of the mouse pointer over the image
     	  ym = main_dispS.mouse_y,		     // Y-coordinate of the mouse pointer over the image
     	  xl = xm*draw_disp.dimx()/main_dispS.dimx(),  // Corresponding X-coordinate of the hatched line
     	  x = xm*imageS.dimx()/main_dispS.dimx(),     // Corresponding X-coordinate of the pointed pixel in the image
     	  y = ym*imageS.dimy()/main_dispS.dimy();     // Corresponding Y-coordinate of the pointex pixel in the image
	  pointerOnImage = true;
        }
        else if (main_dispV.mouse_x>=0 && main_dispV.mouse_y>=0)
        {
          xm = main_dispV.mouse_x,		     // X-coordinate of the mouse pointer over the image
          ym = main_dispV.mouse_y,		     // Y-coordinate of the mouse pointer over the image
          xl = xm*draw_disp.dimx()/main_dispV.dimx(),  // Corresponding X-coordinate of the hatched line
          x = xm*imageV.dimx()/main_dispV.dimx(),     // Corresponding X-coordinate of the pointed pixel in the image
          y = ym*imageV.dimy()/main_dispV.dimy();     // Corresponding Y-coordinate of the pointex pixel in the image
	  pointerOnImage = true;
        }
        else
        {
          // else display a text in the profile display window.
          CImg<unsigned char>(draw_disp.dimx(),draw_disp.dimy()).fill(0).
            draw_text("Mouse pointer is outside the image",draw_disp.dimx()/2-110,draw_disp.dimy()/2-5,white).display(draw_disp);
        }

        if (pointerOnImage)
	{
          // Retrieve color component values at pixel (x,y)
          const unsigned int
	    val_Hue = (int)imageH(x,y,0),
	    val_Sat = (int)imageS(x,y,0),
	    val_Val = (int)imageV(x,y,0);

          // Create and display the image of the intensity profile
          CImg<unsigned char>(draw_disp.dimx(),draw_disp.dimy(),1,3,0).
          draw_axis(0,imageH.dimx()-1,255,0,grey).
          draw_graph(imageH.get_shared_line(y,0,0),red,0,255,0).
          draw_graph(imageS.get_shared_line(y,0,0),green,0,255,0).
          draw_graph(imageV.get_shared_line(y,0,0),blue,0,255,0).
          draw_text(15,5,white,0,11,1,"Pixel (%d,%d):",x,y).
	  draw_text(15,13,red,0,11,1,"Hue: %d",val_Hue).
	  draw_text(15,21,green,0,11,1,"Sat: %d",val_Sat).
	  draw_text(15,29,blue,0,11,1,"Val: %d",val_Val).
          draw_line(xl,0,xl,draw_disp.dimy()-1,white,hatch=cimg::rol(hatch),0.5).
          display(draw_disp);
        }

      // Temporize event loop
      cimg::wait(20);
     } 
    } break;

    case 2: {

      
    } break;

    case 3: {

      
    } break;

    case 4: {

      
    } break;
    
    case 5: {


    } break;

    case 6: {

      
      
    } break;

    case 7: {

      
    } break;

    case 8: {

      
    } break;
    
    case 9: {

      
    } break;

    case 10: {

    
    } break;
    }
  }
    
  // Exit demo menu
  //----------------
  fprintf(stderr,"Exit ImageLab\n");
  return 0;
}
