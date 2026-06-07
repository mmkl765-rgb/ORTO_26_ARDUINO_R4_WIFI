// GESTIONE PAGINE WEB - questa funzione gestisce l'invio delle pagine web in base alla pressione dei
// link intercettando  le pagine come comandi. inoltre trasforma i tag contenuti tra i "%" nei numeri o strighe 
// specifici in base al contesto tramite la funzione "processor". 

void GESTISCI_PAGINA_WEB(WiFiClient& client, const char* pagina) {
  
  // --- 1. Inviamo le intestazioni standard (La pagina esiste!) ---
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println(); // Riga vuota obbligatoria

  // 1. Dichiariamo i due buffer vuoti e definiamo i "recinti" entro i quali devono stare i dati
  String bufferTag = "";
  String bufferInvio = ""; 
  // ricordarsi che nell'allocare la ram devi tener conto di letture al limite: sono arrivato a 199 caratteri (<200 per scrivere) e inizia il
  // tag che ne occupoa 10, finirei a 209 quindi con 9 in overflow !!! stai largo !!!! non scenere sotto il 230 meglio 250 !!!
  bufferInvio.reserve(250); // Alloca subito 250 byte in RAM per la dimensione del buffer dell'HTML
  bufferTag.reserve(30);    // Alloca subito 30 byte per contenere i nomi dei tag (es. %VERSIONE%)
  bool dentroIlTag = false;
  int i = 0;

  // --- 2. IL SETACCIO GENERICO IN TEMPO REALE ---
  // Leggiamo la pagina carattere per carattere finché non finisce ('\0')
  while (pagina[i] != '\0') {
    char carattereAttuale = pagina[i];

    if (carattereAttuale == '%') {
      if (!dentroIlTag) {
        // Inizio del tag: svuotiamo il buffer di testo normale inviando quello che c'è finora
        if (bufferInvio.length() > 0) {
          client.print(bufferInvio);   // invia quello che hai letto fin'ora..
          bufferInvio = "";
        }
        dentroIlTag = true;
        bufferTag = "";
      }
      else {
        // Fine del tag: interpelliamo il doganiere (processor) e incolliamo il valore vero
        dentroIlTag = false;
        bufferInvio += processor(bufferTag);
      }
    } else {
      if (dentroIlTag) {
        bufferTag += carattereAttuale; // Stiamo accumulando il nome del tag (es. VERSIONE)
      }
      else {
        bufferInvio += carattereAttuale; // Testo HTML normale
        // Per non finire la RAM dell'Arduino, ogni 200 caratteri svuotiamo il magazzino sul Wi-Fi
        if (bufferInvio.length() >= 200) {
          client.print(bufferInvio);
          bufferInvio = "";
        }
      }
    }
    i++;
  }

  // Svuotiamo l'ultimo carico rimasto nel buffer prima di chiudere la funzione
  if (bufferInvio.length() > 0) {
    client.print(bufferInvio);  // invia il resto se avanza qualosa...
  }
}