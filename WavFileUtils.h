#ifndef WAVFILEUTILS_H
#define WAVFILEUTILS_H

#include "typedef.h"
#include <iostream>
#include <string>

using namespace std;

#define WAV_HEADER_LEN (16) //wav文件头结构的标准长度, 超过此长度说明有拓展信息

typedef struct _WAV_FORMAT
{
    char ChunkID[4];         /* "RIFF" */
    uint32 ChunkSize;       /* 从下一个字段首地址开始到文件末尾的总字节数。该字段的数值加 8 为当前文件的实际长度。 */
    char Format[4];          /* "WAVE" */

    /* sub-chunk "fmt" */
    char Subchunk1ID[4];     /* "fmt " */
    uint32 Subchunk1Size;   /* 标记sub-chunk段的长度, 其数值不确定,取决于编码格式。可以是 16、 18 、20、40 等, 通常为16 */
    uint16 AudioFormat;     /* 常见的 WAV 文件使用 PCM 脉冲编码调制格式,该数值通常为 1 */
    uint16 NumChannnels;    /* 声道个数 单声道为 1,立体声或双声道为 2 */
    uint32 SampleRate;      /* 采样频率 每个声道单位时间采样次数。常用的采样频率有 11025, 22050 和 44100 kHz。 */
    uint32 ByteRate;        /* 数据传输速率,该数值为:声道数×采样频率×每样本的数据位数/8。播放软件利用此值可以估计缓冲区的大小。 */
    uint16 BlockAlign;      /* 数据块对齐单位 采样帧大小。该数值为:声道数×位数/8。播放软件需要一次处理多个该值大小的字节数据,用该数值调整缓冲区。 */
    uint16 BitsPerSample;   /* 采样位数 存储每个采样值所用的二进制数位数。常见的位数有 4、8、12、16、24、32 */
}
WAV_FORMAT;

typedef struct _RIFF_HEADER
{
    char title[4];
    uint32 len;
}
RIFF_HEADER;

/* 程序内部使用的wav信息结构 */
typedef struct _WavInfo
{
    uint16 numChannels;
    uint32 sampleRate;
    uint16 bitsPerSample;
    uint32 dataStartPos;
    uint32 dataLen;
}
WavInfo;

typedef enum _WavFileRet
{
    WAV_LOAD_OK = 0,
    WAV_OPEN_ERR,
    WAV_READ_ERR,
    WAV_RIFF_ERR,
    WAV_WAVE_ERR,
    WAV_FMT_ERR,
    WAV_NOT_PCM_ERR,
    WAV_NO_DATA_ERR,
}
WavFileRet;

class WavFileUtils
{
public:
    WavFileUtils();
    /* Wav信息读取 */
	WavFileRet load(std::string path);
    bool getInfo(WavInfo& info);

	/* Wav生成 */
	int32 create();

private:
    FILE* m_fp;
    WAV_FORMAT m_wavInfo;
    uint32 m_dataStartPos;
    uint32 m_dataLen;

    bool m_isLoadOK;
};

#endif // WAVFILEUTILS_H
