#include "WavUtils.h"
#include <stdio.h>
#include <string.h>

using namespace std;

WavUtils::WavUtils()
{
    m_fp = NULL;
	m_mode = WAV_UTILS_UNDEF_MODE;
    memset(&m_wavChunk, 0, sizeof(m_wavChunk));
	m_dataStartPos = 0;
	m_dataLen = 0;
}

WavUtils::~WavUtils()
{
	if (m_fp != NULL)
		fclose(m_fp);
	m_fp = NULL;
}

WavUtilsRet WavUtils::load(string path)
{
	if (m_mode != WAV_UTILS_UNDEF_MODE)
		return WAV_UTILS_NOT_IN_RIGHT_MODE;

	WavUtilsRet ret = WAV_UTILS_OK;
    if (m_fp != NULL)
    {
        fclose(m_fp);
    }

    m_fp = fopen(path.c_str(), "rb");
    if (m_fp == NULL)
    {   //文件打开失败
        return WAV_UTILS_OPEN_ERR;
    }

    size_t read_ret = fread(&m_wavChunk, sizeof(m_wavChunk), 1, m_fp);
    if (read_ret != 1)
    {   //读取错误
        ret = WAV_UTILS_READ_ERR;
    }
    else if (string(m_wavChunk.ChunkID, 4) != "RIFF")
    {   //开头不是约定的"RIFF"字符串
        ret = WAV_UTILS_RIFF_ERR;
    }
    else if (string(m_wavChunk.Format, 4) != "WAVE")
    {   //格式不是固定字符串"WAVE"
        ret = WAV_UTILS_WAVE_ERR;
    }
    else
    {
        //开始寻找fmt段和data段
        int32 curPos = 0;
        while(curPos != EOF)
        {
            if (curPos > MAX_WAV_HEADER_SIZE)
            {   //如果已经找了很大的范围, 还是没有找到数据段, 则退出
                ret = WAV_UTILS_NO_DATA_ERR;
                break;
            }

            SubChunkHeader subChunkHeader;
            memset(&subChunkHeader, 0, sizeof(subChunkHeader));
            read_ret = fread(&subChunkHeader, sizeof(subChunkHeader), 1, m_fp);
            if (read_ret != 1)
            {   //读取数据段头信息失败
                ret = WAV_UTILS_READ_ERR;
                break;
            }
            if (string(subChunkHeader.title, 4) == "data")
            {   //数据段
                cout << "found data seg, len = " << subChunkHeader.len << endl;
                m_dataStartPos = ftello64(m_fp);
                cout << "data start pos = " << m_dataStartPos << endl;
                m_dataLen = subChunkHeader.len;
                break;
            }
            else if (string(subChunkHeader.title, 4) == "fmt ")
            {   //fmt段
                cout << "title: " << string(subChunkHeader.title, 4).c_str() <<
                            " len: " << subChunkHeader.len << endl;
                memset(&m_subChunkFmt, 0, sizeof(m_subChunkFmt));
                read_ret = fread(&m_subChunkFmt, sizeof(m_subChunkFmt), 1, m_fp);
                if (read_ret != 1)
                {   //读取fmt段失败
                    ret = WAV_UTILS_READ_ERR;
                    break;
                }
                int32 lenDiff = subChunkHeader.len - sizeof(m_subChunkFmt);
                if (lenDiff > 0)
                {
                    fseeko64(m_fp, lenDiff, SEEK_CUR);
                }
                else if (lenDiff == 0)
                {   //正好读完, 不做任何操作
                    ;
                }
                else
                {
                    ret = WAV_UTILS_FMT_ERR;
                    break;
                }
            }
            else
            {   //不是数据段, 也不是fmt段, 跳过
                cout << "title: " << string(subChunkHeader.title, 4).c_str() <<
                            " len: " << subChunkHeader.len << endl;
                if (subChunkHeader.len != 0)
                    fseek(m_fp, subChunkHeader.len, SEEK_CUR);
            }
            curPos = ftell(m_fp);
            if (curPos == EOF)
            {   //文件结束, 尚未找到data段
                ret = WAV_UTILS_NO_DATA_ERR;
            }
        }
    }
    fclose(m_fp);
	m_fp = NULL;
    if (ret == WAV_UTILS_OK)
		m_mode = WAV_UTILS_READ_MODE;
    return ret;
}

