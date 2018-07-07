/** 
 *  @file           filter.h
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          Filter module header file
 */

#ifndef _FILTER_H_
#define _FILTER_H_

#include <stdint.h>
#include "model.h"

/**
 *  @defgroup Filter_module_group Filter buffer
 *   @{
 */

uint16_t filter_buffer(volatile uint16_t* buffer, int size, unsigned offset);

/**
 *  @}
 */

#endif /* _FILTER_H_ */
