import cv2
import sys
import os
from sewar.full_ref import *
# sewar-lib from https://github.com/andrewekhalel/sewar

def getMetric(metric):
	if metric == "mse":
		score = mse(src, comp)
	elif metric == "rmse":
		score = rmse(src, comp)
	elif metric == "psnr":
		score = psnr(src, comp)
	elif metric == "rmse_sw":
		score = rmse_sw(src, comp)
	elif metric == "uqi":
		score = uqi(src, comp)
	elif metric == "ssim":
		score = ssim(src, comp)
	elif metric == "ergas":
		score = ergas(src, comp)
	elif metric == "scc":
		score = scc(src, comp)
	elif metric == "rase":
		score = rase(src, comp)
	elif metric == "sam":
		score = sam(src, comp)
	elif metric == "msssim":
		score = msssim(src, comp)
	elif metric == "vifp":
		score = vifp(src, comp)
	else:
		print("Metric ", metric, " is NOT supported")
	return score

def getFilename(path):
	return os.path.basename(path)[:-4]


if len(sys.argv) < 4:
	print("correct usage:      python metrics.py -[metric] referenceFile1 compareFile2\n\n"
        	"supported formats:  png, jpg, jp2 (not jxr, bpg)\n"
        	"supported metrics:  mse, rmse, psnr, rmse_sw, uqi, ssim, ergas, \n"
        	"\t\t    scc, rase, sam, msssim, vifp");
else:
	f = open('scores.txt', 'w')
	metric = sys.argv[1][1:]
	src = cv2.imread(sys.argv[2])  # reference image
	sys.argv[2] = getFilename(sys.argv[2])

	for x in range(3, len(sys.argv)):
		comp = cv2.imread(sys.argv[x],1)  # image to compare
		sys.argv[x] = getFilename(sys.argv[x])
		tmp_score = getMetric(metric)
		out = "\n{} -> {}\n{}:\t{}".format(sys.argv[2], sys.argv[x], metric.upper(),tmp_score)
		print(out)
		f.write(out)

	

