#ifndef __IO_DEF_H
#define __IO_DEF_H


#include "global.h"
#include "config.h"
#include "sfr.h"


#if (SD0_MAPPING == SD0MAP_G1)
    //SDCMD(PA2), SDCLK(PA1), SDDAT0(PA3)
    #define SD_MUX_IO_INIT()        {GPIOADE |= BIT(2)|BIT(1)|BIT(3);\
                                    GPIOACLR  = BIT(1);\
                                    GPIOADIR &= ~BIT(1);\
                                    GPIOAPU  &= ~BIT(1);\
                                    GPIOADIR |= BIT(2) | BIT(3);\
                                    GPIOAPU  |= BIT(2) | BIT(3);\
                                    GPIOAFEN |= BIT(2) | BIT(3);\
                                    FUNCMCON0 = SD0MAP_G1;}
    #define SD_IO_INIT()            {GPIOADE |= BIT(2)|BIT(1)|BIT(3);\
                                    GPIOADIR &= ~BIT(1);\
                                    GPIOADIR |= BIT(2) | BIT(3);\
                                    GPIOAPU  |= BIT(2) | BIT(3);\
                                    GPIOAFEN |= BIT(2) | BIT(3);\
                                    FUNCMCON0 = SD0MAP_G1;}
    #define SD_CLK_DIR_IN()         {GPIOADIR |= BIT(1); GPIOAPU  |= BIT(1);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOADIR |= BIT(1); GPIOAPU  &= ~BIT(1);}
    #define SD_CLK_DIR_OUT()        {GPIOAPU  &= ~BIT(1); GPIOADIR &= ~BIT(1);}
    #define SD_MUX_DETECT_INIT()    {GPIOADE |= BIT(1); GPIOAPU  |= BIT(1); GPIOADIR |= BIT(1);}
    #define SD_MUX_IS_ONLINE()      ((GPIOA & BIT(1)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOADIR & BIT(1)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOAPU500K & BIT(2))
    #define SD_CMD_MUX_PU300R()     {GPIOAPU500K |= BIT(2); GPIOAPU &= ~BIT(2);}
    #define SD_CMD_MUX_PU10K()      {GPIOAPU |= BIT(2); GPIOAPU500K &= ~BIT(2);}
    #define SD_DAT_MUX_PU300R()     {GPIOAPU500K |= BIT(3); GPIOAPU &= ~BIT(3);}
    #define SD_DAT_MUX_PU10K()      {GPIOAPU |= BIT(3); GPIOAPU500K &= ~BIT(3);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOA & BIT(2)) == 0)


    #define SD_CLK_OUT_H()          {GPIOASET = BIT(1);}
    #define SD_CLK_OUT_L()          {GPIOACLR = BIT(1);}
    #define SD_CLK_STA()            (GPIOA & BIT(1))

    #define SD_DAT_DIR_OUT()        {GPIOADE |= BIT(3); GPIOADIR &= ~BIT(3);}
    #define SD_DAT_DIR_IN()         {GPIOADIR |= BIT(3); GPIOAPU  |= BIT(3);}
    #define SD_DAT_OUT_H()          {GPIOASET = BIT(3);}
    #define SD_DAT_OUT_L()          {GPIOACLR = BIT(3);}
    #define SD_DAT_STA()            (GPIOA & BIT(3))

    #define SD_CMD_DIR_OUT()        {GPIOADE |= BIT(2); GPIOADIR &= ~BIT(2);}
    #define SD_CMD_DIR_IN()         {GPIOADIR |= BIT(2); GPIOAPU  |= BIT(2);}
    #define SD_CMD_OUT_H()          {GPIOASET = BIT(2);}
    #define SD_CMD_OUT_L()          {GPIOACLR = BIT(2);}
    #define SD_CMD_STA()            (GPIOA & BIT(2))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PA1
    #define SDCMD_IO                IO_PA2
    #define SDDAT_IO                IO_PA3

