/** 
 *  @file           states.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          FSM header file
 */

#ifndef _STATES_H_
#define _STATES_H_

#include "CO_can.h"

/**
 *  @page FSM_page Finite State Machine (FSM)
 *  
 *  VCU firmware is based upon 4 states:
 *      - @anchor STAND "STAND":  ignition of the car, you return here every time SC is undervoltage
 *      - @anchor HVON "HV ON": active high voltage is only accessed from STAND via AIRbutton
 *                  and SC voltage > 3V
 *      - @anchor DRIVE "DRIVE":  the safe driving status, accessible by RTD procedure but also 
 *                  with pedals implausibility through the return procedure
 *      - @anchor NOTDRIVE "NOT DRIVE": error with pedals, the sensors of the pedals are disconnected or out
 *                  range. Torque request and braking request are disabled; you enter only
 *                  by pedals implausibility or absence of the brake.
 *
 *  @image html fsm_diagram.svg
 *  @image latex fsm_diagram.eps "FSM diagram" width=10cm
 *
 */

/**
 *  @defgroup stages_group Finite State Machine (FSM)
 *   @{
 */

/// FSM states enum
typedef enum enum_nodeState {
    STAND = 0,  /*!< STAND state */
    HVON,       /*!< HV ON state */
    DRIVE,      /*!< DRIVE state */
    NOTDRIVE,   /*!< NOT DRIVE state */
    MAX_STATES  /*!< max number of states */
} e_nodeState;

/**
 *  @brief      Return current state on the @ref FSM_page
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *
 *  @return     current state on the FSM
 */
e_nodeState getState();

/**
 *  @brief      Set current state on the @ref FSM_page
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *
 *  @param[in]  newState    New state transition
 */
void setState(e_nodeState newState);

/**
 *  @brief      Execute current state task
 *  
 *  @author     Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 */
void state_dispatch();

/**
 *  @}
 */

#endif /* _STATES_H_ */