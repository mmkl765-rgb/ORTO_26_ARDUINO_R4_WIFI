// FILE PRONCIPALE DEL SISTEMA DI IRRIGAZIONE ORTO
// LE FUNZIONI SONO CONTENUTE NEL FILE FINZIONI.ino GESTITE TUTTE CON VARIABILI GLOBALI
// IL COMPILATORE LE INCLUDE AUTOMATICAMENTE ALLA COMPILAZIONIE

// INCLUSIONI VARIE...
    #include "DEBUG.h"                  // file per la gestione debug (pre-processore)
    #include <LiquidCrystal_I2C.h>      // includo libreria display i2c
    #include "PIN.h"                    // header per la definizione dei pin arduino
    #include "UMIDITA.h"                // header per i valori _RAW di umidita e soglia
    #include "TIMERS.h"                 // header per valori di timer e contatempo vari
    #include "VERSIONI.h"               // header versioni software

// BUFFER_SCRITTURA: arrray di caratteri per la creazione del buffer di scrittura per lcd
char W_BUFF[21];  // 20 caratteri su una linea + terminatore! gestione overflow ok

// CREAZIONE LCD con i parametri presi ca i2c monitor
LiquidCrystal_I2C lcd(0x27, 20, 4);  // settaggio LCD indirizzo 0x27 display 20 caratteri 4 linee

// STRUTTURA DATI per immagazzinamento dati letture aree, con l'array nella strutura posso fare
// uno storico delle letture (giocando con i posizionali) da visualizzare in uno SHOW DATI...
// i valori di partenza non sono messi a "0" in quanto potrebbe partire accidentalmente
// 15 posizioni nell'array mi permettono di controllare:
// 1) con intervallo letture di 30 min => 7 ore
// 2) con intervallo letture di 20 min => 4 ore
struct DATI { 
  int     NUMERO_MISURAZIONI = 0;
  uint8_t NUMERO_IRRIGAZIONI = 0;
  long Z1_RAW = 0;
  long Z2_RAW = 0;
  uint8_t ZONA1[15]; 
  uint8_t ZONA2[15];
};
DATI TERRENO;

// definizioni dei timers (spostato da file esterno per non rischiare conflitti in compilazione)
unsigned long timer_display;                                  // cronometro per aggiornamento display
unsigned long timer_letture;                                  // cronometro da usare con millis() per il controllo sensori + irrigazione
unsigned long timer_irrigazione;                              // cronometro da usare per controllo irrigazione ogni 6 h
unsigned long timer_sonovivo;                                 // cronometro da usare per il controllo del lampeggio del led verde

unsigned long PROSSIMA_LETTURA;                               // countdown per la prossima lettura sensori
unsigned long PROSSIMA_IRRIGAZIONE;                           // countdown per la prossima irrigazione se serve

// definizione dello stato pulsante
uint8_t STATO_PULSANTE = 0;                          // valore di partenza corrispondente alla modalità ordinaria  

// DEFINIZIONE MODALITA OPAERATIVA : definendo la modalità test si attiva il test dei sensori
// dai dati di lettura (visualizzati sia grezzi che in % ) si possono tarare le discrepanze
// #define TEST_MODE


// ====== DEFINIZIONI PER LA PARTE WEB ======

#include "WiFiS3.h"
#include "PAGINA_HOME.h"               // Includo la homepage
#include "PAGINA_WEB_IRRIGAZIONE.h"    // includo la pagina di "cortesia"
int refresh_web_page = 0;              // variabile per la gestione del tempo di refresh dinamico in secondi

// definizione parametri wifi
IPAddress local_IP(192, 168, 1, 222); // <--- Metti l'IP che preferisci (usa le virgole!)
IPAddress gateway(192, 168, 1, 222);  // Di solito coincide con l'IP della scheda
IPAddress subnet(255, 255, 255, 0); // La maschera di rete standard
const char ssid[] = "Wi-Fi_Orto_MIK";
const char pass[] = "orto2026";
bool WifiOK = true;

WiFiServer server(80);

