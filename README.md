
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
