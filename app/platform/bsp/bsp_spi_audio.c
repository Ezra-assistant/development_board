#include "include.h"

#if SPI1_AUDIO_EN

#define TRACE_EN            1
#if     TRACE_EN
#define TRACE(...)          my_printf(__VA_ARGS__)
#define TRACE_R(...)        my_print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

void spi1_irq_read(void);
void spi1_irq_tx(void);

void spi1_audio_cs_en(void);
void spi1_audio_cs_dis(void);
void spi1_audio_cs_init(void);

u32 get_sysclk_nhz(void);
void obuf_put_samples_w(void *buf, uint len);//SPI1 中断入口函数
uint calc_crc(void *buf, uint len, uint seed);

volatile bool flag_spk_data_kick = true;

#define SPI1_AUDIO_CS_INIT()                spi1_audio_cs_init()
#define SPI1_AUDIO_CS_EN()                  spi1_audio_cs_en()
#define SPI1_AUDIO_CS_DIS()                 spi1_audio_cs_dis()

#define EDGE_INT_PORT                       (SPI1_AUDIO_CS_IO - 1)
#define EDGE_PIN_IRQ_DIS()                  {PORTINTEN &= ~BIT(EDGE_INT_PORT);}
#define EDGE_PIN_IRQ_EN()                   {PORTINTEN |=  BIT(EDGE_INT_PORT);}

#if CIRCULAR_BUF_EN
// u8 spi1_tx_buf[SPI1_TXBUF_SIZE];
// u8 spi1_rx_buf[SPI_RX_MAX][SPI1_RXBUF_SIZE];
u8 tmp_send_buf[576];
cbuf_t cbuf_spi1tx AT(.spi_buf);
#endif

gpio_t spi1_audio_cs_gpio AT(.spi_tx_buf);
u8 spi1_tx_buf[SPI1_TXBUF_SIZE] AT(.spi_tx_buf);
u8 spi1_rx_buf[SPI1_RXBUF_SIZE] AT(.spi_recv_buf);

DMABUF_VARS dbuf_spi1rx AT(.spi_buf);
SPI1_VARS spi1_trans_vars AT(.spi_buf);

/*----------------------------------------------------------------------------*/
//SPI主从机数据交互测试
//command01
//主机发送有效数据
#if 0 //简单数据测试
AT(.com_rodata.spi1_cmd)
static const u8 master_tx_cmd_01[] = {
    0xAA,0x88,  //data---
    0x44,0x22,  //data---
    0x11,0xFF,  //data---
    0x12,0x34,  //data---
    0x56,0x78,  //data---
    0x66,
};

//command08
//从机发送有效数据
AT(.com_rodata.spi1_cmd)
static const u8 slave_tx_cmd_08[] = {
    0x55,0x12,  //data---
    0x34,0x56,  //data---
    0x78,0xFF,  //data---
    0x88,0x44,  //data---
    0x22,0x11,  //data---
    0x67
};
#endif

const u8 spi1_test_table[SPI1_TXBUF_SIZE] = {
    0x0,  0x1,   0x2,  0x3,  0x4,  0x5, 0x6,   0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,

    0x0,  0x1,   0x2,  0x3,  0x4,  0x5, 0x6,   0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,

    0x0,  0x1,   0x2,  0x3,  0x4,  0x5, 0x6,   0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,

    0x0,  0x1,   0x2,  0x3,  0x4,  0x5, 0x6,   0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
};

/*----------------------------------------------------------------------------*/


/*!**************************************************************************************************
\brief          SPI1初始化
\SPI主机调用spi1_master_init，SPI从机调用spi1_salve_init
\param[in]      No param_in
\param[out]     No param_out
\return         No return
****************************************************************************************************/
AT(.text.spiflash1_drv)
void spi1_master_init(u32 baud)
{
#if SPI3WIREEN
    //SPI1_G1: CLK_PB0, DI_PB2, DO_PB1,   CS_PA9
    GPIOBFEN |= BIT(0)|BIT(1)|BIT(2);
    GPIOBDE |=  BIT(0)|BIT(1)|BIT(2);
    GPIOBDIR &= ~(BIT(0) | BIT(1));  //CLK,DO output
    GPIOBDIR |= BIT(2);  //DI input
    GPIOBPU |= BIT(2);   //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = (SPI1_G1<<12);  //G5
#else
    //SPI1_G1: CLK_PB0, DIO_PB1   //CS_PA9
    GPIOBFEN |= BIT(0)|BIT(1);
    GPIOBDE |=  BIT(0)|BIT(1);
    GPIOBDIR &= ~(BIT(0) | BIT(1));  //CLK,DO output
    GPIOBDIR |= BIT(1);  //DI input
    GPIOBPU |= BIT(1);   //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = (SPI1_G1<<12);  //G1
#endif
    memset(&spi1_trans_vars, 0, sizeof(spi1_trans_vars));
    spi1_trans_vars.spi1_main_sta   = SPI1_M_CHK_PROC;
    // spi1_trans_vars.mic_frame       = SPI_AUDIO_FRAME;
    
#if CIRCULAR_BUF_EN
    circular_buf_init(&cbuf_spi1tx, (void *)spi1_tx_buf, SPI1_TXBUF_SIZE);
#endif
    dma_buf_init();

    SPI1_AUDIO_CS_INIT();
    SPI1BAUD = get_sysclk_nhz()/baud - 1;//SPI主机初始化时钟
    spi1_register_init(SPI_CHK_REV);
    // spi1_kick(SPI_IRQ_REV, spi1_rx_buf, SPI1_RXBUF_SIZE);//SPI IRQ 接收设置
    // sys_irq_init(IRQ_SPI_VECTOR, 0, spi1_irq_handler);
    printf("[spi1_master_init SPI1CON: 0x%X]\n", SPI1CON);
}

