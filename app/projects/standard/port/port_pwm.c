#include "include.h"
#include "port_pwm.h"

#if (PWM_HW_EN || PWM_TMR2_EN)
AT(.text.bsp.pwm)
void pwm_gpio_config(u8 io_num)
{
    if(io_num == IO_NONE) {
        return;
    }

    gpio_t gpio;
    gpio_cfg_init(&gpio, io_num);
    gpio.sfr[GPIOxDE] |= BIT(gpio.num);
    gpio.sfr[GPIOxFEN] |= BIT(gpio.num);
    gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);
}
#endif

/*****************************************************************************
 * Module    : HW_PWM
 *****************************************************************************/
#if PWM_HW_EN
const u8 pwm_hw_map_sel[] = PWM_HW_MAP_SEL;

pwm_init_typedef  pwm_cfg_init AT(.pwm_buf);

//系统时钟PWM系数
const pwm_hw_tbl_t pwm_table[] = {
    {0,  SYS_RC2M},
    {3,  SYS_24M},
    {6,  SYS_48M},
    {10, SYS_80M},
    {15, SYS_120M},
    {20, SYS_160M},
};

//寄存器配置
void bsp_pwm_io_init(u8 io_num_0, u8 io_num_1, u8 io_num_2, u8 io_num_3)
{
    FUNCMCON1 |= BIT(8);    //PWM map to G1

    if(io_num_0) {
        pwm_gpio_config(IO_PA6);
        PWMCON = (PWMCON & ~(0x3 << 16)) | (0x0 << 16);
    }

    if(io_num_1) {
        pwm_gpio_config(IO_PA7);
        PWMCON = (PWMCON & ~(0x3 << 18)) | (0x1 << 18);
    }

    if(io_num_2) {
        pwm_gpio_config(IO_PA11);
        PWMCON = (PWMCON & ~(0x3 << 20)) | (0x2 << 20);
    }

    if(io_num_3) {
        pwm_gpio_config(IO_PA12);
        PWMCON = (PWMCON & ~(0x3 << 22)) | (0x3 << 22);
    }
}

void pwm_map_all_io_select(u8 pwm0_io, u8 pwm1_io, u8 pwm2_io, u8 pwm3_io)
{
    if(pwm0_io) {
        pwm_gpio_config(pwm0_io);
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 0)) | (0 << 0);                                        //PWM0 map
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 0)) | (pwm0_io << 0);
        PWMCON = (PWMCON & ~(0x3 << 16)) | (0x0 << 16);                                           //PWM0 select PWM_TMR0
    }

    if(pwm1_io) {
        pwm_gpio_config(pwm1_io);                                                                 //PWM1 map
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 8)) | (1 << 8);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 8)) | (pwm1_io << 8);
        PWMCON = (PWMCON & ~(0x3 << 18)) | (0x1 << 18);                                           //PWM1 select PWM_TMR1
    }

    if(pwm2_io) {
        pwm_gpio_config(pwm2_io);                                                                 //PWM2 map
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 16)) | (2 << 16);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 16)) | (pwm2_io << 16);
        PWMCON = (PWMCON & ~(0x3 << 20)) | (0x2 << 20);                                           //PWM2 select PWM_TMR2
    }

    if(pwm3_io) {
        pwm_gpio_config(pwm3_io);                                                                 //PWM3 map
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 24)) | (3 << 24);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 24)) | (pwm3_io << 24);
        PWMCON = (PWMCON & ~(0x3 << 22)) | (0x3 << 22);                                           //PWM3 select PWM_TMR3
    }

}

void bsp_pwm_clk_init(void)
{
    CLKGAT1 |= BIT(8);     //PWM clken

    //CLKGAT0 |= BIT(25);    //TMR2 clken
}

void bsp_pwm_inv_cfg(u8 en0,u8 en1,u8 en2,u8 en3)
{
    ///PWM0~3 invert
    if(en0) {
        PWMCON |= BIT(8);
    } else {
        PWMCON &= ~BIT(8);
    }

    if(en1) {
        PWMCON |= BIT(9);
    } else {
        PWMCON &= ~BIT(9);
    }

    if(en2) {
        PWMCON |= BIT(10);
    } else {
        PWMCON &= ~BIT(10);
    }

    if(en3) {
        PWMCON |= BIT(11);
    } else {
        PWMCON &= ~BIT(11);
    }
}

