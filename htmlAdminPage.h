const char ADMIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <head>
  ###<CSS>###
  </head>
  <body>
  <div id="so_fa">
  <div class="container">
  <h1>Winkelmesser zur Bestimmung von RC-Modell Einstelldaten</h1>
  <p>Version: <span id="id_version">0.10</span></p>

  <h2>Einstellungen f&uuml;r den RuderwegMessSensor</h2>

  <hr>
  <h3>Winkel - Einstellungen:</h3>
  <div class="row">
    <div class="col-25">
    <input type="checkbox" id="id_invertAngle" name="invertAngle" value="x"
      onchange="sendNameValue(this.id, this.checked)">
    </div>
    <div class="col-75">
      <label for="id_invertAngle">Invertiere die Winkelanzeige (+/-)</label>
    </div>
  </div>

  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_anglePrec_P001" name="nm_anglePrecision" value="P001"
        onchange="sendNameValue(this.name, this.value)">
    </div>
    <div class="col-75">
      <label for="id_anglePrec_P001">0.01&deg;-Anzeige-Genauigkeit</label>
    </div>
  </div>

  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_anglePrec_P010" name="nm_anglePrecision" value="P010"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_anglePrec_P010"> 0.1&deg;-Anzeige-Genauigkeit</label>
    </div>
  </div>
  </div>

  <div class="container">
  <h4>Winkel - Bezugsachse</h4>
  <p>W&auml;hle die Bezugsachse aus die benutzt werden soll:</p>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_xAxis" name="nm_referenceAxis" value="xAxis"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_xAxis"> X-Achse</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_yAxis" name="nm_referenceAxis" value="yAxis"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_yAxis"> Y-Achse</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_zAxis" name="nm_referenceAxis" value="zAxis"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_zAxis"> Z-Achse</label>
    </div>
  </div>
  </div>
  <p>Auf dem Sensor sind oft Pfeile mit den Drehachsen aufgedruckt.
  Die X-Achse liegt meist orthogonal zur langen Ausrichtung des Chips</p>
  <hr>

  <div class="container">
  <h3>Ruderausschlag - Einstellungen:</h3>
  <div class="row">
    <div class="col-25">
      <input type="checkbox" id="id_invertAmplitude" name="invertAmplitudeValue" value="xx"
        onchange="sendNameValue(this.id, this.checked)">
    </div>
    <div class="col-75">
      <label for="id_invertAmplitude"> Invertiere die Weganzeige (+/-)</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_amplPrec_P001" name="nm_precisionAmplitude" value="P001"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_amplPrec_P001"> 0.01 mm-Anzeige-Genauigkeit</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_amplPrec_P010" name="nm_precisionAmplitude" value="P010"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_amplPrec_P010"> 0.1 mm-Anzeige-Genauigkeit</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_amplPrec_P050" name="nm_precisionAmplitude" value="P050"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_amplPrec_P050"> 0.5 mm-Anzeige-Genauigkeit</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_amplPrec_P100" name="nm_precisionAmplitude" value="P100"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_amplPrec_P100"> 1.0 mm-Anzeige-Genauigkeit</label>
    </div>
  </div>
  </div>
  <hr>
  <div class="container">
  <h3>WiFi - Einstellungen</h3>
  <p>&Auml;nderungen des WiFi-Einstellungen m&uuml;ssen zuerst gespeichert werden und wirken sich erst nach Neustart aus.</p>
  <h4>Zugangsdaten zur Verbindung mit WLAN:</h4>
  <div class="row">
    <div class="col-25">
      <input type="text" id="id_wlanSsid" name="ssid"
        onchange="sendNameValue(this.id, this.value)" placeholder="WLAN SSID">
    </div>
    <div class="col-75">
      <label for="ssid">WLAN - SSID (max. 15 Zeichen)</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
        <input type="text" id="id_wlanPasswd" name="password"
	  onchange="sendNameValue(this.id, this.value)" placeholder="WLAN Passwort">
    </div>
    <div class="col-75">
        <label for="password">WLAN - Passwort (max. 63 Zeichen) </label>
    </div>
  </div>
  </div>
  <div class="container">
    <p>WLAN ist: <span id="id_wlanConnetion">nicht verbunden</span></p>
    <h4>AccessPoint SSID UHU / 192.168.4.1 : </h4>
  <div class="row">
    <div class="col-25">
      <input type="checkbox" id="id_apActive" name="id_apActive" value="xx"
        onchange="sendNameValue(this.id, this.checked)" >
    </div>
    <div class="col-75">
      <label for="id_apActive"> AccessPoint mit SSID: UHU ist aktiv</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="text" id="id_apPasswd" name="id_apPasswd"
        onchange="sendNameValue(this.id, this.value)" placeholder="AccessPoint Passwort">
    </div>
    <div class="col-75">
     <label for="id_apPasswd">
	     Passwort des AccessPoint (AP) Zuganges (min. 8, max. 64 Zeichen) </label>
    </div>
  </div>
  </div>

<hr>
  <div class="container">
  <div class="row">
    <div class="col-25">
      <button type="button" id="id_save" name="cmd_saveConfig" value="yes" onclick="sendNameValue(this.name, this.value)">
      Speichern</button>
    </div>
    <div class="col-75">
     <label for="id_save">
      Speichert die Einstellungen
     </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
    <button type="button" id="id_reset" name="cmd_resetConfig" value="yes" onclick="sendNameValue(this.name, this.value)">
      Alles zur&uuml;cksetzten</button>
    </div>
    <div class="col-75">
      <label for="id_reset">
        Setzt alles Einstellungen zur&uuml;ck, WLAN Zugangsdaten werden gel&ouml;scht
     </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
	    <button type="button" id="id_backToRoot" onclick="window.location.href='/'">Zur&uuml;ck</button>
    </div>
    <div class="col-75">
      <label for="id_backToRoot">
         zur&uuml;ck zur Hauptseite, ohne Speichern
     </label>
    </div>
  </div>
  </div>
  <!--
	<a href="/" target="_parent">
    <button type="button">Zur&uuml;ck</button>
  </a>: zur&uuml;ck zur Hauptseite, ohne Speichern
  -->
</div>

  ###<SCRIPT>###
  <script>
  getData(
    "id_version",
    "id_invertAngle",
    "nm_anglePrecision",
    "nm_referenceAxis",
    "id_invertAmplitude",
    "nm_precisionAmplitude",
    "id_wlanSsid",
    "id_apActive",
    "id_wlanConnetion");
  </script>
  </body>
  </html>
)=====";