AT(.text.spiflash1_drv)
void spi1_salve_init(void)
{
#if SPI3WIREEN
    //SPI1_G1: CLK_PB0, DI_PB2, DO_PB1,   CS_PA9
	GPIOBFEN |= BIT(1)|BIT(2)|BIT(0);
    GPIOBDE  |= BIT(1)|BIT(2)|BIT(0);
    GPIOBDIR &= ~BIT(1);                //DO output
    GPIOBDIR |= BIT(0)|BIT(2);
    GPIOBPU  |= BIT(0)|BIT(2);
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = (SPI1_G1<<12);
#else
    //SPI1_G1: CLK_PB0, DIO_PB1
	GPIOBFEN |= BIT(0)|BIT(1);
    GPIOBDE  |= BIT(0)|BIT(1);
    GPIOBDIR |= BIT(0)|BIT(1);
    GPIOBPU  |= BIT(0)|BIT(1);
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = (SPI1_G1<<12);
#endif
    memset(&spi1_trans_vars, 0, sizeof(spi1_trans_vars));
    spi1_trans_vars.spi1_main_sta   = SPI1_S_CHK_PROC;
    // spi1_trans_vars.mic_frame       = SPI_AUDIO_FRAME;

#if CIRCULAR_BUF_EN
    circular_buf_init(&cbuf_spi1tx, (void *)spi1_tx_buf, SPI1_TXBUF_SIZE);
#endif

    dma_buf_init();
    SPI1_AUDIO_CS_INIT();//cs input
    cs_edge_isr_init();

#if SPI1_MISO_CTRL
    spi1_miso_disable();
#endif

    spi1_register_init(SPI_CHK_REV);
    #if SPI1DMAINT
    SPI1CON |= BIT(SPI_RXSEL);
    #endif
    // sys_irq_init(IRQ_SPI_VECTOR, 0, spi1_irq_handler);
    // spi1_kick(SPI_CHK_REV, spi1_rx_buf[dbuf_spi1rx.rxbuf_rear], SPI1_RXBUF_SIZE);  //SPI IRQ 接收设置
    printf("[spi1_salve_init SPI1CON: 0x%X]\n", SPI1CON);
}

AT(.text.spi1_cs)
void cs_edge_isr_init(void)
{
    gpio_t *g = &spi1_audio_cs_gpio;
    g->sfr[GPIOxFEN] &= ~g->pin;    //set to GPIO
    g->sfr[GPIOxDE]  |=  g->pin;    //set to digital IO
    g->sfr[GPIOxDIR] |=  g->pin;    //set to input

    PORTINTEDG |= BIT(EDGE_INT_PORT);//下降沿触发
    PORTINTEN |= BIT(EDGE_INT_PORT);//interrupt enable

    WKUPCPND = (BIT(22) | BIT(23));//clear pending
    WKUPEDG |= BIT(6);//ports falling edge
    // WKUPEDG &= ~BIT(7);//ports rising edge
    // WKUPCON |= BIT(6) | BIT(7) | BIT(16);//wakeup 6and7 enable, interrupt enable
    WKUPCON |= BIT(6) | BIT(16);//wakeup 6 enable, interrupt enable
    NMICON |= BIT(IRQ_PORT_VECTOR);
    sys_irq_init(IRQ_PORT_VECTOR, 1, cs_edge_isr);
}

