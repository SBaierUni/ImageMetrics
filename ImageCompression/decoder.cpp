#define LOG_BINSEARCH

#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <cmath>

std::ofstream& ReportFile(bool initialized = true) {
	static std::ofstream ret;
	if(!initialized) {
		ret.open("report.txt");
		if(!ret.is_open())
			std::cerr << "ERROR: Could not open report file!\n";
	}
	return ret;
}

bool fetchImageList(std::vector<std::string>& imageFilenameList) {
	
	std::ifstream imageList_FS("imageList.txt");
	if(imageList_FS.is_open()) {
		std::string line;
		while(getline(imageList_FS, line))
			imageFilenameList.push_back(line);
		imageList_FS.close();
	} else
		return false;
	
	return true;
	
}

size_t getFileSize(const std::string& filename) {
	
	std::ifstream file_FS(filename, std::ios_base::binary | std::ios_base::ate);
	if(!file_FS.is_open()) return 0;
	size_t ret = file_FS.tellg();
	file_FS.close();
	return ret;
	
}

const uint32_t FORMAT_JPG = 1;
const uint32_t FORMAT_JP2 = 2;
const uint32_t FORMAT_JXR = 3;
const uint32_t FORMAT_BPG = 4;

std::string translateFormatToString(const uint32_t format) {

	switch(format) {
		case FORMAT_JPG: return std::string("JPEG");
		case FORMAT_JP2: return std::string("JPEG2000");
		case FORMAT_JXR: return std::string("JXR");
		case FORMAT_BPG: return std::string("BPG");
		default: return std::string("Unknown format");
	}

}

std::string createNConvertCommand(const std::string& inFilename, const std::string& outFilename, const uint32_t quality, const bool isJXR = false) {
	return std::string("./nconvert -quiet -o " + outFilename + " -out png" + " " + inFilename) + " > /dev/null 2>&1";
}

std::string createBPGENCCommand(const std::string& inFilename, const std::string& outFilename, const uint32_t quality) {
	return std::string("bpgdec -o" + outFilename + " " + inFilename);
}

std::string createConvertCommand(const std::string& inFilename, const std::string& outFilename, const uint32_t quality) {
	return std::string("convert -quiet " + inFilename + " " + outFilename);
}

uint32_t binarySearchConvert(const std::string& sourceFilename, const std::string& targetFilename, const size_t targetFilesize, const uint32_t format, uint32_t x, uint32_t currQual) {
	
	if(format == FORMAT_BPG && currQual > 10)
		return 0;
	if(format != FORMAT_BPG && currQual > 100)
		return 0;

	if(currQual == 0)
		currQual = 1;

	size_t currFilesize = getFileSize(targetFilename);
#ifdef LOG_BINSEARCH
	std::cout << "Converting " << sourceFilename << " to format " << translateFormatToString(format) <<" with quality: " << currQual << ". Current filesize is: " << currFilesize << std::endl;
#endif
	
	system(std::string("rm -rf " + targetFilename).c_str());
	switch(format) {
		case FORMAT_JPG: { system(createNConvertCommand(sourceFilename, targetFilename, currQual).c_str()); } break;
		case FORMAT_JP2: { system(createConvertCommand(sourceFilename, targetFilename, currQual).c_str()); } break;
		case FORMAT_JXR: { system(createNConvertCommand(sourceFilename, targetFilename, currQual, true).c_str()); } break;
		case FORMAT_BPG: { system(createBPGENCCommand(sourceFilename, targetFilename, currQual).c_str()); } break;
	}

	if(x == 1)
		return currQual;

	uint32_t y = (uint32_t)ceil((float)x / 2.0f);	
	currFilesize = getFileSize(targetFilename);
	if(currFilesize > targetFilesize)
		return binarySearchConvert(sourceFilename, targetFilename, targetFilesize, format, y, (format == FORMAT_BPG) ? currQual + x : currQual - x);
	else if(currFilesize < targetFilesize)
		return binarySearchConvert(sourceFilename, targetFilename, targetFilesize, format, y, (format == FORMAT_BPG) ? currQual - x : currQual + x);
	else
		return currQual;
}

