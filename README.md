
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


# Avant de compiler et utiliser `cameroa`
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

Une [page du wiki](https://github.com/mael65/cameroa/wiki/Modifications-faites-au-projet-pour-le-rendre-compilable) permet de comprendre quelles changements ont été faits pour rendre la compilation possible. Une autre [page du wiki](https://github.com/mael65/cameroa/wiki/Ce-que-Loic-J-a-fait-pour-compiler-Cameroa) rend compte du travail précédemment effectué pour la recompilation.
