/*****************************************************************************
 * Module    : Configs
 * File      : config_define.h
 * Function  : 定义用户参数常量
 *****************************************************************************/
#ifndef CONFIG_DEFINE_H
#define CONFIG_DEFINE_H

/*****************************************************************************
 * Module    : 显示相关配置选择列表
 *****************************************************************************/
//显示驱动屏选择
#define DISPLAY_NO                      0                                       //无显示模块
#define DISPLAY_LCD                     0x100                                   //选用LCD点阵屏做为显示驱动
#define DISPLAY_LCDSEG                  0x200                                   //选用断码屏做为显示驱动
#define DISPLAY_LEDSEG                  0x400                                   //选用数码管做为显示驱动

#define GUI_NO                          DISPLAY_NO                              //无主题，无显示
#define GUI_LCD                         (DISPLAY_LCD | 0x00)                    //LCD点阵屏(128*64)
#define GUI_LCDSEG                      (DISPLAY_LCDSEG | 0x00)                 //断码屏默认主题
#define GUI_LEDSEG_5C7S                 (DISPLAY_LEDSEG | 0x00)                 //5C7S 数码管
#define GUI_LEDSEG_7P7S                 (DISPLAY_LEDSEG | 0x01)                 //7PIN 数码管 按COM方式扫描
#define GUI_LEDSEG_3P7S                 (DISPLAY_LEDSEG | 0x02)                 //3PIN 7段数码管
#define GUI_LEDSEG_6C6S                 (DISPLAY_LEDSEG | 0x04)                 //6C6S 6段数码管

/*****************************************************************************
 * Module    : FLASH大小定义
 *****************************************************************************/
#define FSIZE_32M                       0x2000000   //要支持32MB-flash需要替换libplatform_32m.a
#define FSIZE_16M                       0x1000000   //MAX flash size
#define FSIZE_8M                        0x800000
#define FSIZE_4M                        0x400000
#define FSIZE_2M                        0x200000
#define FSIZE_1M                        0x100000
#define FSIZE_512K                      0x80000
#define FSIZE_256K                      0x40000
#define FSIZE_128K                      0x20000

/*****************************************************************************
 * Module    : 提示音语言配置选择列表
 *****************************************************************************/
#define LANG_EN         0               //英文提示音
#define LANG_ZH         1               //中文提示音
#define LANG_EN_ZH      2               //英文、中文双语提示音

/*****************************************************************************
 * Module    : ADC通路选择列表
 *****************************************************************************/
#define ADCCH_PA0       0               //SARADC channel 0
#define ADCCH_PA1       1               //SARADC channel 1
#define ADCCH_PA2       2               //SARADC channel 2
#define ADCCH_PA3       3               //SARADC channel 3
#define ADCCH_PA4       4               //SARADC channel 4
#define ADCCH_PA5       5               //SARADC channel 5
#define ADCCH_PA6       6               //SARADC channel 6
#define ADCCH_PA7       7               //SARADC channel 7
#define ADCCH_PA14      8               //SARADC channel 8
#define ADCCH_PA15      9               //SARADC channel 9
#define ADCCH_PB0       10              //SARADC channel 10
#define ADCCH_PB1       11              //SARADC channel 11
#define ADCCH_PB4       12              //SARADC channel 12
#define ADCCH_PB5       13              //SARADC channel 13
#define ADCCH_BGOP      14              //SARADC channel 14     BG/Tsensor
#define ADCCH_VBAT      15              //SARADC channel 15     vbatdiv2


/*****************************************************************************
 * Module    : Timer Capture Mapping选择列表
 *****************************************************************************/
#define TMR0MAP_PA14    (1 << 0)        //TMR0CAP G1  capture mapping: PA14
#define TMR1MAP_PB9     (1 << 0)        //TMR1CAP G1  capture mapping: PB9
#define TMR2MAP_PA1     (1 << 0)        //TMR2CAP G1  capture mapping: PA1


/*****************************************************************************
 * Module    : IRRX Mapping选择列表
 *****************************************************************************/
