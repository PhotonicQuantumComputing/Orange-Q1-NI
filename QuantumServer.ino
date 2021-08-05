#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>

#define ssid ""
#define password ""
#define domain "qc" //http://qc.local

WiFiServer server(80);
float ref_val;
void setup() {
Serial.begin(1000000);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid,password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (!MDNS.begin(domain)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
 
 server.begin();
  Serial.println("TCP server started");

   MDNS.addService("http", "tcp", 80);
   ref_val=mean_val();
}

void loop() {
  // put your main code here, to run repeatedly:
   MDNS.update();
   if(millis()%100000<100){
     ref_val=mean_val();

   }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("");
  Serial.println("New client");

  // Wait for data from client to become available
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);
  client.flush();

  String s;
  if (req == "/") {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><head><title>Q-Server|Orange-Q1</title></head><body bgcolor=\"#ff9000\">";
    s+= "<h1>Orange-1Q</h1><p>Orange Q1 is a quantum computer. It consists one qubit The qubit fixed in the state of |v>=0.707106|0>+0.707106|1>. You can measure it</p>";
    s+= "<br/></br><iframe src=\"m1\"></iframe>";
    s+= "<br/><br/><a href=\"m0\">Only get the value</a><br/>The Orange 1Q is open source. Get the <a href=\"https://github.com/PhotonicQuantumComputing/Orange-Q1\" target=\"_blank\">source here</a></body></html>\r\n\r\n";
    Serial.println("Sending 200");
  }
  else if (req=="/m0"){
    s="HTTP/1.1 200 OK\r\nContent-Type: text/plain \r\n\r\n";
    if (analogRead(A0)>ref_val){
      s+="1";
    }
    else{
      s+="0";
    }
  }
    else if (req=="/m1"){
    s="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><head><title>Q-Server|Orange-Q1|Measure</title></head><body bgcolor=\"#ff4034\"><h1>";
    if (analogRead(A0)>ref_val){
      s+="1";
    }
    else{
      s+="0";
    }
    s+= "</h1></body></html>\r\n\r\n";
  }
  else {
    s = "HTTP/1.1 404 Not Found\r\n\r\n";
    Serial.println("Sending 404");
  }
  client.print(s);

  Serial.println("Done with client");
}

float mean_val(){
  long sum=0;
  float result;
  for (int i=0; i<100;i++){
    sum+=analogRead(A0);
  }
  result=sum/100;
  return result;
}