#elif (SD0_MAPPING == SD0MAP_G2)
    //SDCMD(PB1), SDCLK(PB0), SDDAT0(PB2)
    #define SD_MUX_IO_INIT()        {GPIOBDE |= BIT(1)|BIT(0)|BIT(2);\
                                    GPIOBCLR  = BIT(0);\
                                    GPIOBDIR &= ~BIT(0);\
                                    GPIOBPU  &= ~BIT(0);\
                                    GPIOBDIR |= BIT(1) | BIT(2);\
                                    GPIOBPU  |= BIT(1) | BIT(2);\
                                    GPIOBFEN |= BIT(1) | BIT(2);\
                                    FUNCMCON0 = SD0MAP_G2;}
    #define SD_IO_INIT()            {GPIOBDE |= BIT(1)|BIT(0)|BIT(2);\
                                    GPIOBDIR &= ~BIT(0);\
                                    GPIOBDIR |= BIT(1) | BIT(2);\
                                    GPIOBPU  |= BIT(1) | BIT(2);\
                                    GPIOBFEN |= BIT(1) | BIT(2);\
                                    FUNCMCON0 = SD0MAP_G2;}
    #define SD_CLK_DIR_IN()         {GPIOBDIR |= BIT(0); GPIOBPU  |= BIT(0);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOBDIR |= BIT(0); GPIOBPU  &= ~BIT(0);}
    #define SD_CLK_DIR_OUT()        {GPIOBPU  &= ~BIT(0); GPIOBDIR &= ~BIT(0);}
    #define SD_MUX_DETECT_INIT()    {GPIOBDE |= BIT(0); GPIOBPU  |= BIT(0); GPIOBDIR |= BIT(0);}
    #define SD_MUX_IS_ONLINE()      ((GPIOB & BIT(0)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOBDIR & BIT(0)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOBPU500K & BIT(1))
    #define SD_CMD_MUX_PU300R()     {GPIOBPU500K |= BIT(1); GPIOBPU &= ~BIT(1);}
    #define SD_CMD_MUX_PU10K()      {GPIOBPU |= BIT(1); GPIOBPU500K &= ~BIT(1);}
    #define SD_DAT_MUX_PU300R()     {GPIOBPU500K |= BIT(2); GPIOBPU &= ~BIT(2);}
    #define SD_DAT_MUX_PU10K()      {GPIOBPU |= BIT(2); GPIOBPU500K &= ~BIT(2);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOB & BIT(1)) == 0)


    #define SD_CLK_OUT_H()          {GPIOBSET = BIT(0);}
    #define SD_CLK_OUT_L()          {GPIOBCLR = BIT(0);}
    #define SD_CLK_STA()            (GPIOB & BIT(0))

    #define SD_DAT_DIR_OUT()        {GPIOBDE |= BIT(2); GPIOBDIR &= ~BIT(2);}
    #define SD_DAT_DIR_IN()         {GPIOBDIR |= BIT(2); GPIOBPU  |= BIT(2);}
    #define SD_DAT_OUT_H()          {GPIOBSET = BIT(2);}
    #define SD_DAT_OUT_L()          {GPIOBCLR = BIT(2);}
    #define SD_DAT_STA()            (GPIOB & BIT(2))

    #define SD_CMD_DIR_OUT()        {GPIOBDE |= BIT(1); GPIOBDIR &= ~BIT(1);}
    #define SD_CMD_DIR_IN()         {GPIOBDIR |= BIT(1); GPIOBPU  |= BIT(1);}
    #define SD_CMD_OUT_H()          {GPIOBSET = BIT(1);}
    #define SD_CMD_OUT_L()          {GPIOBCLR = BIT(1);}
    #define SD_CMD_STA()            (GPIOB & BIT(1))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PB0
    #define SDCMD_IO                IO_PB1
    #define SDDAT_IO                IO_PB2

