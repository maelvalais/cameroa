/* HEADER DU MODULE DU WIDGET DE LA ZONE POSE IMAGE

   (C)David.Romeuf@univ-lyon1.fr 05/2006
*/

#ifndef _WIDGETPOSEIMAGE_
#define _WIDGETPOSEIMAGE_

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

#define TAILLE_X_OBJPOSEIMAGE_RECOMMANDEE	512	// Taille recommandee en x de l'objet
#define TAILLE_Y_OBJPOSEIMAGE_RECOMMANDEE	512	// Taille recommandee en y de l'objet

#define NB_ELEMENTS_LUT		2048			// Nombre d'elements pour la Look Up Table
#define NB_ELEMENTS_PALETTE	2048			// Nombre d'elements pour la palette

#define MAX_ROUGE		255			// Valeur maximale de la composante rouge
#define MAX_VERT		255			// Valeur maximale de la composante verte
#define MAX_BLEU		255			// Valeur maximale de la composante bleue


// Types des palettes d'affichage
//
enum TypesPalettes
{
	NoirEtBlanc,
	Halpha,
	NoirOrangeBlanc,
	NoirVertBlanc,
	NoirBleuBlanc,
	PaletteNULL
};


// Liste des zones affichables possibles de l'image courante
//
enum ListeZAPIC
{
	Z0,		// Image entiere
	Z1,		// Coin superieur gauche
	Z2,
	Z3,		// Coin superieur droit
	Z4,		// Tier superieur gauche
	Z5,
	Z6,
	Z7,		// 2/3 superieur gauche
	Z8,
	Z9,
	ZoneNULL
};


// Widget de zone d'affichage graphique d'une pose image
//
class ObjPoseImage : public QWidget
{
	Q_OBJECT

// Zone inaccessible sauf par l'objet lui meme
private :

// Zone protegee de l'exterieur sauf des classes qui heritent
protected:
	// Proprietes
	std::valarray<long> DonneesIC;			// Donnees de l'image courante dans un tableau de valeur de la STL C++
	QDateTime DateHeureDebutPose;			// Date et heure de debut de la pose
	QDateTime DateHeureFinPose;				// Date et heure de fin de la pose
	int DureePose;							// Duree de la pose en 1/10000 s
	long DimXIC;					// Dimension en x de l'image courante
	long DimYIC;					// Dimension en y de l'image courante
	long ValMinIC;					// Valeur minimale des donnees de l'image
	long ValMaxIC;					// Valeur maximale des donnees de l'image
	long SeuilBasConsigne;				// Seuil bas de visualisation en consigne
	long SeuilHautConsigne;				// Seuil haut de visualisation en consigne
	long SeuilBas;					// Seuil bas de visualisation effectif
	long aSeuilBas;					// Pour sauvegarder le seuil bas utilise lors du dernier trace
	long SeuilHaut;					// Seuil haut de visualisation effectif
	long aSeuilHaut;				// Pour sauvegarder le seuil haut utilise lors du dernier trace
	double PuiLUT;					// Puissance de la forme de la Look Up Table
	double aPuiLUT;					// Pour sauvegarder la puissance de la LUT lors du dernier trace
	ListeZAPIC ZoneAffichee;			// Zone affichee de l'image courante
	ListeZAPIC aZoneAffichee;			// Pour sauvegarder l'ancienne zone affichee
	int ICJamaisAffichee;				// Drapeau pour signifier que l'image courante chargee n'a jamais ete affichee
	std::valarray<long> DonneesICAff;		// Donnees de l'image courante affichee sur l'ecran dans un valarray de la STL C++
	QPixmap ICPixmap;				// Pixmap pour l'affichage rapide de l'image courante via bitBlt sur ce widget
	double MaxRouge;				// Valeur maximale pour la composante rouge
	double MaxVert;					// Valeur maximale pour la composante verte
	double MaxBleu;					// Valeur maximale pour la composante bleue
	TypesPalettes TPalette;				// Type de palette
	TypesPalettes aTPalette;			// Pour sauvegarder le type de palette
	QPen PinceauxPalette[NB_ELEMENTS_PALETTE];	// Pinceaux de la palette
	int LUT[NB_ELEMENTS_LUT];			// Nombre d'elements de la Look Up Table (indice n palette fn valeur)

