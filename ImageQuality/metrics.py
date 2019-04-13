import cv2
import sys
import os
from sewar.full_ref import *
# sewar-lib from https://github.com/andrewekhalel/sewar

def getMetric(metric):
	if metric == "mse":
		score = mse(img_src, img_comp)
	elif metric == "rmse":
		score = rmse(img_src, img_comp)
	elif metric == "psnr":
		score = psnr(img_src, img_comp)
	elif metric == "rmse_sw":
		score = rmse_sw(img_src, img_comp)
	elif metric == "uqi":
		score = uqi(img_src, img_comp)
	elif metric == "ssim":
		score = ssim(img_src, img_comp)
	elif metric == "ergas":
		score = ergas(img_src, img_comp)
	elif metric == "scc":
		score = scc(img_src, img_comp)
	elif metric == "rase":
		score = rase(img_src, img_comp)
	elif metric == "sam":
		score = sam(img_src, img_comp)
	elif metric == "msssim":
		score = msssim(img_src, img_comp)
	elif metric == "vifp":
		score = vifp(img_src, img_comp)
	else:
		print("Metric ", metric, " is NOT supported")
	return score


def getFilename(path):
	return os.path.basename(path)[:-4]


def printUsage():
	print("correct usage:      python metrics.py -[metric] referenceFile1 compareFile2\n\n"
        	"supported formats:  png, jpg, jp2 (not jxr, bpg)\n"
        	"supported metrics:  mse, rmse, psnr, rmse_sw, uqi, ssim, ergas, \n"
        	"\t\t    scc, rase, sam, msssim, vifp")


if len(sys.argv) < 4:
	printUsage()
else:
	f = open('scores.txt', 'w')
	metric = sys.argv[1][1:]
	img_src = cv2.imread(sys.argv[2])  # reference image
	sys.argv[2] = getFilename(sys.argv[2])

	for x in range(3, len(sys.argv)):
		img_comp = cv2.imread(sys.argv[x],1)  # image to compare
		sys.argv[x] = getFilename(sys.argv[x])
		out_score = getMetric(metric)

		out = "\n{} -> {}\n{}:\t{}".format(sys.argv[2], sys.argv[x], metric.upper(),out_score)
		print(out)
		f.write(out)

	

