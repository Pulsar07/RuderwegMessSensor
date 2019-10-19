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

  <h2>Einstellungen f&uuml;r den Ruderweg-Messsensor</h2>

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
  <h4>Messverfahren Ruderausschlag:</h4>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_distance_arc" name="nm_distancetype" value="arc"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_distance_arc">Messverfahren Kreisbogen</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_distance_chord" name="nm_distancetype" value="chord"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_distance_chord">Messverfahren Kreissehne / Abstand der Punkte</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="radio" id="id_vertical_distance" name="nm_distancetype" value="vertical_distance"
        onchange="sendNameValue(this.name, this.value)" >
    </div>
    <div class="col-75">
      <label for="id_distance_vertical">Messverfahren senkrechter Abstand (Tisch)</label>
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
    <p>WLAN ist: <span id="id_wlanConnetion">nicht verbunden</span></p>
  </div>
  <div class="container">
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
  <h3> Sensor-Kalibrierung</h3>
  <div class="row">
    <div class="col-25">
    <button type="button" id="id_calibrate" name="cmd_calibrate" value="yes"
      onclick="setElementValue('id_resp_calibrate', 'Kalibrierung l&auml;uft ...'); sendNameValue(this.name, this.value); getData('id_resp_calibrate');">
      Kalibrierung</button>
    </div>
    <div class="col-75">
      <label for="id_calibrate">
        Kalibriere den Sensor (nur MPU-6050)
     </label>
    </div>
  </div>
    <h4> Sensor Status: <span id="id_resp_calibrate"> - </span></h4>
    <p> Zur Kalibrierung muss der MPU-6050 Sensor mit der Unterseite (glatte Seite des Boards) m&ouml;glichst exakt horizontal liegen und darf sich ca. 5s nicht bewegen.
    </p>
  </div>
<hr>
  <div class="container">
  <div class="row">
    <div class="col-25">
      <button type="button" id="id_save" name="cmd_saveConfig" value="yes" onclick="sendAll(); sendNameValue(this.name, this.value)">
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
	    <button type="button" id="id_backToRoot" onclick="back()">Zur&uuml;ck</button>
    </div>
    <div class="col-75">
      <label for="id_backToRoot">
         zur&uuml;ck zur Hauptseite, ohne Speichern
     </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
    <button type="button" id="id_mcrestart" name="cmd_mcrestart" value="yes" onclick="sendNameValue(this.name, this.value)">
      Neustart Sensor</button>
    </div>
    <div class="col-75">
      <label for="id_mcrestart">
        Startet den Sensor neu
     </label>
    </div>
  </div>
  <p>Alle &Auml;nderungen wirken sich sofort aus und k&ouml;nnen mit dem &quot;Zur&uuml;ck&quot;-Button benutzt/getestet werden. Ausnahmen sind WiFi-Daten, die sich erst nach &quot;Speichern&quot; und &quot;Neustart&quot; auswirken.</p>
  <p>Sollen Einstellungen (auch Kalibrierung) nach einem Neustart erhalten bleiben, muss auf jeden Fall &quot;Speichern&quot; geklickt werden.</p>
  </div>
</div>

  ###<SCRIPT>###
  <script>

  function back() {
    sendAll();
    setTimeout(backToRoot, 200, );
  }

  function backToRoot() {
    window.location.href='/';
  }

  function sendAll() {
    sendValueForId("id_wlanSsid");
    sendValueForId("id_wlanPasswd");
    sendValueForId("id_apPasswd");
  }

  getData(
    "id_version",
    "id_invertAngle",
    "nm_anglePrecision",
    "nm_referenceAxis",
    "id_invertAmplitude",
    "nm_precisionAmplitude",
    "nm_distancetype",
    "id_wlanSsid",
    "id_apActive",
    "id_wlanConnetion",
    "id_resp_calibrate");
  </script>
  </body>
  </html>
)=====";
