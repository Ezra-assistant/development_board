#include "include.h"

#define bsp_i2c_delay() delay_us(5)
#if AUTH_CP_VERSION
#define bsp_i2c_delay() delay_us(6)
#define Auth_CP_Write       (0x20)
#define Auth_CP_Read        (0x21)
#else
#define bsp_i2c_delay() delay_us(5)
#define Auth_CP_Write       (0x40)
#define Auth_CP_Read        (0x41)
#endif

#if I2C_SW_EN
//AT(.text.bsp.i2c)
//static void bsp_i2c_delay(void)
//{
//    u8 delay = 60;
//    while (delay--) {
//        asm("nop");
//    }
//}

//ACK: The transmitter releases the SDA line (HIGH->LOW) during the acknowledge clock pulse
AT(.text.bsp.i2c)
void bsp_i2c_tx_ack(void)//发送ACK应答
{
    I2C_SDA_OUT();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SCL_L();
}

AT(.text.bsp.i2c)
bool bsp_i2c_rx_ack(void)//接收ACK应答
{
    bool ret = false;
    I2C_SDA_IN();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    if (!I2C_SDA_IS_H()) {
        ret = true;
    }
    I2C_SCL_L();
    return ret;
}

//NACK: The transmitter holds the SDA line (keep HIGH) during the acknowledge clock pulse
AT(.text.bsp.i2c)
void bsp_i2c_tx_nack(void)//发送NACK非应答
{
    I2C_SDA_OUT();
    I2C_SDA_H();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SCL_L();
}

//START: A HIGH to LOW transition on the SDA line while SCL is HIGH is one such unique case.
AT(.text.bsp.i2c)
void bsp_i2c_start(void)//开始I2C通信
{
#if I2C_MUX_SD_EN
    if (is_det_sdcard_busy()) {
        return;
    }
    if (FUNCMCON0 & 0x0f) {
        FUNCMCON0 = 0x0f;
        delay_us(5);
    }
#endif // I2C_MUX_SD_EN

    I2C_SDA_SCL_OUT();
    I2C_SDA_SCL_H();
    bsp_i2c_delay();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_L();
}

//STOP: A LOW to HIGH transition on the SDA line while SCL is HIGH
AT(.text.bsp.i2c)
void bsp_i2c_stop(void)//停止I2C通信
{
    I2C_SDA_OUT();
    I2C_SDA_L();
    bsp_i2c_delay();
    I2C_SCL_H();
    bsp_i2c_delay();
    I2C_SDA_H();
}

//tx 1byte
AT(.text.bsp.i2c)
void bsp_i2c_tx_byte(uint8_t dat)//发送1个字节
{
    u8 i;
    I2C_SDA_OUT();
    for (i=0; i<8; i++) {
        if (dat & BIT(7)) {
            I2C_SDA_H();
        } else {
            I2C_SDA_L();
        }
        bsp_i2c_delay();
        I2C_SCL_H();
        bsp_i2c_delay();
        I2C_SCL_L();
        dat <<= 1;
    }
}

//rx 1byte
AT(.text.bsp.i2c)
uint8_t bsp_i2c_rx_byte(void)//接受1个字节
{
    u8 i, dat = 0;
    I2C_SDA_IN();
    for (i=0; i<8; i++) {
        bsp_i2c_delay();
        I2C_SCL_H();
        bsp_i2c_delay();
        dat <<= 1;
        if (I2C_SDA_IS_H()) {
            dat |= BIT(0);
        }
        I2C_SCL_L();
    }
    return dat;
}

AT(.text.bsp.i2c)
void bsp_i2c_init(void)//初始化I2C通信
{
    I2C_SDA_SCL_OUT();
    I2C_SDA_H();
    delay_5ms(2);
}

