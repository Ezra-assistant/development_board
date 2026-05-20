#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_IDLE_EN
AT(.text.func.idle.msg)
void func_idle_message(u16 msg)
{
    switch (msg) {

        case KU_AB_PLAY:
            #if I2C_HW_EN_TEST
                my_printf("KU_AB_PLAY\n");
                i2c_hw_test();
            #endif

            #if I2C_SW_EN_TEST
                my_printf("KU_AB_PLAY\n");
                // sw_i2c_test();
                sw_i2c_single_byte_test();
            #endif
            break;

        default:
            func_message(msg);
            break;
    }
}
#endif // FUNC_IDLE_EN
