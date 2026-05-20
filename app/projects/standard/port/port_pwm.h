#ifndef _PORT_PWM_H
#define _PORT_PWM_H

typedef enum{
    CLOCK_DEVIDE_1 = 0x0,
    CLOCK_DEVIDE_2,
    CLOCK_DEVIDE_4,
    CLOCK_DEVIDE_8,
    CLOCK_DEVIDE_16,
    CLOCK_DEVIDE_32,
    CLOCK_DEVIDE_48,
    CLOCK_DEVIDE_64,
}PWM_PRE_SEL_DIVIDE_TYPEDEF;

typedef enum{
    PWM_IO_DIS      =0x0,
    PWM_IO_EN       =0x1,
}PWM_GPIO_STATE_SEL;

typedef enum{
    PWM_INV_DIS      =0x0,
    PWM_INV_EN       =0x1,
}PWM_INV_STATE_SEL;

typedef enum{
    PWM_IE_DIS      =0x0,
    PWM_IE_EN       =0x1,
}PWM_IE_STATE_SEL;

typedef enum{
    PWM_DIS      =0x0,
    PWM_EN       =0x1,
}PWM_EN_SEL;

typedef enum{
    TIMER_PWM_MODE = 0x0,
    CAPTURE_RISING_MODE,
    CAPTURE_FALLING_MODE,
    CAPTURE_RISING_OR_FALLING_MODE,
}PWM_TMR_MODE_TYPEDEF_SEL;

//PWM_DUTY
typedef struct{
    u16 tmr0duty;
    u16 tmr1duty;
    u16 tmr2duty;
    u16 tmr3duty;
}PWM_TMRDUTY_TYPEDEF;

typedef struct{
    u16 tmr0pr;
    u16 tmr1pr;
    u16 tmr2pr;
    u16 tmr3pr;
}PWM_TMRPR_TYPEDEF;

//PWM_PRE_SEL
typedef struct{
    PWM_PRE_SEL_DIVIDE_TYPEDEF pwm0pre_sel;
    PWM_PRE_SEL_DIVIDE_TYPEDEF pwm1pre_sel;
    PWM_PRE_SEL_DIVIDE_TYPEDEF pwm2pre_sel;
    PWM_PRE_SEL_DIVIDE_TYPEDEF pwm3pre_sel;
}PWM_PRE_SEL_TYPEDEF;

//PWM_IO_SEL
typedef struct{
    PWM_GPIO_STATE_SEL pwm0_io_state;
    PWM_GPIO_STATE_SEL pwm1_io_state;
    PWM_GPIO_STATE_SEL pwm2_io_state;
    PWM_GPIO_STATE_SEL pwm3_io_state;
}PWM_IO_TYPEDEF;

//PWM_INV_SEL
typedef struct{
    PWM_INV_STATE_SEL pwm0_inv;
    PWM_INV_STATE_SEL pwm1_inv;
    PWM_INV_STATE_SEL pwm2_inv;
    PWM_INV_STATE_SEL pwm3_inv;
}PWM_INV_TYPEDEF;

//PWM_IE_SEL
typedef struct{
    PWM_IE_STATE_SEL pwm0_ie;
    PWM_IE_STATE_SEL pwm1_ie;
    PWM_IE_STATE_SEL pwm2_ie;
    PWM_IE_STATE_SEL pwm3_ie;
}PWM_IE_TYPEDEF;

//PWM_EN_SEL
typedef struct{
    PWM_EN_SEL pwm0_en;
    PWM_EN_SEL pwm1_en;
    PWM_EN_SEL pwm2_en;
    PWM_EN_SEL pwm3_en;
}PWM_EN_TYPEDEF;

//PWM_TMR_SEL
typedef struct{
    PWM_TMR_MODE_TYPEDEF_SEL pwm0_tmr_mode_state;
    PWM_TMR_MODE_TYPEDEF_SEL pwm1_tmr_mode_state;
    PWM_TMR_MODE_TYPEDEF_SEL pwm2_tmr_mode_state;
    PWM_TMR_MODE_TYPEDEF_SEL pwm3_tmr_mode_state;
}PWM_TMR_MODE_TYPEDEF;

//PWM_TMR_SEL
typedef struct{
    u8 pwm0_map_io;
    u8 pwm1_map_io;
    u8 pwm2_map_io;
    u8 pwm3_map_io;
}PWM_MAP_IO_TYPEDEF;

