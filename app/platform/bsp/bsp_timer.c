#include "include.h"

#define USR_TMR50US_ISR 1
#define IRRX_SIGNAL_SCAN 1



#define COMMON_IRRX_SIGNAL_SCAN 0







#if TMR2_US_EN

#define US_1S_TEST      0   //测试10us定时,累计到达1s的时间,通过download检查打印时间
#define US_IO_TEST      0   //PA9翻转IO测试us定时

#if US_1S_TEST
u32 us10_tick;
AT(.com_rodata.isr)
const char str_t3[] = "10us_tick: %d\n";
#endif//test






/* 自定义内容 */
u8 tmr10us_cnt;


/* ===== 红外遥控器 信号捕获 ===== */
#if IRRX_SIGNAL_SCAN
/* 红外遥控器信号捕获 变量 */
// 定义解码状态
#define IR_STAGE_IDLE       0
#define IR_STAGE_LEAD_LOW   1
#define IR_STAGE_LEAD_HIGH  2
#define IR_STAGE_DATA       3
#define IR_STAGE_REPEAT     4

// 定义发送代码
/* 代码说明：
    1、捕获完 9ms的引导码低电平         后会发送：0x0D00
    2、捕获完 整段红外信号              后会发送：0x0D11
    3、捕获完 2.25ms的重复码高电平      后会发送：0x0D33
    4、捕获完 0.56ms的重复码低电平      后会发送：0x0D44
*/
#define IR_MSG_START_LOW      0x0D00
#define IR_MSG_DATA_COMPLETE  0x0D11
#define IR_MSG_REPEAT_HIGH    0x0D33
#define IR_MSG_REPEAT_LOW     0x0D44

// 定义完整捕获的静态变量
static u8 ir_stage = IR_STAGE_IDLE;
static u8 io_irrx_last_sta = 1;       // 默认状态就是高电平
static u32 pulse_duration_us = 0;     // 统一用一个变量记录当前电平持续时间
static u8 offset_point = 0;

// 定义残缺捕获专用的静态变量（防止多轮中断调用时丢失状态）
static u8 special_bit_cnt = 0; // 残缺模式专用的计数器
static u8 special_ir_stage = IR_STAGE_LEAD_HIGH;


