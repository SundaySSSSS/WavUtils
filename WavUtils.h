#ifndef WAVUTILS_H
#define WAVUTILS_H

#include "typedef.h"
#include <iostream>
#include <string>

using namespace std;

#define WAV_HEADER_LEN (16) //wav文件头结构的标准长度, 超过此长度说明有拓展信息



#pragma pack(push, 1)
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
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _RIFF_HEADER
{
    char title[4];
    uint32 len;
}
RIFF_HEADER;
#pragma pack(pop)

/* 程序内部使用的wav信息结构 */
typedef struct _WavInfo
{
    uint16 numChannels;
    uint32 sampleRate;
    uint16 bitsPerSample;
	bool isFloat;			//是否为浮点型数据
    uint32 dataStartPos;	//读取时有效, 写入时无效
    uint32 dataLen;			//读取时有效, 写入时无效
}
WavInfo;

typedef enum _WavUtilsRet
{
    WAV_UTILS_OK = 0,
    WAV_UTILS_OPEN_ERR,
    WAV_UTILS_READ_ERR,
	WAV_UTILS_WRITE_ERR,
    WAV_UTILS_RIFF_ERR,		//开头不是约定的"RIFF"字符串
    WAV_UTILS_WAVE_ERR,		//格式不是固定字符串"WAVE"
    WAV_UTILS_FMT_ERR,		//不是固定的字符串"fmt "
    WAV_UTILS_NOT_PCM_ERR,	//不是Windows PCM格式
    WAV_UTILS_NO_DATA_ERR,	//文件结束, 尚未找到data段
	WAV_UTILS_INPUT_ERROR,	//输入参数有误
	WAV_UTILS_NOT_IN_RIGHT_MODE,	//当前所处模式有误
}
WavUtilsRet;

//当前wav文件模式
typedef enum _WavUtilsMode
{
	WAV_UTILS_UNDEF_MODE,	//尚未确定的状态, 在此状态下可以读wav或写wav
	WAV_UTILS_WRITE_MODE,	//wav写模式
	WAV_UTILS_READ_MODE,	//wav读模式
}
WavUtilsMode;

class WavUtils
{
public:
    WavUtils();
	virtual ~WavUtils();
    /* Wav信息读取 */
	WavUtilsRet load(std::string path);
    bool getInfo(WavInfo& info);

	/* Wav生成 */
	//创建wav文件, 并写入文件头
	WavUtilsRet create(std::string path, const WavInfo& info);	
	//写入wav文件
	WavUtilsRet write(const char* buf, int32 size);
	//关闭wav文件, 在关闭时, 会根据统计的数据量写入文件头
	WavUtilsRet close();

	

private:
	WavUtilsMode m_mode;
    FILE* m_fp;
    WAV_FORMAT m_wavFormat;
    int32 m_dataStartPos;
    int32 m_dataLen;
};

#endif // WAVUTILS_H
