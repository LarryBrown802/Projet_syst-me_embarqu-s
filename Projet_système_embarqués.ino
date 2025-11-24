#include <Wire.h>              // I2C
#include <Adafruit_Sensor.h>   // Capteurs génériques
#include <Adafruit_BME280.h>  
#include <RTClib.h>            
#include <EEPROM.h>          
#include <ChainableLED.h>      
#include <SD.h>                
 
#define BOUTON_ROUGE 3    
#define BOUTON_VERT 2    
#define LED_DATA 7         // Data LED chaîne
#define LED_CLOCK 8        // Clock LED chaîne
#define NBLED 1          
#define CHIP_SD 4          
#define LIGHT_PIN A0       // Capteur luminosité
#define BME_280 0x76       // Adresse I2C BME280
 
struct Config {
  int LOG_INTERVALL;
  int FILE_MAX_SIZE;
  int TIMEOUT;
  int LUMIN;
  int LUMIN_LOW;
  int LUMIN_HIGH;
  int TEMP_AIR;
  int MIN_TEMP_AIR;
  int MAX_TEMP_AIR;
  int HYGR;
  int HYGR_MINT;
  int HYGR_MAXT;
  int PRESSURE;
  int PRESSURE_MIN;
  int PRESSURE_MAX;
};
 
const Config CONFIG_DEFAUT PROGMEM = { // Config par défaut
  10, 4096, 30, 1, 255, 768, 1, -10, 60, 1, 0, 50, 1, 850, 1080
};
 
Config config; //structure config
RTC_DS3231 rtc;
ChainableLED led(LED_DATA, LED_CLOCK, NBLED);
Adafruit_BME280 bme;
 
short mode = 0, modePrecedent = 0;
volatile bool appuiRouge = false, appuiVert = false;
unsigned long debutAppuiRouge = 0, debutAppuiVert = 0, derniereAction = 0;
unsigned long dernier_enregistrement = 0;
const unsigned long TEMPS_APPUI_LONG = 2000;
const unsigned long TIMEOUT_CONFIG = 280000;
int NumFichier = 0, Jprecedent = -1;
bool sdActive = true;
 
void chargerConfig() {
  EEPROM.get(0, config); // charge la config dansl’EEPROM à l’adresse 0
}
 
void sauvegarderConfig() {
  EEPROM.put(0, config); // Sauvegarde la config dansl’EEPROM à l’adresse 0
}
 
void RESET() { //reset la config
  memcpy_P(&config, &CONFIG_DEFAUT, sizeof(Config));
  sauvegarderConfig();
  Serial.println(F("OK"));
}
 
void afficherConfig() {
  Serial.println(F("CONFIG"));
  Serial.print(F("LOG_INTERVALL:")); Serial.println(config.LOG_INTERVALL);
  Serial.print(F("FILE_MAX_SIZE:")); Serial.println(config.FILE_MAX_SIZE);
  Serial.print(F("TIMEOUT:")); Serial.println(config.TIMEOUT);
  Serial.print(F("LUMIN:")); Serial.println(config.LUMIN);
  Serial.print(F("TEMP_AIR:")); Serial.println(config.TEMP_AIR);
  Serial.print(F("HYGR:")); Serial.println(config.HYGR);
  Serial.print(F("PRESSURE:")); Serial.println(config.PRESSURE);
}
 
void IntRouge() {
  if (digitalRead(BOUTON_ROUGE) == LOW) {
    debutAppuiRouge = millis();
    appuiRouge = true;
  } else {
    appuiRouge = false;
    derniereAction = millis();
  }
}
 
void IntVert() {
  if (digitalRead(BOUTON_VERT) == LOW) {
    debutAppuiVert = millis();
    appuiVert = true;
  } else {
    appuiVert = false;
    derniereAction = millis();
  }
}
 
void changerMode(short nouveauMode) {
  mode = nouveauMode;
  derniereAction = millis();
 
  if (mode == 3 && sdActive) {
    SD.end();
    sdActive = false;
  } else if (mode != 3 && !sdActive) {
    SD.begin(CHIP_SD);
    sdActive = true;
  }
 
  switch (mode) {
    case 0: led.setColorRGB(0, 0, 255, 0); Serial.println(F("STD")); break;
    case 1: led.setColorRGB(0, 255, 255, 0); Serial.println(F("CFG")); break;
    case 2: led.setColorRGB(0, 0, 0, 255); Serial.println(F("ECO")); break;
    case 3: led.setColorRGB(0, 255, 128, 0); Serial.println(F("MNT")); break;
  }
}
 
