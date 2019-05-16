This project started out as a Microsoft Visual C++ 2013 oriented Solution, 
but has since morphed into a code running primarily on Linux for now.  

As it does use cross platform open source libraries, it
should be buildable with other tools and platforms.

It currently makes use of LibRaw, LibAv, wxWidgets, and Boost.

The first main project was Test5, which is used for testing the capabilities of:
  LibRaw:  domosaicing
  wxWidgets:  graphical presentation and picture scaling/translation
  Boost:  various tools such as smart_ptr and timers
  
The current active project in this bundle is 'Jackson', which is meant 
be used for projective scenarios.  At this point, it can project to multiple 
displays, uses OpenGL for rendering, uses LibAv to open run almost any 
video file (but doesn't play the audio track yet), offers 
interactive addition/removal of images and videos, with
individual manual translation, scaling, and rotation capabilities. The
next step is provide a cue list, and to be able to run with remote 
head-ends, such as the Nvidia TK1.

At some point, some OpenFrameworks-like and Processing-like features will be added 
to facilitate and animations.

This is a 64 bit oriented solution.  
The 32 bit versions are not configured to properly build.

Something to look at
* scenegraph: https://github.com/skypjack/entt
