#include "filter.h"
#include "sort.h"

#define FILTER_BOUND			(8)

#define USE_LOOP_UNROLLING		(1)

#define pos(x, offset)      ((x) * offset)

/**
 *  @author		Arella Matteo
 *  @brief		This function filters the input buffer with an average filter.
 *  @param		buffer [in] The buffer to filter
 *  @param		size [in] The size of the buffer
 *  @retval		Filtered value
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
