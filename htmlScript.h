const char SCRIPT[] PROGMEM = R"=====(
  <script>
 a=new AudioContext()
  function beep(vol, freq, duration){
    v=a.createOscillator()
    u=a.createGain()
    v.connect(u)
    v.frequency.value=freq
    v.type="square"
    u.connect(a.destination)
    u.gain.value=vol*0.01
    v.start(a.currentTime)
    v.stop(a.currentTime+duration*0.001)
  }
  
    function beepTarget() {
    deltaAmplitude= id_targetAmplitude.value-document.getElementById('id_amplitudeValue').innerText;
    beepFreq = 800 + Math.sign(deltaAmplitude)*200 + deltaAmplitude*10;
    console.log("deltaAmplitude: " + deltaAmplitude + "   beepFreq: " + beepFreq);
    if (id_traceTargetAmplitude.checked) {
      beep(50, beepFreq, 80);
          console.log("dA: " + deltaAmplitude + "   bFr: " + beepFreq);
      if (Math.abs(deltaAmplitude) < 0.2) {
        beep(100,900,700);
        window.navigator.vibrate([150,100,150]);
      }
    }
  }
  
  function setClassElementsReadonly(aClassname, aValue) {
    var elements = document.getElementsByClassName(aClassname);
    console.log("found elements: " + elements.length);
    for (var i = 0; i < elements.length; i++) {
      var val = true;
      if (aValue == "false") {
        val = false;
      } 
      console.log("setting " + elements[i].id + " to readonly = " + val);
      elements[i].readOnly = val;
      elements[i].disabled = val;
    }
  }

  function setElementValue(aId, aValue) {
    var htmlElement = document.getElementById(aId);
    htmlElement.innerHTML = aValue;
  }

  function pollData(aId, aCount) {
    if( typeof pollData.counter == 'undefined' ) {
      pollData.counter = 0;
    }
    if (aCount < pollData.counter++) {
      getData(aId);
      setTimeout(pollData, 1000, aId, aCount);
    }
  }

  function sendValueForId(aId) {
    var value = document.getElementById(aId).value;
    if (value.length != 0) {
      sendNameValue(aId, value);
    }
  }

  function sendNameValue(aId, aValue) {
    if (aValue == "NA") {
        aValue=document.getElementById(aId).value;
    }
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      parseResponse(this);
    };
    xhttp.open("GET", "setDataReq?name="+aId+"&value="+aValue, true);
    xhttp.send();
  }

  function parseResponse(aResponse) {
      if (aResponse.readyState == 4 && aResponse.status == 200) {
        var responseValues = aResponse.responseText.split(";");
        console.log("responseValues.length:" + responseValues.length);
        for (var i = 0; i < responseValues.length; i++) {
          var element = responseValues[i].split("=");
          var elementId = element[0];
          console.log("elementId:" + elementId);
          if (elementId == "") { break }
          var elementValue = element[1];
          console.log("elementValue:" + elementValue);
          var htmlElement = document.getElementById(elementId);
          if (htmlElement == null) { 
            console.log("element with id:" + elementId + " not found");
            continue; 
          }
          if (htmlElement.type == "radio") {
             htmlElement.checked = true;
          } else if (htmlElement.type == "checkbox") {
             htmlElement.checked = true;
          } else if (htmlElement.type == "password") {
            htmlElement.value = elementValue;
          } else if (htmlElement.type == "text") {
            htmlElement.value = elementValue;
          } else if (htmlElement.type == "range") {
            htmlElement.value = elementValue;
            if (element.length > 3) {
              console.log("min :" + element[2]);
              htmlElement.min = element[2];
              console.log("max :" + element[3]);
              htmlElement.max = element[3];
            }
          } else if (htmlElement.type == "number") {
            htmlElement.value = elementValue;
          } else {
            htmlElement.innerHTML = elementValue;
          }
        }
      }
  }

  function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      parseResponse(this);
    };
    var requestLocation="getDataReq?";
    for (var i = 0; i < arguments.length; i++) {
       requestLocation += arguments[i]+"&";
    }
    requestLocation = requestLocation.substring(0,requestLocation.length-1);
    xhttp.open("GET", requestLocation, true);
    xhttp.send();
  }
  </script>
)=====";