void bsp_pwm_dump_sfr(void)
{
    printf("PWMCON = 0x%08x\n",PWMCON);
    printf("PWMR0CON = 0x%08x\n",PTMR0CON);
    printf("PWMR1CON = 0x%08x\n",PTMR1CON);
}

void bsp_pwm_pre_en(u8 pre0,u8 pre1,u8 pre2,u8 pre3)
{
    ///PWM0~3 Pre_sel selection  000:clock divide 1  001:clock divide 2  010:clock divide 4  011:clock divide 8  100:clock divide 16  101:clock divide 32  110:clock divide 48  111:clock divide 64
    PTMR0CON = (PTMR0CON & ~(0x7 << 4)) | (pre0 << 4);
    PTMR1CON = (PTMR1CON & ~(0x7 << 4)) | (pre1 << 4);
    PTMR2CON = (PTMR2CON & ~(0x7 << 4)) | (pre2 << 4);
    PTMR3CON = (PTMR3CON & ~(0x7 << 4)) | (pre3 << 4);
}

void bsp_pwm_tmr_mode(u8 mode0,u8 mode1,u8 mode2,u8 mode3)
{
    ///PWM0~3 TMR_MODE
    PTMR0CON = (PTMR0CON & ~(0x7 << 14)) | (mode0 << 14);
    PTMR1CON = (PTMR1CON & ~(0x7 << 14)) | (mode1 << 14);
    PTMR2CON = (PTMR2CON & ~(0x7 << 14)) | (mode2 << 14);
    PTMR3CON = (PTMR3CON & ~(0x7 << 14)) | (mode3 << 14);
}

void bsp_pwm_duty_cfg(u16 duty0, u16 duty1, u16 duty2, u16 duty3, u8 sys_sync)
{
    ///PWM0~3 PTMRDUTY
    PTMR0DUTY = BIT(0) * duty0 * sys_sync;
    PTMR1DUTY = BIT(0) * duty1 * sys_sync;
    PTMR2DUTY = BIT(0) * duty2 * sys_sync;
    PTMR3DUTY = BIT(0) * duty3 * sys_sync;
}

void bsp_pwm_pr_cfg(u16 pr0, u16 pr1, u16 pr2, u16 pr3, u8 sys_sync)
{
    ///PWM0~3 PTMRDUTY
    PTMR0PR = BIT(0) * pr0 * sys_sync;
    PTMR1PR = BIT(0) * pr1 * sys_sync;
    PTMR2PR = BIT(0) * pr2 * sys_sync;
    PTMR3PR = BIT(0) * pr3 * sys_sync;
}

void bsp_pwm_en(u8 pwm0_en, u8 pwm1_en, u8 pwm2_en, u8 pwm3_en)
{
    if(pwm0_en) {
        PWMCON |= BIT(0);                                           //TMR0 en
        PTMR0CON |= BIT(0);
        PWMCON |= BIT(4);

    } else {
        PWMCON &= ~BIT(0);                                          //TMR0 dis
        PTMR0CON &= ~BIT(0);
        PWMCON &= ~BIT(4);
    }

    if(pwm1_en) {
        PWMCON |= BIT(1);                                           //TMR1 en
        PTMR1CON |= BIT(0);
        PWMCON |= BIT(5);
    } else {
        PWMCON &= ~BIT(1);                                          //TMR1 dis
        PTMR1CON &= ~BIT(0);
        PWMCON &= ~BIT(5);
    }

    if(pwm2_en) {
        PWMCON |= BIT(2);                                           //TMR2 en
        PTMR2CON |= BIT(0);
        PWMCON |= BIT(6);
    } else {
        PWMCON &= ~BIT(2);                                          //TMR2 dis
        PTMR2CON &= ~BIT(0);
        PWMCON &= ~BIT(6);
    }

    if(pwm3_en) {
        PWMCON |= BIT(3);                                           //TMR3 en
        PTMR3CON |= BIT(0);
        PWMCON |= BIT(7);
    } else {
        PWMCON &= ~BIT(3);                                          //TMR3 dis
        PTMR3CON &= ~BIT(0);
        PWMCON &= ~BIT(7);
    }
}

