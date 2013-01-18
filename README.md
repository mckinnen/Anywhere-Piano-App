CGFxOpenGL is the rendering class.  THe header contains the class itself.  The cpp contains the actual drawing
of the piano.

WinMain creates the screen on which the piano is drawn.  It creates an instance of the rendering class and initializes
the properties within the class.  

Note:  The image may flash or blink.  The z-fighting is caused by the image being drawn the the background and being
swapped from the back to the foreground.  I did this project on my own, with the help of OpenGL tutorial websites and
forums.  I recevied little help from my advisor; I had no previous knowledge of OpenGL before this project.  
Here is a list of my sources:

Astle, Dave and Kevin Hawkins.  Beginning OpenGL Game Programming.  First Edition.  Course Technology(March 2004).  
http://programming-technique.blogspot.com/search/label/GLUT%20Tutorial.  C and C++ Programming Techniques.  Code on
      handling mouse event.
http://www.swiftless.com/tutorials/opengl/gldrivers.html.  OpenGL Drivers Swiftless Tutorial.  Lib and DLL files for
      OpenGL.
http://www.ambiera.com/irrklang/.  Irrklang:  An Audio Library for C, C++,and  .Net and a High Level 2D and 3D Sound
      Engine.  Drivers for the Irrklang sound engine.
http://www.songho.ca/opengl/index.html.  OpenGL.  Code for OpenGL rendering techniques.  
http://msdn.microsoft.com/en-us/library/ms685035%28VS.85%29.aspx.  Reading Input Buffer Events(Windows).  Code for
      handling the mouse and keyboard events.
http://nehe.gamedev.net/article/using_gluunproject/16013/.  NeHe Productions:  Using gluUnProject.  Code for handling
      the picking event; how to translate the window coordinates from Windows the OpenGL.
http://www.lighthouse3d.com/opengl/picking/index.php?color1.  OpenGL @ Lighthouse 3D – Picking Tutorial.  Code for the
      picking methods.
http://www.opengl.org/archives/resources/faq/technical/polygonoffset.htm.  OpenGL FAQ / 13 Drawing Lines over Polygons
      and Using Polygon Offset.  Code for eliminating z-fighting.
http://www.opengl.org/archives/resources/faq/technical/selection.htm.  OpenGL FAQ/ 20 Picking and Using Selection.  Code
      for using the Selection buffer to handle the user’s click.
http://www.gamedev.net/topic/509789-opengl-without-glut/.  OpenGL Without GLUT?  - OpenGL – GameDev.net.  Open forum.
      Code for the mouse click event.