bool checkAppuiLong(bool appui, unsigned long debut) { //vérifie la durée de l appui
  return appui && (millis() - debut >= TEMPS_APPUI_LONG);
}
 
void erreurRTC() {
  Serial.println(F("ER RTC - Verifiez connexions"));
  while (true) {
    // Tentative de reconnexion toutes les 5 secondes
    led.setColorRGB(0, 255, 0, 0);
    delay(500);
    led.setColorRGB(0, 0, 0, 255);
    delay(500);
   
    static unsigned long dernierTest = 0;
    if (millis() - dernierTest > 5000) {
      dernierTest = millis();
      if (rtc.begin()) {
        Serial.println(F("RTC OK - Redemarre"));
        led.setColorRGB(0, 0, 255, 0);
        delay(1000);
        asm volatile ("jmp 0"); // // Redémarrage du microcontrôleur
      }
    }
  }
}
 
void erreurCapteur() {
  Serial.println(F("ER CAPT - Verifiez BME280"));
  while (true) {
    led.setColorRGB(0, 255, 0, 0);
    delay(500);
    led.setColorRGB(0, 0, 255, 0);
    delay(500);
   
    static unsigned long dernierTest = 0;
    if (millis() - dernierTest > 5000) {
      dernierTest = millis();
      if (bme.begin(BME_280)) {
        Serial.println(F("BME280 OK - Redemarre"));
        led.setColorRGB(0, 0, 255, 0);
        delay(1000);
        asm volatile ("jmp 0"); // // Redémarrage du microcontrôleur
      }
    }
  }
}
 
void erreurSDAcces() {
  Serial.println(F("ER SD - Inserez carte SD"));
  int tentatives = 0;
  while (tentatives < 10) {
    led.setColorRGB(0, 255, 0, 0);
    delay(500);
    led.setColorRGB(0, 255, 255, 255);
    delay(1000);
   
    // Tentative de reconnexion
    if (SD.begin(CHIP_SD)) {
      Serial.println(F("SD OK - Reprise"));
      sdActive = true;
      led.setColorRGB(0, 0, 255, 0);
      delay(1000);
      return; // Sortie de la fonction, continue normalement
    }
    tentatives++;
  }
  Serial.println(F("SD ECHEC - Mode degrade"));
  sdActive = false;
  // Continue sans carte SD
}
 
void alerteDonneesIncoherentes() {
  Serial.println(F("PB DATA"));
  for (int i = 0; i < 6; i++) {
    led.setColorRGB(0, 255, 0, 0);
    delay(500);
    led.setColorRGB(0, 0, 255, 0);
    delay(1000);
  }
  changerMode(mode);
}
 
void CLOCK(int h, int m, int s) {
  DateTime now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), h, m, s));
  Serial.println(F("OK"));
}
 
void DATE(int mois, int jour, int annee) {
  DateTime now = rtc.now();
  rtc.adjust(DateTime(annee, mois, jour, now.hour(), now.minute(), now.second()));
  Serial.println(F("OK"));
}
 
void VERSION() {
  Serial.println(F("2.4,344"));
}
 
