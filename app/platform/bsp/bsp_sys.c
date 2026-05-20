#include "include.h"

xcfg_cb_t xcfg_cb;

uint8_t cfg_spiflash_speed_up_en    = SPIFLASH_SPEED_UP_EN;
uint8_t cfg_flash_dual_read         = FLASH_DUAL_READ;
uint8_t cfg_uart0_baud_update_en    = (!USER_UART0_EN);

void sd_detect(void);
void tbox_uart_isr(void);
void testbox_init(void);
bool exspiflash_init(void);
bool sdcard_music_init(void);
void ledseg_6c6s_clr(void);
void bsp_stretch_init(void);
void huart_dump_init(void);
void freqdet_init(void);
bool drc_v3_init(const void *bin, int bin_size);
void ir_rx_110ms_isr(void);
void sys_compiler(void);

AT(.text.bsp.sys)
void bsp_clr_mute_sta(void)
{
    if (sys_cb.mute) {
        sys_cb.mute = 0;
// #if DAC_DNR_EN
//         dac_dnr_set_sta(sys_cb.dnr_sta);
// #endif
    }
}

AT(.text.bsp.sys)
bool bsp_get_mute_sta(void)
{
    return sys_cb.mute;
}

#if MUSIC_SDCARD_EN
AT(.com_text.detect)
void sd_detect(void)
{
    if ((!is_sd_support()) || (IS_DET_SD_BUSY())) {
        return;
    }
    if (SD_IS_ONLINE()) {
        if (dev_online_filter(DEV_SDCARD)) {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
//            printf("sd insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD)) {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
//            printf("sd remove\n");
        }
    }
}
#endif // MUSIC_SDCARD_EN

#if MUSIC_SDCARD1_EN
AT(.com_text.detect)
void sd1_detect(void)
{
    if ((!is_sd1_support()) || (IS_DET_SD1_BUSY())) {
        return;
    }
    if (SD1_IS_ONLINE()) {
        if (dev_online_filter(DEV_SDCARD1)) {
            sd1_insert();
            msg_enqueue(EVT_SD1_INSERT);
//            printf("sd1 insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD1)) {
            sd1_remove();
            msg_enqueue(EVT_SD1_REMOVE);
//            printf("sd1 remove\n");
        }
    }
}
#endif // MUSIC_SDCARD_EN

void sys_ram_info_dump(void)
{
    extern u32 __comm_size;
    extern u32 __bss_size;
    extern u32 __bss1_size;

    extern u32 __data_vma;
    extern u32 __data_ram_size;
    extern u32 __data1_vma;
    extern u32 __data1_ram_size;
    extern u32 __comm_vma;
    extern u32 __comm_ram_size;

    u32 comm_free  = (u32)&__comm_ram_size - (u32)&__comm_size;
    u32 data_free = (u32)&__data_ram_size - (u32)&__bss_size;
    u32 data1_free = (u32)&__data1_ram_size - (u32)&__bss1_size;
    u32 ram_total = comm_free + data_free + data1_free;

    printf("ram_info:\n");
    printf("data:  remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", data_free,  (u32)&__data_ram_size,  (u32)&__data_vma, (u32)&__data_vma + (u32)&__data_ram_size);
    printf("data1: remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", data1_free,  (u32)&__data1_ram_size,  (u32)&__data1_vma, (u32)&__data1_vma + (u32)&__data1_ram_size);
    printf("comm:  remain:%6d(Byte), total:%6d(Byte), range:[0x%x - 0x%x]\n", comm_free, (u32)&__comm_ram_size, (u32)&__comm_vma, (u32)&__comm_vma + (u32)&__comm_ram_size);
    printf("remain_total: %dK\n",ram_total / 1024);
}

#if DEBUG_SPI_LOAD
extern void set_dump(void (*dump_err_func)(uint err_num, ...), void (*dump_info_func)(uint info_num, ...));
AT(.com_rodata.band)
const char strload[] = "->";

AT(.com_text.debug)
void debug_spi_load(uint info_num, ...)
{
    va_list param;
    va_start(param, info_num);
    if (1 == info_num) {
        my_printf(strload);
    }
}

