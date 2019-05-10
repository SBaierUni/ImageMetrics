import matplotlib.pyplot as plt
import re
import statistics
#plt.plot([1,2,3,4])
#plt.ylabel('some numbers')
#plt.show()

def getRawFilename(name):
	m = re.match(r'.*(_\d+_([a-z]|\d){3})', name)
	name = name[:-len(m.groups()[0])-1]
	return name

def getCompRatio(name):
	m = re.match(r'.*(_([a-z]|\d){3})', name)
	name = name[len(getRawFilename(name))+1:-len(m.groups()[0])-1]
	return name

import pandas
data = pandas.read_csv('./scores.csv')

# filename,srccompRatio,srcformat,comcompRatio,comformat,metric,score
fn = data['filename']
srcRatio = data['srccompRatio']
met = data['metric']
scurr = []

for i in range(0,len(fn)):
	if (fn[i] == fn[0]) & (srcRatio[i] == srcRatio[0]) & (met[i] == met[0]):
		scurr.append(data['score'][i])

x = met
print(scurr)
y = sum(scurr) / float(len(scurr))	# average
plt.figure(figsize = (5, 5))
plt.plot(x, y, 'r--', x, y**2, 'bs', x, y**3, 'g^')
#plt.show()