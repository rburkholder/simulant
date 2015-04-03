/* 
 * File:   panelOpenGL.h
 * Author: rpb
 *
 * Created on March 15, 2015, 10:13 PM
 */

#pragma once

#include <wx/wx.h>
#include <wx/glcanvas.h>

#ifdef __WXMAC__
#include "OpenGL/glu.h"
#include "OpenGL/gl.h"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif


template <typename CRTP>
class canvasOpenGL: public wxGLCanvas  {
public:
  canvasOpenGL( wxFrame* parent, int* args );
  virtual ~canvasOpenGL( );
protected:
private:
  
  bool m_bFirstTime;
  wxGLContext*	m_context;
  
  void OnPaint() {};
  void OnPaintInit() {};
  void OnResize( int w, int h ) {};
  
  void Resized( wxSizeEvent& evt );
  void Render( wxPaintEvent& evt );
};

template <typename CRTP>
canvasOpenGL<CRTP>::canvasOpenGL( wxFrame* parent, int* args )
: wxGLCanvas( parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE ),
  m_bFirstTime( true ), m_context( 0 )
{
  m_context = new wxGLContext( this );
  
  // To avoid flashing on MSW
  SetBackgroundStyle( wxBG_STYLE_CUSTOM );
  
  wxGLCanvas::Bind( wxEVT_PAINT, &canvasOpenGL<CRTP>::Render, this );
  wxGLCanvas::Bind( wxEVT_SIZE, &canvasOpenGL<CRTP>::Resized, this );
}

template <typename CRTP>
canvasOpenGL<CRTP>::~canvasOpenGL( ) {
  if ( 0 != m_context ) delete m_context;
}

template <typename CRTP>
void canvasOpenGL<CRTP>::Render( wxPaintEvent& evt ) {
  
  //std::cout << "render" << std::endl;
  
  if (!IsShown( )) return;

  wxGLCanvas::SetCurrent( *m_context );
  wxPaintDC( this ); // only to be used in paint events. use wxClientDC to paint outside the paint event

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable(GL_DEBUG_OUTPUT);
  
  //
  // stuff starts here down 
  
  if ( m_bFirstTime ) {
    m_bFirstTime = false;
    // https://www.opengl.org/sdk/docs/man/html/glGetString.xhtml
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;  
//    auto res1 = glGetString(GL_VERSION);
//    auto res2 = glGetString(GL_RENDERER);
//    auto res3 = glGetString(GL_SHADING_LANGUAGE_VERSION);
//    auto res4 = glGetString(GL_VENDOR);

    if ( &canvasOpenGL<CRTP>::OnPaintInit != &CRTP::OnPaintInit ) {
      static_cast<CRTP*>( this )->OnPaintInit();
    }
  }

  if ( &canvasOpenGL<CRTP>::OnPaint != &CRTP::OnPaint ) {
    static_cast<CRTP*>( this )->OnPaint();
  }

  // stuff ends above to here  
  //
  
  glFlush( );
  SwapBuffers( );
  
}

template <typename CRTP>
void canvasOpenGL<CRTP>::Resized( wxSizeEvent& evt ) {
  //	wxGLCanvas::OnSize(evt);
  
  if ( &canvasOpenGL<CRTP>::OnResize != &CRTP::OnResize ) {
    static_cast<CRTP*>( this )->OnResize( evt.GetSize().GetWidth(), evt.GetSize().GetWidth() );
  }
  
  Refresh( );
}

