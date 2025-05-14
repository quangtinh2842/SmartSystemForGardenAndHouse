const char HOME_PAGE[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
    <head>
        <title>Wi-Fi Setup</title>
        <style>
        </style>
    </head>
    <body>
        <div>
            <h1>Wi-Fi Setup</h1>
        </div>
        <div>
            <div>SSID: </div>
            <div><input id="ssid"/></div>
            <div>Password: </div>
            <div><input id="pass"/></div>
            <div>
                <button id="submitButton" onclick="submitButtonPressed()">SUBMIT</button>
                <button id="skipButton" onclick="skipButtonPressed()">SKIP</button>
            </div>
            <div id="note" style="color: black;"></div>
        </div>
        <script>
            var request = new XMLHttpRequest();
            
            function submitButtonPressed() {
                let ssid = document.getElementById("ssid").value;
                let pass = document.getElementById("pass").value;
                
                if (ssid == "") {
                    document.getElementById("note").innerHTML = "The SSID is empty.";
                } else {
                    request.open("GET", "/handleSubmit?ssid="+ssid+"&pass="+pass, true);
                    request.onreadystatechange = readystatechangeHandle;
                    request.send();
                }
            }
            
            function skipButtonPressed() {
                request.open("GET", "/handleSkip", true);
                request.onreadystatechange = readystatechangeHandle;
                request.send();
            }
            
            function readystatechangeHandle() {
                if (request.readyState == 4 && request.status == 200) {
                    document.getElementById("note").innerHTML = request.responseText;
                }
            }
        </script>
    </body>
</html>
)=====";