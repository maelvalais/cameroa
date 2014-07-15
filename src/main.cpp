/* MODULE PRINCIPAL DE L'APPLICATION KDE : CamerOA

   LOGICIEL RESEAU DE CONTROLE DE CAMERA CCD

   (C)David.Romeuf@univ-lyon1.fr 09/05/2006 par David Romeuf
*/

// Inclusions C++
//
#include <iostream>
#include <new>

// Inclusions Qt et KDE
//
#include <qdir.h>
#include <qcstring.h>
#include <qmutex.h>
#include <qsemaphore.h>
#include <qstring.h>
#include <qwidget.h>
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

// Inclusions de l'applications
//
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include "cameroa.h"

// Inclusions pour la camera
//
#include "Apogee.h"
#include "ApnCamera.h"
#include "SimulateurApogee.h"


// Declarations
//

static const char description[] = I18N_NOOP("A KDE KPart Application");
static const char version[] = "1.0";

// Liste des options supportees par la ligne de commande
//
enum ArgumentsApplication {
	CheminRepCamerOA,
	CheminFichCertCA_OA,
	CheminFichCertServCamerOA,
	CheminFichClePriveeServCamerOA,
	CheminFichParamDH,
	MdpClePriveeServeur,
	AdresseClientAutorise,
	PortCanalCommandes,
	PortCanalDonnees,
	ArretSysteme
};

const char *OptionsLC[] = {
	"chemcameroa",
	"chemficaoa",
	"chemficertserveur",
	"chemficleprivserveur",
	"chemfiparamdh",
	"mdpcleprivserveur",
	"adresseclientautorise",
	"portcanalcommandes",
	"portcanaldonnees",
	"arretsysteme"
};

static KCmdLineOptions options[] =
{
	{"chemcameroa ",I18N_NOOP("Chemin vers le repertoire de CamerOA"),"/CamerOA"},
	{"chemficaoa ",I18N_NOOP("Chemin et nom du certificat (PEM) du CA des OA"),"/CamerOA/ssl/CertificatCA_OA.pem"},
	{"chemficertserveur ",I18N_NOOP("Chemin et nom du certificat de ce serveur CamerOA"),"/CamerOA/ssl/CertificatServeurCamerOA.pem"},
	{"chemficleprivserveur ",I18N_NOOP("Chemin et nom du fichier PEM contenant la cle privee de ce serveur CamerOA"),"/CamerOA/ssl/ClePriveeServeurCamerOA.pem"},
	{"chemfiparamdh ",I18N_NOOP("Chemin et nom du fichier PEM contenant l'alea des parametres Diffie-Hellman de ce serveur"),"/CamerOA/ssl/Parametres-Diffie-Hellman-CamerOA.pem"},
	{"mdpcleprivserveur ",I18N_NOOP("Mot de passe d'acces a la cle privee de ce serveur"),"???"},
	{"adresseclientautorise ",I18N_NOOP("Adresse IP du client autorise sous la forme x.x.x.x"),"192.168.6.1"},
	{"portcanalcommandes ",I18N_NOOP("Numero du port tcp pour le canal des commandes"),"33443"},
	{"portcanaldonnees ",I18N_NOOP("Numero du port tcp pour le canal des donnees"),"33444"},
	{"arretsysteme ",I18N_NOOP("Lancement de l'arret du systeme en quittant CamerOA"),"n"},
//	{" ", I18N_NOOP(""),""},
	KCmdLineLastOption
};


// - Pour les processus legers ---------------------------------------------------------------------------------------------------------

// Temps d'attente de la terminaison d'un processus leger generique
//
#define TEMPS_ATTENTE_TERMINAISON_PROCESSUS_LEGER	10000

// Temps d'attente de la terminaison d'un processus leger serveur reseau
//
#define TEMPS_ATTENTE_TERMINAISON_PROCESSUS_SERVEUR	10000

#define TIMEOUT_EMISSION	5	// Timeout des liaisons pour les canaux de commandes et de donnees
#define TIMEOUT_RECEPTION	120

char MotDePasseClePriveeServeurCAMEROA[TAILLE_MAX_MDP_CLE_PRIVEE];	// Mot de passe pour acces a la cle privee du serveur

