Although this is a Visual C++ 2013 oriented Solution, 
it does use cross platform open source libraries.  It
should therefore be buildable with other tools and platforms.

It currently makes use of LibRaw, wxWidgets, and Boost.

Project Test5 is used for testing the capabilities of:
  LibRaw:  domosaicing
  wxWidgets:  graphical presentation and picture scaling/translation
  Boost:  various tools such as smart_ptr and timers

At some point, some OpenFrameworks-like and Processing-like features will be added 
to facilitate multi-window projections and animations via OpenGL.

This is a 64 bit oriented solution.  
The 32 bit versions are not configured to properly build.

