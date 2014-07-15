/* HEADER DE DEFINITION DE LA STRUCTURE DE DONNEES D'UNE TRAME IMAGE CAMEROA

  (C)David.Romeuf@univ-lyon1.fr 09/05/2006 par David Romeuf
*/


#ifndef _TRAMEIMAGECAMEROA_H_
#define _TRAMEIMAGECAMEROA_H_

// Les structures de donnees de CamerOA
//
#define CH_MAG_ENTDONCAMEROA		"a1w2z3x4e5c6r7v8t9bAyBnC"
#define TAILLE_CH_MAG_ENTDONCAMEROA	25								// longueur chaine+1 pour compter le 0

#define TRAME_IMAGE					0
#define TRAME_CENTRAGE_IMAGE		1
#define TRAME_CENTRAGE_H			2
#define TRAME_CENTRAGE_V			3

#define TYPE_IMG_CAMEROA_POSEI		0	// Pose image
#define TYPE_IMG_CAMEROA_POSEBIAS	1	// Pose de BIAS
#define TYPE_IMG_CAMEROA_POSEDARK	2	// Pose de DARK
#define TYPE_IMG_CAMEROA_POSEC		3	// Pose de centrage

struct EnteteTrameDonneesCamerOA
{
	char chaine_magique[TAILLE_CH_MAG_ENTDONCAMEROA];	// Chaine magique empreinte pour reconnaitre le debut de la trame video
	char type_trame;									// Le type de trame image
	char type_image;									// Le type d'image en fonction du type de trame
	unsigned long tx;									// La dimension en abscisse de l'image  (on va envoyer de l'unsigned short 16 bits)
	unsigned long ty;									// La dimension en ordonnee de l'image
	QDateTime debut;									// Heure du debut de pose
	QDateTime fin;										// Heure de fin de pose
	int duree;											// Duree de la pose en 1/10000 de seconde
};

#endif
