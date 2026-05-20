#include "include.h"
#include "lcd_display.h"
#include "bsp_spi_audio.h"

#if LCD_DISPLAY_EN
LCD_GPIO_t lcd_gpio AT(.com_rodata.lcd);
uchar bmp_current_buf[BMP_BUF_LEN] AT(.com_rodata.lcd);
res_img_t res_img AT(.com_rodata.lcd);
static u32 bsp_lcd_ticks;

extern const pic_param pic_charge_table[RES_PIC_TBL_LEN];

//IO初始化
void lcd_io_init(void)
{
    gpio_t *g_rst = &lcd_gpio.rst;
    gpio_t *g_rs  = &lcd_gpio.rs;
    gpio_t *g_cs  = &lcd_gpio.cs;

    gpio_cfg_init(g_rst, LCD_RST_IO);
    if (g_rst->sfr) {
        g_rst->sfr[GPIOxFEN] &= ~g_rst->pin;
        g_rst->sfr[GPIOxDE]  |=  g_rst->pin;
        g_rst->sfr[GPIOxDIR] &= ~g_rst->pin;
        g_rst->sfr[GPIOxSET]  =  g_rst->pin;
    }
    gpio_cfg_init(g_rs, LCD_RS_IO);
    if (g_rs->sfr) {
        g_rs->sfr[GPIOxFEN] &= ~g_rs->pin;
        g_rs->sfr[GPIOxDE]  |=  g_rs->pin;
        g_rs->sfr[GPIOxDIR] &= ~g_rs->pin;
        g_rs->sfr[GPIOxSET]  =  g_rs->pin;
    }
    gpio_cfg_init(g_cs, LCD_CS_IO);
    if (g_cs->sfr) {
        g_cs->sfr[GPIOxFEN] &= ~g_cs->pin;
        g_cs->sfr[GPIOxDE]  |=  g_cs->pin;
        g_cs->sfr[GPIOxDIR] &= ~g_cs->pin;
        g_cs->sfr[GPIOxSET]  =  g_cs->pin;
    }
}

void spi1_init(u32 baud)
{
#if   SPIFLASH_MAP == SPI1MAP_G1
    //SPI1_G1: CLK_PB0, DIO_PB1,   //CS_PB3
    GPIOBFEN |=   BIT(0)|BIT(1);
    GPIOBDE  |=   BIT(0)|BIT(1);
    GPIOBDIR &= ~(BIT(0)|BIT(1));   //CLK,DO output
    GPIOBDIR |=   BIT(1);           //DI input
    GPIOBPU  |=   BIT(1);           //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G1;
#elif SPIFLASH_MAP == SPI1MAP_G2
    //SPI1_G2: CLK_PA14, DIO_PA15,   //CS_PB3
    GPIOAFEN |=   BIT(14)|BIT(15);
    GPIOADE  |=   BIT(14)|BIT(15);
    GPIOADIR &= ~(BIT(14)|BIT(15)); //CLK,DO output
    GPIOADIR |=   BIT(15);          //DI input
    GPIOAPU  |=   BIT(15);          //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G2;
#elif SPIFLASH_MAP == SPI1MAP_G3
    //SPI1_G3: CLK_PA6, DIO_PA7,   //CS_PB3
    GPIOAFEN |=   BIT(6)|BIT(7);
    GPIOADE  |=   BIT(6)|BIT(7);
    GPIOADIR &= ~(BIT(6)|BIT(7));   //CLK,DO output
    GPIOADIR |=   BIT(7);           //DI input
    GPIOAPU  |=   BIT(7);           //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G3;
#elif SPIFLASH_MAP == SPI1MAP_G4
    //SPI1_G4: CLK_PB8, DIO_PB9,   //CS_PB3
    GPIOBFEN |=   BIT(8)|BIT(9);
    GPIOBDE  |=   BIT(8)|BIT(9);
    GPIOBDIR &= ~(BIT(8)|BIT(9));   //CLK,DO output
    GPIOBDIR |=   BIT(9);           //DI input
    GPIOBPU  |=   BIT(9);           //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G4;
#elif SPIFLASH_MAP == SPI1MAP_G5
    //SPI1_G5: CLK_PB1, DIO_PB9,   //CS_PB3
    GPIOBFEN |=   BIT(1)|BIT(9);
    GPIOBDE  |=   BIT(1)|BIT(9);
    GPIOBDIR &= ~(BIT(1)|BIT(9));   //CLK,DO output
    GPIOBDIR |=   BIT(9);           //DI input
    GPIOBPU  |=   BIT(9);           //DI pull up 10K
    FUNCMCON1 = (0x0F<<12);
    FUNCMCON1 = SPI1MAP_G5;
#endif
    SPI1BAUD = get_sysclk_nhz()/baud - 1;   //sys 24M.
    SPI1CON =  0x01 | (1<<2) | (0<<5) | (0<<10);
}