#elif (SD0_MAPPING == SD0MAP_G3)
    //SDCMD(PB5), SDCLK(PB4), SDDAT0(PB6)
    #define SD_MUX_IO_INIT()        {GPIOBDE |= BIT(5)|BIT(4)|BIT(6);\
                                    GPIOBCLR  = BIT(4);\
                                    GPIOBDIR &= ~BIT(4);\
                                    GPIOBPU  &= ~BIT(4);\
                                    GPIOBDIR |= BIT(5) | BIT(6);\
                                    GPIOBPU  |= BIT(5) | BIT(6);\
                                    GPIOBFEN |= BIT(5) | BIT(6);\
                                    FUNCMCON0 = SD0MAP_G3;}
    #define SD_IO_INIT()            {GPIOBDE |= BIT(5)|BIT(4)|BIT(6);\
                                    GPIOBDIR &= ~BIT(1);\
                                    GPIOBDIR |= BIT(5) | BIT(6);\
                                    GPIOBPU  |= BIT(5) | BIT(6);\
                                    GPIOBFEN |= BIT(5) | BIT(6);\
                                    FUNCMCON0 = SD0MAP_G3;}
    #define SD_CLK_DIR_IN()         {GPIOBDIR |= BIT(4); GPIOBPU  |= BIT(4);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOBDIR |= BIT(4); GPIOBPU  &= ~BIT(4);}
    #define SD_CLK_DIR_OUT()        {GPIOBPU  &= ~BIT(4); GPIOBDIR &= ~BIT(4);}
    #define SD_MUX_DETECT_INIT()    {GPIOBDE |= BIT(4); GPIOBPU  |= BIT(4); GPIOBDIR |= BIT(4);}
    #define SD_MUX_IS_ONLINE()      ((GPIOB & BIT(4)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOBDIR & BIT(4)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOBPU500K & BIT(5))
    #define SD_CMD_MUX_PU300R()     {GPIOBPU500K |= BIT(5); GPIOBPU &= ~BIT(5);}
    #define SD_CMD_MUX_PU10K()      {GPIOBPU |= BIT(5); GPIOBPU500K &= ~BIT(5);}
    #define SD_DAT_MUX_PU300R()     {GPIOBPU500K |= BIT(6); GPIOBPU &= ~BIT(6);}
    #define SD_DAT_MUX_PU10K()      {GPIOBPU |= BIT(6); GPIOBPU500K &= ~BIT(6);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOB & BIT(5)) == 0)


    #define SD_CLK_OUT_H()          {GPIOBSET = BIT(4);}
    #define SD_CLK_OUT_L()          {GPIOBCLR = BIT(4);}
    #define SD_CLK_STA()            (GPIOB & BIT(4))

    #define SD_DAT_DIR_OUT()        {GPIOBDE |= BIT(6); GPIOBDIR &= ~BIT(6);}
    #define SD_DAT_DIR_IN()         {GPIOBDIR |= BIT(6); GPIOBPU  |= BIT(6);}
    #define SD_DAT_OUT_H()          {GPIOBSET = BIT(6);}
    #define SD_DAT_OUT_L()          {GPIOBCLR = BIT(6);}
    #define SD_DAT_STA()            (GPIOB & BIT(6))

    #define SD_CMD_DIR_OUT()        {GPIOBDE |= BIT(5); GPIOBDIR &= ~BIT(5);}
    #define SD_CMD_DIR_IN()         {GPIOBDIR |= BIT(5); GPIOBPU  |= BIT(5);}
    #define SD_CMD_OUT_H()          {GPIOBSET = BIT(5);}
    #define SD_CMD_OUT_L()          {GPIOBCLR = BIT(5);}
    #define SD_CMD_STA()            (GPIOB & BIT(5))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PB4
    #define SDCMD_IO                IO_PB5
    #define SDDAT_IO                IO_PB6

