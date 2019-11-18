const char CALIBRATE[] PROGMEM = R"=====(
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
    <p> Zur Kalibrierung muss der MPU-6050 Sensor mit der Unterseite (glatte Seite des Boards) m&ouml;glichst exakt horizontal liegen und darf sich ca. 10-15s nicht bewegen.
    </p>
  </div>
)=====";
