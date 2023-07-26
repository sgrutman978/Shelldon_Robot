const char MAIN_page[] PROGMEM = R"=====(
  <!doctype html>
<head>
  <style>
    .grid-container {
      display: grid;
      grid-template-columns: auto auto auto;
      width: 270px;
      padding: 10px;
    }
    .grid-item {
      /*background-color: rgba(255, 255, 255, 0.8);*/
      background-color: #2196F3;
      border: 1px solid rgba(0, 0, 0, 0.8);
      padding: 20px;
      font-size: 30px;
      text-align: center;
    }
    .upSpeed{
      background-color: green;
    }
    .downSpeed{
      background-color: orange;
    }
    .stop{
      background-color: pink;
    }
  </style>
  <script>
  function motorCommand(x) {
    console.log("meheheh")
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, false);
     xhr.send();
     console.log(xhr.responseText);
   }
  </script>
</head>

<title>Hello, my name is Shelldon</title>
<a href="forward" target="myIframe">ON</a><br>
<a href="stop" target="myIframe">OFF</a><br>

<div class="grid-container">
  <button class="grid-item upSpeed" onclick="motorCommand('upl')"></button>
  <button class="grid-item" onmousedown="motorCommand('forward');" ontouchstart="motorCommand('forward');" onmouseup="motorCommand('stop');" ontouchend="motorCommand('stop');">F</button>
  <button class="grid-item upSpeed" onclick="motorCommand('upr')"></button>
  <button class="grid-item" onmousedown="motorCommand('left');" ontouchstart="motorCommand('left');" onmouseup="motorCommand('stop');" ontouchend="motorCommand('stop');">L</button>
  <button class="grid-item stop" onclick="motorCommand('stop')">S</button>
  <button class="grid-item"onmousedown="motorCommand('right');" ontouchstart="motorCommand('right');" onmouseup="motorCommand('stop');" ontouchend="motorCommand('stop');">R</button>
  <button class="grid-item downSpeed" onclick="motorCommand('downl')"></button>
  <button class="grid-item" onmousedown="motorCommand('backward');" ontouchstart="motorCommand('backward');" onmouseup="motorCommand('stop');" ontouchend="motorCommand('stop');">B</button>
  <button class="grid-item downSpeed" onclick="motorCommand('downr')"></button>
</div>

State:<iframe name="myIframe" width="100" height="25" frameBorder="0">

    <div class="container">
        <label for="left_speed">Left Speed:</label>
        <input type="number" id="left_speed" min="0" max="100" value="50">
        <label for="right_speed">Right Speed:</label>
        <input type="number" id="right_speed" min="0" max="100" value="50">
    </div>
)=====";
