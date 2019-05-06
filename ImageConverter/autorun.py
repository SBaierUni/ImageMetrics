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

sourcePath = "source/*.png"
convertedPath = "converted/"
metrics = ("psnr", "uqi", "ssim", "ergas", "sam", "vifp") # prefered metrics
referenceQualities = (10, 30)
comparableQualities = (20, 40, 60, 80)
numOfFormats = 4	# bpg, jp2, jpg, jxr
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
	for x in comparableQualities:
		os.system("./ImageConverter -p -c " + str(x) + " " + sourcePath)
if quality:
	images = os.listdir(convertedPath)
	diffSourceImages = len(images) / (len(comparableQualities) * numOfFormats)
	diffCompImages = len(comparableQualities) * numOfFormats
	numOfCompareOp = int(diffSourceImages * len(metrics) * diffCompImages * (diffCompImages - 1) / 2)
	progressbar = tqdm(total=numOfCompareOp)
	
	while images:
		reference = images[0]
		ref = getRawFilename(reference)
	
		del images[0]
		for x in metrics:
			for img in images:
				if getRawFilename(img) == ref:
					os.system("python ImageQuality.py -" + x + " " + convertedPath + reference + " " + convertedPath + img)
				progressbar.update()
	
	progressbar.close()