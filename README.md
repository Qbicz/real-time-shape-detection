# real-time-shape-detection
Project goal: use GPU on ODROID-XU4 and OpenCL acceleration to achieve real time shape and orientation detection of fast moving objects.
Part of system for automatic segregation of seeds in an arboretum.

# Run code on Ubuntu using your webcam
First, please install OpenCV 2.4
```bash
sudo apt install libopencv-dev
```
Compile application:
```bash
cd PC/camera
make
```
Run the application using default camera (usually webcam).
```bash
./capture
```
You will see that in the video stream from camera the application finds an object, shows its bounding box and computes the orientation. Orientation direction is shown in form of arrows.

Example output of the algorithm running on ODROID-XU4 minicomputer with Basler acA2000-165um high-speed camera:
<img src="https://raw.githubusercontent.com/Qbicz/real-time-shape-detection/master/report/real-moving-PCA.png", alt="Contours and orientation of a moving seed" width="400">

# Some bibliography suggestions:
http://www.sci.utah.edu/~gerig/CS7960-S2010/handouts/Hu.pdf

http://www.sciencedirect.com/science/article/pii/003132039090053N

http://ro.ecu.edu.au/cgi/viewcontent.cgi?article=7351&context=ecuworks

http://www.ingentaconnect.com/content/tcsae/tcsae/2016/00000032/00000004/art00006

#Odroid XU4 user manual
http://magazine.odroid.com/wp-content/uploads/odroid-xu4-user-manual.pdf

#Configuring OpenCL on Odroid:
http://granolamatt.com/working/2015/02/configure-opencl-on-odroid-xu3/
http://forum.odroid.com/viewtopic.php?f=95&t=5559

#OpenCL introduction:
http://www.drdobbs.com/parallel/a-gentle-introduction-to-opencl/231002854
https://people.maths.ox.ac.uk/gilesm/cuda/new_lectures/lec1.pdf
https://people.maths.ox.ac.uk/gilesm/cuda/new_lectures/lec2.pdf
