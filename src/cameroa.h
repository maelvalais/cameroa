/* HEADER DU MODULE DE LA CLASSE DE LA FENETRE PRINCIPALE DE L'APPLICATION CamerOA

   LOGICIEL RESEAU DE CONTROLE DE CAMERA CCD

   (C)David.Romeuf@univ-lyon1.fr 09/05/2006 par David Romeuf
*/


#ifndef _CAMEROA_H_
#define _CAMEROA_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Inclusions Qt et KDE
//
#include <kmainwindow.h>
#include <kapplication.h>
#include <qevent.h>
#include <qlabel.h>
#include <qmutex.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <qstring.h>
#include <qthread.h>
#include <qtimer.h>
#include <qvbox.h>

// Inclusions C++
//
#include "WidgetPoseImage.h"
#include "WidgetPoseCentrage.h"
#include "pointscomm.h"
#include "trameimage.h"

// Definitions
//
#define TAILLE_X_BASE_CAMEROA	1024
#define TAILLE_Y_BASE_CAMEROA	552

#define TAILLE_BOUTON_MAXI_X	32
#define TAILLE_BOUTON_MAXI_Y	32

#define TAILLE_MINI_CHAINE		255

#define FICHIER_SAUV_CONSIGNES	"CamerOA_Consignes.dat"

#define ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGECENTRAGE		30000
#define ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_H_CENTRAGE	30001
#define ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_V_CENTRAGE	30002
#define ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE				30003
#define ID_CUSTOM_EVENT_CAMEROA_CTEMP							30004
#define ID_CUSTOM_EVENT_CAMEROA_CTPI							30005
#define ID_CUSTOM_EVENT_CAMEROA_CTPC							30006
#define ID_CUSTOM_EVENT_CAMEROA_QUIT							30007
#define ID_CUSTOM_EVENT_CAMEROA_POSEI							30008
#define ID_CUSTOM_EVENT_CAMEROA_ARRETPOSE						30009
#define ID_CUSTOM_EVENT_CAMEROA_FSHUTTEROPEN					30010
#define ID_CUSTOM_EVENT_CAMEROA_FSHUTTERCLOSE					30011
#define ID_CUSTOM_EVENT_CAMEROA_FANLOW							30012
#define ID_CUSTOM_EVENT_CAMEROA_FANMEDIUM						30013
#define ID_CUSTOM_EVENT_CAMEROA_FANHIGH							30014
#define ID_CUSTOM_EVENT_CAMEROA_FANOFF							30015
#define ID_CUSTOM_EVENT_CAMEROA_UTILSHUTCENT					30016
#define ID_CUSTOM_EVENT_CAMEROA_NONUTILSHUTCENT					30017
#define ID_CUSTOM_EVENT_CAMEROA_POSEDOUBLEI						30018
#define ID_CUSTOM_EVENT_CAMEROA_POSEBIAS						30019
#define ID_CUSTOM_EVENT_CAMEROA_POSEDARK						30020
#define ID_CUSTOM_EVENT_CAMEROA_CTPD							30021
#define ID_CUSTOM_EVENT_CAMEROA_PARAM_AMPLI_PROFH				30022
#define ID_CUSTOM_EVENT_CAMEROA_PARAM_AMPLI_PROFV				30023

#define MIN_CTEMP	-40
#define MAX_CTEMP	25

#define MIN_TPCENTRAGE	1
#define MAX_TPCENTRAGE	600000

#define MIN_TPIMAGE	1
#define MAX_TPIMAGE	36000000

#define MIN_MATRICE_AMP_PROF	1
#define MAX_MATRICE_AMP_PROF	100

#define MIN_LARG_ZA_PROF		1.0
#define MAX_LARG_ZA_PROF		100.0

#define MIN_AMPLI_PROF			0.0
#define MAX_AMPLI_PROF			1000.0

#define NB_VIDAGE_POSE_IMAGE			2
#define NB_VIDAGE_POSE_CENTRAGE			1

#define MODULO_CALAGE_TEMPOREL_POSE		2

#define BINNING_POSE_VIDAGE_IMAGE		4
#define BINNING_POSE_VIDAGE_CENTRAGE	8

#define BINNING_POSE_IMAGE_CENTRAGE	1
#define BINNING_POSE_CENTRAGE		1
#define NB_LIGNES_POSE_CENTRAGE		4	
#define NB_COLONNES_POSE_CENTRAGE	4