AT(.text.spiflash1_drv)
void spi1_register_init(u8 in_mode)
{
    switch(in_mode){
        case SPI_DMA_REV://DMA
            //DMA接收SPI1控制寄存器设置：使能；从模式；中断使能；
            //DMA和两线模式下需要设置接收标记位
            if(SPI1_MASTER_EN){
                SPI1CON &= ~BIT(SPISM);
                // SPI1CON =  BIT(SPIEN);
                SPI1CON =  BIT(SPIEN) | BIT(SPI_SMPS);//| BIT(SPIIE) | BIT(SPI_RXSEL);
            }else{
                SPI1CON =  BIT(SPIEN) | BIT(SPIIE) | BIT(SPI_RXSEL) | BIT(SPISM) | BIT(SPI_SMPS);
            }
            #if SPI2WIREEN
            	SPI1CON |= BIT(SPI_BUSMODE_LOWBIT);				//两线模式;SPI_BUSMODE_LOWBIT
            #endif
            // sys_irq_init(IRQ_SPI_VECTOR, 0, spi1_irq_handler);
            break;
        case SPI_IRQ_REV://中断
            //中断接收SPI1控制寄存器设置：使能；从模式；中断使能;
            //三线模式下无需设置收发标记
            #if SPI3WIREEN
                if(SPI1_MASTER_EN){
                    // SPI1CPND = BIT(16);
                    SPI1CON &= ~BIT(SPISM);
                    SPI1CON =  BIT(SPIEN);
                }else{
                    SPI1CPND = BIT(16);
                    SPI1CON =  BIT(SPIEN) | BIT(SPIIE) | BIT(SPISM) ;
                }
            #elif SPI2WIREEN
            	SPI1CON =  BIT(SPI_RXSEL) |BIT(SPI_BUSMODE_LOWBIT);
            #endif
            // sys_irq_init(IRQ_SPI_VECTOR, 0,spi1_irq_handler);
            break;
        case SPI_CHK_REV://查询
            if(SPI1_MASTER_EN){
                SPI1CON =  BIT(SPIEN);
            }else{
                SPI1CON =  BIT(SPIEN) | BIT(SPISM);
            }
            break;
        default:
            break;
    }
}

//SPI1 cs
AT(.text.spiflash1_drv)
void spi1_audio_cs_init(void)
{
    gpio_t *g = &spi1_audio_cs_gpio;
    gpio_cfg_init(g, SPI1_AUDIO_CS_IO);
    if(SPI1_MASTER_EN){
        if (g->sfr) {
            g->sfr[GPIOxFEN] &= ~g->pin;
            g->sfr[GPIOxDE]  |=  g->pin;
            g->sfr[GPIOxDIR] &= ~g->pin;
            g->sfr[GPIOxSET]  =  g->pin;
        }
    }else{
        if (g->sfr) {
            g->sfr[GPIOxFEN] &= ~g->pin;
            g->sfr[GPIOxDE]  |=  g->pin;
            g->sfr[GPIOxDIR] |=  g->pin;
            // g->sfr[GPIOxPU]  |=  g->pin;
        }
    }
}
AT(.com_text.spi1_audio)
void spi1_audio_cs_en(void)
{
    gpio_t *g = &spi1_audio_cs_gpio;
    if (!g->sfr) {
        spi1_audio_cs_init();
    }
    if(SPI1_MASTER_EN){
        g->sfr[GPIOxCLR]  =  g->pin;
    }
}
AT(.com_text.spi1_audio)
void spi1_audio_cs_dis(void)
{
    gpio_t *g = &spi1_audio_cs_gpio;
    if(SPI1_MASTER_EN){
        g->sfr[GPIOxSET]  =  g->pin;
    }
}
/*
AT(.com_text.spi1_audio)
u8 spi1_cs_get(void)
{
    u8 status = GPIOA & BIT(3);
    return status;
}
*/

#if SPI1_MISO_CTRL
AT(.com_text.spi1_miso)
void spi1_miso_disable(void)
{
    GPIOBFEN &= ~BIT(1);
    GPIOBDE  &= ~BIT(1);
    GPIOBDIR |=  BIT(1);
}

AT(.com_text.spi1_miso)
void spi1_miso_enable(void)
{
    GPIOBFEN |=  BIT(1);
    GPIOBDE  |=  BIT(1);
    GPIOBDIR &= ~BIT(1);
}
#endif

/*
//SPI1单字节发送
AT(.text.spiflash1_drv)
static void spi1_sendbyte(u8 data)
{
    SPI1CON &= ~BIT(4);                         //TX
    SPI1BUF = data;
    while (!(SPI1CON & BIT(SPIPND)));           //Wait pending
}
*/

