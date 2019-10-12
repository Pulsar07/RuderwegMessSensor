const char ADMIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <body>

  <div id="so_fa">
  <h1>Winkelmesser zur Bestimmung von RC-Modell Einstelldaten</h1>
  <p>Version: <span id="versionValue">0.10</span></p>

  <h2>Einstellungen f&uuml;r den RuderwegMessSonsor</h2>
  <hr>
  <h3>Winkel - Einstellungen</h3>
  <p>W&auml;hle die Bezugsachse aus die benutzt werden soll:</p>
  <input type="radio" id="xAxis" name="referenceAxis" value="xAxis"
    onchange="pushSetting('referenceAxis', this.value)" checked="checked">
  <label for="xAxis"> : X-Achse</label>
  <br>
  <input type="radio" id="yAxis" name="referenceAxis" value="yAxis"
    onchange="pushSetting('referenceAxis', this.value)">
  <label for="yAxis"> : Y-Achse</label>
  <p>Auf dem Sensor sind oft Pfeile mit den Drehachsen aufgedruckt.
  Die X-Achse liegt meist orthogonal zur langen Ausrichtung des Chips</p>
  <hr>
  <h3>WiFi - Einstellungen</h3>
  <div>
  <p>Gib die Zugangsdaten deines WLANs ein:</p>
  <input type="text" id="ssid" name="ssid" onchange="pushSetting('wifissid', this.value)">
  <label for="ssid">WLAN - SSID: </label>
  <br>
  <input type="text" id="password" name="password" onchange="pushSetting('wifipassword', this.value)" >
  <label for="password">WLAN - Passwort: </label>
    <hr>
  	<a href="/?settings=save" target="_parent">
      <button type="button">Speichern</button>
    </a>: Speichert die Daten und geht zur&uuml;ck zur Hauptseite
    <br>
      <button type="button" onclick="pushSetting('resetConfig', 'na')">
       Alles zur&uuml;cksetzten</button>
       : Setzt alles Einstellungen zur&uuml;ck, WLAN Zugangsdaten werden gel&ouml;scht
    <br>
  	<a href="/?settings=nosave" target="_parent">
      <button type="button">Zur&uuml;ck</button>
    </a>: zur&uuml;ck zur Hauptseite, ohne Speichern
  </div>
  <script>
  function pushSetting(aName, aValue) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "pushSetting?name="+aName+"&value="+aValue, true);
    xhttp.send();
  }

  function getVersion() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var responseData = this.responseText;
        document.getElementById("versionValue").innerHTML = responseData;
      }
    };
    xhttp.open("GET", "readVersion", true);
    xhttp.send();
  }
  getVersion();
  </script>
  </html>
)=====";
