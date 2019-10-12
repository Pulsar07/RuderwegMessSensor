const char ADMIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <body>

  <div id="so_fa">
  <h1>Winkelmesser zur Bestimmung von RC-Modell Einstelldaten</h1>
  <p>Version: <span id="id_version">0.10</span></p>

  <h2>Einstellungen f&uuml;r den RuderwegMessSonsor</h2>

  <hr>
  <h3>Winkel - Einstellungen:</h3>
  <input type="checkbox" id="id_invertAngle" name="invertAngle" value="x"
    onchange="sendNameValue(this.id, this.checked)">
  <label for="id_invertAngle"> : Invertiere die Winkelanzeige (+/-)</label>
  <br>
  <input type="radio" id="id_anglePrec_P001" name="nm_anglePrecision" value="P001"
    onchange="sendNameValue(this.name, this.value)" >
  <label for="id_anglePrec_P001"> : 0.01&deg;-Anzeige-Genauigkeit</label>
  <br>
  <input type="radio" id="id_anglePrec_P010" name="nm_anglePrecision" value="P010"
    onchange="sendNameValue(this.name, this.value)" >
  <label for="id_anglePrec_P010"> : 0.1&deg;-Anzeige-Genauigkeit</label>
  <br>
  <h4>Winkel - Bezugsachse</h4>
  <p>W&auml;hle die Bezugsachse aus die benutzt werden soll:</p>
  <input type="radio" id="id_xAxis" name="nm_referenceAxis" value="xAxis"
    onchange="sendNameValue(this.name, this.value)" >
  <label for="id_xAxis"> : X-Achse</label>
  <br>
  <input type="radio" id="id_yAxis" name="nm_referenceAxis" value="yAxis"
    onchange="sendNameValue(this.name, this.value)" >
  <label for="id_yAxis"> : Y-Achse</label>
  <br>
  <input type="radio" id="id_zAxis" name="nm_referenceAxis" value="zAxis"
    onchange="sendNameValue(this.name, this.value)" >
  <label for="id_zAxis"> : Z-Achse</label>
  <p>Auf dem Sensor sind oft Pfeile mit den Drehachsen aufgedruckt.
  Die X-Achse liegt meist orthogonal zur langen Ausrichtung des Chips</p>
  <hr>
  <h3>Ruderausschlag - Einstellungen:</h3>

  <input type="checkbox" id="id_invertAmplitude" name="invertAmplitudeValue" value="xx"
    onchange="sendNameValue(this.id, this.checked)">
  <label for="id_invertAmplitude"> : Invertiere die Weganzeige (+/-)</label>
  <br>
  <input type="radio" id="id_amplPrec_P010" name="nm_precisionAmplitude" value="P010"
    onchange="sendNameValue(this.name, this.value)" >
  <label for="id_amplPrec_P010"> : 0.1 mm-Anzeige-Genauigkeit</label>
  <br>
  <input type="radio" id="id_amplPrec_P050" name="nm_precisionAmplitude" value="P050"
    onchange="sendNameValue(this.name, this.value)" >
  <label for="id_amplPrec_P050"> : 0.5 mm-Anzeige-Genauigkeit</label>
  <br>
  <input type="radio" id="id_amplPrec_P100" name="nm_precisionAmplitude" value="P100"
    onchange="sendNameValue(this.name, this.value)" >
  <label for="id_amplPrec_P100"> : 1.0 mm-Anzeige-Genauigkeit</label>
  <br>
  <hr>
  <h3>WiFi - Einstellungen</h3>
  <p>&Auml;nderungen des WiFi-Einstellungen müssen zuerst gespeichert werden und wirken sich erst nach Neustart aus.</p>
  <div>
  <h4>Zugangsdaten zur Verbindung mit WLAN:</h4>
  <input type="text" id="id_wlanSsid" name="ssid" onchange="sendNameValue(this.id, this.value)">
  <label for="ssid">: WLAN - SSID (max. 15 Zeichen)</label>
  <br>
  <input type="text" id="id_wlanPasswd" name="password" onchange="sendNameValue(this.id, this.value)" >
  <label for="password">: WLAN - Passwort (max. 63 Zeichen) </label>
  <br>
<p>WLAN ist: <span id="id_wlanConnetion">nicht verbunden</span></p>
<h4>AccessPoint SSID UHU / 192.168.4.1 : </h4>
<input type="checkbox" id="id_apActive" name="id_apActive" value="xx" checked="checked"
  onchange="sendNameValue(this.id, this.checked)" >
<label for="id_apActive"> : AccessPoint mit SSID: UHU ist aktiv</label>
<br>
<input type="text" id="id_apPasswd" name="id_apPasswd" onchange="sendNameValue(this.id, this.value)" >
<label for="id_apPasswd">: Passwort des AccessPoint (AP) Zuganges (min. 8, max. 64 Zeichen) </label>

<hr>
	<a href="/?name=cmd_saveConfig&value=yes" target="_parent">
    <button type="button">Speichern</button>
  </a>: Speichert die Daten und geht zur&uuml;ck zur Hauptseite
  <br>
    <button type="button" name="cmd_resetConfig" value="yes" onclick="sendNameValue(this.name, this.value)">
     Alles zur&uuml;cksetzten</button>
     : Setzt alles Einstellungen zur&uuml;ck, WLAN Zugangsdaten werden gel&ouml;scht
  <br>
	<a href="/" target="_parent">
    <button type="button">Zur&uuml;ck</button>
  </a>: zur&uuml;ck zur Hauptseite, ohne Speichern
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
    "id_wlanPasswd",
    "id_apActive",
    "id_apPasswd",
    "id_wlanConnetion");
  </script>
  </html>
)=====";
