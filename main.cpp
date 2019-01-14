#include "WavUtils.h"
#include "DataFormatTrans.h"

#if 1

using namespace dft;

int main(int argc, char* argv[])
{
	WavUtils wu;
	wu.load("C:\\test\\WOTW_origin.wav");
	WavInfo info;
	wu.getInfo(info);
	
	WavUtils wavUtils;
	wavUtils.create("C:\\test\\WOTW_new.wav", info);

	FILE* fp = fopen("C:\\test\\WOTW_origin.wav", "rb");
	if (fp != NULL)
	{
		_fseeki64(fp, info.dataStartPos, SEEK_SET);
		char temp[1024 * 512] = {};
		while (1 == fread(temp, 1024 * 512, 1, fp))
		{
			wavUtils.write(temp, 1024 * 512);
		}
		fclose(fp);
	}

	wavUtils.close();
	
	
	
	DataFormatTrans<int, float> Dft;
	int a[5] = {};
	float b = 5.0f;
	Dft.transDataFormat(&(a[0]), 5, b);

	system("pause");
	return 0;
}
#endif