// AT(.com_rodata.bat)
// const char spi_test[] = "%X ";
#if CIRCULAR_BUF_EN
//SPI主机: n字节发送, n字节接收(需要SPI从机配合进行n字节接收,n字节发送)
AT(.com_text.spi1_audio)
void spi1_send_buf_do(u8 *txbuf, u8 *rxbuf, u32 len)
{
    #if !SPI1DMAINT
    u32 i = 0;
    u32 tmp_kick_cnt = len;
    #endif
    u32 templen = len;
    u8 *tx_pt = txbuf;
    u8 *rx_pt = rxbuf;
    SPI1_AUDIO_CS_EN();
    delay_us(5);                                //配合SPI从机的响应速度进行调整延时
    // GLOBAL_INT_DISABLE();
    if(!is_rx_dmabuf_full()){
        dbuf_spi1rx.rxbuf_rear = (dbuf_spi1rx.rxbuf_rear + 1) % SPI_RX_MAX;
    #if !SPI1DMAINT
        // SPI1CON &= ~BIT(4);
        for(i = 0; i < templen; i++){           //TX
            SPI1BUF = *tx_pt;
            while (!(SPI1CON & BIT(SPIPND)));
            tx_pt++;
        }

        // SPI1CON |= BIT(4);
        SPI1BUF = 0xff;
        while(tmp_kick_cnt){                    //RX
            if(SPI1CON & BIT(SPIPND)){
                *rx_pt = SPI1BUF;
                // my_printf(spi_test, *rxbuf);
                SPI1BUF = 0xff;
                rx_pt++;
                tmp_kick_cnt--;
            }
            while (!(SPI1CON & BIT(SPIPND)));
        }
    #else
        SPI1DMAADR = DMA_ADR(tx_pt);
        SPI1DMACNT = templen;
        while (!(SPI1CON & BIT(SPIPND)));//TX

        SPI1CON |= BIT(SPI_RXSEL);
        delay_us(2);
        SPI1DMAADR = DMA_ADR(rx_pt);
        SPI1DMACNT = templen;
        while (!(SPI1CON & BIT(SPIPND)));//RX
        SPI1CON &= ~BIT(SPI_RXSEL);
    #endif
        dbuf_spi1rx.rx_m_succ = 1;
    }
    // GLOBAL_INT_RESTORE();
    SPI1_AUDIO_CS_DIS();
}
#endif

/*
//SPI DMA发送 调用之前需要确定函数spi1_tx_init已初始化
AT(.text.spiflash1_drv)
static void spi1_dma_sendbytes(void *data, u32 len)
{
    SPI1_AUDIO_CS_EN();
    if(0 == SPI1_MASTER_EN){
        // delay_us(50);
    }
    SPI1DMAADR = DMA_ADR(data);
    SPI1DMACNT = len;
    delay_us(100);//wait data to send
    SPI1_AUDIO_CS_DIS();
}
*/

/*!**************************************************************************************************
\brief          循环缓存buf数据接口(满后等待)
\param[in]      No param_in
\param[out]     No param_out
\return         No return
****************************************************************************************************/
#if CIRCULAR_BUF_EN
#define EF_PRINTF  0
#define WR_TIMES_100MS  0

#if WR_TIMES_100MS
AT(.com_text.iis_ext_cst)
const char spkr100[] = "spkrpro: [%d]  sams [%d] ----";
AT(.com_text.iis_ext_cst)
const char spkw100[] = " spkwpro: [%d]  sams [%d]  chmode[%d]\n";

u16  spkrtick = 0;
u16  spkwtick = 0;
u16  spkrcnt = 0;
u16  spkwcnt = 0;
#endif // WR_TIMES_100MS

#if EF_PRINTF
AT(.com_text.iis_ext_cst)
const char buf_f[] = "f ";
AT(.com_text.iis_ext_cst)
const char buf_e[] = "e ";
#endif

/*----------------------------------------------------------------------------*/
AT(.com_text.cir_write)
bool spk_write_to_cirbuf_process(void* buf, u16 length)
{
    if(circular_buf_is_write_able(&cbuf_spi1tx, length)){
        my_printf("write buf\n");
        circular_buf_write(&cbuf_spi1tx, (u8 *)buf, length);  //写mic数据
        return true;
    }else{
        #if EF_PRINTF
        printf(buf_f);
        #endif
        return false;
    }
}

AT(.com_text.cir_read)
bool spk_read_by_cirbuf_process(u8 *output, u16 length)
{
    if(cbuf_spi1tx.data_len >= SPI_AUDIO_FRAME){
        circular_buf_read(&cbuf_spi1tx, output, length);  //读cbuf数据
        return true;
    }else{
        #if EF_PRINTF
        printf(buf_e);
        #endif
        return false;
    }
}

