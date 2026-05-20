#ifndef _LCD_DISPLAY_H
#define _LCD_DISPLAY_H

//屏幕分辨率
#define COL                 160                 //pixel column
#define ROW                 80                  //pixel row

//常用的颜色数据
#define RED  		        0xf800      
#define GREEN		        0x07e0      
#define BLUE 		        0x001f      
#define WHITE		        0xffff      
#define BLACK		        0x0000      
#define YELLOW              0xffe0   

//图片解压信息
#define IMG_HEAD            0x4D49             //压缩图片头
#define RES_PIC_TBL_LEN     6                  //bin文件索引数组大小（示例用）

//显示图像信息
#define PIC_WIDTH           80                 //图片实际宽度
#define PIC_HEIGHT          160                //图片实际长度
#define X_OFFSET            26                 //解决0.96寸屏幕起始点非0的bug，可根据实际情况调整
#define Y_OFFSET            1

#define BMP_BUF_LEN         (80*16)            //缓存buf大小
#define FRAME_PER_SECOND    2                  //测试用FPS
#define PIC_FRAME_NUM       RES_PIC_TBL_LEN    //一个动画包含图片数（示例用）

#define LCD_RST_EN()        lcd_pin_en(LCD_RST)
#define LCD_RST_DIS()       lcd_pin_dis(LCD_RST)
#define LCD_RS_EN()         lcd_pin_en(LCD_RS)
#define LCD_RS_DIS()        lcd_pin_dis(LCD_RS)
#define LCD_CS_EN()         lcd_pin_en(LCD_CS)
#define LCD_CS_DIS()        lcd_pin_dis(LCD_CS)

enum {
    LCD_RST = 0,
    LCD_CS,
    LCD_RS
};

typedef struct{
    gpio_t rst;
    gpio_t cs;
    gpio_t rs;
}LCD_GPIO_t;

typedef struct res_img_t_{
    u16 sig;
    u8 rsvd;
    u8 bits;
    u16 width;
    u16 height;
    u16 pallet_width;
    u8 pallet_bits;
    u8 pallet_rsvd;
}res_img_t;

typedef struct {
    u32 *addr;
    u32 *len;
} pic_param;

void lcd_display_init(void);
#if LCD_TEST_EN
void lcd_test_process(void);
#endif

#endif