void bsp_pwm_ie(u8 pwm0_ie, u8 pwm1_ie, u8 pwm2_ie, u8 pwm3_ie)
{
    if(pwm0_ie) {
        PTMR0CON |= BIT(7);
    } else {
        PTMR0CON &= ~BIT(7);
    }

    if(pwm1_ie) {
        PTMR1CON |= BIT(7);
    } else {
        PTMR1CON &= ~BIT(7);
    }

    if(pwm2_ie) {
        PTMR2CON |= BIT(7);
    } else {
        PTMR2CON &= ~BIT(7);
    }

    if(pwm3_ie) {
        PTMR3CON |= BIT(7);
    } else {
        PTMR2CON &= ~BIT(7);
    }
}

//初始化和中断
AT(.com_text.isr.pwm)
void pwm_isr(void)
{
    if(PTMR0CON & BIT(9)) {
        PTMR0CPND |= BIT(9);
    }
    if(PTMR1CON & BIT(9)) {
        PTMR1CPND |= BIT(9);
    }
    if(PTMR2CON & BIT(9)) {
        PTMR2CPND |= BIT(9);
    }
    if(PTMR3CON & BIT(9)) {
        PTMR3CPND |= BIT(9);
    }
}

void pwm_clk_sync(void)
{
    u8 sys_clk = get_cur_sysclk();
    static u8 sys_clk_pre = 0;
    if(sys_clk == sys_clk_pre) {
        return ;
    }

    my_printf("sys_clk %d\n",sys_clk);
    u8 p_clk_val = 0;
    pwm_init_typedef *p = &pwm_cfg_init;

    switch (sys_clk) {

        case SYS_RC2M:
            return;
            break;

        case SYS_24M:
            p_clk_val = pwm_table[sys_clk].pwm_val;
            break;

        case SYS_48M:
            p_clk_val = pwm_table[sys_clk].pwm_val;
            break;

        case SYS_80M:
            p_clk_val = pwm_table[sys_clk].pwm_val;
            break;

        case SYS_120M:
            p_clk_val = pwm_table[sys_clk].pwm_val;
            break;

        case SYS_160M:
            p_clk_val = pwm_table[sys_clk].pwm_val;
            break;

        default:
            break;
    }
    sys_clk_pre = sys_clk;

    bsp_pwm_en(PWM_DIS,PWM_DIS,PWM_DIS,PWM_DIS);

    bsp_pwm_duty_cfg(p->tmrduty.tmr0duty, p->tmrduty.tmr1duty, p->tmrduty.tmr2duty, p->tmrduty.tmr3duty, p_clk_val);

    bsp_pwm_pr_cfg(p->tmrpr.tmr0pr, p->tmrpr.tmr1pr, p->tmrpr.tmr2pr, p->tmrpr.tmr3pr, p_clk_val);

    bsp_pwm_en(p->en_state.pwm0_en, p->en_state.pwm1_en, p->en_state.pwm2_en, p->en_state.pwm3_en);
}

void bsp_pwm_hw_init(pwm_init_typedef *pwm_cfg)
{
    pwm_init_typedef *p = pwm_cfg;
    u8 sys_clk = get_cur_sysclk();

    if(p) {
        bsp_pwm_clk_init();

#if !PWM_HW_MAP_IO_EN  //不映射，则使用默认IO
        bsp_pwm_io_init(p->io_state.pwm0_io_state, p->io_state.pwm1_io_state, p->io_state.pwm2_io_state, p->io_state.pwm3_io_state);
#endif
        //分频
        bsp_pwm_pre_en(p->clock_pre_sel.pwm0pre_sel, p->clock_pre_sel.pwm1pre_sel, p->clock_pre_sel.pwm2pre_sel, p->clock_pre_sel.pwm3pre_sel);
        //mode
        bsp_pwm_tmr_mode(p->tmr_mode.pwm0_tmr_mode_state, p->tmr_mode.pwm1_tmr_mode_state, p->tmr_mode.pwm2_tmr_mode_state, p->tmr_mode.pwm3_tmr_mode_state);
        //duty
        bsp_pwm_duty_cfg(p->tmrduty.tmr0duty, p->tmrduty.tmr1duty, p->tmrduty.tmr2duty, p->tmrduty.tmr3duty, pwm_table[sys_clk].pwm_val );   //默认48M系统时钟系数
        //周期
        bsp_pwm_pr_cfg(p->tmrpr.tmr0pr, p->tmrpr.tmr1pr, p->tmrpr.tmr2pr, p->tmrpr.tmr3pr, pwm_table[sys_clk].pwm_val );                     //默认48M系统时钟系数
        //是否反相
        bsp_pwm_inv_cfg(p->inv_state.pwm0_inv, p->inv_state.pwm1_inv, p->inv_state.pwm2_inv, p->inv_state.pwm3_inv);
        //是否开中断
        bsp_pwm_ie(p->ie_state.pwm0_ie, p->ie_state.pwm1_ie, p->ie_state.pwm2_ie, p->ie_state.pwm3_ie);
        //是否使能
        bsp_pwm_en(p->en_state.pwm0_en, p->en_state.pwm1_en, p->en_state.pwm2_en, p->en_state.pwm3_en);

        pwm_clk_sync();
        delay_us(100);
    }
    pwm_irq_init();
}