/*----------------------------------------------------------------------------*/
//循环队列缓冲判空
AT(.com_text.rx_read)
u8 is_rx_dmabuf_empty(void)
{
    if(dbuf_spi1rx.rxbuf_rear != dbuf_spi1rx.rxbuf_front){
        //my_printf("1y");
        return 0;
    }else{
        //my_printf("1n");        //缓冲区空
        return 1;
    }
}
#endif

#if CIRCULAR_BUF_EN
//循环队列缓冲判满
AT(.com_text.rx_read)
u8 is_rx_dmabuf_full(void)
{
    if(dbuf_spi1rx.rxbuf_front != (dbuf_spi1rx.rxbuf_rear + 1) % SPI_RX_MAX){
        // my_printf("2y");
        return 0;
    }else{
        // my_printf("2n");         //缓冲区满
        return 1;
    }
}
#endif

AT(.text.spiflash1_drv)
void dma_buf_init(void)
{
    memset(&dbuf_spi1rx, 0, sizeof(dbuf_spi1rx));
    memset(spi1_rx_buf, 0, SPI1_RXBUF_SIZE);
    // for(u8 i = 0; i < SPI_RX_MAX ; i++){
    //     memset(spi1_rx_buf[i], 0, SPI1_RXBUF_SIZE);
    // }
}

/*----------------------------------------------------------------------------*/


/*!**************************************************************************************************
\brief          SPI1 irq\DMA\check 接收
\param[in]      No param_in
\param[out]     No param_out
\return         No return
****************************************************************************************************/
// AT(.com_text.spi_info)
// const char str_spi1[] = "+,%X\n";
#if CIRCULAR_BUF_EN
AT(.com_text.timer) //FIQ
void spi1_irq_handler(void)     //SPI1中断处理函数
{
    if(SPI1CON & BIT(SPIPND)){
        SPI1CPND = BIT(16);
        // my_printf(str_spi1, 0);
        //u8 rev;
        //rev = SPI1BUF;
        // my_printf(str_spi1, rev);
        //SPI1BUF = 0xFF;
        spi1_trans_vars.bo_irq_done = false;
    }
}

// AT(.text.spiflash1_drv)
AT(.com_text.spi1_kick)
void spi1_irq_read(void)    // SPI1三线中断接收
{
    SPI1CPND = BIT(16);
    // printf("WATI IRQ INT..\n");
    if(!SPI1_MASTER_EN){
        SPI1BUF = 0xFF;
    }
    // SPI1BUF = 0xff;
    //while(1);
}

// SPI1三线查询接收
AT(.com_text.spi1_kick)
void spi1_check_read(void)
{
    SPI1BUF = 0xff;
    while(!dbuf_spi1rx.rx_s_succ){
        if(SPI1CON & BIT(SPIPND)){
            //u8 r_dat = SPI1BUF;
            SPI1BUF = 0x55;
            dbuf_spi1rx.rx_cnt++;
            if(dbuf_spi1rx.rx_cnt >= SPI_KICK_NUM){
                dbuf_spi1rx.rx_cnt = 0;
                // dbuf_spi1rx.rx_s_succ = 1;
            }
            // printf("[0x%X,0x%X]",r_dat,SPI1CON);
        }
    }
}

AT(.com_text.spi1_kick)
void spi1_dma_read(void *buf, u32 len)  // SPI1三线DMA接收
{
	//printf("SPI1CON = 0x%X\n",SPI1CON);
    SPI1DMAADR = DMA_ADR(buf); //dac_obuf spi_recv_buf
    //printf("SPI1DMAADR = 0x%X,0x%X\n",SPI1DMAADR,spi_recv_buf);
    SPI1DMACNT = len;           //接收的时候需要kick start

    // printf("WATI DMA INT..\n");
    // SPI1BUF = 0xff;
    SPI1CPND = BIT(16);         //clear pending
}


//SPI1 接收接口函数
//mode:SPI_CHK_REV :查询接收
//SPI_IRQ_REV :中断单字节接收
//SPI_DMA_REV :中断DMA接收
// AT(.text.spiflash1_drv)
AT(.com_text.spi1_kick)
void spi1_kick(u8 mode, void *in_buf, u32 len)
{
    switch(mode){
        case SPI_DMA_REV: spi1_dma_read(in_buf,len);    //DMA
            break;
        case SPI_IRQ_REV: spi1_irq_read();              //中断
            break;
        case SPI_CHK_REV: spi1_check_read();            //查询
            break;
        default:
            break;
    }
}

