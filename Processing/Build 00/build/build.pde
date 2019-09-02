//**********************************************************************
// Author            : Bryan Costanza (GitHub: TravelByRocket)
// Date created      : 20190707
// Purpose           : Touch-responsive inflatable sculpture based on slime mold
// Revision History  : 
// 20190707 -- Starting with a visual simulator to test concepts 
// 20190708 -- create class to represent touchable points on screen
// 20190715 -- 
//**********************************************************************


////////////////////////////////////////
// VARIABLES ///////////////////////////
////////////////////////////////////////
String dataDir    = "../../data/";

void settings() {
  size(800,800);
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
  //LEFT SIDE
  touchPoints.add(new TouchPoint("ALPH",width*2/10,height*2/10));
  touchPoints.add(new TouchPoint("BRAV",width*2/10,height*3/10));
  touchPoints.add(new TouchPoint("CHAR",width*2/10,height*4/10));
  touchPoints.add(new TouchPoint("DELT",width*2/10,height*5/10));
  touchPoints.add(new TouchPoint("ECHO",width*2/10,height*6/10));
  //RIGHT SIDE
  touchPoints.add(new TouchPoint("FOXT",width*8/10,height*5/10));
  touchPoints.add(new TouchPoint("GOLF",width*8/10,height*6/10));
  touchPoints.add(new TouchPoint("HOTE",width*8/10,height*7/10));
  touchPoints.add(new TouchPoint("INDI",width*8/10,height*8/10));
  touchPoints.add(new TouchPoint("JULI",width*8/10,height*9/10));
}

////////////////////////////////////////
// DRAW ////////////////////////////////
////////////////////////////////////////

void draw() {
  background(simGray);
  for (TouchPoint tp : touchPoints){
    tp.draw();
  }
  
  section64(width/2,height/2,20);

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

////////////////////////////////////////
// SECTION64 FUNCTION //////////////////
////////////////////////////////////////

void section64(float startX, float startY, float theta){
  // theta starts at 3 o'clock and proceeds CW
  for (int i = 0; i < 64; i++){
    if(i == 0) {ellipse(startX,startY,10,10);}
    point(startX + 2 * i * cos(radians(theta)),startY + 2 * i * sin(radians(theta)));
  }
  // draw arrow at start
  line(startX, startY, startX + 10 * cos(radians(theta+180+30)), startY + 10 * sin(radians(theta+180+30)));
  line(startX, startY, startX + 10 * cos(radians(theta+180-30)), startY + 10 * sin(radians(theta+180-30)));
  // draw arrow at end
  line(startX + 128 * cos(radians(theta)), startY + 128 * sin(radians(theta)), 
       startX + 128 * cos(radians(theta)) + 10 * cos(radians(theta+180+30)), startY  + 128 * cos(radians(theta)) + 10 * sin(radians(theta+180+30)));
  line(startX + 128 * cos(radians(theta)), startY + 128 * sin(radians(theta)), 
       startX + 128 * cos(radians(theta)) + 10 * cos(radians(theta+180-30)), startY + 128 * sin(radians(theta)) + 10 * sin(radians(theta+180-30)));
}