#define IRMAP_PA1       (1 << 20)       //G1  capture mapping: PA1
#define IRMAP_PA2       (2 << 20)       //G2  capture mapping: PA2
#define IRMAP_PA7       (3 << 20)       //G3  capture mapping: PA7
#define IRMAP_PB2       (4 << 20)       //G4  capture mapping: PB2
#define IRMAP_PB5       (5 << 20)       //G5  capture mapping: PB5
#define IRMAP_PB6       (6 << 20)       //G6  capture mapping: PB6
#define IRMAP_PE0       (7 << 20)       //G7  capture mapping: PE0
#define IRMAP_PF0       (8 << 20)       //G8  capture mapping: PF0


/*****************************************************************************
 * Module    : Clock output Mapping选择列表
 *****************************************************************************/
#define CLKOMAP_PB9     (1 << 0)        //G1 Clock output mapping: PB9
#define CLKOMAP_PA3     (2 << 0)        //G2 Clock output mapping: PA3
#define CLKOMAP_PB1     (3 << 0)        //G3 Clock output mapping: PB1


/*****************************************************************************
 * Module    : sd0 Mapping选择列表
 *****************************************************************************/
#define SD0MAP_G1       (1 << 0)       //G1  SDCLK(PA1), SDCMD(PA2), SDDAT0(PA3)
#define SD0MAP_G2       (2 << 0)       //G2  SDCLK(PB0), SDCMD(PB1), SDDAT0(PB2)
#define SD0MAP_G3       (3 << 0)       //G3  SDCLK(PB4), SDCMD(PB5), SDDAT0(PB6)
#define SD0MAP_G4       (4 << 0)       //G4  SDCLK(PA6), SDCMD(PA7), SDDAT0(PA8)


/*****************************************************************************
 * Module    : spi1 Mapping选择列表 (FUNCMCON1)
 *****************************************************************************/
#define SPI1MAP_G1     (1 << 12)        //G1 SPI1CLK(PB0),  SPI1DI(PB2),  SPI1DO(PB1)
#define SPI1MAP_G2     (2 << 12)        //G2 SPI1CLK(PA14), SPI1DI(PA13), SPI1DO(PA15)
#define SPI1MAP_G3     (3 << 12)        //G3 SPI1CLK(PA6),  SPI1DI(PA8),  SPI1DO(PA7)
#define SPI1MAP_G4     (4 << 12)        //G4 SPI1CLK(PB8),  SPI1DI(PB7),  SPI1DO(PB9)
#define SPI1MAP_G5     (5 << 12)        //G5 SPI1CLK(PB1),  SPI1DI(PB2),  SPI1DO(PB9)


/*****************************************************************************
 * Module    : uart0 Mapping选择列表
 *****************************************************************************/
#define UTX0MAP_PA6     (1 << 8)        //G1 uart0 tx: PA6
#define UTX0MAP_PA0     (2 << 8)        //G2 uart0 tx: PA0
#define UTX0MAP_PB9     (3 << 8)        //G3 uart0 tx: PB9
#define UTX0MAP_PB1     (4 << 8)        //G4 uart0 tx: PB1
#define UTX0MAP_PB2     (5 << 8)        //G5 uart0 tx: PB2
#define UTX0MAP_PB0     (6 << 8)        //G6 uart0 tx: PB0

#define URX0MAP_PA7     (1 << 12)       //G1 uart0 rx: PA7
#define URX0MAP_PA1     (2 << 12)       //G2 uart0 rx: PA1
#define URX0MAP_PB2     (3 << 12)       //G3 uart0 rx: PB2
#define URX0MAP_PB0     (4 << 12)       //G4 uart0 rx: PB0
#define URX0MAP_TX      (7 << 12)       //G7 uart0 map to TX pin by UT0TXMAP select(1线模式)

/*****************************************************************************
* Module    : hsuart select table
*****************************************************************************/
#define HSTX_PA4       (1 << 16)       //HSTX G1: PA4
#define HSTX_PA5       (2 << 16)       //HSTX G2: PA5
#define HSTX_PB9       (3 << 16)       //HSTX G3: PB9
#define HSTX_PB1       (4 << 16)       //HSTX G4: PB1
#define HSTX_PB2       (5 << 16)       //HSTX G5: PB2
#define HSTX_PB0       (6 << 16)       //HSTX G6: PB0

#define HSRX_PA4       (1 << 20)       //HSRX G1: PA4
#define HSRX_PA5       (2 << 20)       //HSRX G2: PA5
#define HSRX_PB9       (3 << 20)       //HSRX G3: PB9
#define HSRX_PB1       (4 << 20)       //HSRX G4: PB1