//SPI主从机数据收发与命令解码主循环
AT(.text.spiflash1_drv)
void spi1_main_sta_switch(E_SPI1_STA e_sta)
{
    if(e_sta != spi1_trans_vars.spi1_main_sta){
        spi1_trans_vars.spi1_main_sta = e_sta;
    }
}

#define spi1_print_test     1
const char str_tx_info[] = "SPITX: M/S = %d, tx_speed: %dbyte/s, rx_speed: %dbyte/s\n";
const char str_rx_info[] = "SPIRX: M/S = %d, tx_speed: %dbyte/s, rx_speed: %dbyte/s\n";

AT(.text.spiflash1_drv)
void spi1_main_process(void)
{
#if (SPI1_MASTER_EN)
    switch(spi1_trans_vars.spi1_main_sta){
        case SPI1_M_CHK_PROC:
            if(flag_spk_data_kick){
                flag_spk_data_kick = false;
                if(spk_read_by_cirbuf_process(tmp_send_buf, SPI_AUDIO_FRAME)){
                    my_printf("spi master test\n");
                    #if 0//简单数据测试
                    memcpy(tmp_send_buf, master_tx_cmd_01, SPI_KICK_NUM);
                    memset(tmp_recv_buf, 0, sizeof(tmp_recv_buf));
                    #endif
                    spi1_send_buf_do(tmp_send_buf, tmp_recv_buf, SPI_KICK_NUM);
                    #if 0//SPI_TX_RX_TEST
                    my_print_r(tmp_send_buf, SPI_KICK_NUM);
                    #endif
                    spi1_trans_vars.spi1_send_cnt += (SPI_KICK_NUM/2);//test
                }else{
                    my_printf("E1 \n");
                    // spi1_send_buf_do((u8 *)master_fail_cmd_02, SPI_KICK_NUM);
                }
                spi1_trans_vars.wait_tick = tick_get();
            }
            break;
        case SPI1_M_RX:
            break;
        case SPI1_M_CMD:
            break;
        case SPI1_M_IDLE:
            break;
        default:
            my_printf("master error process\n");
            break;
    }
    if(1 == dbuf_spi1rx.rx_m_succ){
        dbuf_spi1rx.rx_m_succ = 0;
        dbuf_spi1rx.rxbuf_front = (dbuf_spi1rx.rxbuf_front + 1) % SPI_RX_MAX;
    }
    //printf test
    if(tick_check_expire(spi1_trans_vars.main_proc_tick, 1000)){
        spi1_trans_vars.main_proc_tick = tick_get();
        // my_printf(str_tx_info, xcfg_cb.spi1_slave_master, spi1_trans_vars.spi1_send_cnt, spi1_trans_vars.spi1_recv_cnt);
        spi1_trans_vars.spi1_send_cnt = 0;
        spi1_trans_vars.spi1_recv_cnt = 0;
    }

#else
    switch(spi1_trans_vars.spi1_main_sta){
        case SPI1_S_CHK_PROC://deafult RX
            if(flag_spk_data_kick){
                flag_spk_data_kick = false;
                if(spk_read_by_cirbuf_process(tmp_send_buf, SPI_AUDIO_FRAME)){
                    my_printf("spi slave test\n");
                    #if 0//简单数据测试
                    memcpy(tmp_send_buf, slave_tx_cmd_08, SPI_KICK_NUM);//test
                    #endif
                    spi1_trans_vars.spi1_send_cnt += (SPI_KICK_NUM/2);//test
                }else{
                    // my_printf("slave mic frame fail");
                }
            }
            break;
        case SPI1_S_RX:
            break;
        case SPI1_S_CMD:
            break;
        case SPI1_S_IDLE:
            break;
        default:
            my_printf("slave error process\n");
            break;
    }
    if(1 == dbuf_spi1rx.rx_s_succ){
        dbuf_spi1rx.rx_s_succ = 0;
        #if 0//SPI_TX_RX_TEST
        my_print_r(tmp_recv_buf,  SPI_KICK_NUM);
        memset(tmp_recv_buf, 0, sizeof(tmp_recv_buf));
        #endif
        dbuf_spi1rx.rxbuf_front = (dbuf_spi1rx.rxbuf_front + 1) % SPI_RX_MAX;
    }
    //printf test
    if(tick_check_expire(spi1_trans_vars.main_proc_tick, 1000)){
        spi1_trans_vars.main_proc_tick = tick_get();
        // my_printf(str_rx_info, xcfg_cb.spi1_slave_master, spi1_trans_vars.spi1_send_cnt, spi1_trans_vars.spi1_recv_cnt);
        spi1_trans_vars.spi1_send_cnt = 0;
        spi1_trans_vars.spi1_recv_cnt = 0;
    }
#endif
}
#endif

