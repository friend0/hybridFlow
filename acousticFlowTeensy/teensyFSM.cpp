/*
* @Author: Ryan A. Rodriguez
* @Date:   2016-03-07 15:48:56
* @Last Modified by:   Ryan A. Rodriguez
* @Last Modified time: 2016-03-08 02:44:45
*/

/**************************************************************************
*@File teensyFSMFSM.C                                                      *
*@author Ryan Rodriguez                                                   *
*@date 12/27/14                                                           *
*@brief This module implements the teensyFSM state machine                 *
*This module is the software implementation of the teensyFSM state machine, *
*and is intended for research purposes at this moment.                    *
**************************************************************************/

#include "teensyFSM.h"
#include <ctype.h>

/****************************************************************
Events
****************************************************************/


/****************************************************************
States
****************************************************************/
void teensyFSMCtor(teensyFSM *self)
{
    _FsmCtor_(&self->super_, &teensyFSM_initial);
}

void initial(teensyFSM *self, Event const *e)
{
    /* ... initialization of teensyFSM attributes */
    // Serial.println("teensyFSM initialized");
    _FsmTran_((Fsm *)self, &teensyFSM_default);
}

// dont' really need a default state..
void default(teensyFSM *self, Event const *e)
{

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }    
}

void default(teensyFSM *self, Event const *e){

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 

}

void sleeping(teensyFSM *self, Event const *e){


    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 

}

void polling(teensyFSM *self, Event const *e){


    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 

}

void sampling(teensyFSM *self, Event const *e){


    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 

}

void fft(teensyFSM *self, Event const *e){


    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 

}

void extracting(teensyFSM *self, Event const *e){


    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 

}

void estimating(teensyFSM *self, Event const *e){


    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 

}

void transmitting(teensyFSM *self, Event const *e){


    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 

}

void testing(teensyFSM *self, Event const *e){


    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    }

    switch (e->signal)
    {

        case INTERVAL_TIMER_EXPIRED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &polling);
            break;

        case FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sampling);
            break;

        case NO_FLOW_DETECTED:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case DONE_ESTIMATING:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;

        case NO_EVENT:
            // Serial.println("defaultNOEVENT");
            _FsmTran_((Fsm *)self, &sleeping);
            break;   

    }

    if (e->transition == true) {
        printf("EXECUTE TRANSITION!\n");
        e->transition = false;
    } 
    
}

/**
* @brief Used to determine the event that should be passed to the FsmDispatch function
*
* Using the hBridgeEvent class, we utilize the data variable 'code' to switch the signal
* of the Event super-class. Next, we tahBridgeEvent the updated Event signal and dispatch it to
* the current state function pointed to by Fsm of the class hBridge.
*
*
* @param self [description]
* @param e    [description]
*/

char teensyFSMTransitionFunction(teensyFSM *self, teensyFSMEvent *e)
{
    //After dereferencing, self is an hBridge object
    //
    //  teensyFSMEvent e is a wrapper structure containing members Char code, and Event super_.
    //  Code is just a data variable used to switch the signal attribute of the super_ Event.
    //  In our case, code should be a tuple of Il and Vc, used to switch the signal of the Event structure.
    //  The super_ Event is then passed to the dispatch function which calls the state function.
    //  The state function reads the signal and responds with the appropriate transition.

    //First, get the event pointed to by teensyFSM event
    //Next, get the signal pointed to by the event in teensyFSMEvent
    void *    funptr = (void*) self-> super_.state__;
    //void    *funptr = self->super_.state__;

    if(funptr == &teensyFSM_default){
        switch (e->code)                  //This switch uses the data attribute 'code' of the teensyFSM Event
        {
            //case '.' : return -1;          // terminate the test
            //case '+' : e->super_.signal = VDC; break;
            //case '-' : e->super_.signal = NEG_VDC; break;
            //pcase '0' : e->super_.signal = ZERO_VDC; break;

            default : e->super_.signal = NO_EVENT; break;
        }
    }

    else
        ;

    return 0;
}



/**
* End State Definitions
*/


/**
* The main function below is to be used with teensyFSMEventyboard input or Bucchi
* automaton for testing purposes.
*/

/**
int main()
{
    int returner = 0;
    //Declare the variable k to be of the type 'teensyFSM', where teensyFSM is the class
    //wrapping the FSM
    teensyFSM teensyFSM;

    //The teensyFSMEvent the class teensyFSM, get the FSM it contains, and point it to an initialization state
    teensyFSMCtor(&teensyFSM);
    FsmInit((Fsm *)&teensyFSM, 0);
    for (;;)
    {
        teensyFSMEvent teensyFSMEvent;                   //teensyFSMEvent a new event on every cycle
        //printf("\nSignal<-");             //output the signal attribute of the event object

        // teensyFSMEvent.code should be the value sampled at ADC for actual implementation
        // teensyFSMEvent.code = getc(stdin);            //obtain user input, use the data attribute 'code' to store it
        // getc(stdin);                      //discard newline '\n' //

        void    *funptr = teensyFSM.super_.state__;

        returner = teensyFSMTransitionFunction(teensyFSM, &teensyFSMEvent);
        if(returner == -1) return 0;
        FsmDispatch((Fsm *)&teensyFSM, (Event *)&teensyFSMEvent);  //dispatch

        funptr = teensyFSM.super_.state__;
    }
    return 0;
}
**/