void convertImage(const std::string& sourceFilename, const std::string& outputDirectory, const size_t targetFilesize, const uint32_t format) {
	
	// Source image must be png to ensure losslessness.
	if(std::string(sourceFilename.end() - 3, sourceFilename.end()) != "png") {
		ReportFile() << "ERROR: Source file must be .png!\n";
		return;
	}
	
	std::string targetFilename = std::string(sourceFilename.begin(), sourceFilename.end() - 3);
	targetFilename = outputDirectory + targetFilename;
	
	uint32_t qual;
	int32_t maxQual;
	
	switch(format) {
			
		case FORMAT_JPG: {
			qual = 100;
			maxQual = 1;
			targetFilename += "jpg";
		} break;
			
		case FORMAT_JP2: {
			qual = 100;
			maxQual = 0;
			targetFilename += "jp2";
		} break;
			
		case FORMAT_JXR:{
			qual = 100;
			maxQual = 1;
			targetFilename += "jxr";
		} break;
			
		case FORMAT_BPG: {
			qual = 1;
			maxQual = 10;
			targetFilename += "bpg";
		} break;
			
		default: {
			ReportFile() << "ERROR: Unknown output format specified!\n";
			return;
		}
			
	}
	
	uint32_t finalQual = binarySearchConvert(sourceFilename, targetFilename, targetFilesize, format, (format == FORMAT_BPG) ? 3 : 25, (format == FORMAT_BPG) ? 5 : 50);
	
	ReportFile() << "INFO: Converted " << sourceFilename << " with " << translateFormatToString(format) << " and quality value of " << qual << ".\n";
	
}

void convertImageToRatio(const uint32_t format, const float targetRatio, const size_t targetSize, const std::string& imageFilename) {
	
	if(targetRatio <= 1.0f) {
		ReportFile() << "ERROR: Target ratio of " << imageFilename << " is <= 1.0\n";
		return;
	}
	
	size_t originalImageSize = getFileSize(imageFilename);
	if(originalImageSize == 0) {
		ReportFile() << "ERROR: Image \"" << imageFilename << "\" either does not exist or is empty!\n";
		return;
	}
	
	size_t targetImageSize = targetSize;

	if(targetSize == 0)
		targetImageSize = (size_t)((float)originalImageSize/(float)targetRatio);
	
	if(format == 0){
		convertImage(imageFilename, "converted/", targetImageSize, FORMAT_JPG);
		convertImage(imageFilename, "converted/", targetImageSize, FORMAT_JP2);
		convertImage(imageFilename, "converted/", targetImageSize, FORMAT_JXR);
		convertImage(imageFilename, "converted/", targetImageSize, FORMAT_BPG);
	} else
		convertImage(imageFilename, "converted/", targetImageSize, format);
}

int main(int argc, char *argv[]) {
 	if (argc < 2) {
        fputs("usage: ./ImageConverter [options ...]\n\nOptions:\n"
        	"-a  \t\t convert source into all available formats\n"
        	"-f  format\t available formats: jpg, jp2, jxr, bpg\n"
        	"-c  compRatio\t compression ratio >= 1.0\n"
        	"-s  size [kB]\t target size for compressed files in kB\n", stderr);
        exit(1);
    }

    // standard parameter options
    uint32_t format = FORMAT_JPG;
    uint32_t cnt = 1;
	float ratio = 3;
	size_t targetSize = 0;

	ReportFile(false);
	
	std::vector<std::string> imageFilenameList;
	
	if(!fetchImageList(imageFilenameList))
		ReportFile() << "ERROR: Cannot open image filename list!\n";

    while(cnt < argc) {
    	if (strcmp(argv[cnt], "-f") == 0) {
    		cnt++;
    		if(strcmp(argv[cnt], "jpg") == 0) format = FORMAT_JPG;
    		else if (strcmp(argv[cnt], "jp2") == 0) format = FORMAT_JP2;
    		else if (strcmp(argv[cnt], "jxr") == 0) format = FORMAT_JXR;
    		else if (strcmp(argv[cnt], "bpg") == 0) format = FORMAT_BPG;
    		else ReportFile() << "ERROR: Format not supported!\n";
   		} else if(strcmp(argv[cnt], "-a") == 0)
   			format = 0;
   		else if(strcmp(argv[cnt], "-c") == 0)
   			ratio = std::stof(argv[++cnt]);
   		else if(strcmp(argv[cnt], "-s") == 0)
   			targetSize = (size_t)(std::stoi(argv[++cnt]) * 1000);
   		cnt++;
	}

	for(auto& e : imageFilenameList) {
		convertImageToRatio(format, ratio, targetSize, e);
	}

	ReportFile().close();
}
