
#ifndef STATE_LOADING_H
#define STATE_LOADING_H
#include <StateMachine.h>

class state_loading : public State {
  public:
  virtual void init() override;
  virtual void update() override;
  virtual void draw() override;
};
int load_x = 0;

void state_loading::init() {
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(INVERSE);
}

void state_loading::update() {
  if (load_x >= 104) {
    load_x = 0;
    nextState(1); // nextState(1);
  }
  load_x += 1;
}

void state_loading::draw() {
  display.fillRoundRect(12, 22, load_x, 23, 2, ON);
  display.drawRoundRect(10, 20, 108, 27, 3, ON);

  display.setCursor(15, 40);
  display.print("LOADING");

  display.swapBuffer();
  display.clear();
  delay(20);
}
#endif