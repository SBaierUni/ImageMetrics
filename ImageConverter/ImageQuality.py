import cv2
import sys
import os
import re
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
		score = ssim(img_src, img_comp)[0]
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

def getRawFilename(name):
	m = re.match(r'.*(_\d+_([a-z]|\d){3})', name)
	name = name[:-len(m.groups()[0])]
	return name

def getCompRatio(name):
	m = re.match(r'.*(_([a-z]|\d){3})', name)
	name = name[len(getRawFilename(name))+1:-len(m.groups()[0])]
	return name

def getFormat(name):
	return name[len(name)-3:]


def printUsage():
	print("usage: python metrics.py [options ...] reference comparable\n"
		"supported file formats:  png, jpg, jp2 (not jxr, bpg)\n\nOptions:\n"
        "-metric\t\tmse, rmse, psnr, rmse_sw, uqi, ssim, ergas, scc, rase, sam, msssim, vifp\n")


if len(sys.argv) < 4:
	printUsage()
	exit()

f = open('report.txt', 'a+')
csv = open('scores.csv', 'a+')

metric = sys.argv[1][1:]	# first argument is metric
img_src = cv2.imread(sys.argv[2])  # reference image
sys.argv[2] = getFilename(sys.argv[2])
for x in range(3, len(sys.argv)):
	img_comp = cv2.imread(sys.argv[x],1)  # image to compare
	sys.argv[x] = getFilename(sys.argv[x])
	out_score = getMetric(metric)
	out = "\n{} -> {}\n{}:\t{}".format(sys.argv[2], sys.argv[x], metric.upper(), out_score)
	f.write(out)

	# filename,srccompRatio,srcformat,comcompRatio,comformat,metric,score
	out = "\n{},{},{},{},{},{},{}".format(getRawFilename(sys.argv[2]), getCompRatio(sys.argv[2]), getFormat(sys.argv[2]), getCompRatio(sys.argv[x]), getFormat(sys.argv[x]), metric.upper(), out_score)
	csv.write(out)

f.close()
csv.close()