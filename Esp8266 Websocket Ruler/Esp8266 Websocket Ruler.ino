#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>


const char* ssid="madam curie";
const char* password="Calculus_go_2018";
int LED=2;
int websockMillis=50;

const int trigP = 2;  //D4 Or GPIO-2 of nodemcu
const int echoP = 0;  //D3 Or GPIO-0 of nodemcu

long duration;
int distance;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 500;  //the value is a number of mill

ESP8266WebServer server(80);
WebSocketsServer webSocket=WebSocketsServer(88);
String webSite,JSONtxt;
boolean LEDonoff=true;

const char webSiteCont[] PROGMEM = 
R"=====(

<!DOCTYPE HTML>
<HTML>
<META name='viewport' content='width=device-width, initial-scale=1'>



<style>

#dynRectangle{
  width:0px;
  height:11px;
  position: fixed;
  top: 9px;
  background-color: red;
  z-index: -1;
  margin-top:200px;
}

#rectangle{

  width:1001px;
  

  background-image: 
    linear-gradient(90deg, 
      rgba(73, 73, 73, 0.5) 0, 
      rgba(73, 73, 73, 0.5) 2%, 
      transparent 2%
    ), 
    linear-gradient(180deg, 
      #ffffff 50%, 
      transparent 50%
    ), 
    linear-gradient(90deg, 
      transparent 50%, 
      rgba(73, 73, 73, 0.5) 50%, 
      rgba(73, 73, 73, 0.5) 52%, 
      transparent 52%
    ), 
    linear-gradient(180deg, 
      #ffffff 70%, 
      transparent 70%
    ), 
    linear-gradient(90deg, 
      transparent 10%,
      rgba(73, 73, 73, 0.4) 10%, 
      rgba(73, 73, 73, 0.4) 12%, 
      transparent 12%, 
      
      transparent 20%,
      rgba(73, 73, 73, 0.4) 20%, 
      rgba(73, 73, 73, 0.4) 22%, 
      transparent 22%, 
      
      transparent 30%, 
      rgba(73, 73, 73, 0.4) 30%,
      rgba(73, 73, 73, 0.4) 32%, 
      transparent 32%, 
      
      transparent 40%, 
      rgba(73, 73, 73, 0.4) 40%, 
      rgba(73, 73, 73, 0.4) 42%, 
      transparent 42%, 
      
      transparent 60%, 
      rgba(73, 73, 73, 0.4) 60%, 
      rgba(73, 73, 73, 0.4) 62%, 
      transparent 62%, 
      
      transparent 70%, 
      rgba(73, 73, 73, 0.4) 70%, 
      rgba(73, 73, 73, 0.4) 72%, 
      transparent 72%, 
      
      transparent 80%, 
      rgba(73, 73, 73, 0.4) 80%, 
      rgba(73, 73, 73, 0.4) 82%, 
      transparent 82%, 
      
      transparent 90%, 
      rgba(73, 73, 73, 0.4) 90%, 
      rgba(73, 73, 73, 0.4) 92%, 
      transparent 92%
    );
  background-size: 50px 20px;
  background-repeat: repeat-x;
  min-height: 20px;
  
  /* only needed for labels */
  white-space:nowrap;
  font-size:0;
  margin:auto;
  margin-top:200px;
  padding:0;
}
label {
  font-size:9px;
  padding-top:2px;
  display:inline-block;
  width:100px;
  text-indent:3px;
}

</style>




<body>

<div id="rectangle">
  <label>0</label>
  <label>100</label>
  <label>200</label>
  <label>300</label>
  <label>400</label>
  <label>500</label>
  <label>600</label>
  <label>700</label>
  <label>800</label>
  <label>900</label>
  <label>1000</label>
  <div id="dynRectangle"></div>
</div>

  
</body>


<SCRIPT>

  var a = 0;
  InitWebSocket()
  function InitWebSocket()
  {
    websock = new WebSocket('ws://'+window.location.hostname+':88/');
    websock.onmessage=function(evt)
    {
       JSONobj = JSON.parse(evt.data);
       var a = parseInt(JSONobj.LEDonoff);
       document.getElementById("dynRectangle").style.width = a+"px";
    } // end of onmessage
      
  } // end of InıtWebSocket




</SCRIPT>
</HTML>


)=====";

void WebSite(){

  server.send(400,"text/html",webSiteCont);
  
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t welength)
{
  String payloadString = (const char *)payload;
  Serial.print("payloadString= ");
  Serial.println(payloadString);

  if(type == WStype_TEXT) // receive text from cliet
  {
    byte separator=payloadString.indexOf('=');
    String var = payloadString.substring(0,separator);
    Serial.print("var= ");
    Serial.println(var);
    String val = payloadString.substring(separator+1);
    Serial.print("val =");
    Serial.println(val);
    Serial.println(" ");

    if(var == "LEDonoff")
    {
      LEDonoff = false;
      if(val=="ON")LEDonoff = true;
    }   
  } 
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(500);  
  }
  WiFi.mode(WIFI_STA);
  Serial.println(" Start ESP ");
  Serial.println(WiFi.localIP());
  server.on("/",WebSite);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  pinMode(trigP, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoP, INPUT);   // Sets the echoPin as an Input
  startMillis = millis();  //initial start time 
}

int dist = 0;


void loop() 
{
  webSocket.loop();
  server.handleClient();

  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {
    digitalWrite(trigP, LOW);   // Makes trigPin low
    delayMicroseconds(2);       // 2 micro second delay 
    
    digitalWrite(trigP, HIGH);  // tigPin high
    delayMicroseconds(10);      // trigPin high for 10 micro seconds
    digitalWrite(trigP, LOW);   // trigPin low
    
    duration = pulseIn(echoP, HIGH);   //Read echo pin, time in microseconds
    distance= duration*0.034/2;        //Calculating actual/real distance
    
    //Serial.print("Distance = ");        //Output distance on arduino serial monitor 
    Serial.println(distance);
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.

  }

  
  dist = distance * 1000 / 185;
  //if(dist == 1000)
  //  dist = 0;
    
  //Serial.println(dist);
  String LEDswitch = String(dist);
  JSONtxt = "{\"LEDonoff\":\""+LEDswitch+"\"}";
  webSocket.broadcastTXT(JSONtxt);

}













