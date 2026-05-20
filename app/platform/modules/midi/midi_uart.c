#include "include.h"
#include "midi_uart.h"

#if MIDI_UART0_EN

#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

isr_t register_isr(int vector, isr_t isr);
uint8_t checksum_calculation(uint8_t *ptr, uint16_t start, uint16_t len);

S_UART_BUF sUartbuf AT(.uart_buf);//TX/RX
MIDI_UART_DECODE_PACKET sUartPacket AT(.uart_buf);

static u8 uart_state = UART_STATE_INIT;

AT(.text.bsp.uart)
void midi_uart_init(void)
{
    memset(&sUartbuf, 0, sizeof(sUartbuf));
    memset(&sUartPacket, 0, sizeof(sUartPacket));
    midi_udet_init(115200);
    midi_register_uart0_isr_init(midi_uart0_isr);
    uart_state = UART_STATE_INIT_FINISH;
    midi_uart_sysinit_finish();
    uart_state = UART_STATE_SYNC;
}

AT(.text.bsp.uart)
void midi_udet_init(u32 baudrate)
{
    u32 baud = 0;
    baud = ((SYS_CLK + (baudrate / 2)) / baudrate) - 1;
    printf("baudrate11:%d\n",baudrate);
    CLKGAT1 |= BIT(11);                             //enable uart0 clk
    UDETCON = 0;

#if (!UART_CROSSBAR)
#if UART0_DUPLEX_SEL
    #if (UART0_TX_PORT == UTX0MAP_PA6)
        GPIOADE  |=  BIT(6);
        GPIOAPU  |=  BIT(6);
        GPIOADIR &= ~BIT(6);
        GPIOAFEN |=  BIT(6);
    #elif (UART0_TX_PORT == UTX0MAP_PA0)
        GPIOADE  |=  BIT(0);
        GPIOAPU  |=  BIT(0);
        GPIOADIR &= ~BIT(0);
        GPIOAFEN |=  BIT(0);
    #elif (UART0_TX_PORT == UTX0MAP_PB9)
        GPIOBDE  |=  BIT(9);
        GPIOBPU  |=  BIT(9);
        GPIOBDIR &= ~BIT(9);
        GPIOBFEN |=  BIT(9);
    #elif (UART0_TX_PORT == UTX0MAP_PB1)
        GPIOBDE  |=  BIT(1);
        GPIOBPU  |=  BIT(1);
        GPIOBDIR &= ~BIT(1);
        GPIOBFEN |=  BIT(1);
    #elif (UART0_TX_PORT == UTX0MAP_PB2)
        GPIOBDE  |=  BIT(2);
        GPIOBPU  |=  BIT(2);
        GPIOBDIR &= ~BIT(2);
        GPIOBFEN |=  BIT(2);
    #elif (UART0_TX_PORT == UTX0MAP_PB0)
        GPIOBDE  |=  BIT(0);
        GPIOBPU  |=  BIT(0);
        GPIOBDIR &= ~BIT(0);
        GPIOBFEN |=  BIT(0);
    #endif

    #if (UART0_RX_PORT == URX0MAP_PA7)
        GPIOADE  |= BIT(7);
        GPIOAPU  |= BIT(7);
        GPIOADIR |= BIT(7);
        GPIOAFEN |= BIT(7);
    #elif (UART0_RX_PORT == URX0MAP_PA1)
        GPIOADE  |= BIT(1);
        GPIOAPU  |= BIT(1);
        GPIOADIR |= BIT(1);
        GPIOAFEN |= BIT(1);
    #elif (UART0_RX_PORT == URX0MAP_PB2)
        GPIOBDE  |= BIT(2);
        GPIOBPU  |= BIT(2);
        GPIOBDIR |= BIT(2);
        GPIOBFEN |= BIT(2);
    #elif (UART0_RX_PORT == URX0MAP_PB0)
        GPIOBDE  |= BIT(0);
        GPIOBPU  |= BIT(0);
        GPIOBDIR |= BIT(0);
        GPIOBFEN |= BIT(0);
    #endif
    FUNCMCON0 = UART0_TX_PORT | UART0_RX_PORT;
#else
    #if (UART0_TX_PORT == UTX0MAP_PA6)
        GPIOADE  |=  BIT(6);
        GPIOAPU  |=  BIT(6);
        GPIOADIR |=  BIT(6);
        GPIOAFEN |=  BIT(6);
    #elif (UART0_TX_PORT == UTX0MAP_PA0)
        GPIOADE  |=  BIT(0);
        GPIOAPU  |=  BIT(0);
        GPIOADIR |=  BIT(0);
        GPIOAFEN |=  BIT(0);
    #elif (UART0_TX_PORT == UTX0MAP_PB9)
        GPIOBDE  |=  BIT(9);
        GPIOBPU  |=  BIT(9);
        GPIOBDIR |=  BIT(9);
        GPIOBFEN |=  BIT(9);
    #elif (UART0_TX_PORT == UTX0MAP_PB1)
        GPIOBDE  |=  BIT(1);
        GPIOBPU  |=  BIT(1);
        GPIOBDIR |=  BIT(1);
        GPIOBFEN |=  BIT(1);
    #elif (UART0_TX_PORT == UTX0MAP_PB2)
        GPIOBDE  |=  BIT(2);
        GPIOBPU  |=  BIT(2);
        GPIOBDIR |=  BIT(2);
        GPIOBFEN |=  BIT(2);
    #elif (UART0_TX_PORT == UTX0MAP_PB0)
        GPIOBDE  |=  BIT(0);
        GPIOBPU  |=  BIT(0);
        GPIOBDIR |=  BIT(0);
        GPIOBFEN |=  BIT(0);
    #endif
    FUNCMCON0 =  UART0_TX_PORT | URX0MAP_TX;                //RX map to TX
#endif
#endif
    UDETBAUD = (baud << 16) | baud;

#if UART_CROSSBAR
    if(UART_CROSSBAR_IO > IO_PA15){
        GPIOBFEN |= BIT(UART_CROSSBAR_IO-1-IO_PA15);
        GPIOBDE  |= BIT(UART_CROSSBAR_IO-1-IO_PA15);
        GPIOBDIR |= BIT(UART_CROSSBAR_IO-1-IO_PA15);
        GPIOAPU  |= BIT(UART_CROSSBAR_IO-1-IO_PA15);
    }else{
        GPIOAFEN |= BIT(UART_CROSSBAR_IO-1);
        GPIOADE  |= BIT(UART_CROSSBAR_IO-1);
        GPIOADIR |= BIT(UART_CROSSBAR_IO-1);
        GPIOAPU  |= BIT(UART_CROSSBAR_IO-1);
    }

#if UART_TX_OR_RX
    FUNCMCON0 = ((FUNCMCON0 & ~(0x7 << 12))  | (5 << 12) );                            //uart0 rx map to channel2, RX map to TX
    FUNCINCON = ((FUNCINCON & ~(0x3f << 16)) | ((UART_CROSSBAR_IO - 1) << 16));        //channel2_rx
#else
    FUNCOUTMCON = ((FUNCOUTMCON & ~(0x1f << 16)) | (UART_CROSSBAR_IO << 16));          //channel2_tx
    FUNCOUTCON  = ((FUNCOUTCON & ~(0xf << 16)) | (6 << 16));                           //function select uart0_tx
#endif
    UDETCON = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(0);       //RX EN, One line, fix baud, 2 Stop bits, Uart EN
#else
    #if UART0_DUPLEX_SEL
        UDETCON = BIT(7) | BIT(5) | BIT(4) | BIT(0);            //RX EN, TX/RX separate, fix baud, 2 Stop bits, Uart EN
    #else
        UDETCON = BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(0);   //RX EN, One line, fix baud, 2 Stop bits, Uart EN
    #endif
#endif
    UDETCON = (UDETCON & ~(0xf<<16)) | (0x5<<16);
    UDETCON = (UDETCON & ~(0xf<<20)) | (0x5<<20);
    UDETCON = (UDETCON & ~(0xf<<24)) | (0x5<<24);
    UDETCPND = BIT(10) | BIT(15) | BIT(18);                     //CLR Pending
}

