/* MODULE DU WIDGET DE LA ZONE POSE CENTRAGE

   (C)David.Romeuf@univ-lyon1.fr 05/2006
*/

// Inclusions C++
//
#include <iostream>
#include <new>
#include <memory>
#include <valarray>
#include <cmath>

using namespace std;

// Inclusions Qt et KDE
//
#include <qfontmetrics.h>
#include <qsize.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qbrush.h>
#include <qwmatrix.h>

// Inclusions module
//
#include "WidgetPoseImage.h"
#include "WidgetPoseCentrage.h"


// Constructeur de la classe ObjPoseCentrage
//
ObjPoseCentrage::ObjPoseCentrage(QWidget *parent,const char *name,WFlags f) : ObjPoseImage(parent,name,f)
{
	// Couleur de fond du widget
	//
	setPaletteBackgroundColor(QColor(Qt::black));
	
	ForceICH_NULL();
	ForceICV_NULL();
	
	lpdacpICH=0.0;
	DimDH=10;
	adcpICH=0.0;
	
	lpdacpICV=0.0;
	DimDV=10;
	adcpICV=0.0;
}


// Destructeur de la classe ObjPoseCentrage
//
ObjPoseCentrage::~ObjPoseCentrage()
{
}


// Force l'image centrage horizontal a devenir NULL
//
void ObjPoseCentrage::ForceICH_NULL(void)
{
	DonneesICH.resize(0);
	DimXICH=0;
	DimYICH=0;
	ValMinICH=0;
	ValMaxICH=0;
}


// Force l'image centrage vertical a devenir NULL
//
void ObjPoseCentrage::ForceICV_NULL(void)
{
	DonneesICV.resize(0);
	DimXICV=0;
	DimYICV=0;
	ValMinICV=0;
	ValMaxICV=0;
}


// Fonction de parametrage de l'amplification au centre du profil horizontal
//
// CE:	On passe la dimension de la matrice de convolution (signal carre) en pixels ;
//
//		On passe la largeur de la zone amplifiee en pourcentage de la largeur totale du profil ;
//
//		On passe le facteur d'amplification des details.
//
// CS:	-
//
void ObjPoseCentrage::ParamAmplificationProfilH(long dimension,double largeur,double facteur)
{
	lpdacpICH=largeur;
	DimDH=dimension;
	adcpICH=facteur;
}


// Fonction de parametrage de l'amplification au centre du profil vertical
//
// CE:	On passe la dimension de la matrice de convolution (signal carre) en pixels ;
//
//		On passe la largeur de la zone amplifiee en pourcentage de la largeur totale du profil ;
//
//		On passe le facteur d'amplification des details.
//
// CS:	-
//
void ObjPoseCentrage::ParamAmplificationProfilV(long dimension,double largeur,double facteur)
{
	lpdacpICV=largeur;
	DimDV=dimension;
	adcpICV=facteur;
}


// Fonction de chargement d'une image centrage horizontal dans l'objet
//
// CE:	On passe la dimension de l'image en abscisse (nombre de colonnes) ;
//
//	On passe la dimension de l'image en ordonnee (nombre de lignes) ;
//
//	On passe un pointeur sur unsigned long vers la source des donnees ;
//
// CS:	-
//
void ObjPoseCentrage::ChargerImageH(unsigned short tx,unsigned short ty,unsigned short *pt,QDateTime debut,QDateTime fin,int duree)
{
	// Dimension de l'image chargee
	//
	DimXICH=tx;
	DimYICH=ty;

	// Chargement des pixels dans le tableau de valeur
	//
	unsigned long taille=tx;
	taille*=ty;

	DonneesICH.resize(taille);	// Redimensionnement du tableau de valeurs si besoin

	long i=0;

	while( taille-- )
	{
		DonneesICH[i++]=(long) *pt;

		pt++;
	}
	
	// Recuperation des valeurs extremes
	//
	ValMinICH=DonneesICH.min();
	ValMaxICH=DonneesICH.max();
//std::cout << "ICH: " << ValMinICH << "," << ValMaxICH << std::endl;

	DateHeureDebutPose=debut;
	DateHeureFinPose=fin;
	DureePose=duree;
}


