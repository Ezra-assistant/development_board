#ifndef _MATRIX_KEY_H
#define _MATRIX_KEY_H

#define ARRAY_NUM(array)    (sizeof(array)/sizeof(array[0]))

#define MATRIX_KEY_ADC_TRIGGER      0x360

#define MATRIX_KEY_BASE             0x80        //以此为原点,计算按键滤波
#define MATRIX_KEY_PRESS_TIMES      (2)         //矩阵按键判断按下的次数
#define MATRIX_KEY_RLS_TIMES        (2)         //矩阵按键判断释放的次数
#define MATRIX_KEY_MASK_TIMES       (6)         //矩阵按键避免一次按下,多次消息发送

#define IO_NOCGT_KEY_BASE            0x80        //以此为原点,计算按键滤波
#define IO_NOCGT_KEY_PRESS_TIMES     (6)         //IOKEY按键判断按下的次数
#define IO_NOCGT_KEY_RLS_TIMES       (6)         //IOKEY按键判断释放的次数
#define IO_NOCGT_KEY_LONG_TIMES      (200)       //长按次数

typedef struct{
    gpio_t gpio0;
    gpio_t gpio1;
    gpio_t gpio2;
    gpio_t gpio3;
    gpio_t gpio4;
    gpio_t gpio5;
    gpio_t gpio6;
    gpio_t gpio7;
}maxtrix_key_t;

void matrix_xy_key_scan(void);
void matrix_xy_key_scan_kick(void);
void get_matrix_xy_key(void);
void matrix_xy_key_proc(void);
void matrix_xy_key_init(u16 *adc_ch);

void io_nocft_key_init(void);
void io_nocft_key_scan(void);
void get_io_nocft_key(void);
#endif
