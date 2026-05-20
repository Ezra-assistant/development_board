#include "include.h"

#define KEY_GPIO_NUM 0

//正常启动Main函数
int main(void)
{
    u32 rst_reason;
    rst_reason = sys_rst_init();                            // 检查复位源（上电复位/看门狗复位/低功耗唤醒等），返回32为复位原因码
    printf("Hello AB15x standard: %08x\n", rst_reason);
    sys_rst_dump(rst_reason);                               // 系统复位原因诊断函数，用于解析并输出触发系统复位的具体原因
    sys_cb.wakeup_reason = lowpwr_get_wakeup_source();

    sys_ram_info_dump();                                    // 打印系统RAM的使用情况信息

    bsp_sys_init();                                         // 通常配置时钟、中断和 GPIO 等底层驱动初始化

    printf("This is a testing code!\n");
    
    // func_run();                                             // 核心应用入口
    user_main();

    return 0;
}

//升级完成
void update_complete(int mode)
{
    bsp_update_init();
    printf("update complete: %d\n", mode);
    if (mode == 0) {
        WDT_DIS();
        while (1);
    }
    WDT_RST();
}
