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

AT(.com_text.isr)
void timer2_isr(void)
{
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
