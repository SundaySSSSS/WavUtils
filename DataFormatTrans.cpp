#include "DataFormatTrans.h"

using namespace dft;


template <class T_in, class T_out>
int32 DataFormatTrans<T_in, T_out>::transDataFormat2BytePerData(DataFormat dataFormat)
{
	uint16 bytePerData = 0;
	switch (dataFormat)
	{
	case FORMAT_INT8:
	case FORMAT_UINT8:
		bytePerData = 1;
		break;
	case FORMAT_INT16:
	case FORMAT_UINT16:
		bytePerData = 2;
		break;
	case FORMAT_INT32:
	case FORMAT_UINT32:
	case FORMAT_FLOAT32:
		bytePerData = 4;
		break;
	case FORMAT_INT64:
	case FORMAT_UINT64:
	case FORMAT_DOUBLE64:
	default:
		bytePerData = 8;
		break;
	}
	return bytePerData;
}