#elif (SD0_MAPPING == SD0MAP_G4)
    //SDCMD(PA7), SDCLK(PA6), SDDAT0(PB8)
    #define SD_MUX_IO_INIT()        {GPIOADE |= BIT(7)|BIT(6)|BIT(8);\
                                    GPIOACLR  = BIT(6);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOAPU  &= ~BIT(6);\
                                    GPIOADIR |= BIT(7) | BIT(8);\
                                    GPIOAPU  |= BIT(7) | BIT(8);\
                                    GPIOAFEN |= BIT(7) | BIT(8);\
                                    FUNCMCON0 = SD0MAP_G4;}
    #define SD_IO_INIT()            {GPIOADE |= BIT(7)|BIT(6)|BIT(8);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOADIR |= BIT(7) | BIT(8);\
                                    GPIOAPU  |= BIT(7) | BIT(8);\
                                    GPIOAFEN |= BIT(7) | BIT(8);\
                                    FUNCMCON0 = SD0MAP_G4;}
    #define SD_CLK_DIR_IN()         {GPIOADIR |= BIT(6); GPIOAPU  |= BIT(6);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOADIR |= BIT(6); GPIOAPU  &= ~BIT(6);}
    #define SD_CLK_DIR_OUT()        {GPIOAPU  &= ~BIT(6); GPIOADIR &= ~BIT(6);}
    #define SD_MUX_DETECT_INIT()    {GPIOADE |= BIT(6); GPIOAPU  |= BIT(6); GPIOADIR |= BIT(6);}
    #define SD_MUX_IS_ONLINE()      ((GPIOA & BIT(6)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOADIR & BIT(6)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOAPU500K & BIT(7))
    #define SD_CMD_MUX_PU300R()     {GPIOAPU500K |= BIT(7); GPIOAPU &= ~BIT(7);}
    #define SD_CMD_MUX_PU10K()      {GPIOAPU |= BIT(7); GPIOAPU500K &= ~BIT(7);}
    #define SD_DAT_MUX_PU300R()     {GPIOAPU500K |= BIT(8); GPIOAPU &= ~BIT(8);}
    #define SD_DAT_MUX_PU10K()      {GPIOAPU |= BIT(8); GPIOAPU500K &= ~BIT(8);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOA & BIT(7)) == 0)


    #define SD_CLK_OUT_H()          {GPIOASET = BIT(6);}
    #define SD_CLK_OUT_L()          {GPIOACLR = BIT(6);}
    #define SD_CLK_STA()            (GPIOA & BIT(6))

    #define SD_DAT_DIR_OUT()        {GPIOADE |= BIT(8); GPIOADIR &= ~BIT(8);}
    #define SD_DAT_DIR_IN()         {GPIOADIR |= BIT(8); GPIOAPU  |= BIT(8);}
    #define SD_DAT_OUT_H()          {GPIOASET = BIT(8);}
    #define SD_DAT_OUT_L()          {GPIOACLR = BIT(8);}
    #define SD_DAT_STA()            (GPIOA & BIT(8))

    #define SD_CMD_DIR_OUT()        {GPIOADE |= BIT(7); GPIOADIR &= ~BIT(7);}
    #define SD_CMD_DIR_IN()         {GPIOADIR |= BIT(7); GPIOAPU  |= BIT(7);}
    #define SD_CMD_OUT_H()          {GPIOASET = BIT(7);}
    #define SD_CMD_OUT_L()          {GPIOACLR = BIT(7);}
    #define SD_CMD_STA()            (GPIOA & BIT(7))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PA6
    #define SDCMD_IO                IO_PA7
    #define SDDAT_IO                IO_PA8
#else
    #define SD_MUX_IO_INIT()
    #define SD_IO_INIT()
    #define SD_CLK_DIR_IN()
    #define SD_CLK_IN_DIS_PU10K()
    #define SD_CLK_DIR_OUT()
    #define SD_MUX_DETECT_INIT()
    #define SD_MUX_IS_ONLINE()      0
    #define SD_MUX_IS_BUSY()        0
    #define SD_MUX_CMD_IS_BUSY()    0
    #define SD_CMD_MUX_PU300R()
    #define SD_CMD_MUX_PU10K()
    #define SD_CMD_MUX_IS_ONLINE()  0

    #define SD_CLK_OUT_H()
    #define SD_CLK_OUT_L()
    #define SD_CLK_STA()            0

    #define SD_DAT_DIR_OUT()
    #define SD_DAT_DIR_IN()
    #define SD_DAT_OUT_H()
    #define SD_DAT_OUT_L()
    #define SD_DAT_STA()            0

    #define SD_CMD_DIR_OUT()
    #define SD_CMD_DIR_IN()
    #define SD_CMD_OUT_H()
    #define SD_CMD_OUT_L()
    #define SD_CMD_STA()            0

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_NONE
    #define SDCMD_IO                IO_NONE
    #define SDDAT_IO                IO_NONE
#endif


