void setup() {
  
  Wire.begin(APDS9960_SDA,APDS9960_SCL);
  pinMode(APDS9960_INT, INPUT);
  attachInterrupt(0, interruptRoutine, FALLING);
  Serial.begin(9600);
  u8g2.begin();
  drawLoadingScreen();
  getCreds();
  
  getSetTime();
  nowMoment = rtc.now();
  drawTimeWindow();
  checkGestureModule();
    
}
