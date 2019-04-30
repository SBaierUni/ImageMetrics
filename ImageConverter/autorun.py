import cv2
import sys
import os
from tqdm import tqdm

def printUsage():
	print("usage: python autorun.py [options ...]\n\n"
		"Options:\n-nc\t\tuse images from folder without converting\n")


sourcePath = "source/*.png"
convertedPath = "converted/"
metrics = ("mse", "rmse", "psnr", "ergas", "rase", "sam") # prefered metrics
convert = 1


if len(sys.argv) > 1:
	if sys.argv[1] == "-h":
		printUsage()
		exit()
	if sys.argv[1] == "-nc":
		convert = 0
		os.system("make")
if convert:
	os.system("make clean")
	os.system("make")
	for x in range(10, 100, 30):
		os.system("./ImageConverter -p -c " + str(x) + " " + sourcePath)

images = os.listdir(convertedPath)
numOfCompares = int(len(metrics) * len(images) * (len(images) - 1) / 2)
pbar = tqdm(total=numOfCompares)

while images:
	reference = images[0]
	del images[0]
	for x in metrics:
		for img in images:
			os.system("python ImageQuality.py -" + x + " " + convertedPath + reference + " " + convertedPath + img)
			pbar.update()

pbar.close()