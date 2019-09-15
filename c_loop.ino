void loop() {
  // put your main code here, to run repeatedly:
  if(test){
    server.handleClient();
  }
  
  
  if(gestureModuleUp==true){
    attachGestureSensor();
  }
  handleScreen();
}
