#include "self_def_key_scan.h"

void key_init(void) {

}

u8 get_key_val(u8 num) {
    
}

AT(.com_rodata.key)
static int count = 0;

void key_scan(void) {

    /* 检测 */  
    count++;

    if (count % 2000 == 1) {
        // printf("GPIOA2 = 0x%X\n", GPIOA & BIT(2));
    }
    

    /* 消抖 */


    /* 推送 */


}