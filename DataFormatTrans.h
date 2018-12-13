#ifndef DATA_FORMAT_TRANS_H_
#define DATA_FORMAT_TRANS_H_

#ifndef DATA_FORMAT_STRUCT_
#define DATA_FORMAT_STRUCT_

#include <vector>
#include "typedef.h"

namespace dft
{

	/* 目前支持的数据格式 */
	typedef enum _DataFormat
	{
		FORMAT_INT8,
		FORMAT_INT16,
		FORMAT_INT32,
		FORMAT_INT64,
		FORMAT_UINT8,
		FORMAT_UINT16,
		FORMAT_UINT32,
		FORMAT_UINT64,
		FORMAT_FLOAT32,
		FORMAT_DOUBLE64,
	}
	DataFormat;
#endif /* DATA_FORMAT_STRUCT_ */

	template <class T_in, class T_out>
	class DataFormatTrans
	{
	public:
		DataFormatTrans() {}
		virtual ~DataFormatTrans() {}

		/* 公有静态工具函数 */
		//数据类型和每个数据点长度之间的相互转化
		static int32 transDataFormat2BytePerData(DataFormat dataFormat);

		//根据数据格式进行转化
		T_out transDataFormat(const T_in* pBufIn, int32 sizeIn, T_out a)
		{
			if (pBufIn == NULL)
				return NULL;
		}
	};

}
#endif /* DATA_FORMAT_TRANS_H_ */
