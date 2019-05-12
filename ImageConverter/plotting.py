import matplotlib.pyplot as plt
import re
import statistics
import pandas as pd

data = pd.read_csv('./scores.csv')

# filename,srccompRatio,srcformat,comcompRatio,comformat,metric,score
filename = data.filename.drop_duplicates()	
srcformats = data.srcformat.drop_duplicates()
ratios = data.srccompRatio.drop_duplicates()
metrics = data.metric.drop_duplicates()		
formats = data.comformat.drop_duplicates()

dat = []
penis = []
df = pd.DataFrame(columns=['format', 'metric', 'avg'])

for f in srcformats:
	for r in ratios:
		for fo in formats:
			for m in metrics:
				avg = 0
				for file in filename:
					sc = data.loc[(data["filename"] == file) & (data["srccompRatio"] == r) & (data["srcformat"] == f) & 
					(data["comformat"] == fo) & (data["metric"] == m)]["score"]
					sc = sum(sc) / float(len(sc))	# average
					avg = avg + sc
				if [fo, m] not in dat:
					dat.append([fo, m])
					df = df.append({"format" : fo, "metric" : m, "avg" : avg / float(len(filename))}, ignore_index=True)

plt.figure(figsize = (5, 5))
plt.yscale("log")
plt.title(filename[0])
for m in metrics:
	plt.plot(df[df["metric"] == m]["format"], df[df["metric"] == m]["avg"], "x-", label=m)
plt.legend()
plt.show()