AT(.text.bsp.uart)
void midi_register_uart0_isr_init(isr_t isr)
{
    UDETCON  |= BIT(2);                 //Receive int enable
    UDETCPND |= BIT(9);                 //clear RX Pending

    register_isr(IRQ_UART_VECTOR, isr);
    PICPR &= ~BIT(IRQ_UART_VECTOR);
    PICEN |= BIT(IRQ_UART_VECTOR);
}

AT(.com_text.uart)
void midi_uart0_isr(void)
{
    u8 data;
    if(UDETCON & BIT(9))
    {
        data = UDETDATA;
        // my_printf("data : %x\n",data);
        UDETCPND = BIT(9);//clear RX pending
        sUartbuf.decodeBuf[sUartbuf.w_cnt] = data;

        if((sUartbuf.w_cnt + 1) >= UART_RX_BUF_LEN){
            sUartbuf.w_cnt = 0;
        }else{
            sUartbuf.w_cnt++;
        }
    }
}

//***************************************************************************************//
//uart获取1字节数据接口
AT(.com_text.uart)
u8 midi_uart_get_data(u8 *charData)
{
    if(sUartbuf.r_cnt != sUartbuf.w_cnt)
    {
        *charData = sUartbuf.decodeBuf[sUartbuf.r_cnt];
        //TRACE("Read sUartbuf.decodeBuf[%d]: 0x%X\n", sUartbuf.r_cnt, sUartbuf.decodeBuf[sUartbuf.r_cnt]);
        if((sUartbuf.r_cnt + 1) >= UART_RX_BUF_LEN){
            sUartbuf.r_cnt = 0;
        }else{
            sUartbuf.r_cnt++;
        }
        return 1;
    }
    return 0;
}

