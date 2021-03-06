/* MODULE DE LA CLASSE DE LA FENETRE PRINCIPALE DE L'APPLICATION CamerOA

   LOGICIEL RESEAU DE CONTROLE DE CAMERA CCD

   (C)David.Romeuf@univ-lyon1.fr 09/05/2006 par David Romeuf
*/

// Inclusion C
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

// Inclusions C++
//
#include <iostream>
#include <new>
#include <memory>
#include <valarray>
#include <cmath>

using namespace std;

// Inclusions KDE et Qt
//
#include <kapplication.h>
#include <kmainwindow.h>
#include <klocale.h>
#include <qapplication.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qdialog.h>
#include <qfile.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qmutex.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qsemaphore.h>
#include <qsizepolicy.h>
#include <qspinbox.h>
#include <qstatusbar.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qtimer.h>

// Inclusion des icons dans le code : ATTENTION: ils doivent etre declares en "static const char *" (const important)
//  pour ne pas generer des warnings de compilation avec gcc
//
#include <IconFanModeHigh.xpm>
#include <IconFanModeLow.xpm>
#include <IconFanModeMedium.xpm>
#include <IconFanModeOff.xpm>
#include <IconForceShutterClose.xpm>
#include <IconForceShutterOpen.xpm>
#include <IconNonUtilisationShutterCentrage.xpm>
#include <IconPoseBias.xpm>
#include <IconPoseDark.xpm>
#include <IconPoseDoubleImage.xpm>
#include <IconPoseImage.xpm>
#include <IconStopPoseImage.xpm>
#include <IconUtilisationShutterCentrage.xpm>

// Inclusions CamerOA
//
#include "cameroa.h"
#include "ListesCommandesReponsesCamerOA.h"
#include "WidgetPoseImage.h"
#include "WidgetPoseCentrage.h"


// Inclusions pour la camera
//
#include "Apogee.h"
#include "ApnCamera.h"
#include "SimulateurApogee.h"

// L'objet camera CCD
//
#if not defined(_SIMULATEUR_APOGEE)
extern CApnCamera *CameraCCD;
#else
extern SimulateurApogeeUSB *CameraCCD;
#endif
// Pointeur sur le buffer pixels physiques du capteur (physique et pas image) d'une pose image (image, BIAS, DARK)
//
extern unsigned short *BufferPixelsPhysiquesImage;
extern QMutex MutexBufferPixelsPhysiquesImage;

// Pointeur sur le buffer pixels physiques du capteur (physique et pas image) de vidage du CCD pour differencier la zone memoire
//
extern unsigned short *BufferPixelsPhysiquesVidage;
extern QMutex MutexBufferPixelsPhysiquesVidage;

// Pointeur sur le buffer pixels physiques du capteur (physique et pas image) d'une pose de centrage
//
extern unsigned short *BufferPixelsPhysiquesCentrage;
extern QMutex MutexBufferPixelsPhysiquesCentrage;

// Pointeur sur le buffer pixels d'une pose de centrage horizontal
//
extern unsigned short *BufferPixelsPCH;
extern QMutex MutexBufferPixelsPCH;

// Pointeur sur le buffer pixels d'une pose de centrage vertical
//
extern unsigned short *BufferPixelsPCV;
extern QMutex MutexBufferPixelsPCV;


// Les semaphores et mutex pour synchroniser les processus legers, proteger leurs donnees
//
extern QSemaphore SemaphoreSyncLancementThreadCamerOA;	// Synchronisation du lancement des threads de CamerOA


