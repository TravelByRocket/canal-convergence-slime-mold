//**********************************************************************
// Author            : Bryan Costanza (GitHub: TravelByRocket)
// Date created      : 20190707
// Purpose           : Touch-responsive inflatable sculpture basd on slime mold
// Revision History  : 
// 20190707 -- Starting with a visual simulator to test concepts 
// 20190708 -- create class to represent touchable points on screen
//**********************************************************************


////////////////////////////////////////
// VARIABLES ///////////////////////////
////////////////////////////////////////
String dataDir    = "../../data/";

void settings() {
  size(600,600);
}

color simGray = color(150);
color activeGray = color(200);

ArrayList<TouchPoint> touchPoints = new ArrayList<TouchPoint>();

////////////////////////////////////////
// SETUP ///////////////////////////////
////////////////////////////////////////

void setup() {
  fontCreateAndSelect();
  background(simGray);
  touchPoints.add(new TouchPoint("ALPH",width*2/10,height*3/10));
  touchPoints.add(new TouchPoint("BRAV",width*3/10,height*7/10));
  touchPoints.add(new TouchPoint("CHAR",width*5/10,height*4/10));
  touchPoints.add(new TouchPoint("DELT",width*8/10,height*2/10));
  touchPoints.add(new TouchPoint("ECHO",width*7/10,height*8/10));
}

////////////////////////////////////////
// DRAW ////////////////////////////////
////////////////////////////////////////

void draw() {
  background(simGray);
  for (TouchPoint tp : touchPoints){
    tp.draw();
  }
}

void fontCreateAndSelect(){
  textFont(createFont(dataDir+"Archivo-Regular.ttf", 20));
}

////////////////////////////////////////
// TOUCHPOINT CLASS ////////////////////
////////////////////////////////////////

class TouchPoint {
  String name;
  float posx;
  float posy;
  float size = 80;
  boolean isPressed = false;
  
  TouchPoint(String name_, float posx_, float posy_){
    name = name_;
    posx = posx_;
    posy = posy_;
  }
  
  void draw(){
    checkPressed();
    drawButton();
    drawName();
  }
  
  void checkPressed(){
    if(mousePressed
        && mouseX > posx - size/2 && mouseX < posx + size/2
        && mouseY > posy - size/2 && mouseY < posy + size/2){
      isPressed = true;
    } else {
      isPressed = false;
    }
  }
  
  void drawButton(){
    rectMode(CENTER);
    stroke(0);
    if(isPressed){
      fill(activeGray);    
    } else {
     noFill();
    }
    rect(posx,posy,size,size);
  }
  
  void drawName(){
    fill(0);
    textAlign(CENTER,CENTER);
    text(name, posx, posy);
  }
  
}
