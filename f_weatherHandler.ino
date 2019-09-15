
void getWeather(){
  String tempInStr;
  int tempInInt;
  int tempInStrLen;
  
  if(wifiConnected == true){
      HTTPClient http;
      String apiUrl;
      apiUrl = "http://api.openweathermap.org/data/2.5/weather?q="+currentLocation+"&appid="+weatherApiKey+"&units=metric";
      Serial.println(apiUrl);
      http.begin(apiUrl);  //Specify request destination
      
      int httpCode = http.GET();                                                                  //Send the request
      
      if (httpCode > 0) { //Check the returning code
        //String payload = http.getString();   //Get the request response payload
        //Serial.println(payload);                     //Print the response payload
        String result = http.getString();
        
        JsonObject& root = weatherBuffer.parseObject(result);
        if (!root.success()) {
          Serial.println("parseObject() failed");
          delay(10000);
          return;
        }
        Serial.println("parse done");
        main = root["weather"][0]["main"];
        icon = root["weather"][0]["icon"];
        temp = root["main"]["temp"];
        Serial.println(main);
        Serial.println(icon);
        Serial.println(temp);

        tempInInt = round(temp);
        
        tempInStr = hexify(tempInInt,"",true);
        tempInStrLen = tempInStr.length()+1;
      
      }else{
        icon = "no_response";
        weatherFetchFailure = true;
      }
      Serial.println("*******");   

  }else{
      icon = "no_wifi";
      weatherFetchFailure = true;
  }
      u8g2.clearBuffer();         // clear the internal memory
      u8g2.setDrawColor(1);
      if( weatherFetchFailure == false){
        
        char tempChar[tempInStrLen];
        tempInStr.toCharArray(tempChar, tempInStrLen);
        Serial.println("char temp");
        Serial.println(tempChar);
        u8g2.setFont(u8g2_font_chikita_tr);
        u8g2.drawStr(0,7,main);
        u8g2.drawXBM(0, 10, weather_width, weather_height, bitSelect(icon));
        u8g2.setFont(u8g2_font_timB12_tf);
        u8g2.drawStr(48,35,tempChar);
      }else{
        if(icon=="no_response"){
          u8g2.drawXBM(20, 0, weather_width, weather_height, noConnection_bits); 
          u8g2.setFont(u8g2_font_chikita_tf);
          u8g2.drawStr(0,46,"Connection Issue");
        }else{
          u8g2.drawXBM(20, 0, weather_width, weather_height, noWifi_bits); 
          u8g2.setFont(u8g2_font_chikita_tf);
          u8g2.drawStr(0,46,"WiFi Unavailable");
        }
      }
      u8g2.sendBuffer();          // transfer internal memory to the display
      weatherFetchFailure = false;
}
unsigned char* bitSelect(const char* icon){
          Serial.println("inside bitSelect");
          Serial.println(icon);
          //String strIcon = String(icon);
          
          if((strcmp(icon,"50d") == 0)||(strcmp(icon,"50n") == 0)){
            Serial.println("nistbits");
            return Mist_bits;
          }
          else if(strcmp(icon,"01d") == 0){
            Serial.println("clear sun bits");
            return clearSun_bits;
          }
          else if(strcmp(icon,"01n") == 0){
            Serial.println("clear moonbits");
            return clearMoon_bits;
          }
          else if(strcmp(icon,"02d") == 0){
            Serial.println("few clouds bits");
            return fewClouds_bits;
          }
          else if(strcmp(icon,"02n") == 0){
            Serial.println("few vloud night bits");
            return fewCloudsNight_bits;
          }
          else if((strcmp(icon,"03d") == 0)||(strcmp(icon,"03n") == 0)){
            Serial.println("sc clouds bits");
            return ScatteredClouds_bits;
          }
          else if((strcmp(icon,"04d") == 0)||(strcmp(icon,"04n") == 0)){
          //else if(icon == "04d"||icon == "04n"){
            Serial.println("br cluods bits");
            return brokenClouds_bits;
          }
          else if((strcmp(icon,"09d") == 0)||(strcmp(icon,"09n") == 0)||(strcmp(icon,"10d") == 0)||(strcmp(icon,"10n") == 0)){
          //else if(icon == "09d"||icon == "09n"||icon == "10d"||icon == "10n"){
            Serial.println("rain bits");
            return Rain_bits;
          }
          else if((strcmp(icon,"11d") == 0)||(strcmp(icon,"11n") == 0)){
          //else if(icon == "11d"||icon == "11n"){
            Serial.println("thunder bits");
            return Thunderstorm_bits;
          }
          else if((strcmp(icon,"13d") == 0)||(strcmp(icon,"013n") == 0)){
          //else if(icon == "13d"||icon == "13n"){
            Serial.println("snow bits");
            return Snow_bits;
          }
          //No wifi and no response
          else if(strcmp(icon,"no_wifi") == 0){
            Serial.println("No wifi bits");
            return noWifi_bits;
          }
          else if(strcmp(icon,"no_response") == 0){
            Serial.println("no response bits");
            return noConnection_bits;
          }
         return noWifi_bits;
}
