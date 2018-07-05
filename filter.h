#ifndef _FILTER_H_
#define _FILTER_H_

#include <stdint.h>
#include "model.h"

uint16_t filter_buffer(volatile uint16_t* buffer, int size, unsigned offset);

#endif /* _FILTER_H_ */