/*****************************************************************************
 * Module    : 录音文件类型列表
 *****************************************************************************/
#define REC_NO          0
#define REC_WAV         1              //PCM WAV
#define REC_ADPCM       2              //ADPCM WAV
#define REC_MP3         3


/*****************************************************************************
* Module    : TouchKey通路
*****************************************************************************/
#define TK_PA2_CH0          0
#define TK_PA7_CH1          1
#define TK_PB2_CH2          2
#define TK_PB6_CH3          3

/*****************************************************************************
* Module    : DAC OUT Sample Rate
*****************************************************************************/
#define DAC_OUT_44K1    0               //dac out sample rate 44.1K
#define DAC_OUT_48K     1               //dac out sample rate 48K

/*****************************************************************************
* Module    : DAC LDOH Select
*****************************************************************************/
#define AU_LDOH_2V4     0               //VDDAUD LDO voltage 2.4V
#define AU_LDOH_2V5     1               //VDDAUD LDO voltage 2.5V
#define AU_LDOH_2V7     2               //VDDAUD LDO voltage 2.7V
#define AU_LDOH_2V9     3               //VDDAUD LDO voltage 2.9V
#define AU_LDOH_3V1     4               //VDDAUD LDO voltage 3.1V
#define AU_LDOH_3V2     5               //VDDAUD LDO voltage 3.2V

/*****************************************************************************
* Module    : 电池低电电压列表
*****************************************************************************/
#define VBAT_2V8            0       //2.8v
#define VBAT_2V9            1       //2.9v
#define VBAT_3V0            2       //3.0v
#define VBAT_3V1            3       //3.1v
#define VBAT_3V2            4       //3.2v
#define VBAT_3V3            5       //3.3v
#define VBAT_3V4            6       //3.4v
#define VBAT_3V5            7       //3.5v
#define VBAT_3V6            8       //3.6v
#define VBAT_3V7            9       //3.7v
#define VBAT_3V8            10      //3.8v

/*****************************************************************************
* Module    : uart0 printf IO列表
*****************************************************************************/
#define PRINTF_NONE         0           //关闭UART0打印信息
#define PRINTF_PA0          1
#define PRINTF_PA6          2
#define PRINTF_PB0          3
#define PRINTF_PB1          4
#define PRINTF_PB2          5
#define PRINTF_PB9          6

/*****************************************************************************
* Module    : GPIO list
*****************************************************************************/
#define IO_NONE             0
#define IO_PA0              1
#define IO_PA1              2
#define IO_PA2              3
#define IO_PA3              4
#define IO_PA4              5
#define IO_PA5              6
#define IO_PA6              7
#define IO_PA7              8
#define IO_PA8              9
#define IO_PA9              10
#define IO_PA10             11
#define IO_PA11             12
#define IO_PA12             13
#define IO_PA13             14
#define IO_PA14             15
#define IO_PA15             16
#define IO_PB0              17
#define IO_PB1              18
#define IO_PB2              19
#define IO_PB3              20
#define IO_PB4              21
#define IO_PB5              22
#define IO_PB6              23
#define IO_PB7              24
#define IO_PB8              25
#define IO_PB9              26
#define IO_PB10             27
#define IO_PB11             28
#define IO_MAX_NUM          28

#define IO_MUX_SDCLK        26
#define IO_MUX_SDCMD        27

/*****************************************************************************
* Module    : wakeup circuit port
*****************************************************************************/
#define WKU_PA7             0
#define WKU_PB0             1
#define WKU_PB1             2
#define WKU_PB2             3
#define WKU_PB9             4
#define WKU_PA0             5

/*****************************************************************************
* Module    : Function output select table
*****************************************************************************/
#define FUNCO_PWM0          0
#define FUNCO_PWM1          1
#define FUNCO_PWM2          2
#define FUNCO_PWM3          3
#define FUNCO_TMR2PWM0      4
#define FUNCO_TMR2PWM1      5
#define FUNCO_UART0TX       6
#define FUNCO_HSUTTX        7
#define FUNCO_IIC_SDA       8
#define FUNCO_IIC_SCL       9
#define FUNCO_SPI1_DO       10
#define FUNCO_SPI1_DI       11
#define FUNCO_SPI1_CLK      12
#define FUNCO_RES0          13
#define FUNCO_RES1          14
#define FUNCO_CLKOUT        15