///硬件PWM初始化函数
void pwm_hw_cfg_init(void)
{
    memset(&pwm_cfg_init, 0, sizeof(pwm_cfg_init));
#if PWM_HW_MAP_IO_EN
    pwm_map_all_io_select(pwm_hw_map_sel[0],pwm_hw_map_sel[1],pwm_hw_map_sel[2],pwm_hw_map_sel[3]);        //PWM IO映射
#else
    pwm_cfg_init.io_state.pwm0_io_state    =    PWM_IO_EN;       //只有PWM OUTPUT 对应的IO使能，才会操操作相关的寄存器，才能输出PWM波
    pwm_cfg_init.io_state.pwm1_io_state    =    PWM_IO_EN;
    pwm_cfg_init.io_state.pwm2_io_state    =    PWM_IO_EN;
    pwm_cfg_init.io_state.pwm3_io_state    =    PWM_IO_EN;
#endif // PWM_HW_MAP_IO_EN

	//参数配置
    //占空比duty (范围是 0~0xffff, duty)
    pwm_cfg_init.tmrduty.tmr0duty              =    0x7fff;
    pwm_cfg_init.tmrduty.tmr1duty              =    0x7000;
    pwm_cfg_init.tmrduty.tmr2duty              =    0x7fff;
    pwm_cfg_init.tmrduty.tmr3duty              =    0x7000;
    //周期PR (PWM周期范围时0~0xffff, PWM周期必须大于PWM的占空比)
    pwm_cfg_init.tmrpr.tmr0pr                  =    0xffff;
    pwm_cfg_init.tmrpr.tmr1pr                  =    0xffff;
    pwm_cfg_init.tmrpr.tmr2pr                  =    0xffff;
    pwm_cfg_init.tmrpr.tmr3pr                  =    0xffff;
    //是否反相invert
    pwm_cfg_init.inv_state.pwm0_inv            =    PWM_INV_EN;
    pwm_cfg_init.inv_state.pwm1_inv            =    PWM_INV_EN;
    pwm_cfg_init.inv_state.pwm2_inv            =    PWM_INV_EN;
    pwm_cfg_init.inv_state.pwm3_inv            =    PWM_INV_EN;
    //分频选择pre_sel
    pwm_cfg_init.clock_pre_sel.pwm0pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm1pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm2pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm3pre_sel     =    CLOCK_DEVIDE_1;
    //timer or PWM mode 默认PWM模式不需要修改
    pwm_cfg_init.tmr_mode.pwm0_tmr_mode_state  =    TIMER_PWM_MODE;
    pwm_cfg_init.tmr_mode.pwm1_tmr_mode_state  =    TIMER_PWM_MODE;
    pwm_cfg_init.tmr_mode.pwm2_tmr_mode_state  =    TIMER_PWM_MODE;
    pwm_cfg_init.tmr_mode.pwm3_tmr_mode_state  =    TIMER_PWM_MODE;
    //是否开启中断
    pwm_cfg_init.ie_state.pwm0_ie              =    PWM_IE_EN;
    pwm_cfg_init.ie_state.pwm1_ie              =    PWM_IE_EN;
    pwm_cfg_init.ie_state.pwm2_ie              =    PWM_IE_EN;
    pwm_cfg_init.ie_state.pwm3_ie              =    PWM_IE_EN;

#if PWM_HW_1KHZ_TEST_EN
    //系统时钟48M，PWM频率 = 系统时钟(48M) / （PWM周期(tmrpr)+1） / PWM分频(clock_pre_sel) / 时钟系数6
    // 48000000 / (7999（0x1f3f） + 1)/ 6 = 1000HZ
    pwm_cfg_init.tmrduty.tmr0duty              =    0xf9f;
    pwm_cfg_init.tmrduty.tmr1duty              =    0xf9f;
    pwm_cfg_init.tmrduty.tmr2duty              =    0xf9f;
    pwm_cfg_init.tmrduty.tmr3duty              =    0xf9f;

    pwm_cfg_init.tmrpr.tmr0pr                  =    0x1f3f;
    pwm_cfg_init.tmrpr.tmr1pr                  =    0x1f3f;
    pwm_cfg_init.tmrpr.tmr2pr                  =    0x1f3f;
    pwm_cfg_init.tmrpr.tmr3pr                  =    0x1f3f;

    pwm_cfg_init.clock_pre_sel.pwm0pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm1pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm2pre_sel     =    CLOCK_DEVIDE_1;
    pwm_cfg_init.clock_pre_sel.pwm3pre_sel     =    CLOCK_DEVIDE_1;
#endif // PWM_HW_1KHZ_TEST_EN
    //是否开启PWM
    pwm_cfg_init.en_state.pwm0_en              =    PWM_EN;
    pwm_cfg_init.en_state.pwm1_en              =    PWM_EN;
    pwm_cfg_init.en_state.pwm2_en              =    PWM_EN;
    pwm_cfg_init.en_state.pwm3_en              =    PWM_EN;

    bsp_pwm_hw_init(&pwm_cfg_init);
}
#endif //PWM_HW_EN



