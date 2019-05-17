import cv2 		# opencv
import sys 		# arguments
import os 		# system commands
import re 		# matching strings
import numpy
import scipy.ndimage	# vifp
from tqdm import tqdm	# progressbar
from skimage.measure import compare_ssim	# ssim
from sewar.full_ref import *	# some metrics

# global parameter variables
source_path = "source/*.png"
converted_path = "converted/"
metrics = ("psnr", "uqi", "ssim", "scc", "sam", "vifp")
reference_qualities = (10, 30)
comparable_qualities = (20, 40, 60, 80)
reference_formats = ("jp2", "jpg")
num_comp_formats = 4	# bpg, jp2, jpg, jxr

def main():
	do_conv, do_qual = load_arguments()

	if do_conv:
		for ref_qual in reference_qualities:
			for ref_format in reference_formats:
				os.system("./ImageConverter -p -f " + ref_format + " -c " + str(ref_qual) + " " + source_path)
		for com_qual in comparable_qualities:
			os.system("./ImageConverter -p -c " + str(com_qual) + " " + source_path)
	
	if do_qual:
		csv = open('scores.csv', 'w')
		csv.write("filename,srccompRatio,srcformat,comcompRatio,comformat,metric,score")
		csv.close()
	
		images = os.listdir(converted_path)
		images.sort()
		img_per_src = len(comparable_qualities) * num_comp_formats + len(reference_qualities) * len(reference_formats)
		no_of_src_img = len(images) / img_per_src
		no_of_com_img = len(comparable_qualities) * num_comp_formats * len(reference_qualities) * len(reference_formats)
		no_of_com_ops = int(no_of_src_img * no_of_com_img * len(metrics))
		progressbar = tqdm(total=no_of_com_ops)

		f = open('report.txt', 'a+')
		csv = open('scores.csv', 'a+')

		while images:
			ref_list, com_list = getReferenceFiles(images, img_per_src)
	
			while ref_list:
				reference = ref_list[0]
				del ref_list[0]
		
				for comp in com_list:
					for m in metrics:
						score = img_comparison(m, reference, comp)
						out = "\n{} -> {}\n{}:\t{}".format(reference, comp, m.upper(), score)
						f.write(out)
	
						# filename,srccompRatio,srcformat,comcompRatio,comformat,metric,score
						out = "\n{},{},{},{},{},{},{}".format(getRawFilename(reference), getCompRatio(reference), 
							getFormat(reference), getCompRatio(comp), getFormat(comp), m.upper(), score)
						csv.write(out)
						progressbar.update()

		progressbar.close()
		f.close()
		csv.close()

def img_comparison(metric, src_name, comp_name):
	img_src = cv2.imread(converted_path + src_name)		# reference image
	img_comp = cv2.imread(converted_path + comp_name)	# comparable image
	return getMetric(metric, img_src, img_comp)

def printUsage():
	print("Usage: python autorun.py [options ...]\n\n"
		"Options:\n-c\t\tconvert images from 'source' folder"
		"\n-q\t\tcompute quality metrics from images in 'converted' folder\n\n"
		"Info:\nSupported convertion:\tpng, jpg, jp2 --> jpg, jxr, jp2, bpg\n"
		"Supported  metrics:\tmse, rmse, psnr, rmse_sw, uqi, ssim, ergas, scc, rase, sam, msssim, vifp\n"
		"Output data:\t\tscores.csv\nProcessing info:\treport.txt")

def getRawFilename(name):
	m = re.match(r'.*(_\d+_([a-z]|\d){3}\.png)', name)
	name = name[:-len(m.groups()[0])]
	return name

def getCompRatio(name):
	m = re.match(r'.*(_([a-z]|\d){3}\.png)', name)
	name = name[len(getRawFilename(name))+1:-len(m.groups()[0])]
	return name

def getFormat(name):
	return name[len(name)-7:-4]

def getReferenceFiles(img_list, img_per_src):
	ref_list = []
	com_list = []
	for i in img_list[:img_per_src]:
		curr_img = int(getCompRatio(i))
		if curr_img in reference_qualities:
			ref_list.append(i)
		else:
			com_list.append(i)
	del img_list[0:img_per_src]
	return ref_list, com_list

def load_arguments():
	do_conversion = 0
	do_quality = 0
	
	if len(sys.argv) < 2:
		printUsage()
		exit()
	
	for arg in sys.argv:
		if arg == "-c":
			do_conversion = 1
			os.system("make clean")
		elif arg == "-q":
			do_quality = 1
	
	os.system("make")
	return do_conversion, do_quality


def getMetric(metric, img_src, img_comp):
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
		score = compare_ssim(img_src, img_comp, full=True,  multichannel=True)[0]
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
		score = vifp_mscale(img_src, img_comp)
	else:
		print("Metric ", metric, " is NOT supported")
	return score


def vifp_mscale(ref, dist):
    sigma_nsq=2
    eps = 1e-10

    num = 0.0
    den = 0.0
    for scale in range(1, 5):
       
        N = 2**(4-scale+1) + 1
        sd = N/5.0

        if (scale > 1):
            ref = scipy.ndimage.gaussian_filter(ref, sd)
            dist = scipy.ndimage.gaussian_filter(dist, sd)
            ref = ref[::2, ::2]
            dist = dist[::2, ::2]
                
        mu1 = scipy.ndimage.gaussian_filter(ref, sd)
        mu2 = scipy.ndimage.gaussian_filter(dist, sd)
        mu1_sq = mu1 * mu1
        mu2_sq = mu2 * mu2
        mu1_mu2 = mu1 * mu2
        sigma1_sq = scipy.ndimage.gaussian_filter(ref * ref, sd) - mu1_sq
        sigma2_sq = scipy.ndimage.gaussian_filter(dist * dist, sd) - mu2_sq
        sigma12 = scipy.ndimage.gaussian_filter(ref * dist, sd) - mu1_mu2
        
        sigma1_sq[sigma1_sq<0] = 0
        sigma2_sq[sigma2_sq<0] = 0
        
        g = sigma12 / (sigma1_sq + eps)
        sv_sq = sigma2_sq - g * sigma12
        
        g[sigma1_sq<eps] = 0
        sv_sq[sigma1_sq<eps] = sigma2_sq[sigma1_sq<eps]
        sigma1_sq[sigma1_sq<eps] = 0
        
        g[sigma2_sq<eps] = 0
        sv_sq[sigma2_sq<eps] = 0
        
        sv_sq[g<0] = sigma2_sq[g<0]
        g[g<0] = 0
        sv_sq[sv_sq<=eps] = eps
        
        num += numpy.sum(numpy.log10(1 + g * g * sigma1_sq / (sv_sq + sigma_nsq)))
        den += numpy.sum(numpy.log10(1 + sigma1_sq / sigma_nsq))
        
    vifp = num/den

    return vifp


if __name__ == '__main__':
    main()