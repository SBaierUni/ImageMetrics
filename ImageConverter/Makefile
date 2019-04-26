
default:
	g++ main.cpp -o ImageConverter

.PHONY:
	cleanImages clean

cleanImages:
	rm -f converted/*.jpg
	rm -f converted/*.jp2
	rm -f converted/*.jxr
	rm -f converted/*.bpg
	rm -f converted/*.png
	rm -f report.txt
	rm -f scores.txt

clean: cleanImages
	rm -f ImageConverter