//SD1
#if (SD1_MAPPING == SD0MAP_G1)
    //SDCMD(PA2), SDCLK(PA1), SDDAT0(PA3)
    #define SD1_MUX_IO_INIT()        {GPIOADE |= BIT(2)|BIT(1)|BIT(3);\
                                     GPIOACLR  = BIT(1);\
                                     GPIOADIR &= ~BIT(1);\
                                     GPIOAPU  &= ~BIT(1);\
                                     GPIOADIR |= BIT(2) | BIT(3);\
                                     GPIOAPU  |= BIT(2) | BIT(3);\
                                     GPIOAFEN |= BIT(2) | BIT(3);\
                                     FUNCMCON0 = SD0MAP_G1;}
    #define SD1_IO_INIT()            {GPIOADE |= BIT(2)|BIT(1)|BIT(3);\
                                     GPIOADIR &= ~BIT(1);\
                                     GPIOADIR |= BIT(2) | BIT(3);\
                                     GPIOAPU  |= BIT(2) | BIT(3);\
                                     GPIOAFEN |= BIT(2) | BIT(3);\
                                     FUNCMCON0 = SD0MAP_G1;}
    #define SD1_CLK_DIR_IN()         {GPIOADIR |= BIT(1); GPIOAPU  |= BIT(1);}
    #define SD1_CLK_IN_DIS_PU10K()   {GPIOADIR |= BIT(1); GPIOAPU  &= ~BIT(1);}
    #define SD1_CLK_DIR_OUT()        {GPIOAPU  &= ~BIT(1); GPIOADIR &= ~BIT(1);}
    #define SD1_MUX_DETECT_INIT()    {GPIOADE |= BIT(1); GPIOAPU  |= BIT(1); GPIOADIR |= BIT(1);}
    #define SD1_MUX_IS_ONLINE()      ((GPIOA & BIT(1)) == 0)
    #define SD1_MUX_IS_BUSY()        ((GPIOADIR & BIT(1)) == 0)
    #define SD1_MUX_CMD_IS_BUSY()    (GPIOAPU300 & BIT(2))
    #define SD1_CMD_MUX_PU300R()     {GPIOAPU300 |= BIT(2); GPIOAPU &= ~BIT(2);}
    #define SD1_CMD_MUX_PU10K()      {GPIOAPU |= BIT(2); GPIOAPU300 &= ~BIT(2);}
    #define SD1_DAT_MUX_PU300R()     {GPIOAPU300 |= BIT(3); GPIOAPU &= ~BIT(3);}
    #define SD1_DAT_MUX_PU10K()      {GPIOAPU |= BIT(3); GPIOAPU300 &= ~BIT(3);}
    #define SD1_CMD_MUX_IS_ONLINE()  ((GPIOA & BIT(2)) == 0)


    #define SD1_CLK_OUT_H()          {GPIOASET = BIT(1);}
    #define SD1_CLK_OUT_L()          {GPIOACLR = BIT(1);}
    #define SD1_CLK_STA()            (GPIOA & BIT(1))

    #define SD1_DAT_DIR_OUT()        {GPIOADE |= BIT(3); GPIOADIR &= ~BIT(3);}
    #define SD1_DAT_DIR_IN()         {GPIOADIR |= BIT(3); GPIOAPU  |= BIT(3);}
    #define SD1_DAT_OUT_H()          {GPIOASET = BIT(3);}
    #define SD1_DAT_OUT_L()          {GPIOACLR = BIT(3);}
    #define SD1_DAT_STA()            (GPIOA & BIT(3))

    #define SD1_CMD_DIR_OUT()        {GPIOADE |= BIT(2); GPIOADIR &= ~BIT(2);}
    #define SD1_CMD_DIR_IN()         {GPIOADIR |= BIT(2); GPIOAPU  |= BIT(2);}
    #define SD1_CMD_OUT_H()          {GPIOASET = BIT(2);}
    #define SD1_CMD_OUT_L()          {GPIOACLR = BIT(2);}
    #define SD1_CMD_STA()            (GPIOA & BIT(2))

    #define SD1_DAT_DIS_UP()
    #define SD1_DAT_RES_UP()

    #define SD1CLK_IO                IO_PA1
    #define SD1CMD_IO                IO_PA2
    #define SD1DAT_IO                IO_PA3

