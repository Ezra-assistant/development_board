#ifndef _MIDI_VOICE_H
#define _MIDI_VOICE_H

#define VOICE_PROG_GRANDPNO         0       //原声大钢琴
#define VOICE_PROG_BRITEPNO         1       //明亮钢琴
#define VOICE_PROG_EGRAND           2       //三角电钢琴
#define VOICE_PROG_HNKYTONK         3       //酒吧钢琴
#define VOICE_PROG_EPIANO1          4       //电钢琴1
#define VOICE_PROG_EPIANO2          5       //电钢琴2
#define VOICE_PROG_HARPSI           6       //拨弦古钢琴
#define VOICE_PROG_CLAVI            7       //击弦古钢琴
#define VOICE_PROG_CELESTA          8       //钢片琴
#define VOICE_PROG_GLOCKEN          9       //钟琴

#define VOICE_PROG_MUSICBOX         10      //八音盒
#define VOICE_PROG_VIBRA            11      //颤音琴
#define VOICE_PROG_MARIMBA          12      //马林巴
#define VOICE_PROG_XYLOPHON         13      //木琴
#define VOICE_PROG_TUBULBEL         14      //管琴
#define VOICE_PROG_DULCIMER         15      //德西马琴
#define VOICE_PROG_DRAWORGN         16      //拉杆风琴
#define VOICE_PROG_PERCORGN         17      //打击风琴
#define VOICE_PROG_ROCKORGN         18      //摇滚风琴
#define VOICE_PROG_CHURORGN         19      //教堂风琴

#define VOICE_PROG_REEDORGN         20      //簧管风琴
#define VOICE_PROG_ACORDION         21      //手风琴
#define VOICE_PROG_HARMNICA         22      //口琴
#define VOICE_PROG_TANGOACD         23      //探戈手风琴
#define VOICE_PROG_NYLONGTR         24      //尼龙吉他
#define VOICE_PROG_STEELGTR         25      //钢弦吉他
#define VOICE_PROG_JAZZGTR          26      //爵士电吉他
#define VOICE_PROG_CLEANGTR         27      //清音电吉他
#define VOICE_PROG_MUTEDGTR         28      //制音电吉他
#define VOICE_PROG_OVRDRIVER        29      //过载吉他

#define VOICE_PROG_DISTGTR          30      //失真吉他
#define VOICE_PROG_GTRHARMO         31      //吉他泛音
#define VOICE_PROG_ACOBASS          32      //原声贝司
#define VOICE_PROG_FNGRBASS         33      //指弹电贝司
#define VOICE_PROG_PICKBASS         34      //拨片电贝司
#define VOICE_PROG_FRETLESS         35      //无品贝司
#define VOICE_PROG_SLAPBAS1         36      //击弦贝司1
#define VOICE_PROG_SLAPBAS2         37      //击弦贝司2
#define VOICE_PROG_SYNBASS1         38      //合成贝司1
#define VOICE_PROG_SYNBASS2         39      //合成贝司2

#define VOICE_PROG_VIOLIN           40      //小提琴
#define VOICE_PROG_VIOLA            41      //中提琴
#define VOICE_PROG_CELLO            42      //大提琴
#define VOICE_PROG_CONTRABS         43      //低音提琴
#define VOICE_PROG_TREMSTR          44      //颤音弦乐
#define VOICE_PROG_PIZZSTR          45      //拨弦弦乐
#define VOICE_PROG_HARP             46      //竖琴
#define VOICE_PROG_TIMPANI          47      //定音鼓
#define VOICE_PROG_STRINGS1         48      //弦乐合奏1
#define VOICE_PROG_STRINGS2         49      //弦乐合奏2

#define VOICE_PROG_SYNSTRS1         50      //合成弦乐1
#define VOICE_PROG_SYNSTRS2         51      //合成弦乐2
#define VOICE_PROG_CHOIRAAH         52      //"啊"声合唱
#define VOICE_PROG_VOICEOOH         53      //"欧"声合唱
#define VOICE_PROG_SYNVOICE         54      //合成人声
#define VOICE_PROG_ORCHHIT          55      //管弦齐奏
#define VOICE_PROG_TRUMPET          56      //小号
#define VOICE_PROG_TROMBONE         57      //长号
#define VOICE_PROG_TUBA             58      //低音号
#define VOICE_PROG_MUTETRP          59      //弱音小号

#define VOICE_PROG_FRHORN           60      //圆号
#define VOICE_PROG_BRASS            61      //铜管合奏
#define VOICE_PROG_SYNBRAS1         62      //合成铜管1
#define VOICE_PROG_SYNBRAS2         63      //合成铜管2
#define VOICE_PROG_SPRNOSAX         64      //高音萨克斯
#define VOICE_PROG_ALTOSAX          65      //中音萨克斯
#define VOICE_PROG_TENORSAX         66      //次中音萨克斯
#define VOICE_PROG_BARISAX          67      //低音萨克斯
#define VOICE_PROG_OBOE             68      //高音双簧管
#define VOICE_PROG_ENGHORN          69      //中音双簧管

