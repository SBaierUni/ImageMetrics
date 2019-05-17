# ImageMetrics
Multimedia PS

## Autorun Usage

```
> python autorun.py
```
No filenames with '_' allowed.


## Plotting Scores

```
> python plot_scores.py
```



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