/*****************************************************************************
 * Module    : TMR2PWM
 *****************************************************************************/
#if PWM_TMR2_EN

const u8 pwm_tmr2_map_sel[] = PWM_TMR2_MAP_SEL;

//寄存器配置
void bsp_tmr2pwm_clk_init(void)
{
    CLKGAT0 |= BIT(25);
}

void bsp_tmr2pwm_en(u8 pwm0_sta, u8 pwm1_sta)
{
    if(pwm0_sta) {
        TMR2CON |= BIT(16);    //pwm en
    } else {
        TMR2CON &= ~BIT(16);
    }

    if(pwm1_sta) {
        TMR2CON |= BIT(18);    //pwm en
    } else {
        TMR2CON &= ~BIT(18);
    }
}

void bsp_tmr2pwm_mode(u8 mode)
{
    TMR2CON = (TMR2CON & ~(0x3 << 14)) | (mode << 14);
}

void bsp_tmr2pwm_inv_en(u8 pwm0_sta, u8 pwm1_sta)
{
    if(pwm0_sta) {
        TMR2CON |= BIT(17);
    } else {
        TMR2CON &= ~BIT(17);
    }

    if(pwm1_sta) {
        TMR2CON |= BIT(19);
    } else {
        TMR2CON &= ~BIT(19);
    }
}

void bsp_tmr2pwm_pr_config(u32 pwmpr)
{
    TMR2PR = pwmpr;
}

void bsp_tmr2pwm_duty_config(u16 pwm0duty, u16 pwm1duty)
{
    TMR2DUTY0 = pwm0duty;
    TMR2DUTY1 = pwm1duty;
}

void tmr2pwm_map_io_select(u8 pwm0_io, u8 pwm1_io)
{
    if(pwm0_io) {
        pwm_gpio_config(pwm0_io);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 0)) | (pwm0_io << 0);
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 0)) | (4 << 0);
    }

    if(pwm1_io) {
        pwm_gpio_config(pwm1_io);
        FUNCOUTMCON = (FUNCOUTMCON & ~(0xf << 8)) | (pwm1_io << 8);
        FUNCOUTCON =(FUNCOUTCON & ~(0xf << 8)) | (5 << 8);
    }
}