AT(.text.lcd_drv)
void lcd_pin_en(u8 gpio_pin)
{
    gpio_t *g;
    if(gpio_pin == LCD_RST){
        g = &lcd_gpio.rst;
    }
    else if(gpio_pin == LCD_CS){
        g = &lcd_gpio.cs;
    }
    else {
        g = &lcd_gpio.rs;
    }
    g->sfr[GPIOxSET] = g->pin;
}

AT(.text.lcd_drv)
void lcd_pin_dis(u8 gpio_pin)
{
    gpio_t *g;
    if(gpio_pin == LCD_RST){
        g = &lcd_gpio.rst;
    }
    else if(gpio_pin == LCD_CS){
        g = &lcd_gpio.cs;
    }
    else{
        g = &lcd_gpio.rs;
    }
    g->sfr[GPIOxCLR] = g->pin;
}

AT(.text.lcd_drv)
void spi_dma_sendbyte(u8* data, u32 data_len)
{
	SPI1CON &= ~BIT(4);
	SPI1DMAADR = DMA_ADR(data);
	SPI1DMACNT = data_len;
	while(!(SPI1CON & BIT(16)));
}

AT(.text.lcd_drv)
void  send_data_spi(uchar dat)
{
    spi_dma_sendbyte(&dat, 1);
}

AT(.text.lcd_drv)
void write_comm(uint i)
{
    LCD_CS_DIS();
	LCD_RS_DIS();
	send_data_spi(i);
    LCD_CS_EN();
}

AT(.text.lcd_drv)
void write_data(uint i)
{
    LCD_CS_DIS();
    LCD_RS_EN();
	send_data_spi(i);
    LCD_CS_EN();
}

//寄存器初始化
//如需修改请参考对应的屏幕手册
void lcd_register_init(void)
{
    LCD_RST_EN();
    delay_us(80);
    LCD_RST_DIS();
    delay_us(80);
    LCD_RST_EN();
    delay_us(480);

//ST7735S+HSD0.96
    write_comm(0x11);	 //sleep out
	delay_ms(120);

    write_comm(0xB1);    //不同颜色下屏幕的刷新率 B1 B2 B3
    write_data(0x05);
    write_data(0x3C);
    write_data(0x3C);

    write_comm(0xB2);
    write_data(0x05);
    write_data(0x3C);
    write_data(0x3C);

    write_comm(0xB3);
    write_data(0x05);
    write_data(0x3C);
    write_data(0x3C);
    write_data(0x05);
    write_data(0x3C);
    write_data(0x3C);

    write_comm(0xB4);    //B4 屏幕是否反色
    write_data(0x03);
    write_data(0x02);

    write_comm(0x21);    //21 屏幕反色

    write_comm(0xC0);    //C0 C1 C2 C3 C4显示屏在不用颜色不同模式下的电压
    write_data(0xE0);
    write_data(0x00);
    write_data(0x84);

    write_comm(0xC1);
    write_data(0xC4);

    write_comm(0xC2);
    write_data(0x0D);
    write_data(0x00);

    write_comm(0xC3);
    write_data(0x8D);
    write_data(0x2A);

    write_comm(0xC4);
    write_data(0x8D);
    write_data(0xEE);

    write_comm(0xC5);    //VCOM
    write_data(0x04);

    write_comm(0x36);   //显示方向：横屏/竖屏 颜色格式：RGB/BGR
    write_data(0xC8);

    write_comm(0xE0);	 //配置伽马极性
    write_data(0x09);
    write_data(0x18);
    write_data(0x1A);
    write_data(0x21);
    write_data(0x3F);
    write_data(0x3B);
    write_data(0x31);
    write_data(0x32);
    write_data(0x2F);
    write_data(0x2A);
    write_data(0x30);
    write_data(0x37);
    write_data(0x00);
    write_data(0x05);
    write_data(0x00);
    write_data(0x10);

    write_comm(0xE1);	//配置伽马极性
    write_data(0x07);
    write_data(0x14);
    write_data(0x15);
    write_data(0x1A);
    write_data(0x3F);
    write_data(0x33);
    write_data(0x2C);
    write_data(0x2E);
    write_data(0x2B);
    write_data(0x28);
    write_data(0x2F);
    write_data(0x3E);
    write_data(0x00);
    write_data(0x04);
    write_data(0x02);
    write_data(0x10);

    write_comm(0x35);
    write_data(0x00);

    write_comm(0x3A);	//像素格式
    write_data(0x05); 	//05  16bit/pixel

    write_comm(0x29);	//进行显示
    write_comm(0x2C);
}

