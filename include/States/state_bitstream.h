

#ifndef STATE_BITSTREAM_H
#define STATE_BITSTREAM_H
#include <StateMachine.h>
#include "Images/bitStream_img.h"

void drawCube();
void drawGIF();
class state_bitstream : public State {
  public:
  virtual void init() override;
  virtual void update() override;
  virtual void draw() override;
  void drawCube();
};
unsigned long fd;
void state_bitstream::init() { display.setFont(&FreeSans9pt7b); }

void state_bitstream::update() {}

void state_bitstream::draw() {
  display.playAnimation(0, 0, bitStream_data, bitStream_header);
  display.drawLine(48, 22, 123, 22, ON);
  display.setCursor(48, 20);
  display.print("bitStream");
  drawCube();

  display.swapBuffer();
  display.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cube Functions
float *rotate(float &x, float &y, float angle) {
  float s = sin(angle);
  float c = cos(angle);
  float *result = new float[2];
  result[0] = x * c - y * s;
  result[1] = y * c + x * s;
  return result;
}

float rotationXspeed = 0.008;
float rotationYspeed = 0.006;
float rotationZspeed = 0.001;

float cubeX = 0;
float cubeY = 0;
float cubeZ = -5;

int cubeOffset_x = DISPLAY_WIDTH / 2 + 20;
int cubeOffset_y = DISPLAY_HEIGHT / 2 + 12;
float cubeSize = 50;
float cameraDistance = 1.0;
float rotationX = 0;
float rotationY = 0;
float rotationZ = 0;

float vertices[][3] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                       {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};

int indices[][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                    {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

void state_bitstream::drawCube() {

  for (int i = 0; i < 12; i++) {
    int *index = indices[i];
    int a = index[0];
    int b = index[1];

    float *vertex1 = vertices[a];
    float *vertex2 = vertices[b];

    float x1, y1, z1, x2, y2, z2;
    x1 = vertex1[0];
    y1 = vertex1[1];
    z1 = vertex1[2];
    float *temp_Rotation = rotate(x1, z1, rotationY);
    x1 = temp_Rotation[0];
    z1 = temp_Rotation[1];
    delete temp_Rotation;

    temp_Rotation = rotate(y1, z1, rotationX);
    y1 = temp_Rotation[0];
    z1 = temp_Rotation[1];
    delete temp_Rotation;

    x1 = (float)(x1 * cubeSize);
    y1 = (float)(y1 * cubeSize);

    x2 = (float)vertex2[0];
    y2 = (float)vertex2[1];
    z2 = (float)vertex2[2];

    temp_Rotation = rotate(x2, z2, rotationY);
    x2 = temp_Rotation[0];
    z2 = temp_Rotation[1];
    delete temp_Rotation;

    temp_Rotation = rotate(y2, z2, rotationX);
    y2 = temp_Rotation[0];
    z2 = temp_Rotation[1];
    delete temp_Rotation;

    x2 = (float)(x2 * cubeSize);
    y2 = (float)(y2 * cubeSize);

    x1 += cubeX;
    x2 += cubeX;
    y1 += cubeY;
    y2 += cubeY;
    z1 += cubeZ;
    z2 += cubeZ;

    x1 /= z1 * cameraDistance;
    x2 /= z2 * cameraDistance;
    y1 /= z1 * cameraDistance;
    y2 /= z2 * cameraDistance;

    x1 += cubeOffset_x;
    x2 += cubeOffset_x;
    y1 += cubeOffset_y;
    y2 += cubeOffset_y;

    display.drawLine(x1, y1, x2, y2, ON);
  }

  rotationX += rotationXspeed + random(-0.001, 0.001);
  rotationY += rotationYspeed + random(-0.001, 0.001);
  rotationZ += rotationZspeed + random(-0.001, 0.001);
}

#endif