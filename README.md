
Ce dépôt contient les sources du programme de gestion des caméras APOGEE. Chaque machine (4 en tout) fait tourner ce programme. Les images capturées sont ensuite envoyées à SPV1 (qui fait tourner [`supervclimso`](https://github.com/mael65/supervclimso)). Les problèmes à résoudre sur le programme sont contenus dans l'onglet "[Issues](https://github.com/mael65/cameroa/issues)."

Le système CLIMSO est composé de 3 autres programmes : [`supervclimso`](https://github.com/mael65/supervclimso), [`terminoa`](https://github.com/mael65/terminoa) et [`roboa`](https://github.com/mael65/roboa).

J'ai mis en ligne ces versions en retirant toute partie posant problème (certificats SSL, adresses privées internes, ports...). J'espère aussi que David Romeuf ne m'en voudra pas d'avoir copié son travail sur un espace de développement public, qui permettra peut-être de simplifier la maintenance de ce logiciel complexe.

Voici un schéma récapitulatif des quatre programmes (crédit : David Romeuf, 2007) :
![Description logicielle du système CLIMSO](http://www.climso.fr/images/projet/CLIMSO-DescriptionSchematique-ProcessusCommunications-800l.jpg)

Maël VALAIS


# Lancer plusieurs instances de `cameroa` sur une même machine
J'ai cherché un moyen de lancer plusieurs instances de `cameroa` sur une même machine. Pour faire ça, il faut que les deux sockets utilisés (pour les commandes et pour les données) soient reliés à des ports différents pour chaque instance.

Pour `supervclimso`, il est nécessaire de changer dans `main.cpp` les ports liés à chaque socket commande/données pour les quatre `cameroa`. C'est à partir de la ligne 395 : 

		adresse=0xC0A80614;		// 192.168.6.20
		pt_BuffStockMdpClePriveeClient=MotDePasseClePriveeClientSUPERVCLIMSO[0];
		pt_FnMotDePasseClePriveeChiffree=FnMotDePasseClePriveeChiffreeSUPERVCLIMSO_0;
		portc=33443;
		portd=33444;

Pour `cameroa`, il suffit de modifier comment on appelle le programme : les ports pour les commandes et pour les données peuvent être données dans les paramètres (disponibles dans OptionsL de main.cpp).

		./cameroa -arretsysteme o -chemcameroa /CamerOA-3 -chemficaoa /CamerOA-3/ssl/CertificatCA_OA.pem -chemficertserveur /CamerOA-3/ssl/CertificatServeurCamerOA3.pem -chemficleprivserveur /CamerOA-3/ssl/ClePriveeServeurCamerOA3.pem -chemfiparamdh /CamerOA-3/ssl/Parametres-Diffie-Hellman-CamerOA3.pem -mdpcleprivserveur imagerie3 -adresseclientautorise 192.168.6.1 -portcanalcommandes 33443 -portcanaldonnees 33444
	
Il suffira alors de modifier les deux ports.

# Modifications obligatoires pour compiler et utiliser `cameroa`
Ces opérations sont nécessaires à la compilation de cameroa. Pour le moment, on a exclu l'utilisation de 64 bits à cause de problèmes de communication. La compilation/exécution fonctionne donc très bien sur un Fedora 20 32 bits.
 
## Installer les librairies nécessaires

		yum install gcc gcc-c++ qt3 qt3-devel kdelibs3 kdelibs3-devel libusb libusb-devel autoconf automake libtool
		
Et aussi éventuellement `git` pour récupérer les sources depuis le dépôt (il est aussi possible de télécharger un .zip directement) :

		yum install git

## Rendre possible l'usage de la caméra pour les utilisateurs non root

Quand on lance la commande suivante en étant un utilisateur non root :
		./cameroa -arretsysteme o -chemcameroa /CamerOA-3 -chemficaoa /CamerOA-3/ssl/CertificatCA_OA.pem -chemficertserveur /CamerOA-3/ssl/CertificatServeurCamerOA3.pem -chemficleprivserveur /CamerOA-3/ssl/ClePriveeServeurCamerOA3.pem -chemfiparamdh /CamerOA-3/ssl/Parametres-Diffie-Hellman-CamerOA3.pem -mdpcleprivserveur imagerie3 -adresseclientautorise 192.168.6.1 -portcanalcommandes 33443 -portcanaldonnees 33444

 On obtient l'erreur :

		APOGEE.DLL - CApnCamera::CApnCamera()APOGEE.DLL - CApnCamera::InitDriver() -> BEGINCamerOA: ERREUR: Impossible d'initialiser le systeme de la camera.

Vérifions si la caméra est bien connectée :
	
		lsusb
		Bus 002 Device 013: ID 125c:0010 Apogee Inc. Alta series CCD

Sous Linux, les périphériques USB sont gérés au niveau des privilèges par les règles `udev`. Prennez donc l'id vendeur et l'id produit (125c et 0010 ici) pour ensuite les utiliser pour créer la règle udev. On se met en super-user :

		su 
		vim /etc/udev/rules.d/51-mes-regles-climso.rules
		
On peut aussi mettre ce fichier dans `/usr/lib/udev/rules.d`. Maintenant, on donne la règle suivante (à écrire dans le fichier créé) :

		ACTION=="add", SUBSYSTEM=="usb", ATTRS{idVendor}==="125c", ATTRS{idProduct}=="0010", MODE="0666"

C'est le dernier argument (`0666`) qui permet à tout utilisateur de lire/écrire sur le périphérique. Tappez `:wq + Entrée` pour sauver et quitter vim. On redémarre et on vérifie que ça a bien marché grâce aux commandes :

		lsusb

Notez le numéro de bus et de device de la caméra. Puis lancez la commande :

		ls -l /dev/bus/usb/(numDeBus)/(numDuDevice)

Et normalement, les droits seront en `rwxrw-rw`. Si on veut afficher les attributs possibles pour ce périphérique (pour la règle udev) :

		udevadm info --query=all --name=/dev/bus/usb/002/006 --attribute-walk

# Modifications apportées vis à vis du source d'origine

Avant de faire tout ce qui suit, Loic Jahan avait déjà commencé à essayer de compiler cameroa : [ce qu'il a écrit](https://github.com/mael65/projet-climso/wiki/Ce-que-Loic-J-a-fait-pour-compiler-Cameroa).

## Quel source de départ ?
Différentes versions de cameroa ont été trouvées. J'ai pris celle contenue dans `climso20100203-apres-mission-maintenance.tar.gz2` qui semble la plus récente. À l'intérieur, j'ai pris `Cameroa v1.9`.

## Erreurs liées à pointcomm.h
Il y a eu des changements dans ssl.h entre la version de 2006 et celle d'aujourd'hui : pas mal de membres sont passés en "const". C'est le cas sur :
	
	const SSL_METHOD*
	const SSL_CIPHER*

Il a juste fallu corriger tout cela dans le code existant ; pas besoin d'includes différents (ou de librairies différentes) de celles de 2014.

## Erreurs liées à ApnCamData_*.c/.h
Les fichiers `ApnCamData_*` correspondent chacun au driver d'une caméra spécifique ALTA. Tout d'abord, j'ai dû modifier ces fichiers car des erreurs avec certaines macros spécifiques à Visual Studio apparaissaient :

	#if _MVS_VER > 1000 

remplacé par

	#if defined(_MVS_VER) && _MVS_VER > 1000


## Erreurs liées à Apn.h et stdafx.h
Il s'agissait d'erreurs liées à la macro `LINUX` non déclarée sur ma machine. La machine de David Romeuf contenait une version de gcc gérant la macro `LINUX`. Pour connaitre les macros spécifiques à sa propre machine, j'ai tappé :
	
	touch essai.h; cpp -dM essai.h; rm essai.h

on obtient la liste des macros prédéfinies pour ce système. La macro `LINUX` n'existant pas, j'ai recherché une alternative. `linux` et `__linux__` existaient, j'ai donc choisi la seconde option en remplaçant tous les 

	#if LINUX 

par 

	#if defined(LINUX) || defined(__linux__)

dans les deux fichiers.

Source: https://gcc.gnu.org/onlinedocs/cpp/System-specific-Predefined-Macros.html#System-specific-Predefined-Macros


## Problème avec libcrypto.so.10
Lors du linkage, à la fin de la compilation (lors qu'on a appelé `make`), on tombe sur l'erreur :

	/usr/bin/ld: pointscomm.o: undefined reference to symbol 'BIO_ctrl@@libcrypto.so.10'
	/usr/bin/ld: note: 'BIO_ctrl@@libcrypto.so.10' is defined in DSO /lib/libcrypto.so.10 so try adding it to the linker command line
	/lib/libcrypto.so.10: could not read symbols: In	valid operation

Il s'agit apparemment d'un problème d'ordre d'appel des librairies (http://stackoverflow.com/questions/17812344/undefined-reference-to-symbol-bio-ctrllibcrypto-so-10). La seule résolution donnée par Loic Jahan est d'ajouter le flag de linkage `LDFLAGS=-lcrypto`. On appelera ainsi, pour la configuration (avant de faire `make`) :

	./configure LDFLAGS=-lcrypto

Les paramètres d'édition de liens, de préproc et de compilation sont situés dans src/Makefile.am


## Utiliser la librairie Qt-3.3 au lieu de Qt4

Pour compiler, il faut pour le moment la version 3 de la librairie Qt. Il est possible de l'installer par `yum install qt3` par exemple sous fedora/red-hat.

Pour vérifier quelle version de Qt est utilisée : on regarde ce qui est dans le path.
	
	echo $PATH
	/usr/lib/qt4/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/home/admin/.local/bin:/home/admin/bin

Ici, qt4 est utilisé. Sachant qu'on veut Qt3, j'ai dû chercher la raison pour laquelle qt4 est ajouté au $PATH. En réalité, on aurait pu faire un 

	export PATH=/usr/lib/qt3/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/home/admin/.local/bin:/home/admin/bin

pour avoir qt3 à la place de qt3. Mais j'ai trouvé un moyen plus profond : par défaut, au démarrage de la machine, tous les .sh du dossier /etc/profile.d sont exécutés. Ces scripts ajoutent au PATH un morceau, ou alors ajoutent des variables globales...
Dans notre cas, le fichier qt.sh (pour qt-3.3) est responsable de l'ajout dans la variable $PATH. J'ai modifié le nom en qt.sh.disabled et crée le fichier qt4.sh qui contient l'ajout du dossier de qt4 au PATH.
Du coup, les commandes qmake, moc, rcc, uic... sont lancées depuis le répertoire ciblé dans le $PATH.

Pour switcher entre qt-3.3 et qt4, il suffit donc de modifier le nom de qt4.sh en qt4.sh.qqchose et qt.sh.disabled en qt.sh.

	[root@localhost profile.d]# cd /etc/profile.d; mv qt4.sh qt4.sh.disabled; mv qt.sh.disabled qt.sh
	[root@localhost src]# cd /etc/profile.d; mv qt.sh qt.sh.disabled; mv qt4.sh.disabled qt4.sh

Voir l'exemple pour `/etc/profile.d/qt3.sh` : https://github.com/mael65/projet-climso/wiki/Exemple-de--etc-profile.d-qt3.sh

## Résoudre le problème avec les symboles vtables manquants
Tous les fichiers contenant la macro `Q_OBJECT` doivent être parsés par `moc` (un utilitaire lié à Qt) et les `.moc.cpp` ainsi obtenus doivent être inclus 
lors de la compilation. 

C'est ce que j'ai fait dans le `src/Makefile.am` :

	cameroa_SOURCES=$(cameroa_SOURCES) 		WidgetPoseImage.moc.cpp WidgetPoseCentrage.moc.cpp

# Modifications ultérieures/non obligatoires
Ces modifications ne sont pas obligatoires pour la recompilation.

## Faire marcher le simulateur
David Romeuf a crée une classe `SimulateurApogee.cpp` permettant de tester les cameroa sans caméra. Il est possible de compiler un exécutable contenant ce simulateur.

Dans `main.cpp`, j'ai ajouté la macro `_SIMULATEUR_APOGEE` à la ligne 155 :

	#ifndef _SIMULATEUR_APOGEE
		CameraCCD=new CApnCamera();
	#else
		CameraCCD=new SimulateurApogeeUSB(false);
	#endif

J'ai aussi ajouté un nouvel executable `cameroa_simulateur` dans `src/Makefile.am`, en ajoutant le `LDFLAG -D_SIMULATEUR_APOGEE` pour activer le simulateur.

## Suppression des drivers inutiles
La plupart des fichiers `ApnCamData_*` sont inutiles car correspondent à des drivers inutilisés. Il suffit de supprimer ceux inutilisés et garder celui correspondant aux quatre caméras présentes.

J'ai découvert que le driver utilisé était le `KAI4020MLB.cpp` J'ai donc supprimé tous les `ApnCamData_*.cpp` et `.h` sauf celui-ci. Ensuite, il faut modifier les `#include` dans `src/ApnCamera.h` et le swich-case `switch ( m_pvtCameraID )` à la ligne 2615 dans `src/Apn/Camera.cpp` : il faut supprimer tous les case sauf celui correspondant à `KAI4020MLB`.

Il faut aussi supprimer tous ces fichiers dans `src/Makefile.am` (dans les variables `noinst_HEADERS` et `cameroa_SOURCES`)

## Ajouts utiles dans src/Makefile.am
Au lieu de faire la commande 

		./configure LDFLAGS=-lcrypto

à chaque configuration, j'ai modifié le fichier Makefile.am. Ainsi, j'ai ajouté `-lcrypto` dans la variable `cameroa_LDADD` :

	cameroa_LDADD = -lssl -lusb $(LIB_KDEUI) -lcrypto