#elif (SD1_MAPPING == SD0MAP_G2)
    //SDCMD(PB1), SDCLK(PB0), SDDAT0(PB2)
    #define SD1_MUX_IO_INIT()       {GPIOBDE |= BIT(1)|BIT(0)|BIT(2);\
                                    GPIOBCLR  = BIT(0);\
                                    GPIOBDIR &= ~BIT(0);\
                                    GPIOBPU  &= ~BIT(0);\
                                    GPIOBDIR |= BIT(1) | BIT(2);\
                                    GPIOBPU  |= BIT(1) | BIT(2);\
                                    GPIOBFEN |= BIT(1) | BIT(2);\
                                    FUNCMCON0 = SD0MAP_G2;}
    #define SD1_IO_INIT()           {GPIOBDE |= BIT(1)|BIT(0)|BIT(2);\
                                    GPIOBDIR &= ~BIT(0);\
                                    GPIOBDIR |= BIT(1) | BIT(2);\
                                    GPIOBPU  |= BIT(1) | BIT(2);\
                                    GPIOBFEN |= BIT(1) | BIT(2);\
                                    FUNCMCON0 = SD0MAP_G2;}
    #define SD1_CLK_DIR_IN()        {GPIOBDIR |= BIT(0); GPIOBPU  |= BIT(0);}
    #define SD1_CLK_IN_DIS_PU10K()  {GPIOBDIR |= BIT(0); GPIOBPU  &= ~BIT(0);}
    #define SD1_CLK_DIR_OUT()       {GPIOBPU  &= ~BIT(0); GPIOBDIR &= ~BIT(0);}
    #define SD1_MUX_DETECT_INIT()   {GPIOBDE |= BIT(0); GPIOBPU  |= BIT(0); GPIOBDIR |= BIT(0);}
    #define SD1_MUX_IS_ONLINE()     ((GPIOB & BIT(0)) == 0)
    #define SD1_MUX_IS_BUSY()       ((GPIOBDIR & BIT(0)) == 0)
    #define SD1_MUX_CMD_IS_BUSY()   (GPIOBPU300 & BIT(1))
    #define SD1_CMD_MUX_PU300R()    {GPIOBPU300 |= BIT(1); GPIOBPU &= ~BIT(1);}
    #define SD1_CMD_MUX_PU10K()     {GPIOBPU |= BIT(1); GPIOBPU300 &= ~BIT(1);}
    #define SD1_DAT_MUX_PU300R()    {GPIOBPU300 |= BIT(2); GPIOBPU &= ~BIT(2);}
    #define SD1_DAT_MUX_PU10K()     {GPIOBPU |= BIT(2); GPIOBPU300 &= ~BIT(2);}
    #define SD1_CMD_MUX_IS_ONLINE() ((GPIOB & BIT(1)) == 0)


    #define SD1_CLK_OUT_H()         {GPIOBSET = BIT(0);}
    #define SD1_CLK_OUT_L()         {GPIOBCLR = BIT(0);}
    #define SD1_CLK_STA()           (GPIOB & BIT(0))

    #define SD1_DAT_DIR_OUT()       {GPIOBDE |= BIT(2); GPIOBDIR &= ~BIT(2);}
    #define SD1_DAT_DIR_IN()        {GPIOBDIR |= BIT(2); GPIOBPU  |= BIT(2);}
    #define SD1_DAT_OUT_H()         {GPIOBSET = BIT(2);}
    #define SD1_DAT_OUT_L()         {GPIOBCLR = BIT(2);}
    #define SD1_DAT_STA()           (GPIOB & BIT(2))

    #define SD1_CMD_DIR_OUT()       {GPIOBDE |= BIT(1); GPIOBDIR &= ~BIT(1);}
    #define SD1_CMD_DIR_IN()        {GPIOBDIR |= BIT(1); GPIOBPU  |= BIT(1);}
    #define SD1_CMD_OUT_H()         {GPIOBSET = BIT(1);}
    #define SD1_CMD_OUT_L()         {GPIOBCLR = BIT(1);}
    #define SD1_CMD_STA()           (GPIOB & BIT(1))

    #define SD1_DAT_DIS_UP()
    #define SD1_DAT_RES_UP()

    #define SD1CLK_IO               IO_PB0
    #define SD1CMD_IO               IO_PB1
    #define SD1DAT_IO               IO_PB2

