import cv2
import sys
import os
import re
from tqdm import tqdm

def printUsage():
	print("usage: python autorun.py [options ...]\n\n"
		"Options:\n-c\t\tconvert images from 'source' folder"
		"\n-q\t\tcompute quality metrics images in 'converted' folder\n")

def getRawFilename(name):
	m = re.match(r'.*(_\d+_([a-z]|\d){3}\.png)', name)
	name = name[:-len(m.groups()[0])]
	return name

def getCompRatio(name):
	m = re.match(r'.*(_([a-z]|\d){3}\.png)', name)
	name = name[len(getRawFilename(name))+1:-len(m.groups()[0])]
	return name

def getReferenceFiles(im):
	refList = []
	comList = []
	for i in im[:imagesPerSrc]:
		tmp = int(getCompRatio(i))
		if tmp in referenceQualities:
			refList.append(i)
		else:
			comList.append(i)
	del im[0:imagesPerSrc]
	return refList, comList

sourcePath = "source/*.png"
convertedPath = "converted/"
metrics = ("psnr", "uqi", "ssim", "ergas", "sam", "vifp")
referenceQualities = (10, 30)
comparableQualities = (20, 40, 60, 80)
numOfRefFormats = 2		# jp2, jpg
numOfCompFormats = 4	# bpg, jp2, jpg, jxr
imagesPerSrc = len(comparableQualities) * numOfCompFormats + len(referenceQualities) * numOfRefFormats
convert = 0
quality = 0


if len(sys.argv) < 2:
	printUsage()
	exit()

for arg in sys.argv:
	if arg == "-c":
		convert = 1
		os.system("make clean")
	elif arg == "-q":
		quality = 1

os.system("make")

if convert:
	for x in referenceQualities:
		os.system("./ImageConverter -p -f jpg -c " + str(x) + " " + sourcePath)
		os.system("./ImageConverter -p -f jp2 -c " + str(x) + " " + sourcePath)
	for x in comparableQualities:
		os.system("./ImageConverter -p -c " + str(x) + " " + sourcePath)

if quality:

	csv = open('scores.csv', 'w')
	csv.write("filename,srccompRatio,srcformat,comcompRatio,comformat,metric,score")
	csv.close()

	images = os.listdir(convertedPath)
	images.sort()
	diffSourceImages = len(images) / imagesPerSrc
	diffCompImages = len(comparableQualities) * numOfCompFormats * len(referenceQualities) * numOfRefFormats
	numOfCompareOp = int(diffSourceImages * diffCompImages * len(metrics))
	progressbar = tqdm(total=numOfCompareOp)
	

	while images:
		refList, comList = getReferenceFiles(images)

		while refList:
			reference = refList[0]
			ref = getRawFilename(reference)
	
			del refList[0]
			for img in comList:
				for x in metrics:
					os.system("python ImageQuality.py -" + x + " " + convertedPath + reference + " " + convertedPath + img)
					progressbar.update()
	
	progressbar.close()