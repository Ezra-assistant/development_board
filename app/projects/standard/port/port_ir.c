#include "include.h"

#if IRRX_SW_EN

#if IR_ADDR_FF00_EN
AT(.com_rodata.ir.table)
const u8 ir_tbl_FF00[96] =
{
// 	   0              1 		     2	            3             4               5             6                 7
/*0*/  NO_KEY,        NO_KEY,        NO_KEY,        NO_KEY,       NO_KEY,         NO_KEY,       NO_KEY,           KEY_VOL_DOWN,  //7
       KEY_NUM_4,     KEY_EQ,        NO_KEY,	    NO_KEY,		  KEY_NUM_1,      KEY_REPEAT,	NO_KEY,			  NO_KEY,        //15
/*1*/  NO_KEY,        NO_KEY,        NO_KEY,        NO_KEY,       NO_KEY,         KEY_VOL_UP,   KEY_NUM_0,	      NO_KEY,        //23
       KEY_NUM_2,     NO_KEY,        NO_KEY,        NO_KEY,	      KEY_NUM_5,      NO_KEY,	    NO_KEY,			  NO_KEY,        //31
/*2*/  NO_KEY,        NO_KEY,        NO_KEY,        NO_KEY,       NO_KEY,         NO_KEY,	    NO_KEY,	     	  NO_KEY,        //39
       NO_KEY,	      NO_KEY,	     NO_KEY,	    NO_KEY,	      NO_KEY,	      NO_KEY,	    NO_KEY,	     	  NO_KEY,        //47
/*3*/  NO_KEY,        NO_KEY,        NO_KEY,        NO_KEY,       NO_KEY,         NO_KEY,	    NO_KEY,			  NO_KEY,        //55
       NO_KEY,		  NO_KEY,		 NO_KEY,	    NO_KEY,		  NO_KEY,		  NO_KEY,	    NO_KEY,			  NO_KEY,        //63
/*4*/  KEY_NEXT,      NO_KEY,        KEY_NUM_7,     KEY_PLAY,     KEY_PREV,       KEY_IR_POWER, KEY_MODE,         KEY_MUTE,      //71
       NO_KEY,        NO_KEY,      	 KEY_NUM_9,     NO_KEY,       NO_KEY,      	  NO_KEY,       NO_KEY,      	  NO_KEY,        //79
/*5*/  NO_KEY,        NO_KEY,        KEY_NUM_8,     NO_KEY,       NO_KEY,         NO_KEY,	    NO_KEY,			  NO_KEY,        //87
       NO_KEY,		  NO_KEY,		 KEY_NUM_6,	    NO_KEY,		  NO_KEY,		  NO_KEY,	    KEY_NUM_3,	      NO_KEY,        //95
};
#endif // IR_ADDR_FF00_EN

#if IR_ADDR_BF00_EN
AT(.com_rodata.ir.table)
const u8 ir_tbl_BF00[32] =
{
   //  0              1              2              3             4               5             6                 7
/*0*/  KEY_IR_POWER,KEY_PLAY,       KEY_MODE,      NO_KEY,       KEY_NUM_1,      KEY_NUM_2,    KEY_NUM_3,        NO_KEY,
       KEY_NUM_4,   KEY_NUM_5,      KEY_NUM_6,     NO_KEY,       KEY_NUM_7,      KEY_NUM_8,    KEY_NUM_9,        NO_KEY,
/*1*/  KEY_NUM_0,   NO_KEY,         KEY_VOL_UP,    NO_KEY,       KEY_PREV,       KEY_NEXT,     KEY_VOL_DOWN,     NO_KEY,
       NO_KEY,      NO_KEY,         NO_KEY,        NO_KEY,       NO_KEY,         NO_KEY,       NO_KEY,           NO_KEY,
};
#endif // IR_ADDR_BF00_EN

