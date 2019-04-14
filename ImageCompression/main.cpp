#define LOG_BINSEARCH

#include <fstream>
#include <iostream>
#include <vector>
#include <string.h>
#include <cmath>

const uint32_t FORMAT_ALL = 0;
const uint32_t FORMAT_JPG = 1;
const uint32_t FORMAT_JP2 = 2;
const uint32_t FORMAT_JXR = 3;
const uint32_t FORMAT_BPG = 4;

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

std::string translateFormatToString(const uint32_t format) {

	switch(format) {
		case FORMAT_JPG: return std::string("JPEG");
		case FORMAT_JP2: return std::string("JPEG2000");
		case FORMAT_JXR: return std::string("JXR");
		case FORMAT_BPG: return std::string("BPG");
		default: return std::string("Unknown format");
	}
}

std::string createConvertCommand(const uint32_t format, const std::string& inFilename, const std::string& outFilename, const uint32_t quality) {

	switch(format) {
		case FORMAT_JPG: return std::string("./nconvert -quiet -o " + outFilename + " -out jpeg -q " + std::to_string(quality) + " " + inFilename) + " > /dev/null 2>&1";
		case FORMAT_JXR: return std::string("./nconvert -quiet -o " + outFilename + " -out jxr -q " + std::to_string(quality) + " " + inFilename) + " > /dev/null 2>&1";
		case FORMAT_JP2: return std::string("convert -quiet " + inFilename + " -quality " + std::to_string(quality) + " " + outFilename);
		case FORMAT_BPG: return std::string("bpgenc -q " + std::to_string(51 - quality) + " " + inFilename + " -o " + outFilename);
		default: return "";
	}
}

uint32_t binarySearchConvert(const std::string& sourceFilename, const std::string& targetFilename, const size_t targetFilesize, const uint32_t format, uint32_t x, int currQual) {

	if (currQual > 100)
		currQual = 100;
	else if (format == FORMAT_BPG && currQual > 51)
		currQual = 51;
	else if (currQual < 0)
		currQual = 0;

	size_t currFilesize = getFileSize(targetFilename);
#ifdef LOG_BINSEARCH
	std::cout << "Converting " << sourceFilename << " to format " << translateFormatToString(format) <<" with quality: " << currQual << ". Current filesize is: " << currFilesize << std::endl;
#endif
	
	system(std::string("rm -rf " + targetFilename).c_str());
	system(createConvertCommand(format, sourceFilename, targetFilename, currQual).c_str());

	if(x == 1) return currQual;

	uint32_t y = (uint32_t)ceil((float) x / 2.0f);	
	currFilesize = getFileSize(targetFilename);

	if(currFilesize > targetFilesize) return binarySearchConvert(sourceFilename, targetFilename, targetFilesize, format, y, currQual - x);
	else if(currFilesize < targetFilesize) return binarySearchConvert(sourceFilename, targetFilename, targetFilesize, format, y, currQual + x);
	else return currQual;
}

uint32_t linearSearch(const std::string& sourceFilename, const std::string& targetFilename, const size_t targetFilesize, const uint32_t format, int currQual) {

	size_t currFilesize = getFileSize(targetFilename);

	while (currFilesize < targetFilesize) {
		currQual++;

		if (currQual > 100 || format == FORMAT_BPG && currQual > 51)
			return currQual - 1;

		#ifdef LOG_BINSEARCH
			std::cout << "Converting " << sourceFilename << " to format " << translateFormatToString(format) <<" with quality: " << currQual << ". Current filesize is: " << currFilesize << std::endl;
		#endif

		system(std::string("rm -rf " + targetFilename).c_str());
		system(createConvertCommand(format, sourceFilename, targetFilename, currQual).c_str());

		currFilesize = getFileSize(targetFilename);
	}
	while (currFilesize > targetFilesize) {
		currQual--;

		if (currQual < 0)
			return currQual + 1;

		#ifdef LOG_BINSEARCH
			std::cout << "Converting " << sourceFilename << " to format " << translateFormatToString(format) <<" with quality: " << currQual << ". Current filesize is: " << currFilesize << std::endl;
		#endif

		system(std::string("rm -rf " + targetFilename).c_str());
		system(createConvertCommand(format, sourceFilename, targetFilename, currQual).c_str());

		currFilesize = getFileSize(targetFilename);
	}
	
   	return currQual;
}

