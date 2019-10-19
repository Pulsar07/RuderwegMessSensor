const char SCRIPT[] PROGMEM = R"=====(
  <script>

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
    xhttp.open("GET", "setDataReq?name="+aId+"&value="+aValue, true);
    xhttp.send();
  }

  function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var responseValues = this.responseText.split(";");
        for (var i = 0; i < responseValues.length; i++) {
          var element = responseValues[i].split("=");
          var elementId = element[0];
          var elementValue = element[1];
          var htmlElement = document.getElementById(elementId);
          if (htmlElement.type == "radio") {
             htmlElement.checked = true;
          } else if (htmlElement.type == "checkbox") {
             htmlElement.checked = true;
          } else if (htmlElement.type == "password") {
            htmlElement.value = elementValue;
          } else if (htmlElement.type == "text") {
            htmlElement.value = elementValue;
          } else if (htmlElement.type == "number") {
            htmlElement.value = elementValue;
          } else {
            htmlElement.innerHTML = elementValue;
          }
        }
      }
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