#define MAX_COMPOSANTES_RVB_OBJ_CENTRAGE	200

#define TAILLE_MAX_BUFFER_LECTURE	2048


// Predeclaration des classes pour imbrication de pointeurs entre processus
//
class CamerOA;
class ProcessusLegerControleCamera;
class ProcessusLegerServeurCommandes;
class ProcessusLegerServeurDonnees;


// Classe de definition du thread de controle de la camera
//
class ProcessusLegerControleCamera : public QThread
{
private:
	int DrapeauDemandeTerminaison;				// Drapeau de demande de terminaison propre du processus
	QMutex MutexDrapeauDemandeTerminaison;

	int DemandeRTC_CCD;							// Drapeau de demande de rafraichissement de la temperature CCD
	QMutex MutexDemandeRTC_CCD;

	int DemandeRTR;								// Drapeau de demande de rafraichissement de la temperature radiateur
	QMutex MutexDemandeRTR;

	int DemandeRPP;								// Drapeau de demande de rafraichissement de la puissance Peltier
	QMutex MutexDemandeRPP;

	int DemandeFanOff;							// Drapeau de demande de ventilateur off
	QMutex MutexDemandeFanOff;

	int DemandeFanLow;							// Drapeau de demande de ventilateur low
	QMutex MutexDemandeFanLow;

	int DemandeFanMedium;						// Drapeau de demande de ventilateur medium
	QMutex MutexDemandeFanMedium;

	int DemandeFanHigh;							// Drapeau de demande de ventilateur high
	QMutex MutexDemandeFanHigh;

	int DemandePCT;								// Drapeau de demande de parametrage de la consigne de temperature
	QMutex MutexDemandePCT;

	int DemandeFSO;								// Drapeau de demande de forcage de l'obturateur ouvert
	QMutex MutexDemandeFSO;

	int DemandeFSC;								// Drapeau de demande de forcage de l'obturateur ferme
	QMutex MutexDemandeFSC;

	int DemandePoseI;							// Drapeau de demande de pose image
	QMutex MutexDemandePoseI;

	int DemandePoseDoubleI;						// Drapeau de demande de pose image doublee
	QMutex MutexDemandePoseDoubleI;

	int DemandePoseB;							// Drapeau de demande de pose BIAS
	QMutex MutexDemandePoseB;

	int DemandePoseD;							// Drapeau de demande de pose DARK
	QMutex MutexDemandePoseD;

	double TemperatureCCD;						// Temperature du CCD en degres Celsius
	QMutex MutexTemperatureCCD;

	QDateTime DateHeureTemperatureCCD;			// Date et heure de la releve de la temperature
	QMutex MutexDateHeureTemperatureCCD;

	double TemperatureRadiateur;				// Temperature du radiateur en degres Celsius
	QMutex MutexTemperatureRadiateur;

	QDateTime DateHeureTemperatureRadiateur;	// Date et heure de la releve de la temperature
	QMutex MutexDateHeureTemperatureRadiateur;

	double PuissancePeltier;					// Puissance electrique des etages Peltier
	QMutex MutexPuissancePeltier;

	QDateTime DateHeurePuissancePeltier;		// Date et heure de la releve de la puissance
	QMutex MutexDateHeurePuissancePeltier;

	QDateTime DateHeureDebutPoseImage;			// Date et heure de depart de la pose image courante
	QMutex MutexDateHeureDebutPoseImage;

	QDateTime DateHeureFinPoseImage;			// Date et heure de fin de la pose image courante
	QMutex MutexDateHeureFinPoseImage;

	int DureePoseImage;							// Duree de la pose image courante
	QMutex MutexDureePoseImage;

	QDateTime DateHeureDebutPoseCentrage;		// Date et heure de debut de la pose image courante de centrage
	QMutex MutexDateHeureDebutPoseCentrage;

	QDateTime DateHeureFinPoseCentrage;			// Date et heure de fin de la pose image courante de centrage
	QMutex MutexDateHeureFinPoseCentrage;

	int DureePoseCentrage;						// Duree de la pose image courante de centrage
	QMutex MutexDureePoseCentrage;

	QDateTime DateHeureDebutPoseH;				// Date et heure de debut de la pose image courante de centrage horizontal
	QMutex MutexDateHeureDebutPoseH;

	QDateTime DateHeureFinPoseH;				// Date et heure de fin de la pose image courante de centrage horizontal
	QMutex MutexDateHeureFinPoseH;