//写入屏幕地址
//（图像显示范围，默认为全屏幕显示）
AT(.text.lcd_drv)
void block_write(uint x_start, uint x_end, uint y_start, uint y_end)
{
	x_start += X_OFFSET;
	x_end   += X_OFFSET;
    y_start += Y_OFFSET;
    y_end   += Y_OFFSET;

	write_comm(0x2A);		//column address set
	write_data(x_start >> 8);
	write_data(x_start);
	write_data(x_end >> 8);
	write_data(x_end);

	write_comm(0x2B);		//row address set
	write_data(y_start >> 8);
	write_data(y_start);
	write_data(y_end >> 8);
	write_data(y_end);

	write_comm(0x2c);
}

//全屏幕颜色填充
AT(.text.lcd_drv)
void disp_color(uint color)
{
	uint i,j;
	block_write(0, (ROW - 1), 0, (COL - 1));
    LCD_CS_DIS();
    LCD_RS_EN();
#if 1
    uint temp = 0xffff;
    temp &= (color >> 8);
#endif

	for(i=0;i<ROW;i++)
	{
	    for(j=0;j<COL;j++)
		{
			send_data_spi(color >> 8);
			send_data_spi(color);
		}
	}
    LCD_CS_EN();
}

//图片解码及显示
void pic_res_read(u32 *addr, u32 *len, u8 fps)
{
    if(*len == 0){
        return;
    }

    u32 temp_addr = *addr;
    u8 *picture;
    int i, j;
    memcpy(&res_img, (const void*)temp_addr, sizeof(res_img));

    if (res_img.sig != IMG_HEAD) {                       //sig : 0x494D
        printf("sig no correct:%x\n", res_img.sig);
        return;
    }

    if(res_img.rsvd){                                   //带压缩的解码显示
        u16 *pallet = (u16*)(temp_addr + sizeof(res_img));
        picture = (u8*)(pallet + res_img.pallet_width);
	    u32 time_expire = 1000 / fps;

        block_write(0, (PIC_WIDTH - 1), 0, (PIC_HEIGHT - 1));
        LCD_CS_DIS();
        LCD_RS_EN();
        bsp_lcd_ticks = tick_get();

        u16 pic_u16_temp;
        for(i = 0;i < (res_img.width * res_img.height * 2 / BMP_BUF_LEN); i++){
            for(j = 0; j < BMP_BUF_LEN; j+=2){
                pic_u16_temp = *(pallet + (*picture));
                bmp_current_buf[j] = (u8)(pic_u16_temp &0xff);
                bmp_current_buf[j+1] = (u8)((pic_u16_temp >> 8) & 0xff);
                picture++;
            }
            spi_dma_sendbyte(bmp_current_buf, BMP_BUF_LEN);
        }
        while(!(tick_check_expire(bsp_lcd_ticks, time_expire)));
        LCD_CS_EN();
    }
    else{                                              //不带压缩的解码显示
        picture = (u8*)(temp_addr + sizeof(u8) * 8);
	    u32 time_expire = 1000 / fps;

        block_write(0, (PIC_WIDTH - 1), 0, (PIC_HEIGHT - 1));
        LCD_CS_DIS();
        LCD_RS_EN();
        bsp_lcd_ticks = tick_get();

        for(i = 0;i < (res_img.width * res_img.height * 2 / BMP_BUF_LEN); i++){
            for(j = 0; j < BMP_BUF_LEN; j++){
                *(bmp_current_buf +j) = *(picture + j);
            }
            picture += BMP_BUF_LEN;
            spi_dma_sendbyte(bmp_current_buf, BMP_BUF_LEN);
        }
        while(!(tick_check_expire(bsp_lcd_ticks, time_expire)));
        LCD_CS_EN();        

    }
}

//显示动图
//pic：gif图片数组名
//frame_num：图片数量
//frame_per_second：每秒帧数
void disp_gif(const pic_param* pic, u8 frame_num, u8 frame_per_second)
{
	u8 i = 0;
	if(*(pic->len) == 0){
		printf("erorr! empty array!\n");
		return;
	}

	for(i = 0; i < frame_num; i++){
		pic_res_read(pic[i].addr, pic[i].len, frame_per_second);
	}
}

//屏幕初始化
void lcd_display_init(void)
{
    my_printf("%s\n",__func__);
    lcd_io_init();
	spi1_init(SPIFLASH_BAUD);
    lcd_register_init();
#if LCD_TEST_EN
    lcd_test_process();
#endif
}

#if LCD_TEST_EN
//trunk未添加图片示例到res文件夹下，如需测试请先添加图片资源，再修改pic_tbl.c下的图片索引数组
void lcd_test_process(void)
{
    my_printf("%s\n",__func__);
	WDT_DIS();
	while(1){
#if 0
        //gif test
        disp_gif(pic_charge_table, PIC_FRAME_NUM, FRAME_PER_SECOND);

        //full screen color test
		// delay_5ms(100);
		// disp_color(RED);
		// delay_5ms(100);
		// disp_color(BLACK);
		// delay_5ms(100);
		// disp_color(BLUE);
#endif
    }
}
#endif

#endif