// --- 1. LA FUNZIONE PROCESSOR (Il nostro doganiere) ---
String processor(const String& var) {
  if (var == "REFRESH_TIME_WW")   { return String(refresh_web_page);                }
  if (var == "VERSIONE_WW")       { return String(VERSIONE);                        }
  // --- 1. LIVELLO CISTERNA: Usiamo la tua funzione che controlla il sensore fisicamente
  if (var == "STATO_CISTERNA_WW") { 
    // Se VERIFICA_LIVELLO restituisce 1 significa che l'acqua c'è (OK), altrimenti è VUOTA
   if (VERIFICA_LIVELLO(LIVELLO_H2O, false) == 1) { return "ACQUA PRESENTE"; }
   else { return "VUOTA - BLOCCO"; }
  }
  if (var == "UMIDITA_Z1_WW")     { return String(TERRENO.ZONA1[0]);                }
  if (var == "STATO_Z1_WW")       { 
    if (TERRENO.ZONA1[0] >= SOGLIARH_Z1+6) { return "TERRENO OK"; }
    else if (TERRENO.ZONA1[0] >= SOGLIARH_Z1 && TERRENO.ZONA1[0] <= SOGLIARH_Z1+6 ) { return "QUASI DA BAGNARE"; }
    else { return "TERRENO SECCO"; }
  }
  if (var == "UMIDITA_Z2_WW")     { return String(TERRENO.ZONA2[0]);                }
  if (var == "STATO_Z2_WW")       { 
    if (TERRENO.ZONA2[0] >= SOGLIARH_Z2+6) { return "TERRENO OK"; }
    else if (TERRENO.ZONA2[0] >= SOGLIARH_Z2 && TERRENO.ZONA2[0] <= SOGLIARH_Z2+6 ) { return "QUASI DA BAGNARE"; }
    else { return "TERRENO SECCO"; }
  }
  if (var == "UPTIME_WW") { 
    unsigned long tot_min = millis() / 60000UL; int m = tot_min % 60; int h = (tot_min / 60) % 24; int g = tot_min / 1440;
    char buf_uptime[16]; snprintf_P(buf_uptime, sizeof(buf_uptime), PSTR("%02dg-%02dh:%02dm"), g, h, m);
    return String(buf_uptime);
  }
  if (var == "MISURAZIONI_WW")    { return String(TERRENO.NUMERO_MISURAZIONI);      }
  if (var == "IRRIGAZIONI_WW")    { return String(TERRENO.NUMERO_IRRIGAZIONI);      }
  for (int i = 0; i < 15; i++) {
    // Costruiamo i tag da cercare al volo, es: "VAL0_Z1", "VAL1_Z1", ecc...
    String tagZona1 = "VAL" + String(i) + "_Z1_WW"; String tagZona2 = "VAL" + String(i) + "_Z2_WW";
    // Se il browser sta chiedendo il tag della Zona 1 di questo indice
    if (var == tagZona1) { return String(TERRENO.ZONA1[i]); }
    // Se il browser sta chiedendo il tag della Zona 2 di questo indice
    if (var == tagZona2) { return String(TERRENO.ZONA2[i]); }
  }
  // Se non trova corrispondenze, cancella il tag tornando nulla ("")
  return "";
}

// =============  FINE DEFINIZIONI PARTE WEB =====


void setup() {
    // Tutto il setup è stato trasferito nel file C_SETTAGGIO.ino e viene incollato prima della compilazione
    // in ordine alfabetico (quindi per terzo). Qui basta solo richiamare la funzione vuota SETTAGGIO_SISTEMA()
    // ed il setup è fatto !!!

    SETTAGGIO_SISTEMA();

}

void loop() {
  // ===================================  MODO TEST =======================================
  // se definito la moddalita test carica solo la funzione che esegue i test ricorsivamente
  #ifdef TEST_MODE
    ESEGUI_TEST_MODE();
  #endif
  // ===================================  FINE MODO TEST =======================================

  // ===============================  MODO ORDINARIO =============================================
  #ifndef TEST_MODE // modalita ordinaria solo se non abiliti il modo test
    // se non è i lmodo test => è il modo ordinario pertanto basta 
    // eseguire la funzione ESECUZIONE_SISTEMA_IRRIGAZIONE contenuta nel file D_ESECUZIONE.ino

    ESECUZIONE_SISTEMA_IRRIGAZIONE();

  #endif
  // *********************************************  FINE MODO ORDINARIO **********************************************************

  // lampeggiamento del "sono vivo !"
    if (millis()-timer_sonovivo >= SONO_VIVO ) { 
      digitalWrite (LED_V,!digitalRead(LED_V));
      timer_sonovivo = millis();
    }

  prtln("FINE LOOP"); 
  
  // LampeggiaLED(LED_V,2,200);


} // FINE LOOP REALE

