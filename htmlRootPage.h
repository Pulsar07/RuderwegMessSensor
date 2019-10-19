const char MAIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <head>
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
      <br><label id="id_amplitudeCalcMethod" class=measureRemark for="id_angle">unbekannte Messmethode</label>
    </div>
  </div>
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
      <input type="number" id="id_rudderDepth" onchange="sendNameValue(this.id, this.value*10)"
         style="width: 6em" maxlength="3" min="5" max="150" step="1" value="0"></input>
    </div>
    <div class="col-75">
      <label for="id_rudderDepth">Setze Rudertiefe in mm</label>
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
      Misst die akutelle Nulllage, die Min- und Maxwert und zeigt sie an
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
	 <label class="flightphaseValue" id="id_ruddernulllabel">
  	 <span id="id_ruddernull">0.00</span>mm
	 </label>
    </div>
    <div class="col-75">
      <label class="flightphaseValue" for="id_ruddernulllabel">
        Null-Wert
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
  <br><br><a href="https://so-fa.de/nh">www.so-fa.de/nh : Albatross, die Seite f&uuml;r Modellflug und Technik</a>
  ###<SCRIPT>###
  <script>
  setInterval(function() {
    // Call a function repetatively with 2 Second interval
    getData("id_angleValue", "id_amplitudeValue", "cpx_flightphase" );
  }, 1000); //2000mSeconds update rate
  getData("id_version", "id_rudderDepth", "id_sensortype", "id_amplitudeCalcMethod");
  </script>
  </body>
  </html>
)=====";
