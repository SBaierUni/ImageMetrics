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

void throwErrorAndExit(const std::string& errorMessage) {
	std::cout << errorMessage << std::endl;
	ReportFile() << errorMessage;
	exit(1);
}

void printUsageAndExit(){
	fputs("usage: ./ImageConverter [options ...] inputFiles.png\n\nOptions:\n"
        	"-p  \t\tcompress and convert into png format\n"
        	"-f  format\tall(default), jpg, jp2, jxr, bpg\n"
        	"-c  compRatio\tcompression ratio >= 1.0\n"
        	"-s  size [kB]\ttarget size for compressed files\n", stderr);
	exit(1);
}

std::string base_name(std::string const & path) {
  return path.substr(path.find_last_of("/\\") + 1);
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

std::string getEncodingCommand(const uint32_t format, const std::string& sourcePath, const std::string& outputPath, const uint32_t quality) {

	switch(format) {
		case FORMAT_JPG: return std::string("./nconvert -quiet -o " + outputPath + " -out jpeg -q " + std::to_string(quality) + " " + sourcePath);
		case FORMAT_JXR: return std::string("./nconvert -quiet -o " + outputPath + " -out jxr -q " + std::to_string(quality) + " " + sourcePath);
		case FORMAT_JP2: return std::string("convert -quiet " + sourcePath + " -quality " + std::to_string(quality) + " " + outputPath);
		// BPG quality was calculated inverse
		case FORMAT_BPG: return std::string("bpgenc -q " + std::to_string(51 - quality) + " " + sourcePath + " -o " + outputPath);
		default: return "";
	}
}

std::string getDecodingCommand(const std::string& sourcePath, const std::string& outputDirectory, const uint32_t format) {

	std::string formatString = std::string(sourcePath.end() - 3, sourcePath.end());
	std::string targetFilename = std::string(sourcePath.begin(), sourcePath.end() - 4);
	targetFilename = targetFilename + "_" + formatString + ".png";

	switch(format) {
		case FORMAT_JPG: // skip to JXR
		case FORMAT_JXR: return std::string("./nconvert -quiet -o " + targetFilename + " -out png " + sourcePath);
		case FORMAT_JP2: return std::string("convert -quiet " + sourcePath + " " + targetFilename);
		case FORMAT_BPG: return std::string("bpgdec " + sourcePath + " -o " + targetFilename);
		default: return "";
	}
}

uint32_t binarySearchConvert(const std::string& sourceFilename, const std::string& targetFilename, const size_t targetFilesize, const uint32_t format, uint32_t x, int currQual) {

	if (currQual < 0) currQual = 0;
	else if (currQual > 100) currQual = 100;
	else if (format == FORMAT_BPG && currQual > 51) currQual = 51;

	size_t currFilesize = getFileSize(targetFilename);

	//std::cout << "\b->" << std::flush;
	std::cout << "\b=D" << std::flush;
	
	system(std::string("rm -rf " + targetFilename).c_str());
	system(getEncodingCommand(format, sourceFilename, targetFilename, currQual).c_str());

	if(x == 1) return currQual;

	uint32_t y = (uint32_t)ceil((float) x / 2.0f);	
	currFilesize = getFileSize(targetFilename);

	if(currFilesize == targetFilesize) return currQual;

	return binarySearchConvert(sourceFilename, targetFilename, targetFilesize, format, y, (currFilesize > targetFilesize) ? currQual - x : currQual + x);
}

uint32_t linearSearch(const std::string& sourceFilename, const std::string& targetFilename, const size_t targetFilesize, const uint32_t format, int currQual) {

	size_t currFilesize = getFileSize(targetFilename);

	while (currFilesize < targetFilesize) {
		currQual++;

		if (currQual > 100 || format == FORMAT_BPG && currQual > 51)
			return currQual - 1;

		//std::cout << "\b->" << std::flush;
		std::cout << "\b=D" << std::flush;

		system(std::string("rm -rf " + targetFilename).c_str());
		system(getEncodingCommand(format, sourceFilename, targetFilename, currQual).c_str());

		currFilesize = getFileSize(targetFilename);
	}
	while (currFilesize > targetFilesize) {
		currQual--;

		if (currQual < 0)
			return currQual + 1;

		//std::cout << "\b->" << std::flush;
		std::cout << "\b=D" << std::flush;

		system(std::string("rm -rf " + targetFilename).c_str());
		system(getEncodingCommand(format, sourceFilename, targetFilename, currQual).c_str());

		currFilesize = getFileSize(targetFilename);
	}
	std::cout << "\nConverting " << sourceFilename << " to format " << translateFormatToString(format) <<" with quality: " << currQual << ". Current filesize is: " << currFilesize << std::endl;

   	return currQual;
}

void convertImage(const std::string& sourceFilename, const std::string& outputDirectory, const size_t targetFilesize, const float compRate, const uint32_t format, const uint32_t outInPNG) {

	// Source image must be png to ensure losslessness
	if(std::string(sourceFilename.end() - 3, sourceFilename.end()) != "png")
		throwErrorAndExit("ERROR: Source file must be .png!\n");
	
	// TODO not supporting decimal digits
	std::string targetFilename = base_name(std::string(sourceFilename.begin(), sourceFilename.end() - 4));
	std::string compRatioString = std::to_string((int)compRate);
	targetFilename = outputDirectory + targetFilename + "_" + compRatioString;
	
	uint32_t qual = 100;

	if(format == FORMAT_JPG) targetFilename += ".jpg";
	else if(format == FORMAT_JP2) targetFilename += ".jp2";
	else if(format == FORMAT_JXR) targetFilename += ".jxr";
	else if(format == FORMAT_BPG) {
		qual = 51;
		targetFilename += ".bpg";
	} else throwErrorAndExit("ERROR: Unknown output format specified!\n");

	//std::cout << ">>>" << std::flush;
	std::cout << "BB" << std::flush;
	uint32_t tmpQual = binarySearchConvert(sourceFilename, targetFilename, targetFilesize, format, (format == FORMAT_BPG) ? 13 : 25, (format == FORMAT_BPG) ? 26 : 50);
	uint32_t finalQual = linearSearch(sourceFilename, targetFilename, targetFilesize, format, tmpQual);

	// delete compressed images and convert to png
	if(outInPNG == 1) {
		system(getDecodingCommand(targetFilename, outputDirectory, format).c_str());
		system(std::string("rm -rf " + targetFilename).c_str());
	}
	
	ReportFile() << "INFO: Converted " << sourceFilename << " with " << translateFormatToString(format) << " and quality value of " << finalQual << ".\n";
}

void convertImageToRatio(const uint32_t format, const float targetRatio, const size_t targetSize, const uint32_t outInPNG, const std::string& imageFilename) {
	
	if(targetRatio < 1.0f)
		throwErrorAndExit("ERROR: Target ratio of " + imageFilename + " is < 1.0\n");
	
	size_t originalImageSize = getFileSize(imageFilename);
	if(originalImageSize == 0)
		throwErrorAndExit("ERROR: Image \"" + imageFilename + "\" either does not exist or is empty!\n");
	
	size_t targetImageSize = targetSize;

	if(targetSize == 0)
		targetImageSize = (size_t)((float)originalImageSize/(float)targetRatio);
	
	if(format == FORMAT_ALL){
		convertImage(imageFilename, "converted/", targetImageSize, targetRatio, FORMAT_JPG, outInPNG);
		convertImage(imageFilename, "converted/", targetImageSize, targetRatio, FORMAT_JP2, outInPNG);
		convertImage(imageFilename, "converted/", targetImageSize, targetRatio, FORMAT_JXR, outInPNG);
		convertImage(imageFilename, "converted/", targetImageSize, targetRatio, FORMAT_BPG, outInPNG);
	} else
		convertImage(imageFilename, "converted/", targetImageSize, targetRatio, format, outInPNG);
}

int main(int argc, char *argv[]) {

 	if (argc < 2)
        printUsageAndExit();
	ReportFile(false);
	
	// standard parameter options
	std::vector<std::string> imageFilenameList;
    uint32_t format = FORMAT_ALL;
    uint32_t outInPNG = 0;
    uint32_t arg_cnt = 1;
	float ratio = 1;
	size_t targetSize = 0;

    while(arg_cnt < argc) {
    	if (strcmp(argv[arg_cnt], "-f") == 0) {
    		arg_cnt++;
    		if (strcmp(argv[arg_cnt], "all") == 0) format = FORMAT_ALL;
    		else if(strcmp(argv[arg_cnt], "jpg") == 0) format = FORMAT_JPG;
    		else if (strcmp(argv[arg_cnt], "jp2") == 0) format = FORMAT_JP2;
    		else if (strcmp(argv[arg_cnt], "jxr") == 0) format = FORMAT_JXR;
    		else if (strcmp(argv[arg_cnt], "bpg") == 0) format = FORMAT_BPG;
    		else throwErrorAndExit("ERROR: Format not supported!\n");
   		} else if(strcmp(argv[arg_cnt], "-p") == 0)
   			outInPNG = 1;
   		else if(strcmp(argv[arg_cnt], "-c") == 0)
   			ratio = std::stof(argv[++arg_cnt]);
   		else if(strcmp(argv[arg_cnt], "-s") == 0)
   			targetSize = (size_t)(std::stoi(argv[++arg_cnt]) * 1000);
   		else
			imageFilenameList.push_back(argv[arg_cnt]);
   		arg_cnt++;
	}

	for(auto& e : imageFilenameList)
		convertImageToRatio(format, ratio, targetSize, outInPNG, e);

	ReportFile().close();
}
