// Salva questo codice dentro un file chiamato pagina_web_2.h nella stessa cartella del tuo sketch
const char PAGINA_WEB_IRRIGAZIONE[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
<html lang="it">
<head>
    <meta charset="UTF-8">
    
    <meta http-equiv="refresh" content="%REFRESH_TIME_WW%; url=http://192.168.1.222">

    <!-- 
    vedere se si puo fare anche cosi:
    <meta http-equiv="refresh" content="630; url=http://%LOCAL_IP_WW%">
    dandogli l'ip in un posto solo , se cambi ip non ti serve correggere il codice in giro....
    
    -->

    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Orto 2026 - Controllo Remoto</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f0f2f5; margin: 0; padding: 15px; color: #333; }
        h1 { text-align: center; color: #2e7d32; font-size: 24px; margin-bottom: 20px; }
        .container { max-width: 600px; margin: 0 auto; }
        .card { background: white; padding: 15px; margin-bottom: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .card-title { font-size: 18px; font-weight: bold; margin-bottom: 10px; display: flex; align-items: center; gap: 8px; }
        .data-row { display: flex; justify-content: space-between; padding: 6px 0; border-bottom: 1px solid #eee; }
        .data-row:last-child { border-bottom: none; }
        .stato-ok { background-color: #e8f5e9; border-left: 5px solid #4caf50; color: #2e7d32; }
        .stato-allarme { background-color: #ffebee; border-left: 5px solid #f44336; color: #c62828; }
        .btn-bagna { display: block; text-align: center; background-color: #2196f3; color: white; 
                     padding: 10px; margin-top: 10px; border-radius: 5px; text-decoration: none; font-weight: bold; box-sizing: border-box; }
        .btn-bagna:hover { background-color: #0b7dda; }
    </style>
	    <!-- icona pomodoro  = &#127813; -->
	    <!-- icona zucchina  = &#129362; -->
	    <!-- icona tegoline  = &#129739; -->
	    <!-- icona germoglio = &#127793; -->
	    <!-- icova aromatiche = &#127807;&#127793; -->
</head>
<body>
    <div class="container">
       <h1>🌿🌿&nbsp; ORTO V2026 &nbsp;🌿🌿</h1>	
       	<div class="card">
       	    <div class="card-title">&#128167;&#128167; ATTIVATA IRRIGAZIONE FOTZATA &#128167;&#128167;</div>
           	<div class="data-row"><span>Il sistema sembrerà bloccato per massimo 10 minuti, tempo per completare l'irrigazione di tutte le zone.</span></div>
            <div class="data-row"><span>Refresh pagina dinamico in base alla scelta della zona da irrigare.</span></div>
            <div class="data-row"><span>Si potrà visualizzare lo stato avanzamento della irrigazione solo con la barra avanzamento sul LCD.</span></div>
          </div>
    <!-- COPYRIGHT IN FONDO ALLA PAGINA -->
        <div style="text-align: center; font-size: 13px; color: #888; margin-top: 20px; margin-bottom: 10px;">
	        ORTO AUTOMATICO - VERSIONE %VERSIONE_WW%<br>
            &copy; 2026 MIK. Tutti i diritti riservati. <br>
            <span style="font-size: 11px; color: #aaa;">Proprietà intellettuale dell'Orto</span>
    </div>
  </body></html> 
)rawliteral";