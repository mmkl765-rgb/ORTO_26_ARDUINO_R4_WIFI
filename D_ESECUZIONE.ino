/*  FILE DI ESECUZIONE CICLI: questo file contiene il loop in una unica funzione.
    Per richiamarla dove serve nel loop basta far eseguire
    la funzione ESECUZIONE_SISTEMA_IRRIGAZIONE(); ed il gioco è fatto!!!
*/

void ESECUZIONE_SISTEMA_IRRIGAZIONE() {

  // =============================== GESTIONE DEL REBOOT PREVENTIVO OGNI 10 GG -INIZIO-  ==================================================
  if (millis() > AUTO_RESET) { STATO_PULSANTE = 3; }  // PER RIAVVIARE BASTA SCRIVERE COSI :  NVIC_SystemReset(); // ed il sistema si riavvia da zero
  // =============================== GESTIONE DEL REBOOT PREVENTIVO OGNI 10 GG -FINE- ==================================================

  // =====================================================================================================================================
  // ----------------------------------- GESTIONE PAGINA WEB - TELECOMANDO -------------------------------------
  if (WifiOK) {
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Telefono connesso! Elaborazione pagina... ");
      String richiestaAttuale = "";
      // --- TIMEOUT DI SICUREZZA: 2 secondi ---
      unsigned long tempoInizio = millis();
      const unsigned long timeout = 2000; 
      // Il ciclo gira finché è connesso E non è scaduto il tempo (se uno dei 2 casi è false esce dal ciclo)
      while (client.connected() && (millis() - tempoInizio < timeout)) {
        if (client.available()) {
          char c = client.read();
          richiestaAttuale += c;
          // Se il browser ha finito di mandare la richiesta HTTP
          if (c == '\n' && richiestaAttuale.endsWith("\r\n\r\n")) {
            // --- 1. Intercettato il "bagna" se c'è... ---
            if (richiestaAttuale.indexOf("GET /bagna") != -1) {
              /* le righe commentate servono per gestire l'irrigazione forzata per ogni singola zona, versione di poi
              if (richiestaAttuale.indexOf("GET /bagna_z1") != -1) {
                STATO_PULSANTE = 21;  // Solo Zucchine
                refresh_web_page = 300 + 60 + 30; 
                Serial.println("Attivazione: Solo Zucchine (STATO 21)");
              } 
              else if (richiestaAttuale.indexOf("GET /bagna_z2") != -1) {
                refresh_web_page = 120 + 60 + 30; 
                STATO_PULSANTE = 22;  // Solo Pomodori 
                Serial.println("Attivazione: Solo Pomodori (STATO 22)");
              } 
              else {
                STATO_PULSANTE = 20;  // Tutto
                refresh_web_page = 300 + 120 + 60 + 60 + 30; 
                Serial.println("Attivazione: Irrigazione Totale (STATO 20)");
              }
              */
              STATO_PULSANTE = 20;  // Tutto
              refresh_web_page = 10*60;  // 10 min * 60 sec
              Serial.println("Attivazione: Irrigazione Totale (STATO 20)");
              Serial.println("Invio pagina di cortesia...");
              GESTISCI_PAGINA_WEB(client, PAGINA_WEB_IRRIGAZIONE);              
              break; // Usciamo dal while, la risposta è stata data
            }
            // --- 2. Se non c'è i l"bagna" => richiesta della homepage ---
            else if (richiestaAttuale.indexOf("GET / ") != -1) { 
              Serial.println("Richiesta Home Page: Invio i dati dell'orto...");
              GESTISCI_PAGINA_WEB(client, PAGINA_HOME);
              STATO_PULSANTE = 0; 
              break; // Usciamo dal while
            }
            // --- 3.Richiesta pagine inesistenti o link errati ---
            else {
              Serial.println("Richiesta sconosciuta! Errore 404");
              client.println("HTTP/1.1 404 Not Found"); 
              client.println("Content-Type: text/html; charset=utf-8"); 
              client.println("Connection: close");
              client.println(); 
              client.println("<html><body><div style='text-align:center;'><br><br><h2>ER 404 - Pagina non trovata nel sistema ORTO!</h2></div></body></html>");
              break; // Usciamo dal while
            }
          } // Fine controllo \r\n\r\n
        } // Fine client.available()
      } // Fine ciclo while 
    // --- CHIUSURA INVIO PAGINA ad di fuori del while (connesso+timeout) ---
    client.flush(); 
    delay(1); 
    client.stop();
    Serial.println("Connessione chiusa.");
    } // Fine controllo if(client)
  } // Fine del if (WifiOK)
  // -------------------------- FINE PARTE GESTIONE PAGINE WEB ----------------------------------------------------------------
  // =====================================================================================================================================

  // =======================================================================================================
  // INIZIO LOGICA IDRAULICA AUTOMATICA (VECCHIO SKETCH CHE GIRAVA SU ARDUINO UNO R3)

    // RIAVVIO AUTOMATICO: dopo il tempo impostato esegue un reboot preventivo contro "glitch" 
    // abilito lo STATO a 3 cosi sfrutto la macchina a stati
    if (millis() > AUTO_RESET) { STATO_PULSANTE = 3; }

    // STATO OPERATIVO: in base alla tempo di ressione del pulsante effettua
    //                  operazioni diverse. Leggi a dislpay premendo il pulsante.
    uint8_t lettura = GESTIONE_PULSANTE(PULSANTE);
    if (lettura != 0) STATO_PULSANTE = lettura;
    prt("STATO PULSANTE NEL LOOP = "); prtln_v (STATO_PULSANTE);

    // MACCHINA A STATI
      // 0) MODALITA' ORDINARIA - con controllo livello acqua ed in caso blocco totale
      if (STATO_PULSANTE==0) {
        // aggiornamento del display solo dopo il tempo impostato di 1 minuto e controllo livello h20 (metti si buchi la cisterna!)
        if (millis() - timer_display >= REFRESH_LCD_MODO_ORDINARIO && VERIFICA_LIVELLO(LIVELLO_H2O,false) == 1) {
          PULISCI_LCD();
          // messaggi display...
          snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("  MODO ORDINARIO")); prtln_v(W_BUFF);  SCRIVI_LCD(0, 0, W_BUFF);
          snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR(" Z1: %2d%% | Z2: %2d%%"), TERRENO.ZONA1[0],TERRENO.ZONA2[0]); // 17 caratteri
          prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
          //strcpy_P(W_BUFF, PSTR("  TIMER CONTROLLI  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
          PROSSIMA_LETTURA      = (INTERVALLO_LETTURE - (millis() - timer_letture));            // valore in millis
          snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("LETT.  %2lu MIN (%3d)"), PROSSIMA_LETTURA/60000UL, TERRENO.NUMERO_MISURAZIONI );
          prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
          PROSSIMA_IRRIGAZIONE  = (INTERVALLO_IRRIGAZIONI - (millis() - timer_irrigazione));    // valore in millis
          snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("IRR.  %2lu MIN (%3d)"), PROSSIMA_IRRIGAZIONE/60000UL, TERRENO.NUMERO_IRRIGAZIONI);
          prtln_v(W_BUFF); SCRIVI_LCD(0, 3, W_BUFF);
          // reset timer aggiornamento LCD
          timer_display = millis();
        }
        
        // se sono passati 30 minuti dalla ultima lettura rieffettua la lettura
        if (millis() - timer_letture >= INTERVALLO_LETTURE) {
          PULISCI_LCD();
          strcpy_P(W_BUFF, PSTR("LETTURA SENSORI ...")); SCRIVI_LCD(0, 1, W_BUFF);
          LETTURA_SENSORI ();
          timer_letture = millis();   // reset timer letture ad "adesso"
        } 
        
        // se è passato il tempo per il controllo valori irrigazione e se c'e' acqua in cisterna
        // N.B.: se non ci fosse acqua in cisterna la funzione stessa gestisce il blocco totale del ciclo !!!
        if (millis() - timer_irrigazione >= INTERVALLO_IRRIGAZIONI && VERIFICA_LIVELLO(LIVELLO_H2O,false) == 1) { 
          PULISCI_LCD(); LampeggiaLED(LED_B, 4, 100);    // warning visivo
          // se serve bagna le zone
          strcpy_P(W_BUFF, PSTR("VERIFICA IRRIGAZIONE")); prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
          // ZONA 1
          if (TERRENO.ZONA1[0] <= SOGLIARH_Z1) {
            strcpy_P(W_BUFF, PSTR("ZONA 1 => IRRIGO...")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
            // bagna con tempo differenziato tra Z1 e Z2
            BAGNA_ZONA(VALVOLA_Z1, RELE_POMPA,TEMPO_IRRIGAZIONE_Z1); 
            strcpy_P(W_BUFF, PSTR("ZONA 1 => FINE !   ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
            }
          else { 
            strcpy_P(W_BUFF, PSTR("ZONA 1 => TERRA OK ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
            }
          // ZONA 2
          if (TERRENO.ZONA2[0] <= SOGLIARH_Z2) {
            strcpy_P(W_BUFF, PSTR("ZONA 2 => IRRIGO...")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
            // bagna con tempo differenziato tra Z1 e Z2
            BAGNA_ZONA(VALVOLA_Z2, RELE_POMPA,TEMPO_IRRIGAZIONE_Z2); 
            strcpy_P(W_BUFF, PSTR("ZONA 2 => FINE !   ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
            }
          else { 
            strcpy_P(W_BUFF, PSTR("ZONA 2 => TERRA OK ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
            }
          //poi resetta il timer "ad adesso"
          timer_irrigazione = millis();
          delay(500);
        } 
         STATO_PULSANTE=0;   // forza lo stato nella modalita automatica prima di uscire
      }

      // 1) VEDI DATI : visualizza tutti i dati del sistema, tempo attivita ecc ecc
      if (STATO_PULSANTE==1) {
        PULISCI_LCD();
        // prima schermata...
        strcpy_P(W_BUFF, PSTR("MODO  VEDI  DATI "));   prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
        strcpy_P(W_BUFF, PSTR("- ORTO AUTOMATICO -")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        strcpy_P(W_BUFF, PSTR("VERSIONE: ORTO V2026")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("VERSIONE SW: %s"), VERSIONE); prtln_v(W_BUFF); SCRIVI_LCD(0, 3, W_BUFF);
        delay (4000);
        // seconda schermata
        PULISCI_LCD();
        unsigned long t = millis() / 1000UL; // t in secondi
        int giorni = t / 86400UL; int ore = (t % 86400UL) / 3600; int minuti = (t % 3600) / 60; // da secondi a minuti-ore-giorni
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("UPTIME: %02dGG %02d:%02d"), giorni, ore, minuti);
        prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
        snprintf_P(W_BUFF, sizeof(W_BUFF),PSTR("MISURAZIONI: %d"),TERRENO.NUMERO_MISURAZIONI);
        prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        snprintf_P(W_BUFF, sizeof(W_BUFF),PSTR("IRRIGAZIONI: %d"),TERRENO.NUMERO_IRRIGAZIONI);
        prtln_v(W_BUFF); SCRIVI_LCD(0, 3, W_BUFF);
        delay (5000);
        // terza schermata - storico comparativo
        PULISCI_LCD();
        // Riga 0: Intestazione colonne (20 caratteri esatti)
        strcpy_P(W_BUFF, PSTR("[STORICO]   Z1 |  Z2")); SCRIVI_LCD(0, 0, W_BUFF); prtln_v(W_BUFF);
        // Riga 1 : i valori di adesso
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("-ADESSO:   %2u%% | %2u%%"), TERRENO.ZONA1[0], TERRENO.ZONA2[0]); SCRIVI_LCD(0, 1, W_BUFF); prtln_v(W_BUFF);
        // Riga 2: Lettura di 2 ore fa (indice [4])
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR(" -2 ORE:   %2u%% | %2u%%"), TERRENO.ZONA1[4], TERRENO.ZONA2[4]); SCRIVI_LCD(0, 2, W_BUFF); prtln_v(W_BUFF);
        // Riga 3: Lettura di 4 ore fa (indice [8])
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR(" -4 ORE:   %2u%% | %2u%%"), TERRENO.ZONA1[8], TERRENO.ZONA2[8]); SCRIVI_LCD(0, 3, W_BUFF); prtln_v(W_BUFF);
        // Un unico delay finale per dare il tempo di leggere, poi torna in automatico
        delay(8000); PULISCI_LCD(); STATO_PULSANTE = 0; // Torna in modalità ordinaria
      }

      // 2) IRRIGAZIONE FORZATA DI TUTTE LE AREE ( CIRCA 10 MINUTI !)
      // NUOVO CODICE per la versione WEB = 20 letto come 2 0  con la possibilita di fare irrigazione
      // forzata anche delle singole zone (modificare anche la gestione pulsante per inserire i nuovi
      // codici in caso di irrigazione forzata singole zone !!!!)
      // la funzione è di tipo bloccante pertanto finche non finisce l'irrigazione forzatat non fa altro
      if (STATO_PULSANTE==20 && VERIFICA_LIVELLO(LIVELLO_H2O,false) == 1) {
        LampeggiaLED(LED_B, 4, 150); digitalWrite(LED_B, HIGH);
        PULISCI_LCD();
        strcpy_P(W_BUFF, PSTR("IRRIGAZIONE FORZATA"));  prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
        strcpy_P(W_BUFF, PSTR("ZONA1: IRRIG. INIZIO")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        BAGNA_ZONA(VALVOLA_Z1, RELE_POMPA,TEMPO_IRRIGAZIONE_Z1);
        strcpy_P(W_BUFF, PSTR("ZONA1: IRRIG. FINE  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        strcpy_P(W_BUFF, PSTR("ZONA2: IRRIG. INIZIO")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        BAGNA_ZONA(VALVOLA_Z2, RELE_POMPA,TEMPO_IRRIGAZIONE_Z2);
        strcpy_P(W_BUFF, PSTR("ZONA2: IRRIG. FINE  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        digitalWrite(LED_B, LOW); STATO_PULSANTE=0; PULISCI_LCD();
      }

      // 3) REBOOT FORZATO VOLUTO DA UTENTE
      if (STATO_PULSANTE==3) {
        PULISCI_LCD();
        if (millis() > AUTO_RESET) {
          strcpy_P(W_BUFF, PSTR("  - AUTO-RESET -  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
          strcpy_P(W_BUFF, PSTR("MANUTENZIONE 10 GG")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        }
        else {
          strcpy_P(W_BUFF, PSTR("- REBOOT FORZATO -"));     prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
          strcpy_P(W_BUFF, PSTR("RICHIESTO DA UTENTE"));    prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
          strcpy_P(W_BUFF, PSTR("TRA 2 SECONDI"));          prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        }
        BARRA_AVANZAMENTO(2000, 3);
        // al posto del vecchio WDT ...
        NVIC_SystemReset();               // reset totale cpu arduini r4 wifi
      }

}

