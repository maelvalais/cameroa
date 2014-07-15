/* HEADER DU MODULE DU WIDGET DE LA ZONE POSE CENTRAGE

   (C)David.Romeuf@univ-lyon1.fr 05/2006
*/

#ifndef _WIDGETPOSECENTRAGE_
#define _WIDGETPOSECENTRAGE_

// Inclusions C++
//
#include <iostream>
#include <valarray>

// Inclusions Qt
//
#include <qcolor.h>
#include <qdatetime.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qstring.h>
#include <qwidget.h>

// Inclusion CamerOA
//
#include "WidgetPoseImage.h"


#define TAILLE_X_OBJPOSECENTRAGE_RECOMMANDEE	512	// Taille recommandee en x de l'objet
#define TAILLE_Y_OBJPOSECENTRAGE_RECOMMANDEE	512	// Taille recommandee en y de l'objet


// Widget de zone d'affichage graphique d'une pose centrage
//
class ObjPoseCentrage : public ObjPoseImage
{
	Q_OBJECT

// Zone inaccessible sauf par l'objet lui meme
private :

// Zone protegee de l'exterieur sauf des classes qui heritent
protected:
	// Proprietes
	//
	std::valarray<long> DonneesICH;			// Donnees de l'image centrage horizontal dans un tableau de valeur de la STL C++
	QDateTime DateHeureDebutPoseICH;		// Date et heure de debut de la pose de centrage horizontal
	QDateTime DateHeureFinPoseICH;			// Date et heure de fin de la pose de centrage horizontal
	int DureePoseICH;						// Duree de la pose de centrage horizontal en 1/10000 s
	long DimXICH;							// Dimension en x de l'image centrage horizontal
	long DimYICH;							// Dimension en y de l'image centrage horizontal
	long ValMinICH;							// Valeur minimale des donnees de l'image centrage horizontal
	long ValMaxICH;							// Valeur maximale des donnees de l'image centrage horizontal
	double lpdacpICH;						// Largeur pourcentage profil details amplifies au centre du profil horizontal
	long DimDH;								// Dimension typique des details a amplifier en unite d'element du tableau
	double adcpICH;							// Amplification details au centre du profil horizontal

	std::valarray<long> DonneesICV;			// Donnees de l'image centrage vertical dans un tableau de valeur de la STL C++
	QDateTime DateHeureDebutPoseICV;		// Date et heure de debut de la pose de centrage vertical
	QDateTime DateHeureFinPoseICV;			// Date et heure de fin de la pose de centrage vertical
	int DureePoseICV;						// Duree de la pose de centrage vertical en 1/10000 s
	long DimXICV;							// Dimension en x de l'image centrage vertical
	long DimYICV;							// Dimension en y de l'image centrage vertical
	long ValMinICV;							// Valeur minimale des donnees de l'image centrage vertical
	long ValMaxICV;							// Valeur maximale des donnees de l'image centrage vertical
	double lpdacpICV;						// Largeur pourcentage profil details amplifies au centre du profil vertical
	long DimDV;								// Dimension typique des details a amplifier en unite d'element du tableau
	double adcpICV;							// Amplification details au centre du profil vertical
	
	// Methodes
	void paintEvent(QPaintEvent *event);		// Surcharge du slot herite de ObjPoseImage
	QSize sizeHint();							// Surcharge de la fonction retournant la dimension recommandee pour le widget

// Zone accessible par l'exterieur
public:
	// Constructeur de l'objet (jamais de type de retour)
	//
	ObjPoseCentrage(QWidget *parent=0,const char *name=0,WFlags f=0);
	
	// Destructeur de l'objet (jamais de type de retour et d'argument)
	~ObjPoseCentrage();

	void ForceICH_NULL(void);	// Force l'image centrage horizontal a l'etat NULL
	void ForceICV_NULL(void);	// Force l'image centrage vertical a l'etat NULL

	// Fonction de chargement d'une image centrage horizontal dans l'objet
	//
	void ChargerImageH(unsigned short tx,unsigned short ty,unsigned short *pt,QDateTime debut,QDateTime fin,int duree);
	
	// Fonction de chargement d'une image centrage vertical dans l'objet
	//
	void ChargerImageV(unsigned short tx,unsigned short ty,unsigned short *pt,QDateTime debut,QDateTime fin,int duree);
	
	// Fonction de representation graphique de la zone de centrage en profil
	//
	void RepresenterZoneCentrage(void);
	
	// Fonction de parametrage de l'amplification au centre du profil horizontal
	//
	void ParamAmplificationProfilH(long dimension,double largeur,double facteur);
	
	// Fonction de parametrage de l'amplification au centre du profil vertical
	//
	void ParamAmplificationProfilV(long dimension, double largeur,double facteur);
};

#endif
