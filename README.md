# ImageMetrics
Multimedia PS


## Image Compression
### Usage

+ add input-image-path to imageList.txt

```
> ./ImageConverter -a -p -c 10
```

### Available Compression Qualities

| FORMAT | MINIMAL COMPRESSION | MAXIMAL COMPRESSION  |
|:------:|:-------------------:|:--------------------:|
| jpeg   | 100 			       | 0  				  |
| jxr    | 100     			   | 0  				  |
| jp2    | 100    		 	   | 0  				  |
| bpg    | 0   			 	   | 51 				  |


## Image Quality
### Dependencies
Python Sewar Library
```
> pip install sewar
```

### Usage

```
> python metrics.py -metric res/reference.png res/gaben_bpg_30.png
```
+ supported formats: png, jpg, jp2

| METRIC  | BEST	| WORSE 	  | STEPSIZE	|
|:-------:|:-------:|:-----------:|:-----------:|
| MSE	  |	0		| higher	  | normal 		|
| RSME 	  | 0		| higher	  | normal 		|
| PSNR	  | infinite| smaller	  | normal 		|
| RMSE_SW | 0		| higer		  | normal 		|
| UQI	  |	1		| smaller	  | normal 		|
| SSIM	  | 1		| smaller	  | normal 		|
| ERGAS	  | 0		| higher	  | very big 	|
| SCC	  |	0.98  	| smaller	  | exponential |
| RASE	  | 0 		| higher	  | big 		|
| SAM 	  | 0 		| higher	  | very small 	|
| MSSSIM  | 1		| lower		  | normal		|
| VIFP	  | 0.99 	| smaller	  | normal 		|