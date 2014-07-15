/* MODULE DU WIDGET DE LA ZONE POSE IMAGE

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


// Constructeur de la classe ObjPoseImage
//
ObjPoseImage::ObjPoseImage(QWidget *parent,const char *name,WFlags f) : QWidget(parent,name,f)
{
	// Couleur de fond du widget
	//
	setPaletteBackgroundColor(QColor(Qt::black));
	
	ForceIC_NULL();
	ForceICAff_NULL();
	ICPixmap.setOptimization(QPixmap::BestOptim);
	
	SeuilBasConsigne=0;
	SeuilHautConsigne=0;
	SeuilBas=0;
	SeuilHaut=0;
	aSeuilBas=-1;
	aSeuilHaut=-1;
	aTPalette=PaletteNULL;
	aZoneAffichee=ZoneNULL;
	ICJamaisAffichee=true;

	MaxRouge=MAX_ROUGE;
	MaxVert=MAX_VERT;
	MaxBleu=MAX_BLEU;

	TypePalette(NoirEtBlanc);
	PuissanceLUT(1.0);
	aPuiLUT=-1.0;
	ZoneAfficheeIC(Z0);

	update();	// Pour forcer l'affichage par defaut
}


// Destructeur de la classe ObjPoseImage
//
ObjPoseImage::~ObjPoseImage()
{
}


// Force l'image courante a devenir NULL
//
void ObjPoseImage::ForceIC_NULL(void)
{
	DonneesIC.resize(0);
	DimXIC=0;
	DimYIC=0;
	ValMinIC=0;
	ValMaxIC=0;
}


// Force l'image courante affichee a devenir NULL
//
void ObjPoseImage::ForceICAff_NULL(void)
{
	DonneesICAff.resize(0);
}


// Fonction de chargement d'une image dans l'objet
//
// CE:	On passe la dimension de l'image en abscisse (nombre de colonnes) ;
//
//	On passe la dimension de l'image en ordonnee (nombre de lignes) ;
//
//	On passe un pointeur sur unsigned long vers la source des donnees ;
//
// CS:	-
//
void ObjPoseImage::ChargerImage(unsigned short tx,unsigned short ty,unsigned short *pt,QDateTime debut,QDateTime fin,int duree)
{
	// Dimension de l'image chargee
	//
	DimXIC=tx;
	DimYIC=ty;

	// Chargement des pixels dans le tableau de valeur
	//
	unsigned long taille=tx;
	taille*=ty;

	DonneesIC.resize(taille);	// Redimensionnement du tableau de valeurs si besoin

	unsigned long i=0;

	while( taille-- )
	{
		DonneesIC[i++]=(long) *pt;

		pt++;
	} 

	// Recuperation des valeurs extremes
	//
	ValMinIC=DonneesIC.min();
	ValMaxIC=DonneesIC.max();
//std::cout << "IC: " << ValMinIC << "," << ValMaxIC << std::endl;

	// Les dates
	//
	DateHeureDebutPose=debut;
	DateHeureFinPose=fin;
	DureePose=duree;

	// L'image courante chargee n'a jamais ete affichee
	//
	ICJamaisAffichee=true;
}


// Fonction retournant la valeur minimale de l'image
//
long ObjPoseImage::ValMinImage(void)
{
	return ValMinIC;
}

// Fonction retournant la valeur maximale de l'image
//
long ObjPoseImage::ValMaxImage(void)
{
	return ValMaxIC;
}


// Fonction de parametrage de la zone affichee de l'image courante
//
// CE:	On passe la zone a afficher
//
void ObjPoseImage::ZoneAfficheeIC(ListeZAPIC zone)
{
	ZoneAffichee=zone;
}


// Fonction retournant la zone affichee de l'image courante
//
ListeZAPIC ObjPoseImage::ZoneAfficheeIC(void)
{
	return(ZoneAffichee);
}


// Fonction pour fixer la valeur maximale de la composante rouge
//
void ObjPoseImage::FixeMaxRouge(int valeur)
{
	MaxRouge=(double) valeur;
}


// Fonction pour fixer la valeur maximale de la composante verte
//
void ObjPoseImage::FixeMaxVert(int valeur)
{
	MaxVert=(double) valeur;
}


// Fonction pour fixer la valeur maximale de la composante bleue
//
void ObjPoseImage::FixeMaxBleu(int valeur)
{
	MaxBleu=(double) valeur;
}


// Fonction de parametrage du type de palette utilisee
//
void ObjPoseImage::TypePalette(TypesPalettes type)
{
	double liR,liV,liB;	// Limite inferieure en pourcentage du NB_ELEMENTS_PALETTE en dessous de laquelle la composante est nulle
	double lsR,lsV,lsB;	// Limite superieure en pourcentage du NB_ELEMENTS_PALETTE au dessus de laquelle la composante est maximale
	double lsRMliR;		// Valeur de l'intervale de la composante rouge
	double lsVMliV;		// Valeur de l'intervale de la composante verte
	double lsBMliB;		// Valeur de l'intervale de la composante bleue
	
	// Parametrage de la propriete de l'objet
	//
	TPalette=type;
	
	// Parametrage des fonctions de couleur
	//
	liR=liV=liB=lsR=lsV=lsB=(NB_ELEMENTS_PALETTE-1);
	
	switch( TPalette )
	{
		case NoirEtBlanc:
			liR=liV=liB=0;
			break;
			
		case Halpha:
			liR*=0;
			liV*=0.470;
			liB*=0.294;
			lsR*=0.666;
			lsV*=1.0;
			lsB*=1.0;
			break;
			
		case NoirOrangeBlanc:
			liR*=0;
			liV*=0.156;
			liB*=0.509;
			lsR*=0.59;
			lsV*=0.784;
			lsB*=1.0;
			break;
			
		case NoirVertBlanc:
			liR*=0.15;
			liV*=0;
			liB*=0.2;
			lsR*=0.98;
			lsV*=1.0;
			lsB*=0.95;
			break;
			
		case NoirBleuBlanc:
			liR*=0.2;
			liV*=0.15;
			liB*=0.0;
			lsR*=0.98;
			lsV*=0.95;
			lsB*=1.0;
			break;
			
		case PaletteNULL: break;
	}
	
	// Valeur des intervales
	//
	lsRMliR=lsR-liR;
	lsVMliV=lsV-liV;
	lsBMliB=lsB-liB;
	
	// Calcul des pinceaux de la palette
	//
	for( int i=0; i < NB_ELEMENTS_PALETTE; i++ )
	{
		double rouge;
		double vert;
		double bleu;
		
		// Calcul de la composante rouge
		//
		rouge=(double) i;
		
		if( rouge < liR )
		{
			rouge=0;
		}
		else
		{
			if( rouge > lsR ) rouge=MaxRouge; else rouge=(rouge-liR)*MaxRouge/lsRMliR;
		}
		
		// Calcul de la composante verte
		//
		vert=(double) i;
		
		if( vert < liV )
		{
			vert=0;
		}
		else
		{
			if( vert > lsV ) vert=MaxVert; else vert=(vert-liV)*MaxVert/lsVMliV;
		}
		
		// Calcul de la composante bleue
		//
		bleu=(double) i;
		
		if( bleu < liB )
		{
			bleu=0;
		}
		else
		{
			if( bleu > lsB ) bleu=MaxBleu; else bleu=(bleu-liB)*MaxBleu/lsBMliB;
		}
		
		PinceauxPalette[i].setColor(QColor((int) rouge,(int) vert,(int) bleu));
		PinceauxPalette[i].setStyle(Qt::SolidLine);
		PinceauxPalette[i].setWidth(0);
	}
}


// Fonction retournant le type de palette utilisee
//
TypesPalettes ObjPoseImage::TypePalette(void)
{
	return(TPalette);
}


// Fonction de calcul de la Look Up Table qui contient l'indice du numero de pinceau de la palette
//  en fonction de l'intensite rapportee au nombre possible
//
// CE:	On passe la valeur de la puissance x^valeur
//
// CS:	-
//
void ObjPoseImage::PuissanceLUT(double valeur)
{
	// Parametrage de la propriete de l'objet
	//
	PuiLUT=valeur;

	// Calcul du tableau de la LUT
	//	
	for( int i=0; i < NB_ELEMENTS_LUT; i++ )
	{
		double v=(double) i;					// Indice du numero de pinceau de la palette
		double fr;						// Partie fractionnaire
		
		v/=(NB_ELEMENTS_LUT-1.0);
		v=std::pow(v,PuiLUT)*(NB_ELEMENTS_PALETTE-1.0);
		
		fr=std::modf(v,&fr);
		
		if( fr > 0.5 ) v+=1.0;
		
		LUT[i]=(int) v;
	}
}


// Fonction retournant la valeur de la puissance de la LUT de visualisation
//
int ObjPoseImage::PuissanceLUT(void)
{
	double pe,fr;
	
	fr=std::modf(PuiLUT*100.0,&pe);
	
	if( fr > 0.5 ) pe+=1.0;
	
	return((int) pe);
}


// Fonction de parametrage de la consigne du seuil bas de visualisation
//
void ObjPoseImage::ConsigneSeuilBas(long valeur)
{
	SeuilBasConsigne=valeur;
}


// Fonction retournant la consigne du seuil bas de visualisation
//
long ObjPoseImage::ConsigneSeuilBas(void)
{
	return( SeuilBasConsigne );
}


// Fonction retournant le seuil bas de visualisation
//
long ObjPoseImage::ValSeuilBas(void)
{
	return( SeuilBas );
}


// Fonction de parametrage de la consigne du seuil haut de visualisation
//
void ObjPoseImage::ConsigneSeuilHaut(long valeur)
{
	SeuilHautConsigne=valeur;
}


// Fonction retournant la consigne du seuil haut de visualisation
//
long ObjPoseImage::ConsigneSeuilHaut(void)
{
	return( SeuilHautConsigne );
}


// Fonction retournant le seuil haut de visualisation
//
long ObjPoseImage::ValSeuilHaut(void)
{
	return( SeuilHaut );
}


// Fonction de parametrage des seuils en fonction des consignes
//
void ObjPoseImage::FixeSeuilsFnConsignes(void)
{
	if( SeuilBasConsigne >= ValMinIC && SeuilBasConsigne <= ValMaxIC ) SeuilBas=SeuilBasConsigne; else SeuilBas=ValMinIC;
	
	if( SeuilHautConsigne >= ValMinIC && SeuilHautConsigne <= ValMaxIC ) SeuilHaut=SeuilHautConsigne; else SeuilHaut=ValMaxIC;
}


// Fonction de calcul de l'indice du pinceau de la palette pour une valeur
//
// CE:	On passe la valeur de l'intensite du pixel ;
//
// CS:	La fonction retourne l'indice du pinceau dans la palette ;
//
int ObjPoseImage::IndicePinceau(long v)
{
	if( v <= SeuilBas )
	{
		return LUT[0];
	}
	else
	{
		if( v >= SeuilHaut )
		{
			return LUT[NB_ELEMENTS_LUT-1];
		}
		else
		{
			double i=((double) v)-((double) SeuilBas);
			i/=(double) (SeuilHaut-SeuilBas);
			i*=(NB_ELEMENTS_LUT-1);
			
			return LUT[(int) i];
		}
	}	
}


// Passer en mode de non effacement de l'aire du widget avant un paintEvent()
//
// Pour eviter un clignotement car l'aire est repainte en entier
//
void ObjPoseImage::NePasEffacerAvantPaintEvent(void)
{
	setWFlags(Qt::WNoAutoErase);
}


// Surcharge de la fonction de slot paintEvent heritee du QWidget
//
void ObjPoseImage::paintEvent(QPaintEvent *event)
{
	QPaintEvent *ev;	// Pour eviter un Warning a la compilation
	ev=event;

	// Representation graphique de la zone image
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
}


// Surcharge de la fonction retournant la dimension recommandee du widget
//
QSize ObjPoseImage::sizeHint()
{
	QSize dimension_recommandee(TAILLE_X_OBJPOSEIMAGE_RECOMMANDEE,TAILLE_Y_OBJPOSEIMAGE_RECOMMANDEE);
	
	return dimension_recommandee;
}


// Fonction de representation graphique de la zone image
//
//	CE:	-
//
//	CS:	-
//
void ObjPoseImage::RepresenterZoneImage(void)
{
	// On decide des seuils
	//
	if( SeuilBas == 0 && SeuilHaut == 0  && SeuilBasConsigne == 0 && SeuilHautConsigne == 0 )
	{
		// Aucune consigne
		//
		SeuilBasConsigne=SeuilBas=ValMinIC;
		SeuilHautConsigne=SeuilHaut=ValMaxIC;
	}

	// On utilise les consignes si elles sont applicables
	//
	FixeSeuilsFnConsignes();

	// On affiche l'image
	//
	AfficherImage();
}


// Fonction de calcul des donnees en intensite de l'image affichee avec les extremums optimises
//
void ObjPoseImage::CalculerImageAffichee(void)
{
	QPen Pinceau(QColor(15,15,25));		// Un pinceau pour la couleur de progression du calcul
	Pinceau.setStyle(Qt::SolidLine);
	Pinceau.setWidth(0);
	
	QPainter Dessin(this);	// Un objet de dessin sur ce widget
	Dessin.setPen(Pinceau);			// Le crayon utilise pour la progression du calcul
	
	// Initialisation des variables pour la transformation
	//
	double x0s;				// Coordonnees et dimension du carre correspondant dans les donnees source de l'image
	double y0s;
	double txs;
	double tys;
	double x0d=0;				// Coordonnees et dimension du carre affiche dans le widget
	double y0d=0;
	long hauteur_zone=size().height();	// Hauteur du widget de la zone image
	double txd=(double) hauteur_zone;
	double tyd=(double) hauteur_zone;
	double CoefSversD;			// Coefficient de transformation de source vers destination
	double x;				// Variable de travail
	double y;
	double fr;				// Partie fractionnaire 
	
	// Abscisse origine dans l'image source
	//
	switch( ZoneAffichee )
	{
		case Z0:
		case Z1:
		case Z4:
		case Z7:
			x0s=0.0;
			break;
			
		case Z2:
		case Z5:
		case Z8:
			x0s=((double) DimXIC)/3.0;
			break;
	
		case Z3:
		case Z6:
		case Z9:
			x0s=2.0*((double) DimXIC)/3.0;
			break;
			
		case ZoneNULL:	break;
	}
	
	// Ordonnee origine dans l'image source
	//
	txs=tys=((double) DimYIC)/3.0;
	
	switch( ZoneAffichee )
	{
		case Z0:
			y0s=0;
			txs=tys=(double) DimYIC;
			break;
			
		case Z1:
		case Z2:
		case Z3:
			y0s=2.0*((double) DimYIC)/3.0;
			break;
			
		case Z4:
		case Z5:
		case Z6:
			y0s=((double) DimYIC)/3.0;
			break;
	
		case Z7:
		case Z8:
		case Z9:
			y0s=0;
			break;
			
		case ZoneNULL:	break;
	}
	
	// On commence toujours sur un pixel entier
	//
	fr=std::modf(x0s,&x0s);
	fr=std::modf(y0s,&y0s);
	
	// La dimension est toujours entiere
	//
	fr=std::modf(txs,&txs);
	tys=txs;
	
	// Coefficient de transformation de Source vers Destination (Affichee)
	//
	CoefSversD=tyd/tys;
	
	// L'image affichee sera toujours carree avec la hauteur du widget pour dimension
	//
	DonneesICAff.resize(hauteur_zone*hauteur_zone);
	
	// Tous les pixels sont initialises a une valeur negative pour pouvoir realiser le test de l'optimisation
	//
	DonneesICAff=-1;
	
//std::cout << "s:" << x0s << " " << y0s << " " << txs << " " << tys << std::endl;
//std::cout << "d:" << x0d << " " << y0d << " " << txd << " " << tyd << std::endl;
	
	// Si il y a plus de pixels dans l'image source que dans l'image affichee de destination
	//
	if( CoefSversD < 1.0 )
	{
		// Pour chaque pixel de l'image source on calcul le pixel de l'image affichee de destination
		//
		for( y=y0s; y < y0s+tys; y+=1.0 )
		{
			long offsetlignes=(long) (y*DimXIC);
			double yd;		// Ordonnee de (y) dans l'image destination
			
			yd=y0d+(y-y0s)*CoefSversD;
			std::modf(yd,&yd);
			
			// yd indique directement la progression du calcul
			//
			Dessin.drawLine(0,hauteur_zone-1-((int) yd),hauteur_zone-1,hauteur_zone-1-((int) yd));
			
			for( x=x0s; x < x0s+txs; x+=1.0 )
			{
				double xd;	// Abscisse de (x) dans l'image destination
				long indiced;	// Offset dans les tableaux de donnees
				long indices;
				
				// Coordonnees de (x,y) dans l'image destination affichee
				//
				xd=x0d+(x-x0s)*CoefSversD;
				std::modf(xd,&xd);
				
				// Indice du pixel dans le tableau de donnees de l'image source
				//
				indices=offsetlignes+(long) x;
				
				// Indice du pixel dans le tableau de donnees de l'image destination affichee
				//
				if( xd >= txd )
				{
					xd=txd-1;
					std::cout << "ObjPoseImage::CalculerImageAffichee(): xd >= txd." << std::endl;
				}
				if( yd >= tyd )
				{
					yd=tyd-1;
					std::cout << "ObjPoseImage::CalculerImageAffichee(): yd >= tyd." << std::endl;
				}
				indiced=(long) (xd+yd*txd);
				
				// Si le pixel n'a jamais ete calcule
				//
				if( DonneesICAff[indiced] < 0 )
				{
					DonneesICAff[indiced]=DonneesIC[indices];
				}
				else
				{
					if( DonneesIC[indices] < DonneesICAff[indiced] ) DonneesICAff[indiced]=DonneesIC[indices];
					if( DonneesIC[indices] > DonneesICAff[indiced] ) DonneesICAff[indiced]=DonneesIC[indices];
				}
			}
		}
	}
	else
	{
		// Pour chaque pixel de l'image destination affichee on calcul le pixel interpole dans l'image source
		//
		for( y=y0d; y < y0d+tyd; y+=1.0 )
		{
			long offsetligned=(long) (y*txd);
			
			// y indique directement la progression du calcul
			//
			Dessin.drawLine(0,hauteur_zone-1-((int) y),hauteur_zone-1,hauteur_zone-1-((int) y));
			
			for( x=x0d; x < x0d+txd; x+=1.0 )
			{
				double xs;	// Coordonnees de (x,y) dans l'image source
				double ys;
				long indiced;	// Offset dans les tableaux de donnees
				long i;		// Intensite du pixel par interpolation bi-lineaire
				
				// Coordonnees de (x,y) dans l'image source
				//
				xs=x0s+(x-x0d)/CoefSversD;
				ys=y0s+(y-y0d)/CoefSversD;
				
				// Intensite du pixel par interpolation
				//
				i=(long) InterpolationBiLineaire(xs,ys);
				
				// Indice du pixel dans le tableau de donnees de l'image destination affichee
				//
				indiced=offsetligned+(long) x;
				
				// Si le pixel n'a jamais ete calcule
				//
				if( DonneesICAff[indiced] < 0 )
				{
					DonneesICAff[indiced]=i;
				}
				else
				{
					if( i < DonneesICAff[indiced] ) DonneesICAff[indiced]=i;
					if( i > DonneesICAff[indiced] ) DonneesICAff[indiced]=i;
				}
			}
		}
	}

	Dessin.end();			// Liberation des ressources utilisees pour dessiner
}


// Fonction d'interpolation bi-lineaire dans l'image source courante
//

double ObjPoseImage::InterpolationBiLineaire(double x,double y)
{
	double t;		// Constante pour le calcul
	double u;
	double un_m_t;
	double un_m_u;
	double px;		// Partie entiere de x
	double py;		// Partie entiere de y
	double valeur;		// Valeur interpolee
	long ptp;		// Offset sur pixel haut-gauche
	
	// Test si les coordonnees sont sur l'image
	//
	if( x < 0.0 || x >= ((double) DimXIC) || y < 0.0 || y >= ((double) DimYIC) )
	{
		return 0.0;
	}
	
	t=std::modf(x,&px);	// Partie fractionnaire
	u=std::modf(y,&py);
	
	un_m_t=1.0-t;		// 1-t pour vitesse
	un_m_u=1.0-u;		// 1-u pour vitesse
	
	// Cas ou x == dimxi-1
	//
	if( ((long) px) == (DimXIC-1) )
	{
		px-=1.0;
		t=1.0;
		un_m_t=0.0;
	}
	
	// Cas ou y == dimyi-1
	//
	if( ((long) py) == (DimYIC-1) )
	{
		py-=1.0;
		u=1.0;
		un_m_u=0.0;
	}
	
	// Offset sur pixel en haut a gauche du carre a interpoler
	//
	ptp=((long) py)*DimXIC;
	ptp+=(long) px;
	
	// Interpolation Bi-Lineaire
	//
	valeur=((double) DonneesIC[ptp])*un_m_t*un_m_u;
	valeur+=((double) DonneesIC[ptp+1])*t*un_m_u;
	valeur+=((double) DonneesIC[ptp+1+DimXIC])*t*u;
	valeur+=((double) DonneesIC[ptp+DimXIC])*un_m_t*u;
	
	return valeur;
}


// Fonction d'affichage de l'image
//
void ObjPoseImage::AfficherImage(void)
{
	int tracer=false;
	long hauteur_zone=size().height();	// Hauteur du widget de la zone image
	
	// Si on doit recalculer les donnees de l'image affichee avec l'algorithme du contraste optimise
	//
	if( (DonneesICAff.size() != (unsigned long)(hauteur_zone*hauteur_zone)) || (aZoneAffichee != ZoneAffichee) || ICJamaisAffichee )
	{
		CalculerImageAffichee();
		
		tracer=true;
	}
	
	// Si on a besoin de reconstruire/recalculer le Pixmap de l'image en dehors de l'ecran
	//
	if( ICPixmap.width() != hauteur_zone )
	{
		// On redimensionne le QPixmap a la dimension de la zone image
		//
		ICPixmap.resize(hauteur_zone,hauteur_zone);
		
		tracer=true;
	}
	
	if( ICJamaisAffichee ) tracer=true;
	
	// Si on doit tracer dans le QPixmap de l'image courante pour une raison
	//
	if( tracer || (aSeuilBas != SeuilBas) || (aSeuilHaut != SeuilHaut) || (aPuiLUT != PuiLUT) || (aTPalette != TPalette) || (aZoneAffichee != ZoneAffichee) )
	{
		double CoefPente;	// Coefficient de transformation valeur vers indice dans la LUT
		
		// Un objet de dessin sur le QPixmap
		//
		QPainter DessinICP(&ICPixmap);
		
		// Calcul du coefficient de transformation de l'intensite du pixel vers l'indice dans la LUT
		//
		if( SeuilHaut != SeuilBas ) CoefPente=(NB_ELEMENTS_LUT-1)/((double) (SeuilHaut-SeuilBas)); else CoefPente=0.0;
		
		// On trace chaque pixel dans le Pixmap hors ecran
		//
		long ypp=hauteur_zone-1;
		
		for( long y=0; y < (long) hauteur_zone; y++ )
		{
			long offsetligne=y*hauteur_zone;
			
			for( long x=0; x < (long) hauteur_zone; x++ )
			{
				long v=DonneesICAff[x+offsetligne];	// Valeur du pixel a afficher
				
				if( v < SeuilBas )
				{
					// Pixel avec le pinceau du premier indice de la LUT
					//
					DessinICP.setPen(PinceauxPalette[LUT[0]]);
					DessinICP.drawPoint(x,ypp-y);
				}
				else
				{
					if( v > SeuilHaut )
					{
						// Pixel avec le pinceau du dernier indice de la LUT
						//
						DessinICP.setPen(PinceauxPalette[LUT[NB_ELEMENTS_LUT-1]]);
						DessinICP.drawPoint(x,ypp-y);
					}
					else
					{
						// Indice dans la LUT correspondante
						//
						double indiceLUT=(double) v;
						indiceLUT-=(double) SeuilBas;
						indiceLUT*=CoefPente;
						
						// Pixel avec le pinceau de l'indice de la LUT
						//
						DessinICP.setPen(PinceauxPalette[LUT[(long) indiceLUT]]);
						DessinICP.drawPoint(x,ypp-y);
					}
				}
			}
		}

		// Affichage des valeurs extremes de l'image
		//
		QFontMetrics InfosFontCourante=DessinICP.fontMetrics();
		QPen PinceauBlanc(Qt::white);
		DessinICP.setPen(PinceauBlanc);
		QString Chaine=DateHeureDebutPose.toString(QString("yyyy-MM-dd"));
		DessinICP.drawText(5,5+InfosFontCourante.height(),Chaine,-1,QPainter::LTR);
		Chaine=DateHeureDebutPose.toString(QString("hh:mm:ss.zzz"));
		DessinICP.drawText(5,5+InfosFontCourante.height()*2,Chaine,-1,QPainter::LTR);
		Chaine=DateHeureFinPose.toString(QString("hh:mm:ss.zzz"));
		DessinICP.drawText(5,5+InfosFontCourante.height()*3,Chaine,-1,QPainter::LTR);
		Chaine=QString("%1s").arg(((double) (DureePose))/10000.0);
		DessinICP.drawText(5,5+InfosFontCourante.height()*4,Chaine,-1,QPainter::LTR);
		Chaine=QString("%1->%2").arg(ValMinIC).arg(ValMaxIC);
		DessinICP.drawText(5,5+InfosFontCourante.height()*5,Chaine,-1,QPainter::LTR);
		
		// Sauvegarde des parametres de representation
		//
		aSeuilBas=SeuilBas;
		aSeuilHaut=SeuilHaut;
		aPuiLUT=PuiLUT;
		aTPalette=TPalette;
		aZoneAffichee=ZoneAffichee;
		
		// On a terminer de dessiner, on libere les ressources utilisees
		//
		DessinICP.end();
	}
	
	// On affiche directement du QPixmap vers l'objet ObjPoseImage sur l'ecran (QWidget) (ils heritent tous les deux d'un QPaintDevice)
	//
	bitBlt(this,0,0,&ICPixmap,0,0,hauteur_zone,hauteur_zone,Qt::CopyROP,FALSE);
	
	// L'image courante chargee vient d'etre affichee au moins une fois
	//
	ICJamaisAffichee=false;
}