void debug_set(void)
{
    set_dump(NULL, debug_spi_load);
}
#endif

AT(.text.bsp.sys.init)
static void rtc_32k_configure(void)
{
    u32 temp = RTCCON0;
    temp &= ~BIT(6);                            //use RTC internal 32K osc
    temp |=  BIT(2);                            //RTC output clock to core enable
    temp = (temp & ~(0x3 << 8)) | BIT(8);       //CLK2M in RTC power domain source select RC2M_RTC
#if ALARM_WAKEUP_EN
    temp |= BIT(0);                             //RC enable
#endif
    RTCCON0 = temp;
}

AT(.text.bsp.sys.init)
bool rtc_init(void)
{
    u32 temp;
    rtc_32k_configure();
    sys_cb.rtc_first_pwron = 0;
    temp = RTCCON0;
    if (temp & BIT(7)) {
        temp &= ~BIT(7);                        //clear first poweron flag
        RTCCON0 = temp;
        sys_cb.rtc_first_pwron = 1;
        #if FUNC_CLOCK_EN
        rtc_clock_init();
        #endif // FUNC_CLOCK_EN
        printf("rtc 1st pwrup\n");
        return false;
    }
    return true;
}

//UART0打印信息输出GPIO选择，UART0默认G1(PA7)
void uart0_mapping_sel(void)
{
    //等待uart0发送完成
    if(UDETCON & BIT(0)) {
        while (!(UDETCON & BIT(8)));
    }
    GPIOBDE  &= ~BIT(9);
    GPIOBFEN &= ~BIT(9);
    GPIOBPU  &= ~BIT(9);
    FUNCMCON0 = (0xf << 12) | (0xf << 8);           //clear uart0 mapping

#if (UART0_PRINTF_SEL == PRINTF_PA0)
    GPIOADE  |= BIT(0);
    GPIOAPU  |= BIT(0);
    GPIOADIR |= BIT(0);
    GPIOAFEN |= BIT(0);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA0;
#elif (UART0_PRINTF_SEL == PRINTF_PA6)
    GPIOADE  |= BIT(6);
    GPIOAPU  |= BIT(6);
    GPIOADIR |= BIT(6);
    GPIOAFEN |= BIT(6);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PA6;
#elif (UART0_PRINTF_SEL == PRINTF_PB0)
    GPIOBDE  |= BIT(0);
    GPIOBPU  |= BIT(0);
    GPIOBDIR |= BIT(0);
    GPIOBFEN |= BIT(0);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB0;
#elif (UART0_PRINTF_SEL == PRINTF_PB1)
    GPIOBDE  |= BIT(1);
    GPIOBPU  |= BIT(1);
    GPIOBDIR |= BIT(1);
    GPIOBFEN |= BIT(1);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB1;
#elif (UART0_PRINTF_SEL == PRINTF_PB2)
    GPIOBDE  |= BIT(2);
    GPIOBPU  |= BIT(2);
    GPIOBDIR |= BIT(2);
    GPIOBFEN |= BIT(2);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB2;
#elif (UART0_PRINTF_SEL == PRINTF_PB9)
    GPIOBDE  |= BIT(9);
    GPIOBPU  |= BIT(9);
    GPIOBDIR |= BIT(9);
    GPIOBFEN |= BIT(9);
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_PB9;
#endif
#if HUART_EN && HUART_PRINTF_EN
    bsp_huart_init();
    HSUT0CON &= ~BIT(0);                    //DIS RX_EN
    u_huart_tx_set_dir();
    my_printf_init(bsp_huart_putchar_byte); //将所有打印printf映射到huart
#endif
}

AT(.text.bsp.sys.init)
void bsp_compiler(void)
{
    sys_compiler();
}

