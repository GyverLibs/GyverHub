void setup() {
  int w = 400, h = 500;

  size(400, 500);

  background(200, 200, 200);
  stroke(0);
  strokeWeight(1);
  for (int x = 0; x <= w; x += 25) {
    line(x, 0, x, h);
  }
  strokeWeight(2);
  for (int x = 0; x <= w; x += 50) {
    line(x, 0, x, h);
  }
  stroke(255, 255, 255);
  strokeWeight(1);
  for (int y = 0; y <= h; y += 25) {
    line(0, y, w, y);
  }
  strokeWeight(2);
  for (int y = 0; y <= h; y += 50) {
    line(0, y, w, y);
  }

  stroke(255, 0, 0);
  point(75, 25);

  noStroke();
  fill(255, 0, 0);

  rectMode(CORNER);
  rect(0, 0, 50, 50);

  rectMode(CORNERS);
  rect(100, 0, 150, 50, 10);

  rectMode(CENTER);
  rect(225, 25, 50, 50, 10, 10, 20, 20);

  rectMode(RADIUS);
  rect(325, 25, 25, 25);

  ellipseMode(CORNER);
  ellipse(0+50, 0+50, 50, 50);

  ellipseMode(CORNERS);
  ellipse(100+50, 0+50, 150+50, 50+50);

  ellipseMode(CENTER);
  ellipse(225+50, 25+50, 50, 50);

  ellipseMode(RADIUS);
  ellipse(325+50, 25+50, 25, 25);

  stroke(0, 255, 0);
  strokeWeight(20);
  strokeCap(SQUARE);
  line(50, 100+25, 100, 100+25);

  strokeCap(PROJECT);
  line(50+100, 100+25, 100+100, 100+25);

  strokeCap(ROUND);
  line(50+100+100, 100+25, 100+100+100, 100+25);

  fill(0);
  noStroke();
  rectMode(CENTER);
  push();
  translate(350, 150+25);
  rotate(PI/4);
  square(0, 0, 50);
  pop();

  stroke(0, 0, 0, 100);
  fill(255, 255, 0);
  strokeWeight(15);
  strokeJoin(MITER);
  rectMode(CORNER);
  rect(50, 150, 50, 50);

  strokeJoin(BEVEL);
  rect(50+100, 150, 50, 50);

  strokeJoin(ROUND);
  rect(50+100+100, 150, 50, 50);

  textSize(25);
  fill(150, 0, 150);
  noStroke();
  textAlign(LEFT, BOTTOM);
  text("aqdAQD", 0, 250);

  textAlign(CENTER, BOTTOM);
  text("aqdAQD", 0+150, 250);

  textAlign(RIGHT, BOTTOM);
  text("aqdAQD", 0+100+100+100, 250);

  //
  textAlign(LEFT, TOP);
  text("aqdAQD", 0, 250);

  textAlign(CENTER, TOP);
  text("aqdAQD", 0+150, 250);

  textAlign(RIGHT, TOP);
  text("aqdAQD", 0+100+100+100, 250);

  //
  textAlign(LEFT, CENTER);
  text("aqdAQD", 0, 250+50);

  textAlign(CENTER, CENTER);
  text("aqdAQD", 0+150, 250+50);

  textAlign(RIGHT, CENTER);
  text("aqdAQD", 0+100+100+100, 250+50);

  //
  textAlign(LEFT, BASELINE);
  text("aqdAQD", 0, 250+50+50);

  textAlign(CENTER, BASELINE);
  text("aqdAQD", 0+150, 250+50+50);

  textAlign(RIGHT, BASELINE);
  text("aqdAQD", 0+100+100+100, 250+50+50);

  //
  noFill();
  stroke(0);
  strokeWeight(5);
  beginShape();
  vertex(325, 225);
  vertex(375, 225);
  vertex(350, 225+50);
  endShape();

  beginShape();
  vertex(325, 225+50);
  vertex(375, 225+50);
  vertex(350, 225+50+50);
  endShape(CLOSE);

  beginShape();
  fill(255, 0, 0);
  vertex(325, 225+50+50);
  vertex(375, 225+50+50);
  vertex(350, 225+50+50+50);
  endShape(CLOSE);

  noFill();
  bezier(50, 400, 150, 350, 250, 450, 350, 400);

  beginShape();
  vertex(50, 450);
  vertex(150, 450);
  bezierVertex(200, 350, 200, 550, 250, 450);
  endShape();

  arc(300, 400, 75, 50, 0, HALF_PI);
}
void draw() {
  println(mouseX, mouseY);
}
