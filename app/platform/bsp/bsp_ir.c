#include "include.h"

#if IRRX_SW_EN
typedef struct {
    u16 cnt;                            //ir data bit counter
    u16 rpt_cnt;                        //ir repeat counter
    u16 addr;                           //address,  inverted address   Extended NEC: 16bits address
    u16 cmd;                            //command,  inverted command
    u16 RPTERR;
    u16 DATERR;
    u16 ONEERR;
    u16 ZEROERR;
    u16 TOPR;
    u32 tmrlast;
} ir_cb_t;
ir_cb_t ir_cb AT(.buf.ir.cb);

AT(.com_text.str)
const char ir_str[] = "get_irkey: %04x, %04x %x\n";

#if 0
AT(.com_text.str)
const char ir_str1[] = "ir_cb.cmd %d\n";
AT(.com_text.str)
const char ir_str2[] = "n %d\n";
#endif

AT(.com_text.ir)
u8 get_irkey(void)
{
    u8 key_val = NO_KEY;
    if (ir_cb.cnt != 32) {
        return NO_KEY;
    }
    if (ir_cb.addr == IR_NULL && ir_cb.cmd == IR_NULL)
        return 0xff;

#if IR_ADDR_FF00_EN
    if (ir_cb.addr == IR_ADDR_FF00) {
        key_val = ir_tbl_FF00[ir_cb.cmd & 0xff];
//        my_printf(ir_str1,ir_cb.cmd & 0xff);
//        my_printf(ir_str2,ir_tbl_FF00[ir_cb.cmd & 0xff]);
    }
#endif // IR_ADDR_FF00_EN

#if IR_ADDR_BF00_EN
    if (ir_cb.addr == IR_ADDR_BF00) {
        key_val = ir_tbl_BF00[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_BF00_EN

#if IR_ADDR_FD02_EN
    if (ir_cb.addr == IR_ADDR_FD02) {
        key_val = ir_tbl_FD02[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FD02_EN

#if IR_ADDR_FE01_EN
    if (ir_cb.addr == IR_ADDR_FE01) {
        key_val = ir_tbl_FE01[ir_cb.cmd & 0xff];
    }
#endif // IR_ADDR_FE01_EN

#if IR_ADDR_7F80_EN
    if (ir_cb.addr == IR_ADDR_7F80) {
        key_val = ir_tbl_7F80[ir_cb.cmd & 0xff];
    }
#endif

    printf(ir_str, ir_cb.cmd, ir_cb.addr, key_val);
    return key_val;
}
#endif //IRRX_SW_EN


// AT(.com_rodata.bsp.ir)
// const char ir_str[] = "ir: %04x, %02x\n";

///软件timer capture irrx decode
#if IRRX_SW_EN
#define TMR2_RCLK               3000            //xosc24m_div8 3M

//AT(.com_text.str)
//const char test_ir[] = "test_cnt\n";

AT(.com_text.isr.timer)
void software_ir_encode(u32 tmrcnt)
{
    if (ir_cb.cnt == 32) {
        TMR2CNT = 0;
    if ((tmrcnt >= 10) && (tmrcnt <= 12)) {
        //my_printf(test_ir);
        //repeat code is simply 9ms+2.25ms
        ir_cb.rpt_cnt = 0;
    } else {
        ir_cb.rpt_cnt += tmrcnt;

        if (ir_cb.rpt_cnt > 108) {
            ir_cb.rpt_cnt = 0;
            ir_cb.cnt = 0;          //ir key release
        }
    }
    return;
    } else if ((tmrcnt > 7) || (tmrcnt == 0)) {     //A message is started by a 9ms AGC burst
        ir_cb.rpt_cnt = 0;
        ir_cb.cnt = 0;                  //ir key message started
        return;
    }
    ir_cb.cmd >>= 1;
    ir_cb.cnt++;
    if (tmrcnt == 2) {                  //Bit time of 1.125ms(0) or 2.25ms(1)
        ir_cb.cmd |= 0x8000;
    }

    if (ir_cb.cnt == 16) {
        ir_cb.addr = ir_cb.cmd;         //save address data
    } else if (ir_cb.cnt == 32) {
        //got ir key message
        if ((u8)ir_cb.cmd > 96) {
            ir_cb.cmd = NO_KEY;
        }
         //my_printf(ir_str, ir_cb.addr, (u8)ir_cb.cmd);
    }
}

AT(.com_text.isr.timer)
void timer2_isr(void)
{
    u32 tmrcnt;
    if((ir_cb.cnt > 0)&&(ir_cb.cnt < 32)){
        TMR2CPND = BIT(9);
        return;
    }else if (TMR2CON & BIT(9)){
        //timer2 overflow interrupt
        TMR2CPND = BIT(9);
        tmrcnt = 110;                   //110ms overflow
    } else {
        return;
    }
    software_ir_encode(tmrcnt);
}

AT(.text.bsp.ir)
void ir_key_clr(void)
{
    ir_cb.cnt = 0;
}

//timer1 capture
AT(.com_text.isr.timer)
void timer1_isr(void)
{
    u32 tmrcnt;
    u32 tmrpr;
    if(TMR1CON & BIT(9)){
        //timer1 capture interrupt
        if (!ir_cb.cnt){
            TMR2CNT = 0;
        }
        tmrpr = TMR1PR;
        if(ir_cb.tmrlast < tmrpr){
            tmrcnt = tmrpr - ir_cb.tmrlast;
        }else{
            tmrcnt = (0xffff - ir_cb.tmrlast) + tmrpr;
        }
        ir_cb.tmrlast = tmrpr;
        TMR1CPND = BIT(9);
        tmrcnt /= TMR2_RCLK;                //convert to ms
    }else{
        return;
    }
    software_ir_encode(tmrcnt);
}

AT(.text.bsp.ir)
void timer2_init(void)
{
    sys_irq_init(IRQ_TMR2_VECTOR, 0, timer2_isr);
    TMR2CON = 0;
	TMR2CNT = 0;
	TMR2PR  = TMR2_RCLK*110 - 1;                            //110ms Timer overflow interrupt
    TMR2CON = (2 << 1) | (3 << 4) | BIT(7) | BIT(0);        //timer2 clk = xosc24m / 8
    PICPR &= ~BIT(IRQ_TMR2_VECTOR);
	PICEN |= BIT(IRQ_TMR2_VECTOR);
}

AT(.text.bsp.ir)
void timer1_init(void)
{
    sys_irq_init(IRQ_TMR1_VECTOR, 0, timer1_isr);
    TMR1CON = 0;
	TMR1CNT = 0;
    TMR1CON = (2 << 1) | (3 << 4) | (2 << 14) | BIT(7) | BIT(0);
    PICPR &= ~BIT(IRQ_TMR1_VECTOR);
	PICEN |= BIT(IRQ_TMR1_VECTOR);
}

AT(.text.bsp.ir)
void irrx_sw_init(void)
{
    IR_CAPTURE_PORT();
    FUNCMCON1 = (2 << 16);              //enable timer1 map
    FUNCINCON = (IRRX_MAPPING - 1);

    memset(&ir_cb, 0, sizeof(ir_cb));
    timer1_init();
    timer2_init();
}
#endif // IRRX_SW_EN


