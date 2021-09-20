Button CenterBTN,LockBTN;

Slider2D LeftStick, RightStick;
boolean stickSetup=false,toggleCenter=true,toggleLock=true;

/*************************************/
//Joystick setup


//JoyStick

float Joy[] = new float[8];
int TXmode=2
    ,TXmid=1500
    ;

float leftX = TXmid;
float leftY = 1000;
float rightX = TXmid;
float rightY = TXmid;
int AUX_1=  1000;
int AUX_2 = 1000;
int AUX_3 = 1000;
int AUX_4 = 1000;
/*************************************/



// Button Actions
void CenterJoystick(){
  toggleCenter=!toggleCenter;
}

void TX_MODE(boolean theFlag) {
  toggleCenter=true;toggleLock=true;
  if (theFlag) {
     TXmode = 2;
     modeToggle.setCaptionLabel("Mode 2");}
   else {
     TXmode = 1;
     modeToggle.setCaptionLabel("Mode 1");}
 }



void LOCK(){
  toggleLock=!toggleLock;
}

void setupJoystick(){
  toggleCenter=true;toggleLock=true;
    
  if(TXmode==1) { leftY = TXmid; rightY = 10000; } //Switch to TXMode 1
  
  CenterBTN =      controlP5.addButton("CenterJoystick" ,  1,  xGraph+350, yGraph-190,60,19) .setLabel("Relese").setColorBackground(red_).moveTo("ser_TX");
  LockBTN =        controlP5.addButton("LOCK" ,  1,              xGraph+320, yGraph-100,130,19) .setLabel("unLock Throttle").setColorBackground(red_).moveTo("ser_TX");
   
  stickSetup=true;
  for (i=0;i<4;i++) { Joy[i] =0; }
  for (i=4;i<8;i++) { Joy[i] =1000;}
  
    // add a vertical slider
  controlP5.addSlider("AUX_1")
     .setPosition(150,100)
     .setSize(20,30)
     .setRange(1000,2000)
     .setValue(1000)
     .setNumberOfTickMarks(3)
     .moveTo("ser_TX")
     ;  controlP5.addSlider("AUX_2")
     .setPosition(150,200)
     .setSize(20,30)
     .setRange(1000,2000)
     .setValue(1000)
     .setNumberOfTickMarks(3)
     .moveTo("ser_TX")
     ;
       controlP5.addSlider("AUX_3")
     .setPosition(550,100)
     .setSize(20,30)
     .setRange(1000,2000)
     .setValue(1000)
     .setNumberOfTickMarks(3)
     .moveTo("ser_TX")
     ;  controlP5.addSlider("AUX_4")
     .setPosition(550,200)
     .setSize(20,30)
     .setRange(1000,2000)
     .setValue(1000)
     .setNumberOfTickMarks(3)
     .moveTo("ser_TX")
     ;


LeftStick = controlP5.addSlider2D("Left")
         .setPosition(250,100)
         .setSize(100,100)
         .setMaxX(2000)
         .setMaxY(1000)
         .setMinX(1000)
         .setMinY(2000)
         .setLabel("")
         .setArrayValue(new float[] {500, 500})
         .disableCrosshair()
         .moveTo("ser_TX")
         ;
RightStick = controlP5.addSlider2D("Right")
         .setPosition(425,100)
         .setSize(100,100)
         .setMaxX(2000)
         .setMaxY(1000)
         .setMinX(1000)
         .setMinY(2000)
         .setLabel("")
         .setArrayValue(new float[] {500, 500})
         .disableCrosshair()
         .moveTo("ser_TX")
         ;
}

void Getstick(){
  Joy[2] = (int)LeftStick.arrayValue()[0] ;  // Yaw
  Joy[0] = (int)RightStick.arrayValue()[0]; // Roll
  if(TXmode==2) {
  Joy[3] = (int)LeftStick.arrayValue()[1] ; // Thr
  Joy[1] = (int)RightStick.arrayValue()[1]; // Nick
  }else if(TXmode==1){
  Joy[3] = (int)RightStick.arrayValue()[1]; // Thr
  Joy[1] = (int)LeftStick.arrayValue()[1] ; // Nick
}
  Joy[4] = (int)AUX_1;
  Joy[5] = (int)AUX_2;
  Joy[6] = (int)AUX_3;
  Joy[7] = (int)AUX_4;
}

void  JOYSTICK(){ // Toggle joystick Activation
btnRCJoystick.setLabel("RC Tranmitter");
ToggleJoystick=!ToggleJoystick;
if(!stickSetup)setupJoystick();
if(ToggleJoystick) {btnRCJoystick.setColorBackground(green_);
TX_Info .hide();
TX_Info1.hide();
TX_Info2.hide();
TX_Info3.hide();
}else{btnRCJoystick.setColorBackground(red_);}
}

void SendJoystick()
{
  Getstick();


  if(toggleCenter) {CenterBTN.setColorBackground(green_).setLabel("Relese");}else{CenterBTN.setColorBackground(red_).setLabel("Center");}
  if(toggleLock) {LockBTN.setColorBackground(green_).setLabel(" Throttle  Locked");}else{LockBTN.setColorBackground(red_).setLabel(" Throttle  Relesed");}
  
  if(TXmode==1)
  {//Mode 1
    if (!toggleCenter){
      leftX  = Joy[2]; // Yaw
      rightX = Joy[0]; // Roll      
      leftY  = Joy[1]; // Nick      
    }else{leftX  = 1500;rightX = 1500;leftY  = 1500;}    
    if(!toggleLock){ rightY  = Joy[3]; }else{rightY=1000;Joy[3]=rightY;}// Thr  
  }else
  {// Mode 2
    if (!toggleCenter){
      leftX  = Joy[2]; // Yaw
      rightX = Joy[0]; // Roll
      rightY = Joy[1]; // Nick
    }else{leftX  = 1500;leftY  = 1500;rightX = 1500;rightY = 1500;}
    if(!toggleLock){ leftY  = Joy[3]; }else{leftY=1000;Joy[3]=leftY;}// Thr
  }


  
LeftStick.setArrayValue( new float[] {leftX-1000, leftY-2000});//.setArrayValue(new float[] {600, -600})
RightStick.setArrayValue(new float[] {rightX-1000,rightY-2000});

      List<Character> payload;
      payload = new ArrayList<Character>();
      for (i=0;i<8;i++){payload.add(char((int)Joy[i] % 256));payload.add(char((int)Joy[i] / 256));}
      
      sendRequestMSP(requestMSP(MSP_SET_RAW_RC,payload.toArray(new Character[payload.size()])));
  }
  

