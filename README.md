# promo
Procedural tree modeling project

                                    PROMO

Dependencies
------------
- Qt Creator with Qt SDK
- MinGW 32 bit compiler
- libQGLViewer
- GLUT
- Boost


Known software version compatibility
------------------------------------
- Qt Creator 2.0.1 with Qt SDK 4.7 and libQGLViewer 2.3.17. 
- Qt SDK 5.2 with libQGLViewer  2.5.0
	

Installation
------------
Setup:
- libQGLViewer needs to be compiled from the source. The current compiled
  version will not work with QT5.
  
To build :
- Open the project "promo.pro" using Qt Creator
- First build requests the user to delete the "promo.pro.user" file, to erase 
    the default configuration for building, so that the client may configure 
	his own ".user" automatically
- Some file path in the "promo.pro" file might need to be adapted for your system
   (path to libQGLViewer libs and includepath)
- Build and Run


Use
---
Examples are stored in "\promo\examples",
To run :
- Menu File->Load,
- Select ".txt" file,
- Click on the "Update" button.
Some features are works in progress.


Know issues
-----------
- libQGLView can't be stored in a frame
- Clicking the update button will add the number in "n" to the total number
    of iterations to do. For example, generating a system with a n of 6 then clicking
    update again will generate a system with n=12
