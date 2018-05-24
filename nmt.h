#ifndef _NMT_H_
#define _NMT_H_

#include "CO_can.h"

void proceedNMTstateChange(Message* m);

void slaveSendBootUp();

#endif /* _NMT_H_ */