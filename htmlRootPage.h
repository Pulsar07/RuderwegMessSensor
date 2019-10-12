const char MAIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <body>

  <div>
  <h1>Winkelmesser zur Bestimmung von RC-Modell Einstelldaten</h1>
  <p>Version: <span id="id_version">0.10</span></p>
  <button type="button" name="cmd_taraAngle" value="true" onclick="sendNameValue(this.name, this.value)">TARA</button> : Setzte Winkelmessung auf 0<br>
  <input type="number" id="id_rudderDepth" onchange="sendNameValue(this.id, this.value*10)" style="width: 6em" maxlength="3" min="5" max="150" step="1" value="0"> : Setzte Ruder-Tiefe</input>
  </div>
  <div>
    <h2>Messung:</h2>
  	<h2>Winkel <tab id=t1>: <span id="id_angleValue">0</span>&deg;</h2>
  	<h2>Weg <tab to=t1>: <span id="id_amplitudeValue">0</span>mm</h2>
  </div>
  <a href="/adminPage" target="_parent"><button type="button">Einstellungen &auml;ndern</button></a>
  <br><br><a href="https://so-fa.de/nh">www.so-fa.de/nh : Albatross, die Seite f&uuml;r Modellflug und Technik</a>
  ###<SCRIPT>###
  <script>
  setInterval(function() {
    // Call a function repetatively with 2 Second interval
    getData("id_angleValue", "id_amplitudeValue");
  }, 1000); //2000mSeconds update rate
  getData("id_version", "id_rudderDepth");
  </script>
  </body>
  </html>
)=====";
