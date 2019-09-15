//Has functions testWifi, setupAP, launchWeb, createWebServer, getCreds, drawLoadingScreen, noNetworkDisp, getCurrentLocation, interruptRoutine, handleGesture, attachGestureSensor, checkGestureModule
bool testWifi(){
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 40 ) {
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
    delay(500);
    Serial.print(WiFi.status());//Loading screen here
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");//Show connection failed for 30 seconds here
  return false;
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    st = "<ol>";
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");

      // Print SSID and RSSI for each network found
      st += "<li>";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
      st += "</li>";


      delay(10);
    }
    st += "</ol>";
  }
  Serial.println("");

  delay(100);
  WiFi.softAP(ssid, passphrase, 6);
  Serial.println("softap");
  launchWeb(1);
  Serial.println("over");
}

void launchWeb(int webtype) {
  Serial.println("");
  Serial.println(webtype);
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  // Start the server
  server.begin();
  
  Serial.println("Server started");
}

void createWebServer(int webtype)
{
  if ( webtype == 1 ) {
    server.on("/", []() {
    
      Serial.println("creatingsite");
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
      
    });
    server.on("/setting", []() {
    
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");

        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
        }
        EEPROM.commit();
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.send(statusCode, "application/json", content);
      
    });
  } else if (webtype == 0) {
    server.on("/", []() {
    
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      server.send(200, "application/json", "{\"IP\":\"" + ipStr + "\"}");
      
    });
    server.on("/cleareeprom", []() {
    
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      Serial.println("clearing eeprom");
      for (int i = 0; i < 96; ++i) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
    });
   


  }
}


void getCreds(){
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  //Serial.println(epass);
  String ssidEeprom = esid.c_str();
  if ( ssidEeprom.length() > 1 ) {
    Serial.print(">SSID: ");
    Serial.print(esid.c_str());
    Serial.print(">PASS: ");
    //Serial.print(epass.c_str());
    WiFi.begin(esid.c_str(), epass.c_str());
    if (testWifi()) {
      Serial.println("tested wifi success");
      wifiConnected = true;
      getCurrentLocation();
      launchWeb(0);
      u8g2.clearBuffer(); 
      u8g2.sendBuffer();
      return;
    }
  }
  noNetworkDisp();  
  delay(10000);
  setupAP();
  
}



void drawLoadingScreen(){
  u8g2.clearBuffer(); 
  u8g2.drawXBM(0, 0, pig_width, pig_height, pig_bits);   
  u8g2.sendBuffer();
}

void noNetworkDisp(){
   u8g2.clearBuffer();
    
   u8g2.setFont(u8g2_font_profont10_mf);
   u8g2.drawStr(0,7,"No Wifi Available...");
   u8g2.drawStr(0,16,"Connect to ");
   u8g2.drawStr(0,25,"SSID: Piggy_Box");
   IPAddress ip = WiFi.softAPIP();
   String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
   int ipCount = ipStr.length()+1;
   char ipChar[ipCount];
   ipStr.toCharArray(ipChar, ipCount);
   u8g2.setFont(u8g2_font_profont11_tr);
   u8g2.drawStr(0,37,ipChar); 
   u8g2.sendBuffer();
   
   
}

void getCurrentLocation(){
    
        HTTPClient http;
         
        http.begin(geoLocationUrl); //get the current location details
        int httpCode = http.GET();
        if (httpCode > 0) {
          String locString = http.getString();
          JsonObject& root = locationBuffer.parseObject(locString);
          if (!root.success()) {
            Serial.println("parseObject() failed");
            return;
          }
          locationChar = root["city"];
          currentLocation = String(locationChar);
          Serial.println("getLocFn");
          Serial.println(currentLocation);
        }
    
    Serial.println(currentLocation);
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("UP");  
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        break;
      case DIR_LEFT:
        Serial.println("LEFT");
        if(screenIndex == 4){
          screenIndex = 1;
        }else{
          screenIndex += 1;
        }
        break;
      case DIR_RIGHT:
        Serial.println("RIGHT");
        if(screenIndex == 1){
          screenIndex = 4;
        }else{
          screenIndex -= 1;
        }
        break;
      case DIR_NEAR:
        Serial.println("NEAR");
        break;
      case DIR_FAR:
        Serial.println("FAR");
        break;
      default:
        Serial.println("NONE");
    }
  }
}

void attachGestureSensor(){
  if( isr_flag == 1 ) {
    detachInterrupt(0);
    handleGesture();
    isr_flag = 0;
    //attachInterrupt(0, interruptRoutine, FALLING);
    attachInterrupt(APDS9960_INT, interruptRoutine, FALLING);
  }
}

void checkGestureModule(){
   if ( apds.init() ) {
    Serial.println("APDS-9960 initialization complete");
    gestureModuleUp = true;
  } else {
    Serial.println("Something went wrong during APDS-9960 init!");
    gestureModuleUp = false;
  }

   if ( apds.enableGestureSensor(true) ) {
    Serial.println("Gesture sensor is now running");
    gestureModuleUp = true;
  } else {
    Serial.println("Something went wrong during gesture sensor init!");
    gestureModuleUp = false;
  }
}

