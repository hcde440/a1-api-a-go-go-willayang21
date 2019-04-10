/*
 * This is a sketch that gets the ESP8266 to the WiFi and then connects to api.ipify.org using API key to get
 * our external IP address. With all that, ESP then connects to www.cat-fact.herokuapp.com and www.markerapi.com 
 * to retrieve data. The former doesn't require an API key while the latter does. 
 * 
 * Every time ESP uses an api it has to make a HTTP request, parse the string data, transform the data to json format, and 
 * stores the data we need into strings.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "NETGEAR38"; // wifi name
const char* pass = "imaginarysky003"; // wifi password

const String username = "willayang"; // one api requires username in the query
const String TMkey = "xfNbMqzmP2"; // api key for the trademark api
const String trademark = "sharetea"; 

String fact;  // initialize the string
String description; // initialize the string

void setup() {
  Serial.begin(115200); // initialize serial communications
  delay(10); // delay 10 miliseconds 
  
  // print wifi name
  Serial.print("Connecting to "); 
  Serial.println(ssid); 
  
  WiFi.mode(WIFI_STA); // start connecting to wifi
  WiFi.begin(ssid, pass); // initialize the wifi name and pw

  while (WiFi.status() != WL_CONNECTED) {   // keep connecting until the wifi is connected
    delay(500); // wait for 0.5 sec
    Serial.print(".");  // print dots
  }

  // inform the wifi is connected and print the IP address
  Serial.println(); Serial.println("WiFi connected"); Serial.println();
  Serial.print("Your ESP has been assigned the internal IP address ");
  Serial.println(WiFi.localIP());

  String ipAddress = getIP(); // create a variable to store the IP address
  getFacts(); // call the getFacts function which will parse a daily cat fact from the cat api
  Serial.println("————————————————————————————————"); 
  Serial.println("## Daily Cat Facts ##");
  Serial.println(fact);
  Serial.println();
  Serial.println("## Trademark Description for Sharetea ##");  
  getTM();  // call the getTM function which will parse the trademark data from the trademark api
  Serial.println(description);
}

void loop() {
}

// get the IP Address
String getIP() {
  HTTPClient theClient;   // Initialize the client library
  String ipAddress;       // create a string
  theClient.begin("http://api.ipify.org/?format=json");  // start listening for the incoming connection
  int httpCode = theClient.GET();  // make a HTTP request
  if (httpCode > 0) {     // if the response is not empty
    if (httpCode == 200) {  // check the connection to the endpoint                                
      DynamicJsonBuffer jsonBuffer;  // creates an entry point that handles
                                     //  the memory management and calls the parser
      String payload = theClient.getString();  // get the request response payload
      JsonObject& root = jsonBuffer.parse(payload);  // parse the json
      ipAddress = root["ip"].as<String>();   // return and store the ipaddress as strings
    } else {  // bad connection to the endpoint
      Serial.println("Something went wrong with connecting to the endpoint.");
      return "error";    // error message
    }
  }
  return ipAddress;  // return an IPaddress if the response is valid
}

// get cat facts from the api of cat-fact.herokuapp.com
void getFacts() {
  HTTPClient theClient;  // initialize the HTTPClient
  Serial.println("Making HTTP request for cats");  
  theClient.begin("http://cat-fact.herokuapp.com/facts/random"); // start listening for the incoming connection
  int httpCode = theClient.GET();   // make a HTTP request
  if (httpCode > 0) {               // if the response is not empty
    if (httpCode == 200) {          // check the connection to the endpoint
      Serial.println("Received HTTP payload.");     
      DynamicJsonBuffer jsonBuffer;   //  intialized the JsonBuffer
      String payload = theClient.getString();  // get the request response payload
      Serial.println("Parsing...");
      JsonObject& root = jsonBuffer.parse(payload);  // parse the json
      // Test if parsing succeeds.
      if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println(payload);
        return;
      }
      //Some debugging lines below:
      //      Serial.println(payload);
      //      root.printTo(Serial);

      //  place our data into the string.
      fact = root["text"].as<String>();
    } else {
      Serial.println("Something went wrong with connecting to the endpoint.");
    }
  }
}

// get trademark info from www.markerapi.com api
void getTM() {
  HTTPClient theClient;  // initialize the HTTPClient
  Serial.println("Making HTTP request for trademarks");  
  // // start listening for the incoming connection
  theClient.begin("http://www.markerapi.com/api/v2/trademarks/trademark/" + trademark + "/status/active/start/1/username/" + username + "/password/" + TMkey);
  int httpCode = theClient.GET();   // make a HTTP request
  if (httpCode > 0) {               // if the response is not empty
    if (httpCode == 200) {          // check the connection to the endpoint
      Serial.println("Received HTTP payload.");     
      DynamicJsonBuffer jsonBuffer;   //  intialized the JsonBuffer
      String payload = theClient.getString();  // get the request response payload
      Serial.println("Parsing...");
      JsonObject& root = jsonBuffer.parse(payload);  // parse the json
      // Test if parsing succeeds.
      if (!root.success()) {
        Serial.println("parseObject() failed");
        Serial.println(payload);
        return;
      }
      //Some debugging lines below:
      //      Serial.println(payload);
      //      root.printTo(Serial);
      
      // place the json value into the variable description
      description = root["trademarks"][0]["description"].as<String>();
    } else {
      Serial.println("Something went wrong with connecting to the endpoint.");
    }
  }
}
