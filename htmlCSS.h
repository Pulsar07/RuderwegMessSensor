const char CSS[] PROGMEM = R"=====(
<style>
    * {
      box-sizing: border-box;
    }

    h2, h3, h4, p {
      padding: 0px 0px 0px 40px;
    }

    input[type=text], select, textarea {
      width: 100%;
      padding: 12px;
      border: 1px solid #ccc;
      border-radius: 4px;
      resize: vertical;
    }

    input[type=checkbox], input[type=radio] {
      margin: 10px 12px 10px 15px;
      display: inline-block;
    }

    label {
      padding: 6px 12px 6px 0;
      display: inline-block;
    }

    input[type=submit] {
      background-color: #4CAF50;
      color: white;
      padding: 12px 20px;
      border: none;
      border-radius: 4px;
      cursor: pointer;
      float: right;
    }

    button:hover, input[type=submit]:hover {
      background-color: #99f;
    }

    .container {
      border-radius: 5px;
      background-color: #f2f2f2;
      padding: 20px;
    }

    .measureValue, .flightphaseValue {
      font-size: 1.5em;
      font-weight:bold;
    }

    .version {
      text-align: right;
    }

    .col-25 {
      text-align: left;
      float: right;
      width: 75%;
      padding-left: 5px;
    }

    .col-75 {
      text-align: right;
      float: left;
      width: 25%;
    }

    .row {
      background-color: #fff;
      margin: 5px 0px 5px 0px;
    }

    .row:after {
     /*  margin: 2px */
      content: "";
      display: table;
      clear: both;
    }

    /* Responsive layout - when the screen is less than 600px wide, make the two columns stack on top of each other instead of next to each other
    */
    @media screen and (max-width: 600px) {
      .col-25, .col-75, input[type=submit] {
        width: 100%;
        margin-top: 0;
      }
      .col-75 {
        text-align: left;
      }
      h2, h3, h4, p {
        padding: 0px 0px 0px 0px;
      }
    }

.switch {
 position: relative;
 display: inline-block;
 width: 60px;
 height: 34px;
}

/* Hide default HTML checkbox */
.switch input {
 opacity: 0;
 width: 0;
 height: 0;
}

/* The slider */
.slider {
 position: absolute;
 cursor: pointer;
 top: 0;
 left: 0;
 right: 0;
 bottom: 0;
 background-color: #ccc;
 -webkit-transition: .4s;
 transition: .4s;
}

.slider:before {
 position: absolute;
 content: "";
 height: 26px;
 width: 26px;
 left: 4px;
 bottom: 4px;
 background-color: white;
 -webkit-transition: .4s;
 transition: .4s;
}

input:checked + .slider {
 background-color: #2196F3;
}

input:focus + .slider {
 box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider:before {
 -webkit-transform: translateX(26px);
 -ms-transform: translateX(26px);
 transform: translateX(26px);
}

/* Rounded sliders */
.slider.round {
 border-radius: 34px;
}

.slider.round:before {
 border-radius: 50%;
}

</style>
)=====";