/* 红外遥控器信号捕获 函数 */
// 一、正常完整捕获红外信号
AT(.com_text.irrx)
void irrx_signal_capture(void) {
    // 1. 获取归一化的当前电平 (0 或 1)，避开 BIT(1) 造成的非 1 隐患
    u8 io_irrx_cur_sta = (GPIOA & BIT(1)) ? 1 : 0;

    // 2. 累加当前电平的持续时间（增加防溢出保护，防止超长空闲时 u16/u32 翻转）
    if (pulse_duration_us < 200000) {
        pulse_duration_us += 50;
    }

    // 3. 检测到电平跳变（边缘触发）
    if (io_irrx_cur_sta != io_irrx_last_sta) {
        u8 finished_edge = io_irrx_last_sta;  // 刚刚结束的是什么电平
        u32 duration = pulse_duration_us;     // 拿到刚刚结束的电平宽度

        pulse_duration_us = 0;                // 核心修改：立刻清零，保证新电平计时准确
        io_irrx_last_sta = io_irrx_cur_sta;   // 更新状态

        // ===== 状态机核心逻辑 =====

        // 任何时候收到超长电平（如超过 15ms），强制复位状态机，防止死锁
        if (duration > 15000) {
            ir_stage = IR_STAGE_IDLE;
            offset_point = 0;
            return;
        }

        // 状态一：空闲状态
        if (ir_stage == IR_STAGE_IDLE) {
            // 红外接收头（低电平有效）收到突发的低电平，且时间符合 9ms 引导码
            if (finished_edge == 0 && duration >= 7500 && duration <= 10500) {
                ir_stage = IR_STAGE_LEAD_LOW;
                msg_enqueue(IR_MSG_START_LOW);
            }
        }
        // 状态二：已接收到 9ms 引导低电平
        else if (ir_stage == IR_STAGE_LEAD_LOW) {
            // 状态三：等待 4.5ms 引导高电平（说明是第一次按下发送）
            if (finished_edge == 1 && duration >= 3800 && duration <= 5200) {
                ir_stage = IR_STAGE_DATA;
                check_irrx_register = 0; // 准备开始装载，清零寄存器
                offset_point = 0;        // 偏移指针清零
            }
            // 状态四：等待 2.25ms 重复码高电平（说明按下不松手重复发送）
            else if (finished_edge == 1 && duration >= 1800 && duration <= 2800) {
                ir_stage = IR_STAGE_REPEAT;

                // 发送按住没放手的 msg_code
                msg_enqueue(IR_MSG_REPEAT_HIGH);
            }
            else {
                ir_stage = IR_STAGE_IDLE; // 宽度不对，判定为干扰，退回空闲
            }
        }
        // 状态五：正在接收数据位
        else if (ir_stage == IR_STAGE_DATA) {
            // 根据【高电平】的宽度来判定 0 和 1
            if (finished_edge == 1) {
                // 判定是否为数据 1 (1.69ms)
                if (duration >= 1200 && duration <= 2000) {
                    check_irrx_register |= ((u32)1 << offset_point);
                    offset_point++;
                }
                // 判定是否为数据 0 (0.56ms)
                else if (duration >= 450 && duration <= 700) {
                    offset_point++;
                }
                // 出现非法宽度，直接判定解码失败，复位
                else {
                    ir_stage = IR_STAGE_IDLE;
                }

                // 4. 判定是否接收满 32 位数据 (0 ~ 31 位)
                if (ir_stage != IR_STAGE_IDLE && offset_point == 32) {
                    msg_enqueue(IR_MSG_DATA_COMPLETE);
                    // 在此处可以将完整的 check_irrx_register 赋值给你的全局功能寄存器
                    ir_stage = IR_STAGE_IDLE; // 成功接收，退回空闲状态等待下一次
                }
            }
        }
        // 状态六：如果重复发送，接收最后一个低电平
        else if (ir_stage == IR_STAGE_REPEAT) {
            if (duration >= 450 && duration <= 700) {
                msg_enqueue(IR_MSG_REPEAT_LOW);
            }
            // 出现非法宽度，直接判定解码失败，复位
            ir_stage = IR_STAGE_IDLE;
        }
        // 防御性设计：异常状态强行复位
        else {
            ir_stage = IR_STAGE_IDLE;
        }
    }
}