#elif (SD1_MAPPING == SD0MAP_G3)
    //SDCMD(PB5), SDCLK(PB4), SDDAT0(PB6)
    #define SD1_MUX_IO_INIT()       {GPIOBDE |= BIT(5)|BIT(4)|BIT(6);\
                                    GPIOBCLR  = BIT(4);\
                                    GPIOBDIR &= ~BIT(4);\
                                    GPIOBPU  &= ~BIT(4);\
                                    GPIOBDIR |= BIT(5) | BIT(6);\
                                    GPIOBPU  |= BIT(5) | BIT(6);\
                                    GPIOBFEN |= BIT(5) | BIT(6);\
                                    FUNCMCON0 = SD0MAP_G3;}
    #define SD1_IO_INIT()           {GPIOBDE |= BIT(5)|BIT(4)|BIT(6);\
                                    GPIOBDIR &= ~BIT(1);\
                                    GPIOBDIR |= BIT(5) | BIT(6);\
                                    GPIOBPU  |= BIT(5) | BIT(6);\
                                    GPIOBFEN |= BIT(5) | BIT(6);\
                                    FUNCMCON0 = SD0MAP_G3;}
    #define SD1_CLK_DIR_IN()        {GPIOBDIR |= BIT(4); GPIOBPU  |= BIT(4);}
    #define SD1_CLK_IN_DIS_PU10K()  {GPIOBDIR |= BIT(4); GPIOBPU  &= ~BIT(4);}
    #define SD1_CLK_DIR_OUT()       {GPIOBPU  &= ~BIT(4); GPIOBDIR &= ~BIT(4);}
    #define SD1_MUX_DETECT_INIT()   {GPIOBDE |= BIT(4); GPIOBPU  |= BIT(4); GPIOBDIR |= BIT(4);}
    #define SD1_MUX_IS_ONLINE()     ((GPIOB & BIT(4)) == 0)
    #define SD1_MUX_IS_BUSY()       ((GPIOBDIR & BIT(4)) == 0)
    #define SD1_MUX_CMD_IS_BUSY()   (GPIOBPU300 & BIT(5))
    #define SD1_CMD_MUX_PU300R()    {GPIOBPU300 |= BIT(5); GPIOBPU &= ~BIT(5);}
    #define SD1_CMD_MUX_PU10K()     {GPIOBPU |= BIT(5); GPIOBPU300 &= ~BIT(5);}
    #define SD1_DAT_MUX_PU300R()    {GPIOBPU300 |= BIT(6); GPIOBPU &= ~BIT(6);}
    #define SD1_DAT_MUX_PU10K()     {GPIOBPU |= BIT(6); GPIOBPU300 &= ~BIT(6);}
    #define SD1_CMD_MUX_IS_ONLINE() ((GPIOB & BIT(5)) == 0)


    #define SD1_CLK_OUT_H()          {GPIOBSET = BIT(4);}
    #define SD1_CLK_OUT_L()          {GPIOBCLR = BIT(4);}
    #define SD1_CLK_STA()            (GPIOB & BIT(4))

    #define SD1_DAT_DIR_OUT()        {GPIOBDE |= BIT(6); GPIOBDIR &= ~BIT(6);}
    #define SD1_DAT_DIR_IN()         {GPIOBDIR |= BIT(6); GPIOBPU  |= BIT(6);}
    #define SD1_DAT_OUT_H()          {GPIOBSET = BIT(6);}
    #define SD1_DAT_OUT_L()          {GPIOBCLR = BIT(6);}
    #define SD1_DAT_STA()            (GPIOB & BIT(6))

    #define SD1_CMD_DIR_OUT()        {GPIOBDE |= BIT(5); GPIOBDIR &= ~BIT(5);}
    #define SD1_CMD_DIR_IN()         {GPIOBDIR |= BIT(5); GPIOBPU  |= BIT(5);}
    #define SD1_CMD_OUT_H()          {GPIOBSET = BIT(5);}
    #define SD1_CMD_OUT_L()          {GPIOBCLR = BIT(5);}
    #define SD1_CMD_STA()            (GPIOB & BIT(5))

    #define SD1_DAT_DIS_UP()
    #define SD1_DAT_RES_UP()

    #define SD1CLK_IO                IO_PB4
    #define SD1CMD_IO                IO_PB5
    #define SD1DAT_IO                IO_PB6

