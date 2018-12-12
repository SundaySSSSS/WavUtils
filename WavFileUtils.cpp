#include "WavFileUtils.h"

using namespace std;

WavFileUtils::WavFileUtils()
{
    m_fp = NULL;
    m_isLoadOK = false;
}

int32 WavFileUtils::load(string path)
{
    int32 ret = WAV_LOAD_OK;
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
        cout << "Sample Rate: " << m_wavFormat.SampleRate;
        cout << "Channel Num: " << m_wavFormat.NumChannnels;
        cout << "Bits Per Sample: " << m_wavFormat.BitsPerSample;

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
                            " len: " << riffHeader.len;
                if (riffHeader.len != 0)
                    fseek(m_fp, riffHeader.len, SEEK_CUR);
            }
            else
            {
                cout << "found data seg, len = " << riffHeader.len;
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
    if (ret == WAV_LOAD_OK)
        m_isLoadOK = true;
    return ret;
}

bool WavFileUtils::getInfo(WavInfo &info)
{
    if (m_isLoadOK)
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