	// Methodes
	void paintEvent(QPaintEvent *event);		// Surcharge du slot herite de QWidget
	QSize sizeHint();				// Surcharge de la fonction retournant la dimension recommandee pour le widget
	double InterpolationBiLineaire(double x,double y);	// Fonction d'interpolation bi-lineaire dans l'image

// Zone accessible par l'exterieur
public:
	void ForceIC_NULL(void);			// Force l'image courante a l'etat NULL
	void ForceICAff_NULL(void);			// Force l'image courante affichee a l'etat NULL

	// Constructeur de l'objet (jamais de type de retour)
	ObjPoseImage(QWidget *parent=0,const char *name=0,WFlags f=0);
	
	// Destructeur de l'objet (jamais de type de retour et d'argument)
	~ObjPoseImage();

	// Fonction de chargement d'une image dans l'objet
	//
	void ChargerImage(unsigned short tx,unsigned short ty,unsigned short *pt,QDateTime debut,QDateTime fin,int duree);
	
	// Fonction de parametrage du type de palette utilisee
	//
	void TypePalette(TypesPalettes type);
	
	// Fonction retournant le type de palette utilisee
	//
	TypesPalettes TypePalette(void);

	// Fonction pour fixer la valeur maximale de la composante rouge
	//
	void FixeMaxRouge(int valeur);

	// Fonction pour fixer la valeur maximale de la composante verte
	//
	void FixeMaxVert(int valeur);

	// Fonction pour fixer la valeur maximale de la composante bleue
	//
	void FixeMaxBleu(int valeur);

	// Fonction retournant la valeur minimale de l'image
	//
	long ValMinImage(void);

	// Fonction retournant la valeur maximale de l'image
	//
	long ValMaxImage(void);

	// Fonction de parametrage de la consigne du seuil bas de visualisation
	//
	void ConsigneSeuilBas(long valeur);
	
	// Fonction retournant la consigne du seuil bas
	//
	long ConsigneSeuilBas(void);
	
	// Fonction retournant le seuil bas
	//
	long ValSeuilBas(void);
	
	// Fonction de parametrage de la consigned du seuil haut de visualisation
	//
	void ConsigneSeuilHaut(long valeur);
	
	// Fonction retournant la consigne du seuil haut
	//
	long ConsigneSeuilHaut(void);
	
	// Fonction retournant le seuil haut
	//
	long ValSeuilHaut(void);
	
	// Fonction de calcul de la LUT
	//
	void PuissanceLUT(double valeur);
	
	// Fonction retournant la valeur de la puissance de la LUT de visualisation
	//
	int PuissanceLUT(void);
	
	// Fonction de parametrage des seuils en fonction des consignes
	//
	void FixeSeuilsFnConsignes(void);
	
	// Fonction de parametrage de la zone affichee de l'image courante
	//
	void ZoneAfficheeIC(ListeZAPIC zone);
	
	// Fonction retournant la zone affichee de l'image courante
	//
	ListeZAPIC ZoneAfficheeIC(void);
	
	// Fonction de representation graphique de la zone image
	//
	void RepresenterZoneImage(void);
		
	// Fonction de calcul des donnees en intensite de l'image affichee avec les extremums optimises
	//
	void CalculerImageAffichee(void);
	
	// Fonction d'affichage de l'image
	//
	void AfficherImage(void);
	
	// Fonction de calcul de l'indice du pinceau de la palette pour une valeur
	//
	int IndicePinceau(long v);

	// Passer en mode de non effacement de l'aire du widget avant un paintEvent()
	//
	void NePasEffacerAvantPaintEvent(void);
};

#endif
