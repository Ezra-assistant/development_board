#include "include.h"

#if TMR2_US_EN

#define US_1S_TEST      0   //测试10us定时,累计到达1s的时间,通过download检查打印时间
#define US_IO_TEST      0   //PA9翻转IO测试us定时

#if US_1S_TEST
u32 us10_tick;
AT(.com_rodata.isr)
const char str_t3[] = "10us_tick: %d\n";
#endif//test

AT(.com_text.timer)
void usr_tmr1us_isr(void)
{

}






/* 自定义内容 */
u8 tmr10us_cnt;

/* ===== 实现红外遥控器信号捕获 变量 ===== */
#if 0
static u8 io_irrx_cur_sta = 1;        // 当前电平
static u8 io_irrx_last_sta = 1;       // 上一次电平状态（默认高）
static u16 high_pin_us = 0;           // 当前高电平持续时间(μs)
static u16 low_pin_us = 0;            // 当前低电平持续时间(μs)
static u16 recore_high_pin_us = 0;    // 记录高电平持续时间(μs)，用于后续
static u16 recore_low_pin_us = 0;     // 记录低电平持续时间(μs)，用于后续
static u32 idle_timer_count = 0       // 空闲计时，如果高电平持续超过20次，则判定为空闲
static u8 is_signal_coming_flag = 0;  // 有信号，拉低线
static u8 offset_point = 0;           // 用于偏移装载测试寄存器
#endif
// 定义解码状态
#define IR_STAGE_IDLE       0
#define IR_STAGE_LEAD_LOW   1
#define IR_STAGE_LEAD_HIGH  2
#define IR_STAGE_DATA       3
#define IR_STAGE_REPEAT     4

static u8 ir_stage = IR_STAGE_IDLE;
static u8 io_irrx_last_sta = 1;       // 默认状态就是高电平
static u32 pulse_duration_us = 0;     // 统一用一个变量记录当前电平持续时间
static u8 offset_point = 0;


/* ===== 实现红外遥控器信号捕获 函数 ===== */
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
                msg_enqueue(0x0D00);
            }
        }
        // 状态二：已接收到引导低电平，等待 4.5ms 引导高电平
        else if (ir_stage == IR_STAGE_LEAD_LOW) {
            if (finished_edge == 1 && duration >= 3800 && duration <= 5200) {
                ir_stage = IR_STAGE_DATA;
                check_irrx_register = 0; // 准备开始装载，清零寄存器
                offset_point = 0;        // 偏移指针清零
            } 
            // 状态四：已接收到 repeat 低电平，等待 2.25ms repeat 高电平
            else if (finished_edge == 1 && duration >= 1800 && duration <= 2800) {
                ir_stage = IR_STAGE_REPEAT;

                // 发送按住没放手的 msg_code
                msg_enqueue(0x0D22);
            }
            else {
                ir_stage = IR_STAGE_IDLE; // 宽度不对，判定为干扰，退回空闲
            }
        }
        // 状态三：正在接收数据位
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
                    msg_enqueue(0x0D11);
                    // 在此处可以将完整的 check_irrx_register 赋值给你的全局功能寄存器
                    ir_stage = IR_STAGE_IDLE; // 成功接收，退回空闲状态等待下一次
                }
            }
        }
        // 状态四：如果重复发送，接收最后一个低电平
        else if (ir_stage == IR_STAGE_REPEAT) {
            if (duration >= 450 && duration <= 700) {
                msg_enqueue(0x0D44);
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






AT(.com_text.timer)
void usr_tmr50us_isr(void) {

    irrx_signal_capture();

    


}













AT(.com_text.isr)
void timer2_isr(void)
{
    tmr10us_cnt++;

    if ((tmr10us_cnt % 5) == 0) {
        usr_tmr50us_isr();
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