typedef struct{
    PWM_IO_TYPEDEF io_state;
    PWM_INV_TYPEDEF inv_state;
    PWM_TMRDUTY_TYPEDEF tmrduty;
    PWM_TMRPR_TYPEDEF tmrpr;
    PWM_PRE_SEL_TYPEDEF clock_pre_sel;
    PWM_TMR_MODE_TYPEDEF tmr_mode;
    PWM_MAP_IO_TYPEDEF pwm_map_io;
    PWM_IE_TYPEDEF ie_state;
    PWM_EN_TYPEDEF en_state;
}pwm_init_typedef;
extern pwm_init_typedef  pwm_cfg_init;

typedef struct {
    u8 pwm_val;
    u8 sys_clk_id;
} pwm_hw_tbl_t;

void bsp_pwm_hw_init(pwm_init_typedef *pwm_cfg);
void pwm_gpio_config(u8 io_num);
void pwm_map_all_io_select(u8 pwm0_io, u8 pwm1_io, u8 pwm2_io, u8 pwm3_io);
void pwm_clk_sync(void);

/*****************************************************************************
 * Module TMR2PWM
 *****************************************************************************/
enum{
    PWM_SYS_CLK,
    PWM_SYS_CLKPPP,
    PWM_XOSC24M_CLK,
    PWM_TMR_INC_PIN,
    PWM_RC2M_D0,
    PWM_RTC_RC2M,
    PWM_TMR_INC_CR,
    PWM_CLKOUT_PINP,
};

typedef enum{
    TMR2CLOCK_DEVIDE_1 = 0x0,
    TMR2CLOCK_DEVIDE_2,
    TMR2CLOCK_DEVIDE_4,
    TMR2CLOCK_DEVIDE_8,
    TMR2CLOCK_DEVIDE_16,
    TMR2CLOCK_DEVIDE_32,
    TMR2CLOCK_DEVIDE_64,
}TMR2PWM_PRE_SEL_DIVIDE_TYPEDEF;

typedef enum{
    TMR2PWM_IO_DIS      =0x0,
    TMR2PWM_IO_EN       =0x1,
}TMR2PWM_GPIO_STATE_SEL;

typedef enum{
    TMR2PWM_INV_DIS      =0x0,
    TMR2PWM_INV_EN       =0x1,
}TMR2PWM_INV_STATE_SEL;

typedef enum{
    TMR2_TIMER_PWM_MODE = 0x0,
    TMR2CAPTURE_RISING_MODE,
    TMR2CAPTURE_FALLING_MODE,
    TMR2CAPTURE_RISING_OR_FALLING_MODE,
}TMR2PWM_TMR_MODE_TYPEDEF_SEL;

//PWM_DUTY
typedef struct{
    u16 tmr0duty;
    u16 tmr1duty;
}TMR2PWM_TMRDUTY_TYPEDEF;

typedef struct{
    u32 tmr2pr;
}TMR2PWM_TMRPR_TYPEDEF;

//PWM_PRE_SEL
typedef struct{
    TMR2PWM_PRE_SEL_DIVIDE_TYPEDEF pwmpre_sel;
}TMR2PWM_PRE_SEL_TYPEDEF;

//PWM_IO_SEL
typedef struct{
    TMR2PWM_GPIO_STATE_SEL pwm0_io_state;
    TMR2PWM_GPIO_STATE_SEL pwm1_io_state;
}TMR2PWM_IO_TYPEDEF;

//PWM_INV_SEL
typedef struct{
    TMR2PWM_INV_STATE_SEL pwm0_inv;
    TMR2PWM_INV_STATE_SEL pwm1_inv;
}TMR2PWM_INV_TYPEDEF;

//PWM_TMR_SEL
typedef struct{
    TMR2PWM_TMR_MODE_TYPEDEF_SEL pwm_tmr_mode_state;
}TMR2PWM_TMR_MODE_TYPEDEF;

//PWM_TMR_SEL
typedef struct{
    u8 pwm0_map_io;
    u8 pwm1_map_io;
}TMR2PWM_MAP_IO_TYPEDEF;

typedef struct{
    TMR2PWM_IO_TYPEDEF io_state;
    TMR2PWM_INV_TYPEDEF inv_state;
    TMR2PWM_TMRDUTY_TYPEDEF tmrduty;
    TMR2PWM_TMRPR_TYPEDEF tmrpr;
    TMR2PWM_PRE_SEL_TYPEDEF clock_pre_sel;
    TMR2PWM_TMR_MODE_TYPEDEF tmr_mode;
    TMR2PWM_MAP_IO_TYPEDEF pwm_map_io;
}tmr2pwm_t;
extern tmr2pwm_t pwm_ptr;

void tmr2pwm_map_io_select(u8 pwm0_io, u8 pwm1_io);
void bsp_tmr2pwm_init(tmr2pwm_t *pwm_ptr);
void bsp_pwm_en(u8 pwm0_en, u8 pwm1_en, u8 pwm2_en, u8 pwm3_en);
void pwm_irq_init(void);
#endif