	int DureePoseH;								// Duree de la pose image courante de centrage horizontal
	QMutex MutexDureePoseH;

	QDateTime DateHeureDebutPoseV;				// Date et heure de debut de la pose image courante de centrage vertical
	QMutex MutexDateHeureDebutPoseV;

	QDateTime DateHeureFinPoseV;				// Date et heure de fin de la pose image courante de centrage vertical
	QMutex MutexDateHeureFinPoseV;

	int DureePoseV;								// Duree de la pose image courante de centrage vertical
	QMutex MutexDureePoseV;

	int ConsigneObturateurCentrage;				// Consigne de l'utilisation de l'obturateur durant une pose de centrage
	QMutex MutexConsigneObturateurCentrage;
	int DemandeUtilObtuCentrage;				// Drapeau pour la demande d'utilisation de l'obturateur pour le centrage
	QMutex MutexDemandeUtilObtuCentrage;
	int DemandeNonUtilObtuCentrage;				// Drapeau pour la demande de non utilisation de l'obturateur pour le centrage
	QMutex MutexDemandeNonUtilObtuCentrage;

	int ConsigneTemperature;					// Valeur de consigne de la temperature
	QMutex MutexConsigneTemperature;

	unsigned long CompteurPoseCentrage;			// Compteur du nombre de pose de centrage

public:
	CamerOA *FPCamerOA;							// Pointeur sur la fenetre principale de l'application
	
	// Constructeur du thread
	//
	ProcessusLegerControleCamera(void);
	
	// Destructeur du thread
	//
	virtual ~ProcessusLegerControleCamera();
	
	virtual void run();					// Surcharge de la methode run() qui contient le code d'execution du thread
	
	void DemandeTerminaison(void);		// Fonction de positionnement de la demande de terminaison propre du processus leger

	void DemandeRafraichissementTemperatureCCD(void);		// Demande maj temperature CCD
	void RafraichissementTemperatureCCD(void);				// maj temperature CCD

	void DemandeRafraichissementTemperatureRadiateur(void);	// Demande maj temperature radiateur
	void RafraichissementTemperatureRadiateur(void);		// maj temperature radiateur

	void DemandeRafraichissementPuissancePeltier(void);		// Demande maj puissance peltier
	void RafraichissementPuissancePeltier(void);			// maj puissance peltier

	void DemandeFanModeOff(void);							// Demande ventilateur off
	void FanModeOff(void);									// Ventilateur off

	void DemandeFanModeLow(void);							// Demande ventilateur low
	void FanModeLow(void);									// Ventilateur low

	void DemandeFanModeMedium(void);						// Demande ventilateur medium
	void FanModeMedium(void);								// Ventilateur medium

	void DemandeFanModeHigh(void);							// Demande ventilateur high
	void FanModeHigh(void);									// Ventilateur high

	void DemandeParamConsigneTemperature(int valeur);		// Demande parametrage temperature camera
	void ParamConsigneTemperature(void);					// Parametrage consigne temperature camera

	void DemandeForceShutterOpen(void);						// Demande forcage ouverture obturateur
	void ForceShutterOpen(void);							// Forcage ouverture obturateur

	void DemandeForceShutterClose(void);					// Demande forcage fermeture obturateur
	void ForceShutterClose(void);							// Forcage fermeture obturateur

	void DemandeUtilisationObturateurCentrage(void);		// Demande d'utilisation de l'obturateur pour le centrage
	void UtiliserObturateurCentrage(void);					// Fonction pour utiliser l'obturateur pour le centrage
	void DemandeNonUtilisationObturateurCentrage(void);		// Demande de non utilisation de l'obturateur pour le centrage
	void NePasUtiliserObturateurCentrage(void);				// Fonction pour ne pas utiliser l'obturateur pour le centrage

	void DemandeStopPoseImage(void);						// Demande arret de pose photo

	int PoseCentrageH(void);								// Pose centrage horizontal
	int PoseCentrageV(void);								// Pose centrage vertical
	int PoseCentrageImage(void);							// Pose centrage en image
	void DemandePoseImage(void);							// Demande de prise d'une image
	void DemandePoseDoubleImage(void);						// Demande de prise d'une image en pose double
	void DemandePoseBias(void);								// Demande de prise d'un BIAS
	void DemandePoseDark(void);								// Demande de prise d'un DARK
	int PoseImage(int lumiere);								// Prise d'une image
	int PoseDoubleImage(int lumiere);						// Prise d'une image en pose doublee
	int PoseImageBias(void);								// Prise d'une image de BIAS
	int PoseImageDark(void);								// Prise d'une image de DARK

