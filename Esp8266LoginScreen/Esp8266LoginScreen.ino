#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>


const char* ssid="madam curie";
const char* password="Calculus_go_2018";
int LED=2;
int websockMillis=50;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000*5;  // refresh time

bool flagContent = 0;

ESP8266WebServer server(80);
WebSocketsServer webSocket=WebSocketsServer(88);
String webSite,JSONtxt;
boolean LEDonoff=true;

char webSiteCont[2000];

char webSiteCont1[] = 
R"=====(

<!DOCTYPE html>    
<html>    
<head>    
    <title>Login Form</title>    
    <link rel="stylesheet" type="text/css" href="css/style.css">    
</head>    
<style>
body  
{  
    margin: 0;  
    background-color:#38e000;  
    font-family: 'Arial';  
}  
.login
{  
  width: 282px;  
  overflow: hidden;  
  margin: auto;  
  margin: 20 0 0 0px;  
  padding: 60px;  
  background: #24623f;  
  border-radius: 15px ;          
}  

h2{  
    text-align: center;  
    color: #277582;  
    padding: 20px;  
}  
label{  
    color: #08ffd1;  
    font-size: 17px;  
}  
 
#Pass{  
    width: 300px;  
    height: 30px;  
    border: none;  
    border-radius: 3px;  
    padding-left: 8px;  
      
}  
#log{  
    width: 100px;  
    height: 30px;
    margin-left:200px;
    border: none;  
    border-radius: 17px;  
    color: black;
}  

</style>
<body>    
    <h2>ESP-8266 Login Page</h2><br>    
    <div class="login">    
    <div id="login" >      
    <label><b>Password</b></label>    
        <input type="Password" name="Pass" id="Pass">    
        <br><br>    
        <button onclick="sendPass()" id="log" > Log in </button>
    </div>     
</div>    
</body>    

<SCRIPT>

  InitWebSocket()
  function InitWebSocket()
  {
    websock = new WebSocket('ws://'+window.location.hostname+':88/');

  } // end of InıtWebSocket

   function sendPass(){
    var stringPass=document.getElementById("Pass").value;
    var Password = 'Pass='+stringPass;  
     //alert(stringPass);
    websock.send(Password);
    location.reload();
    } 


</SCRIPT>
</html>  

)=====";

char webSiteCont2[]  = 
R"=====(

<!DOCTYPE HTML>
<HTML>
<META name='viewport' content='width=device-width, initial-scale=1'>

<SCRIPT>

  InitWebSocket()
  function InitWebSocket()
  {
    websock = new WebSocket('ws://'+window.location.hostname+':88/');
    websock.onmessage=function(evt)
    {
       JSONobj = JSON.parse(evt.data);
       document.getElementById('btn').innerHTML = JSONobj.Data;
    } // end of onmessage
      
  } // end of InıtWebSocket
  
  setInterval(function(){ location.reload();},1000*5);


</SCRIPT>

<style> 
#btn{
    position:relative;
    top:200px;
    width: 300px;
    margin: auto;  
    font-size: 100px;
  }
</style>

<BODY>
  <div id="btn"> </div>

</BODY>

</HTML>

)=====";

void WebSite(){

  server.send(200,"text/html",webSiteCont);
  
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
    Serial.print("var=");
    Serial.println(var);
    String val = payloadString.substring(separator+1);
    Serial.print("val=");
    Serial.println(val);
    Serial.println(" ");

    if( var == "Pass" && val == "1234")
    {
        Serial.println("switch screen"); 
        //String str = "hello world";
        size_t sizeStr = sizeof(webSiteCont2) / sizeof(webSiteCont2[0]);
        //Serial.println(sizeStr); 
        memcpy(webSiteCont,webSiteCont2 , sizeStr);
        flagContent = 1;
        startMillis = millis();  //initial start time 
        //str.toCharArray(webSiteCont,3);
    }   
  } 
}

void setup() {
  Serial.begin(9600);
  size_t sizeStr = sizeof(webSiteCont1) / sizeof(webSiteCont1[0]);
  //Serial.println(sizeStr); 
  //sizeStr = sizeof(webSiteCont2) / sizeof(webSiteCont2[0]);        
  //Serial.println(sizeStr); 
  memcpy(webSiteCont, webSiteCont1, sizeStr);
        
  pinMode(LED,OUTPUT);
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

}
unsigned int counter = 0;
void loop() {

 webSocket.loop();
 server.handleClient();
      
  if( flagContent == 1 )
  {
      currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
      if (currentMillis - startMillis >= period)  //test whether the period has elapsed
      {
    
      Serial.println("switch screen"); 
      size_t sizeStr = sizeof(webSiteCont1) / sizeof(webSiteCont1[0]);
      memcpy(webSiteCont,webSiteCont1 , sizeStr);
      flagContent = 0;
      startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
      }
  }
  
  counter ++;
  Serial.println( counter );
  
  if( flagContent == 1 )
  {
    String Data = String(counter);
    JSONtxt = "{\"Data\":\""+Data+"\"}";
    webSocket.broadcastTXT(JSONtxt);
  }
  
}













