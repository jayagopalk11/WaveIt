void getMeetup(){
  WiFiClientSecure client;
  u8g2.clearBuffer();
  u8g2.setDrawColor(1);
  u8g2.setFont(u8g2_font_courB08_tf);
  u8g2.drawStr(20,24,"Loading....");
  u8g2.sendBuffer();
  delay(250);                    
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    displayCalenderError();
    return;
  }
  String url = "/v4/spreadsheets/"+sheetId+"/values/A1:A3?key="+googleApiKey;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
  String line = "";
  Serial.println("request sent");
  while (client.connected()) {
    line = client.readStringUntil('\n');
    Serial.println(line);
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String response = "";
  response = client.readString();
  Serial.println(response);
  client.stop();

  JsonObject& root = meetingBuffer.parseObject(response);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    displayDataError();
    return;
  }
  meeting =  root["values"][0][0];
  holidays =  root["values"][1][0];
  bdays =  root["values"][2][0];

  displayCurrentData();
}


/*
void getMeetings(){
   u8g2.clearBuffer();
   u8g2.setDrawColor(1);
   u8g2.setFont(u8g2_font_courB08_tf);
   u8g2.drawStr(20,24,"Loading....");
   u8g2.sendBuffer();
   delay(250);
   if(wifiConnected == true){
      Serial.println("wifi connected");
      HTTPSRedirect clientLocal(httpsPort);
      clientLocal.stop();
      if (!clientLocal.connected()){
        Serial.println("client not connected so connecting");
        clientLocal.connect(host, httpsPort);
        Serial.println("client connected");
      }
      Serial.println("about to get data");
      meetingData = clientLocal.getData(url, host, googleRedirHost);
      Serial.println("data received");
      //client.close();
      if(meetingData=="error"){
        displayCalenderError();
      }else{
        Serial.println(meetingData.length());
        Serial.println();
        Serial.println(meetingData);
        Serial.println();
        String dataStr = "";
        bool p = false;
        for (int i = 0; i < meetingData.length(); i++) {
          if ((String)meetingData[i] == delimeter)
            p = !p;
      
          if (p && (String)meetingData[i] != delimeter) {
            dataStr += (String)meetingData[i];
          }
        }
        Serial.println(dataStr);
        JsonObject& root = calenderBuffer.parseObject(dataStr);
              if (!root.success()) {
                Serial.println("parseObject() failed");
                displayDataError();
                return;
              }
                meeting = root["meetings"];
                holidays = root["holidays"];
                bdays = root["bdays"];
                bdaysCount = root["bdaysCount"];
                holidaysCount = root["holidaysCount"];
                
                displayCurrentData();
        }
        
   }else{
        u8g2.clearBuffer();
        u8g2.setDrawColor(1);
        u8g2.drawXBM(20, 0, weather_width, weather_height, noWifi_bits); 
        u8g2.setFont(u8g2_font_chikita_tf);
        u8g2.drawStr(0,46,"WiFi Unavailable");
        u8g2.sendBuffer();
   }
          
}

*/
void displayCalenderError(){
          u8g2.clearBuffer();  
          u8g2.setDrawColor(1);   
          u8g2.drawXBM(20, 0, weather_width, weather_height, noConnection_bits); 
          u8g2.setFont(u8g2_font_chikita_tf);
          u8g2.drawStr(0,46,"Connection Issue");
          u8g2.sendBuffer();
}
void displayDataError(){
          u8g2.clearBuffer();
          u8g2.setDrawColor(1);     
          u8g2.drawXBM(20, 0, weather_width, weather_height, noConnection_bits); 
          u8g2.setFont(u8g2_font_chikita_tf);
          u8g2.drawStr(0,46,"Data Error");
          u8g2.sendBuffer();
}
void displayCurrentData(){
          u8g2.clearBuffer();  // clear the internal memory 
          u8g2.setDrawColor(1);
          u8g2.setFont(u8g2_font_courB08_tf);
          u8g2.drawStr(4,7,"Calendar Info");
          u8g2.setFont(u8g2_font_synchronizer_nbp_tf);
          u8g2.drawStr(4,16,"Meetings: ");
          u8g2.drawStr(66,16,meeting);
          
          u8g2.drawStr(4,23,"Holidays: ");
          u8g2.drawStr(66,23,holidays);
          
          u8g2.drawStr(4,30,"Bdays: ");
          u8g2.drawStr(45,30,bdays);
          
          u8g2.sendBuffer();
}

