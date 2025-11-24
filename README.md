Station Météorologique Arduino
Description du Projet
Ce projet implémente une station météorologique complète basée sur Arduino. Le système permet de collecter, enregistrer et surveiller diverses données environnementales comme la température, l'humidité, la pression atmosphérique et la luminosité. Les données sont enregistrées sur une carte SD avec horodatage précis grâce à une horloge temps réel.
Matériel Requis
Pour réaliser ce projet, vous aurez besoin des composants suivants.
Vous devez disposer d'une carte Arduino compatible, idéalement une Arduino Uno ou Nano. Le capteur principal est un BME280 qui mesure simultanément la température, l'humidité et la pression atmosphérique. Ce capteur communique via le protocole I2C à l'adresse 0x76.
Pour l'horodatage précis des mesures, le projet utilise un module RTC DS3231 qui maintient l'heure même lorsque l'Arduino est hors tension. Le stockage des données se fait sur un module carte SD, nécessitant un adaptateur microSD avec interface SPI connecté à la broche 4.
L'interface utilisateur comprend deux boutons poussoirs, un rouge connecté à la broche 3 et un vert connecté à la broche 2. L'état du système est indiqué par une LED RGB chainable connectée aux broches 7 pour les données et 8 pour l'horloge.
Enfin, un capteur de luminosité analogique est connecté à la broche A0 pour mesurer l'intensité lumineuse ambiante.
Installation et Configuration
Commencez par installer l'IDE Arduino depuis le site officiel arduino.cc si ce n'est pas déjà fait. Ensuite, vous devrez installer plusieurs bibliothèques nécessaires au fonctionnement du projet.
Ouvrez le gestionnaire de bibliothèques dans l'IDE Arduino via le menu Croquis puis Inclure une bibliothèque et Gérer les bibliothèques. Recherchez et installez les bibliothèques suivantes : Wire pour la communication I2C, Adafruit Unified Sensor, Adafruit BME280 Library pour le capteur environnemental, RTClib pour l'horloge temps réel, SD pour la carte SD, EEPROM qui est normalement incluse par défaut, et ChainableLED pour contrôler la LED RGB.
Une fois les bibliothèques installées, créez un nouveau dossier nommé meteo et placez-y le fichier meteo.ino. Ouvrez ce fichier dans l'IDE Arduino, sélectionnez votre carte Arduino dans le menu Outils puis Type de carte, choisissez le port série approprié dans Outils puis Port, puis téléversez le programme sur votre carte.
Câblage des Composants
Le module BME280 se connecte via I2C avec SDA sur A4 et SCL sur A5 pour une Arduino Uno. N'oubliez pas de connecter VCC au 3.3V ou 5V selon votre module, et GND à la masse.
Le module RTC DS3231 utilise également l'I2C avec les mêmes broches SDA sur A4 et SCL sur A5, plus l'alimentation VCC au 5V et GND à la masse.
Pour le module carte SD, connectez CS à la broche 4, MOSI à la broche 11, MISO à la broche 12 et SCK à la broche 13. L'alimentation se fait en 5V avec une connexion à la masse.
Les boutons sont simples à connecter. Le bouton rouge va sur la broche 3 et le bouton vert sur la broche 2. Ces boutons sont configurés en INPUT_PULLUP donc connectez simplement une extrémité à la broche et l'autre à la masse.
La LED RGB chainable nécessite deux connexions : la broche de données DI sur la broche 7 et la broche d'horloge CI sur la broche 8, plus l'alimentation en 5V et la masse.
Enfin, le capteur de luminosité analogique se branche simplement sur A0 avec son alimentation appropriée.
Modes de Fonctionnement
Le système propose quatre modes distincts, chacun identifié par une couleur de LED spécifique.
Le mode Standard est indiqué par une LED verte. Dans ce mode, le système enregistre les données selon l'intervalle configuré, par défaut toutes les 5 minutes. Les données sont sauvegardées sur la carte SD et les mesures aberrantes déclenchent une alerte visuelle.
Le mode Configuration s'active avec une LED jaune. Ce mode permet de modifier tous les paramètres du système via le port série. Un timeout de 30 secondes est actif et ramène automatiquement au mode Standard en cas d'inactivité.
Le mode Économique utilise une LED bleue. Il fonctionne comme le mode Standard mais avec un intervalle de mesure quatre fois plus long pour économiser l'énergie. Les données sont toujours enregistrées mais moins fréquemment.
Le mode Maintenance affiche une LED orange. Dans ce mode, les données sont affichées en temps réel sur le port série toutes les 2 secondes sans être enregistrées sur la carte SD. Ce mode est idéal pour le débogage et la vérification du bon fonctionnement des capteurs.
Utilisation des Boutons
Les boutons permettent de naviguer entre les différents modes de fonctionnement.
Depuis le mode Standard, un appui long de 2 secondes sur le bouton vert permet de passer en mode Économique. Un appui long sur le bouton rouge active le mode Maintenance.
En mode Économique, un appui long sur le bouton vert permet de revenir au mode Standard. Le bouton rouge permet toujours d'accéder au mode Maintenance.
Le mode Maintenance se quitte en effectuant un appui long sur le bouton rouge, ce qui ramène au mode précédent, que ce soit Standard ou Économique.
Au démarrage du système, si vous maintenez le bouton rouge enfoncé pendant les 5 premières secondes, le système démarre directement en mode Configuration.
Configuration via Port Série
Le mode Configuration permet de personnaliser tous les aspects du système. Ouvrez le moniteur série à 9600 bauds pour accéder aux commandes.
La commande HELP affiche la liste complète des commandes disponibles avec leur syntaxe. CONFIG permet de visualiser la configuration actuelle de tous les paramètres.
Vous pouvez régler l'intervalle d'enregistrement avec LOG_INTERVALL suivi de la valeur en minutes. La taille maximale des fichiers se définit avec FILE_MAX_SIZE en octets. Le timeout du mode configuration s'ajuste avec TIMEOUT en secondes.
Pour activer ou désactiver les capteurs, utilisez LUMIN=1 ou LUMIN=0 pour la luminosité, TEMP_AIR=1 ou TEMP_AIR=0 pour la température, HYGR=1 ou HYGR=0 pour l'humidité, et PRESSURE=1 ou PRESSURE=0 pour la pression.
Chaque capteur possède des seuils configurables. Pour la luminosité, LUMIN_LOW et LUMIN_HIGH définissent les bornes entre 0 et 1023. La température s'ajuste avec MIN_TEMP_AIR et MAX_TEMP_AIR entre moins 40 et 85 degrés Celsius. L'humidité utilise HYGR_MINT et HYGR_MAXT, également entre moins 40 et 85. La pression se configure avec PRESSURE_MIN et PRESSURE_MAX entre 300 et 1100 hPa.
Pour régler l'heure, utilisez CLOCK=HH:MM:SS avec le format 24 heures. La date se définit avec DATE=MM/JJ/AAAA. La commande RESET restaure tous les paramètres à leurs valeurs par défaut. VERSION affiche le numéro de version du logiciel.
Format des Données Enregistrées
Les données sont enregistrées sur la carte SD dans des fichiers nommés automatiquement selon le format AAMMJJ_N.LOG où AA représente l'année sur deux chiffres, MM le mois, JJ le jour et N un numéro séquentiel qui s'incrémente lorsque la taille maximale du fichier est atteinte.
Chaque ligne de données contient les informations suivantes séparées par des barres obliques : la date et l'heure au format JJ/MM/AAAA HH:MM:SS, la température en degrés Celsius avec deux décimales, l'humidité relative en pourcentage avec deux décimales, la pression atmosphérique en hPa avec deux décimales, l'altitude calculée en mètres avec deux décimales, et enfin la luminosité en valeur brute du capteur entre 0 et 1023.
Un nouveau fichier est créé automatiquement chaque jour à minuit ou lorsque le fichier courant atteint la taille maximale configurée.
Gestion des Erreurs
Le système intègre plusieurs mécanismes de détection et de gestion des erreurs pour assurer un fonctionnement fiable.
Si le module RTC n'est pas détecté au démarrage, la LED clignote en rouge et bleu. Le système tente automatiquement de se reconnecter toutes les 5 secondes. Une fois la connexion rétablie, l'Arduino redémarre automatiquement.
En cas de problème avec le capteur BME280, la LED clignote en rouge et vert. Comme pour le RTC, des tentatives de reconnexion sont effectuées toutes les 5 secondes avec un redémarrage automatique une fois le capteur détecté.
Si la carte SD n'est pas détectée, la LED clignote en rouge et blanc. Le système effectue 10 tentatives de reconnexion. Si la carte reste inaccessible, le système continue de fonctionner en mode dégradé sans enregistrement sur SD.
Lorsque des données incohérentes sont détectées, c'est-à-dire des valeurs hors des plages configurées, une alerte visuelle est déclenchée avec six clignotements rapides rouge et vert. Les données aberrantes ne sont pas enregistrées.
Configuration par Défaut
Le système démarre avec une configuration optimisée pour un usage général.
L'intervalle d'enregistrement est fixé à 10 minutes en mode Standard. La taille maximale des fichiers est limitée à 4096 octets soit environ 4 Ko. Le timeout du mode configuration est de 30 secondes.
Tous les capteurs sont activés par défaut : luminosité, température, humidité et pression. Les seuils de luminosité sont configurés entre 255 et 768 sur une échelle de 0 à 1023. La plage de température valide s'étend de moins 10 à plus 60 degrés Celsius. L'humidité est considérée normale entre 0 et 50 pourcent. La pression atmosphérique acceptable va de 850 à 1080 hPa.
Ces valeurs peuvent être modifiées à tout moment via le mode Configuration et sont sauvegardées de manière persistante dans l'EEPROM de l'Arduino.
Dépannage
Si le système ne démarre pas, vérifiez d'abord toutes les connexions électriques et assurez-vous que l'alimentation est suffisante. Un Arduino Uno nécessite au moins 7V sur l'entrée Vin ou une connexion USB stable.
Si les données ne s'enregistrent pas, vérifiez que la carte SD est correctement formatée en FAT16 ou FAT32 et qu'elle dispose d'espace libre. Assurez-vous également que le module SD est correctement câblé.
En cas de valeurs aberrantes répétées, vérifiez les connexions du capteur BME280 et ajustez éventuellement les seuils de validation dans la configuration. Des câbles I2C trop longs peuvent causer des problèmes de communication.
Si l'heure n'est pas maintenue après une coupure de courant, vérifiez la pile bouton du module RTC DS3231. Cette pile doit être remplacée périodiquement.
Si les commandes série ne fonctionnent pas, assurez-vous que le moniteur série est configuré à 9600 bauds et que l'option de fin de ligne est réglée sur Nouvelle ligne.
Améliorations Possibles
Ce projet peut être étendu de plusieurs manières pour répondre à des besoins spécifiques.
Vous pourriez ajouter un écran LCD ou OLED pour afficher les données en temps réel sans nécessiter une connexion série. Un module WiFi ou Bluetooth permettrait de transmettre les données à distance vers un smartphone ou un serveur web.
Des capteurs supplémentaires peuvent être intégrés facilement : anémomètre pour la vitesse du vent, pluviomètre pour les précipitations, capteur UV pour le rayonnement ultraviolet, ou capteur de qualité de l'air.
L'implémentation d'un mode veille profond réduirait considérablement la consommation électrique pour un fonctionnement sur batterie prolongé. Un système d'alertes pourrait envoyer des notifications en cas de conditions météorologiques extrêmes.
Enfin, l'ajout d'une interface web permettrait de consulter les données historiques et de configurer le système à distance via un navigateur.
Informations Techniques
Le programme utilise environ 344 octets de mémoire SRAM dynamique et s'identifie comme la version 2.4. La communication I2C fonctionne à la vitesse standard de 100 kHz. Les données de configuration sont stockées de manière permanente dans l'EEPROM à l'adresse 0.
Le système utilise des interruptions matérielles pour détecter les appuis sur les boutons, garantissant une réactivité optimale. Les timestamps sont gérés avec une précision de la seconde grâce au module RTC.
La protection contre les rebonds des boutons est assurée par la logique des appuis longs. Un délai de 100 millisecondes dans la boucle principale évite une surcharge du processeur tout en maintenant une bonne réactivité.
Licence et Support
Ce projet est fourni tel quel sans garantie. Vous êtes libre de le modifier et de l'adapter à vos besoins. Pour toute question ou amélioration, n'hésitez pas à documenter vos modifications et à partager vos expériences avec la communauté Arduino.