void bsp_i2c_read_data(u8 addr, u8 *buf, u16 len)
{
    bsp_i2c_start();
    bsp_i2c_tx_byte(Auth_CP_Write);
    while(!bsp_i2c_rx_ack()){
        WDT_CLR();
        bsp_i2c_start();
        bsp_i2c_tx_byte(Auth_CP_Write);
    };

    bsp_i2c_tx_byte(addr);
    bsp_i2c_rx_ack();

    bsp_i2c_stop();
	bsp_i2c_start();
	bsp_i2c_tx_byte(Auth_CP_Read);
    while(!bsp_i2c_rx_ack()){
        WDT_CLR();
        bsp_i2c_start();
        bsp_i2c_tx_byte(Auth_CP_Read);
    };

    for(int i=0;i<len;i++){
        buf[i] = bsp_i2c_rx_byte();
        if(i == (len - 1)){
            bsp_i2c_tx_nack();
        }else{
            bsp_i2c_tx_ack();
        }
    }
	bsp_i2c_stop();
}

void bsp_i2c_write_data(u8 addr, u8 *buf, u8 len)
{
    bsp_i2c_start();
    bsp_i2c_tx_byte(Auth_CP_Write);
    while(!bsp_i2c_rx_ack()){
        WDT_CLR();
        bsp_i2c_start();
        bsp_i2c_tx_byte(Auth_CP_Write);
    };
    bsp_i2c_tx_byte(addr);
    bsp_i2c_rx_ack();
    for(int i = 0;i<len;i++){
        bsp_i2c_tx_byte(buf[i]);
        bsp_i2c_rx_ack();
    }
    bsp_i2c_stop();
}

#if I2C_SW_EN_TEST
// 定义从机设备地址（根据实际从机芯片修改）
#define I2C_SLAVE_ADDR_WRITE    0xA0    // 从机写地址 (示例: EEPROM AT24C02/AT24C01)
#define I2C_SLAVE_ADDR_READ     0xA1    // 从机读地址

