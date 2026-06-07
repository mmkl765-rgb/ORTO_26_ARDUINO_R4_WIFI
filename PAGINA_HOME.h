// HOME PAGE del sistema ORTO WEB. versione 1: con solo un bottone link per l'irrigazione di tutte le zone, 
// come fatto nella parte senza web, in futuro potrei fare l'irrigazione forzata per zona....

const char PAGINA_HOME[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="15">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Orto 2026 - Controllo Remoto</title>
    <style>
        body { font-family: Arial, sans-serif; background-color: #f0f2f5; margin: 0; padding: 15px; color: #333; }
        h1 { text-align: center; color: #2e7d32; font-size: 24px; margin-bottom: 20px; }
        .container { max-width: 600px; margin: 0 auto; }
        .card { background: white; padding: 15px; margin-bottom: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .card-title { font-size: 18px; font-weight: bold; margin-bottom: 10px; display: flex; align-items: center; gap: 8px; justify-content: center; }
        .data-row { display: flex; justify-content: space-between; padding: 6px 0; border-bottom: 1px solid #eee; }
        .data-row:last-child { border-bottom: none; }
        .stato-ok { background-color: #e8f5e9; border-left: 5px solid #4caf50; color: #2e7d32; }
        .stato-allarme { background-color: #ffebee; border-left: 5px solid #f44336; color: #c62828; }
        
        /* STILE UNICO PER TUTTI I BOTTONI BLU */
        .btn-bagna { 
            display: block; 
            text-align: center; 
            background: linear-gradient(135deg, #2196f3 0%, #1976d2 100%); 
            color: white; 
            padding: 12px 20px; 
            margin-top: 15px; 
            border-radius: 6px; 
            text-decoration: none; 
            font-weight: bold; 
            box-sizing: border-box;
            box-shadow: 0 4px 6px rgba(33, 150, 243, 0.3);
            transition: all 0.2s ease-in-out;
            letter-spacing: 0.5px;
        }
        .btn-bagna:hover { 
            background: linear-gradient(135deg, #1976d2 0%, #1565c0 100%);
            box-shadow: 0 6px 12px rgba(33, 150, 243, 0.4);
            transform: translateY(-1px);
        }
        .btn-bagna:active {
            transform: translateY(1px);
            box-shadow: 0 2px 4px rgba(33, 150, 243, 0.4);
        }

        /* CONTENITORE PER AFFIANCARE I BOTTONI SOTTO */
        .btn-container {
            display: flex;
            gap: 10px;
            margin-top: 10px;
        }

        /* VARIANTE PER I BOTTONI PICCOLI (Mantiene lo stile ma ottimizza lo spazio) */
        .btn-bagna-small {
            flex: 1;
            margin-top: 0;
            padding: 10px 5px;
            font-size: 14px;
        }
    </style>
        </head>
    <!-- tag usati messi tra i valori percentuali
        VERSIONE_WW
        STATO_CISTERNA_WW 
        UMIDITA_Z1_WW
        STATO_Z1_WW
        UMIDITA_Z2_WW
        STATO_Z2_WW
        UPTIME_WW
        MISURAZIONI_WW
        IRRIGAZIONI_WW
        da VAL14_Z1_WW a VAL0_Z1_WW (valori array storico Z1)
        da VAL14_Z2_WW a VAL0_Z2_WW (valori array storico Z2)
    -->
<body>
    <div class="container">
        <h1>🌿🌿&nbsp; ORTO SMART V-2026 &nbsp;🌿🌿</h1>   
    <div class="card">
            <div class="card-title">&#128167;&#128167;&nbsp;&nbsp;&nbsp;&nbsp;STATO CISTERNA&nbsp;&nbsp;&nbsp;&nbsp;&#128167;&#128167;</div>
                <div class="data-row"><span>Livello Acqua:</span> <strong>%STATO_CISTERNA_WW%</strong></div>
        </div>
        <div class="card">
            <div class="card-title">&#129362;&nbsp;&nbsp;&#127793;&nbsp;&nbsp;&nbsp;&nbsp;
		ZONA NUMERO 1&nbsp;&nbsp;&nbsp;&nbsp;(Zucchine-Cetrioli)</div>
            <div class="data-row"><span>Umidità Terreno:</span> <strong>%UMIDITA_Z1_WW%</strong></div>
            <div class="data-row"><span>Stato Irrigazione:</span> <strong>%STATO_Z1_WW%</strong></div>

        </div>
        <div class="card">
            <div class="card-title">&#127813;&nbsp;&nbsp;&#127807;&nbsp;&nbsp;&#127793;
		&nbsp;&nbsp;&nbsp;&nbsp;ZONA NUMERO 2&nbsp;&nbsp;&nbsp;&nbsp;(Pomodori-Erbe Aromatiche)</div>
            <div class="data-row"><span>Umidità Terreno:</span> <strong>%UMIDITA_Z2_WW%</strong></div>
            <div class="data-row"><span>Stato Irrigazione:</span><strong>%STATO_Z2_WW%</strong></div>
        </div>
        
        <div class="card">
            <div class="card-title">&#128167;&#128167;&nbsp;&nbsp;&nbsp;&nbsp;IRRIGAZIONE FORZATA&nbsp;&nbsp;&nbsp;&nbsp;&#128167;&#128167;</div>
            <a class="btn-bagna" href="/bagna">&#128167; Avvia Irrigazione Forzata</a>
            <!-- parte commentata : contiene i bottoni link per irrigare in modo forzato le singole zone
            <div class="btn-container">
                <a class="btn-bagna btn-bagna-small" href="/bagnaZ1">&#129362; Solo Zona 1</a>
                <a class="btn-bagna btn-bagna-small" href="/bagnaZ2">&#127813; Solo Zona 2</a>
            </div>
            -->
            <small style="display: block; text-align: center; color: #666; margin-top: 12px;">N.B.: Il sistema sembrerà bloccato durante l'irrigazione.</small>
        </div>
        
        <div class="card">
            <div class="card-title">🌿🌿&nbsp;&nbsp; INFO ORTO 2026 &nbsp;&nbsp;🌿🌿</div>
            <div class="data-row"><span>Versione SW:</span><strong>%VERSIONE_WW%</strong></div>
            <div class="data-row"><span>Acceso da:</span><strong>%UPTIME_WW%</strong></div>
            <div class="data-row"><span>Misurazioni Effettuate:</span><strong>%MISURAZIONI_WW%</strong></div>
            <div class="data-row"><span>Irrigazioni Effettuate:</span><strong>%IRRIGAZIONI_WW%</strong></div>
            <div style="margin-top: 20px; padding-top: 5px;">
        <div class="data-row">📈 Storico Umidità Zucchine (ultime 7h):</div>
                <div style="display: flex; justify-content: space-between; align-items: flex-end; height: 100px; padding: 8px 10px; background: #e9ecef; border-radius: 8px; box-shadow: inset 0 2px 4px rgba(0,0,0,0.08); box-sizing: border-box;">
                    <div style="flex: 1; height: %VAL14_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL13_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL12_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL11_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL10_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL9_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL8_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL7_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL6_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL5_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL4_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL3_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL2_Z1_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL1_Z1_WW%%; min-heght: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL0_Z1_WW%%; min-height: 4px; background-color: #4caf50; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                </div>
            </div>
            <div style="margin-top: 20px; padding-top: 15px; border-top: 1px solid #eee;">
        <div class="data-row">📈 Storico Umidità Pomodori (ultime 7h):</div>
                <div style="display: flex; justify-content: space-between; align-items: flex-end; height: 100px; padding: 8px 10px; background: #e9ecef; border-radius: 8px; box-shadow: inset 0 2px 4px rgba(0,0,0,0.08); box-sizing: border-box;">
                    <div style="flex: 1; height: %VAL14_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL13_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL12_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL11_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL10_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL9_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL8_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL7_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL6_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL5_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL4_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL3_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL2_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                    <div style="flex: 1; height: %VAL1_Z2_WW%%; min-height: 4px; background-color: #2196f3; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>                    
                    <div style="flex: 1; height: %VAL0_Z2_WW%%; min-height: 4px; background-color: #4caf50; margin: 0 3px; border-radius: 4px 4px 0 0;"></div>
                </div>
            </div>
        </div>
   </div>
        <div style="text-align: center; font-size: 13px; color: #888; margin-top: 20px; margin-bottom: 10px;">
            ORTO AUTOMATICO - VERSIONE %VERSIONE_WW%<br>
            &copy; 2026 MIK. Tutti i diritti riservati. <br>
            <span style="font-size: 11px; color: #aaa;">Proprietà intellettuale dell'Orto</span>
        </div>
    </div>
</body>
</html>
)rawliteral";