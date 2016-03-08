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
void teensyFSMCtor(teensyFSM *self);

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
void teensyFSM_initial(teensyFSM *self, Event const *e);

/**
* Implements the default transition
* @param self self reference to teensyFSM
* @param e    event
*/
void default(teensyFSM *self, Event const *e);

void sleeping(teensyFSM *self, Event const *e);

void polling(teensyFSM *self, Event const *e);

void sampling(teensyFSM *self, Event const *e);

void fft(teensyFSM *self, Event const *e);

void extracting(teensyFSM *self, Event const *e);

void estimating(teensyFSM *self, Event const *e);

void transmitting(teensyFSM *self, Event const *e);

void testing(teensyFSM *self, Event const *e);

char transitionFunction(teensyFSM *self, teensyFSMEvent *e);

#endif