//uart发送1字节数据接口
AT(.com_text.uart)
void midi_uart_putchar(char charData)
{
    UDETDATA = charData;
    while(!(UDETCON & BIT(8)));
}

//uart发送多字节数据接口
AT(.text.bsp.uart)
bool midi_send_uart_data(u8 *buf, u8 length)
{
    bool boRet = false;
    for(u8 i = 0; i < length; i++)
    {
        midi_uart_putchar(buf[i]);
        //TRACE("uartSendData: %02x\n", buf[i]);
    }
    boRet = true;
    return boRet;
}

//MIDI 的uart命令控制处理
AT(.text.uart)
void midi_cmdid_process(void)
{
    u8 cmdid, data_one, data_two;

    cmdid = sUartPacket.DataBuf[0];
    data_one = sUartPacket.DataBuf[1];
    data_two = sUartPacket.DataBuf[2];
    TRACE("cmdid:0x%x, data0:0x%x, data1:0x%X\n", cmdid, data_one, data_two);

    switch(cmdid)
    {
        case CMDID_MIDI_TO_BLE:
            break;
        case CMDID_MIDI_TO_BR:
            break;
        case CMDID_MIDI_TO_USBOUT:
            break;
        case CMDID_MIDI_TO_BOTH:
            break;
        case CMDID_HANDSHAKE_STATE:
            //握手应答
            if(data_one == 0x01) {
                uart_state = UART_STATE_CONNECTED;
                TRACE("uart_state 1:%d\n",uart_state);
            }
            //对握手请求回复
            else if(data_one == 0x00) {
                //uart_state = UART_STATE_SYNC;
                TRACE("uart_state 2:%d\n",uart_state);
                midi_uart_sysn_response();
            }
            break;
        case CMDID_SYS_INIT_CMPLT_STATE:
            break;
        case CMDID_USB_CONNECT_STATE:
            break;
        case CMDID_KEYBOARD_PEDAL_CTR:
            break;
    }

}

//MIDI 的标准事件处理
AT(.text.uart)
void midi_message_process(void)
{
    u8 bynote, bynum, byvel;

    bynote = sUartPacket.DataBuf[0];
    bynum = sUartPacket.DataBuf[1];
    byvel = sUartPacket.DataBuf[2];
    TRACE("bynote:0x%x, bynum:0x%x, byvel:0x%x\n", bynote, bynum, byvel);
    bynote = sUartPacket.DataBuf[0] & 0xF0;
    switch(bynote)
    {
        case MIDI_MESSAGE_NOTE_ON:
        case MIDI_MESSAGE_NOTE_OFF:
        {
            mkey_note_onoff(sUartPacket.DataBuf[0], sUartPacket.DataBuf[1], sUartPacket.DataBuf[2]);
        }break;
        case MIDI_MESSAGE_CTRL_CHANGE:
        {
            mkey_ctrl_change(sUartPacket.DataBuf[0], sUartPacket.DataBuf[1], sUartPacket.DataBuf[2]);
        }break;
        case MIDI_MESSAGE_PROG_CHANGE:
        {
            mkey_prog_change(sUartPacket.DataBuf[0], sUartPacket.DataBuf[1]);
        }break;
        case MIDI_MESSAGE_PITCH_BEND:
        {
            mkey_pitch_bend(sUartPacket.DataBuf[0], sUartPacket.DataBuf[1], sUartPacket.DataBuf[2]);
        }break;
        default:break;
    }
}

