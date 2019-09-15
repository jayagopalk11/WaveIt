//Has function hexify, splitHex, getSetTime, NTP CODES

void handleScreen(){
  Serial.print("Screen Index: ");
  Serial.print(screenIndex);
  Serial.println();
  nowMoment = rtc.now();
  
  switch (screenIndex) {
  case 1:
    Serial.println("Time mode");
    //Serial.println(nowMoment.second());
    if(prevSecond != nowMoment.second()){
      drawTimeWindow();
      prevSecond = nowMoment.second();
    }
    showWeather = true;
    showMeetings = true;
    prevDay = 0; //clearing the day so that while switching screens , calendar is still triggered
    break;
  case 2:
    Serial.println("Calendar mode");
    if(prevDay != nowMoment.day()){
      drawCalendarWindow();
      prevDay = nowMoment.day();
    }
    showWeather = true;
    showMeetings = true;
    break;
  case 3:
    {
      Serial.println("Weather");
      prevDay = 0;
      if(showWeather == true){
        getWeather();
      }else{
        int tempMinute = nowMoment.minute();
        if(tempMinute==15 || tempMinute==30 || tempMinute==45 || tempMinute==0){
          if(prevTempMinuteWthr != tempMinute ){
            getWeather();
            prevTempMinuteWthr = tempMinute;
          }
        }
      }
      showWeather = false;
      showMeetings = true;
      break;
    }
    /*
  case 4:
    {
      Serial.println("Meeting");
      //displayCurrentData();
      if(showMeetings == true){
        getMeetup();
      }else{
        int tempMinute = nowMoment.minute();
        if(tempMinute%5 == 0){
          if(prevTempMinuteMeet != tempMinute ){
            getMeetup();
            prevTempMinuteMeet = tempMinute; 
          }
        }
      }
      showMeetings = false;
      showWeather = true;
      prevDay = 0;
      break;
    }
    */
  default:
    break;
  
  }
}

void drawTimeWindow(){
    Serial.println("drawTime fn");
    String HourAndMinute;
    String secondsVal;
    HourAndMinute += hexify(nowMoment.hour(),"hh",false);
    HourAndMinute += "\x3a";
    HourAndMinute += hexify(nowMoment.minute(),"",false);
    secondsVal = hexify(nowMoment.second(),"",false);
    int hamCount = HourAndMinute.length()+1;
    int secCount = secondsVal.length()+1;
    char HAM[hamCount];
    char SEC[secCount];
    HourAndMinute.toCharArray(HAM, hamCount);
    secondsVal.toCharArray(SEC, secCount);
    Serial.println(HAM);
    Serial.println(SEC);
    Serial.println((nowMoment.hour()>12)?"PM":"AM");
    
    u8g2.clearBuffer();
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_timB08_tf);
    u8g2.drawStr(5,10,"Time");
    u8g2.setFont(u8g2_font_timB18_tf);
    u8g2.drawStr(5,35,HAM);
    u8g2.setFont(u8g2_font_timB10_tf);
    u8g2.drawStr(63,35,SEC);
    u8g2.setFont(u8g2_font_timB08_tf);
    u8g2.drawStr(63,22,(nowMoment.hour()>11)?"PM":"AM");
    u8g2.sendBuffer();  
}

void drawCalendarWindow(){
    Serial.println("drawCal fn");
    String dayVal;
    dayVal = hexify(nowMoment.day(),"",false);
    int dayCount = dayVal.length()+1;
    char DAY[dayCount];
    dayVal.toCharArray(DAY, dayCount);

    u8g2.clearBuffer();   
    u8g2.setFontMode(1); 
    u8g2.setDrawColor(1);
    u8g2.drawBox(5,3,75,12);
    u8g2.drawRFrame(5,3,75,43,0);
    u8g2.setFont(u8g2_font_timB18_tf);
    u8g2.drawStr(20,38,DAY);
    u8g2.setFont(u8g2_font_timB08_tf);
    u8g2.drawStr(50,33,daysOfTheWeek[nowMoment.dayOfTheWeek()]);
    u8g2.setDrawColor(0);
    u8g2.drawStr(31,13,monthOfTheYear[nowMoment.month()-1]);
    u8g2.sendBuffer();
}

String hexify(int value,String node,bool tempr){
 
  String position = "";
  if(node == "hh"){
    if(value == 00){
      return "\x31\x32";
    }
    else if(value>0 && value <13){
      position = String(value);
      if(value<10){
        position = "0"+position;
      }
      return splitHex(position,tempr);
    }
    else{
      position = String(value-12);
      if((value-12)<10){
        position = "0"+position;
      }
      return splitHex(position,tempr);
    }
  }
  else{
    position = String(value);
    if(value<10){
        position = "0"+position;
      }
    return splitHex(String(position),tempr);
  }
}

String splitHex(String rawValue, bool temprValue){
  
  char c[3];
  rawValue.toCharArray(c, 3);
  String result = "";
  result += "\x3";
  result += c[0];
  result += "\x3";
  result += c[1];
  if(temprValue==true){
    result += "\xb0 C";
  }
  return result;
}


void getSetTime(){
  if(wifiConnected==true){
    Serial.print("IP number assigned by DHCP is ");
    Serial.println(WiFi.localIP());
    Serial.println("Starting UDP");
    Udp.begin(localPort);
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());
    Serial.println("waiting for sync");
    setSyncProvider(getNtpTime);
    setSyncInterval(300);
  
    Serial.println(hour());
    delay(300); // wait for console opening
    //rtc.adjust(DateTime(2020, 1, 21, 23, 59, 30));
  
  
    if(timeRec != 0){
      rtc.adjust(DateTime(year(),month(),day(),hour(),minute(),second()));
      if (! rtc.begin()) {
        Serial.println("Couldn't find RTC");
        
      }
    
      if (rtc.lostPower()) {
        Serial.println("RTC lost power, lets set the time!");
         rtc.adjust(DateTime(year(),month(),day(),hour(),minute(),second()));
      }
    }
  }
}











/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  timeRec = 0;
  //return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