// 二、唤醒情况，残缺捕获红外信号
#if 1
AT(.com_text.irrx)
void irrx_special_signal_capture(void) {
    // 1. 获取归一化的当前电平
    u8 io_irrx_cur_sta = (GPIOA & BIT(1)) ? 1 : 0;

    // 2. 累加当前电平持续时间
    if (pulse_duration_us < 200000) {
        pulse_duration_us += 50;
    }

    // 3. 检测到电平跳变
    if (io_irrx_cur_sta != io_irrx_last_sta) {
        u8 finished_edge = io_irrx_last_sta;  // 刚刚结束的是什么电平
        u32 duration = pulse_duration_us;     // 拿到刚刚结束的电平宽度

        pulse_duration_us = 0;                // 立刻清零，保证新电平计时准确
        io_irrx_last_sta = io_irrx_cur_sta;   // 更新状态

        // 任何时候收到超长电平（如超过 15ms），说明一帧红外结束或断开，重置
        if (duration > 15000) {
            special_ir_stage = IR_STAGE_LEAD_HIGH;
            special_bit_cnt = 0;
            return;
        }


        if (special_ir_stage == IR_STAGE_LEAD_HIGH) {
            // 判定是否为高电平引导码 (4.5ms)
            if (finished_edge == 0) {
                if (duration > 800 && duration <= 5000) {
                    special_ir_stage = IR_STAGE_DATA;
                    special_bit_cnt = 0;
                }
            }
        }

        // ===== 残缺捕获核心：捕获地址位，数据位 =====
        else if (special_ir_stage == IR_STAGE_DATA) {
            if (finished_edge == 1) {
                u8 current_bit = 0xFF; // 预设为非法数据

                // 判定是否为数据 1 (1.69ms)
                if (duration >= 1200 && duration <= 2000) {
                    current_bit = 1;
                }
                // 判定是否为数据 0 (0.56ms)
                else if (duration >= 450 && duration <= 700) {
                    current_bit = 0;
                }

                // 如果是有效的数据位（0 或 1）
                if (current_bit != 0xFF) {
                    // 核心右移逻辑：整体右移一位
                    check_irrx_register >>= 1;

                    // 如果当前位是 1，把它放在最高位 BIT(31)
                    // 如果是 0，靠右移自动补 0，不需要额外操作
                    if (current_bit == 1) {
                        check_irrx_register |= 0x80000000;
                    }

                    special_bit_cnt++;

                    // 4. 判定是否接收满 32 位数据
                    if (special_bit_cnt == 32) {
                        msg_enqueue(IR_MSG_DATA_COMPLETE);     // 完美达到 32 位，通知主函数
                        special_bit_cnt = 0;     // 清空计数
                        special_ir_stage = IR_STAGE_LEAD_HIGH;
                    }
                }
                else {
                    // 中间出现了杂波干扰，说明数据断层，必须清零重来
                    special_bit_cnt = 0;
                }
            }

            else {
                special_ir_stage = IR_STAGE_LEAD_HIGH;
            }
        }
    }
}
#endif

#endif


/* ============================== 通用 功能库 ======================================== */
/* ===== 通用 红外遥控器 信号捕获 ===== */
#if COMMON_IRRX_SIGNAL_SCAN
/* 红外遥控器信号捕获 变量 */
// 定义解码状态
#define IR_STAGE_IDLE       0
#define IR_STAGE_LEAD_LOW   1
#define IR_STAGE_LEAD_HIGH  2
#define IR_STAGE_DATA       3
#define IR_STAGE_REPEAT     4

// 定义发送代码
/* 代码说明：
    1、捕获完 9ms的引导码低电平         后会发送：0x0D00
    2、捕获完 整段红外信号              后会发送：0x0D11
    3、捕获完 2.25ms的重复码高电平      后会发送：0x0D33
    4、捕获完 0.56ms的重复码低电平      后会发送：0x0D44
*/
#define IR_MSG_START_LOW      0x0D00
#define IR_MSG_DATA_COMPLETE  0x0D11
#define IR_MSG_REPEAT_HIGH    0x0D33
#define IR_MSG_REPEAT_LOW     0x0D44

// 定义完整捕获的静态变量
static u8 ir_stage = IR_STAGE_IDLE;
static u8 io_irrx_last_sta = 1;       // 默认状态就是高电平
static u32 pulse_duration_us = 0;     // 统一用一个变量记录当前电平持续时间
static u8 offset_point = 0;

