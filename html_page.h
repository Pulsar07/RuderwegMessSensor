const char MAIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <body>

  <div id="so_fa">
  <h1>Winkelmesser zur Bestimmung von RC-Modell Einstelldaten</h1>
  <p>Version: <span id="versionValue">0.10</span></p>
  	<button type="button" onclick="setTaraAngle()">TARA</button> : Setzte Winkelmessung auf 0<br>
    <input type="number" onchange="setRudderDepth(this.value)" min="5" max="150" step="0.5" value="30"> : Setzte Ruder-Tiefe</input>
  </div>

  <div>
    <h2>Messung:</h2>
  	<h2>Winkel <tab id=t1>: <span id="angleValue">0</span>&deg;</h2>
  	<h2>Weg <tab to=t1>: <span id="amplitudeValue">0</span>mm</h2>
  </div>
  <script>
  function setRudderDepth(aDepth) {
    var xhttp = new XMLHttpRequest();
    var val = Math.round(aDepth * 10);
    xhttp.open("GET", "setRudderDepth?value="+val, true);
    xhttp.send();
  }

  function setTaraAngle() {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "taraAngle", true);
    xhttp.send();
  }

  setInterval(function() {
    // Call a function repetatively with 2 Second interval
    getData();
  }, 1000); //2000mSeconds update rate

  function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var responseValues = this.responseText.split(";");
        document.getElementById("angleValue").innerHTML = responseValues[0];
        document.getElementById("amplitudeValue").innerHTML = responseValues[1];
      }
    };
    xhttp.open("GET", "readData", true);
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
  <br><br><a href="https://so-fa.de/nh">www.so-fa.de/nh : Albatross, die Seite f&uuml;r Modellflug und Technik</a>
  </body>
  </html>
)=====";