	void CalageTemporel(void);								// Fonction de calage temporel du depart d'une pose image

	// Fonction retournant la prise de temperature du CCD
	//
	double ValeurTemperatureCCD(void);
	QDateTime ValeurDateHeureTemperatureCCD(void);

	// Fonction retournant la prise de temperature du radiateur
	//
	double ValeurTemperatureRadiateur(void);
	QDateTime ValeurDateHeureTemperatureRadiateur(void);

	// Fonction retournant la prise de puissance electrique des etages de Peltier
	//
	double ValeurPuissancePeltier(void);
	QDateTime ValeurDateHeurePuissancePeltier(void);
};


// Classe de definition du thread du serveur chiffre monoclient d'attente des commandes par le reseau
//
class ProcessusLegerServeurCommandes : public QThread
{
private:
	int DrapeauDemandeTerminaison;	// Variable drapeau de demande de terminaison propre du processus

public:
	CamerOA *FPCamerOA;									// Pointeur sur la fenetre principale de l'application
	ProcessusLegerServeurDonnees *threadCanalDonnees;	// Pointeur sur le thread du canal des donnees
	ProcessusLegerControleCamera *threadCamera;			// Pointeur sur le thread de controle de la camera
	
	// Pointeur vers un objet Serveur chiffre mono client
	//
	PointCommServeurNonChiffreMonoClient *Serveur;
	
	// Constructeur du thread
	//
	ProcessusLegerServeurCommandes(CamerOA *papp,uint32_t pAdresse,uint16_t pPort,uint32_t pAdresseClient,int pNbLClientsMax,int pTimeoutSocketPut,int pTimeoutSocketGet,int pTimeoutNegoTLSSSL,void (*pFnHandlerSIGPIPE)(int),const char *MdpClePriveeServeur,char *BuffStockMdpClePriveeServeur,int (*pFnMotDePasseClePriveeChiffree)(char*, int, int, void*),const char *pCheminCertificatCA,const char *pCheminCertificatServeur,const char *pCheminClePriveeServeur,const char *pParametresDH,const char *pListeChiffreurs);
	
	// Destructeur du thread
	//
	virtual ~ProcessusLegerServeurCommandes();
	
	virtual void run();						// Surcharge de la methode run() qui contient le code d'execution du thread
	
	void DemandeTerminaison(void);			// Fonction de positionnement de la demande de terminaison propre du processus leger
};


// Classe de definition du thread du serveur non chiffre monoclient d'attente/emission des donnees par le reseau
//
class ProcessusLegerServeurDonnees : public QThread
{
private:
	int DrapeauDemandeTerminaison;	// Variable drapeau de demande de terminaison propre du processus

protected:

public:
	CamerOA *FPCamerOA;				// Pointeur sur la fenetre principale de l'application
	
	// Pointeur vers un objet Serveur non chiffre mono client
	//
	PointCommServeurNonChiffreMonoClient *Serveur;
	
	// Constructeur du thread
	//
	ProcessusLegerServeurDonnees(CamerOA *papp,uint32_t pAdresse,uint16_t pPort,uint32_t pAdresseClient,int pNbLClientsMax,int pTimeoutSocketPut,int pTimeoutSocketGet);
	
	// Destructeur du thread
	//
	virtual ~ProcessusLegerServeurDonnees();
	
	virtual void run();				// Surcharge de la methode run() qui contient le code d'execution du thread
	
	void DemandeTerminaison(void);	// Fonction de positionnement de la demande de terminaison propre du processus leger
};


