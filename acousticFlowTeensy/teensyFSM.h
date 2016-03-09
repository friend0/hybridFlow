#ifndef TEENSY_FSM
#define TEENSY_FSM


//////////////////////////////////////////
// Declaration of FSM and Event objects //
//////////////////////////////////////////
#include "fsm.h"


/* signals used by the teensyFSM FSM */
enum
{
    INTERVAL_TIMER_EXPIRED,
    FLOW_DETECTED, 
    NO_FLOW_DETECTED,
    DONE_ESTIMATING,
    NO_EVENT,   //qDot = 0
};

typedef struct teensyFSM teensyFSM;
typedef struct teensyFSMEvent teensyFSMEvent;

/**************************************************************************
teensyFSM FSM - Public Type Definitions for teensyFSM FSM and teensyFSM Events
***************************************************************************/
/**
* @brief 'type naming' of the FSM object
*
* The 'teensyFSM' struct is a container for the FSM base class.
* Other attributes of the class are included. Class methods are
* implemented following this.
*/

struct teensyFSM
{
    Fsm super_; /* extend the Event class */
};

struct teensyFSMEvent
{
    Event super_; /* extend the Event class */
    //Attributes
    char code;
};

////////////////////////////////////
// Declaration of state functions //
////////////////////////////////////

/**
* @brief Constructor Function
* This function is to be used for 'instantiating' state machines
* @code(.c)
*     teensyFSM teensyFSM;
*     teensyFSMCtor(&teensyFSM);
* @endcode
* teensyFSMCtor is a wrapper function for a call to:
* @code
*     _FsmCtor_(&self->super_, &teensyFSM_initial);
* @endcode
* which uses the 'super class'
*
* @param self
*/
void teensyFSMCtor(Fsm *self);

/**
* @brief Entry state to be used for initialization and setup of the state machine.
*
* Implements the initial transition of the teensyFSM FSM. To be used for initializations
* and setup of the machine. Can also serve no function but to transition to the default or
* zero states.
*
* @param self self reference to teensyFSM
* @param e    event
*/
void initial(Fsm *self, Event *e);

/**
* Implements the default transition
* @param self self reference to teensyFSM
* @param e    event
*/
void defaultState(Fsm *self, Event *e);

void sleeping(Fsm *self, Event *e);

void polling(Fsm *self, Event *e);

void sampling(Fsm *self, Event *e);

void fft(Fsm *self, Event *e);

void extracting(Fsm *self, Event *e);

void estimating(Fsm *self, Event *e);

void transmitting(Fsm *self, Event *e);

void testing(Fsm *self, Event *e);

char updateFSM(Fsm *self, teensyFSMEvent *e);

#endif