void convertImage(const std::string& sourceFilename, const std::string& outputDirectory, const size_t targetFilesize, const uint32_t format) {

	// Source image must be png to ensure losslessness.
	if(std::string(sourceFilename.end() - 3, sourceFilename.end()) != "png") {
		ReportFile() << "ERROR: Source file must be .png!\n";
		return;
	}
	
	// TODO change to format name_format_compRate.png
	std::string targetFilename = std::string(sourceFilename.begin(), sourceFilename.end() - 3);
	targetFilename = outputDirectory + targetFilename;
	
	uint32_t qual;
	int32_t maxQual;
	// TODO is maxQual unused??
	// TODO changed range temporarly to 51-0
	
	switch(format) {
			
		case FORMAT_JPG: {
			qual = 100;
			maxQual = 0;
			targetFilename += "jpg";
		} break;
			
		case FORMAT_JP2: {
			qual = 100;
			maxQual = 0;
			targetFilename += "jp2";
		} break;
			
		case FORMAT_JXR:{
			qual = 100;
			maxQual = 0;
			targetFilename += "jxr";
		} break;
		
		// max quality = 0, min quality = 51	
		case FORMAT_BPG: {
			qual = 51;
			maxQual = 0;
			targetFilename += "bpg";
		} break;
			
		default: {
			ReportFile() << "ERROR: Unknown output format specified!\n";
			return;
		}
			
	}
	
	uint32_t tmpQual = binarySearchConvert(sourceFilename, targetFilename, targetFilesize, format, (format == FORMAT_BPG) ? 13 : 25, (format == FORMAT_BPG) ? 26 : 50);
	uint32_t finalQual = linearSearch(sourceFilename, targetFilename, targetFilesize, format, tmpQual);
	
	ReportFile() << "INFO: Converted " << sourceFilename << " with " << translateFormatToString(format) << " and quality value of " << finalQual << ".\n";
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
	
	if(format == FORMAT_ALL){
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
        	"-p  \t\t output into png format\n"
        	"-f  format\t available formats: jpg, jp2, jxr, bpg\n"
        	"-c  compRatio\t compression ratio >= 1.0\n"
        	"-s  size [kB]\t target size for compressed files in kB\n", stderr);
        exit(1);
    }

	ReportFile(false);
	
	std::vector<std::string> imageFilenameList;
	
	if(!fetchImageList(imageFilenameList))
		ReportFile() << "ERROR: Cannot open image filename list!\n";

	// standard parameter options
    uint32_t format = FORMAT_ALL;
    uint32_t outInPNG = 0;
    uint32_t arg_cnt = 1;
	float ratio = 1;
	size_t targetSize = 0;

    while(arg_cnt < argc) {
    	if (strcmp(argv[arg_cnt], "-f") == 0) {
    		arg_cnt++;
    		if(strcmp(argv[arg_cnt], "jpg") == 0) format = FORMAT_JPG;
    		else if (strcmp(argv[arg_cnt], "jp2") == 0) format = FORMAT_JP2;
    		else if (strcmp(argv[arg_cnt], "jxr") == 0) format = FORMAT_JXR;
    		else if (strcmp(argv[arg_cnt], "bpg") == 0) format = FORMAT_BPG;
    		else ReportFile() << "ERROR: Format not supported!\n";
   		} else if(strcmp(argv[arg_cnt], "-p") == 0)
   			outInPNG = 1;
   		else if(strcmp(argv[arg_cnt], "-c") == 0)
   			ratio = std::stof(argv[++arg_cnt]);
   		else if(strcmp(argv[arg_cnt], "-s") == 0)
   			targetSize = (size_t)(std::stoi(argv[++arg_cnt]) * 1000);
   		arg_cnt++;
	}

	for(auto& e : imageFilenameList) {
		convertImageToRatio(format, ratio, targetSize, e);
	}

	ReportFile().close();
}