bool WavUtils::getInfo(WavInfo &info)
{
	if (WAV_UTILS_READ_MODE == m_mode)
    {
        info.numChannels = m_subChunkFmt.NumChannnels;
        info.bitsPerSample = m_subChunkFmt.BitsPerSample;
        info.sampleRate = m_subChunkFmt.SampleRate;
        info.dataStartPos = m_dataStartPos;
        info.dataLen = m_dataLen;
        if (m_subChunkFmt.AudioFormat == 3)
		{
			info.isFloat = true;
		}
		else
		{
			info.isFloat = false;
		}
        return true;
    }
    else
        return false;
}

WavUtilsRet WavUtils::create(std::string path, const WavInfo& info)
{
	if (m_mode != WAV_UTILS_UNDEF_MODE)
		return WAV_UTILS_NOT_IN_RIGHT_MODE;

    //准备wav块头
    WavChunk wavChunk;
    memcpy(wavChunk.ChunkID, "RIFF", sizeof(wavChunk.ChunkID));
    memcpy(wavChunk.Format, "WAVE", sizeof(wavChunk.Format));

    //准备fmt子块头
    SubChunkHeader fmtChunkHeader;
    memcpy(fmtChunkHeader.title, "fmt ", sizeof(fmtChunkHeader.title));
    fmtChunkHeader.len = 16;

    //准备fmt子块数据
    SubChunkFmt subChunkFmt;
    if (info.isFloat)
    {
        subChunkFmt.AudioFormat = 3;
    }
    else
    {
        subChunkFmt.AudioFormat = 1;
    }
	if (info.numChannels == 1 || info.numChannels == 2)
        subChunkFmt.NumChannnels = info.numChannels;
	else
		return WAV_UTILS_INPUT_ERROR;
    subChunkFmt.SampleRate = info.sampleRate;
    subChunkFmt.ByteRate = info.numChannels * info.sampleRate * info.bitsPerSample / 8;
    subChunkFmt.BlockAlign = info.numChannels * info.bitsPerSample / 8;
    subChunkFmt.BitsPerSample = info.bitsPerSample;

	m_fp = fopen(path.c_str(), "wb+");
	if (m_fp == NULL)
		return WAV_UTILS_OPEN_ERR;
	
    //准备数据头
    SubChunkHeader dataHeader;
    memcpy(&dataHeader.title, "data", sizeof(dataHeader.title));
	
	//写入文件头
    fwrite(&wavChunk, sizeof(wavChunk), 1, m_fp);
    fwrite(&fmtChunkHeader, sizeof(fmtChunkHeader), 1, m_fp);
    fwrite(&subChunkFmt, sizeof(subChunkFmt), 1, m_fp);
    fwrite(&dataHeader, sizeof(dataHeader), 1, m_fp);

    m_wavChunk = wavChunk;
	m_mode = WAV_UTILS_WRITE_MODE;
    m_dataStartPos = ftello64(m_fp);

	return WAV_UTILS_OK;
}

WavUtilsRet WavUtils::write(const char* buf, int32 size)
{
	if (m_mode != WAV_UTILS_WRITE_MODE)
		return WAV_UTILS_NOT_IN_RIGHT_MODE;

    int32 bytePerSample = m_subChunkFmt.BitsPerSample / 8;
	size = size / bytePerSample * bytePerSample;	//只保留能够被采样点位数整除的数据
	m_dataLen += size;
	WavUtilsRet ret = WAV_UTILS_OK;
	
	int32 writeRet = fwrite(buf, size, 1, m_fp);
	if (writeRet < 1)
	{
		ret = WAV_UTILS_WRITE_ERR;
	}
	return ret;
}

WavUtilsRet WavUtils::close()
{
	if (m_mode != WAV_UTILS_WRITE_MODE)
		return WAV_UTILS_NOT_IN_RIGHT_MODE;
    fseeko64(m_fp, m_dataStartPos - sizeof(SubChunkHeader), SEEK_SET);
	//重新写入数据RIFF头
    SubChunkHeader dataHeader;
    memcpy(&dataHeader.title, "data", sizeof(dataHeader.title));
    dataHeader.len = m_dataLen;
    fwrite(&dataHeader, sizeof(dataHeader), 1, m_fp);
	fclose(m_fp);
	m_fp = NULL;
	return WAV_UTILS_OK;
}


