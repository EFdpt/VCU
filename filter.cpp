/** 
 *  @file           filter.cpp
 *  @author         Arella Matteo <br/>
 *                  (mail: arella.1646983@studenti.uniroma1.it)
 *  @date           2018
 *  @brief          Filter module implementation file
 */

#include "filter.h"

/**
 *  @addtogroup Filter_module_group
 *   @{
 */

/**
 *  @def 	USE_LOOP_UNROLLING
 *  @brief 	Flag macro for using or not loop unrolling into filter function
 */
#define USE_LOOP_UNROLLING		(1)

/**
 *  @def 	pos(x, offset)
 *  @brief 	Buffer indexing macro
 */
#define pos(x, offset)      ((x) * offset)

/**
 *  @brief      This function filters the input buffer with an average filter.
 *              
 *  @author     Arella Matteo <br/>
 *              (mail: arella.1646983@studenti.uniroma1.it)
 *              
 *  @param[in]  buffer  Input buffer
 *  @param[in]  size    Buffer size
 *  @param[in]  offset  Offset between data corresponding to same acquired value
 *  @return     Filtered data
 */
uint16_t filter_buffer(volatile uint16_t* buffer, int size, unsigned offset) {

	volatile int index = 0;

#if USE_LOOP_UNROLLING
	volatile long long sum1 = 0LL;
	volatile long long sum2 = 0LL;
	volatile long long sum3 = 0LL;
	volatile long long sum4 = 0LL;

	for (; index + 4 < size; index += 4) { // use loop unrolling with factor 4
		sum1 += buffer[pos(index, offset)];
		sum2 += buffer[pos(index + 1, offset)];
		sum3 += buffer[pos(index + 2, offset)];
		sum4 += buffer[pos(index + 3, offset)];
	}

	for (; index < size; index++)
		sum1 += buffer[pos(index, offset)];

	return (sum1 + sum2 + sum3 + sum4) / size;
#else
	volatile long long sum = 0LL;

	for (; index < size; index++) {
		sum += buffer[pos(index, offset)];
	}
	return sum / size;
#endif
}

/**
 *  @}
 */