//初始化
void bsp_tmr2pwm_init(tmr2pwm_t *pwm_cfg)
{
    tmr2pwm_t *p = pwm_cfg;

    if(p) {
        my_printf("%s\n",__func__);
        TMR2CON = (TMR2CON & ~(0x7<<1)) | (PWM_TMR2_CLK_SEL << 1);
        //时钟初始化
        bsp_tmr2pwm_clk_init();
        //IO使能
        bsp_tmr2pwm_en(p->io_state.pwm0_io_state, p->io_state.pwm1_io_state);
        //mode
        bsp_tmr2pwm_mode(p->tmr_mode.pwm_tmr_mode_state);
        //分频
        TMR2CON = (TMR2CON & ~(0x7 << 4)) | (p->clock_pre_sel.pwmpre_sel << 4);
        //周期
        bsp_tmr2pwm_pr_config(p->tmrpr.tmr2pr);
        //duty
        bsp_tmr2pwm_duty_config(p->tmrduty.tmr0duty, p->tmrduty.tmr1duty);
        //是否反相
        bsp_tmr2pwm_inv_en(p->inv_state.pwm0_inv,p->inv_state.pwm1_inv);

        TMR2CNT = -1;
        TMR2CON |= BIT(7);
        TMR2CON |= BIT(0);
    }
    //my_printf("TMR2CON 0x%x\n",TMR2CON);
}

///TMR2PWM初始化函数
void tmr2pwm_cfg_init(void)
{
    tmr2pwm_t tmr2_pwm_init;
    memset(&tmr2_pwm_init, 0, sizeof(tmr2_pwm_init));

#if PWM_TMR2_MAP_IO_EN
    tmr2pwm_map_io_select(pwm_tmr2_map_sel[0],pwm_tmr2_map_sel[1]);
#else
    GPIOADE |= BIT(2)| BIT(3);
    GPIOAFEN |= BIT(2)|BIT(3);
    GPIOADIR &= ~(BIT(2)|BIT(3));
    FUNCMCON1 = (FUNCMCON1 & ~(0xf << 4)) | (1 << 4);
#endif // PWM_TMR2_MAP_IO_EN

    //参数配置
    //是否开启PWM
    tmr2_pwm_init.io_state.pwm0_io_state      = TMR2PWM_IO_EN;
    tmr2_pwm_init.io_state.pwm1_io_state      = TMR2PWM_IO_EN;
    //是否反相invert
    tmr2_pwm_init.inv_state.pwm0_inv          = TMR2PWM_INV_EN;
    tmr2_pwm_init.inv_state.pwm1_inv          = TMR2PWM_INV_EN;
    //占空比duty (范围是 0~0xffff, duty)
    tmr2_pwm_init.tmrduty.tmr0duty            = 0x7fff;
    tmr2_pwm_init.tmrduty.tmr1duty            = 0x7fff;
    //周期PR (PWM周期范围时0~0xffff, PWM周期必须大于PWM的占空比)
    tmr2_pwm_init.tmrpr.tmr2pr                = 0xffff;
    //分频选择pre_sel
    tmr2_pwm_init.clock_pre_sel.pwmpre_sel    = TMR2CLOCK_DEVIDE_1;
    //timer or PWM mode 默认PWM模式不需要修改
    tmr2_pwm_init.tmr_mode.pwm_tmr_mode_state = TMR2_TIMER_PWM_MODE;
#if PWM_TMR2_1KHZ_TEST_EN
    //PWM时钟，PWM频率 = PWM时钟 / （PWM周期(tmrpr)+1） / PWM分频(clock_pre_sel)
    //此处选择XOSC24M_CLK，即24M时钟
    tmr2_pwm_init.tmrduty.tmr0duty            = 0x2ee0;
    tmr2_pwm_init.tmrduty.tmr1duty            = 0x2ee0;
    //周期PR (PWM周期范围时0~0xffff, PWM周期必须大于PWM的占空比)
    tmr2_pwm_init.tmrpr.tmr2pr                = 0x5dbf;
    //分频选择pre_sel
    tmr2_pwm_init.clock_pre_sel.pwmpre_sel    = TMR2CLOCK_DEVIDE_1;
#endif // PWM_TMR2_1KHZ_TEST_EN

    bsp_tmr2pwm_init(&tmr2_pwm_init);
}
#endif // PWM_TMR2_EN