#elif (SD1_MAPPING == SD0MAP_G4)
    //SDCMD(PA7), SDCLK(PA6), SDDAT0(PB8)
    #define SD1_MUX_IO_INIT()       {GPIOADE |= BIT(7)|BIT(6)|BIT(8);\
                                    GPIOACLR  = BIT(6);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOAPU  &= ~BIT(6);\
                                    GPIOADIR |= BIT(7) | BIT(8);\
                                    GPIOAPU  |= BIT(7) | BIT(8);\
                                    GPIOAFEN |= BIT(7) | BIT(8);\
                                    FUNCMCON0 = SD0MAP_G4;}
    #define SD1_IO_INIT()           {GPIOADE |= BIT(7)|BIT(6)|BIT(8);\
                                    GPIOADIR &= ~BIT(6);\
                                    GPIOADIR |= BIT(7) | BIT(8);\
                                    GPIOAPU  |= BIT(7) | BIT(8);\
                                    GPIOAFEN |= BIT(7) | BIT(8);\
                                    FUNCMCON0 = SD0MAP_G4;}
    #define SD1_CLK_DIR_IN()        {GPIOADIR |= BIT(6); GPIOAPU  |= BIT(6);}
    #define SD1_CLK_IN_DIS_PU10K()  {GPIOADIR |= BIT(6); GPIOAPU  &= ~BIT(6);}
    #define SD1_CLK_DIR_OUT()       {GPIOAPU  &= ~BIT(6); GPIOADIR &= ~BIT(6);}
    #define SD1_MUX_DETECT_INIT()   {GPIOADE |= BIT(6); GPIOAPU  |= BIT(6); GPIOADIR |= BIT(6);}
    #define SD1_MUX_IS_ONLINE()     ((GPIOA & BIT(6)) == 0)
    #define SD1_MUX_IS_BUSY()       ((GPIOADIR & BIT(6)) == 0)
    #define SD1_MUX_CMD_IS_BUSY()   (GPIOAPU300 & BIT(7))
    #define SD1_CMD_MUX_PU300R()    {GPIOAPU300 |= BIT(7); GPIOAPU &= ~BIT(7);}
    #define SD1_CMD_MUX_PU10K()     {GPIOAPU |= BIT(7); GPIOAPU300 &= ~BIT(7);}
    #define SD1_DAT_MUX_PU300R()    {GPIOAPU300 |= BIT(8); GPIOAPU &= ~BIT(8);}
    #define SD1_DAT_MUX_PU10K()     {GPIOAPU |= BIT(8); GPIOAPU300 &= ~BIT(8);}
    #define SD1_CMD_MUX_IS_ONLINE() ((GPIOA & BIT(7)) == 0)


    #define SD1_CLK_OUT_H()         {GPIOASET = BIT(6);}
    #define SD1_CLK_OUT_L()         {GPIOACLR = BIT(6);}
    #define SD1_CLK_STA()           (GPIOA & BIT(6))

    #define SD1_DAT_DIR_OUT()       {GPIOADE |= BIT(8); GPIOADIR &= ~BIT(8);}
    #define SD1_DAT_DIR_IN()        {GPIOADIR |= BIT(8); GPIOAPU  |= BIT(8);}
    #define SD1_DAT_OUT_H()         {GPIOASET = BIT(8);}
    #define SD1_DAT_OUT_L()         {GPIOACLR = BIT(8);}
    #define SD1_DAT_STA()           (GPIOA & BIT(8))

    #define SD1_CMD_DIR_OUT()       {GPIOADE |= BIT(7); GPIOADIR &= ~BIT(7);}
    #define SD1_CMD_DIR_IN()        {GPIOADIR |= BIT(7); GPIOAPU  |= BIT(7);}
    #define SD1_CMD_OUT_H()         {GPIOASET = BIT(7);}
    #define SD1_CMD_OUT_L()         {GPIOACLR = BIT(7);}
    #define SD1_CMD_STA()           (GPIOA & BIT(7))

    #define SD1_DAT_DIS_UP()
    #define SD1_DAT_RES_UP()

    #define SD1CLK_IO                IO_PA6
    #define SD1CMD_IO                IO_PA7
    #define SD1DAT_IO                IO_PA8
#else
    #define SD1_MUX_IO_INIT()
    #define SD1_IO_INIT()
    #define SD1_CLK_DIR_IN()
    #define SD1_CLK_IN_DIS_PU10K()
    #define SD1_CLK_DIR_OUT()
    #define SD1_MUX_DETECT_INIT()
    #define SD1_MUX_IS_ONLINE()      0
    #define SD1_MUX_IS_BUSY()        0
    #define SD1_MUX_CMD_IS_BUSY()    0
    #define SD1_CMD_MUX_PU300R()
    #define SD1_CMD_MUX_PU10K()
    #define SD1_CMD_MUX_IS_ONLINE()  0

    #define SD1_CLK_OUT_H()
    #define SD1_CLK_OUT_L()
    #define SD1_CLK_STA()            0

    #define SD1_DAT_DIR_OUT()
    #define SD1_DAT_DIR_IN()
    #define SD1_DAT_OUT_H()
    #define SD1_DAT_OUT_L()
    #define SD1_DAT_STA()            0

    #define SD1_CMD_DIR_OUT()
    #define SD1_CMD_DIR_IN()
    #define SD1_CMD_OUT_H()
    #define SD1_CMD_OUT_L()
    #define SD1_CMD_STA()            0

    #define SD1_DAT_DIS_UP()
    #define SD1_DAT_RES_UP()

    #define SD1CLK_IO                IO_NONE
    #define SD1CMD_IO                IO_NONE
    #define SD1DAT_IO                IO_NONE
#endif


#endif //__IO_DEF_H
