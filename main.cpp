#include "WavUtils.h"

int main(int argc, char* argv[])
{
	WavUtils wu;
    wu.load("C:\\test\\Weight of the World.wav");
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

	system("pause");
	return 0;
}
