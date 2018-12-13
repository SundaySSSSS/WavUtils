#ifndef DATA_FORMAT_TRANS_H_
#define DATA_FORMAT_TRANS_H_

#ifndef DATA_FORMAT_STRUCT_
#define DATA_FORMAT_STRUCT_

#include <vector>
#include "typedef.h"

namespace dft
{

	/* Ŀǰ֧�ֵ����ݸ�ʽ */
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

		/* ���о�̬���ߺ��� */
		//�������ͺ�ÿ�����ݵ㳤��֮����໥ת��
		static int32 transDataFormat2BytePerData(DataFormat dataFormat);

		//�������ݸ�ʽ����ת��
		T_out transDataFormat(const T_in* pBufIn, int32 sizeIn, T_out a)
		{
			if (pBufIn == NULL)
				return NULL;
		}
	};

}
#endif /* DATA_FORMAT_TRANS_H_ */
