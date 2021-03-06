# Embroidery Software

This project is about converting a digital image into a file that can be executed by an embroidery machine.

<img src="https://i.imgur.com/sWTMbZu.png" height = "190" width = "910" >

I tested this program and sewed the image (as shown above) using a machine I made (pictured below) that runs on G-code like many CNC machines and 3D printers.

<p align="center">
  <img  src="https://i.imgur.com/Y3kRSzh.png">
</p>

The program takes an image as an input and provides a G-code file as an output. The loading of the converted file into [a G-code platform](https://winder.github.io/ugs_website/) and its subsequent execution can be seen above.

A user interface for this project was made using the [Qt framework](https://www.qt.io/) and provides visuals for [the original conversion program](embroidery_software.cpp).

https://user-images.githubusercontent.com/70965673/133517213-cfee52fa-385a-47b1-8668-8ada1cb62c95.mp4

## Principle of Operation
This section provides a general explanation of how the program works.

The first step involves loading the image's pixels (their RGB values more specifically) into a two-dimensional array. The next step is to reduce the number of colors present in the image. To do this, the image is posterized and then the image's dominant colors (colors with the most pixels) are selected by the user. With the dominant colors selected and in order to express the image in just these colors, every pixel's color is converted to the closest dominant color as determined by calculating the color distance using the Pythagorean theorem.
After this, the dominant colors comprising the areas of the image that are to be sewn are selected.

This selection of colors starts an organizational hierarchy which is followed by shapes and 'pockets'. This hierarchy provides a means of generating and sorting shapes that can be exported and sewn. The base unit is the 'pocket' which represents the vertical, start and end coordinates of an unbroken, unicolor, horizontal length of pixels. These coordinates end up getting translated to physical coordinates that tell the machine where to stitch in space.

<p align="center">
  <img  src="https://i.imgur.com/cVs6Jbm.png" height = "337" width = "667" >
</p>

Pockets are implemented as a class with members for each of the three coordinates needed and are obtained one color and subsequently one row at a time. Pockets are obtained by performing a scan of the two-dimensional array during which the pocket coordinates are recorded and set.

Going up the hierarchy, shapes consist of a collection of adjacent pockets that form a whole and shapes are subsequently organized by color.

<p align="center">
  <img  src="https://i.imgur.com/h9P5Iik.png" height = "445" width = "667" >
</p>

Shapes are generated by starting with an arbitrary pocket of one color and seeing which pockets of the same color are adjacent to or contact the arbitrary pocket. These pockets are then added to the gradually generated shape and are similarly compared with the rest of the pockets of that color. The process repeats itself to generate new shapes for the colors selected. 

Once the pockets are organized into shapes, the order in which they are stored needs to be optimized to provide a path for the machine that will minimize the crossing over of thread while sewing and yield an orderly pattern. Pockets are organized such that adjacent pockets are stored alongside one another. 

<p align="center">
  <img  src="https://i.imgur.com/dJlm7If.png" >
</p>

With collections of sorted pockets that correspond to shapes that can be sewn, the coordinates of each pocket can now be translated to physical values with units and exported so that the image can be sewn by a machine. 
