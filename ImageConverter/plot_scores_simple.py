import matplotlib.pyplot as plt
import pandas as pd
import sys

format_ratio = ["jp2_10", "jp2_30", "jpg_10", "jpg_30"]
metric = ["PSNR", "UQI", "SSIM", "SCC", "SAM", "VIFP"]

if (len(sys.argv) < 2):
		print("Usage: python autorun.py pick metric\n\n"
		 "Available picks:\n1\tjp2_10\n2\tjp2_30\n3\tjpg_10\n4\tjpg_30\n"
		 "Available metrics:\n1\tPSNR\n2\tUQI\n3\tSSIM\n4\tSCC\n5\tSAM\n6\tVIFP\n")
		exit()

format_ratio_pick = int(sys.argv[1])-1
metric_pick = int(sys.argv[2])-1

data = pd.read_csv('./scores.csv')

# original_filename,src_ratio,src_format,com_ratio,com_format,metric,score
filename = data.original_filename.drop_duplicates()	
src_formats = data.src_format.drop_duplicates()
src_ratios = data.src_ratio.drop_duplicates()
metrics = data.metric.drop_duplicates()		
com_formats = data.com_format.drop_duplicates()
com_ratios = data.com_ratio.drop_duplicates()

outputs = []	# different scrRatios and scrFormats

for sf in src_formats:
	tmp = data.loc[data["src_format"] == sf]
	for sr in src_ratios:
		tmp1 = tmp.loc[tmp["src_ratio"] == sr]
		dat = []
		df = pd.DataFrame(columns=['format', 'ratio', 'metric', 'avg'])
		for f in com_formats:
			tmp2 = tmp1.loc[tmp1["com_format"] == f]
			for cr in com_ratios:
				tmp3 = tmp2.loc[tmp2["com_ratio"] == cr]
				for m in metrics:
					tmp4 = tmp3.loc[tmp3["metric"] == m]
					avg = 0
					for file in filename:
						sc = tmp4["score"]	# relevant scores from all different images
						avg = avg + sum(sc) / float(len(sc))	# average
					if [f, cr, m] not in dat:
						dat.append([f, cr, m])
						df = df.append({"format" : str(f), "ratio" : str(cr), "metric" : m, "avg" : avg / float(len(filename))}, ignore_index=True)
		outputs.append([sf, sr, df])

plt.figure()
sf, sr, df = outputs[format_ratio_pick]
plt.title(sf + "_" + str(sr) + "_" + metric[metric_pick])

df = df[df["metric"] == metric[metric_pick]]

for f in com_formats:
	plt.plot(df[df["format"] == f]["ratio"], df[df["format"] == f]["avg"], "x-", label=f)
	#plt.errorbar(df[df["format"] == f]["ratio"], df[df["format"] == f]["avg"], e, linestyle='None', marker='^')

plt.xlabel('Compression Ratio')
plt.ylabel('Score')
plt.legend()
plt.grid(True,which="major",ls="-", color='0.6')
plt.grid(True,which="minor",ls="--", color='0.75')
plt.show()