void spi1_audio_init(void)
{
    if(SPI1_MASTER_EN){
        printf("spi_master_init\n");
        set_sys_clk(SYS_120M);                  //SPI波特率越高,系统时钟要相应的提高
#if !SPI1DMAINT
        spi1_master_init(4000000);
#else
        spi1_master_init(14000000);
#endif
    }else{
        printf("spi_slave_init\n");
        set_sys_clk(SYS_120M);
        spi1_salve_init();
    }
}

//SPI主机: n字节发送, n字节接收(需要SPI从机配合进行n字节接收,n字节发送)
AT(.com_text.spi1_audio)
void spi1_send_buf_do(u8 *txbuf, u8 *rxbuf, u32 len)
{
    #if !SPI1DMAINT
    u32 i = 0;
    #endif
    u32 templen = len;
    u8 *tx_pt = txbuf;
    u8 *rx_pt = rxbuf;
    SPI1_AUDIO_CS_EN();
    delay_us(10);                                //配合SPI从机的响应速度进行调整延时,从机主频越慢,延时要越长

    // if(!is_rx_dmabuf_full()){
    #if !SPI1DMAINT
        #if SPI1_FULL_DUPLEX
        for(i = 0; i < templen; i++){
            SPI1BUF = *tx_pt;
            while (!(SPI1CON & BIT(SPIPND)));
            *rx_pt = SPI1BUF;
            tx_pt++;
            rx_pt++;
        }
        #else
        // SPI1CON &= ~BIT(4);
        for(i = 0; i < templen; i++){           //TX
            SPI1BUF = *tx_pt;
            while (!(SPI1CON & BIT(SPIPND)));
            tx_pt++;
        }

        // SPI1CON |= BIT(4);
        SPI1BUF = 0xff;
        while(templen){                    //RX
            if(SPI1CON & BIT(SPIPND)){
                *rx_pt = SPI1BUF;
                // my_printf(spi_test, *rxbuf);
                SPI1BUF = 0xff;
                rx_pt++;
                templen--;
            }
            while (!(SPI1CON & BIT(SPIPND)));
        }
        #endif
    #else
        SPI1DMAADR = DMA_ADR(tx_pt);
        SPI1DMACNT = templen;
        while (!(SPI1CON & BIT(SPIPND)));//TX

        delay_us(100);                           //配合SPI从机的DMA处理时间调整延时,SPI数据越多延时要越长
        SPI1CON |= (BIT(SPI_RXSEL) | BIT(10));
        SPI1DMAADR = DMA_ADR(rx_pt);
        SPI1DMACNT = templen;
        while (!(SPI1CON & BIT(SPIPND)));//RX
        SPI1CON &= ~(BIT(SPI_RXSEL) | BIT(10));
    #endif
        dbuf_spi1rx.rx_m_succ = 1;
    // }
    SPI1_AUDIO_CS_DIS();
}

//SPI1从机n字节接收->n字节发送
AT(.com_text.spi1_cs) FIQ
void cs_edge_isr(void)
{
    u32 templen = SPI1_RXBUF_SIZE;
    u32 tmp_kick_cnt = SPI1_RXBUF_SIZE;
    #if !SPI1DMAINT
    u32 i = 0;
    #endif
    // if(WKUPEDG & BIT(23)){                      //rise
    //     WKUPCPND = BIT(23);
    // }
    if(WKUPEDG & BIT(22)){                      //fall
        WKUPCPND = BIT(22);
#if SPI1_MISO_CTRL
        spi1_miso_enable();
#endif
        if(!SPI1_MASTER_EN){                    //主机cs拉低kick从机
        #if !SPI1DMAINT
            #if SPI1_FULL_DUPLEX
            SPI1BUF = spi1_tx_buf[0];
            while(tmp_kick_cnt){
                if(SPI1CON & BIT(SPIPND)){
                    spi1_rx_buf[i] = SPI1BUF;
                    i++;
                    if(i < templen){
                        SPI1BUF = spi1_tx_buf[i];
                    }
                    tmp_kick_cnt--;
                }
            }
            dbuf_spi1rx.rx_s_succ = 1;
            #else
            // SPI1CON |= BIT(4);
            SPI1BUF = 0xff;
            while(tmp_kick_cnt){                //RX
                // WDT_CLR();
                if(SPI1CON & BIT(SPIPND)){
                    spi1_rx_buf[i] = SPI1BUF;
                    SPI1BUF = 0xff;
                    i++;
                    tmp_kick_cnt--;
                }
            }
            dbuf_spi1rx.rx_s_succ = 1;

            // SPI1CON &= ~BIT(4);
            for(i = 0; i < templen; i++){       //TX
                SPI1BUF = spi1_tx_buf[i];
                while (!(SPI1CON & BIT(SPIPND)));
            }
            #endif
        #else
            SPI1DMAADR = DMA_ADR(spi1_rx_buf);
            SPI1DMACNT = tmp_kick_cnt;
            SPI1CON |= (BIT(4)); 
            while (!(SPI1CON & BIT(SPIPND)));   //RX
            dbuf_spi1rx.rx_s_succ = 1;
            
            SPI1CON |= BIT(10);
            SPI1CON &= ~BIT(SPI_RXSEL);
            SPI1DMAADR = DMA_ADR(spi1_tx_buf);
            SPI1DMACNT = templen;
            while (!(SPI1CON & BIT(SPIPND)));//TX
            SPI1CON |= BIT(SPI_RXSEL);
            SPI1CON &= ~BIT(10);
        #endif
        }
#if SPI1_MISO_CTRL
        spi1_miso_disable();
#endif
    }
}