void HELP() {
  Serial.println(F("=== COMMANDES DISPONIBLES ==="));
  Serial.println(F("LOG_INTERVALL=<val>    - Intervalle log (min)"));
  Serial.println(F("FILE_MAX_SIZE=<val>    - Taille max fichier"));
  Serial.println(F("TIMEOUT=<val>          - Timeout config"));
  Serial.println(F("LUMIN=<0/1>            - Activer luminosite"));
  Serial.println(F("LUMIN_LOW=<0-1023>     - Seuil bas lumi"));
  Serial.println(F("LUMIN_HIGH=<0-1023>    - Seuil haut lumi"));
  Serial.println(F("TEMP_AIR=<0/1>         - Activer temperature"));
  Serial.println(F("MIN_TEMP_AIR=<-40-85>  - Temp min"));
  Serial.println(F("MAX_TEMP_AIR=<-40-85>  - Temp max"));
  Serial.println(F("HYGR=<0/1>             - Activer hygro"));
  Serial.println(F("HYGR_MINT=<-40-85>     - Hygro temp min"));
  Serial.println(F("HYGR_MAXT=<-40-85>     - Hygro temp max"));
  Serial.println(F("PRESSURE=<0/1>         - Activer pression"));
  Serial.println(F("PRESSURE_MIN=<300-1100>- Pression min"));
  Serial.println(F("PRESSURE_MAX=<300-1100>- Pression max"));
  Serial.println(F("CLOCK=HH:MM:SS         - Regler heure"));
  Serial.println(F("DATE=MM/JJ/AAAA        - Regler date"));
  Serial.println(F("RESET                  - Reset config"));
  Serial.println(F("VERSION                - Version logiciel"));
  Serial.println(F("CONFIG                 - Afficher config"));
  Serial.println(F("HELP                   - Aide commandes"));
  Serial.println(F("=============================="));
}
 
// Fonction pour comparer les préfixes (insensible à la casse)
bool comparePrefix(const char* str, const char* prefix) {
}
 
// Fonction pour extraire la valeur après '='
int extraireValeur(const char* cmd) {
  const char* p = strchr(cmd, '=');
  if (p == NULL) return 0;
  return atoi(p + 1);
}
 
// Fonction pour nettoyer la chaîne (trim + majuscules)
void nettoyerCmd(char* cmd) {
  // Trim début
  char* start = cmd;
  while (*start == ' ' || *start == '\t' || *start == '\r' || *start == '\n') start++;
 
  // Trim fin
  char* end = start + strlen(start) - 1;
  while (end > start && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) end--;
  *(end + 1) = '\0';
 
  // Copier au début si nécessaire
  if (start != cmd) {
    memmove(cmd, start, strlen(start) + 1);
  }
 
  // Convertir en majuscules
  for (char* p = cmd; *p; p++) {
    if (*p >= 'a' && *p <= 'z') *p -= 32;
  }
}
 
void traiterCommande(char* cmd) {
  nettoyerCmd(cmd);
  int val;
  bool ok = true;
 
  if (comparePrefix(cmd, "LOG_INTERVALL=")) {
    val = extraireValeur(cmd);
    if (val > 0) config.LOG_INTERVALL = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "FILE_MAX_SIZE=")) {
    val = extraireValeur(cmd);
    if (val > 0) config.FILE_MAX_SIZE = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "TIMEOUT=")) {
    val = extraireValeur(cmd);
    if (val > 0) config.TIMEOUT = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "LUMIN=")) {
    config.LUMIN = extraireValeur(cmd);
  }
  else if (comparePrefix(cmd, "LUMIN_LOW=")) {
    val = extraireValeur(cmd);
    if (val >= 0 && val <= 1023) config.LUMIN_LOW = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "LUMIN_HIGH=")) {
    val = extraireValeur(cmd);
    if (val >= 0 && val <= 1023) config.LUMIN_HIGH = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "TEMP_AIR=")) {
    config.TEMP_AIR = extraireValeur(cmd);
  }
  else if (comparePrefix(cmd, "MIN_TEMP_AIR=")) {
    val = extraireValeur(cmd);
    if (val >= -40 && val <= 85) config.MIN_TEMP_AIR = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "MAX_TEMP_AIR=")) {
    val = extraireValeur(cmd);
    if (val >= -40 && val <= 85) config.MAX_TEMP_AIR = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "HYGR=")) {
    config.HYGR = extraireValeur(cmd);
  }
  else if (comparePrefix(cmd, "HYGR_MINT=")) {
    val = extraireValeur(cmd);
    if (val >= -40 && val <= 85) config.HYGR_MINT = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "HYGR_MAXT=")) {
    val = extraireValeur(cmd);
    if (val >= -40 && val <= 85) config.HYGR_MAXT = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "PRESSURE=")) {
    config.PRESSURE = extraireValeur(cmd);
  }
  else if (comparePrefix(cmd, "PRESSURE_MIN=")) {
    val = extraireValeur(cmd);
    if (val >= 300 && val <= 1100) config.PRESSURE_MIN = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "PRESSURE_MAX=")) {
    val = extraireValeur(cmd);
    if (val >= 300 && val <= 1100) config.PRESSURE_MAX = val;
    else ok = false;
  }
  else if (comparePrefix(cmd, "CLOCK=")) {
    const char* p1 = strchr(cmd, '=');
    const char* p2 = strchr(p1, ':');
    const char* p3 = strchr(p2 + 1, ':');
    if (p1 && p2 && p3) {
      int h = atoi(p1 + 1);
      int m = atoi(p2 + 1);
      int s = atoi(p3 + 1);
      CLOCK(h, m, s);
    } else ok = false;
    return;
  }
  else if (comparePrefix(cmd, "DATE=")) {
    const char* p1 = strchr(cmd, '=');
    const char* p2 = strchr(p1, '/');
    const char* p3 = strchr(p2 + 1, '/');
    if (p1 && p2 && p3) {
      int mois = atoi(p1 + 1);
      int jour = atoi(p2 + 1);
      int annee = atoi(p3 + 1);
      DATE(mois, jour, annee);
    } else ok = false;
    return;
  }
  else if (strcmp(cmd, "RESET") == 0) { RESET(); return; }
  else if (strcmp(cmd, "VERSION") == 0) { VERSION(); return; }
  else if (strcmp(cmd, "CONFIG") == 0) { afficherConfig(); return; }
  else if (strcmp(cmd, "HELP") == 0) { HELP(); return; }
  else { Serial.println(F("ERR - Tapez HELP pour aide")); return; }
 
  if (ok) {
    sauvegarderConfig();
    Serial.println(F("OK"));
  } else {
    Serial.println(F("ERR"));
  }
}
 
