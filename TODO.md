# TODO

ImageCompression:
+ find better SCC (Spatial correlation) implementation
+ Should -p delete the source files ? -> currently it does
+ Terminal output text for bpg wrong, (changed from 0->51 to 51->0) ->  who cares ?
+ Support decimal digits for comRate in filename ? -> nobody needs that



Uhlibua:
2 python libraries: dlib und face_recognition

verwendet einen HOG-Face Detector (HOG + svm classifier) und face embedding model a modified rasnet34 classification model

Vorteil: es braucht nur 1 Bild zum erkennen, sprich ich habe zwei Bilder wo ich die features extrahiere und einen score herausbekomme, wenn ich die beiden vergleiche.

es sind 5 relevante code-zeilen

Hier der Link zum tutorial:
https://medium.com/data-science-lab-amsterdam/face-recognition-with-python-in-an-hour-or-two-d271324cbeb3