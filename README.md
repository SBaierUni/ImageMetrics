# ImageMetrics
Multimedia PS

## General Usage
Put the images into a folder called "source" in the ImageConverter directory. 
No image filenames with '_' are allowed.
Run the automated python-script with:

```
> python autorun.py
```
The converted images are stored in a folder named "converted".
The image metrics are stored in a file named "scores.csv" and the whole report in a file named "report.txt".
After finishing you can remove all generated files with:

```
> make clean
```


## Plotting Scores

```
> python plot_scores_simple.py
```



## Image Compression
### Usage

```
> ./ImageConverter
```

### Available Compression Qualities

| FORMAT | MINIMAL COMPRESSION | MAXIMAL COMPRESSION  |
|:------:|:-------------------:|:--------------------:|
| jpeg   | 100 			       | 0  				  |
| jxr    | 100     			   | 0  				  |
| jp2    | 100    		 	   | 0  				  |
| bpg    | 51   		 	   | 0 				 	  |


## Image Quality
### Dependencies
Python Sewar Library
```
> pip install sewar
```

+ supported formats: png, jpg, jp2

| METRIC  | BEST	| WORSE 	  | STEPSIZE	| SPEED  | PREFERED |
|:-------:|:-------:|:-----------:|:-----------:|:------:|:--------:|
| MSE	  |	0		| higher	  | normal 		| fast   |          |
| RMSE 	  | 0		| higher	  | normal 		| fast   |          |
| PSNR	  | infinite| smaller	  | normal 		| fast   |     *    |
| RMSE_SW | 0		| higer		  | normal 		| fast   |          |
| UQI	  |	1		| smaller	  | normal 		| normal |     *    |
| SSIM	  | 1		| smaller	  | normal 		| slow   |     *    |
| ERGAS	  | 0		| higher	  | very big 	| normal |          |
| SCC	  |	0.98  	| smaller	  | exponential | slow   |     *    |
| RASE	  | 0 		| higher	  | big 		| normal |          |
| SAM 	  | 0 		| higher	  | very small 	| fast   |     *    |
| MSSSIM  | 1		| smaller	  | normal		| slow   |          |
| VIFP	  | 0.99 	| smaller	  | normal 		| slow   |     *    |



+ Mean Squared Error (MSE)
+ Root Mean Sqaured Error (RMSE)
+ Peak Signal-to-Noise Ratio (PSNR)
+ Structural Similarity Index (SSIM)
+ Universal Quality Image Index (UQI)
+ Erreur Relative Globale Adimensionnelle de Synthèse (ERGAS)
+ Spatial Correlation Coefficient (SCC)
+ Relative Average Spectral Error (RASE)
+ Spectral Angle Mapper (SAM)
+ Multi-scale Structural Similarity Index (MS-SSIM)
+ Visual Information Fidelity (VIF)