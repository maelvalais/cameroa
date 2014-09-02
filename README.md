Ce dépôt contient les sources du programme de gestion des caméras APOGEE. Chaque machine (4 en tout) fait tourner ce programme. Les images capturées sont ensuite envoyées à SPV1 (qui fait tourner [`supervclimso`](https://github.com/mael65/supervclimso)) Les problèmes à résoudre sur le programme sont contenus dans l'onglet "[Issues](issues)."

Le système CLIMSO est composé de 3 autres programmes : [`supervclimso`](https://github.com/mael65/supervclimso), [`terminoa`](https://github.com/mael65/terminoa) et [`roboa`](https://github.com/mael65/roboa).

J'ai mis en ligne ces versions en retirant toute partie posant problème (certificats SSL, adresses privées internes, ports...). J'espère aussi que David Romeuf ne m'en voudra pas d'avoir copié son travail sur un espace de développement public, qui permettra peut-être de simplifier la maintenance de ce logiciel complexe.

Voici un schéma récapitulatif des quatre programmes (crédit : David Romeuf, 2007) :
![Description logicielle du système CLIMSO](http://www.climso.fr/images/projet/CLIMSO-DescriptionSchematique-ProcessusCommunications-800l.jpg)

Maël VALAIS