// Classe de la fenetre principale de l'application CamerOA
//
class CamerOA : public KMainWindow
{
    Q_OBJECT

protected slots:
	void paintEvent(QPaintEvent *event);			// Surcharge du slot herite de QWidget
	void SlotPulsar1s(void);						// Slot pour le signal timeout() du QTimer Pulsar1s de pulsation de la seconde

public slots:
	void SlotBoutonFanModeOff(void);				// Slot pour ventilateur camera off
	void SlotBoutonFanModeLow(void);				// Slot pour ventilateur camera low
	void SlotBoutonFanModeMedium(void);				// Slot pour ventilateur camera medium
	void SlotBoutonFanModeHigh(void);				// Slot pour ventilateur camera high
	void SlotSpinBoxCTEMP(int value);				// Slot pour le changement de la valeur de la spinbox de la consigne de temperature
	void SlotBoutonForceShutterOpen(void);			// Slot pour forcer l'ouverture de l'obturateur
	void SlotBoutonForceShutterClose(void);			// Slot pour forcer la fermeture de l'obturateur
	void SlotBoutonUtiliserShutterCentrage(void);	// Slot pour utiliser l'obturateur durant le centrage
	void SlotBoutonNonUtiliserShutterCentrage(void);// Slot pour ne pas utiliser l'obturateur durant le centrage
	void SlotSpinBoxTempsPoseImage(int value);		// Slot pour le changement de la valeur de la spinbox du temps de pose image en 1/10000 de s
	void SlotBoutonPoseImage(void);					// Slot pour declancher une pose image
	void SlotBoutonPoseDoubleImage(void);			// Slot pour declancher une pose image en pose doublee
	void SlotBoutonPoseBias(void);					// Slot pour declancher une pose de BIAS
	void SlotBoutonPoseDark(void);					// Slot pour declancher une pose de DARK
	void SlotBoutonStopPoseImage(void);				// Slot pour declancher l'arret d'une pose image
	void SlotSpinBoxTempsPoseCentrage(int value);	// Slot pour le changement de la valeur de la spinbox du temps de pose pour le centrage en 1/10000s
	void SlotSpinBoxTempsPoseDark(int value);		// Slot pour le changement de la valeur de la spinbox du temps de pose d'un DARK

protected:
	void AfficherHeureUT(void);					// Fonction pour afficher l'heure UT dans le QLabel de la barre de status

	unsigned long CompteurRafraichissement;		// Compteur pour sequencer les rafraichissements

	QString CheminRepCamerOA;					// Chemin vers le repertoire de base du CamerOA

	int ConsigneTemperature;					// Consigne de la temperature

	int TempsPoseCentrage;						// Temps de pose pour le centrage en 1/10000 de s

	int TempsPoseImage;							// Temps de pose pour prise image en 1/10000 de s

	int TempsPoseDark;							// Temps de pose pour prise DARK en 1/10000 de s

	void customEvent(QCustomEvent *ce);			// Surcharge de la fonction de handler des evenements particuliers crees pour CamerOA


public:
	// Constructeur non typable mais avec des argument(s)
	CamerOA(QString p_chemRepCamerOA,KApplication *p_appli,ProcessusLegerControleCamera *p_ccam);
	
	virtual ~CamerOA();							// Destructeur non typable
	
	KApplication *appli;						// Pointeur vers application KApplication parent de l'objet

	ProcessusLegerControleCamera *PLCamera;		// Pointeur vers processus leger de controle de la camera
	ProcessusLegerServeurCommandes *PLServeurCommandes;	// Pointeur vers le processus leger de serveur des commandes
	ProcessusLegerServeurDonnees *PLServeurDonnees;		// Pointeur vers le processus leger de serveur des donnees

	ObjPoseImage *ObjAffPoseImage;				// Objet d'affichage de la pose image
	ObjPoseCentrage *ObjAffPoseCentrage;		// Objet d'affichage de la pose centrage

	QString CheminRepertoireCamerOA(void);		// Retourne le chemin vers le repertoire de base du CamerOA
	
	QVBox *BoiteRangementVertical;				// Widget de rangement vertical des widgets enfants
	QHBox *BoiteRangementHorizontal;			// Widget de rangement horizontal des widgets enfants

	QStatusBar *BarreStatus;					// Widget de barre horizontale pour presenter des informations de status
	QLabel *LabelHeureUTBarreStatus;			// Widget label ajoute a la barre de status d'affichage de l'heure UT
	QLabel *LabelTempCCD;						// Widget label ajoute a la barre de status d'affichage de la temperature du CCD
	QLabel *LabelTempRad;						// Widget label ajoute a la barre de status d'affichage de la temperature du radiateur
	QLabel *LabelPuissance;						// Widget label ajoute a la barre de status d'affichage de la puissance electrique des etages Peltier
	QTimer *Pulsar1s;							// Timer de pulsation de la seconde de temps
	
