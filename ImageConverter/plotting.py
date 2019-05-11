import matplotlib.pyplot as plt
import re
import statistics
#plt.plot([1,2,3,4])
#plt.ylabel('some numbers')
#plt.show()
import pandas as pd


data = pd.read_csv('./scores.csv')

# filename,srccompRatio,srcformat,comcompRatio,comformat,metric,score
fn = data['filename']
srcRatio = data['srccompRatio']
met = data['metric']
scurr = []


filename = data.filename.drop_duplicates()	# only the filenames
srcformats = data.srcformat.drop_duplicates()	# only the filenames
ratios = data.srccompRatio.drop_duplicates()	# only the filenames
metrics = data.metric.drop_duplicates()	# only the filenames
formats = data.comformat.drop_duplicates()	# only the filenames


# take all files with identical srccompRatio and srcformat
# take all files with identical comformat, comcompRatio is ignored in the graphs
# take all files with identical metric and take average score (should be 4 different avg. scores)
#cs = pd.DataFrame(data)

#for x in range(0, 1000):
#current_tracks = data[data.srccompRatio == data.srccompRatio[0]]
#current_tracks = current_tracks[current_tracks.srcformat == current_tracks.srcformat[0]]
#current_tracks = current_tracks[current_tracks.comformat == current_tracks.comformat[0]]
#current_tracks = current_tracks[current_tracks.metric == current_tracks.metric[0]]

#avg = sum(current_tracks.score) / float(len(current_tracks))	# average

##print(avg)
#print(current_tracks)
dat = []
out = []

for f in srcformats:
	for r in ratios:
		for form in formats:
			for m in metrics:
				lst = []
				for file in filename:
					sc = data.loc[(data["filename"] == file) & (data["srccompRatio"] == r) & (data["srcformat"] == f) & 
					(data["comformat"] == form) & (data["metric"] == m)]["score"]
					sc = sum(sc) / float(len(sc))	# average
					lst.append(sc)
				lst = sum(lst) / float(len(lst))	# average
				if [form, m] not in dat:
					dat.append([form, m])
					out.append([[form, m], lst])


print(out)


#data = data.drop(["test_10_jp2", "test_10_jpg"])



#x = met
#print(scurr)
#y = sum(scurr) / float(len(scurr))	# average
#plt.figure(figsize = (5, 5))
#plt.plot(x, y, 'r--', x, y**2, 'bs', x, y**3, 'g^')
#plt.show()