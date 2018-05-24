#ifndef _DEF_H_
#define _DEF_H_

/*  Function Codes 
 ---------------
 defined in the canopen DS301 
 */
#define NMT	   0x0
#define SYNC       0x1
#define TIME_STAMP 0x2
#define PDO1tx     0x3
#define PDO1rx     0x4
#define PDO2tx     0x5
#define PDO2rx     0x6
#define PDO3tx     0x7
#define PDO3rx     0x8
#define PDO4tx     0x9
#define PDO4rx     0xA
#define SDOtx      0xB
#define SDOrx      0xC
#define NODE_GUARD 0xE
#define LSS 	   0xF

#define GET_FUNC_CODE(COB_ID)	(COB_ID >> 7)
#define SET_FUNC_CODE(COB_ID)   (COB_ID << 7)

/* NMT Command Specifier, sent by master to change a slave state */
/* ------------------------------------------------------------- */
/* Should not be modified */
#define NMT_Start_Node              0x01 // go Operational
#define NMT_Stop_Node               0x02
#define NMT_Enter_PreOperational    0x80
#define NMT_Reset_Node              0x81
#define NMT_Reset_Comunication      0x82

#endif /* _DEF_H_ */