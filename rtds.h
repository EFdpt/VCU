/** 
 *  @file           rtds.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          RTDS header file
 */

#ifndef _READY_TO_DRIVE_SOUND_H_
#define _READY_TO_DRIVE_SOUND_H_

/**
 *  @page RTDS Ready To Drive Sound (RTDS)
 *
 *  The vehicle must make a characteristic sound, continuously for at least one 
 *  second and a maximum of three seconds when it enters ready-to-drive mode.
 *  According @ref FSM_page, RTDS is triggered with transition from @ref HVON state 
 *  to @ref DRIVE state.
 */

/** @defgroup RTDS_group RTDS
 *  
 *  @{
 */

/**
 *  @brief      This function starts RTDS.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 */
void ready_to_drive_sound_start();

/**
 *	@}
 */

#endif /* _READY_TO_DRIVE_SOUND_H_ */