/*****************************************************************************
* Module    : EX_SPIFLASH_SUPPORT
*****************************************************************************/
#define EXSPI_NOT_SUPPORT   0
#define EXSPI_REC           (1<<0)    //外接SPIFLASH支持录音
#define EXSPI_MUSIC         (1<<1)    //外接SPIFLASH支持MP3音乐镜像文件播放

/*****************************************************************************
 * Module    : I2C Mapping选择列表 FUNCMCON2[27:24]
 *****************************************************************************/
#define I2CMAP_PB1PB0       (1 << 24)        //G1 i2c clk: PB1  dat: PB0
#define I2CMAP_PA1PA2       (2 << 24)        //G2 i2c clk: PA1  dat: PA2
#define I2CMAP_PA6PA7       (3 << 24)        //G3 i2c clk: PA6  dat: PA7
#define I2CMAP_PB8PB9       (4 << 24)        //G4 i2c clk: PB8  dat: PB9

/*****************************************************************************
* Module    : 数字增益调节表
*****************************************************************************/
#define GAIN_0DB_VAL          8192
#define GAIN_DIG_P18DB       (GAIN_0DB_VAL / 0.125893)      //65071    //7.943282
#define GAIN_DIG_P17DB       (GAIN_0DB_VAL / 0.141254)      //57994    //7.079458
#define GAIN_DIG_P16DB       (GAIN_0DB_VAL / 0.158489)      //51688    //6.309574
#define GAIN_DIG_P15DB       (GAIN_0DB_VAL / 0.177828)      //46067    //5.623413
#define GAIN_DIG_P14DB       (GAIN_0DB_VAL / 0.199526)      //41057    //5.011872
#define GAIN_DIG_P13DB       (GAIN_0DB_VAL / 0.223872)      //36592    //4.466835
#define GAIN_DIG_P12DB       (GAIN_0DB_VAL / 0.251189)      //32612    //3.981072
#define GAIN_DIG_P11DB       (GAIN_0DB_VAL / 0.281838)      //29066    //3.548134
#define GAIN_DIG_P10DB       (GAIN_0DB_VAL / 0.316228)      //25905    //3.162278
#define GAIN_DIG_P9DB       (GAIN_0DB_VAL / 0.354813)      //23088    //2.818383
#define GAIN_DIG_P8DB       (GAIN_0DB_VAL / 0.398107)      //20577    //2.511886
#define GAIN_DIG_P7DB       (GAIN_0DB_VAL / 0.446684)      //18339    //2.238721
#define GAIN_DIG_P6DB       (GAIN_0DB_VAL / 0.501187)      //16345    //1.995262
#define GAIN_DIG_P5DB       (GAIN_0DB_VAL / 0.562341)      //14567    //1.778279
#define GAIN_DIG_P4DB       (GAIN_0DB_VAL / 0.630957)      //12983    //1.584893
#define GAIN_DIG_P3DB       (GAIN_0DB_VAL / 0.707946)      //11571    //1.412538
#define GAIN_DIG_P2DB       (GAIN_0DB_VAL / 0.794328)      //10313    //1.258925
#define GAIN_DIG_P1DB       (GAIN_0DB_VAL / 0.891251)      //9191    //1.122018
#define GAIN_DIG_N0DB       (GAIN_0DB_VAL / 1.000000)      //8192    //1.000000
#define GAIN_DIG_N1DB       (GAIN_0DB_VAL / 1.122018)      //7301    //0.891251
#define GAIN_DIG_N2DB       (GAIN_0DB_VAL / 1.258925)      //6507    //0.794328
#define GAIN_DIG_N3DB       (GAIN_0DB_VAL / 1.412538)      //5799    //0.707946
#define GAIN_DIG_N4DB       (GAIN_0DB_VAL / 1.584893)      //5168    //0.630957
#define GAIN_DIG_N5DB       (GAIN_0DB_VAL / 1.778279)      //4606    //0.562341
#define GAIN_DIG_N6DB       (GAIN_0DB_VAL / 1.995262)      //4105    //0.501187
#define GAIN_DIG_N7DB       (GAIN_0DB_VAL / 2.238721)      //3659    //0.446684
#define GAIN_DIG_N8DB       (GAIN_0DB_VAL / 2.511886)      //3261    //0.398107
#define GAIN_DIG_N9DB       (GAIN_0DB_VAL / 2.818383)      //2906    //0.354813
#define GAIN_DIG_N10DB       (GAIN_0DB_VAL / 3.162278)      //2590    //0.316228
#define GAIN_DIG_N11DB       (GAIN_0DB_VAL / 3.548134)      //2308    //0.281838
#define GAIN_DIG_N12DB       (GAIN_0DB_VAL / 3.981072)      //2057    //0.251189
#define GAIN_DIG_N13DB       (GAIN_0DB_VAL / 4.466836)      //1833    //0.223872
#define GAIN_DIG_N14DB       (GAIN_0DB_VAL / 5.011872)      //1634    //0.199526
#define GAIN_DIG_N15DB       (GAIN_0DB_VAL / 5.623413)      //1456    //0.177828
#define GAIN_DIG_N16DB       (GAIN_0DB_VAL / 6.309574)      //1298    //0.158489
#define GAIN_DIG_N17DB       (GAIN_0DB_VAL / 7.079459)      //1157    //0.141254
#define GAIN_DIG_N18DB       (GAIN_0DB_VAL / 7.943282)      //1031    //0.125893
#define GAIN_DIG_N19DB       (GAIN_0DB_VAL / 8.912509)      //919    //0.112202
#define GAIN_DIG_N20DB       (GAIN_0DB_VAL / 10.000000)      //819    //0.100000
#define GAIN_DIG_N21DB       (GAIN_0DB_VAL / 11.220183)      //730    //0.089125
#define GAIN_DIG_N22DB       (GAIN_0DB_VAL / 12.589254)      //650    //0.079433
#define GAIN_DIG_N23DB       (GAIN_0DB_VAL / 14.125375)      //579    //0.070795
#define GAIN_DIG_N24DB       (GAIN_0DB_VAL / 15.848934)      //516    //0.063096
#define GAIN_DIG_N25DB       (GAIN_0DB_VAL / 17.782794)      //460    //0.056234
#define GAIN_DIG_N26DB       (GAIN_0DB_VAL / 19.952621)      //410    //0.050119
#define GAIN_DIG_N27DB       (GAIN_0DB_VAL / 22.387212)      //365    //0.044668
#define GAIN_DIG_N28DB       (GAIN_0DB_VAL / 25.118862)      //326    //0.039811
#define GAIN_DIG_N29DB       (GAIN_0DB_VAL / 28.183833)      //290    //0.035481
#define GAIN_DIG_N30DB       (GAIN_0DB_VAL / 31.622778)      //259    //0.031623
#define GAIN_DIG_N31DB       (GAIN_0DB_VAL / 35.481335)      //230    //0.028184
#define GAIN_DIG_N32DB       (GAIN_0DB_VAL / 39.810719)      //205    //0.025119
#define GAIN_DIG_N33DB       (GAIN_0DB_VAL / 44.668358)      //183    //0.022387
#define GAIN_DIG_N34DB       (GAIN_0DB_VAL / 50.118728)      //163    //0.019953
#define GAIN_DIG_N35DB       (GAIN_0DB_VAL / 56.234132)      //145    //0.017783
#define GAIN_DIG_N36DB       (GAIN_0DB_VAL / 63.095731)      //129    //0.015849
#define GAIN_DIG_N37DB       (GAIN_0DB_VAL / 70.794580)      //115    //0.014125
#define GAIN_DIG_N38DB       (GAIN_0DB_VAL / 79.432822)      //103    //0.012589
#define GAIN_DIG_N39DB       (GAIN_0DB_VAL / 89.125104)      //91     //0.011220

/*****************************************************************************
* Module    : uart update IO列表
*****************************************************************************/
#define UART_RX0_G1_PA7 ((1 << 12) | (0 << 0))
#define UART_TX0_G1_PA6 ((1 << 8)  | (0 << 2))
#define UART_RX0_G2_PA1 ((2 << 12) | (0 << 0))
#define UART_TX0_G2_PA0 ((2 << 8)  | (0 << 2))

#define UART_RX0_G3_PB2 ((3 << 12) | (1 << 0))
#define UART_TX0_G3_PB9 ((3 << 8)  | (1 << 2))
#define UART_RX0_G4_PB1 ((4 << 12) | (1 << 0))
#define UART_TX0_G4_PB0 ((4 << 8)  | (1 << 2))

#define UPDATE_UART0     0

#endif //CONFIG_DEFINE_H