#define VOICE_PROG_BASSOON          70      //低音双簧管
#define VOICE_PROG_CLARINET         71      //单簧管
#define VOICE_PROG_PICCOLO          72      //短笛
#define VOICE_PROG_FLUTE            73      //长笛
#define VOICE_PROG_RECORDER         74      //竖笛
#define VOICE_PROG_PANFLUTE         75      //排箫
#define VOICE_PROG_BOTTLE           76      //吹瓶
#define VOICE_PROG_SHAKHCHI         77      //尺八
#define VOICE_PROG_WHISTLE          78      //口哨
#define VOICE_PROG_OCARINA          79      //陶笛

#define VOICE_PROG_SQUARELD         80      //方波主奏
#define VOICE_PROG_SAWLEAD          81      //锯齿波主奏
#define VOICE_PROG_CALIOPLD         82      //女神主奏
#define VOICE_PROG_CHIFFLD          83      //啼鸟主奏
#define VOICE_PROG_CHARANLD         84      //波兰主奏
#define VOICE_PROG_VOICELD          85      //人声主奏
#define VOICE_PROG_FIFTHSLD         86      //五度主奏
#define VOICE_PROG_BASSLEAD         87      //贝司主奏
#define VOICE_PROG_NEWAGEPD         88      //新世纪垫
#define VOICE_PROG_WARMPAD          89      //温暖垫

#define VOICE_PROG_POLYSYPD         90      //复音合成垫
#define VOICE_PROG_CHOIRPAD         91      //合唱垫
#define VOICE_PROG_BOWEDPAD         92      //弓形垫
#define VOICE_PROG_METAPAD          93      //金属垫
#define VOICE_PROG_HALOPAD          94      //环形垫
#define VOICE_PROG_SWEEPPAD         95      //锯擦垫
#define VOICE_PROG_FXRAIN           96      //雨声
#define VOICE_PROG_SOUNDTRK         97      //声轨
#define VOICE_PROG_CRYSTAL          98      //水晶
#define VOICE_PROG_ATMOSPHR         99      //空气

#define VOICE_PROG_BRIGHT           100     //明亮
#define VOICE_PROG_GOBLINS          101     //小精灵
#define VOICE_PROG_ECHOES           102     //回声
#define VOICE_PROG_SCIFI            103     //科幻
#define VOICE_PROG_SITAR            104     //锡塔琴
#define VOICE_PROG_BANJO            105     //班卓琴
#define VOICE_PROG_SHAMISEN         106     //三味线
#define VOICE_PROG_KOTO             107     //十三弦筝
#define VOICE_PROG_KALIMBA          108     //卡林巴
#define VOICE_PROG_BAGPIPE          109     //风笛

#define VOICE_PROG_FIDDLE           110     //民间小提琴
#define VOICE_PROG_SHANAI           111     //山奈琴
#define VOICE_PROG_TNKLBELL         112     //清脆铃
#define VOICE_PROG_AGOGO            113     //阿果果
#define VOICE_PROG_STEELDRM         114     //钢鼓
#define VOICE_PROG_WOODBLOK         115     //合梆
#define VOICE_PROG_TAIKORUM         116     //太鼓
#define VOICE_PROG_MELODTOM         117     //旋律通鼓
#define VOICE_PROG_SYNDRUM          118     //合成鼓
#define VOICE_PROG_REVCYMBL         119     //反向吊镲

#define VOICE_PROG_FRETNOIZ         120     //吉他滑品音
#define VOICE_PROG_BRTHNOIZ         121     //呼吸
#define VOICE_PROG_SEASHORE         122     //海浪
#define VOICE_PROG_TWEET            123     //鸟鸣
#define VOICE_PROG_TELPHONE         124     //电话
#define VOICE_PROG_HELICPTR         125     //直升机
#define VOICE_PROG_APPLAUSE         126     //掌声
#define VOICE_PROG_GUNSHOT          127     //枪声

#define VOICE_PROG_STANDSET         128     //标准鼓组
#define VOICE_PROG_ROOMSET          129     //房间鼓组
#define VOICE_PROG_POWERSET         130     //强力鼓组
#define VOICE_PROG_ELECTSET         131     //电子鼓组
#define VOICE_PROG_ANALGSET         132     //模拟鼓组
#define VOICE_PROG_JAZZSET          133     //爵士鼓组
#define VOICE_PROG_BRUSHSET         134     //刷棒鼓组
#define VOICE_PROG_ORCHESET         135     //管弦乐鼓组

#endif
