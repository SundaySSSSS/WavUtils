#include "WavUtils.h"

using namespace std;

WavUtils::WavUtils()
{
    m_fp = NULL;
	m_mode = WAV_UTILS_UNDEF_MODE;
}

WavUtilsRet WavUtils::load(string path)
{
	if (m_mode != WAV_UTILS_UNDEF_MODE)
		return WAV_NOT_IN_RIGHT_MODE;

	WavUtilsRet ret = WAV_UTILS_OK;
    if (m_fp != NULL)
    {
        fclose(m_fp);
    }

    m_fp = fopen(path.c_str(), "rb");
    if (m_fp == NULL)
    {   //文件打开失败
        ret = WAV_OPEN_ERR;
    }

    size_t read_ret = fread(&m_wavFormat, sizeof(m_wavFormat), 1, m_fp);
    if (read_ret != 1)
    {   //读取错误
        ret = WAV_READ_ERR;
    }
    else if (string(m_wavFormat.ChunkID, 4) != "RIFF")
    {   //开头不是约定的"RIFF"字符串
        ret = WAV_RIFF_ERR;
    }
    else if (string(m_wavFormat.Format, 4) != "WAVE")
    {   //格式不是固定字符串"WAVE"
        ret = WAV_WAVE_ERR;
    }
    else if (string(m_wavFormat.Subchunk1ID, 4) != "fmt ")
    {   //不是固定的字符串"fmt "
        ret = WAV_FMT_ERR;
    }
    else if (m_wavFormat.AudioFormat != 1)
    {   //不是Windows PCM格式
        ret = WAV_NOT_PCM_ERR;
    }
    else
    {
        cout << "Sample Rate: " << m_wavFormat.SampleRate << endl;
        cout << "Channel Num: " << m_wavFormat.NumChannnels << endl;
        cout << "Bits Per Sample: " << m_wavFormat.BitsPerSample << endl;

        //开始寻找数据段
        if (m_wavFormat.Subchunk1Size - WAV_HEADER_LEN != 0)
            fseek(m_fp, m_wavFormat.Subchunk1Size - WAV_HEADER_LEN, SEEK_CUR);

        int32 curPos = 0;
        while(curPos != EOF)
        {
            RIFF_HEADER riffHeader;
            memset(&riffHeader, 0, sizeof(riffHeader));
            read_ret = fread(&riffHeader, sizeof(riffHeader), 1, m_fp);
            if (read_ret != 1)
            {   //读取数据段头信息失败
                ret = WAV_READ_ERR;
                break;
            }
            if (string(riffHeader.title, 4) != "data")
            {   //不是数据段, 继续寻找
                cout << "title: " << string(riffHeader.title, 4).c_str() <<
                            " len: " << riffHeader.len << endl;
                if (riffHeader.len != 0)
                    fseek(m_fp, riffHeader.len, SEEK_CUR);
            }
            else
            {
                cout << "found data seg, len = " << riffHeader.len << endl;
                m_dataStartPos = ftell(m_fp);
                m_dataLen = riffHeader.len;
                break;
            }
            curPos = ftell(m_fp);
            if (curPos == EOF)
            {   //文件结束, 尚未找到data段
                ret = WAV_NO_DATA_ERR;
            }
        }
    }
    fclose(m_fp);
    if (ret == WAV_UTILS_OK)
		m_mode = WAV_UTILS_READ_MODE;
    return ret;
}

bool WavUtils::getInfo(WavInfo &info)
{
	if (WAV_UTILS_READ_MODE == m_mode)
    {
        info.numChannels = m_wavFormat.NumChannnels;
        info.bitsPerSample = m_wavFormat.BitsPerSample;
        info.sampleRate = m_wavFormat.SampleRate;
        info.dataStartPos = m_dataStartPos;
        info.dataLen = m_dataLen;
        return true;
    }
    else
        return false;
}

WavUtilsRet WavUtils::create(std::string path, const WavInfo& info)
{
	if (m_mode != WAV_UTILS_UNDEF_MODE)
		return WAV_NOT_IN_RIGHT_MODE;

	//将输入的wavInfo转换为写入文件的WavFormat
	WAV_FORMAT wavFormat;
	
	memcpy(wavFormat.ChunkID, "RIFF", sizeof(wavFormat.ChunkID));
	memcpy(wavFormat.Format, "WAVE", sizeof(wavFormat.Format));
	memcpy(wavFormat.Subchunk1ID, "fmt ", sizeof(wavFormat.Subchunk1ID));
	wavFormat.Subchunk1Size = 16;
	wavFormat.AudioFormat = 1;
	if (info.numChannels == 1 || info.numChannels == 2)
		wavFormat.NumChannnels = info.numChannels;
	else
		return WAV_UTILS_INPUT_ERROR;
	wavFormat.SampleRate = info.sampleRate;
	wavFormat.ByteRate = info.numChannels * info.sampleRate * info.bitsPerSample / 8;
	wavFormat.BlockAlign = info.numChannels * info.bitsPerSample / 8;
	wavFormat.BitsPerSample = info.bitsPerSample;

	m_fp = fopen(path.c_str(), "wb+");
	if (m_fp == NULL)
		return WAV_OPEN_ERR;
	//写入文件头
	fwrite(&wavFormat, sizeof(wavFormat), 1, m_fp);

	return WAV_UTILS_OK;
}