AT(.com_text.irrx)
void common_irrx_signal_capture(void) {
    // 1. 获取归一化的当前电平 (0 或 1)，避开 BIT(1) 造成的非 1 隐患
    u8 io_irrx_cur_sta = (GPIOA & BIT(1)) ? 1 : 0;

    // 2. 累加当前电平的持续时间（增加防溢出保护，防止超长空闲时 u16/u32 翻转）
    if (pulse_duration_us < 200000) {
        pulse_duration_us += 50;
    }

    // 3. 检测到电平跳变（边缘触发）
    if (io_irrx_cur_sta != io_irrx_last_sta) {
        u8 finished_edge = io_irrx_last_sta;  // 刚刚结束的是什么电平
        u32 duration = pulse_duration_us;     // 拿到刚刚结束的电平宽度

        pulse_duration_us = 0;                // 核心修改：立刻清零，保证新电平计时准确
        io_irrx_last_sta = io_irrx_cur_sta;   // 更新状态

        // ===== 状态机核心逻辑 =====

        // 任何时候收到超长电平（如超过 15ms），强制复位状态机，防止死锁
        if (duration > 15000) {
            ir_stage = IR_STAGE_IDLE;
            offset_point = 0;
            return;
        }

        // 特殊：如果处于休眠状态
        if (sleep_sta_flag) {
            // 直接准备接收 地址位和数据位
            ir_stage = IR_STAGE_DATA;
            check_irrx_register = 0; // 准备开始装载，清零寄存器
            offset_point = 0;        // 偏移指针清零
        }

        // 状态一：空闲状态
        if (ir_stage == IR_STAGE_IDLE) {
            // 红外接收头（低电平有效）收到突发的低电平，且时间符合 9ms 引导码
            if (finished_edge == 0 && duration >= 7500 && duration <= 10500) {
                ir_stage = IR_STAGE_LEAD_LOW;
                msg_enqueue(IR_MSG_START_LOW);
            }
        }
        // 状态二：已接收到 9ms 引导低电平
        else if (ir_stage == IR_STAGE_LEAD_LOW) {
            // 状态三：等待 4.5ms 引导高电平（说明是第一次按下发送）
            if (finished_edge == 1 && duration >= 3800 && duration <= 5200) {
                ir_stage = IR_STAGE_DATA;
                check_irrx_register = 0; // 准备开始装载，清零寄存器
                offset_point = 0;        // 偏移指针清零
            }
            // 状态四：等待 2.25ms 重复码高电平（说明按下不松手重复发送）
            else if (finished_edge == 1 && duration >= 1800 && duration <= 2800) {
                ir_stage = IR_STAGE_REPEAT;

                // 发送按住没放手的 msg_code
                msg_enqueue(IR_MSG_REPEAT_HIGH);
            }
            else {
                ir_stage = IR_STAGE_IDLE; // 宽度不对，判定为干扰，退回空闲
            }
        }
        // 状态五：正在接收数据位
        else if (ir_stage == IR_STAGE_DATA) {
            // 根据【高电平】的宽度来判定 0 和 1
            if (finished_edge == 1) {
                // 判定是否为数据 1 (1.69ms)
                if (duration >= 1200 && duration <= 2000) {
                    check_irrx_register |= ((u32)1 << offset_point);
                    offset_point++;
                }
                // 判定是否为数据 0 (0.56ms)
                else if (duration >= 450 && duration <= 700) {
                    offset_point++;
                }
                // 出现非法宽度，直接判定解码失败，复位
                else {
                    ir_stage = IR_STAGE_IDLE;
                }

                // 4. 判定是否接收满 32 位数据 (0 ~ 31 位)
                if (ir_stage != IR_STAGE_IDLE && offset_point == 32) {
                    msg_enqueue(IR_MSG_DATA_COMPLETE);
                    // 在此处可以将完整的 check_irrx_register 赋值给你的全局功能寄存器
                    ir_stage = IR_STAGE_IDLE; // 成功接收，退回空闲状态等待下一次
                }
            }
        }
        // 状态六：如果重复发送，接收最后一个低电平
        else if (ir_stage == IR_STAGE_REPEAT) {
            if (duration >= 450 && duration <= 700) {
                msg_enqueue(IR_MSG_REPEAT_LOW);
            }
            // 出现非法宽度，直接判定解码失败，复位
            ir_stage = IR_STAGE_IDLE;
        }
        // 防御性设计：异常状态强行复位
        else {
            ir_stage = IR_STAGE_IDLE;
        }
    }
}
#endif