	QPushButton *BoutonFanModeOff;				// Bouton pour passer en mode ventilateur off
	QPushButton *BoutonFanModeLow;				// Bouton pour passer en mode ventilateur low
	QPushButton *BoutonFanModeMedium;			// Bouton pour passer en mode ventilateur medium
	QPushButton *BoutonFanModeHigh;				// Bouton pour passer en mode ventilateur high
	QPushButton *BoutonForceShutterOpen;		// Bouton pour passer en mode obturateur ouvert
	QPushButton *BoutonForceShutterClose;		// Bouton pour passer en mode obturateur ferme
	QPushButton *BoutonUtiliserShutterCentrage;	// Bouton pour utiliser l'obturateur pour le centrage
	QPushButton *BoutonNonUtiliserShutterCentrage;	// Bouton pour ne pas utiliser l'obturateur pour le centrage
	QSpinBox *SpinBoxCTEMP;						// SpinBox d'entree de la consigne de temperature
	QSpinBox *SpinBoxTempsPoseCentrage;			// SpinBox d'entree du temps de pose pour le centrage en 1/10000s
	QPushButton *BoutonPoseImage;				// Bouton pour declanchement d'une pose image
	QPushButton *BoutonPoseDoubleImage;			// Bouton pour declanchement d'une pose image en pose doublee
	QPushButton *BoutonPoseBias;				// Bouton pour declanchement d'une pose BIAS
	QPushButton *BoutonPoseDark;				// Bouton pour declanchement d'une pose DARK
	QPushButton *BoutonStopPoseImage;			// Bouton pour arret d'une pose image
	QSpinBox *SpinBoxTempsPoseImage;			// SpinBox d'entree du temps de pose pour image en 1/10000s
	QSpinBox *SpinBoxTempsPoseDark;				// SpinBox d'entree du temps de pose pour un DARK en 1/10000s

	// Chargement des consignes sauvegardees
	//
	int ChargeConsignes(void);

	// Fonction de parametrage de la consigne de la temperature
	//
	void ParamConsigneTemperature(int valeur);
	
	// Fonction retournant la consigne de temperature
	//
	int ValeurConsigneTemperature(void);

	// Fonction de parametrage du temps de pose pour le centrage en 1/10000s
	//
	void ParamTempsPoseCentrage(int valeur);
	
	// Fonction retournant le temps de pose pour le centrage en 1/10000s
	//
	int ValeurTempsPoseCentrage(void);

	// Fonction de parametrage du temps de pose pour image en 1/10000s
	//
	void ParamTempsPoseImage(int valeur);
	
	// Fonction retournant le temps de pose pour image en 1/10000s
	//
	int ValeurTempsPoseImage(void);

	// Fonction de parametrage du temps de pose pour le DARK en 1/10000s
	//
	void ParamTempsPoseDark(int valeur);
	
	// Fonction retournant le temps de pose pour le DARK en 1/10000s
	//
	int ValeurTempsPoseDark(void);

	// Fonction de rafraichissement de la temperature du CCD par interrogation de la camera
	//
	void RafraichissementTemperatureCCD(void);

	// Fonction de rafraichissement de la temperature du radiateur par interrogation de la camera
	//
	void RafraichissementTemperatureRadiateur(void);

	// Fonction de rafraichissement de la puissance electrique des etages de Peltier
	//
	void RafraichissementPuissancePeltier(void);
};



// Evenements associes a la modification de widgets de la fenetre principale par d'autres processus legers
//
class CEventCamerOA_ChargeAffEnvImgObjetCentrage : public QCustomEvent		// Charger Afficher une nouvelle image dans l'objet graphique, Envoyer au client si connecte
{
	private:
		unsigned short tx;
		unsigned short ty;
		unsigned short *pt;
		QDateTime debut;
		QDateTime fin;
		int duree;

	public:
		CEventCamerOA_ChargeAffEnvImgObjetCentrage(unsigned short ptx,unsigned short pty,unsigned short *ppt,QDateTime pdebut,QDateTime pfin,int pduree) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGECENTRAGE)
		{
			tx=ptx;	ty=pty;	pt=ppt; debut=pdebut; fin=pfin; duree=pduree;
		}

		unsigned short DimX(void)		{	return tx;	}
		unsigned short DimY(void)		{	return ty;	}
		unsigned short *Donnees(void)	{	return pt;	}
		QDateTime Debut(void)			{	return debut; }
		QDateTime Fin(void)				{	return fin;	 }
		int Duree(void)					{	return duree; }
};

