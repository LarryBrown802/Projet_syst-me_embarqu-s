🔧 Station Météorologique Arduino
📌 Description du Projet

Ce projet implémente une station météorologique complète basée sur Arduino. Le système permet de collecter, enregistrer et surveiller diverses données environnementales telles que la température, l'humidité, la pression atmosphérique et la luminosité.

Les données sont enregistrées sur une carte SD avec un horodatage précis grâce à une horloge temps réel.

🧰 Matériel Requis

Pour réaliser ce projet, nous avons besoin des composants suivants :

Une carte Arduino compatible (idéalement Arduino Uno ou Nano)
Un capteur BME280 (température, humidité, pression – I2C adresse 0x76)
Un module RTC DS3231 pour l’horodatage
Un module carte SD (interface SPI, CS sur la broche 4)
Deux boutons poussoirs :
Rouge → broche 3
Vert → broche 2
Une LED RGB chainable (broches 7 et 8)
Un capteur de luminosité analogique (A0)
⚙️ Installation et Configuration

Nous devons installer plusieurs bibliothèques nécessaires au fonctionnement du projet.

Ouvrons le gestionnaire de bibliothèques dans l’IDE Arduino :
Croquis → Inclure une bibliothèque → Gérer les bibliothèques
Installons les bibliothèques suivantes :
Wire
Adafruit Unified Sensor
Adafruit BME280
RTClib
SD
EEPROM (incluse par défaut)
ChainableLED
Créons un dossier nommé meteo et plaçons-y le fichier meteo.ino.
Sélectionnons :
La carte Arduino (menu Outils)
Le port série
Téléversons le programme.
🔌 Câblage des Composants
BME280 (I2C)
SDA → A4
SCL → A5
VCC → 3.3V/5V
GND → Masse
RTC DS3231
SDA → A4
SCL → A5
VCC → 5V
GND → Masse
Module SD
CS → 4
MOSI → 11
MISO → 12
SCK → 13
Boutons
Rouge → broche 3
Vert → broche 2
👉 Connectés à la masse (INPUT_PULLUP)
LED RGB
DI → 7
CI → 8
Capteur de luminosité
Signal → A0
🔄 Modes de Fonctionnement

Le système propose quatre modes :

🟢 Mode Standard
Enregistrement des données selon un intervalle défini
🟡 Mode Configuration
Modification des paramètres via le port série
🔵 Mode Économique
Mesures moins fréquentes pour économiser l’énergie
🟠 Mode Maintenance
Affichage temps réel sans enregistrement
🎮 Utilisation des Boutons

Nous utilisons les boutons pour naviguer entre les modes :

Appui long bouton vert → mode Économique
Appui long bouton rouge → mode Maintenance
Maintien bouton rouge au démarrage → mode Configuration
💻 Configuration via Port Série

Nous utilisons le moniteur série (9600 bauds).

Commandes principales :

HELP → liste des commandes
CONFIG → afficher configuration
LOG_INTERVAL → intervalle d’enregistrement
CLOCK → régler l’heure
DATE → régler la date
RESET → paramètres par défaut
📊 Format des Données

Chaque ligne contient :

JJ/MM/AAAA HH:MM:SS / Temp / Humidité / Pression / Altitude / Luminosité

Les fichiers sont nommés :

AAMMJJ_N.LOG
⚠️ Gestion des Erreurs

Le système détecte automatiquement les erreurs :

RTC non détecté → LED rouge/bleu
BME280 absent → LED rouge/vert
Carte SD absente → LED rouge/blanc
Données aberrantes → clignotements rouge/vert
🔧 Configuration par Défaut
Intervalle : 10 minutes
Taille fichier : 4 Ko
Timeout : 30 secondes
Tous les capteurs activés
🛠️ Dépannage

Si un problème survient, nous vérifions :

Les connexions
L’alimentation
La carte SD (format FAT32)
La pile du RTC
🚀 Améliorations Possibles

Nous pouvons améliorer ce projet en ajoutant :

Un écran LCD/OLED
WiFi ou Bluetooth
Capteurs supplémentaires
Interface web
Mode basse consommation
⚙️ Informations Techniques
Version : 2.4
I2C : 100 kHz
EEPROM : stockage persistant
Interruptions matérielles pour boutons
📄 Licence

Ce projet est fourni tel quel. Nous sommes libres de le modifier et de l’améliorer.
