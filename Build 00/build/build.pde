//**********************************************************************
// Author            : Bryan Costanza (GitHub: TravelByRocket)
// Date created      : 20190707
// Purpose           : Touch-responsive inflatable sculpture basd on slime mold
// Revision History  : 
// 20190707 -- Starting with a visual simulator to test concepts 
//**********************************************************************

String dataDir    = "../../data/";

void settings() {
  size(600,600);
}

color simGray = color(50);

void setup() {
  background(simGray);
}

void draw() {
  background(simGray);
  button1(width/4,height/4,200);
}

void button1(float xloc, float yloc, float size){
  rectMode(CENTER);
  stroke(0);
  if(mousePressed
      && mouseX > xloc - size/2 && mouseX < xloc + size/2
      && mouseY > yloc - size/2 && mouseY < yloc + size/2){
    fill(30);
  } else {
    noFill();
  }
  rect(xloc,yloc,size/2,size/2);
}

class HumanTouch {
  
}
