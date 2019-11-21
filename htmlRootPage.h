const char MAIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <head> 
    <meta http-equiv="cache-control" content="no-cache, must-revalidate, post-check=0, pre-check=0" /> 
    <meta http-equiv="cache-control" content="max-age=0" /> 
    <meta http-equiv="expires" content="0" /> 
    <meta http-equiv="expires" content="Tue, 01 Jan 1980 1:00:00 GMT" /> 
    <meta http-equiv="pragma" content="no-cache" /> 
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Ruderweg-Messsensor</title> 
    ###<CSS>### 
  </head>
  <body>
  <div class="container">
  <h1>Winkelmesser zur Bestimmung von RC-Modell Einstelldaten</h1>
  <p class="version">Version: <span id="id_version">0.10</span></p>
  <hr>
  </div class="container">
  <div class="container">
    <h2>Aktuelle Messwerte:</h2>
    <p>Sensortyp: &lt;<span id="id_sensortype"></span>&gt;</p>
  <div class="row">
    <div class="col-25">
      <button type="button" id="id_tara" name="cmd_taraAngle" value="true"
         onclick="sendNameValue(this.name, this.value)">TARA</button>
    </div>
    <div class="col-75">
      <label for="id_invertAmplitude">Setzte Messwerte auf 0</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
	    <label class=measureValue id="id_angle">  <span id="id_angleValue">0</span>&deg;</label>
    </div>
    <div class="col-75">
      <label class=measureValue for="id_angle">Winkel</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
	    <label class=measureValue id="id_amplitude">  <span id="id_amplitudeValue">0</span>mm</label>
    </div>
    <div class="col-75">
      <label class=measureValue for="id_angle">Ruderweg</label>
      <br><label  class=measureRemark for="id_angleCalcMethod">(<span type=text id="id_amplitudeCalcMethod">unbekannte Messmethode</span>)</label>
    </div>
  </div>
 
  <div class="row">
    <div class="col-25">
      <input type="number" class=measureInput id="id_rudderDepth" onchange="sendNameValue(this.id, this.value*10)"
         style="width: 6em" maxlength="3" min="5" max="150" step="1" value="0"></input>
    </div>
    <div class="col-75">
      <label for="id_rudderDepth" class=measureValue >Setze Rudertiefe in mm</label>
    </div>
  </div>
 
 <hr>
  <div class="row">
    <div class="col-25">
      <input type="number" id="id_targetAmplitude" onchange="sendNameValue(this.id, this.value*10)" 
         style="width: 6em" maxlength="3" min="-100" max="100" step="1" value="5"></input>

        <input type="checkbox" id="id_traceTargetAmplitude" name="traceTargetAmplitude" value="xx"
        onchange="sendNameValue(this.id, this.checked)"  onclick="beep(1,800,10)">

    </div>
    <div class="col-75">
      <label for="id_targetAmplitude">Setze Soll-Ruderweg [mm]</label>
    </div>
  </div>

  <hr>
  <h2>Min-/Max-Ruderweg-Messung:</h2>
  <div class="row">
    <div class="col-25">
      <label class="switch" data-on="Start" data-off="Stop">
      <input id="cmd_flightphaseActive" type="checkbox" onclick="sendNameValue(this.id, this.checked)">
      <span class="slider"></span>
    </div>
    <div class="col-75">
      <label for="cmd_flightphaseActive">
      Misst die akutelle Neutrallage, den Min- und Max-Wert und zeigt sie an
      </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
	 <label class="flightphaseValue" id="id_ruddermaxlabel">
  	 <span id="id_ruddermax">0.00</span>mm
	 </label>
    </div>
    <div class="col-75">
      <label class="flightphaseValue" for="id_ruddermaxlabel">
        Max-Wert
      </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
	 <label class="flightphaseValue" id="id_rudderneutrallabel">
  	 <span id="id_rudderneutral">0.00</span>mm
	 </label>
    </div>
    <div class="col-75">
      <label class="flightphaseValue" for="id_rudderneutrallabel">
        Neutral-Wert
      </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
	 <label class="flightphaseValue" id="id_rudderminlabel">
  	 <span id="id_ruddermin">0.00</span>mm
	 </label>
    </div>
    <div class="col-75">
      <label class="flightphaseValue" for="id_rudderminlabel">
        Min-Wert
      </label>
    </div>
  </div>
  <hr>
  <div class="row">
    <div class="col-25">
      <button type="button" id="id_settings" onclick="window.location.href='/adminPage'">Einstellungen &auml;ndern</button>
    </div>
    <div class="col-75">
      <label for="id_settings">
        Hier k&ouml;nnen die Einstellungen des Winkelmessers ge&auml;ndert werden
      </label>
    </div>
  </div>
  </div class="container">
  <br><br><a href="https://github.com/Pulsar07/RuderwegMessSensor">Projektseite auf GitHub</a>
  ###<SCRIPT>###
  <script>
  setInterval(function() {
    // Call a function repetatively with 2 Second interval
    getData("id_angleValue", "id_amplitudeValue", "cpx_flightphase" );
    beepTarget();
  }, 500); // 500mSeconds update rate
  getData("id_version", "id_rudderDepth", "id_targetAmplitude", "id_sensortype", "id_amplitudeCalcMethod"); 
  </script>
  </body>
  </html>
)=====";
