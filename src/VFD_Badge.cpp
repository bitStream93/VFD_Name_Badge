

#include "vfd.h"
#include <Arduino.h>
#include <SPI.h>
#include <StateMachine.h>
VFD display(10, 12, 14);

// Fonts
#include "Fonts/FreeMonoBold12pt7b.h"
#include "Fonts/FreeSans9pt7b.h"

// States
#include "States/state_bitstream.h"
#include "States/state_loading.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
// STATE MACHINE AND STATE DECLARATION
StateMachine fsm;
state_bitstream s_bitstream;
state_loading s_loading;

///////////////////////////////////////////////////////////////////////////////////////////////////
// SETUP
void setup() {
  display.begin();     // setup the display
  display.setLevel(16); // set the display brightness level [0 - 16]

  display.clear(); // clear the controller side buffer
  display
      .swapBuffer(); // swap the controller side buffer with the display buffer

  // add the states onto the stack
  fsm.add(&s_loading);   // State 0
  fsm.add(&s_bitstream); // State 1

  fsm.initialState(0); // set the initial state (0)
  fsm.begin();         // start the state machine
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
void loop() {
  fsm.run(); // run the state machine
}

///////////////////////////////////////////////////////////////////////////////////////////////////
