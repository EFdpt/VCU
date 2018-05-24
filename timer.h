#ifndef _TIMER_H_
#define _TIMER_H_

void timerInit();

void timerStart();

void timerStop();

// send PDOs periodically
void TimeDispatch();

#endif /* _TIMER_H_ */