class CEventCamerOA_ChargeAffEnvImg_H_ObjetCentrage : public QCustomEvent	// Charger Afficher une nouvelle image horizontale dans l'objet graphique, Envoyer au client si connecte
{
	private:
		unsigned short tx;
		unsigned short ty;
		unsigned short *pt;
		QDateTime debut;
		QDateTime fin;
		int duree;

	public:
		CEventCamerOA_ChargeAffEnvImg_H_ObjetCentrage(unsigned short ptx,unsigned short pty,unsigned short *ppt,QDateTime pdebut,QDateTime pfin,int pduree) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_H_CENTRAGE)
		{
			tx=ptx;	ty=pty;	pt=ppt; debut=pdebut; fin=pfin; duree=pduree;
		}

		unsigned short DimX(void)		{	return tx;	}
		unsigned short DimY(void)		{	return ty;	}
		unsigned short *Donnees(void)	{	return pt;	}
		QDateTime Debut(void)			{	return debut; }
		QDateTime Fin(void)				{	return fin;	 }
		int Duree(void)					{	return duree; }
};

class CEventCamerOA_ChargeAffEnvImg_V_ObjetCentrage : public QCustomEvent	// Charger Afficher une nouvelle image verticale dans l'objet graphique, Envoyer au client si connecte
{
	private:
		unsigned short tx;
		unsigned short ty;
		unsigned short *pt;
		QDateTime debut;
		QDateTime fin;
		int duree;

	public:
		CEventCamerOA_ChargeAffEnvImg_V_ObjetCentrage(unsigned short ptx,unsigned short pty,unsigned short *ppt,QDateTime pdebut,QDateTime pfin,int pduree) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_V_CENTRAGE)
		{
			tx=ptx;	ty=pty;	pt=ppt; debut=pdebut; fin=pfin; duree=pduree;
		}

		unsigned short DimX(void)		{	return tx;	}
		unsigned short DimY(void)		{	return ty;	}
		unsigned short *Donnees(void)	{	return pt;	}
		QDateTime Debut(void)			{	return debut; }
		QDateTime Fin(void)				{	return fin;	 }
		int Duree(void)					{	return duree; }
};

class CEventCamerOA_ChargeAffEnvImgObjetImage : public QCustomEvent		// Charger Afficher une nouvelle image dans l'objet graphique, Envoyer au client si connecte
{
	private:
		int type;
		unsigned short tx;
		unsigned short ty;
		unsigned short *pt;
		QDateTime debut;
		QDateTime fin;
		int duree;

	public:
		CEventCamerOA_ChargeAffEnvImgObjetImage(int ptype,unsigned short ptx,unsigned short pty,unsigned short *ppt,QDateTime pdebut,QDateTime pfin,int pduree) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE)
		{
			type=ptype; tx=ptx;	ty=pty;	pt=ppt; debut=pdebut; fin=pfin; duree=pduree;
		}

		int Type(void)					{	return type; }
		unsigned short DimX(void)		{	return tx;	}
		unsigned short DimY(void)		{	return ty;	}
		unsigned short *Donnees(void)	{	return pt;	}
		QDateTime Debut(void)			{	return debut; }
		QDateTime Fin(void)				{	return fin;	 }
		int Duree(void)					{	return duree; }
};

class  CEventCamerOA_ParamAmpliProfilH: public QCustomEvent		// Parametrer le facteur d'amplification du profil horizontal
{
	private:
		long tm;
		double lz;
		double fa;

	public:
		CEventCamerOA_ParamAmpliProfilH(long ptm,double plz,double pfa) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_PARAM_AMPLI_PROFH)
		{
			tm=ptm;	lz=plz;	fa=pfa;
		}

		long DimMatrice(void)			{	return tm;	}
		double LargeurZone(void)		{	return lz;	}
		double FacteurAmpli(void)		{	return fa;	}
};

class  CEventCamerOA_ParamAmpliProfilV: public QCustomEvent		// Parametrer le facteur d'amplification du profil vertical
{
	private:
		long tm;
		double lz;
		double fa;

	public:
		CEventCamerOA_ParamAmpliProfilV(long ptm,double plz,double pfa) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_PARAM_AMPLI_PROFV)
		{
			tm=ptm;	lz=plz;	fa=pfa;
		}

		long DimMatrice(void)			{	return tm;	}
		double LargeurZone(void)		{	return lz;	}
		double FacteurAmpli(void)		{	return fa;	}
};

