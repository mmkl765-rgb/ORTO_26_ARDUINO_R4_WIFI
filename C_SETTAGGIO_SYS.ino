/* FILE DI SETUP : il file contiene in una unica funzione tutto il stup del sistema controllo irrigazione. 
Il setup, oltre a settare i parametri generali, abilita la rete con una configurazione AP della scheda R4 Wifi.
Uso questo sistema per mantenere pulito il "main".ino, inoltre nominato con il suffisso
"C_" viene incollato per terzo nella fila dei file prima di compilare.*/

void SETTAGGIO_SISTEMA() {

  Serial.begin(9600); while (!Serial) { ; }     // abilita la seriale e finchè non è abilitata aspetta
  lcd.init(); lcd.backlight(); PULISCI_LCD();   // abilita lcd, accendi retroill e pulisci tutto

  // messaggio di benvenuto ...
  strcpy_P(W_BUFF, PSTR("*** ORTO V2026 ***")); prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
  snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("VERSIONE : %s"), VERSIONE); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
  strcpy_P(W_BUFF, PSTR("SETUP SISTEMA..."));   prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  delay(350);

  // set pin vari
  pinMode(PULSANTE,INPUT);
  pinMode(ALIM_SENS, OUTPUT);     digitalWrite(ALIM_SENS, LOW);     // setto alimentazione sensori e forzo la chiusura
  pinMode(ATTIVA_SCHEDA, OUTPUT); digitalWrite(ATTIVA_SCHEDA, LOW); // setto alimentazione scheda e forzo la chiusura
  pinMode(LIVELLO_H2O, INPUT);
  // N.B.: PIN VALVOLE vengono inizialmente impostati come input ed effettuo il cambio del PINMODE nella funzione
  // solo quando mi serve in modo da evitare accensioni involontarie e vibrazioni delle bobine 
  pinMode(VALVOLA_Z1, INPUT);
  pinMode(VALVOLA_Z2, INPUT);        
  pinMode(RELE_COMUNE, INPUT);       
  pinMode(RELE_POMPA, INPUT);
  // se sei in test mode non serve chiudere le valvole altrimenti si...
  #ifndef TEST_MODE              // definito nelle definizioni generali
    // chiusura valvole zona per sicurezza !!! UN PO BRUTALE !!!!
    strcpy_P(W_BUFF, PSTR("- CHIUDO VALVOLE    ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
    digitalWrite (RELE_COMUNE,HIGH); pinMode (RELE_COMUNE,OUTPUT);  // abilita il pin per la chiusura
    digitalWrite(ATTIVA_SCHEDA, HIGH);  delay(10); // attiva scheda rele e stabilizza
    digitalWrite(RELE_COMUNE, LOW);     delay(10);  // attiva uscita rele comune (quello per chiudere le valvole)
    BARRA_AVANZAMENTO(APERTURA_VALVOLA_ZONA, 3);
    digitalWrite(RELE_COMUNE, HIGH);    delay(10);  // attiva uscita rele comune (quello per chiudere le valvole)
    digitalWrite(ATTIVA_SCHEDA, LOW); pinMode(RELE_COMUNE, INPUT);
  #endif
  // set led + test
  strcpy_P(W_BUFF, PSTR("- SET LED + PIN  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  pinMode(LED_R, OUTPUT); LampeggiaLED(LED_R, 2, 100); digitalWrite(LED_R, LOW);
  pinMode(LED_V, OUTPUT); LampeggiaLED(LED_V, 2, 100); digitalWrite(LED_V, LOW);
  pinMode(LED_B, OUTPUT); LampeggiaLED(LED_B, 2, 100); digitalWrite(LED_B, LOW);
  BARRA_AVANZAMENTO(1000, 3);

  // Forzo il convertitore analogico dell'R4 a 10 bit (0-1023) come la vecchia Uno
  analogReadResolution(10);

  // set timer          
  strcpy_P(W_BUFF, PSTR("- SET TIMERS       ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  timer_letture     = millis();                                // faccio la prima lettura nel setup poi una ogni XX tempo
  timer_irrigazione = millis();                                // settaggio cronometri
  timer_display     = millis() - REFRESH_LCD_MODO_ORDINARIO;   // settaggio crono per aggiornamento display
  timer_sonovivo    = millis();                                // settaggio del crono per il lampeggio led
  BARRA_AVANZAMENTO(1000, 3);                                  // barra avanzamento 1 sec
  
  // effettua la prima lettura delle zone ed impostazione storico letture con il primo valore
  strcpy_P(W_BUFF, PSTR("-PRIMA LETTURA ZONE ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  LETTURA_SENSORI ();
  strcpy_P(W_BUFF, PSTR("- SET STORICO")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  for (uint8_t rr=0; rr<15; rr++) { TERRENO.ZONA1[rr] = TERRENO.ZONA1[0]; TERRENO.ZONA2[rr] = TERRENO.ZONA2[0]; }
  delay (500); // giusto per far leggere la scritta, ma si puo anche togliere.....

  // impostazione modo automatico (nel caso solito che all'avvio non venga premuto il pulsante ! )
  STATO_PULSANTE = 0;        // stato = 0 => automatico !
  PULISCI_LCD();
 
  // ========= GESTIONE PAGINA WEB - TELECOMANDO ===================
  strcpy_P(W_BUFF, PSTR(" - AVVIO AREA WEB - ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
  if (WiFi.status() == WL_NO_SHIELD) { 
    strcpy_P(W_BUFF, PSTR("ERR- CHIP WIFI KO  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
    // faccio in modo di continuare senza wifi - mettere flag ???
    WifiOK = false;   // 
    strcpy_P(W_BUFF, PSTR("AVVIO SENZA WIFI !! ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
    BARRA_AVANZAMENTO(1000, 3);           // barra avanzamento 1 sec
  }
  // Avvio access point
  strcpy_P(W_BUFF, PSTR("AVVIO A.P.   ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
  // controllare se la riga del wifi config funziona, possibili problemi!
  WiFi.config(local_IP, gateway, subnet);                 // usare IP fisso per l'AP impostato sopra
  // controllare se la riga del wifi config funziona, possibili problemi!
  WiFi.beginAP(ssid, pass); server.begin();               // avvia AP con le impostazioni scelte sopra
  strcpy_P(W_BUFF, PSTR("RETE WIFI ATTIVATA")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  prtln(" Pronta per il processor! ");
  prtln("Connettersi all'IP:  ");
  // sotto lasciare serial print !!!
  Serial.println (WiFi.localIP());
  BARRA_AVANZAMENTO(500, 3);           // barra avanzamento .5 sec
  // =============== FINE GESTIONE PAGINA WEB ===========================================

  strcpy_P(W_BUFF, PSTR("FINE SETUP SISTEMA ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  BARRA_AVANZAMENTO(1000, 3);           // barra avanzamento 1 sec

// fine funzione SETTAGGIO_SISTEMA
}