//uart解析处理数据
AT(.text.uart)
void midi_parse_uart_cmd_data(u8 uartData)
{
    bool midi_ctrl_decode_success = false;
    bool midi_message_decode_success = false;

    u8 tmp_data = 0;

    switch(sUartPacket.decodeStep)
    {
        case UART_STEP_HEAD_ONE:
        {
            sUartPacket.midi_message = (uartData & 0xF0);
            tmp_data = sUartPacket.midi_message;
            if((tmp_data == 0x90) || (tmp_data == 0x80) || (tmp_data == 0xB0) || (tmp_data == 0xC0) || (tmp_data == 0xE0)) {
                sUartPacket.decodeStep = UART_STEP_MIDI_MESSAGE;
                sUartPacket.DataBuf[sUartPacket.w_cnt] = uartData;
                sUartPacket.w_cnt++;
            }
            else if(UART_HEAD_ONE == uartData) {
                sUartPacket.decodeStep = UART_STEP_HEAD_TWO;
            }
            else {
                TRACE("header1 error\n");
                memset(&sUartPacket, 0, sizeof(sUartPacket));
            }
        }break;
        case UART_STEP_HEAD_TWO:
        {
            if(UART_HEAD_TWO == uartData) {
                sUartPacket.decodeStep = UART_STEP_LENGTH;
            }else {
                TRACE("header2 error\n");
                memset(&sUartPacket, 0, sizeof(sUartPacket));
            }
        }break;
        case UART_STEP_LENGTH:
        {
            TRACE("GET_ULENGTH uartData: 0x%X\n", uartData);
            if(uartData <= UART_CMD_MAX_SIZE){
                sUartPacket.uLength = uartData;
                sUartPacket.decodeStep = UART_STEP_CMDID;
            }else{
                TRACE("cmd length oversize\n");
                memset(&sUartPacket, 0, sizeof(sUartPacket));
            }
        }break;
        case UART_STEP_CMDID:
        {
            sUartPacket.DataBuf[sUartPacket.w_cnt] = uartData;
            //TRACE("w_cnt:%d, uartData:0x%X\n", sUartPacket.w_cnt,sUartPacket.DataBuf[sUartPacket.w_cnt]);
            sUartPacket.w_cnt++;
            if(sUartPacket.w_cnt >= sUartPacket.uLength){
                TRACE("SAVE_UDATA uartData: 0x%X\n", uartData);
                //sUartPacket.decodeStep = CAL_UCHECKSUM;
                midi_ctrl_decode_success = true;
            }
        }break;
        case UART_STEP_MIDI_MESSAGE:
        {
            tmp_data = (sUartPacket.midi_message & 0xF0);
            switch(tmp_data)
            {
                case MIDI_MESSAGE_NOTE_ON:
                case MIDI_MESSAGE_NOTE_OFF:
                case MIDI_MESSAGE_CTRL_CHANGE://BN
                case MIDI_MESSAGE_PITCH_BEND://EN
                {
                    sUartPacket.uLength = 3;
                    sUartPacket.DataBuf[sUartPacket.w_cnt++] = uartData;
                }break;
                case MIDI_MESSAGE_PROG_CHANGE://CN
                {
                    sUartPacket.uLength = 2;
                    sUartPacket.DataBuf[sUartPacket.w_cnt++] = uartData;
                }break;
                default:break;
            }
            if(sUartPacket.w_cnt >= sUartPacket.uLength){
                midi_message_decode_success = true;
            }

        }break;

        default:
            memset(&sUartPacket, 0, sizeof(sUartPacket));
            break;
    }

    if(midi_ctrl_decode_success)
    {
        midi_cmdid_process();
        memset(&sUartPacket, 0, sizeof(sUartPacket));
    }
    if(midi_message_decode_success)
    {
        midi_message_process();
        memset(&sUartPacket, 0, sizeof(sUartPacket));
    }
}

//将uart收到的数据进行处理
AT(.text.uart)
void midi_uart_rx_process(void)
{
    u8 tmp_uart_data;
    while(midi_uart_get_data(&tmp_uart_data)){
        WDT_CLR();
        midi_parse_uart_cmd_data(tmp_uart_data);
    }
}

//tx数据打包发送
AT(.text.uart)
void midi_uart_packet_tx(UART_CMD_ID cmdid, void* data, u8 len)
{
    u8 buf[6];

    buf[0] = UART_HEAD_ONE;
    buf[1] = UART_HEAD_TWO;
    buf[2] = len;
    buf[3] = (u8)cmdid;
    //buf[4] = data;
    memcpy(&buf[4], data, len-1);

    midi_send_uart_data(buf, 4+len-1);
}