#if USR_TMR50US_ISR
AT(.com_text.timer)
void usr_tmr50us_isr(void) {
#if IRRX_SIGNAL_SCAN
    if (!sleep_sta_flag) {
        irrx_signal_capture();
    }
    else {
        irrx_special_signal_capture();
    }
#endif


#if COMMON_IRRX_SIGNAL_SCAN
    common_irrx_signal_capture();
#endif






}
#endif









AT(.com_text.timer)
void usr_tmr1us_isr(void)
{

}

AT(.com_text.isr)
void timer2_isr(void)
{
    tmr10us_cnt++;

    // 50μs 调用一次
    if ((tmr10us_cnt % 5) == 0) {

#if USR_TMR50US_ISR
        usr_tmr50us_isr();
#endif
        tmr10us_cnt = 0;
    }

















    if(TMR2CON & BIT(9)){
        TMR2CPND = BIT(9);
    }
    // usr_tmr1us_isr();

#if US_1S_TEST
    us10_tick++;
    if(100000 == us10_tick)
    {//1000 000us test
        my_printf(str_t3, us10_tick);
        us10_tick = 0;
    }
#endif//US_1S_TEST
#if US_IO_TEST
    GPIOASET = BIT(9);
    asm("nop");asm("nop");asm("nop");
    GPIOACLR = BIT(9);
#endif
}

void timer2_init(void)
{
    printf("timer2_init\n");
    TMR2CON = 0;
    TMR2CNT = 0;
    TMR2PR  = 60 - 1;                                   //60/6000000 == 0.000001s
    TMR2CON = (2 << 1) | (2 << 4) | BIT(7) | BIT(0);    //timer2 clk = xosc24m, div 4
    sys_irq_init(IRQ_TMR2_VECTOR, 0, timer2_isr);
#if US_1S_TEST
    us10_tick = 0;
#endif//US_1S_TEST
#if US_IO_TEST
    GPIOAFEN &= ~BIT(9);
    GPIOADE  |=  BIT(9);
    GPIOADIR &= ~BIT(9);
#endif
}

#endif

#if 0   //定时器边沿捕获
// AT(.com_rodata.test)
// const char test5[] = "%d\n";
u32 cpt_current;
u32 cpt_former;
u32 period;

AT(.com_text.test1)
void tmr1_isr(void)
{
    if(TMR1CON & BIT(9)) {
        GPIOA ^= BIT(5);
        if(!cpt_former){
            cpt_former = TMR1CNT;  //TMR1CNT will run from 0 to 65535
        }else{
            cpt_current = TMR1CNT;  //TMR1CNT will run from 0 to 65535
            if(cpt_current < cpt_former){
                period = 65526 - cpt_former + cpt_current;
            }else{
                period = cpt_current - cpt_former;
            }
            period = period / 3000;
            cpt_former = 0;
        }
        TMR1CPND |= BIT(9);
        // my_printf(test5, period);
    }
}

void tmr1_irq_init(void)
{
    register_isr(IRQ_TMR1_VECTOR, tmr1_isr);
    PICPR &= ~BIT(IRQ_TMR1_VECTOR);                   //low priority interrupt
	PICEN |= BIT(IRQ_TMR1_VECTOR);
}

AT(.com_text.test1)
void tmr1_capture_init_test(void)
{
    CLKGAT0 |= BIT(24);                               //tmr1 clk enable
    FUNCMCON1 = (0x2 << 16);                          //timer1 map映射
    FUNCINCON |= (IO_PA10 - 1);                       //映射到PA10

    GPIOAFEN &= ~BIT(10);
    GPIOADE |= BIT(10);
    GPIOADIR |= BIT(10);                              //注意方向为输出

    TMR1CON  = (2 << 1) | (3 << 4);                   //clk = xosc24m / 8
    TMR1CON |= BIT(15) | BIT(14) | BIT(10) | BIT(7);  // cpt fall edge & cpt rise edge enable、cpt pnd enable、cpt ie
    TMR1CON |= BIT(0);                                //tmr1 enable
    tmr1_irq_init();                                  //注册中断
}

#endif // 0
