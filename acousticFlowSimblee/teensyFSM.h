#ifndef TEENSY_FSM
#define TEENSY_FSM

//////////////////////////////////////////
// Declaration of FSM and Event objects //
//////////////////////////////////////////

typedef struct teensyFSM teensyFSM;
typedef struct teensyFSMEvent teensyFSMEvent;

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
void teensyFSM_initial(teensyFSM *self, Event *e);

/**
* Implements the default transition
* @param self self reference to teensyFSM
* @param e    event
*/
void teensyFSM_default(teensyFSM *self, Event *e);

/**
* Implements the state handler for the case that the H-Bridge
* is supplying +Vdc to the input of the RLC filter.
* @param self self reference to teensyFSM
* @param e    event
*/
void teensyFSM_VDC(teensyFSM *self, Event *e);

/**
* Implements the state handler for the case that the H-Bridge
* is supplying zero volts DC to the input of the RLC filter.
* @param self self reference to teensyFSM
* @param e    event
*/
void teensyFSM_Zero(teensyFSM *self, Event *e);

/**
* Implements the state handler for the case that the H-Bridge
* is supplying -Vdc to the input of the RLC filter.
* @param self self reference to teensyFSM
* @param e    event
*/
void teensyFSM_negVDC(teensyFSM *self, Event *e);


char teensyFSMTransitionFunction(teensyFSM self, teensyFSMEvent *e);

#endif