AT(.text.spiflash1_drv)
void spi1_pcm_send_proc(u8 *txbuf, u32 len)
{
#if SPI1_AUDIO_TEST_EN
    u16 crc_tx_cal = 0, crc_rx_get = 0, crc_rx_chk = 0;
    spi1_trans_vars.spi1_send_cnt++;
    memcpy(spi1_tx_buf, txbuf, len);
    // memset(spi1_tx_buf, 0x5, (SPI1_RXBUF_SIZE - SPI1_CRC_SIZE));//给个常数进行测试
    crc_tx_cal = calc_crc(spi1_tx_buf, (SPI1_TXBUF_SIZE-SPI1_CRC_SIZE), CRC_SEED);              //计算发送数据crc
    spi1_tx_buf[SPI1_TXBUF_SIZE-2] = crc_tx_cal & 0xFF;
    spi1_tx_buf[SPI1_TXBUF_SIZE-1] = (crc_tx_cal >> 8) & 0xFF;
    spi1_send_buf_do(spi1_tx_buf, spi1_rx_buf, SPI1_TXBUF_SIZE);

    crc_rx_chk = calc_crc(spi1_rx_buf, (SPI1_RXBUF_SIZE-SPI1_CRC_SIZE), CRC_SEED);              //校验接收数据crc
    crc_rx_get = spi1_rx_buf[SPI1_RXBUF_SIZE-2] | (spi1_rx_buf[SPI1_RXBUF_SIZE-1] << 8);

    my_printf("cnt: %d, crc_tx_cal: 0x%X, crc_rx_chk: 0x%X, crc_rx_get: 0x%X\n", spi1_trans_vars.spi1_send_cnt, crc_tx_cal, crc_rx_chk, crc_rx_get);
#endif
}

AT(.text.spiflash1_drv)
void spi1_main_process(void)
{
#if SPI1_AUDIO_TEST_EN
    u16 crc_tx_cal = 0, crc_rx_get = 0, crc_rx_chk = 0;
    if(SPI1_MASTER_EN){                 //SPI主机处理

    }else{                              //SPI从机处理
        if(1 == dbuf_spi1rx.rx_s_succ){
            spi1_trans_vars.spi1_recv_cnt++;
            dbuf_spi1rx.rx_s_succ = 0;
            crc_rx_chk = calc_crc(spi1_rx_buf, (SPI1_RXBUF_SIZE - SPI1_CRC_SIZE), CRC_SEED);    //校验接收数据crc
            crc_rx_get = spi1_rx_buf[SPI1_RXBUF_SIZE-2] | (spi1_rx_buf[SPI1_RXBUF_SIZE-1] << 8);

            // memset(spi1_tx_buf, 0x5, (SPI1_RXBUF_SIZE - SPI1_CRC_SIZE));                     //给个常数进行测试
            memcpy(spi1_tx_buf, spi1_test_table, (SPI1_RXBUF_SIZE - SPI1_CRC_SIZE));            //给个数组进行测试
            crc_tx_cal = calc_crc(spi1_tx_buf, (SPI1_TXBUF_SIZE-SPI1_CRC_SIZE), CRC_SEED);      //计算发送数据crc
            spi1_tx_buf[SPI1_TXBUF_SIZE-2] = crc_tx_cal & 0xFF;
            spi1_tx_buf[SPI1_TXBUF_SIZE-1] = (crc_tx_cal >> 8) & 0xFF;

            my_printf("cnt: %d, crc_rx_get: 0x%X, crc_rx_chk: 0x%X, crc_tx_cal: 0x%X\n", spi1_trans_vars.spi1_recv_cnt, crc_rx_get, crc_rx_chk, crc_tx_cal);
        }
    }
#endif
}

#endif

