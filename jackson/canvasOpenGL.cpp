/* 
 * File:   panelOpenGL.cpp
 * Author: rpb
 * 
 * Created on March 15, 2015, 10:13 PM
 */

#include <wx/wx.h>

#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "canvasOpenGL.h"

//canvasOpenGL::canvasOpenGL() {
//  Init();
//}

canvasOpenGL::canvasOpenGL( wxFrame* parent, int* args )
: wxGLCanvas( parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE )
{
  Init();
}

void canvasOpenGL::Init( void ) {
  m_context = new wxGLContext( this );
  
  // To avoid flashing on MSW
  SetBackgroundStyle( wxBG_STYLE_CUSTOM );
  
  Bind( wxEVT_PAINT, &canvasOpenGL::Render, this );
  Bind( wxEVT_SIZE, &canvasOpenGL::Resized, this );
}

canvasOpenGL::~canvasOpenGL( ) {
  delete m_context;
}

void canvasOpenGL::Render( wxPaintEvent& evt ) {
  
}

void canvasOpenGL::Resized( wxSizeEvent& evt )
{
  //	wxGLCanvas::OnSize(evt);

  Refresh( );
}