class CEventCamerOA_Quit : public QCustomEvent			// Demande de sortie du programme
{
	public:
		CEventCamerOA_Quit(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_QUIT) { }
};

class CEventCamerOA_CTemp : public QCustomEvent			// Nouvelle consigne de temperature
{
	private:
		int valeur;										// Valeur de la consigne

	public:
		CEventCamerOA_CTemp(int c) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_CTEMP)
		{
			valeur=c;
		}

		int Valeur(void) { return valeur; }				// Retourne la valeur de la consigne
};

class CEventCamerOA_CTPI : public QCustomEvent			// Nouvelle consigne de temps de pose image
{
	private:
		int valeur;										// Valeur de la consigne

	public:
		CEventCamerOA_CTPI(int c) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_CTPI)
		{
			valeur=c;
		}

		int Valeur(void) { return valeur; }				// Retourne la valeur de la consigne
};

class CEventCamerOA_CTPC : public QCustomEvent			// Nouvelle consigne de temps de pose centrage
{
	private:
		int valeur;										// Valeur de la consigne

	public:
		CEventCamerOA_CTPC(int c) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_CTPC)
		{
			valeur=c;
		}

		int Valeur(void) { return valeur; }				// Retourne la valeur de la consigne
};

class CEventCamerOA_CTPD : public QCustomEvent			// Nouvelle consigne de temps de pose d'un DARK
{
	private:
		int valeur;										// Valeur de la consigne

	public:
		CEventCamerOA_CTPD(int c) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_CTPD)
		{
			valeur=c;
		}

		int Valeur(void) { return valeur; }				// Retourne la valeur de la consigne
};

class CEventCamerOA_PoseImage : public QCustomEvent			// Demande de pose image
{
	public:
		CEventCamerOA_PoseImage(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_POSEI) { }
};

class CEventCamerOA_PoseDoubleImage : public QCustomEvent		// Demande de pose double image
{
	public:
		CEventCamerOA_PoseDoubleImage(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_POSEDOUBLEI) { }
};

class CEventCamerOA_PoseBias : public QCustomEvent				// Demande de pose BIAS
{
	public:
		CEventCamerOA_PoseBias(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_POSEBIAS) { }
};

class CEventCamerOA_PoseDark : public QCustomEvent				// Demande de pose DARK
{
	public:
		CEventCamerOA_PoseDark(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_POSEDARK) { }
};

class CEventCamerOA_ArretPose : public QCustomEvent			// Demande de l'arret d'une pose
{
	public:
		CEventCamerOA_ArretPose(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_ARRETPOSE) { }
};

class CEventCamerOA_ForceShutterOpen : public QCustomEvent	// Demande d'obturateur ouvert
{
	public:
		CEventCamerOA_ForceShutterOpen(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_FSHUTTEROPEN) { }
};

class CEventCamerOA_ForceShutterClose : public QCustomEvent	// Demande d'obturateur ferme
{
	public:
		CEventCamerOA_ForceShutterClose(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_FSHUTTERCLOSE) { }
};

class CEventCamerOA_UtiliserShutterCentrage : public QCustomEvent	// Demande d'utilisation du shutter durant une pose de centrage
{
	public:
		CEventCamerOA_UtiliserShutterCentrage(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_UTILSHUTCENT) { }
};

class CEventCamerOA_NonUtiliserShutterCentrage : public QCustomEvent	// Demande de non utilisation du shutter durant une pose de centrage
{
	public:
		CEventCamerOA_NonUtiliserShutterCentrage(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_NONUTILSHUTCENT) { }
};

class CEventCamerOA_FanLow : public QCustomEvent			// Demande de ventilateur low
{
	public:
		CEventCamerOA_FanLow(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_FANLOW) { }
};

class CEventCamerOA_FanMedium : public QCustomEvent			// Demande de ventilateur medium
{
	public:
		CEventCamerOA_FanMedium(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_FANMEDIUM) { }
};

class CEventCamerOA_FanHigh : public QCustomEvent			// Demande de ventilateur high
{
	public:
		CEventCamerOA_FanHigh(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_FANHIGH) { }
};

class CEventCamerOA_FanOff : public QCustomEvent			// Demande d'arret ventilateur
{
	public:
		CEventCamerOA_FanOff(void) : QCustomEvent(ID_CUSTOM_EVENT_CAMEROA_FANOFF) { }
};

#endif // _CAMEROA_H_