// Fonction de chargement d'une image centrage vertical dans l'objet
//
// CE:	On passe la dimension de l'image en abscisse (nombre de colonnes) ;
//
//	On passe la dimension de l'image en ordonnee (nombre de lignes) ;
//
//	On passe un pointeur sur unsigned long vers la source des donnees ;
//
// CS:	-
//
void ObjPoseCentrage::ChargerImageV(unsigned short tx,unsigned short ty,unsigned short *pt,QDateTime debut,QDateTime fin,int duree)
{
	// Dimension de l'image chargee
	//
	DimXICV=tx;
	DimYICV=ty;

	// Chargement des pixels dans le tableau de valeur
	//
	unsigned long taille=tx;
	taille*=ty;

	DonneesICV.resize(taille);	// Redimensionnement du tableau de valeurs si besoin

	long i=0;

	while( taille-- )
	{
		DonneesICV[i++]=(long) *pt;

		pt++;
	} 

	// Recuperation des valeurs extremes
	//
	ValMinICV=DonneesICV.min();
	ValMaxICV=DonneesICV.max();
//std::cout << "ICV: " << ValMinICV << "," << ValMaxICV << std::endl;

	DateHeureDebutPose=debut;
	DateHeureFinPose=fin;
	DureePose=duree;
}


// Surcharge de la fonction de slot paintEvent heritee du QWidget
//
void ObjPoseCentrage::paintEvent(QPaintEvent *event)
{
	QPaintEvent *ev;	// Pour eviter un Warning a la compilation
	ev=event;

	// Representation en imagerie
	//
	if( DonneesIC.size() != 0 )
	{
		RepresenterZoneImage();
	}
	else
	{
		long largeur_zone=size().width();	// Largeur du widget de la zone image
		long hauteur_zone=size().height();	// Hauteur du widget de la zone image
		
		// L'objet de dessin va directement dessiner sur l'objet qui herite de QWidget et donc de QPaintDevice
		//
		QPainter Dessin(this);

		QFontMetrics InfosFontCourante=Dessin.fontMetrics();
		QPen PinceauBlanc(Qt::white);
		Dessin.setPen(PinceauBlanc);

		Dessin.eraseRect(0,0,largeur_zone,hauteur_zone);

		QString Chaine="aucune image disponible";

		Dessin.drawText(largeur_zone/2-InfosFontCourante.width(Chaine)/2,hauteur_zone/2-InfosFontCourante.height()/2,Chaine,-1,QPainter::LTR);

		Dessin.end();
	}

	// Representation des profils
	//
	if( DonneesICH.size() != 0 || DonneesICV.size() != 0 ) RepresenterZoneCentrage();
}


// Surcharge de la fonction retournant la dimension recommandee du widget
//
QSize ObjPoseCentrage::sizeHint()
{
	QSize dimension_recommandee(TAILLE_X_OBJPOSECENTRAGE_RECOMMANDEE,TAILLE_Y_OBJPOSECENTRAGE_RECOMMANDEE);
	
	return dimension_recommandee;
}