//芯片握手命令请求
AT(.text.uart)
void midi_uart_sysn_request(void)
{
    u8 data = 0;   //数据1：00h 握手请求，01h 握手应答
    midi_uart_packet_tx(CMDID_HANDSHAKE_STATE, (u8*)&data, 0x02);
}

//芯片握手命令应答
AT(.text.uart)
void midi_uart_sysn_response(void)
{
    u8 data = 0x01;   //数据1：00h 握手请求，01h 握手应答
    midi_uart_packet_tx(CMDID_HANDSHAKE_STATE, (u8*)&data, 0x02);
}

//USB连接状态命令
AT(.text.uart)
void midi_uart_usb_state(void)
{
    u8 data = 0;    //数据1：00h usb未连接，01h usb已连接
    midi_uart_packet_tx(CMDID_USB_CONNECT_STATE, (u8*)&data, 0x02);
}

//芯片初始化完成命令
AT(.text.uart)
void midi_uart_sysinit_finish(void)
{
    u8 data = 0x01;     //数据1 初始化完成
    midi_uart_packet_tx(CMDID_SYS_INIT_CMPLT_STATE, (u8*)&data, 0x02);
}

//切换midi数据发送目标
AT(.text.uart)
void midi_uart_data_switch_target(UART_CMD_ID cmdid)
{
    u8 data = 0;
    midi_uart_packet_tx(cmdid, (u8*)&data, 0x01);
}

//键盘产品踏板命令
AT(.text.uart)
void midi_uart_keyboard_pedal(void)
{
    u8 data[2];

    data[0] = 0x01;         //踏板编号
    data[1] = 0x7f;         //踏板模拟量
    midi_uart_packet_tx(CMDID_KEYBOARD_PEDAL_CTR, (u8*)&data, 0x03);
}

//开关音符码串口发送
AT(.text.uart)
void midi_note_onoff_tx(u8 bynote, u8 bynum, u8 byvel)
{
    u8 buf[3];
    buf[0] = bynote;
    buf[1] = bynum;
    buf[2] = byvel;

    midi_send_uart_data(buf, sizeof(buf));
}

//控制器Bn码串口发送
AT(.text.uart)
void midi_ctrl_change_tx(u8 bynote, u8 bynum, u8 byvel)
{
    u8 buf[3];
    buf[0] = bynote;
    buf[1] = bynum;
    buf[2] = byvel;

    midi_send_uart_data(buf, sizeof(buf));
}

//midi 改变乐器Cn码串口发送
AT(.text.uart)
void midi_prog_change_tx(u8 bynote, u8 bynum)
{
    u8 buf[2];
    buf[0] = bynote;
    buf[1] = bynum;

    midi_send_uart_data(buf, sizeof(buf));
}

//midi 滑音En码串口发送
AT(.text.uart)
void midi_pitch_bend_tx(u8 bynote, u8 bynum, u8 byvel)
{
    u8 buf[3];
    buf[0] = bynote;
    buf[1] = bynum;
    buf[2] = byvel;

    midi_send_uart_data(buf, sizeof(buf));
}

AT(.text.uart)
void midi_message_tx(void)
{
    midi_note_onoff_tx(0x96, 0x4A, 0x2D);               //按下音符、音符0x4a、力度0x2d
    midi_ctrl_change_tx(0xB6, 0x7F, 0x7F);              //控制器Bx、控制器号码、控制器参数
    midi_prog_change_tx(0xC6, 0x7F);                    //改变乐Cx、乐器音色号码
    midi_pitch_bend_tx(0xE6, 0x7F, 0x7F );              //滑音Ex、高音低位、高音高位
}

AT(.text.uart)
void midi_uart_tx_process(void)
{
    static u32 uart_tx_tick = 0;

    if(tick_check_expire(uart_tx_tick, 10)){
        uart_tx_tick = tick_get();
        if(uart_state == UART_STATE_SYNC)
        {
            midi_uart_sysn_request();
        }

        if(uart_state == UART_STATE_CONNECTED)
        {
            midi_uart_data_switch_target(CMDID_MIDI_TO_BLE);
            midi_uart_data_switch_target(CMDID_MIDI_TO_BR);
            midi_uart_data_switch_target(CMDID_MIDI_TO_USBOUT);
            midi_uart_data_switch_target(CMDID_MIDI_TO_BOTH);
            midi_uart_usb_state();
            midi_uart_keyboard_pedal();
            midi_message_tx();
        }
    }
}

#endif