// 测试数据缓冲区
static u8 tx_buffer[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
static u8 rx_buffer[16] = {0};

/**
 * @brief  通用 I2C 写数据函数
 * @param  dev_addr_w: 从机写地址
 * @param  reg_addr:   寄存器地址
 * @param  buf:        写入数据缓冲区
 * @param  len:        写入长度
 * @return true: 成功, false: 失败
 */
bool sw_i2c_write(u8 dev_addr_w, u8 reg_addr, u8 *buf, u8 len)
{
    u8 retry = 3;
    
    // 发送起始信号
    bsp_i2c_start();
    
    // 发送从机写地址，等待 ACK
    bsp_i2c_tx_byte(dev_addr_w);
    while (!bsp_i2c_rx_ack()) {
        if (--retry == 0) {
            bsp_i2c_stop();
            return false;  // 从机无应答
        }
        WDT_CLR();
        bsp_i2c_start();
        bsp_i2c_tx_byte(dev_addr_w);
    }
    
    // 发送寄存器地址
    bsp_i2c_tx_byte(reg_addr);
    if (!bsp_i2c_rx_ack()) {
        bsp_i2c_stop();
        return false;
    }
    
    // 发送数据
    for (u8 i = 0; i < len; i++) {
        bsp_i2c_tx_byte(buf[i]);
        if (!bsp_i2c_rx_ack()) {
            bsp_i2c_stop();
            return false;
        }
    }
    
    // 发送停止信号
    bsp_i2c_stop();
    return true;
}

/**
 * @brief  通用 I2C 读数据函数
 * @param  dev_addr_w: 从机写地址
 * @param  dev_addr_r: 从机读地址
 * @param  reg_addr:   寄存器地址
 * @param  buf:        读取数据缓冲区
 * @param  len:        读取长度
 * @return true: 成功, false: 失败
 */
bool sw_i2c_read(u8 dev_addr_w, u8 dev_addr_r, u8 reg_addr, u8 *buf, u16 len)
{
    u8 retry = 3;
    
    // ===== 第一阶段：发送寄存器地址 =====
    bsp_i2c_start();
    
    // 发送从机写地址
    bsp_i2c_tx_byte(dev_addr_w);
    while (!bsp_i2c_rx_ack()) {
        if (--retry == 0) {
            bsp_i2c_stop();
            return false;
        }
        WDT_CLR();
        bsp_i2c_start();
        bsp_i2c_tx_byte(dev_addr_w);
    }
    
    // 发送寄存器地址
    bsp_i2c_tx_byte(reg_addr);
    bsp_i2c_rx_ack();
    
    // ===== 第二阶段：读取数据 =====
    bsp_i2c_stop();
    bsp_i2c_start();
    
    // 发送从机读地址
    retry = 3;
    bsp_i2c_tx_byte(dev_addr_r);
    while (!bsp_i2c_rx_ack()) {
        if (--retry == 0) {
            bsp_i2c_stop();
            return false;
        }
        WDT_CLR();
        bsp_i2c_start();
        bsp_i2c_tx_byte(dev_addr_r);
    }
    
    // 读取数据
    for (u16 i = 0; i < len; i++) {
        buf[i] = bsp_i2c_rx_byte();
        if (i == (len - 1)) {
            bsp_i2c_tx_nack();  // 最后一个字节发送 NACK
        } else {
            bsp_i2c_tx_ack();   // 其他字节发送 ACK
        }
    }
    
    bsp_i2c_stop();
    return true;
}

/**
 * @brief  软件 I2C 测试函数
 */
void sw_i2c_test(void)
{
    bool result;
    u8 reg_addr = 0x00;  // 测试寄存器地址
    
    // 1. 初始化 I2C
    printf("I2C Init...\r\n");
    bsp_i2c_init();
    delay_5ms(10);  // 等待从机准备好
    
    // 2. 测试写入数据
    printf("I2C Write Test...\r\n");
    result = sw_i2c_write(I2C_SLAVE_ADDR_WRITE, reg_addr, tx_buffer, 8);
    if (result) {
        printf("Write OK! Data: ");
        for (u8 i = 0; i < 8; i++) {
            printf("0x%02X ", tx_buffer[i]);
        }
        printf("\r\n");
    } else {
        printf("Write Failed!\r\n");
    }
    
    delay_5ms(10);  // EEPROM 写入需要时间
    
    // 3. 测试读取数据
    printf("I2C Read Test...\r\n");
    memset(rx_buffer, 0, sizeof(rx_buffer));
    result = sw_i2c_read(I2C_SLAVE_ADDR_WRITE, I2C_SLAVE_ADDR_READ, reg_addr, rx_buffer, 8);
    if (result) {
        printf("Read OK! Data: ");
        for (u8 i = 0; i < 8; i++) {
            printf("0x%02X ", rx_buffer[i]);
        }
        printf("\r\n");
    } else {
        printf("Read Failed!\r\n");
    }
    
    // 4. 验证读写数据是否一致
    printf("Data Verify: ");
    if (memcmp(tx_buffer, rx_buffer, 8) == 0) {
        printf("PASS!\r\n");
    } else {
        printf("FAIL!\r\n");
    }
}

/**
 * @brief  单字节读写测试（更简单的测试）
 */
void sw_i2c_single_byte_test(void)
{
    u8 write_data = 0xAB;
    u8 read_data = 0x00;
    u8 reg_addr = 0x10;
    
    printf("Single Byte Test...\r\n");
    
    // 初始化
    bsp_i2c_init();
    delay_5ms(2);
    
    // 写入单字节
    sw_i2c_write(I2C_SLAVE_ADDR_WRITE, reg_addr, &write_data, 1);
    delay_5ms(2);
    
    // 读取单字节
    sw_i2c_read(I2C_SLAVE_ADDR_WRITE, I2C_SLAVE_ADDR_READ, reg_addr, &read_data, 1);
    
    printf("Write: 0x%02X, Read: 0x%02X\r\n", write_data, read_data);
    
    if (write_data == read_data) {
        printf("Single Byte Test PASS!\r\n");
    } else {
        printf("Single Byte Test FAIL!\r\n");
    }
}
#endif //I2C_SW_EN_TEST
#endif //I2C_SW_EN

#if I2C_HW_EN

//AT(.com_rodata.i2c)
//const char str[] = "i2c sned data\n";

volatile uint8_t i2c_tx_done_flag;
volatile uint8_t i2c_rx_done_flag;
volatile uint8_t i2c_tx_again_flag;

#if (!MASTER_OR_SLAVE_MODE)
AT(.com_text.i2c) WEAK
void bsp_i2c_isr(void)
{
    if (IICCON0 & BIT(31)) {
        IICCON0 |= BIT(29);
        if(IICCON0 & BIT(30)){
            i2c_tx_again_flag = 1;// 收到 NAK，标记要重发/出错处理
        }
        else{
            i2c_tx_again_flag = 0;// 收到 ACK，正常
        }

        if (IICCON1 & RDATA) {              //read data done
            i2c_rx_done_flag = 1;
        }
        if (IICCON1 & WDATA) {              //send data done
//            printf(str);
        }
        i2c_tx_done_flag = 1;
    }
}
#else
enum stm_t {
	STM_IDLE = 0,
	STM_TX   = 1,
	STM_RX   = 2
};
u8 iic_buf[128] AT(.com_test.iic_buf);
static uint8_t ram[256] AT(.com_text.iic_buf);
static uint8_t adr;
static uint8_t *slave_buf;
static enum stm_t stm; // 保存软件自己的状态机，记录Done信号到来之前软件的收发状态

// AT(.com_text.iic_ch)
// const char ch[] = "stm:%d\n";
AT(.com_text.i2c)
static void save_dat(uint32_t dat, uint32_t cnt, uint8_t *buf) // buf for check ////把 IICDATA 中的 cnt 个字节，按“低字节先写”的顺序，依次写入从机内部 RAM（ram[]），写入起始地址是 adr
{
	uint8_t addr = adr;
	while (cnt--) {
		ram[addr] = dat & 0xFF;
		++addr;
		dat >>= 8;
	}
}

AT(.com_text.i2c)
static uint32_t read_dat(void)//从 dat 中依次取出最低字节，写入 ram[addr]，并 addr++
{
	uint32_t dat;
	uint8_t addr = adr;

	dat  = ram[addr++];
	dat |= ram[addr++] << 8;
	dat |= ram[addr++] << 16;
	dat |= ram[addr++] << 24;

	return dat;
}

AT(.com_text.i2c)
static void bump_adr(uint32_t cnt)//把寄存器地址自增 cnt，用于“写多个连续寄存器”
{
	adr += cnt;
}

AT(.com_text.i2c)
static uint32_t get_adr(uint32_t dat)//把 dat 的最低 1 字节作为寄存器地址 adr，返回剩下的数据（dat >> 8）
{
	adr = dat;
	return dat >> 8;
}

AT(.com_text.i2c) WEAK
void bsp_i2c_isr(void)
{
    u32 sta, con;
    u32 dat, cnt;
    u32 sta1, con1;

    con = IICCON0;
    sta = IICSSTS;
    cnt = (sta >> 8) & 7;//让 bit10~bit8 这三个位移动到最低 3 bit 的位置，只保留右移后得到的最低 3 bit，屏蔽掉其它高位

    WDT_DIS();
    if(con & BIT(31)){
        if(cnt){//读写数据部分
            dat = IICDATA;
            switch(stm){
            case STM_IDLE://主机写的第 1 个 byte 是寄存器地址
                dat = get_adr(dat);
                --cnt;
            case STM_RX://主机正在往从机写数据
                save_dat(dat, cnt, slave_buf);
                bump_adr(cnt);
                break;
            case STM_TX://主机正在读从机的数据
                bump_adr(cnt);
                break;
            }
        }

        if(sta & BIT(4)){//STOP 处理 & DONE 清除
            IICCON0 |= BIT(29);//清除 DONE 标志位
            con1 = IICCON0;
            sta1 = IICSSTS;
            if((!(con1 & BIT(31))) && (sta1 != sta)){
                sta = sta1;
            }
            stm = STM_IDLE;
        }

        IICCON0 |= BIT(29);//清除 DONE 标志位
        if(sta &BIT(6)){//从机发送模式
            dat = read_dat();
            IICDATA = dat;
            IICCON1 = 4;//准备 4 字节数据
            IICSSTS = (((sta & BIT(2)) | BIT(3) | BIT(4)) << 16) | BIT(17);//清除 IICSSTS 的 START、RESTART、STOP 标志；从机发送数据给主机
            stm = STM_TX;
        }
        else{//从机接受模式
            IICSSTS = (((sta & BIT(2)) | BIT(3) | BIT(4)) << 16) | BIT(16);//清除 IICSSTS 的 START、RESTART、STOP 标志；主机写数据给从机
            stm = (sta & BIT(5)) ? STM_RX : STM_IDLE;
        }
    }
}
#endif

static u32 bsp_i2c_config(u32 i2c_cfg, u16 dev_addr, u16 reg_addr, u32 dat)
{
    while (i2c_tx_done_flag == 0);
    i2c_tx_done_flag = 0;
    IICCMDA = (u8)dev_addr | ((u32)(dev_addr >> 8)) << 24 | //将 dev_addr 的高 8 位和低 8 位分别左移 24 位和 0 位，然后进行按位或运算
              (u32)((u8)reg_addr) << 8 | (u32)((u8)(reg_addr >> 8)) << 16;//将 reg_addr 的高 8 位和低 8 位分别左移 16 位和 8 位，然后进行按位或运算
    IICCON1 = i2c_cfg;
    IICDATA = dat;
    IICCON0 |= BIT(28);                     // kick

    if(i2c_cfg & RDATA){
            while (i2c_rx_done_flag == 0);
            i2c_rx_done_flag = 0;
        return IICDATA;
    }else{
        return 0;
    }
}

void bsp_i2c_tx_byte(u16 dev_addr, u16 reg_addr, u32 data)
{
    WDT_DIS();
    u32 cfg;
    cfg = 1;
    cfg |= START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | WDATA | STOP_FLAG | NACK;
    bsp_i2c_config(cfg, dev_addr, reg_addr, data);
}

void bsp_i2c_tx_buf(u16 dev_addr, u16 reg_addr, u8 *buf, u16 len)//此接口一次性最多只能发送4Byte的数据
{
    WDT_DIS();
    u32 tx_data = 0;
    int ptr = 0;
    int cnt = 0;
    int first = 1, last = 0;
    u32 cfg;
#if 1
    while(len){
        cnt = len > DATA_CNT ? DATA_CNT : len;
        if(len <= 4){
            last = 1;
        }
        cfg = cnt;
        cfg |= WDATA;
        if(first){
            cfg |= START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 ;
        }
        if(last){
            cfg |= STOP_FLAG | NACK;
        }
        //tx_data = 0;//每一包数据前先把 tx_data 清零
        for(int i = 0; i < cnt; i++){
            tx_data |= (buf[ptr*DATA_CNT + i] << 8 * i);
        }
        bsp_i2c_config(cfg, dev_addr, reg_addr, tx_data);
        len -= cnt;
        ptr++;
        first = 0;
    }

#else //用于无法立即响应回ack的从机
    cfg |=  START_FLAG0 | DEV_ADDR0 | STOP_FLAG ;
    do{
        bsp_i2c_config(cfg, dev_addr, reg_addr, tx_data);
    }while(i2c_tx_again_flag);

    while(len > DATA_CNT){
        len -= DATA_CNT;
        cfg = DATA_CNT;
        for(int i = 0; i < DATA_CNT; i++){
            tx_data |= (buf[i] << 8 * i);
        }
        if(!cnt){                                       //只有第一次进循环需要start_flag dev_addr reg_addr
            cfg |= WDATA | START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 ;
        }
        else{
            cfg |= WDATA ;
        }
        bsp_i2c_config(cfg, dev_addr, reg_addr, tx_data);
        cnt++;
    }

    cfg = len;
    if(cnt){
        cfg |= WDATA | STOP_FLAG | NACK ;
        tx_data = 0;
    }
    else{
        cfg |= START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | WDATA | STOP_FLAG | NACK ;
    }
    for(int i = 0; i < len; i++){
        tx_data |= (buf[cnt * DATA_CNT + i] << 8 * i);
    }
    bsp_i2c_config(cfg, dev_addr, reg_addr, tx_data);

#endif
}

void bsp_i2c_rx_buf(u16 dev_addr, u16 reg_addr, u8 *buf, u16 len)
{
    WDT_DIS();
    int ptr = 0;
    int cnt = 0;
    int first = 1, last = 0;
    u32 iic_data;
    u32 cfg;

    if (buf == NULL || len == 0) {
       return;
    }

#if 1
    while(len){
        cnt = len > DATA_CNT ? DATA_CNT : len;
        if(len <= 4){
            last = 1;
        }
        cfg = cnt;
        cfg |= RDATA;
        if(first){
            cfg |= START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 | DEV_ADDR1;
        }
        if(last){
            cfg |= STOP_FLAG | NACK;
        }

        iic_data = bsp_i2c_config(cfg, dev_addr, reg_addr, 0);
        for(int i = 0; i < cnt; i++){
            buf[ptr * DATA_CNT + i] = (u8)(iic_data >> (8 * i));
        }
        ptr++;
        len -= cnt;
        first = 0;
    }

#else //用于无法立即响应回ack的从机
    while(len > DATA_CNT){                           //超过DATA_CNT的部分，分DATA_CNT每次读取放进buf
        len -= DATA_CNT;
        cfg = DATA_CNT;
        if(!cnt){                                   //只有第一次进循环需要start_flag dev_addr reg_addr
            cfg |= RDATA | START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 ;
        }
        else{
            cfg |= RDATA ;
        }
        iic_data = bsp_i2c_config(cfg, dev_addr, reg_addr, 0);

        while(IICCON0 & BIT(30)){                   //没接收到从机ack就继续写0x23
            cfg = DATA_CNT;
            cfg |= RDATA | DEV_ADDR1 | START_FLAG1 ;
            iic_data = bsp_i2c_config(cfg, dev_addr, reg_addr, 0);
        }

        for(int i = 0; i < DATA_CNT; i++){
            buf[cnt * DATA_CNT + i] = (u8)(iic_data >> (8 * i));
        }
        cnt++;
    }

    cfg = len;
    if(cnt){
        cfg |= RDATA | STOP_FLAG | NACK;
    }
    else{                                           //读取长度不超过DATA_CNT，一次即可完成
        cfg |= RDATA | START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 | STOP_FLAG | NACK;
    }
    iic_data = bsp_i2c_config(cfg, dev_addr, reg_addr, 0);

    for(int i = 0; i < len; i++){
        buf[cnt * DATA_CNT + i] = (u8)(iic_data >> (8 * i));
    }
#endif
}

#if IIC_DMA_EN
void bsp_i2c_tx_dma(u16 dev_addr, u16 reg_addr, u8 *buf, u16 len)
{
    WDT_DIS();
    IICCMDA = (u8)dev_addr | (u32)((u8)reg_addr) << 8 ;

    IICDMACNT = (IICDMACNT & 0xffff) | ((len-1) << 16);
    IICDMACNT |= BIT(0) | BIT(1);
    IICDMAADR = DMA_ADR(buf);

    IICCON1 = WDATA | START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | STOP_FLAG | NACK;
    IICCON0 |= BIT(28);

    if (IICCON0 & BIT(1))
    {
        while (i2c_tx_done_flag == 0);
        i2c_tx_done_flag = 0;
    }
    else{
        while(!(IICDMACNT & BIT(2)));
    }
    while(!(IICDMACNT & BIT(2)));
}

void bsp_i2c_rx_dma(u16 dev_addr, u16 reg_addr, u8 *buf, u16 len)
{
    WDT_DIS();
    IICCMDA = (u8)dev_addr | ((u32)(dev_addr >> 8)) << 24 |
              (u32)((u8)reg_addr) << 8;

    IICDMACNT = (len-1) << 16;
    IICDMACNT |= BIT(0);
    IICDMACNT &= ~BIT(1);
    IICDMAADR = DMA_ADR(buf);

    IICCON1 = RDATA | START_FLAG0 | DEV_ADDR0 | REG_ADDR_1 | START_FLAG1 | DEV_ADDR1 | STOP_FLAG | NACK;
    IICCON0 |= BIT(28);                     // kick

    if (IICCON0 & BIT(1))
    {
        while (i2c_rx_done_flag == 0);
        i2c_rx_done_flag = 0;
    }
    else{
        while(!(IICDMACNT & BIT(2)));
    }
}
#endif

#if MASTER_OR_SLAVE_MODE
void bsp_i2c_slave_init(uint32_t adr, u8* buf)
{
    #if (I2C_MAPPING == I2CMAP_PB1PB0)
    GPIOBDE  |= (BIT(1) | BIT(0));
    GPIOBDIR |= (BIT(1) | BIT(0));
    GPIOBPU  |= (BIT(1) | BIT(0));
    GPIOBFEN |= (BIT(1) | BIT(0));
    FUNCMCON0 = I2CMAP_PB1PB0;
#elif (I2C_MAPPING == I2CMAP_PA1PA2)
    GPIOADE  |= (BIT(1) | BIT(2));
    GPIOADIR |= (BIT(1) | BIT(2));
    GPIOAPU  |= (BIT(1) | BIT(2));
    GPIOAFEN |= (BIT(1) | BIT(2));
    FUNCMCON0 = I2CMAP_PA1PA2;
#elif (I2C_MAPPING == I2CMAP_PA6PA7)
    GPIOADE  |= (BIT(6) | BIT(7));
    GPIOADIR |= (BIT(6) | BIT(7));
    GPIOAPU  |= (BIT(6) | BIT(7));
    GPIOAFEN |= (BIT(6) | BIT(7));
    FUNCMCON0 = I2CMAP_PA6PA7;
#elif (I2C_MAPPING == I2CMAP_PB8PB9)
    GPIOBDE  |= (BIT(8) | BIT(9));
    GPIOBDIR |= (BIT(8) | BIT(9));
    GPIOBPU  |= (BIT(8) | BIT(9));
    GPIOBFEN |= (BIT(8) | BIT(9));
    FUNCMCON0 = I2CMAP_PB8PB9;
#endif
    IICCMDA = adr;
    IICCON0 = 1 << 0 | //IIC Enable
              1 << 1 | //Interrupt Enable
              1 << 12| //Mode 0:Master 1:Slave
              1 << 13; //Data Sample at 0:falling 1:rising
    stm = STM_IDLE;
    slave_buf = buf;
    sys_irq_init(IRQ_IIC_VECTOR, 0, bsp_i2c_isr);

}
#else
void bsp_i2c_master_init(void)
{
    CLKCON1 = (CLKCON1 & ~(1 << 23)) | (0 << 23);   //I2C clk select, 0:rc2m_clk, 1:x26m_clkdiv8
    CLKGAT1 |= BIT(6);                              //I2C clk enable
    RSTCON0 |= BIT(3);                              //I2C release reset, enable model function
    RTCCON3 &= ~BIT(6);
    FUNCMCON0 = 0xf << 24;

#if (I2C_MAPPING == I2CMAP_PB1PB0)
    GPIOBDE  |= BIT(1) | BIT(0);
    GPIOBDIR |= BIT(1) | BIT(0);
    GPIOBPU  |= BIT(1) | BIT(0);
    GPIOBFEN |= BIT(1) | BIT(0);
    FUNCMCON0 = I2CMAP_PB1PB0;
#elif (I2C_MAPPING == I2CMAP_PA1PA2)
    GPIOADE  |= BIT(1) | BIT(2);
    GPIOADIR |= BIT(1) | BIT(2);
    GPIOAPU  |= BIT(1) | BIT(2);
    GPIOAFEN |= BIT(1) | BIT(2);
    FUNCMCON0 = I2CMAP_PA1PA2;
#elif (I2C_MAPPING == I2CMAP_PA6PA7)
    GPIOADE  |= BIT(6) | BIT(7);
    GPIOADIR |= BIT(6) | BIT(7);
    GPIOAPU  |= BIT(6) | BIT(7);
    GPIOAFEN |= BIT(6) | BIT(7);
    FUNCMCON0 = I2CMAP_PA6PA7;
#elif (I2C_MAPPING == I2CMAP_PB8PB9)
    GPIOBDE  |= BIT(8) | BIT(9);
    GPIOBDIR |= BIT(8) | BIT(9);
    GPIOBPU  |= BIT(8) | BIT(9);
    GPIOBFEN |= BIT(8) | BIT(9);
    FUNCMCON0 = I2CMAP_PB8PB9;
#endif

    i2c_tx_done_flag = 1;
    i2c_rx_done_flag = 0;
    i2c_tx_again_flag = 0;

    IICCON0 =   1 << 0 | // iicen
                1 << 1 | // iic int
                1 << 2 | // hold cnt[3:2]
                9 << 4; // pos div [9:4] //可适当降低I2C时钟频率，避免数据传输错误(<<4前面的数字越大，时钟频率越低)

    sys_irq_init(IRQ_IIC_VECTOR, 0, bsp_i2c_isr);
}
#endif

#if I2C_HW_EN_TEST
//测试示例
void i2c_hw_test(void)
{
#if MASTER_OR_SLAVE_MODE
    extern uint8_t ram[256];  // 直接声明外部变量
    memset(ram, 0, 256);
    WDT_DIS();
    bsp_i2c_slave_init(0x40, iic_buf);

    // 循环打印
    while(1) {
        delay_5ms(500);

        // 打印 ram 的内容
        printf("RAM:");
        for(int i = 0; i < 32; i++) {
            printf("%02X ", ram[i]);
        }
        printf("\n\n");  // 额外的换行便于区分

        // 清零 ram 前32字节
        memset(ram, 0, 256);
    }
#else
    bsp_i2c_master_init();
    u8 rx_buf[32];
    u8 tx_buf[32]={0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                   0x09, 0x0A, 0x0B, 0X0C, 0X0D, 0X0E, 0X0F, 0X10,
                   0X11, 0X12, 0X13, 0X14, 0X15, 0X16, 0X17, 0X18,
                   0X19, 0X1A, 0X1B, 0X1C, 0X1D, 0X1E, 0X1F, 0x20};
#endif
#if IIC_DMA_EN //dma test
    bsp_i2c_tx_dma(0xA0,0x00,tx_buf,8);
    delay_5ms(10);
    memset(rx_buf, 0, 32);
    my_print_r(rx_buf, 8);
    bsp_i2c_rx_dma(0xA1A0,0x00,rx_buf,8);       //0xa1a0为dev_adr
    delay_5ms(10);
    my_printf("IICDATA: %08x\n", IICDATA);
    my_print_r(rx_buf, 8);      
#endif

#if 0//tx byte test//发送单个字节
    bsp_i2c_tx_byte(0xA0,0x00,tx_buf[0]);
    delay_5ms(10);
    bsp_i2c_rx_buf(0xA1A0,0x00,rx_buf,1);//0xa1a0为dev_adr
    delay_5ms(10);
    my_print_r(rx_buf, 1);
#endif

#if 0//tx buf test、、发送多个字节
    bsp_i2c_tx_buf(0xA0,0x00,tx_buf,4);
    delay_5ms(10);
    bsp_i2c_rx_buf(0xA1A0,0x00,rx_buf,4);//0xa1a0为dev_adr
    delay_5ms(10);
    my_print_r(rx_buf, 4);
#endif

}
#endif

#else
AT(.com_text.i2c) WEAK
void bsp_i2c_isr(void)
{
}
#endif