// Fonction de representation graphique de la zone Centrage
//
//	CE:	-
//
//	CS:	-
//
void ObjPoseCentrage::RepresenterZoneCentrage(void)
{
	int x;					// Variables de travail
	int y;
	QPainter Dessin;			// Un objet de dessin
	QPen PinceauJaune(Qt::yellow);		// Un pinceau jaune pour le profil horizontal
	QPen PinceauCyan(Qt::cyan);		// Un pinceau cyan pour le profil vertical

	// L'objet de dessin va directement dessiner sur ObjPoseCentrage qui herite de QWidget et donc de QPaintDevice
	//
	Dessin.begin(this);

	PinceauJaune.setStyle(Qt::SolidLine);
	PinceauJaune.setWidth(0);
	PinceauCyan.setStyle(Qt::SolidLine);
	PinceauCyan.setWidth(0);

	// Si on peut, on trace le profil de la pose de centrage horizontal
	//
	if( DonneesICH.size() != 0 && DimXICH != 0 && DimYICH != 0 )
	{
		int xx=-1;				// Coordonnees pour tracer le profil
		int yy=-1;
		int axx=-1;				// Anciennes coordonnees pour tracer le profil
		int ayy=-1;
		std::valarray<long> Valeurs;		// Valeurs du profil par colonne
		long ValeurMin;				// Valeur mini du profil integre
		long ValeurMax;				// Valeur maxi du profil integre
		double coefx;				// Coefficient de transformation sur l'axe des colonnes
		double coefp;				// Coefficient de transformation sur l'axe du profil

		// On commence par ajouter les valeurs des colonnes
		//
		Valeurs.resize(DimXICH);
		
		Valeurs=0;
		
		for( x=0; x < DimXICH; x++ )
		{
			for( y=0; y < DimYICH; y++ )
			{
				Valeurs[x]+=DonneesICH[x+y*DimXICH];
			}
		}
	
		// Si on doit amplifier une partie du centre du profil
		//
		if( lpdacpICH != 0.0 )
		{
			long i;				// Variable index travail
			std::valarray<long> pmoyen;	// Profil moyen sur 2x DimDH valeurs
			std::valarray<long> mdetails;	// Masque des details
			
			pmoyen.resize(DimXICH);
			mdetails.resize(DimXICH);
			
			pmoyen=0;
			mdetails=0;
		
			// On calcule le profil moyen sur un filtre carre de DimDH membres
			//
			for( i=0; i < DimXICH; i++ )
			{
				long indice;		// Indice dans le tableau
				long nbc=1;		// Nombre de cellules ajoutees
				
				pmoyen[i]=Valeurs[i];
				
				for( long j=1; j < DimDH; j++ )
				{
					indice=i-j;
					while( indice < 0 ) indice+=DimXICH;
					
					pmoyen[i]+=Valeurs[indice];
					
					indice=i+j;
					while( indice >= DimXICH ) indice-=DimXICH;
					
					pmoyen[i]+=Valeurs[indice];
					
					nbc+=2;
				}
				
				pmoyen[i]/=nbc;
			}
			
			// On calcule le masque des details
			//
			mdetails=Valeurs-pmoyen;
			
			// On amplifie les details dans la zone parametree
			//
			double bornei=50.0-lpdacpICH/2.0;
			double bornes=bornei+lpdacpICH;
			
			for( i=0; i < DimXICH; i++ )
			{
				// On passe i en pourcentage du nombre de membres
				//
				double pourc_i=(double) i;
				pourc_i/=(double) (DimXICH-1);
				pourc_i*=100.0;
			
				// Si on doit amplifier cette zone courante
				//
				if( pourc_i >= bornei && pourc_i <= bornes )
				{
					double enplus=((double) mdetails[i])*adcpICH;	// Valeur ajoute en plus
					
					Valeurs[i]+=(long) enplus;
				}
			}
		}
		
		// On va maintenant rechercher les extremums
		//
		ValeurMin=Valeurs.min();
		ValeurMax=Valeurs.max();
		
		// Calcul des coefficients de transformation
		//
		coefx=((double) size().width()) / ((double) DimXICH);
		if( ValeurMin != ValeurMax ) coefp=((double) size().height()) / ((double) (ValeurMax-ValeurMin)); else coefp=0.0;
		
		// On trace le profil
		//
		Dessin.setPen(PinceauJaune);
		
		for( x=0; x < DimXICH; x++ )
		{
			// Coordonnees de la valeur courante dans l'objet
			//
			xx=(int) (coefx*((double) x));
			yy=size().height()-(int) (coefp*((double) (Valeurs[x]-ValeurMin)));
			
			if( axx < 0 )
			{
				axx=xx;
				ayy=yy;
			}
			
			Dessin.drawLine(axx,ayy,xx,yy);
			
			axx=xx;
			ayy=yy;
		}

		// Affichage des valeurs extremes de la coupe
		//
		QFontMetrics InfosFontCourante=Dessin.fontMetrics();
		QString Chaine=QString("%1->%2").arg(ValMinICH).arg(ValMaxICH);
		Dessin.drawText(5,5+InfosFontCourante.height()*6,Chaine,-1,QPainter::LTR);	
	}
	
	
	// Si on peut, on trace le profil de la pose de centrage vertical
	//
	if( DonneesICV.size() != 0 && DimXICV != 0 && DimYICV != 0 )
	{
		int xx=-1;				// Coordonnees pour tracer le profil
		int yy=-1;
		int axx=-1;				// Anciennes coordonnees pour tracer le profil
		int ayy=-1;
		std::valarray<long> Valeurs;		// Valeurs du profil par ligne
		long ValeurMin;				// Valeur mini du profil integre
		long ValeurMax;				// Valeur maxi du profil integre
		double coefx;				// Coefficient de transformation sur l'axe des lignes
		double coefp;				// Coefficient de transformation sur l'axe du profil

		// On commence par ajouter les valeurs des lignes
		//
		Valeurs.resize(DimYICV);
		
		Valeurs=0;

		for( x=0; x < DimXICV; x++ )
		{
			for( y=0; y < DimYICV; y++ )
			{
				Valeurs[y]+=DonneesICV[x+y*DimXICV];
			}
		}

		// Si on doit amplifier une partie du centre du profil
		//
		if( lpdacpICV != 0.0 )
		{
			long i;				// Variable index travail
			std::valarray<long> pmoyen;	// Profil moyen sur 2x DimDV valeurs
			std::valarray<long> mdetails;	// Masque des details
			
			pmoyen.resize(DimYICV);
			mdetails.resize(DimYICV);
			
			pmoyen=0;
			mdetails=0;
			
			// On calcule le profil moyen sur un filtre carre de DimDV membres
			//
			for( i=0; i < DimYICV; i++ )
			{
				long indice;		// Indice dans le tableau
				long nbc=1;		// Nombre de cellules ajoutees
				
				pmoyen[i]=Valeurs[i];
				
				for( long j=1; j < DimDV; j++ )
				{
					indice=i-j;
					while( indice < 0 ) indice+=DimYICV;
					
					pmoyen[i]+=Valeurs[indice];
					
					indice=i+j;
					while( indice >= DimYICV ) indice-=DimYICV;
					
					pmoyen[i]+=Valeurs[indice];
					
					nbc+=2;
				}
				
				pmoyen[i]/=nbc;
			}
			
			// On calcule le masque des details
			//
			mdetails=Valeurs-pmoyen;
			
			// On amplifie les details dans la zone parametree
			//
			double bornei=50.0-lpdacpICV/2.0;
			double bornes=bornei+lpdacpICV;
			
			for( i=0; i < DimYICV; i++ )
			{
				// On passe i en pourcentage du nombre de membres
				//
				double pourc_i=(double) i;
				pourc_i/=(double) (DimYICV-1);
				pourc_i*=100.0;
			
				// Si on doit amplifier cette zone courante
				//
				if( pourc_i >= bornei && pourc_i <= bornes )
				{
					double enplus=((double) mdetails[i])*adcpICV;	// Valeur ajoute en plus
				
					Valeurs[i]+=(long) enplus;
				}
			}
		}
		
		// On va maintenant rechercher les extremums
		//
		ValeurMin=Valeurs.min();
		ValeurMax=Valeurs.max();

		// Calcul des coefficients de transformation
		//
		coefx=((double) size().height()) / ((double) DimYICV);
		if( ValeurMin != ValeurMax ) coefp=((double) size().width()) / ((double) (ValeurMax-ValeurMin)); else coefp=0.0;

		// On trace le profil
		//
		Dessin.setPen(PinceauCyan);

		for( y=0; y < DimYICV; y++ )
		{
			// Coordonnees de la valeur courante dans l'objet
			//
			xx=(int) (coefp*((double) (Valeurs[y]-ValeurMin)));
			yy=size().height()-(int) (coefx*((double) y));

			if( axx < 0 )
			{
				axx=xx;
				ayy=yy;
			}

			Dessin.drawLine(axx,ayy,xx,yy);

			axx=xx;
			ayy=yy;
		}

		// Affichage des valeurs extremes de la coupe
		//
		QFontMetrics InfosFontCourante=Dessin.fontMetrics();
		QString Chaine=QString("%1->%2").arg(ValMinICV).arg(ValMaxICV);
		Dessin.drawText(5,5+InfosFontCourante.height()*7,Chaine,-1,QPainter::LTR);	
	}

	Dessin.end();		// Liberation des ressources utilisees pour dessiner
}