// FONCTION APPELEE PAR LA LIBRAIRIE SSL POUR LIRE OU STOCKER LES FICHIERS PEM CONTENANT UNE CLE CHIFFREE
//
// CE:	La librairie SSL passe un pointeur vers le tableau ou doit etre copie le mot de passe ;
//
// 	La librairie SSL passe la dimension maximale du mot de passe ;
//
// 	La librairie SSL passe 0 si la fonction est utilisee pour lire/decryptee, ou, 1 si la fonction est appelee pour ecrire/encryptee
//
// 	La librairie SSL passe un pointeur vers une donnee passee par la routine PEM. Il permet qu'une donnee arbitraire soit passee
// 	 a cette fonction par une application (comme par exemple un identifieur de fenetre dans une application graphique).
//
// CS:	La fonction doit retourner la longueur du mot de passe.
//
int FnMotDePasseClePriveeChiffreeCAMEROA(char *buf,int size,int rwflag,void *data)
{
	rwflag=rwflag;	// pour eviter un warning lors de la compilation
	data=data;
	
	if( size < (int) (strlen(MotDePasseClePriveeServeurCAMEROA)+1) ) return 0;

	strcpy(buf,MotDePasseClePriveeServeurCAMEROA);

	return( strlen(buf) );
}

// FONCTION DE HANDLER DU SIGNAL SIGPIPE
//
// CE:	Le systeme passe le signal ;
//
// CS:	-
//
void FnHandlerSIGPIPECamerOA(int signal)
{
	std::cerr << "PointCommServeurChiffreMonoClient: Signal " << signal << "->SIGPIPE<- recu par le processus." << std::endl;
}

QSemaphore SemaphoreSyncLancementThreadCamerOA(3);

// - Fin Pour les processus legers -----------------------------------------------------------------------------------------------------


// Declarations pour la camera :
//
// Comme il s'agit d'un unique peripherique physique nous declarons en globale
//
#define NumeroCamera	1

// Pointeur sur un objet camera du driver Apogee
#ifndef _SIMULATEUR_APOGEE
CApnCamera *CameraCCD;
#else
SimulateurApogeeUSB *CameraCCD;
#endif

// Pointeur sur le buffer pixels physiques du capteur (physique et pas image) d'une pose image (image, BIAS, DARK)
//
unsigned short *BufferPixelsPhysiquesImage;
QMutex MutexBufferPixelsPhysiquesImage;

// Pointeur sur le buffer pixels physiques du capteur (physique et pas image) de vidage du CCD pour differencier la zone memoire
//
unsigned short *BufferPixelsPhysiquesVidage;
QMutex MutexBufferPixelsPhysiquesVidage;

// Pointeur sur le buffer pixels physiques du capteur (physique et pas image) d'une pose de centrage
//
unsigned short *BufferPixelsPhysiquesCentrage;
QMutex MutexBufferPixelsPhysiquesCentrage;

// Pointeur sur le buffer pixels d'une pose de centrage horizontal
//
unsigned short *BufferPixelsPCH;
QMutex MutexBufferPixelsPCH;

// Pointeur sur le buffer pixels d'une pose de centrage vertical
//
unsigned short *BufferPixelsPCV;
QMutex MutexBufferPixelsPCV;


