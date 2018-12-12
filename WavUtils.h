#ifndef WAVFILEUTILS_H
#define WAVFILEUTILS_H

#include "typedef.h"
#include <iostream>
#include <string>

using namespace std;

#define WAV_HEADER_LEN (16) //wav文件头结构的标准长度, 超过此长度说明有拓展信息

#ifndef DATA_FORMAT_STRUCT_
#define DATA_FORMAT_STRUCT_
typedef enum _DataFormat
{
	FORMAT_INT8,
	FORMAT_INT16,
	FORMAT_INT32,
	FORMAT_UNKNOWN,
}
DataFormat;
#endif /* DATA_FORMAT_STRUCT_ */

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
    uint32 dataStartPos;	//读取时有效, 写入时无效
    uint32 dataLen;			//读取时有效, 写入时无效
}
WavInfo;

typedef enum _WavUtilsRet
{
    WAV_UTILS_OK = 0,
    WAV_OPEN_ERR,
    WAV_READ_ERR,
    WAV_RIFF_ERR,
    WAV_WAVE_ERR,
    WAV_FMT_ERR,
    WAV_NOT_PCM_ERR,
    WAV_NO_DATA_ERR,
	WAV_UTILS_INPUT_ERROR,	//输入参数有误
	WAV_NOT_IN_RIGHT_MODE,	///当前所处模式有误
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
    /* Wav信息读取 */
	WavUtilsRet load(std::string path);
    bool getInfo(WavInfo& info);

	/* Wav生成 */
	//创建wav文件, 并写入文件头
	WavUtilsRet create(std::string path, const WavInfo& info);	
	//写入wav文件
	WavUtilsRet write(const char* buf, int32 size, DataFormat dataFormat = FORMAT_INT32);
	//关闭wav文件, 参数isFixHeader标记是否对文件头进行修改(目前只修正文件中数据量)
	void close(bool isFixHeader);	

private:
	WavUtilsMode m_mode;
    FILE* m_fp;
    WAV_FORMAT m_wavFormat;
    uint32 m_dataStartPos;
    uint32 m_dataLen;

	//数据类型和每个数据点长度之间的相互转化
	DataFormat transBitPerData2DataFormat(uint16 bitPerData);
	uint16 transDataFormat2BitPerData(DataFormat dataFormat);

	//************************************
	// Method:    transDataFormat
	// Returns:   char*	- 转换后数据存放的buffer
	// Function:  将输入buf中的数据类型转换进行转换
	// Parameter: const void * bufIn - 输入buffer
	// Parameter: int32 sizeIn - 输入buffer的长度, 单位为bufIn类型的个数, 例如多少个float, 不表示字节
	// Parameter: DataFormat formatIn - 输入buffer的数据类型
	// Parameter: DataFormat formatOut - 要变成那种数据类型的输出
	//************************************
	char* transDataFormat(const void* pBufIn, int32 sizeIn, DataFormat formatIn, DataFormat formatOut);

	//************************************
	// Method:    freeTranDataFormatReturnBuf
	// Returns:   void
	// Function: 释放transDataFormat函数返回的内存空间
	// Parameter: char * pBuf
	//************************************
	void freeTranDataFormatReturnBuf(char* pBuf);
	//数据转换时临时使用的buffer
	char* m_pDataTransTempBuf;
};

#endif // WAVFILEUTILS_H
