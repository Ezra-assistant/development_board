#include "include.h"
#include "fft_proc.h"

#if FFT_PROC_EN

nr_buf_t nr_buf AT(.fft_buf.buf);

//FFT处理
AT(.com_text.func.fft)
void fft_process(s16 *data)
{
    int i;
	s16 *output = data;
    nr_buf_t *p = &nr_buf;

    for (i = 0; i < PART_LEN; i++) {
		p->xbuf[i + PART_LEN] =data[i];
	}

    // Windowed near fft
	for (i = 0; i < PART_LEN; i++) {
		p->fft[i] = (p->xbuf[i] * (int)sqrtHanning[i] + (1<<(HANN_SHIFT-1))) >> HANN_SHIFT;
		p->fft[PART_LEN + i] = (p->xbuf[PART_LEN + i] * (int)sqrtHanning[PART_LEN - i] + (1<<(HANN_SHIFT-1))) >> HANN_SHIFT;
	}

    fft_forward((s32*)p->fft);  //fft
    //fft后的数据是去掉对称的部分的,节省buf空间.(第二个点保存中间对称点的实部,第三个点开始按照实虚实虚保存)

    fft_inverse((s32*)p->fft);  //ifft

    for (i = 0; i < PART_LEN; i++) {
		// Do we need saturation ?
		p->fft[i] = AEC_SPL_SAT(AEC_SPL_WORD16_MAX, p->fft[i], AEC_SPL_WORD16_MIN);
		p->fft[i] = ((p->fft[i] * (int)sqrtHanning[i] + (1<<(HANN_SHIFT-1))) >> HANN_SHIFT) + p->outBuf[i];

        output[i] = (short)AEC_SPL_SAT(AEC_SPL_WORD16_MAX, p->fft[i], AEC_SPL_WORD16_MIN);
        //de-emp

		p->outBuf[i] = (p->fft[PART_LEN + i] * (int)sqrtHanning[PART_LEN - i] + (1<<(HANN_SHIFT-1))) >> HANN_SHIFT;

	}
    memcpy(p->xbuf, p->xbuf + PART_LEN, 4 * PART_LEN);
}

//FFT初始化
void fft_user_init(void)
{
#if FFT_512_EN
        fft_512_init();
#elif FFT_256_EN
        fft_256_init();
#elif FFT_128_EN
        fft_128_init();
#endif
}

#if 0   //FFT测试函数
#if FFT_512_EN
#define SAMPLES_LEN         256
#endif
#if FFT_256_EN
#define SAMPLES_LEN         128
#endif
#if FFT_128_EN
#define SAMPLES_LEN         64
#endif
u8 tpcm_buf[SAMPLES_LEN*2] AT(.fft_buf.pcm_data);

void fft_test(void)
{
    u8 *data = (u8 *)RES_BUF_FILE_TEST_PCM;
    u32 len = RES_LEN_FILE_TEST_PCM;
    fft_user_init();
    dac_gpdma_reset();
    dac_spr_set(SPR_16000);
    for(u32 i=0; i < len; i+= (SAMPLES_LEN*2)){
        WDT_CLR();
        memcpy(tpcm_buf, &data[i], sizeof(tpcm_buf));
        fft_process((s16*)tpcm_buf);
        while(!dac_gpdma_is_ready());
        dac_gpdma_kick((s16 *)tpcm_buf, SAMPLES_LEN);//需要注意GPDMA_BUF_SIZE的长度设置
    }
}
#endif

#endif