// Fonction principale de l'application
//
int main(int argc, char **argv)
{
	int lancement=true;			// Drapeau pour savoir si on doit lancer l'application
	int ArretSystemeEnQuittant=false;	// Drapeau pour le lancement de l'arret du systeme en quittant l'application
	int retour=0;				// Valeur de retour de la fin de l'application
	
	// Renseignements KDE
	//
	KAboutData about("cameroa", I18N_NOOP("CamerOA"), version, description,KAboutData::License_GPL, "(C) 2006 David Romeuf", 0, 0, "David.Romeuf@univ-lyon1.fr");
	about.addAuthor( "David Romeuf", 0, "David.Romeuf@univ-lyon1.fr" );
	
	
	// Initialisation des options de la ligne de commande (avec les Qt et KDE specifiques)
	//
	KCmdLineArgs::init(argc, argv, &about);
	
	// Ajout des options possibles sur la ligne de commande supportees par l'application
	//
	KCmdLineArgs::addCmdLineOptions(options);
	
	// Acces aux arguments reconnus par l'application
	//
	KCmdLineArgs *arguments=KCmdLineArgs::parsedArgs();
	
	// On test la validite des arguments
	//
	if( !QDir(arguments->getOption(OptionsLC[CheminRepCamerOA])).exists() )
	{
		std::cerr << "CamerOA: ERREUR: Le repertoire " << arguments->getOption(OptionsLC[CheminRepCamerOA]) << " n'existe pas." << std::endl;
		lancement=false;
	}
	
	struct in_addr AdresseClient;
	
	if( !inet_aton(arguments->getOption(OptionsLC[AdresseClientAutorise]),&AdresseClient) )
	{
		std::cerr << "CamerOA: ERREUR: L'adresse du client autorise " << arguments->getOption(OptionsLC[AdresseClientAutorise]) << "est invalide." << std::endl;
		lancement=false;
	}
	AdresseClient.s_addr=ntohl(AdresseClient.s_addr);
	
	if( QString(arguments->getOption(OptionsLC[ArretSysteme])) == QString("o") ) ArretSystemeEnQuittant=true;
	
	
	if( lancement )
	{
		// Instanciation de l'objet camera :
		//
		// ApnCamera.cpp contient le code de la classe CApnCamera definit dans ApnCamera.h commun
		//  a toutes les versions d'interfaces peripheriques (USB, Ethernet). Aucun code n'est
		//  specialise USB/Linux.
		//
		// ApnCamera_USB.cpp contient le code de la classe CApnCamera definit dans ApnCamera.h
		//  specialise pour la version USB (par exemple la methode InitDriver() qui n'est pas
		//  codee dans ApnCamera.cpp).
		//
		// ApogeeUsbLinux.cpp contient toutes les fonctions de communication via USB pour Linux
		//  utilisees par l'objet CApnCamera (par exemple : ApnUsbOpen() qui recherche les
		//  peripheriques par Id vendeur, ApnUsbClose()).
		//
#ifndef _SIMULATEUR_APOGEE
		CameraCCD=new CApnCamera();
#else
		CameraCCD=new SimulateurApogeeUSB(false);
#endif

		// Initialisation du systeme de la camera et test de communication USB
		//
		// Le numero de camera est celui trouve par ordre d'Id vendeur Apogee en scannant tous
		//  les bus USB.
		//
		if( !CameraCCD->InitDriver(NumeroCamera,0,0) )
		{
			std::cout << "CamerOA: ERREUR: Impossible d'initialiser le systeme de la camera." << std::endl;
			exit(EXIT_FAILURE);
		}

		// Reinitialisation complete du systeme de la camera et permission du flushing (vidage)
		//  FPGA_BIT_CMD_FLUSH
		//
		if( !CameraCCD->ResetSystem() )
		{
			std::cout << "CamerOA: ERREUR: Impossible de reinitialiser le systeme de la camera." << std::endl;
			exit(EXIT_FAILURE);
		}

		// Recuperation des informations de la camera (version Linux speciale dans
		//  ApnCamera_Linux.cpp)
		//
		CameraCCD->sensorInfo();

		// Mode de fonctionnement de la camera
		//
		CameraCCD->write_CameraMode(Apn_CameraMode_Normal);

		// Parametrage du mode des diodes LED sur la camera
		//
		//  Apn_LedMode_DisableAll, Apn_LedMode_DisableWhileExpose, Apn_LedMode_EnableAll
		//
		CameraCCD->write_LedMode(Apn_LedMode_EnableAll);

		// Puissance lumineuse des diodes LED
		//
		CameraCCD->write_TestLedBrightness(100.0);

		// Fonction de la LED A
		//
		CameraCCD->write_LedState(0,Apn_LedState_Flushing);

		// Fonction de la LED B
		//
		CameraCCD->write_LedState(1,Apn_LedState_ImageActive);

		// Allocation memoire des buffers pixels physiques pour cette camera
		//
		// Il s'agit du nombre de pixels physiques et pas surface image du capteur pour avoir de la marge
		//
		if( (BufferPixelsPhysiquesImage=new (std::nothrow) unsigned short[CameraCCD->m_TotalRows*CameraCCD->m_TotalColumns]) == NULL )
		{
			std::cout << "CamerOA: ERREUR: Impossible d'allouer le buffer pixels physiques image pour cette camera." << std::endl;
			exit(EXIT_FAILURE);
		}

		if( (BufferPixelsPhysiquesVidage=new (std::nothrow) unsigned short[CameraCCD->m_TotalRows*CameraCCD->m_TotalColumns]) == NULL )
		{
			std::cout << "CamerOA: ERREUR: Impossible d'allouer le buffer pixels physiques vidage pour cette camera." << std::endl;
			exit(EXIT_FAILURE);
		}

		if( (BufferPixelsPhysiquesCentrage=new (std::nothrow) unsigned short[CameraCCD->m_TotalRows*CameraCCD->m_TotalColumns]) == NULL )
		{
			std::cout << "CamerOA: ERREUR: Impossible d'allouer le buffer pixels physiques centrage pour cette camera." << std::endl;
			exit(EXIT_FAILURE);
		}

		// Allocation memoire du buffer pixels d'une pose de centrage horizontal
		//
		if( (BufferPixelsPCH=new (std::nothrow) unsigned short[NB_LIGNES_POSE_CENTRAGE*CameraCCD->m_ImagingColumns/BINNING_POSE_CENTRAGE]) == NULL )
		{
			std::cout << "CamerOA: ERREUR: Impossible d'allouer le buffer pixels d'une pose de centrage horizontal pour cette camera." << std::endl;
			exit(EXIT_FAILURE);
		}

		// Allocation memoire du buffer pixels d'une pose de centrage vertical
		//
		if( (BufferPixelsPCV=new (std::nothrow) unsigned short[NB_COLONNES_POSE_CENTRAGE*CameraCCD->m_ImagingRows/BINNING_POSE_CENTRAGE]) == NULL )
		{
			std::cout << "CamerOA: ERREUR: Impossible d'allouer le buffer pixels d'une pose de centrage vertical pour cette camera." << std::endl;
			exit(EXIT_FAILURE);
		}


		// Creation d'un objet application KDE
		//
		KApplication appli;
		
		// Pointeur sur un objet de fenetre principale KDE
		//
		CamerOA *FenetrePrincipale=0;		// Pointeur sur objet fenetre principale de notre application

		// Instanciation du processus leger de controle de la camera
		//
		// ATTENTION: On le fait avant la fenetre principale car celle-ci a besoin d'un pointeur
		//  vers le thread de controle de la camera
		//
		ProcessusLegerControleCamera PLCamera;

		// Si l'application est restauree par le gestionnaire de session
		//
		if( appli.isRestored() )
		{
			// On restaure l'application a l'aide de l'objet de configuration de la session sauve lors de la fermeture de session
			//
			RESTORE(CamerOA(arguments->getOption(OptionsLC[CheminRepCamerOA]),&appli,&PLCamera));
		}
		else
		{
			// Pas de restauration de session donc on demarre l'application normalement
			//
			
			// Creation de l'objet fenetre principale de l'application
			//
			if( (FenetrePrincipale=new (std::nothrow) CamerOA(arguments->getOption(OptionsLC[CheminRepCamerOA]),&appli,&PLCamera)) == NULL )
			{
    				std::cerr << "CamerOA: ERREUR: Impossible de creer la fenetre principale KMainWindow de l'application." << std::endl;
					appli.exit(-1);
			}

			// Le processus leger de controle de la camera contient un pointeur vers la fenetre
			//   principale de l'application
			//
			// ATTENTION: Il faut le placer immediatement apres la creation de la fenetre 
			//  principale de l'application car le thread utilise ses methodes
			//
			PLCamera.FPCamerOA=FenetrePrincipale;

			// On fixe la fenetre principale pour l'objet application KDE
			//
			appli.setMainWidget(FenetrePrincipale);
		
			// On fixe quelques proprietes de la fenetre principale heritees de QWidget
			//
			FenetrePrincipale->setMinimumSize(QSize(TAILLE_X_BASE_CAMEROA,TAILLE_Y_BASE_CAMEROA));
		
			// Chargement des consignes sauvegardees
			//
			if( !FenetrePrincipale->ChargeConsignes() ) exit(EXIT_FAILURE);
		}
		
		// Lancement du processus leger serveur reseau des commandes sur le CamerOA
		//
		ProcessusLegerServeurCommandes PLServeurCommandes(FenetrePrincipale,INADDR_ANY,QString(arguments->getOption(OptionsLC[PortCanalCommandes])).toInt(),AdresseClient.s_addr,2,TIMEOUT_EMISSION,TIMEOUT_RECEPTION,5,FnHandlerSIGPIPECamerOA,arguments->getOption(OptionsLC[MdpClePriveeServeur]),MotDePasseClePriveeServeurCAMEROA,FnMotDePasseClePriveeChiffreeCAMEROA,arguments->getOption(OptionsLC[CheminFichCertCA_OA]),arguments->getOption(OptionsLC[CheminFichCertServCamerOA]),arguments->getOption(OptionsLC[CheminFichClePriveeServCamerOA]),arguments->getOption(OptionsLC[CheminFichParamDH]),"HIGH");

		FenetrePrincipale->PLServeurCommandes=&PLServeurCommandes;
		
		PLServeurCommandes.start();
		
		// Lancement du processus leger serveur reseau des donnees sur le CamerOA
		//
		ProcessusLegerServeurDonnees PLServeurDonnees(FenetrePrincipale,INADDR_ANY,QString(arguments->getOption(OptionsLC[PortCanalDonnees])).toInt(),AdresseClient.s_addr,2,TIMEOUT_EMISSION,TIMEOUT_RECEPTION);

		FenetrePrincipale->PLServeurDonnees=&PLServeurDonnees;
		
		PLServeurDonnees.start();
		
		// Les pointeurs entre les processus
		//
		PLServeurCommandes.threadCanalDonnees=&PLServeurDonnees;
		PLServeurCommandes.threadCamera=&PLCamera;
		
		// Lancement du processus leger de controle de la camera
		//
		PLCamera.start();
		
		// Tant que les threads ne sont pas tous lances et operationnels
		//
		while( SemaphoreSyncLancementThreadCamerOA.available() > 0 );
		
		// On affiche la fenetre principale
		//
		FenetrePrincipale->show();
		
		// Demarrage du timer une fois que tout est lance et affiche
		//
		FenetrePrincipale->Pulsar1s->start(1000,FALSE);
		
		
		// FenetrePrincipale a un drapeau WDestructiveClose par defaut, elle se detruira elle meme.
		//
		retour=appli.exec();


		//	On demande l'arret des processus legers
		//
		if( PLServeurCommandes.running() )
		{
			PLServeurCommandes.DemandeTerminaison();
		}

		if( PLServeurDonnees.running() )
		{
			PLServeurDonnees.DemandeTerminaison();
		}

		if( PLCamera.running() )
		{
			PLCamera.DemandeTerminaison();
		}

		// Si le processus leger de controle de la camera tourne encore
		//  on attend la terminaison propre par lui meme
		//
		while( PLCamera.running() );
		PLCamera.wait(TEMPS_ATTENTE_TERMINAISON_PROCESSUS_LEGER);

		// Fermeture de la communication USB avec la camera
		//
		CameraCCD->write_ForceShutterOpen(false);
		CameraCCD->ResetSystem();
		usleep(1000000);
		CameraCCD->CloseDriver();
		
		// On attend la terminaison du processus leger avant de retourner la valeur
		//
		PLServeurCommandes.wait(TEMPS_ATTENTE_TERMINAISON_PROCESSUS_SERVEUR);

		// On attend la terminaison du processus leger avant de retourner la valeur
		//
		PLServeurDonnees.wait(TEMPS_ATTENTE_TERMINAISON_PROCESSUS_SERVEUR);

		
		// On lave la liste des options et arguments de la ligne de commande de l'application
		//
		arguments->clear();
		
		
		// Destruction de l'objet camera
		//
		delete CameraCCD;
		
		
		// Liberation de la memoire utilisee pour la gestion de la camera
		//
		delete [] BufferPixelsPhysiquesImage;
		delete [] BufferPixelsPhysiquesVidage;
		delete [] BufferPixelsPhysiquesCentrage;
		delete [] BufferPixelsPCH;
		delete [] BufferPixelsPCV;


		// Si on a demande l'arret du systeme en quittant l'application CamerOA
		//
		if( ArretSystemeEnQuittant )
		{
			std::cout << "Lancement de la demande de l'arret du systeme dans 60 secondes." << std::endl;
			
			// On utilise la commande propre du systeme via la commande sudo
			//
			// Il faut installer l'utilitaire sudo et configurer /etc/sudoers avec la ligne :
			//
			//  dromeuf cameroa-1=NOPASSWD: /sbin/halt
			//	observateur cameroa-1=NOPASSWD: /sbin/halt
			//
			//  qui permet par exemple a l'utilisateur dromeuf depuis la machine jedi, sans mot de passe,
			//   de lancer la commande /sbin/halt
			//
			system("/bin/sync ; /bin/sleep 60s ; /usr/bin/sudo /sbin/halt");
		}

		// Resultat de l'execution de la QApplication heritee par KApplication
		//
		return retour;
	}
}
