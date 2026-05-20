#ifndef _SELF_DEF_KEY_SCAN_H
#define _SELF_DEF_KEY_SCAN_H

#include "include.h"

/* 发送编号 */
#define KEY_SHORT               0x000    
#define KEY_SHORT_UP            0x800       
#define KEY_LONG                0xA00      
#define KEY_LONG_UP             0xC00       
#define KEY_HOLD                0xE00       

/* 长短按阈值 */
#define KEY_SCAN_TIMES          6           // 按键防抖的扫描次数：6 * 5 = 30ms
#define KEY_LONG_TIMES          200         // 长按键的次数：200 * 5 = 1000ms

void key_init(void);            // 按键初始化
u8 get_key_val(u8 num);      // 获取按键值

/* 按键检测、消抖、推送 */
void key_scan(void);

#endif
