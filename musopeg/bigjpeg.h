#ifndef BIGJPEG_H
#define BIGJPEG_H


class BigJPEG
{
private:
	int byteWidth;
	std::vector<char *> lines;
	void setByteWidth(const int& count);
	bool storeScannedLine(const JSAMPARRAY sampledLine);

public:
	BigJPEG(const std::string& fileName);
};

#endif // BIGJPEG_H