// Constructeur de la classe de la fenetre principale de l'application CamerOA
//
// KMainWindow est un widget de haut niveau qui permet de gerer la geometrie de la fenetre principale et des widgets enfants 
//
CamerOA::CamerOA(QString p_chemRepCamerOA,KApplication *p_appli,ProcessusLegerControleCamera *p_ccam) : KMainWindow( 0, "CamerOA-KMainWindow" )
{
	// Initialisation des variables
	//
	CheminRepCamerOA=p_chemRepCamerOA;

	appli=p_appli;
	PLCamera=p_ccam;

	CompteurRafraichissement=0;

	// Creation du widget de la boite de rangement vertical
	//
	if( (BoiteRangementVertical=new (std::nothrow) QVBox(this,"CamerOA-KMainWindow-BoiteRangementVertical")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QVBox:BoiteRangementVertical de la fenetre principale KMainWindow." << std::endl;
	}

	// Creation du widget de la boite de rangement horizontal
	//
	if( (BoiteRangementHorizontal=new (std::nothrow) QHBox(BoiteRangementVertical,"CamerOA-KMainWindow-BoiteRangementHorizontal")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QVBox:BoiteRangementHorizontal de la fenetre principale KMainWindow." << std::endl;
	}

	// Creation du widget d'affichage de la pose image
	//
	if( (ObjAffPoseImage=new (std::nothrow) ObjPoseImage(BoiteRangementHorizontal,"CamerOA-KMainWindow-ObjAffPoseImage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget ObjPoseImage:ObjAffPoseImage de la fenetre principale KMainWindow." << std::endl;
	}
	ObjAffPoseImage->setMinimumSize(TAILLE_X_OBJPOSEIMAGE_RECOMMANDEE,TAILLE_Y_OBJPOSEIMAGE_RECOMMANDEE);
	ObjAffPoseImage->setMaximumSize(TAILLE_X_OBJPOSEIMAGE_RECOMMANDEE,TAILLE_Y_OBJPOSEIMAGE_RECOMMANDEE);

	// Creation du widget d'affichage des poses de centrage
	//
	if( (ObjAffPoseCentrage=new (std::nothrow) ObjPoseCentrage(BoiteRangementHorizontal,"CamerOA-KMainWindow-ObjAffPoseCentrage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget ObjPoseImage:ObjAffPoseCentrage de la fenetre principale KMainWindow." << std::endl;
	}
	ObjAffPoseCentrage->FixeMaxRouge(MAX_COMPOSANTES_RVB_OBJ_CENTRAGE);
	ObjAffPoseCentrage->FixeMaxVert(MAX_COMPOSANTES_RVB_OBJ_CENTRAGE);
	ObjAffPoseCentrage->FixeMaxBleu(MAX_COMPOSANTES_RVB_OBJ_CENTRAGE);
	ObjAffPoseCentrage->TypePalette(NoirEtBlanc);
	ObjAffPoseCentrage->setMinimumSize(TAILLE_X_OBJPOSECENTRAGE_RECOMMANDEE,TAILLE_Y_OBJPOSECENTRAGE_RECOMMANDEE);
	ObjAffPoseCentrage->setMaximumSize(TAILLE_X_OBJPOSECENTRAGE_RECOMMANDEE,TAILLE_Y_OBJPOSECENTRAGE_RECOMMANDEE);
	ObjAffPoseCentrage->ParamAmplificationProfilH(MIN_MATRICE_AMP_PROF,MIN_LARG_ZA_PROF,MIN_AMPLI_PROF);
	ObjAffPoseCentrage->ParamAmplificationProfilV(MIN_MATRICE_AMP_PROF,MIN_LARG_ZA_PROF,MIN_AMPLI_PROF);
	
	// Creation du widget de barre horizontale pour presenter des informations de status
	//
	if( (BarreStatus=new (std::nothrow) QStatusBar(BoiteRangementVertical,"CamerOA-KMainWindow-BarreStatus")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QStatusBar:BarreStatus de la fenetre principale KMainWindow." << std::endl;
	}

	// La barre de status a une dimension verticale fixe (celle de sa creation)
	//
	BarreStatus->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));

	// Creation du widget label d'affichage de l'heure UT ajoute a la barre de status
	//
	if( (LabelHeureUTBarreStatus=new (std::nothrow) QLabel("00:00:00 UT",this,"CamerOA-KMainWindow-LabelHeureUTBarreStatus")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QLabel:LabelHeureUTBarreStatus de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	LabelHeureUTBarreStatus->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));

	// Creation du widget label d'affichage de la temperature du CCD
	//
	if( (LabelTempCCD=new (std::nothrow) QLabel("C:-00.00",this,"CamerOA-KMainWindow-LabelTempCCD")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QLabel:LabelTempCCD de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	LabelTempCCD->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));

	// Creation du widget label d'affichage de la temperature du radiateur
	//
	if( (LabelTempRad=new (std::nothrow) QLabel("R:-00.00",this,"CamerOA-KMainWindow-LabelTempRad")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QLabel:LabelTempRad de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	LabelTempRad->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));

	// Creation du widget label d'affichage de la puissance electrique de l'etage Peltier
	//
	if( (LabelPuissance=new (std::nothrow) QLabel("P:000.0%",this,"CamerOA-KMainWindow-LabelPuissance")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QLabel:LabelPuissance de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	LabelPuissance->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));

	// Creation du widget bouton poussoir de passage en mode ventilateur off
	//
	if( (BoutonFanModeOff=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconFanModeOff_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonFanModeOff")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonFanModeOff de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonFanModeOff->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonFanModeOff->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de passage en mode ventilateur low
	//
	if( (BoutonFanModeLow=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconFanModeLow_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonFanModeLow")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonFanModeLow de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonFanModeLow->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonFanModeLow->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de passage en mode ventilateur medium
	//
	if( (BoutonFanModeMedium=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconFanModeMedium_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonFanModeMedium")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonFanModeMedium de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonFanModeMedium->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonFanModeMedium->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de passage en mode ventilateur high
	//
	if( (BoutonFanModeHigh=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconFanModeHigh_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonFanModeHigh")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonFanModeHigh de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonFanModeHigh->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonFanModeHigh->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de forcage de l'ouverture de l'obturateur
	//
	if( (BoutonForceShutterOpen=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconForceShutterOpen_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonForceShutterOpen")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonForceShutterOpen de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonForceShutterOpen->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonForceShutterOpen->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de forcage de la fermeture de l'obturateur
	//
	if( (BoutonForceShutterClose=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconForceShutterClose_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonForceShutterClose")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonForceShutterClose de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonForceShutterClose->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonForceShutterClose->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de declanchement d'une pose image
	//
	if( (BoutonPoseImage=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconPoseImage_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonPoseImage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonPoseImage de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonPoseImage->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonPoseImage->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de declanchement d'une pose image en pose doublee
	//
	if( (BoutonPoseDoubleImage=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconPoseDoubleImage_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonPoseDoubleImage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonPoseDoubleImage de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonPoseDoubleImage->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonPoseDoubleImage->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de declanchement d'une pose de BIAS
	//
	if( (BoutonPoseBias=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconPoseBias_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonPoseBias")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonPoseBias de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonPoseBias->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonPoseBias->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de declanchement d'une pose de DARK
	//
	if( (BoutonPoseDark=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconPoseDark_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonPoseDark")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonPoseDark de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonPoseDark->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonPoseDark->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir pour utiliser l'obturateur pour le centrage
	//
	if( (BoutonUtiliserShutterCentrage=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconUtilisationShutterCentrage_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonUtiliserShutterCentrage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonUtiliserShutterCentrage de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonUtiliserShutterCentrage->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonUtiliserShutterCentrage->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir pour ne pas utiliser l'obturateur pour le centrage
	//
	if( (BoutonNonUtiliserShutterCentrage=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconNonUtilisationShutterCentrage_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonNonUtiliserShutterCentrage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonNonUtiliserShutterCentrage de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonNonUtiliserShutterCentrage->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonNonUtiliserShutterCentrage->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Creation du widget bouton poussoir de d'arret d'une pose image
	//
	if( (BoutonStopPoseImage=new (std::nothrow) QPushButton(QIconSet(QPixmap(IconStopPoseImage_xpm),QIconSet::Automatic),"",this,"CamerOA-KMainWindow-BoutonStopPoseImage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QPushButton:BoutonStopPoseImage de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	BoutonStopPoseImage->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum));
	BoutonStopPoseImage->setMaximumSize(TAILLE_BOUTON_MAXI_X,TAILLE_BOUTON_MAXI_Y);

	// Connexion du signal clicked() envoye par les boutons aux slots respectifs SlotBouton_() de l'objet
	//
	connect(BoutonFanModeOff,SIGNAL(clicked()),this,SLOT(SlotBoutonFanModeOff()));
	connect(BoutonFanModeLow,SIGNAL(clicked()),this,SLOT(SlotBoutonFanModeLow()));
	connect(BoutonFanModeMedium,SIGNAL(clicked()),this,SLOT(SlotBoutonFanModeMedium()));
	connect(BoutonFanModeHigh,SIGNAL(clicked()),this,SLOT(SlotBoutonFanModeHigh()));
	connect(BoutonForceShutterOpen,SIGNAL(clicked()),this,SLOT(SlotBoutonForceShutterOpen()));
	connect(BoutonForceShutterClose,SIGNAL(clicked()),this,SLOT(SlotBoutonForceShutterClose()));
	connect(BoutonUtiliserShutterCentrage,SIGNAL(clicked()),this,SLOT(SlotBoutonUtiliserShutterCentrage()));
	connect(BoutonNonUtiliserShutterCentrage,SIGNAL(clicked()),this,SLOT(SlotBoutonNonUtiliserShutterCentrage()));
	connect(BoutonPoseImage,SIGNAL(clicked()),this,SLOT(SlotBoutonPoseImage()));
	connect(BoutonPoseDoubleImage,SIGNAL(clicked()),this,SLOT(SlotBoutonPoseDoubleImage()));
	connect(BoutonPoseBias,SIGNAL(clicked()),this,SLOT(SlotBoutonPoseBias()));
	connect(BoutonPoseDark,SIGNAL(clicked()),this,SLOT(SlotBoutonPoseDark()));
	connect(BoutonStopPoseImage,SIGNAL(clicked()),this,SLOT(SlotBoutonStopPoseImage()));

	// Creation du widget d'entree SpinBox de la consigne de la temperature ajoute a la barre de status
	//
	if( (SpinBoxCTEMP=new (std::nothrow) QSpinBox(MIN_CTEMP,MAX_CTEMP,1,this,"CamerOA-KMainWindow-SpinBoxCTEMP")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QSpinBox:SpinBoxCTEMP de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	SpinBoxCTEMP->setPrefix("t ");
	//SpinBoxCTEMP->setLineStep(1);

	// Creation du widget d'entree SpinBox du temps de pose pour le centrage en 1/10000s
	//
	if( (SpinBoxTempsPoseCentrage=new (std::nothrow) QSpinBox(MIN_TPCENTRAGE,MAX_TPCENTRAGE,10,this,"CamerOA-KMainWindow-SpinBoxTempsPoseCentrage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QSpinBox:SpinBoxTempsPoseCentrage de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	SpinBoxTempsPoseCentrage->setPrefix("C ");
	//SpinBoxTempsPoseCentrage->setLineStep(10);

	// Creation du widget d'entree SpinBox du temps de pose pour image en 1/10000s
	//
	if( (SpinBoxTempsPoseImage=new (std::nothrow) QSpinBox(MIN_TPIMAGE,MAX_TPIMAGE,100,this,"CamerOA-KMainWindow-SpinBoxTempsPoseImage")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QSpinBox:SpinBoxTempsPoseImage de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	SpinBoxTempsPoseImage->setPrefix("I ");
	//SpinBoxTempsPoseImage->setLineStep(100);

	// Creation du widget d'entree SpinBox du temps de pose pour un DARK en 1/10000s
	//
	if( (SpinBoxTempsPoseDark=new (std::nothrow) QSpinBox(MIN_TPIMAGE,MAX_TPIMAGE,100,this,"CamerOA-KMainWindow-SpinBoxTempsPoseDark")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le widget QSpinBox:SpinBoxTempsPoseDark de la barre de status de la fenetre principale KMainWindow." << std::endl;
	}
	SpinBoxTempsPoseDark->setPrefix("D ");
	//SpinBoxTempsPoseDark->setLineStep(100);

	// On ajoute les widgets a la barre de status
	//
	BarreStatus->addWidget(SpinBoxTempsPoseCentrage,0,TRUE);
	BarreStatus->addWidget(BoutonUtiliserShutterCentrage,0,TRUE);
	BarreStatus->addWidget(BoutonNonUtiliserShutterCentrage,0,TRUE);
	BarreStatus->addWidget(BoutonPoseImage,0,TRUE);
	BarreStatus->addWidget(BoutonPoseDoubleImage,0,TRUE);
	BarreStatus->addWidget(SpinBoxTempsPoseImage,0,TRUE);
	BarreStatus->addWidget(BoutonStopPoseImage,0,TRUE);
	BarreStatus->addWidget(BoutonPoseBias,0,TRUE);
	BarreStatus->addWidget(BoutonPoseDark,0,TRUE);
	BarreStatus->addWidget(SpinBoxTempsPoseDark,0,TRUE);
	BarreStatus->addWidget(BoutonForceShutterOpen,0,TRUE);
	BarreStatus->addWidget(BoutonForceShutterClose,0,TRUE);
	BarreStatus->addWidget(BoutonFanModeLow,0,TRUE);
	BarreStatus->addWidget(BoutonFanModeMedium,0,TRUE);
	BarreStatus->addWidget(BoutonFanModeHigh,0,TRUE);
	BarreStatus->addWidget(BoutonFanModeOff,0,TRUE);
	BarreStatus->addWidget(SpinBoxCTEMP,0,TRUE);
	BarreStatus->addWidget(LabelTempCCD,0,TRUE);
	BarreStatus->addWidget(LabelTempRad,0,TRUE);
	BarreStatus->addWidget(LabelPuissance,0,TRUE);
	BarreStatus->addWidget(LabelHeureUTBarreStatus,0,TRUE);

	// Creation du timer de pulsation de la seconde de temps
	//
	if( (Pulsar1s=new (std::nothrow) QTimer(this,"CamerOA-KMainWindow-Pulsar1s")) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible de creer le timer QTimer:Pulsar1s de la fenetre principale KMainWindow." << std::endl;
	}

	// Connexion du signal timeout() envoye par le timer au slot SlotPulsar1s() de l'objet
	//
	connect(Pulsar1s,SIGNAL(timeout()),this,SLOT(SlotPulsar1s()));

	// Connexion du signal valueChanged() envoye par les QSpinBox aux slots respectifs SlotSpinBoxVC_() de l'objet
	//
	connect(SpinBoxCTEMP,SIGNAL(valueChanged(int)),this,SLOT(SlotSpinBoxCTEMP(int)));
	connect(SpinBoxTempsPoseCentrage,SIGNAL(valueChanged(int)),this,SLOT(SlotSpinBoxTempsPoseCentrage(int)));
	connect(SpinBoxTempsPoseImage,SIGNAL(valueChanged(int)),this,SLOT(SlotSpinBoxTempsPoseImage(int)));
	connect(SpinBoxTempsPoseDark,SIGNAL(valueChanged(int)),this,SLOT(SlotSpinBoxTempsPoseDark(int)));
}


// Destructeur de l'objet de la fenetre principale
//
CamerOA::~CamerOA()
{
	// Sauvegarde des consignes
	//
	QFile FichierConsignes(CheminRepCamerOA+"/"+FICHIER_SAUV_CONSIGNES);
	
	if( FichierConsignes.open(IO_WriteOnly) )
	{
		QTextStream FluxTexte(&FichierConsignes);	// Flux en mode texte
		QString Ligne;					// Une chaine qui sera une ligne du fichier
		
		FichierConsignes.at(0);
		
		// Sauvegarde de la consigne du seuil bas de visualisation
		//
		Ligne=QString("%1\n").arg(ValeurConsigneTemperature());
		FluxTexte << Ligne;
		
		// Sauvegarde du temps de pose pour le centrage en 1/10000s
		//
		Ligne=QString("%1\n").arg(ValeurTempsPoseCentrage());
		FluxTexte << Ligne;
		
		// Sauvegarde du temps de pose pour image en 1/10000s
		//
		Ligne=QString("%1\n").arg(ValeurTempsPoseImage());
		FluxTexte << Ligne;
		
		// Sauvegarde du temps de pose pour DARK en 1/10000s
		//
		Ligne=QString("%1\n").arg(ValeurTempsPoseDark());
		FluxTexte << Ligne;
		
		FichierConsignes.close();
	}
	
	// Arret du timer de pulsation de la seconde de temps
	//
	Pulsar1s->stop();
	
	// Destruction des objets
	//
/*
	delete BoutonPoseDark;
	delete BoutonPoseBias;
	delete BoutonPoseDoubleImage;
	delete BoutonPoseImage;
	delete BoutonStopPoseImage;
	delete BoutonFanModeOff;
	delete BoutonFanModeLow;
	delete BoutonFanModeMedium;
	delete BoutonFanModeHigh;
	delete BoutonForceShutterOpen;
	delete BoutonForceShutterClose;
	delete BoutonUtiliserShutterCentrage;
	delete BoutonNonUtiliserShutterCentrage;
	delete Pulsar1s;
	delete SpinBoxTempsPoseDark;
	delete SpinBoxTempsPoseImage;
	delete SpinBoxTempsPoseCentrage;
	delete SpinBoxCTEMP;
	delete LabelTempCCD;
	delete LabelTempRad;
	delete LabelPuissance;
	delete LabelHeureUTBarreStatus;
	delete BarreStatus;
	delete ObjAffPoseImage;
	delete ObjAffPoseCentrage;
	delete BoiteRangementHorizontal;
	delete BoiteRangementVertical;
*/
}


// Fonction de chargement des consignes sauvegardees
//
int CamerOA::ChargeConsignes(void)
{
	// Lecture des consignes sauvegardees
	//
	QFile FichierConsignes(CheminRepCamerOA+"/"+FICHIER_SAUV_CONSIGNES);

	if( FichierConsignes.open(IO_ReadOnly) )
	{
		QTextStream FluxTexte(&FichierConsignes);	// Flux en mode texte
		QString Ligne;					// Une chaine qui sera une ligne du fichier

		FichierConsignes.at(0);

		// Lecture de la consigne de temperature
		//
		Ligne=FluxTexte.readLine();

		// Programmation de la temperature de consigne
		//
		if( !Ligne.isNull() ) ParamConsigneTemperature(std::atoi(Ligne));

		// Lecture du temps de pose pour le centrage
		//
		Ligne=FluxTexte.readLine();

		// Programmation du temps de pose pour le centrage
		//
		if( !Ligne.isNull() ) ParamTempsPoseCentrage(std::atoi(Ligne));

		// Lecture du temps de pose pour image
		//
		Ligne=FluxTexte.readLine();

		// Programmation du temps de pose pour image
		//
		if( !Ligne.isNull() ) ParamTempsPoseImage(std::atoi(Ligne));

		// Lecture du temps de pose pour DARK
		//
		Ligne=FluxTexte.readLine();

		// Programmation du temps de pose pour DARK
		//
		if( !Ligne.isNull() ) ParamTempsPoseDark(std::atoi(Ligne));

		FichierConsignes.close();
	}

	SpinBoxTempsPoseCentrage->setValue(ValeurTempsPoseCentrage());
	SpinBoxTempsPoseImage->setValue(ValeurTempsPoseImage());
	SpinBoxTempsPoseDark->setValue(ValeurTempsPoseDark());
	SpinBoxCTEMP->setValue(ValeurConsigneTemperature());

	return true;
}


// Surcharge de la fonction de slot paintEvent heritee du QWidget
//
void CamerOA::paintEvent(QPaintEvent *event)
{
	QPaintEvent *ev;		// Pour eviter un Warning a la compilation
	ev=event;

	// On redimensionne la boite de rangement verticale
	//
	BoiteRangementVertical->resize(this->size());
}


// Fonction de rafraichissement de la temperature du CCD par interrogation de la camera
//
void CamerOA::RafraichissementTemperatureCCD(void)
{
	PLCamera->DemandeRafraichissementTemperatureCCD();
}


// Fonction de rafraichissement de la temperature du radiateur par interrogation de la camera
//
void CamerOA::RafraichissementTemperatureRadiateur(void)
{
	PLCamera->DemandeRafraichissementTemperatureRadiateur();
}


// Fonction de rafraichissement de la puissance electrique des etages de Peltier
//
void CamerOA::RafraichissementPuissancePeltier(void)
{
	PLCamera->DemandeRafraichissementPuissancePeltier();
}


// Slot du signal clicked() pour ventilateur camera off
//
void CamerOA::SlotBoutonFanModeOff(void)
{
	PLCamera->DemandeFanModeOff();
}


// Slot du signal clicked() pour ventilateur camera low
//
void CamerOA::SlotBoutonFanModeLow(void)
{
	PLCamera->DemandeFanModeLow();
}


// Slot du signal clicked() pour ventilateur camera medium
//
void CamerOA::SlotBoutonFanModeMedium(void)
{
	PLCamera->DemandeFanModeMedium();
}


// Slot du signal clicked() pour ventilateur camera high
//
void CamerOA::SlotBoutonFanModeHigh(void)
{
	PLCamera->DemandeFanModeHigh();
}


// Slot pour le changement de la valeur de la spinbox de la consigne de temperature
//
void CamerOA::SlotSpinBoxCTEMP(int value)
{
	ParamConsigneTemperature(value);
}


// Fonction de parametrage de la consigne de temperature
//
void CamerOA::ParamConsigneTemperature(int valeur)
{
	ConsigneTemperature=valeur;

	PLCamera->DemandeParamConsigneTemperature(ConsigneTemperature);
}


// Fonction retournant la consigne de temperature
//
int CamerOA::ValeurConsigneTemperature(void)
{
	return( ConsigneTemperature );
}


// Fonction de parametrage du temps de pose pour le centrage en 1/10000s
//
void CamerOA::ParamTempsPoseCentrage(int valeur)
{
	TempsPoseCentrage=valeur;
}


// Fonction retournant le temps de pose pour le centrage en 1/10000s
//
int CamerOA::ValeurTempsPoseCentrage(void)
{
	return(TempsPoseCentrage);
}


// Fonction de parametrage du temps de pose pour image en 1/10000s
//
void CamerOA::ParamTempsPoseImage(int valeur)
{
	TempsPoseImage=valeur;
}


// Fonction retournant le temps de pose pour image en 1/10000s
//
int CamerOA::ValeurTempsPoseImage(void)
{
	return(TempsPoseImage);
}


// Fonction de parametrage du temps de pose pour le DARK en 1/10000s
//
void CamerOA::ParamTempsPoseDark(int valeur)
{
	TempsPoseDark=valeur;
}


// Fonction retournant le temps de pose pour le DARK en 1/10000s
//
int CamerOA::ValeurTempsPoseDark(void)
{
	return(TempsPoseDark);
}


// Slot pour forcer l'ouverture de l'obturateur
//
void CamerOA::SlotBoutonForceShutterOpen(void)
{
	PLCamera->DemandeForceShutterOpen();
}


// Slot pour forcer la fermeture de l'obturateur
//
void CamerOA::SlotBoutonForceShutterClose(void)
{
	PLCamera->DemandeForceShutterClose();
}


// Slot pour utiliser l'obturateur pour le centrage
//
void CamerOA::SlotBoutonUtiliserShutterCentrage(void)
{
	PLCamera->DemandeUtilisationObturateurCentrage();
}


// Slot pour ne pas utiliser l'obturateur pour le centrage
//
void CamerOA::SlotBoutonNonUtiliserShutterCentrage(void)
{
	PLCamera->DemandeNonUtilisationObturateurCentrage();
}


// Slot pour le changement de la valeur de la spinbox du temps de pose image en 1/10000 de s
//
void CamerOA::SlotSpinBoxTempsPoseImage(int value)
{
	ParamTempsPoseImage(value);
}


// Slot pour le changement de la valeur de la spinbox du temps de pose d'un DARK en 1/10000 de s
//
void CamerOA::SlotSpinBoxTempsPoseDark(int value)
{
	ParamTempsPoseDark(value);
}


// Slot pour declancher une pose image
//
void CamerOA::SlotBoutonPoseImage(void)
{
	PLCamera->DemandePoseImage();
}


// Slot pour declancher une pose image en pose doublee
//
void CamerOA::SlotBoutonPoseDoubleImage(void)
{
	PLCamera->DemandePoseDoubleImage();
}


// Slot pour declancher une pose de BIAS
//
void CamerOA::SlotBoutonPoseBias(void)
{
	PLCamera->DemandePoseBias();
}


// Slot pour declancher une pose de DARK
//
void CamerOA::SlotBoutonPoseDark(void)
{
	PLCamera->DemandePoseDark();
}


// Slot pour l'arret une pose image
//
void CamerOA::SlotBoutonStopPoseImage(void)
{
	// Demande d'arret de pose image
	//
	CameraCCD->StopExposure(true);
}


// Slot pour le changement de la valeur de la spinbox du temps de pose pour le centrage en 1/10000s
//
void CamerOA::SlotSpinBoxTempsPoseCentrage(int value)
{
	ParamTempsPoseCentrage(value);
}


// Slot du signal timeout() du QTimer de pulsation de la seconde de temps
//
void CamerOA::SlotPulsar1s(void)
{
	// Affichage de l'heure UT dans la barre de status
	//
	AfficherHeureUT();

	//
	// Partie du code pour le rafraichissement des donnees de la camera CCD
	//

	// Toutes les deux secondes on rafraichit la temperature du CCD
	//
	if( !(CompteurRafraichissement % 2) ) RafraichissementTemperatureCCD();

	// Toutes les quatres secondes on rafraichit la temperature du radiateur
	//
	if( !(CompteurRafraichissement % 4) ) RafraichissementTemperatureRadiateur();

	// Toutes les cinq secondes on rafraichit la puissance electrique des etages Peltier
	//
	if( !(CompteurRafraichissement % 5) ) RafraichissementPuissancePeltier();


	// Affichage des valeurs a chaque seconde pour reactualisation la plus rapide possible
	//
	QString ChaineTemperatureCCD=QString("C:%1").arg(PLCamera->ValeurTemperatureCCD(),6,'f',2);
	LabelTempCCD->setText(ChaineTemperatureCCD);

	QString ChaineTemperatureRadiateur=QString("R:%1").arg(PLCamera->ValeurTemperatureRadiateur(),6,'f',2);
	LabelTempRad->setText(ChaineTemperatureRadiateur);

	QString ChainePuissance=QString("P:%1%").arg(PLCamera->ValeurPuissancePeltier(),5,'f',1);
	LabelPuissance->setText(ChainePuissance);


	// Incrementation du compteur de sequencement du rafraichissement
	//
	CompteurRafraichissement++;
}


// Fonction d'affichage de l'heure UT dans la barre de status
//
void CamerOA::AfficherHeureUT(void)
{
	// On recupere l'heure courante UT du systeme
	//
	QTime HeureUT(QTime::currentTime(Qt::UTC));
	
	// Composition de la chaine de l'heure
	//
	QString ChaineHeureUT=QString("%1:%2:%3 UT").arg(HeureUT.hour(),2).arg(HeureUT.minute(),2).arg(HeureUT.second(),2);
	if( ChaineHeureUT.mid(0,1) == QString(" ") ) ChaineHeureUT.replace(0,1,"0");
	if( ChaineHeureUT.mid(3,1) == QString(" ") ) ChaineHeureUT.replace(3,1,"0");
	if( ChaineHeureUT.mid(6,1) == QString(" ") ) ChaineHeureUT.replace(6,1,"0");
	
	// Changement du texte du label
	//
	LabelHeureUTBarreStatus->setText(ChaineHeureUT);
}


// Retourne le chemin vers le repertoire de base du CamerOA
//
QString CamerOA::CheminRepertoireCamerOA(void)
{
	return CheminRepCamerOA;
}


// Surcharge de la fonction de handler des evenements particuliers crees et adresses pour CamerOA
//
void CamerOA::customEvent(QCustomEvent *ce)
{
	// On aiguille selon le type d'evenement particulier recu par CamerOA
	//

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGECENTRAGE )
	{
		// Evenement de demande de chargement d'une nouvelle image dans l'objet de centrage et d'affichage
		//
		CEventCamerOA_ChargeAffEnvImgObjetCentrage *event=(CEventCamerOA_ChargeAffEnvImgObjetCentrage *) ce;	// Typage de l'evenement

		MutexBufferPixelsPhysiquesCentrage.lock();

		ObjAffPoseCentrage->ChargerImage(event->DimX(),event->DimY(),event->Donnees(),event->Debut(),event->Fin(),event->Duree());

		// Si un client est connecte sur le canal des donnees, on peut envoyer une trame image au client
		//
		if( PLServeurDonnees->Serveur->UnClientEstConnecte() )
		{
			// Constitution de l'entete de la trame d'image
			//
			struct EnteteTrameDonneesCamerOA Entete;

			strcpy(Entete.chaine_magique,CH_MAG_ENTDONCAMEROA);
			Entete.type_trame=TRAME_CENTRAGE_IMAGE;					// Trame de type image de centrage
			Entete.type_image=TYPE_IMG_CAMEROA_POSEC;				// Image de centrage
			Entete.tx=event->DimX();
			Entete.ty=event->DimY();
			Entete.debut=event->Debut();
			Entete.fin=event->Fin();
			Entete.duree=event->Duree();

			// Emission de l'entete de la trame image de centrage
			//
			if( PLServeurDonnees->Serveur->EnvoyerDonneesSocketSession(&Entete,sizeof(struct EnteteTrameDonneesCamerOA)) == -1 )
			{
				std::cerr << "CamerOA: ERREUR: ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGECENTRAGE: EnvoyerDonneesSocketSession(): Erreur lors de l'emission des donnees de l'entete." << std::endl;
			}

			// Emission des donnees de l'image de centrage
			//
			if( PLServeurDonnees->Serveur->EnvoyerDonneesSocketSession(event->Donnees(),event->DimX()*event->DimY()*sizeof(unsigned short)) == -1 )
			{
				std::cerr << "CamerOA: ERREUR: ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGECENTRAGE: EnvoyerDonneesSocketSession(): Erreur lors de l'emission des donnees." << std::endl;
			}
		}

		MutexBufferPixelsPhysiquesCentrage.unlock();

//std::cout << "Ext: " << ObjAffPoseCentrage->ValMinImage() << "," << ObjAffPoseCentrage->ValMaxImage() << std::endl;

		ObjAffPoseCentrage->ConsigneSeuilBas(ObjAffPoseCentrage->ValMinImage());
		ObjAffPoseCentrage->ConsigneSeuilHaut(ObjAffPoseCentrage->ValMaxImage());

//std::cout << "Consigne: " << ObjAffPoseCentrage->ConsigneSeuilBas() << "," << ObjAffPoseCentrage->ConsigneSeuilHaut() << std::endl;

		ObjAffPoseCentrage->update();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_H_CENTRAGE )
	{
		// Evenement de demande de chargement d'une nouvelle image de centrage horizontal dans l'objet et d'affichage
		//
		CEventCamerOA_ChargeAffEnvImg_H_ObjetCentrage *event=(CEventCamerOA_ChargeAffEnvImg_H_ObjetCentrage *) ce;	// Typage de l'evenement

		MutexBufferPixelsPCH.lock();

		ObjAffPoseCentrage->ChargerImageH(event->DimX(),event->DimY(),event->Donnees(),event->Debut(),event->Fin(),event->Duree());

		// Si un client est connecte sur le canal des donnees, on peut envoyer une trame image au client
		//
		if( PLServeurDonnees->Serveur->UnClientEstConnecte() )
		{
			// Constitution de l'entete de la trame d'image
			//
			struct EnteteTrameDonneesCamerOA Entete;

			strcpy(Entete.chaine_magique,CH_MAG_ENTDONCAMEROA);
			Entete.type_trame=TRAME_CENTRAGE_H;						// Trame de type image de centrage
			Entete.type_image=TYPE_IMG_CAMEROA_POSEC;				// Image de centrage
			Entete.tx=event->DimX();
			Entete.ty=event->DimY();
			Entete.debut=event->Debut();
			Entete.fin=event->Fin();
			Entete.duree=event->Duree();

			// Emission de l'entete de la trame image de centrage horizontal
			//
			if( PLServeurDonnees->Serveur->EnvoyerDonneesSocketSession(&Entete,sizeof(struct EnteteTrameDonneesCamerOA)) == -1 )
			{
				std::cerr << "CamerOA: ERREUR: ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_H_CENTRAGE: EnvoyerDonneesSocketSession(): Erreur lors de l'emission des donnees de l'entete." << std::endl;
			}

			// Emission des donnees de l'image de centrage horizontal
			//
			if( PLServeurDonnees->Serveur->EnvoyerDonneesSocketSession(event->Donnees(),event->DimX()*event->DimY()*sizeof(unsigned short)) == -1 )
			{
				std::cerr << "CamerOA: ERREUR: ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_H_CENTRAGE: EnvoyerDonneesSocketSession(): Erreur lors de l'emission des donnees." << std::endl;
			}
		}

		MutexBufferPixelsPCH.unlock();

		ObjAffPoseCentrage->update();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_V_CENTRAGE )
	{
		// Evenement de demande de chargement d'une nouvelle image dans l'objet de centrage et d'affichage
		//
		CEventCamerOA_ChargeAffEnvImg_V_ObjetCentrage *event=(CEventCamerOA_ChargeAffEnvImg_V_ObjetCentrage *) ce;	// Typage de l'evenement

		MutexBufferPixelsPCV.lock();

		ObjAffPoseCentrage->ChargerImageV(event->DimX(),event->DimY(),event->Donnees(),event->Debut(),event->Fin(),event->Duree());

		// Si un client est connecte sur le canal des donnees, on peut envoyer une trame image au client
		//
		if( PLServeurDonnees->Serveur->UnClientEstConnecte() )
		{
			// Constitution de l'entete de la trame d'image
			//
			struct EnteteTrameDonneesCamerOA Entete;

			strcpy(Entete.chaine_magique,CH_MAG_ENTDONCAMEROA);
			Entete.type_trame=TRAME_CENTRAGE_V;						// Trame de type image de centrage
			Entete.type_image=TYPE_IMG_CAMEROA_POSEC;				// Image de centrage
			Entete.tx=event->DimX();
			Entete.ty=event->DimY();
			Entete.debut=event->Debut();
			Entete.fin=event->Fin();
			Entete.duree=event->Duree();

			// Emission de l'entete de la trame image de centrage vertical
			//
			if( PLServeurDonnees->Serveur->EnvoyerDonneesSocketSession(&Entete,sizeof(struct EnteteTrameDonneesCamerOA)) == -1 )
			{
				std::cerr << "CamerOA: ERREUR: ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_V_CENTRAGE: EnvoyerDonneesSocketSession(): Erreur lors de l'emission des donnees de l'entete." << std::endl;
			}

			// Emission des donnees de l'image de centrage vertical
			//
			if( PLServeurDonnees->Serveur->EnvoyerDonneesSocketSession(event->Donnees(),event->DimX()*event->DimY()*sizeof(unsigned short)) == -1 )
			{
				std::cerr << "CamerOA: ERREUR: ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE_V_CENTRAGE: EnvoyerDonneesSocketSession(): Erreur lors de l'emission des donnees." << std::endl;
			}
		}

		MutexBufferPixelsPCV.unlock();

		ObjAffPoseCentrage->update();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE )
	{
		// Evenement de demande de chargement d'une nouvelle image dans l'objet d'affichage d'une image
		//
		CEventCamerOA_ChargeAffEnvImgObjetImage *event=(CEventCamerOA_ChargeAffEnvImgObjetImage *) ce;	// Typage de l'evenement

		MutexBufferPixelsPhysiquesImage.lock();

		ObjAffPoseImage->ChargerImage(event->DimX(),event->DimY(),event->Donnees(),event->Debut(),event->Fin(),event->Duree());

		// Si un client est connecte sur le canal des donnees, on peut envoyer une trame image au client
		//
		if( PLServeurDonnees->Serveur->UnClientEstConnecte() )
		{
			// Constitution de l'entete de la trame d'image
			//
			struct EnteteTrameDonneesCamerOA Entete;

			strcpy(Entete.chaine_magique,CH_MAG_ENTDONCAMEROA);
			Entete.type_trame=TRAME_IMAGE;						// Trame de type image
			Entete.type_image=event->Type();
			Entete.tx=event->DimX();
			Entete.ty=event->DimY();
			Entete.debut=event->Debut();
			Entete.fin=event->Fin();
			Entete.duree=event->Duree();

			// Emission de l'entete de la trame image
			//
			if( PLServeurDonnees->Serveur->EnvoyerDonneesSocketSession(&Entete,sizeof(struct EnteteTrameDonneesCamerOA)) == -1 )
			{
				std::cerr << "CamerOA: ERREUR: ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE: EnvoyerDonneesSocketSession(): Erreur lors de l'emission des donnees de l'entete." << std::endl;
			}

			// Emission des donnees de l'image
			//
			if( PLServeurDonnees->Serveur->EnvoyerDonneesSocketSession(event->Donnees(),event->DimX()*event->DimY()*sizeof(unsigned short)) == -1 )
			{
				std::cerr << "CamerOA: ERREUR: ID_CUSTOM_EVENT_CAMEROA_CHARGERAFFENVIMAGE: EnvoyerDonneesSocketSession(): Erreur lors de l'emission des donnees." << std::endl;
			}
		}

		MutexBufferPixelsPhysiquesImage.unlock();

		ObjAffPoseCentrage->ConsigneSeuilBas(ObjAffPoseCentrage->ValMinImage());
		ObjAffPoseCentrage->ConsigneSeuilHaut(ObjAffPoseCentrage->ValMaxImage());

		ObjAffPoseImage->update();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_PARAM_AMPLI_PROFH )
	{
		// Evenement de demande de parametrage de l'objet graphique pour l'amplification du profil horizontal
		//
		CEventCamerOA_ParamAmpliProfilH *event=(CEventCamerOA_ParamAmpliProfilH *) ce;	// Typage de l'evenement

		ObjAffPoseCentrage->ParamAmplificationProfilH(event->DimMatrice(),event->LargeurZone(),event->FacteurAmpli());

		ObjAffPoseCentrage->update();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_PARAM_AMPLI_PROFV )
	{
		// Evenement de demande de parametrage de l'objet graphique pour l'amplification du profil vertical
		//
		CEventCamerOA_ParamAmpliProfilV *event=(CEventCamerOA_ParamAmpliProfilV *) ce;	// Typage de l'evenement

		ObjAffPoseCentrage->ParamAmplificationProfilV(event->DimMatrice(),event->LargeurZone(),event->FacteurAmpli());

		ObjAffPoseCentrage->update();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_QUIT )
	{
		// Evenement de demande d'arret du programme CamerOA
		//
		appli->closeAllWindows();
		appli->quit();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_CTEMP )
	{
		// Evenement de nouvelle consigne de temperature
		//
		CEventCamerOA_CTemp *event=(CEventCamerOA_CTemp *) ce;	// Typage de l'evenement

		SpinBoxCTEMP->setValue(event->Valeur());

		SlotSpinBoxCTEMP(event->Valeur());

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_CTPI )
	{
		// Evenement de nouvelle consigne du temps de pose image
		//
		CEventCamerOA_CTPI *event=(CEventCamerOA_CTPI *) ce;	// Typage de l'evenement

		SpinBoxTempsPoseImage->setValue(event->Valeur());

		SlotSpinBoxTempsPoseImage(event->Valeur());

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_CTPC )
	{
		// Evenement de nouvelle consigne du temps de pose centrage
		//
		CEventCamerOA_CTPC *event=(CEventCamerOA_CTPC *) ce;	// Typage de l'evenement

		SpinBoxTempsPoseCentrage->setValue(event->Valeur());

		SlotSpinBoxTempsPoseCentrage(event->Valeur());

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_CTPD )
	{
		// Evenement de nouvelle consigne du temps de pose d'un DARK
		//
		CEventCamerOA_CTPD *event=(CEventCamerOA_CTPD *) ce;	// Typage de l'evenement

		SpinBoxTempsPoseDark->setValue(event->Valeur());

		SlotSpinBoxTempsPoseDark(event->Valeur());

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_POSEI )
	{
		// Evenement de demande de pose image
		//
		SlotBoutonPoseImage();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_POSEDOUBLEI )
	{
		// Evenement de demande de pose double image
		//
		SlotBoutonPoseDoubleImage();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_POSEBIAS )
	{
		// Evenement de demande de pose de BIAS
		//
		SlotBoutonPoseBias();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_POSEDARK )
	{
		// Evenement de demande de pose de DARK
		//
		SlotBoutonPoseDark();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_ARRETPOSE )
	{
		// Evenement de demande d'arret de la pose en cours
		//
		SlotBoutonStopPoseImage();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_FSHUTTEROPEN )
	{
		// Evenement de demande d'obturateur ouvert
		//
		SlotBoutonForceShutterOpen();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_FSHUTTERCLOSE )
	{
		// Evenement de demande d'obturateur ferme
		//
		SlotBoutonForceShutterClose();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_UTILSHUTCENT )
	{
		// Evenement de demande d'utilisation de l'obturateur pour le centrage
		//
		SlotBoutonUtiliserShutterCentrage();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_NONUTILSHUTCENT )
	{
		// Evenement de demande de non utilisation de l'obturateur pour le centrage
		//
		SlotBoutonNonUtiliserShutterCentrage();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_FANLOW )
	{
		// Evenement de demande de ventilateur camera low
		//
		SlotBoutonFanModeLow();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_FANMEDIUM )
	{
		// Evenement de demande de ventilateur camera medium
		//
		SlotBoutonFanModeMedium();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_FANHIGH )
	{
		// Evenement de demande de ventilateur camera high
		//
		SlotBoutonFanModeHigh();

		return;
	}

	if( ce->type() == ID_CUSTOM_EVENT_CAMEROA_FANOFF )
	{
		// Evenement de demande de ventilateur off
		//
		SlotBoutonFanModeOff();

		return;
	}
}




// - Thread
//---------------------------------------------------------------------------------------------------------------------

// Constructeur du processus leger de controle de la camera
//
// CE:	On passe un pointeur vers l'application CamerOA ;
//
// CS:	-
//
ProcessusLegerControleCamera::ProcessusLegerControleCamera(void)
{
	// Initialisation des variables
	//
	DrapeauDemandeTerminaison=false;
	DemandeRTC_CCD=false;
	DemandeRTR=false;
	DemandeRPP=false;
	DemandeFanOff=false;
	DemandeFanLow=false;
	DemandeFanMedium=false;
	DemandeFanHigh=false;
	DemandePCT=false;
	DemandeFSO=false;
	DemandeFSC=false;
	DemandePoseI=false;
	DemandePoseDoubleI=false;
	DemandePoseB=false;
	DemandePoseD=false;
	ConsigneObturateurCentrage=false;	// Version C1: =true On reste obturateur fermé, non forcé ouvert ; =false pour les autres version avec forcage obturateur ouvert
	DemandeUtilObtuCentrage=false;
	DemandeNonUtilObtuCentrage=false;

	TemperatureCCD=99.0;
	DateHeureTemperatureCCD=QDateTime::currentDateTime(Qt::UTC);

	TemperatureRadiateur=99.0;
	DateHeureTemperatureRadiateur=DateHeureTemperatureCCD;

	PuissancePeltier=0.0;
	DateHeurePuissancePeltier=DateHeureTemperatureCCD;

	DateHeureDebutPoseImage=DateHeureFinPoseImage=DateHeureTemperatureCCD;
	DateHeureDebutPoseCentrage=DateHeureFinPoseCentrage=DateHeureDebutPoseImage;
	DateHeureDebutPoseH=DateHeureFinPoseH=DateHeureDebutPoseImage;
	DateHeureDebutPoseV=DateHeureFinPoseV=DateHeureDebutPoseImage;
	DureePoseImage=DureePoseCentrage=DureePoseH=DureePoseV=0;

	ConsigneTemperature=-20;

	CompteurPoseCentrage=0;

	FPCamerOA=NULL;
}


// Destructeur du processus leger de controle de la camera
//
ProcessusLegerControleCamera::~ProcessusLegerControleCamera()
{
	// Si le thread est encore actif et donc que le serveur ne s'est pas termine normalement
	//
	if( running() )
	{
		terminate();	// On termine le thread de maniere brutale
	}
}


// Fonction de la demande de terminaison propre du processus leger
//
void ProcessusLegerControleCamera::DemandeTerminaison(void)
{
	DrapeauDemandeTerminaison=true;
}


// Surcharge de la methode run() qui contient le code d'execution du thread
//  de controle de la camera
//
void ProcessusLegerControleCamera::run()
{
	int Sortir=false;	// Drapeau d'indication de sortie de la boucle de traitement des demandes de connexion
	
	// On capture un element du semaphore de synchronisation
	//
	SemaphoreSyncLancementThreadCamerOA++;


	// On peut interroger la camera pour les variables physiques
	//
	RafraichissementTemperatureCCD();

	RafraichissementTemperatureRadiateur();

	RafraichissementPuissancePeltier();

	// On demarre l'asservissement de la temperature
	//
	ParamConsigneTemperature();
	CameraCCD->write_CoolerEnable(Camera_CoolerMode_On);

	// Dans tout le programme on telechargera une image a la fois
	//
	CameraCCD->write_ImageCount(1);

	// On realise continuellement des images pour le centrage et a la demande des poses images
	//
	if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

	do
	{
/* COMMENTAIRE 25/07/2007:
	Comme nous avons éliminé les poses de centrage H et V en coupe, nous réalisons des poses de centrage toutes les 3 secondes pour l'autre version que C1
*/
		// Si on doit realiser une pose image de centrage
		//

		// Version autre que C1: On recupere l'heure courante UT du systeme
		//
		QTime HeureUT(QTime::currentTime(Qt::UTC));

		if( /* (CompteurPoseCentrage % 10) == 0 */  /* Version C1: true */  /* Version autres: */ (HeureUT.second() % 3) == 0 )
		{
			if( PoseCentrageImage() )
			{
				CEventCamerOA_ChargeAffEnvImgObjetCentrage *event=new CEventCamerOA_ChargeAffEnvImgObjetCentrage(CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesCentrage,DateHeureDebutPoseCentrage,DateHeureFinPoseCentrage,DureePoseCentrage);

				QApplication::postEvent(FPCamerOA,event);
			}
		}

/* Code eliminé depuis le 24/07/2007 car les courbes ne servaient pas vraiment au centrage

		// On realise systematiquement la pose pour le centrage horizontal
		//
		if( PoseCentrageH() )
		{
			CEventCamerOA_ChargeAffEnvImg_H_ObjetCentrage *event=new CEventCamerOA_ChargeAffEnvImg_H_ObjetCentrage(CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPCH,DateHeureDebutPoseH,DateHeureFinPoseH,DureePoseH);

			QApplication::postEvent(FPCamerOA,event);

			CompteurPoseCentrage++;
		}
*/
		// Si on demande une pose image
		//
		if( DemandePoseI )
		{
			if( PoseImage(true) )
			{
				CEventCamerOA_ChargeAffEnvImgObjetImage *event=new CEventCamerOA_ChargeAffEnvImgObjetImage(TYPE_IMG_CAMEROA_POSEI,CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesImage,DateHeureDebutPoseImage,DateHeureFinPoseImage,DureePoseImage);

				QApplication::postEvent(FPCamerOA,event);

				// On attend 1/100s avant de reouvrir l'obturateur
				//
				usleep(10000);
			}

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

			MutexDemandePoseI.lock();
			DemandePoseI=false;
			MutexDemandePoseI.unlock();
		}

		// Si on demande une pose image doublee
		//
		if( DemandePoseDoubleI )
		{
			if( PoseDoubleImage(true) )
			{
				CEventCamerOA_ChargeAffEnvImgObjetImage *event=new CEventCamerOA_ChargeAffEnvImgObjetImage(TYPE_IMG_CAMEROA_POSEI,CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesImage,DateHeureDebutPoseImage,DateHeureFinPoseImage,DureePoseImage);

				QApplication::postEvent(FPCamerOA,event);

				// On attend 1/100s avant de reouvrir l'obturateur
				//
				usleep(10000);
			}

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

			MutexDemandePoseDoubleI.lock();
			DemandePoseDoubleI=false;
			MutexDemandePoseDoubleI.unlock();
		}

		// Si on demande une pose image de BIAS
		//
		if( DemandePoseB )
		{
			if( PoseImageBias() )
			{
				CEventCamerOA_ChargeAffEnvImgObjetImage *event=new CEventCamerOA_ChargeAffEnvImgObjetImage(TYPE_IMG_CAMEROA_POSEBIAS,CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesImage,DateHeureDebutPoseImage,DateHeureFinPoseImage,DureePoseImage);

				QApplication::postEvent(FPCamerOA,event);

				// On attend 1/100s avant de reouvrir l'obturateur
				//
				usleep(10000);
			}

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

			MutexDemandePoseB.lock();
			DemandePoseB=false;
			MutexDemandePoseB.unlock();
		}

		// Si on demande une pose image de DARK
		//
		if( DemandePoseD )
		{
			if( PoseImageDark() )
			{
				CEventCamerOA_ChargeAffEnvImgObjetImage *event=new CEventCamerOA_ChargeAffEnvImgObjetImage(TYPE_IMG_CAMEROA_POSEDARK,CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesImage,DateHeureDebutPoseImage,DateHeureFinPoseImage,DureePoseImage);

				QApplication::postEvent(FPCamerOA,event);

				// On attend 1/100s avant de reouvrir l'obturateur
				//
				usleep(10000);
			}

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

			MutexDemandePoseD.lock();
			DemandePoseD=false;
			MutexDemandePoseD.unlock();
		}

/* Code eliminé depuis le 24/07/2007 car les courbes ne servaient pas vraiment au centrage

		// On realise systematiquement la pose pour le centrage vertical
		//
		if( PoseCentrageV() )
		{
			CEventCamerOA_ChargeAffEnvImg_V_ObjetCentrage *event=new CEventCamerOA_ChargeAffEnvImg_V_ObjetCentrage(CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPCV,DateHeureDebutPoseV,DateHeureFinPoseV,DureePoseV);

			QApplication::postEvent(FPCamerOA,event);

			CompteurPoseCentrage++;
		}
*/
		// Si on demande une pose image
		//
		if( DemandePoseI )
		{
			if( PoseImage(true) )
			{
				CEventCamerOA_ChargeAffEnvImgObjetImage *event=new CEventCamerOA_ChargeAffEnvImgObjetImage(TYPE_IMG_CAMEROA_POSEI,CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesImage,DateHeureDebutPoseImage,DateHeureFinPoseImage,DureePoseImage);

				QApplication::postEvent(FPCamerOA,event);

				// On attend 1/100s avant de reouvrir l'obturateur
				//
				usleep(10000);
			}

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

			MutexDemandePoseI.lock();
			DemandePoseI=false;
			MutexDemandePoseI.unlock();
		}
		
		// Si on demande une pose image doublee
		//
		if( DemandePoseDoubleI )
		{
			if( PoseDoubleImage(true) )
			{
				CEventCamerOA_ChargeAffEnvImgObjetImage *event=new CEventCamerOA_ChargeAffEnvImgObjetImage(TYPE_IMG_CAMEROA_POSEI,CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesImage,DateHeureDebutPoseImage,DateHeureFinPoseImage,DureePoseImage);

				QApplication::postEvent(FPCamerOA,event);

				// On attend 1/100s avant de reouvrir l'obturateur
				//
				usleep(10000);
			}

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

			MutexDemandePoseDoubleI.lock();
			DemandePoseDoubleI=false;
			MutexDemandePoseDoubleI.unlock();
		}

		// Si on demande une pose image de BIAS
		//
		if( DemandePoseB )
		{
			if( PoseImageBias() )
			{
				CEventCamerOA_ChargeAffEnvImgObjetImage *event=new CEventCamerOA_ChargeAffEnvImgObjetImage(TYPE_IMG_CAMEROA_POSEBIAS,CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesImage,DateHeureDebutPoseImage,DateHeureFinPoseImage,DureePoseImage);

				QApplication::postEvent(FPCamerOA,event);

				// On attend 1/100s avant de reouvrir l'obturateur
				//
				usleep(10000);
			}

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

			MutexDemandePoseB.lock();
			DemandePoseB=false;
			MutexDemandePoseB.unlock();
		}

		// Si on demande une pose image de DARK
		//
		if( DemandePoseD )
		{
			if( PoseImageDark() )
			{
				CEventCamerOA_ChargeAffEnvImgObjetImage *event=new CEventCamerOA_ChargeAffEnvImgObjetImage(TYPE_IMG_CAMEROA_POSEDARK,CameraCCD->m_pvtRoiPixelsH,CameraCCD->m_pvtRoiPixelsV,BufferPixelsPhysiquesImage,DateHeureDebutPoseImage,DateHeureFinPoseImage,DureePoseImage);

				QApplication::postEvent(FPCamerOA,event);

				// On attend 1/100s avant de reouvrir l'obturateur
				//
				usleep(10000);
			}

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();

			MutexDemandePoseD.lock();
			DemandePoseD=false;
			MutexDemandePoseD.unlock();
		}

		// Si on demande le parametrage de la consigne de temperature
		//
		if( DemandePCT )
		{
			ParamConsigneTemperature();

			MutexDemandePCT.lock();
			DemandePCT=false;
			MutexDemandePCT.unlock();
		}

		// Si on demande le rafraichissement de la temperature CCD
		//
		if( DemandeRTC_CCD )
		{
			RafraichissementTemperatureCCD();

			MutexDemandeRTC_CCD.lock();
			DemandeRTC_CCD=false;
			MutexDemandeRTC_CCD.unlock();
		}

		// Si on demande le rafraichissement de la temperature radiateur
		//
		if( DemandeRTR )
		{
			RafraichissementTemperatureRadiateur();

			MutexDemandeRTR.lock();
			DemandeRTR=false;
			MutexDemandeRTR.unlock();
		}

		// Si on demande de rafraichissement de la puissance Peltier
		//
		if( DemandeRPP )
		{
			RafraichissementPuissancePeltier();

			MutexDemandeRPP.lock();
			DemandeRPP=false;
			MutexDemandeRPP.unlock();
		}

		// Si on demande le ventilateur off
		//
		if( DemandeFanOff )
		{
			FanModeOff();

			MutexDemandeFanOff.lock();
			DemandeFanOff=false;
			MutexDemandeFanOff.unlock();
		}

		// Si on demande le ventilateur low
		//
		if( DemandeFanLow )
		{
			FanModeLow();

			MutexDemandeFanLow.lock();
			DemandeFanLow=false;
			MutexDemandeFanLow.unlock();
		}

		// Si on demande le ventilateur medium
		//
		if( DemandeFanMedium )
		{
			FanModeMedium();

			MutexDemandeFanMedium.lock();
			DemandeFanMedium=false;
			MutexDemandeFanMedium.unlock();
		}

		// Si on demande le ventilateur high
		//
		if( DemandeFanHigh )
		{
			FanModeHigh();

			MutexDemandeFanHigh.lock();
			DemandeFanHigh=false;
			MutexDemandeFanHigh.unlock();
		}

		// Si on demande le forcage de l'obturateur ouvert
		//
		if( DemandeFSO )
		{
			ForceShutterOpen();

			MutexDemandeFSO.lock();
			DemandeFSO=false;
			MutexDemandeFSO.unlock();
		}

		// Si on demande le forcage de l'obturateur ferme
		//
		if( DemandeFSC )
		{
			ForceShutterClose();

			MutexDemandeFSC.lock();
			DemandeFSC=false;
			MutexDemandeFSC.unlock();
		}

		// Si on demande l'utilisation de l'obturateur pour le centrage
		//
		if( DemandeUtilObtuCentrage )
		{
			// On fixe la consigne
			//
			UtiliserObturateurCentrage();

			// On annule la demande courante
			//
			MutexDemandeUtilObtuCentrage.lock();
			DemandeUtilObtuCentrage=false;
			MutexDemandeUtilObtuCentrage.unlock();

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();
		}

		// Si on demande la non utilisation de l'obturateur pour le centrage
		//
		if( DemandeNonUtilObtuCentrage )
		{
			// On fixe la consigne
			//
			NePasUtiliserObturateurCentrage();

			// On annule la demande courante
			//
			MutexDemandeNonUtilObtuCentrage.lock();
			DemandeNonUtilObtuCentrage=false;
			MutexDemandeNonUtilObtuCentrage.unlock();

			if( !ConsigneObturateurCentrage ) ForceShutterOpen(); else ForceShutterClose();
		}

		if( DrapeauDemandeTerminaison ) Sortir=true;

	} while( !Sortir );
}


// Fonction retournant la prise de temperature du CCD
//
double ProcessusLegerControleCamera::ValeurTemperatureCCD(void)
{
	return TemperatureCCD;
}

QDateTime ProcessusLegerControleCamera::ValeurDateHeureTemperatureCCD(void)
{
	return DateHeureTemperatureCCD;
}


// Fonction retournant la prise de temperature du radiateur
//
double ProcessusLegerControleCamera::ValeurTemperatureRadiateur(void)
{
	return TemperatureRadiateur;
}

QDateTime ProcessusLegerControleCamera::ValeurDateHeureTemperatureRadiateur(void)
{
	return DateHeureTemperatureRadiateur;
}


// Fonction retournant prise de la puissance electrique des etages de Peltier
//
double ProcessusLegerControleCamera::ValeurPuissancePeltier(void)
{
	return PuissancePeltier;
}

QDateTime ProcessusLegerControleCamera::ValeurDateHeurePuissancePeltier(void)
{
	return DateHeurePuissancePeltier;
}


// Fonction de demande de rafraichissement de la temperature du CCD par interrogation de la camera
//
void ProcessusLegerControleCamera::DemandeRafraichissementTemperatureCCD(void)
{
	MutexDemandeRTC_CCD.lock();
	DemandeRTC_CCD=true;
	MutexDemandeRTC_CCD.unlock();
}


// Fonction de rafraichissement de la temperature du CCD par interrogation de la camera
//
void ProcessusLegerControleCamera::RafraichissementTemperatureCCD(void)
{
	MutexTemperatureCCD.lock();
	MutexDateHeureTemperatureCCD.lock();

	TemperatureCCD=CameraCCD->read_TempCCD();
	DateHeureTemperatureCCD=QDateTime::currentDateTime(Qt::UTC);

	MutexTemperatureCCD.unlock();
	MutexDateHeureTemperatureCCD.unlock();
}


// Fonction de demande de rafraichissement de la temperature du radiateur par interrogation de la camera
//
void ProcessusLegerControleCamera::DemandeRafraichissementTemperatureRadiateur(void)
{
	MutexDemandeRTR.lock();
	DemandeRTR=true;
	MutexDemandeRTR.unlock();
}


// Fonction de rafraichissement de la temperature du radiateur par interrogation de la camera
//
void ProcessusLegerControleCamera::RafraichissementTemperatureRadiateur(void)
{
	MutexTemperatureRadiateur.lock();
	MutexDateHeureTemperatureRadiateur.lock();

	TemperatureRadiateur=CameraCCD->read_TempHeatsink();
	DateHeureTemperatureRadiateur=QDateTime::currentDateTime(Qt::UTC);

	MutexTemperatureRadiateur.unlock();
	MutexDateHeureTemperatureRadiateur.unlock();
}


// Fonction de demande de rafraichissement de la puissance electrique des etages de Peltier
//
void ProcessusLegerControleCamera::DemandeRafraichissementPuissancePeltier(void)
{
	MutexDemandeRPP.lock();
	DemandeRPP=true;
	MutexDemandeRPP.unlock();
}


// Fonction de rafraichissement de la puissance electrique des etages de Peltier
//
void ProcessusLegerControleCamera::RafraichissementPuissancePeltier(void)
{
	MutexPuissancePeltier.lock();
	MutexDateHeurePuissancePeltier.lock();

	PuissancePeltier=CameraCCD->read_CoolerDrive();
	DateHeurePuissancePeltier=QDateTime::currentDateTime(Qt::UTC);

	MutexPuissancePeltier.unlock();
	MutexDateHeurePuissancePeltier.unlock();
}


// Demande pour ventilateur camera off
//
void ProcessusLegerControleCamera::DemandeFanModeOff(void)
{
	MutexDemandeFanOff.lock();
	DemandeFanOff=true;
	MutexDemandeFanOff.unlock();
}


// Ventilateur camera off
//
void ProcessusLegerControleCamera::FanModeOff(void)
{
	CameraCCD->write_FanMode(Apn_FanMode_Off);
}


// Demande pour ventilateur camera low
//
void ProcessusLegerControleCamera::DemandeFanModeLow(void)
{
	MutexDemandeFanLow.lock();
	DemandeFanLow=true;
	MutexDemandeFanLow.unlock();
}


// Ventilateur camera low
//
void ProcessusLegerControleCamera::FanModeLow(void)
{
	CameraCCD->write_FanMode(Apn_FanMode_Low);
}

// Demande pour ventilateur camera medium
//
void ProcessusLegerControleCamera::DemandeFanModeMedium(void)
{
	MutexDemandeFanMedium.lock();
	DemandeFanMedium=true;
	MutexDemandeFanMedium.unlock();
}


// Ventilateur camera medium
//
void ProcessusLegerControleCamera::FanModeMedium(void)
{
	CameraCCD->write_FanMode(Apn_FanMode_Medium);
}


// Demande pour ventilateur camera high
//
void ProcessusLegerControleCamera::DemandeFanModeHigh(void)
{
	MutexDemandeFanHigh.lock();
	DemandeFanHigh=true;
	MutexDemandeFanHigh.unlock();
}


// Demande pour ventilateur camera high
//
void ProcessusLegerControleCamera::FanModeHigh(void)
{
	CameraCCD->write_FanMode(Apn_FanMode_High);
}


// Demande de parametrage de la consigne de temperature
//
void ProcessusLegerControleCamera::DemandeParamConsigneTemperature(int valeur)
{
	MutexDemandePCT.lock();
	MutexConsigneTemperature.lock();

	DemandePCT=true;
	ConsigneTemperature=valeur;

	MutexDemandePCT.unlock();
	MutexConsigneTemperature.unlock();
}


// Demande de parametrage de la consigne de temperature
//
void ProcessusLegerControleCamera::ParamConsigneTemperature(void)
{
	CameraCCD->write_CoolerSetPoint((double) ConsigneTemperature);
}


// Demande pour forcer l'ouverture de l'obturateur
//
void ProcessusLegerControleCamera::DemandeForceShutterOpen(void)
{
	MutexDemandeFSO.lock();
	DemandeFSO=true;
	MutexDemandeFSO.unlock();
}


// Demande pour utiliser l'obturateur en mode centrage
//
void ProcessusLegerControleCamera::DemandeUtilisationObturateurCentrage(void)
{
	MutexDemandeUtilObtuCentrage.lock();
	DemandeUtilObtuCentrage=true;
	MutexDemandeUtilObtuCentrage.unlock();
}


// Utiliser l'obturateur pour le centrage
//
void ProcessusLegerControleCamera::UtiliserObturateurCentrage(void)
{
	MutexConsigneObturateurCentrage.lock();
	ConsigneObturateurCentrage=true;
	MutexConsigneObturateurCentrage.unlock();
}


// Demande pour ne pas utiliser l'obturateur en mode centrage
//
void ProcessusLegerControleCamera::DemandeNonUtilisationObturateurCentrage(void)
{
	MutexDemandeNonUtilObtuCentrage.lock();
	DemandeNonUtilObtuCentrage=true;
	MutexDemandeNonUtilObtuCentrage.unlock();
}


// Ne pas utiliser l'obturateur pour le centrage
//
void ProcessusLegerControleCamera::NePasUtiliserObturateurCentrage(void)
{
	MutexConsigneObturateurCentrage.lock();
	ConsigneObturateurCentrage=false;
	MutexConsigneObturateurCentrage.unlock();
}


// Forcer l'ouverture de l'obturateur
//
void ProcessusLegerControleCamera::ForceShutterOpen(void)
{
	CameraCCD->write_ForceShutterOpen(true);
}


// Demande pour forcer la fermeture de l'obturateur
//
void ProcessusLegerControleCamera::DemandeForceShutterClose(void)
{
	MutexDemandeFSC.lock();
	DemandeFSC=true;
	MutexDemandeFSC.unlock();
}


// Forcer la fermeture de l'obturateur
//
void ProcessusLegerControleCamera::ForceShutterClose(void)
{
	CameraCCD->write_ForceShutterOpen(false);
}


// Demande pour l'arret une pose image
//
void ProcessusLegerControleCamera::DemandeStopPoseImage(void)
{
	// Demande d'arret de pose image
	//
	CameraCCD->StopExposure(true);
}


// Pose de la camera pour centrage horizontal
//
int ProcessusLegerControleCamera::PoseCentrageH(void)
{
	unsigned short Largeur=0;	// Nombre de colonnes lues
	unsigned short Hauteur=0;	// Nombre de lignes lues
	unsigned long Compteur=0;	// Nombre d'images lues

	// On digitalisera en 12 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_TwelveBit);

	if( !ConsigneObturateurCentrage )
	{
		// On commence par vider le chip CCD : OBLIGATOIRE SI L'OBTURATEUR RESTE OUVERT
		//
		CameraCCD->m_pvtRoiStartX=0;
		CameraCCD->m_pvtRoiPixelsH=1;
		CameraCCD->write_RoiBinningH(BINNING_POSE_VIDAGE_CENTRAGE);

		CameraCCD->m_pvtRoiStartY=0;
		CameraCCD->m_pvtRoiPixelsV=1;
		CameraCCD->write_RoiBinningV(BINNING_POSE_VIDAGE_CENTRAGE);

		for( int vidage=0; vidage < NB_VIDAGE_POSE_CENTRAGE; vidage++ )
		{
			if( !CameraCCD->Expose(0.0,false) )
			{
				std::cerr << "CamerOA: ERREUR: Expose(): vidage centrage horizontal." << std::endl;
			}

			MutexBufferPixelsPhysiquesVidage.lock();

			if( CameraCCD->GetImageData(BufferPixelsPhysiquesVidage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
			{
				std::cerr << "CamerOA: ERREUR: GetImageData(): vidage centrage horizontal." << std::endl;
			}

			MutexBufferPixelsPhysiquesVidage.unlock();
		}
	}


	// On realise la pose
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns/BINNING_POSE_CENTRAGE;
	CameraCCD->write_RoiBinningH(BINNING_POSE_CENTRAGE);

	CameraCCD->m_pvtRoiStartY=CameraCCD->m_ImagingRows/2;
	CameraCCD->m_pvtRoiPixelsV=NB_LIGNES_POSE_CENTRAGE;
	CameraCCD->write_RoiBinningV(BINNING_POSE_CENTRAGE);

	MutexBufferPixelsPCH.lock();
	MutexDateHeureDebutPoseH.lock();
	MutexDateHeureFinPoseH.lock();
	MutexDureePoseH.lock();

	if( !CameraCCD->Expose(((double) (DureePoseH=FPCamerOA->ValeurTempsPoseCentrage()))/10000.0,true) )
	{
		std::cerr << "CamerOA: ERREUR: Expose(): pose centrage horizontal." << std::endl;
	}

	DateHeureDebutPoseH=QDateTime::currentDateTime(Qt::UTC);
	usleep(FPCamerOA->ValeurTempsPoseCentrage()*100);
	DateHeureFinPoseH=QDateTime::currentDateTime(Qt::UTC);

	if( CameraCCD->GetImageData(BufferPixelsPCH,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
	{
		std::cerr << "CamerOA: ERREUR: GetImageData(): pose centrage horizontal." << std::endl;
	}

	MutexDateHeureDebutPoseH.unlock();
	MutexDateHeureFinPoseH.unlock();
	MutexDureePoseH.unlock();
	MutexBufferPixelsPCH.unlock();

//std::cout << "Pose H:" << Largeur << "," << Hauteur << "," << Compteur << std::endl;

	if( CameraCCD->m_pvtRoiPixelsH == Largeur && CameraCCD->m_pvtRoiPixelsV == Hauteur ) return true; else return false;
}


// Pose de la camera pour centrage vertical
//
int ProcessusLegerControleCamera::PoseCentrageV(void)
{
	unsigned short Largeur=0;	// Nombre de colonnes lues
	unsigned short Hauteur=0;	// Nombre de lignes lues
	unsigned long Compteur=0;	// Nombre d'images lues

	// On digitalisera en 12 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_TwelveBit);

	if( !ConsigneObturateurCentrage )
	{
		// On commence par vider le chip CCD : OBLIGATOIRE SI L'OBTURATEUR RESTE OUVERT
		//
		CameraCCD->m_pvtRoiStartX=0;
		CameraCCD->m_pvtRoiPixelsH=1;
		CameraCCD->write_RoiBinningH(BINNING_POSE_VIDAGE_CENTRAGE);

		CameraCCD->m_pvtRoiStartY=0;
		CameraCCD->m_pvtRoiPixelsV=1;
		CameraCCD->write_RoiBinningV(BINNING_POSE_VIDAGE_CENTRAGE);

		for( int vidage=0; vidage < NB_VIDAGE_POSE_CENTRAGE; vidage++ )
		{
			if( !CameraCCD->Expose(0.0,false) )
			{
				std::cerr << "CamerOA: ERREUR: Expose(): vidage centrage vertical." << std::endl;
			}

			MutexBufferPixelsPhysiquesVidage.lock();

			if( CameraCCD->GetImageData(BufferPixelsPhysiquesVidage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
			{
				std::cerr << "CamerOA: ERREUR: GetImageData(): vidage centrage vertical." << std::endl;
			}

			MutexBufferPixelsPhysiquesVidage.unlock();
		}
	}


	// On realise la pose
	//
	CameraCCD->m_pvtRoiStartX=CameraCCD->m_ImagingColumns/2;
	CameraCCD->m_pvtRoiPixelsH=NB_COLONNES_POSE_CENTRAGE;
	CameraCCD->write_RoiBinningH(BINNING_POSE_CENTRAGE);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows/BINNING_POSE_CENTRAGE;
	CameraCCD->write_RoiBinningV(BINNING_POSE_CENTRAGE);

	MutexBufferPixelsPCV.lock();
	MutexDateHeureDebutPoseV.lock();
	MutexDateHeureFinPoseV.lock();
	MutexDureePoseV.lock();

	if( !CameraCCD->Expose(((double) (DureePoseV=FPCamerOA->ValeurTempsPoseCentrage()))/10000.0,true) )
	{
		std::cerr << "CamerOA: ERREUR: Expose(): pose centrage vertical." << std::endl;
	}

	DateHeureDebutPoseV=QDateTime::currentDateTime(Qt::UTC);
	usleep(FPCamerOA->ValeurTempsPoseCentrage()*100);
	DateHeureFinPoseV=QDateTime::currentDateTime(Qt::UTC);

	if( CameraCCD->GetImageData(BufferPixelsPCV,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
	{
		std::cerr << "CamerOA: ERREUR: GetImageData(): pose centrage vertical." << std::endl;
	}

	MutexDateHeureDebutPoseV.unlock();
	MutexDateHeureFinPoseV.unlock();
	MutexDureePoseV.unlock();
	MutexBufferPixelsPCV.unlock();

//std::cout << "Pose V:" << Largeur << "," << Hauteur << "," << Compteur << std::endl;

	if( CameraCCD->m_pvtRoiPixelsH == Largeur && CameraCCD->m_pvtRoiPixelsV == Hauteur ) return true; else return false;
}


// Pose de la camera pour le centrage en image
//
int ProcessusLegerControleCamera::PoseCentrageImage(void)
{
	unsigned short Largeur=0;	// Nombre de colonnes lues
	unsigned short Hauteur=0;	// Nombre de lignes lues
	unsigned long Compteur=0;	// Nombre d'images lues

//	COMMENTAIRES IMPORTANTS 25/07/2007 :
//
//	Nous aurons deux versions de CamerOA. Une version pour C1 qui réalise une pose avec obturateur pour la pose de centrage
//	 car nous avions trop de lumière et nous saturions et donc difficulté à faire un centrage correctement de l'instrument,
//	 passage en 16 bits car trop de flux et gain non réglable en 12 bits.
//	 Pour l'image de centrage en C1 nous travaillons avec l'obturateur fermé et pas systèmatiquement ouvert
//	Une autre version pour les autres instruments, qui travaille avec l'obturateur ouvert systèmatiquement en 12 bits.
//	Pour la compilation des versions, il faut bien changer la valeur d'initialisation de ConsigneObturateurCentrage dans
//	 ProcessusLegerControleCamera::ProcessusLegerControleCamera(void)
//	ConsigneObturateurCentrage=true pour la version C1 avec obturateur non forcé ouvert avant la pose de centrage.
//	ConsigneObturateurCentrage=false pour la version avec obturateur forcé ouvert avant la pose de centrage.


	// Version C1: On ne force plus l'ouverture de l'obturateur de maniere systematique
	//
//	CameraCCD->write_ForceShutterOpen(false);

	// Version C1: On patiente 1/100s le temps qu'il se ferme
	//
//	usleep(10000);

	// Toutes Versions: On digitalisera en 12 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_TwelveBit);

	// On commence par vider le chip CCD : OBLIGATOIRE SI L'OBTURATEUR RESTE OUVERT
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=1;
	CameraCCD->write_RoiBinningH(BINNING_POSE_VIDAGE_CENTRAGE);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=1;
	CameraCCD->write_RoiBinningV(BINNING_POSE_VIDAGE_CENTRAGE);

	for( int vidage=0; vidage < NB_VIDAGE_POSE_CENTRAGE; vidage++ )
	{
		if( !CameraCCD->Expose(0.0,false) )
		{
			std::cerr << "CamerOA: ERREUR: Expose(): vidage pose image centrage." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.lock();

		if( CameraCCD->GetImageData(BufferPixelsPhysiquesVidage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
		{
			std::cerr << "CamerOA: ERREUR: GetImageData(): vidage pose image centrage." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.unlock();
	}


	// Version C1: On realise la pose en 16 bits
	//
//	CameraCCD->write_DataBits(Apn_Resolution_SixteenBit);

	// On realise la pose
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns/BINNING_POSE_IMAGE_CENTRAGE;
	CameraCCD->write_RoiBinningH(BINNING_POSE_CENTRAGE);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows/BINNING_POSE_IMAGE_CENTRAGE;
	CameraCCD->write_RoiBinningV(BINNING_POSE_CENTRAGE);

	MutexBufferPixelsPhysiquesCentrage.lock();
	MutexDateHeureDebutPoseCentrage.lock();
	MutexDateHeureFinPoseCentrage.lock();
	MutexDureePoseCentrage.lock();

	if( !CameraCCD->Expose(((double) (DureePoseCentrage=FPCamerOA->ValeurTempsPoseCentrage()))/10000.0,true) )
	{
		std::cerr << "CamerOA: ERREUR: Expose(): pose image centrage." << std::endl;
	}

	DateHeureDebutPoseCentrage=QDateTime::currentDateTime(Qt::UTC);
	usleep(FPCamerOA->ValeurTempsPoseCentrage()*100);
	DateHeureFinPoseCentrage=QDateTime::currentDateTime(Qt::UTC);

	if( CameraCCD->GetImageData(BufferPixelsPhysiquesCentrage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
	{
		std::cerr << "CamerOA: ERREUR: GetImageData(): pose image centrage." << std::endl;
	}

	MutexDateHeureDebutPoseCentrage.unlock();
	MutexDateHeureFinPoseCentrage.unlock();
	MutexDureePoseCentrage.unlock();
	MutexBufferPixelsPhysiquesCentrage.unlock();

//std::cout << "Pose CIM:" << Largeur << "," << Hauteur << "," << Compteur << std::endl;

	if( CameraCCD->m_pvtRoiPixelsH == Largeur && CameraCCD->m_pvtRoiPixelsV == Hauteur ) return true; else return false;
}


// Demande de prise d'image de BIAS
//
// CE:	-
//
void ProcessusLegerControleCamera::DemandePoseBias(void)
{
	MutexDemandePoseB.lock();

	DemandePoseB=true;

	MutexDemandePoseB.unlock();
}


// Prise d'une image de BIAS
//
// CE:	-
//
// CS:	La fonction est vraie si l'image est bien transferee
//
int ProcessusLegerControleCamera::PoseImageBias(void)
{
	unsigned short Largeur=0;	// Nombre de colonnes lues
	unsigned short Hauteur=0;	// Nombre de lignes lues
	unsigned long Compteur=0;	// Nombre d'images lues

	// On videra en 12 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_TwelveBit);

	// On ne force plus l'ouverture de l'obturateur de maniere systematique
	//
	CameraCCD->write_ForceShutterOpen(false);

	// On patiente 1/100s le temps qu'il se ferme
	//
	usleep(10000);

	// On vide bien la camera en mode binning avant de faire une pose image
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningH(BINNING_POSE_VIDAGE_IMAGE);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningV(BINNING_POSE_VIDAGE_IMAGE);

	for( int vidage=0; vidage < NB_VIDAGE_POSE_IMAGE; vidage++ )
	{
		if( !CameraCCD->Expose(0.0,false) )
		{
			std::cerr << "CamerOA: ERREUR: Expose(): vidage pose BIAS." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.lock();

		if( CameraCCD->GetImageData(BufferPixelsPhysiquesVidage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
		{
			std::cerr << "CamerOA: ERREUR: GetImageData(): vidage pose BIAS." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.unlock();
	}

	// On digitalisera en 16 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_SixteenBit);

	// On commence la pose image de BIAS selon les consignes
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns;
	CameraCCD->write_RoiBinningH(1);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows;
	CameraCCD->write_RoiBinningV(1);

	MutexBufferPixelsPhysiquesImage.lock();
	MutexDateHeureDebutPoseImage.lock();
	MutexDateHeureFinPoseImage.lock();
	MutexDureePoseImage.lock();

	if( !CameraCCD->Expose(0.0,false) )
	{
		std::cerr << "CamerOA: ERREUR: Expose(): pose BIAS." << std::endl;

		return false;
	}

	DateHeureDebutPoseImage=QDateTime::currentDateTime(Qt::UTC);
	DateHeureFinPoseImage=DateHeureDebutPoseImage;
	DureePoseImage=0;

	if( CameraCCD->GetImageData(BufferPixelsPhysiquesImage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
	{
		std::cerr << "CamerOA: ERREUR: GetImageData(): pose BIAS." << std::endl;

		return false;
	}

	MutexDateHeureDebutPoseImage.unlock();
	MutexDateHeureFinPoseImage.unlock();
	MutexDureePoseImage.unlock();
	MutexBufferPixelsPhysiquesImage.unlock();

	if( CameraCCD->m_pvtRoiPixelsH == Largeur && CameraCCD->m_pvtRoiPixelsV == Hauteur ) return true; else return false;
}


// Demande de prise d'image de DARK
//
// CE:	-
//
void ProcessusLegerControleCamera::DemandePoseDark(void)
{
	MutexDemandePoseD.lock();

	DemandePoseD=true;

	MutexDemandePoseD.unlock();
}


// Prise d'une image de DARK
//
// CE:	-
//
// CS:	La fonction est vraie si l'image est bien transferee
//
int ProcessusLegerControleCamera::PoseImageDark(void)
{
	unsigned short Largeur=0;	// Nombre de colonnes lues
	unsigned short Hauteur=0;	// Nombre de lignes lues
	unsigned long Compteur=0;	// Nombre d'images lues

	// On videra en 12 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_TwelveBit);

	// On ne force plus l'ouverture de l'obturateur de maniere systematique
	//
	CameraCCD->write_ForceShutterOpen(false);

	// On patiente 1/100s le temps qu'il se ferme
	//
	usleep(10000);

	// On vide bien la camera en mode binning avant de faire une pose image
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningH(BINNING_POSE_VIDAGE_IMAGE);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningV(BINNING_POSE_VIDAGE_IMAGE);

	for( int vidage=0; vidage < NB_VIDAGE_POSE_IMAGE; vidage++ )
	{
		if( !CameraCCD->Expose(0.0,false) )
		{
			std::cerr << "CamerOA: ERREUR: Expose(): vidage pose DARK" << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.lock();

		if( CameraCCD->GetImageData(BufferPixelsPhysiquesVidage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
		{
			std::cerr << "CamerOA: ERREUR: GetImageData(): vidage pose DARK." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.unlock();
	}

	// On digitalisera en 16 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_SixteenBit);

	// On commence la pose image de DARK selon les consignes
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns;
	CameraCCD->write_RoiBinningH(1);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows;
	CameraCCD->write_RoiBinningV(1);

	MutexBufferPixelsPhysiquesImage.lock();
	MutexDateHeureDebutPoseImage.lock();
	MutexDateHeureFinPoseImage.lock();
	MutexDureePoseImage.lock();

	if( !CameraCCD->Expose(((double) (DureePoseImage=FPCamerOA->ValeurTempsPoseDark()))/10000.0,false) )
	{
		std::cerr << "CamerOA: ERREUR: Expose(): pose image DARK." << std::endl;

		return false;
	}

	DateHeureDebutPoseImage=QDateTime::currentDateTime(Qt::UTC);
	usleep(FPCamerOA->ValeurTempsPoseDark()*100);
	DateHeureFinPoseImage=QDateTime::currentDateTime(Qt::UTC);
	
	if( CameraCCD->GetImageData(BufferPixelsPhysiquesImage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
	{
		std::cerr << "CamerOA: ERREUR: GetImageData(): pose image DARK." << std::endl;

		return false;
	}

	MutexDateHeureDebutPoseImage.unlock();
	MutexDateHeureFinPoseImage.unlock();
	MutexDureePoseImage.unlock();
	MutexBufferPixelsPhysiquesImage.unlock();

	if( CameraCCD->m_pvtRoiPixelsH == Largeur && CameraCCD->m_pvtRoiPixelsV == Hauteur ) return true; else return false;
}


// Fonction de calage temporel du depart d'une pose image
//
void ProcessusLegerControleCamera::CalageTemporel(void)
{
	QTime heure;	// Widget de l'heure

	heure=QTime::currentTime(Qt::UTC);

	// Si on est dans le module 0, il faut attendre sa fin, pour attendre le prochain
	//
	if( (heure.second() % MODULO_CALAGE_TEMPOREL_POSE) == 0 )
	{
		do
		{
			heure=QTime::currentTime(Qt::UTC);
		} while( (heure.second() % MODULO_CALAGE_TEMPOREL_POSE) == 0 );
	}

	// On attend le prochain module 0
	//
	do
	{
		heure=QTime::currentTime(Qt::UTC);
	} while( (heure.second() % MODULO_CALAGE_TEMPOREL_POSE) != 0 );
}


// Demande de prise d'image
//
// CE:	-
//
void ProcessusLegerControleCamera::DemandePoseImage(void)
{
	MutexDemandePoseI.lock();

	DemandePoseI=true;

	MutexDemandePoseI.unlock();
}


// Prise d'une image
//
// CE:	On passe une valeur vraie si la CCD doit voir la lumiere pour cette pose
//
// CS:	La fonction est vraie si l'image est bien transferee
//
int ProcessusLegerControleCamera::PoseImage(int lumiere)
{
	unsigned short Largeur=0;	// Nombre de colonnes lues
	unsigned short Hauteur=0;	// Nombre de lignes lues
	unsigned long Compteur=0;	// Nombre d'images lues

	// On videra en 12 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_TwelveBit);

	// On ne force plus l'ouverture de l'obturateur de maniere systematique
	//
	CameraCCD->write_ForceShutterOpen(false);

	// On patiente 1/100s le temps qu'il se ferme
	//
	usleep(10000);

	// On vide bien la camera en mode binning avant de faire une pose image
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningH(BINNING_POSE_VIDAGE_IMAGE);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningV(BINNING_POSE_VIDAGE_IMAGE);

	// Calage temporel du depart de pose
	//
	CalageTemporel();

	for( int vidage=0; vidage < NB_VIDAGE_POSE_IMAGE; vidage++ )
	{
		if( !CameraCCD->Expose(0.0,false) )
		{
			std::cerr << "CamerOA: ERREUR: Expose(): vidage pose image." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.lock();

		if( CameraCCD->GetImageData(BufferPixelsPhysiquesVidage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
		{
			std::cerr << "CamerOA: ERREUR: GetImageData(): vidage pose image." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.unlock();

//std::cout << "Pose VI:" << Largeur << "," << Hauteur << "," << Compteur << std::endl;
	}

	// On digitalisera en 16 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_SixteenBit);

	// On commence la pose image selon les consignes
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns;
	CameraCCD->write_RoiBinningH(1);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows;
	CameraCCD->write_RoiBinningV(1);

	MutexBufferPixelsPhysiquesImage.lock();
	MutexDateHeureDebutPoseImage.lock();
	MutexDateHeureFinPoseImage.lock();
	MutexDureePoseImage.lock();

	if( !CameraCCD->Expose(((double) (DureePoseImage=FPCamerOA->ValeurTempsPoseImage()))/10000.0,lumiere) )
	{
		std::cerr << "CamerOA: ERREUR: Expose(): pose image." << std::endl;

		return false;
	}

	DateHeureDebutPoseImage=QDateTime::currentDateTime(Qt::UTC);	// Expose() n'attend pas la fin de la pose pour revenir incertitude de ms
	usleep(FPCamerOA->ValeurTempsPoseImage()*100);					// On patiente pour obtenir la date de fin de pose
	DateHeureFinPoseImage=QDateTime::currentDateTime(Qt::UTC);

	if( CameraCCD->GetImageData(BufferPixelsPhysiquesImage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
	{
		std::cerr << "CamerOA: ERREUR: GetImageData(): pose image." << std::endl;

		return false;
	}


	MutexDateHeureDebutPoseImage.unlock();
	MutexDateHeureFinPoseImage.unlock();
	MutexDureePoseImage.unlock();
	MutexBufferPixelsPhysiquesImage.unlock();

//std::cout << "Pose I:" << Largeur << "," << Hauteur << "," << Compteur << std::endl;

	if( CameraCCD->m_pvtRoiPixelsH == Largeur && CameraCCD->m_pvtRoiPixelsV == Hauteur ) return true; else return false;
}


// Demande de prise d'image en pose doublee
//
// CE:	-
//
void ProcessusLegerControleCamera::DemandePoseDoubleImage(void)
{
	MutexDemandePoseDoubleI.lock();

	DemandePoseDoubleI=true;

	MutexDemandePoseDoubleI.unlock();
}


// Prise d'une image en pose doublee
//
// CE:	On passe une valeur vraie si la CCD doit voir la lumiere pour cette pose
//
// CS:	La fonction est vraie si l'image est bien transferee
//
int ProcessusLegerControleCamera::PoseDoubleImage(int lumiere)
{
	unsigned short Largeur=0;					// Nombre de colonnes lues
	unsigned short Hauteur=0;					// Nombre de lignes lues
	unsigned long i;							// Indice variable
	unsigned long Compteur=0;					// Nombre d'images lues
	unsigned short *BufferPremiereImage;		// Espace de stockage de la premiere pose image

	// On reserve l'espace pour la double pose
	//
	if( (BufferPremiereImage=new (std::nothrow) unsigned short[CameraCCD->m_TotalRows*CameraCCD->m_TotalColumns]) == NULL )
	{
		std::cerr << "CamerOA: ERREUR: Impossible d'allouer le buffer de la premiere image pour la pose doublee." << std::endl;
	}

	// On videra en 12 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_TwelveBit);

	// On ne force plus l'ouverture de l'obturateur de maniere systematique
	//
	CameraCCD->write_ForceShutterOpen(false);

	// On patiente 1/100s le temps qu'il se ferme
	//
	usleep(10000);

	// On vide bien la camera en mode binning avant de faire une pose image
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningH(BINNING_POSE_VIDAGE_IMAGE);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningV(BINNING_POSE_VIDAGE_IMAGE);

	// Calage temporel du depart de pose
	//
	CalageTemporel();

	for( int vidage=0; vidage < NB_VIDAGE_POSE_IMAGE; vidage++ )
	{
		if( !CameraCCD->Expose(0.0,false) )
		{
			std::cerr << "CamerOA: ERREUR: Expose(): premier vidage de la pose image doublee." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.lock();

		if( CameraCCD->GetImageData(BufferPixelsPhysiquesVidage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
		{
			std::cerr << "CamerOA: ERREUR: GetImageData(): premier vidage de la pose image doublee." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.unlock();
	}

	// On digitalisera en 16 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_SixteenBit);

	// On commence la pose premiere pose image selon les consignes
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns;
	CameraCCD->write_RoiBinningH(1);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows;
	CameraCCD->write_RoiBinningV(1);

	MutexBufferPixelsPhysiquesImage.lock();
	MutexDateHeureDebutPoseImage.lock();
	MutexDateHeureFinPoseImage.lock();
	MutexDureePoseImage.lock();

	if( !CameraCCD->Expose(((double) (DureePoseImage=FPCamerOA->ValeurTempsPoseImage()))/10000.0,lumiere) )
	{
		std::cerr << "CamerOA: ERREUR: Expose(): premiere pose image doublee." << std::endl;

		return false;
	}

	DateHeureDebutPoseImage=QDateTime::currentDateTime(Qt::UTC);
	usleep(FPCamerOA->ValeurTempsPoseImage()*100);
	DateHeureFinPoseImage=QDateTime::currentDateTime(Qt::UTC);

	if( CameraCCD->GetImageData(BufferPremiereImage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
	{
		std::cerr << "CamerOA: ERREUR: GetImageData(): premiere pose image doublee." << std::endl;

		delete [] BufferPremiereImage;

		return false;
	}

	//
	// La deuxieme pose immediatement derriere
	//

	// On videra en 12 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_TwelveBit);

	// On vide bien la camera en mode binning avant de faire une pose image
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningH(BINNING_POSE_VIDAGE_IMAGE);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows/BINNING_POSE_VIDAGE_IMAGE;
	CameraCCD->write_RoiBinningV(BINNING_POSE_VIDAGE_IMAGE);

	for( int vidage=0; vidage < 1; vidage++ )
	{
		if( !CameraCCD->Expose(0.0,false) )
		{
			std::cerr << "CamerOA: ERREUR: Expose(): deuxieme vidage de la pose image doublee." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.lock();

		if( CameraCCD->GetImageData(BufferPixelsPhysiquesVidage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
		{
			std::cerr << "CamerOA: ERREUR: GetImageData(): deuxieme vidage de la pose image doublee." << std::endl;
		}

		MutexBufferPixelsPhysiquesVidage.unlock();
	}

	// On digitalisera en 16 bits
	//
	CameraCCD->write_DataBits(Apn_Resolution_SixteenBit);

	// On commence la pose premiere pose image selon les consignes
	//
	CameraCCD->m_pvtRoiStartX=0;
	CameraCCD->m_pvtRoiPixelsH=CameraCCD->m_ImagingColumns;
	CameraCCD->write_RoiBinningH(1);

	CameraCCD->m_pvtRoiStartY=0;
	CameraCCD->m_pvtRoiPixelsV=CameraCCD->m_ImagingRows;
	CameraCCD->write_RoiBinningV(1);

	if( !CameraCCD->Expose(((double) FPCamerOA->ValeurTempsPoseImage())/10000.0,lumiere) )
	{
		std::cerr << "CamerOA: ERREUR: Expose(): deuxieme pose image doublee." << std::endl;

		return false;
	}

	if( CameraCCD->GetImageData(BufferPixelsPhysiquesImage,Largeur,Hauteur,Compteur) != CAPNCAMERA_SUCCESS )
	{
		std::cerr << "CamerOA: ERREUR: GetImageData(): deuxieme pose image doublee." << std::endl;

		delete [] BufferPremiereImage;

		return false;
	}

	// On calcule la moyenne des deux poses
	//
	unsigned short *pt1=BufferPixelsPhysiquesImage;
	unsigned short *pt2=BufferPremiereImage;

	for( i=0; i < (unsigned long) (Largeur*Hauteur); i++ )
	{
		unsigned long v=*pt1+*pt2;

		v>>=1;

		*pt1=(unsigned short) v;

		pt1++;
		pt2++;
	}

	MutexDateHeureDebutPoseImage.unlock();
	MutexDateHeureFinPoseImage.unlock();
	MutexDureePoseImage.unlock();
	MutexBufferPixelsPhysiquesImage.unlock();

	// Liberation de l'espace memoire de la premiere pose
	//
	delete [] BufferPremiereImage;


	if( CameraCCD->m_pvtRoiPixelsH == Largeur && CameraCCD->m_pvtRoiPixelsV == Hauteur ) return true; else return false;
}


//---------------------------------------------------------------------------------------------------------------------

// Constructeur du processus leger serveur chiffre monoclient d'attente des commandes par le reseau
//
// CE:	On passe un pointeur vers la fenetre principale de l'application CamerOA ;
//
//		On passe l'adresse IP d'attachement en valeur host (0x________) ;
//
//		On passe le port d'attachement en valeur host (0x____) ;
//
//		On passe l'adresse IP du client autorise en valeur host (0x________) ;
//
//		On passe le timeout en secondes pour la tentative d'ecriture de donnees dans la socket ;
//
//		On passe le timeout en secondes pour la tentative de lecture de donnees dans la socket ;
//
//		On passe le timeout en secondes de l'initiative de la negociation TLS/SSL ;
//
//		On passe un pointeur sur la fonction C de handler du signal SIGPIPE ;
//
//		On passe un pointeur sur une chaine de char qui contient le mot de passe pour acceder a la cle privee du serveur SSL
//		 Ce mot de passe ne doit pas contenir plus de TAILLE_MAX_MDP_CLE_PRIVEE-1 caracteres ;
//
//		On passe un pointeur sur char vers un buffer de stockage du mot de passe pour acceder a la cle privee du serveur SSL
//		 Ce buffer doit etre reserve avec TAILLE_MAX_MDP_CLE_PRIVEE elements ;
//
//		On passe un pointeur sur la fonction C appelee par la librairie SSL lors de la demande du mot de passe pour acceder a la cle
//		 privee du serveur SSL stockee dans un fichier PEM ;
//
//		On passe un pointeur sur une chaine de char qui contient le chemin complet du fichier PEM du certificat
//		 de l'autorite de certification CA qui a signe les certificats du serveur ;
//
//		On passe un pointeur sur une chaine de char qui contient le chemin complet du fichier PEM du certificat du serveur ;
//
//		On passe un pointeur sur une chaine de char qui contient le chemin complet du fichier PEM de la cle privee du serveur ;
//
//		On passe un pointeur sur une chaine de char qui contient le chemin complet du fichier PEM des parametres Diffie-Hellman aleatoires ;
//
//		On passe la chaine de la liste des chiffreurs que le serveur doit utiliser ;
//
// CS:	-
//
ProcessusLegerServeurCommandes::ProcessusLegerServeurCommandes(CamerOA *papp,uint32_t pAdresse,uint16_t pPort,uint32_t pAdresseClient,int pNbLClientsMax,int pTimeoutSocketPut,int pTimeoutSocketGet,int pTimeoutNegoTLSSSL,void (*pFnHandlerSIGPIPE)(int),const char *MdpClePriveeServeur,char *BuffStockMdpClePriveeServeur,int (*pFnMotDePasseClePriveeChiffree)(char*, int, int, void*),const char *pCheminCertificatCA,const char *pCheminCertificatServeur,const char *pCheminClePriveeServeur,const char *pParametresDH,const char *pListeChiffreurs)
{
	// Initialisation des variables
	//
	DrapeauDemandeTerminaison=false;
	
	// Instanciation de l'objet serveur chiffre monoclient
	//
	if( (Serveur=new (std::nothrow) PointCommServeurChiffreMonoClient(false,pAdresse,pPort,pAdresseClient,pNbLClientsMax,pTimeoutSocketPut,pTimeoutSocketGet,pTimeoutNegoTLSSSL,true,pFnHandlerSIGPIPE,MdpClePriveeServeur,BuffStockMdpClePriveeServeur,pFnMotDePasseClePriveeChiffree,pCheminCertificatCA,pCheminCertificatServeur,pCheminClePriveeServeur,pParametresDH,pListeChiffreurs)) == NULL )
	{
		std::cerr << "ProcessusLegerServeurCommandes: ERREUR: Impossible de creer l'objet serveur chiffre monoclient." << std::endl;
	}
	
	// Pointeur vers la fenetre principale de l'application CamerOA
	//
	FPCamerOA=papp;
}


// Destructeur du processus leger serveur chiffre monoclient d'attente des commandes par le reseau
//
ProcessusLegerServeurCommandes::~ProcessusLegerServeurCommandes()
{
	// Si le thread est encore actif et donc que le serveur ne s'est pas termine normalement
	//
	if( running() )
	{
		terminate();	// On termine le thread de maniere brutale
	}
	
	// On supprime l'objet Serveur
	//
	delete Serveur;
}


// Fonction de la demande de terminaison propre du processus leger
//
void ProcessusLegerServeurCommandes::DemandeTerminaison(void)
{
	DrapeauDemandeTerminaison=true;
	
	// Les sockets passent en mode non bloquant
	//
	Serveur->SocketNonBloquante();
	Serveur->SocketSessionNonBloquante();
}


// Surcharge de la methode run() qui contient le code d'execution du thread
//  du serveur chiffre monoclient d'attente des commandes par le reseau
//
void ProcessusLegerServeurCommandes::run()
{
	int Sortir=false;	// Drapeau d'indication de sortie de la boucle de traitement des demandes de connexion
	
	// On capture un element du semaphore de synchronisation
	//
	SemaphoreSyncLancementThreadCamerOA++;
	
	// On lance l'ecoute reseau systeme (socket normale) en attente d'une connexion sur le port du service IP:Port
	//
	// Chaque connexion individuelle sera traitee dans la boucle d'acceptation des sessions qui va suivre
	//
	if( !Serveur->EcouteReseau() ) return;
	
	// Traitement de chaque demande de connexion
	//
	do
	{
		// Attente et acceptation d'une requete de connexion par un client autorise
		//
		if( Serveur->PossibleLireDonneesSocket(100) )		// S'il y a une tentative de connexion a lire sous 100ms (TRES IMPORTANT POUR NE PAS RESTER BLOQUE SUR accept() )
		if( Serveur->AttenteAccepterSessionAutorisee() )	// Il y a une demande de connexion a lire alors on accept()
		{
			// Si il y a eu une demande de connexion et que le client est autorise
			//
			if( Serveur->SessionAccepteeAutorisee() )
			{
				// Si on a demande au processus de terminer proprement son execution
				//
				if( !DrapeauDemandeTerminaison )
				{
					// On demande l'initiation de la negociation SSL
					//
					if( Serveur->NegociationConnexionSSL() )
					{
						// Si on a demande au processus de terminer proprement son execution
						//
						if( !DrapeauDemandeTerminaison )
						{
							int BoucleCommandes=true;	// Indicateur de l'etat de la boucle d'attente des  commandes du client
							int SesameOuvreToi=false;	// Chaine mot de passe pour Login
											//  et accepter les commandes clients
							unsigned long CompteurEnAttente=0;	// Compteur de commandes en attentes recues
							QString ChaineGets;		// Chaine recue
							ChaineGets.reserve(TAILLE_MINI_CHAINE);
							QString ChainePuts;		// Chaine pour BIO_puts()
							ChainePuts.reserve(TAILLE_MINI_CHAINE);
							char ChaineRecue[TAILLE_MAXI_CHAINE_BIO];	// Chaine recues par BIO_gets()
							
							// Chaine de bienvenue au client
							//
							ChainePuts="Bienvenue sur CamerOA Canal des Commandes (Chiffrement:"+QString(SSL_CIPHER_get_name(Serveur->DescripteurChiffreurConnexionSSL()))+", Protocoles:"+QString(SSL_CIPHER_get_version(Serveur->DescripteurChiffreurConnexionSSL()))+")\n";
							
							if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
							
							// Boucle d'attente des commandes du client
							//
							while( BoucleCommandes && !DrapeauDemandeTerminaison )
							{
								int RetourGets;		// Valeur retournee par la fonction de reception
								long Valeur1;		// Valeur pour extraction de parametre
								QString ParamChaine1;	// Chaine pour extraction de parametre
								ParamChaine1.reserve(TAILLE_MINI_CHAINE);
								
								if( (RetourGets=Serveur->RecevoirChaineBIO(ChaineRecue)) > 0 )
								{
									int i=0;		// Variable indice
									int IdCmdClient;	// Id de commande client
									
									// On coupe la chaine au premier \r ou \n
									//
									// On supprime les espacements multiples  ATTENTION: TRES IMPORTANT POUR QUE QString.section(" ",,) FONCTIONNE CORRECTEMENT
									//
									ChaineGets="";
									while( ChaineRecue[i] != 0 )
									{
										if( ChaineRecue[i] == '\r' ) break;
										if( ChaineRecue[i] == '\n' ) break;
										if( ChaineRecue[i] != ' ' )
										{
											ChaineGets+=ChaineRecue[i];
										}
										else
										{
											// Le caractere courant est un espacement, on ne l'ajoute pas si le dernier caractere de ChaineGets est aussi un espacement
											//
											if( ChaineGets.length() > 0 )
											{
												if( ChaineGets.right(1) != " " ) ChaineGets+=ChaineRecue[i];
											}
										}
										i++;
									}
//std::cout << "Ligne recue propre:" << ChaineGets << std::endl;

									if( SesameOuvreToi && Serveur->ObjetModeVerbeux() ) std::cout << "CamerOA: S<-C " << RetourGets << ": " << ChaineGets << std::endl;

									// Recherche de la commande dans la liste
									//
									for( IdCmdClient=0; ListeCmdClientCamerOA[IdCmdClient] != QString(""); IdCmdClient++ ) if( RetourGets > 2 ) if( ListeCmdClientCamerOA[IdCmdClient] == ChaineGets.left(ListeCmdClientCamerOA[IdCmdClient].length()) ) break;
									
									// Si la chaine login/mdp sesame n'a pas ete recue
									//
									if( !SesameOuvreToi )
									{
										if( IdCmdClient == CAMEROA_CMD_SESAMEOUVRETOI )
										{
											SesameOuvreToi=true;
										}
										
										IdCmdClient=-1;
									}
									
									// Selon l'identifieur de la commande
									//
									switch( IdCmdClient )
									{
										case CAMEROA_CMD_ARRET:
											//
											// Arret et sortie du logiciel
											//
											{
												CEventCamerOA_Quit *event=new CEventCamerOA_Quit();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											break;
										
										case CAMEROA_CMD_DECONNEXION:
											//
											// Deconnexion du client
											//
											BoucleCommandes=false;
											break;
										
										case CAMEROA_CMD_EN_ATTENTE:
											//
											// Serveur en attente, en vie ?
											//
											CompteurEnAttente++;
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_EN_ATTENTE]+QString("%1\n").arg(CompteurEnAttente);
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_CTEMP:
											//
											// Fixer la consigne de temperature
											//
											ParamChaine1=ChaineGets.right(ChaineGets.length()-ListeCmdClientCamerOA[IdCmdClient].length());
										
											Valeur1=ParamChaine1.toLong();
										
											if( Valeur1 >= MIN_CTEMP && Valeur1 <= MAX_CTEMP ) 
											{
												CEventCamerOA_CTemp *event=new CEventCamerOA_CTemp(Valeur1);

												QApplication::postEvent(FPCamerOA,event);
										
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_CTEMP];
												ChainePuts.append("\n");
											}
											else
											{
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_NONOK_CTEMP];
												ChainePuts.append("\n");
											}
											
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_CTPI:
											//
											// Fixer la consigne de temps de pose image
											//
											ParamChaine1=ChaineGets.right(ChaineGets.length()-ListeCmdClientCamerOA[IdCmdClient].length());
										
											Valeur1=ParamChaine1.toLong();
										
											if( Valeur1 >= MIN_TPIMAGE && Valeur1 <= MAX_TPIMAGE ) 
											{
												CEventCamerOA_CTPI *event=new CEventCamerOA_CTPI(Valeur1);

												QApplication::postEvent(FPCamerOA,event);
										
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_CTPI];
												ChainePuts.append("\n");
											}
											else
											{
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_NONOK_CTPI];
												ChainePuts.append("\n");
											}
											
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_CTPC:
											//
											// Fixer la consigne de temps de pose centrage
											//
											ParamChaine1=ChaineGets.right(ChaineGets.length()-ListeCmdClientCamerOA[IdCmdClient].length());
										
											Valeur1=ParamChaine1.toLong();
										
											if( Valeur1 >= MIN_TPCENTRAGE && Valeur1 <= MAX_TPCENTRAGE ) 
											{
												CEventCamerOA_CTPC *event=new CEventCamerOA_CTPC(Valeur1);

												QApplication::postEvent(FPCamerOA,event);
										
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_CTPC];
												ChainePuts.append("\n");
											}
											else
											{
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_NONOK_CTPC];
												ChainePuts.append("\n");
											}
											
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_CTPD:
											//
											// Fixer la consigne de temps de pose d'un DARK
											//
											ParamChaine1=ChaineGets.right(ChaineGets.length()-ListeCmdClientCamerOA[IdCmdClient].length());
										
											Valeur1=ParamChaine1.toLong();
										
											if( Valeur1 >= MIN_TPIMAGE && Valeur1 <= MAX_TPIMAGE ) 
											{
												CEventCamerOA_CTPD *event=new CEventCamerOA_CTPD(Valeur1);

												QApplication::postEvent(FPCamerOA,event);
										
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_CTPD];
												ChainePuts.append("\n");
											}
											else
											{
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_NONOK_CTPD];
												ChainePuts.append("\n");
											}
											
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_POSEI:
											//
											// Demande d'une pose image
											//
											{
												CEventCamerOA_PoseImage *event=new CEventCamerOA_PoseImage();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_POSEI];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_POSEDOUBLEI:
											//
											// Demande d'une pose double image
											//
											{
												CEventCamerOA_PoseDoubleImage *event=new CEventCamerOA_PoseDoubleImage();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_POSEDOUBLEI];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_POSEIBIAS:
											//
											// Demande d'une pose de BIAS
											//
											{
												CEventCamerOA_PoseBias *event=new CEventCamerOA_PoseBias();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_POSEIBIAS];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_POSEIDARK:
											//
											// Demande d'une pose de DARK
											//
											{
												CEventCamerOA_PoseDark *event=new CEventCamerOA_PoseDark();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_POSEIDARK];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_ARRETPOSE:
											//
											// Demande de l'arret d'une pose
											//
											{
												CEventCamerOA_ArretPose *event=new CEventCamerOA_ArretPose();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_ARRETPOSE];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_FSHUTTEROPEN:
											//
											// Demande d'obturateur ouvert
											//
											{
												CEventCamerOA_ForceShutterOpen *event=new CEventCamerOA_ForceShutterOpen();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_FSHUTTEROPEN];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_FSHUTTERCLOSE:
											//
											// Demande d'obturateur ferme
											//
											{
												CEventCamerOA_ForceShutterClose *event=new CEventCamerOA_ForceShutterClose();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_FSHUTTERCLOSE];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_MODEPOSECAOBTU:
											//
											// Demande de mode pose centrage avec obturateur
											//
											{
												CEventCamerOA_UtiliserShutterCentrage *event=new CEventCamerOA_UtiliserShutterCentrage();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_MODEPOSECAOBTU];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_MODEPOSECSOBTU:
											//
											// Demande de mode pose centrage sans obturateur
											//
											{
												CEventCamerOA_NonUtiliserShutterCentrage *event=new CEventCamerOA_NonUtiliserShutterCentrage();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_MODEPOSECSOBTU];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_FANLOW:
											//
											// Demande de ventilateur low
											//
											{
												CEventCamerOA_FanLow *event=new CEventCamerOA_FanLow();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_FANLOW];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_FANMEDIUM:
											//
											// Demande de ventilateur medium
											//
											{
												CEventCamerOA_FanMedium *event=new CEventCamerOA_FanMedium();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_FANMEDIUM];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_FANHIGH:
											//
											// Demande de ventilateur high
											//
											{
												CEventCamerOA_FanHigh *event=new CEventCamerOA_FanHigh();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_FANHIGH];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_FANOFF:
											//
											// Demande de l'arret des ventilateurs
											//
											{
												CEventCamerOA_FanOff *event=new CEventCamerOA_FanOff();

												QApplication::postEvent(FPCamerOA,event);
											}
										
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_FANOFF];
											ChainePuts.append("\n");
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;

										case CAMEROA_CMD_TEMPCCD:
											//
											// Demande de la temperature du CCD
											//
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_TEMPCCD]+threadCamera->ValeurDateHeureTemperatureCCD().toString(Qt::ISODate)+QString(" %1\n").arg(threadCamera->ValeurTemperatureCCD(),6,'f',2);
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_TEMPRADIATEUR:
											//
											// Demande de la temperature du radiateur
											//
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_TEMPRADIATEUR]+threadCamera->ValeurDateHeureTemperatureRadiateur().toString(Qt::ISODate)+QString(" %1\n").arg(threadCamera->ValeurTemperatureRadiateur(),6,'f',2);
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;
										
										case CAMEROA_CMD_PUIPELTIER:
											//
											// Demande de la puissance des Peltiers
											//
											ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_PUIPELTIER]+threadCamera->ValeurDateHeurePuissancePeltier().toString(Qt::ISODate)+QString(" %1\n").arg(threadCamera->ValeurPuissancePeltier(),6,'f',2);
										
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;

										case CAMEROA_CMD_PARAMAMPLIPROFH:
											//
											// Fixer les parametres d'amplifaction des details du profil horizontal
											//
											{
												QString arg1,arg2,arg3;		// Les arguments de la commande envoyee
												double Valeur2,Valeur3;

												ParamChaine1=ChaineGets.right(ChaineGets.length()-ListeCmdClientCamerOA[IdCmdClient].length());

												arg1=ParamChaine1.section(" ",0,0);		// largeur matrice en pixels
												arg2=ParamChaine1.section(" ",1,1);		// % profil amplifie
												arg3=ParamChaine1.section(" ",2,2);		// facteur d'amplification

												Valeur1=arg1.toLong();
												Valeur2=arg2.toDouble();
												Valeur3=arg3.toDouble();
										
												if( Valeur1 >= MIN_MATRICE_AMP_PROF && Valeur1 <= MAX_MATRICE_AMP_PROF && Valeur2 >= MIN_LARG_ZA_PROF && Valeur2 <= MAX_LARG_ZA_PROF && Valeur3 >= MIN_AMPLI_PROF && Valeur3 <= MAX_AMPLI_PROF ) 
												{
													CEventCamerOA_ParamAmpliProfilH *event=new CEventCamerOA_ParamAmpliProfilH(Valeur1,Valeur2,Valeur3);

													QApplication::postEvent(FPCamerOA,event);
										
													ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_PARAMAMPLIPROFH];
													ChainePuts.append("\n");
												}
												else
												{
													ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_NONOK_PARAMAMPLIPROFH];
													ChainePuts.append("\n");
												}
											}
											
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;

										case CAMEROA_CMD_PARAMAMPLIPROFV:
											//
											// Fixer les parametres d'amplifaction des details du profil vertical
											//
											{
												QString arg1,arg2,arg3;		// Les arguments de la commande envoyee
												double Valeur2,Valeur3;

												ParamChaine1=ChaineGets.right(ChaineGets.length()-ListeCmdClientCamerOA[IdCmdClient].length());

												arg1=ParamChaine1.section(" ",0,0);		// largeur matrice en pixels
												arg2=ParamChaine1.section(" ",1,1);		// % profil amplifie
												arg3=ParamChaine1.section(" ",2,2);		// facteur d'amplification

												Valeur1=arg1.toLong();
												Valeur2=arg2.toDouble();
												Valeur3=arg3.toDouble();
										
												if( Valeur1 >= MIN_MATRICE_AMP_PROF && Valeur1 <= MAX_MATRICE_AMP_PROF && Valeur2 >= MIN_LARG_ZA_PROF && Valeur2 <= MAX_LARG_ZA_PROF && Valeur3 >= MIN_AMPLI_PROF && Valeur3 <= MAX_AMPLI_PROF ) 
												{
													CEventCamerOA_ParamAmpliProfilV *event=new CEventCamerOA_ParamAmpliProfilV(Valeur1,Valeur2,Valeur3);

													QApplication::postEvent(FPCamerOA,event);
										
													ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_OK_PARAMAMPLIPROFV];
													ChainePuts.append("\n");
												}
												else
												{
													ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_NONOK_PARAMAMPLIPROFV];
													ChainePuts.append("\n");
												}
											}
											
											if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
										
											break;

										default:
											// Si la chaine login/mdp sesame est recue
											//
											if( SesameOuvreToi && IdCmdClient != -1 )
											{
												ChainePuts=ListeRepServeurCamerOA[CAMEROA_REP_CMD_INCONNUE];
												ChainePuts.append("\n");
										
												if( Serveur->EnvoyerChaineBIO(ChainePuts.ascii()) <= 0 ) BoucleCommandes=false;
											}
											break;
									}
								}
								else
								{
									BoucleCommandes=false;
								}
							}
							
							// Fermeture de la connexion SSL courante
							//
							Serveur->FermetureConnexionSSL();
						}
					}
				}
			}
		}
		else
		{
			Sortir=true;
		}
		
		if( DrapeauDemandeTerminaison ) Sortir=true;
		
	} while( !Sortir );
}


// - Thread ---------------------------------------------------------------------------------------------------------------------

// Constructeur du processus leger serveur non chiffre monoclient d'attente/emission des donnees par le reseau
//
// CE:	On passe un pointeur vers la fenetre principale de l'application CamerOA ;
//
//	On passe l'adresse IP d'attachement en valeur host (0x________) ;
//
//	On passe le port d'attachement en valeur host (0x____) ;
//
//	On passe l'adresse IP du client autorise en valeur host (0x________) ;
//
//	On passe le timeout en secondes pour la tentative d'ecriture de donnees dans la socket ;
//
//	On passe le timeout en secondes pour la tentative de lecture de donnees dans la socket ;
//
// CS:	-
//
ProcessusLegerServeurDonnees::ProcessusLegerServeurDonnees(CamerOA *papp,uint32_t pAdresse,uint16_t pPort,uint32_t pAdresseClient,int pNbLClientsMax,int pTimeoutSocketPut,int pTimeoutSocketGet)
{
	// Initialisation des variables
	//
	DrapeauDemandeTerminaison=false;
	
	// Instanciation de l'objet serveur non chiffre monoclient
	//
	if( (Serveur=new (std::nothrow) PointCommServeurNonChiffreMonoClient(false,pAdresse,pPort,pAdresseClient,pNbLClientsMax,pTimeoutSocketPut,pTimeoutSocketGet)) == NULL )
	{
		std::cerr << "ProcessusLegerServeurDonnees: ERREUR: Impossible de creer l'objet serveur non chiffre monoclient." << std::endl;
	}
	
	// Pointeur vers l'application CamerOA
	//
	FPCamerOA=papp;
}


// Destructeur du processus leger serveur non chiffre monoclient d'attente des donnees par le reseau
//
ProcessusLegerServeurDonnees::~ProcessusLegerServeurDonnees()
{
	// Si le thread est encore actif et donc que le serveur ne s'est pas termine normalement
	//
	if( running() )
	{
		terminate();	// On termine le thread de maniere brutale
	}
	
	// On supprime l'objet Serveur
	//
	delete Serveur;
}


// Fonction de la demande de terminaison propre du processus leger
//
void ProcessusLegerServeurDonnees::DemandeTerminaison(void)
{
	DrapeauDemandeTerminaison=true;
	
	// Les sockets passent en mode non bloquant
	//
	Serveur->SocketNonBloquante();
	Serveur->SocketSessionNonBloquante();
}


// Surcharge de la methode run() qui contient le code d'execution du thread
//  du serveur non chiffre monoclient d'attente des donnees
//
void ProcessusLegerServeurDonnees::run()
{
	int Sortir=false;			// Drapeau d'indication de sortie de la boucle de traitement des demandes de connexion
	
	// On capture un element du semaphore de synchronisation
	//
	SemaphoreSyncLancementThreadCamerOA++;
	
	// On lance l'ecoute reseau systeme (socket normale) en attente d'une connexion sur le port du service IP:Port
	//
	// Chaque connexion individuelle sera traitee dans la boucle d'acceptation des sessions qui va suivre
	//
	if( !Serveur->EcouteReseau() ) return;
	
	// Traitement de chaque demande de connexion
	//
	do
	{
		// Attente et acceptation d'une requete de connexion par un client autorise
		//
		if( Serveur->PossibleLireDonneesSocket(100) )		// S'il y a une tentative de connexion a lire sous 100ms (TRES IMPORTANT POUR NE PAS RESTER BLOQUE SUR accept() )
		if( Serveur->AttenteAccepterSessionAutorisee() )	// Il y a une demande de connexion a lire alors on accept()
		{
			// Si il y a eu une demande de connexion et que le client est autorise
			//
			if( Serveur->SessionAccepteeAutorisee() )
			{
				// Si on a demande au processus de terminer proprement son execution
				//
				if( !DrapeauDemandeTerminaison )
				{
					int BoucleDonnees=true;		// Indicateur de l'etat de la boucle d'attente des donnees du client
					QString ChainePuts;			// Chaine pour BIO_puts()
					ChainePuts.reserve(TAILLE_MINI_CHAINE);

					// Chaine de bienvenue au client
					//
					ChainePuts="Bienvenue sur CamerOA Canal des Donnees\n";
					
					if( Serveur->EnvoyerChaineSocketSession(ChainePuts.ascii()) <= 0 ) BoucleDonnees=false;
					
					// Boucle d'attente des donnees du client
					//
					while( BoucleDonnees && !DrapeauDemandeTerminaison )
					{
						// Si des donnees sont disponibles a lire sur la socket de session dans les 1s qui suivent
						// Cette commande permet de ne pas rester bloquer tout le temps et de tourner dans la boucle du thread
						//
						if( Serveur->PossibleLireDonneesSocketSession(1000) )
						{
							int nbe;								// Le nombre d'octets lus
							char Buffer[TAILLE_MAX_BUFFER_LECTURE];

							// Ce serveur ne va servir qu'a envoyer des donnees, aussi, toutes les donnees qu'il peut recevoir seront
							//  lues et non utilisees
							//
							if( (nbe=Serveur->LireDonneesSocketSession(Buffer,TAILLE_MAX_BUFFER_LECTURE)) == -1 )
							{
								std::cerr << "ProcessusLegerServeurDonnees: LireDonneesSocketSession : ERREUR: Erreur lors de la lecture des donnees sur la socket de session." << std::endl;
							}

							if( nbe == 0 )
							{
								// Il y avait quelque chose a lire mais la fonction n'a rien lue, la socket est sans doute fermee
								//  du cote du client donc on peut fermer la session courante et attendre les prochaines connexions
								//
								BoucleDonnees=false;
							}
						}
					}
					
					// Fermeture de la connexion courante
					//
					Serveur->FermetureSession();
				}
			}
		}
		else
		{
			Sortir=true;
		}
		
		if( DrapeauDemandeTerminaison ) Sortir=true;
		
	} while( !Sortir );
}


#include "cameroa.moc"