#if IR_ADDR_FD02_EN
AT(.com_rodata.ir.table)
const u8 ir_tbl_FD02[32] =
{
   //  0              1              2              3             4               5               6               7
/*0*/  KEY_IR_POWER,  KEY_MODE,      KEY_MUTE,    NO_KEY,       KEY_PLAY,       KEY_PREV,       KEY_NEXT,       NO_KEY,
       KEY_EQ,        KEY_VOL_DOWN,  KEY_VOL_UP,  NO_KEY,       KEY_NUM_0,      KEY_REPEAT,     KEY_USB_SD,     NO_KEY,
/*1*/  KEY_NUM_1,     KEY_NUM_2,     KEY_NUM_3,   NO_KEY,       KEY_NUM_4,      KEY_NUM_5,      KEY_NUM_6,      NO_KEY,
       KEY_NUM_7,     KEY_NUM_8,     KEY_NUM_9,   NO_KEY,       NO_KEY,         NO_KEY,         NO_KEY,         NO_KEY,
};
#endif // IR_ADDR_FD02_EN

#if IR_ADDR_FE01_EN
AT(.com_rodata.ir.table)
const u8 ir_tbl_FE01[32] =
{
   // 0               1              2               3             4              5               6               7
/*0*/ NO_KEY,         KEY_EQ,        KEY_VOL_DOWN,   KEY_VOL_UP,   KEY_NUM_0,     KEY_PREV,       KEY_NEXT,       KEY_NUM_1,
      KEY_NUM_2,      KEY_NUM_3,     KEY_NUM_4,      NO_KEY,       KEY_NUM_7,     KEY_NUM_8,      KEY_NUM_9,      KEY_IR_POWER,
/*1*/ NO_KEY,         NO_KEY,        KEY_PLAY,       NO_KEY,       NO_KEY,        NO_KEY,         NO_KEY,         NO_KEY,
      NO_KEY,         KEY_MODE,      KEY_CH_DOWN,    KEY_NUM_5,    NO_KEY,        NO_KEY,         KEY_CH_UP,      KEY_NUM_6,
};
#endif // IR_ADDR_FE01_EN

#if 0
AT(.com_rodata.ir.table)
const u8 ir_tbl_FF00[96] =
{
// 	   0              1 		     2	            3             4               5             6                 7
/*0*/  NO_KEY,        NO_KEY,        NO_KEY,        NO_KEY,       NO_KEY,         NO_KEY,       NO_KEY,           KEY_VOL_DOWN,  //7
       KEY_NUM_4,     KEY_EQ,        NO_KEY,	    NO_KEY,		  KEY_NUM_1,      KEY_REPEAT,	NO_KEY,			  NO_KEY,        //15
/*1*/  NO_KEY,        KEY_IR_POWER,  NO_KEY,        NO_KEY,       NO_KEY,         KEY_VOL_UP,   KEY_NUM_0,	      KEY_VOL_DOWN,  //23
       KEY_NUM_2,     NO_KEY,        NO_KEY,        NO_KEY,	      KEY_NUM_5,      NO_KEY,	    NO_KEY,			  NO_KEY,        //31
/*2*/  NO_KEY,        NO_KEY,        NO_KEY,        NO_KEY,       NO_KEY,         NO_KEY,	    NO_KEY,	     	  NO_KEY,        //39
       NO_KEY,	      NO_KEY,	     NO_KEY,	    NO_KEY,	      NO_KEY,	      NO_KEY,	    NO_KEY,	     	  NO_KEY,        //47
/*3*/  NO_KEY,        NO_KEY,        NO_KEY,        NO_KEY,       NO_KEY,         NO_KEY,	    NO_KEY,			  NO_KEY,        //55
       NO_KEY,		  NO_KEY,		 NO_KEY,	    NO_KEY,		  NO_KEY,		  NO_KEY,	    NO_KEY,			  NO_KEY,        //63
/*4*/  KEY_NEXT,      NO_KEY,        KEY_NUM_7,     KEY_PLAY,     KEY_PREV,       KEY_IR_POWER, KEY_MODE,         KEY_MUTE,      //71
       NO_KEY,        NO_KEY,      	 KEY_NUM_9,     NO_KEY,       NO_KEY,      	  NO_KEY,       NO_KEY,      	  NO_KEY,        //79
/*5*/  NO_KEY,        NO_KEY,        KEY_NUM_8,     NO_KEY,       NO_KEY,         NO_KEY,	    NO_KEY,			  NO_KEY,        //87
       NO_KEY,		  NO_KEY,		 KEY_NUM_6,	    NO_KEY,		  NO_KEY,		  NO_KEY,	    KEY_NUM_3,	      NO_KEY,        //95
};
#endif

#endif // IRRX_SW_EN