#if MUSIC_SDCARD_EN
AT(.com_text.detect)
void sd_soft_cmd_detect(u32 check_ms) //check_ms 时间间隔检测一次.  //主循环中执行检测.
{
    static u32 check_ticks = 0;
//    if (!SD_IS_SOFT_DETECT()) {  //配置工具中是否配置sd检测.
//        return;
//    }
    if (tick_check_expire(check_ticks, check_ms) || (0 == check_ticks)) {  //每隔100ms才检测一次.
        check_ticks = tick_get();
    }else {
        return;
    }
    if (sd_soft_detect()) {
        if (dev_online_filter(DEV_SDCARD)) {
            sd_insert();
            msg_enqueue(EVT_SD_INSERT);
            my_printf("\nsd soft insert\n");
        }
    } else {
        if (dev_offline_filter(DEV_SDCARD)) {
            sd_remove();
            msg_enqueue(EVT_SD_REMOVE);
            my_printf("\nsd soft remove\n");
        }
    }
}

AT(.text.bsp.sys.init)
void sd_soft_detect_poweron_check(void)  //开机检测
{
//    if (!SD_IS_SOFT_DETECT()) {  //配置工具中是否配置sd检测.
//        return;
//    }
    dev_delay_times(DEV_SDCARD, 1);  //检测到1次成功, 则认为SD在线.
    u8 i = 5;
    while(i--) {
        sd_soft_cmd_detect(0);
        if (dev_is_online(DEV_SDCARD)) {
            break;
        }
        delay_ms(10);
    }
    dev_delay_times(DEV_SDCARD, 3);
}
#endif

void mclr_l_init(u8 io_num)
{
    CLKGAT0 |=  BIT(15);                        //lp clken
    RTCCON0 |=  BIT(0);                         //RC enable
    WKUPCON  =  BIT(29) | ((io_num-1) << 24);   //设置resetIO
    // RTCCON3 |=  BIT(3);                      //切换到vlcore供电
    // RTCCON3 &= ~BIT(5);
    RTCCON12 |= (1 << 15) | (6 << 12);          //先开 bit15，4+6 ≈10s
    RTCCON12 &= ~(3 << 4);                      //GPIO 10s reset enable
}

void mclr_10s_rst_io_init(u8 io_num)
{
    if(io_num > IO_PA15){
        GPIOBDE  |= BIT(io_num-1-IO_PA15);
        GPIOBPU  |= BIT(io_num-1-IO_PA15);
        GPIOBDIR |= BIT(io_num-1-IO_PA15);
    } else {
        GPIOADE  |= BIT(io_num-1);
        GPIOAPU  |= BIT(io_num-1);
        GPIOADIR |= BIT(io_num-1);
    }
}

void mclr_reset_init(void)
{
    RTCCON12 |= ((3 << 2) | (3 << 4) | (3 << 6));
    if(MCLR_RESRT_MODE == 1){               //长按复位(任意IO)
        mclr_10s_rst_io_init(MCLR_10S_RST_IO);
        if(RTCCON10 & BIT(13)) {
            RTCCON12 |= (3 << 4);           //GPIO 10s reset disable
            RTCCON10 |= BIT(13);            //clear pnd
            CRSTPND  |= BIT(19);
            mclr_l_init(MCLR_10S_RST_IO);   //10s reset init
        } else {
            mclr_l_init(MCLR_10S_RST_IO);   //10s reset init
        }
        RTCCON12 |=  ((3 << 2) | (3 << 6));
    }else if(MCLR_RESRT_MODE == 2){         //短按复位(PB10)
        GPIOBDE  |= BIT(10);
        GPIOBPU  |= BIT(10);
        GPIOBDIR |= BIT(10);
        if(RTCCON10 & BIT(9)) {
            RTCCON10 |= BIT(9);             //clear pnd
        } else {
            // RTCCON3  |=  BIT(3);         //切换到vlcore供电
            // RTCCON3  &= ~BIT(5);
            RTCCON12 &= ~(3 << 2);          //PB10 reset enable
        }
        RTCCON12 |=  ((3 << 4) | (3 << 6));
    }else{
        GPIOAPU &= ~BIT(0);                 //取消硬件默认上拉
        GPIOBPU &= ~BIT(10);
    }
}

//系统启动最初的位置
void platform_main_insert(void)
{
    mclr_reset_init();
}