void lireDateHeure(char* b) {
  DateTime now = rtc.now();
  sprintf(b, "%02d/%02d/%04d %02d:%02d:%02d",
          now.day(), now.month(), now.year(),
          now.hour(), now.minute(), now.second());
}
 
bool validerDonnees(float temp, float humi, float press, int lumi) {
  bool valide = true;
  if (temp < config.MIN_TEMP_AIR || temp > config.MAX_TEMP_AIR) valide = false;
  if (press < config.PRESSURE_MIN || press > config.PRESSURE_MAX) valide = false;
  if (lumi < 0 || lumi > 1023) valide = false;
  if (!valide) alerteDonneesIncoherentes();
  return valide;
}
 
String genererNomFichier(int nf) {
  DateTime now = rtc.now();
  char nom[15];
  sprintf(nom, "%02d%02d%02d_%d.LOG",
          now.year() % 100, now.month(), now.day(), nf);
  return String(nom);
}
 
void sauvegarder(const char* dh, float t, float h, float p, float a, int l) {
  // Vérifier si la carte SD est active
  if (!sdActive) {
    Serial.println(F("SD inactive - Tentative reconnexion"));
    if (SD.begin(CHIP_SD)) {
      sdActive = true;
      Serial.println(F("SD reconnectee"));
    } else {
      Serial.println(F("SD toujours inactive"));
      return;
    }
  }
 
  DateTime now = rtc.now();
  if (now.day() != Jprecedent) {
    NumFichier = 0;
    Jprecedent = now.day();
  }
 
  String nom = genererNomFichier(NumFichier);
  File f = SD.open(nom.c_str(), FILE_WRITE);
  if (!f) {
    Serial.println(F("Erreur ouverture fichier"));
    erreurSDAcces();
    return;
  }
 
  f.print(dh); f.print('/');
  f.print(t, 2); f.print('/');
  f.print(h, 2); f.print('/');
  f.print(p, 2); f.print('/');
  f.print(a, 2); f.print('/');
  f.println(l);
 
  unsigned long taille = f.size();
  f.close();
 
  if (taille >= config.FILE_MAX_SIZE) NumFichier++;
}
 
void envoyerDonnees(const char* dh, float t, float h, float p, float a, int l) {
  Serial.print(dh); Serial.print('/');
  Serial.print(t, 2); Serial.print('/');
  Serial.print(h, 2); Serial.print('/');
  Serial.print(p, 2); Serial.print('/');
  Serial.print(a, 2); Serial.print('/');
  Serial.println(l);
}
 
