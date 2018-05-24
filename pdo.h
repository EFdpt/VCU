#ifndef _PDO_H_
#define _PDO_H_

#include "def.h"
#include "CO_can.h"
#include "common.h"

void buildPDO(uint8_t PDOtype, Message* pdo);

#if defined(_RETRO_)
void proceedPDO(Message* m);
#endif

#endif /* _PDO_H_ */