void modeStandard() {
  if (checkAppuiLong(appuiRouge, debutAppuiRouge)) {
    modePrecedent = mode;
    changerMode(3);
    appuiRouge = false;
    return;
  }
 
  if (checkAppuiLong(appuiVert, debutAppuiVert)) {
    changerMode(2);
    appuiVert = false;
    return;
  }
 
  unsigned long intervalle = (unsigned long)config.LOG_INTERVALL * 30000;
 
  if (millis() - dernier_enregistrement >= intervalle) {
    dernier_enregistrement = millis();
    char dh[40];
    lireDateHeure(dh);
 
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0F;
    float a = bme.readAltitude(1013.25);
    int l = analogRead(LIGHT_PIN);
 
    if (config.TEMP_AIR && config.LUMIN) {
      validerDonnees(t, h, p, l);
      sauvegarder(dh, t, h, p, a, l);
    }
  }
}
 
void modeConfiguration() {
  Serial.println(F("CMD:LOG_INTERVALL=,FILE_MAX_SIZE=,RESET,VERSION,CONFIG"));
  unsigned long debut = millis();
  char buffer[64];
 
  while (mode == 1) {
    if (millis() - debut >= TIMEOUT_CONFIG) {
      changerMode(0);
      break;
    }
 
    if (Serial.available() > 0) {
      int len = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
      buffer[len] = '\0';
      traiterCommande(buffer);
      debut = millis();
    }
    delay(100);
  }
}
 
void modeEconomique() {
  if (checkAppuiLong(appuiVert, debutAppuiVert)) {
    changerMode(0);
    appuiVert = false;
    return;
  }
 
  if (checkAppuiLong(appuiRouge, debutAppuiRouge)) {
    modePrecedent = mode;
    changerMode(3);
    appuiRouge = false;
    return;
  }
 
  unsigned long intervalle = (unsigned long)config.LOG_INTERVALL * 120000;
 
  if (millis() - dernier_enregistrement >= intervalle) {
    dernier_enregistrement = millis();
    char dh[40];
    lireDateHeure(dh);
 
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0F;
    float a = bme.readAltitude(1013.25);
    int l = analogRead(LIGHT_PIN);
 
    sauvegarder(dh, t, h, p, a, l);
  }
}
 
void modeMaintenance() {
  Serial.println(F("MAINTENANCE"));
 
  while (mode == 3) {
    if (checkAppuiLong(appuiRouge, debutAppuiRouge)) {
      changerMode(modePrecedent);
      appuiRouge = false;
      break;
    }
 
    char dh[40];
    lireDateHeure(dh);
 
    float t = bme.readTemperature();
    float h = bme.readHumidity();
    float p = bme.readPressure() / 100.0F;
    float a = bme.readAltitude(1013.25);
    int l = analogRead(LIGHT_PIN);
 
    envoyerDonnees(dh, t, h, p, a, l);
   
    delay(2000);
  }
}
 
void setup() {
  Serial.begin(9600);
  Serial.println(F("METEO"));
 
  pinMode(BOUTON_ROUGE, INPUT_PULLUP);
  pinMode(BOUTON_VERT, INPUT_PULLUP);
 
  led.init();
  led.setColorRGB(0, 0, 0, 0);
 
  chargerConfig();
 
  if (!rtc.begin()) erreurRTC();
  if (!bme.begin(BME_280)) erreurCapteur();
  if (!SD.begin(CHIP_SD)) erreurSDAcces();
 
  attachInterrupt(digitalPinToInterrupt(BOUTON_ROUGE), IntRouge, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BOUTON_VERT), IntVert, CHANGE);
 
  unsigned long debut = millis();
  bool rougeAppuye = false;
 
  while (millis() - debut < 5000) {
    if (digitalRead(BOUTON_ROUGE) == LOW) rougeAppuye = true;
  }
 
  if (rougeAppuye) {
    changerMode(1);
    modeConfiguration();
  } else {
    changerMode(0);
  }
 
  DateTime now = rtc.now();
  Jprecedent = now.day();
  dernier_enregistrement = millis();
}
 
void loop() {
  switch (mode) {
    case 0: modeStandard(); break;
    case 1: modeConfiguration(); break;
    case 2: modeEconomique(); break;
    case 3: modeMaintenance(); break;
  }
  delay(100);
}
 