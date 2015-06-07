/* 
 * File:   panelOpenGL.h
 * Author: rpb
 *
 * Created on March 15, 2015, 10:13 PM
 */

#pragma once

#include "CanvasBase.h"

// ===============================

template <typename CRTP>
class CanvasOpenGL: public CanvasBase  {
public:
  CanvasOpenGL( wxFrame* parent, int* args );
  virtual ~CanvasOpenGL( void );
protected:
  bool Initialized( void ) { return m_bPaintInited; }
private:
  
  bool m_bPaintInited;
  
  wxGLContext*	m_context;
  
  void OnPaint() {};  // empty placeholder for crtp
  void OnPaintInit() {};  // empty placeholder for crtp
  void OnResize( int w, int h ) {};  // empty placeholder for crtp
  
  void HandleResize( wxSizeEvent& event );
  void HandlePaint( wxPaintEvent& event );
};

template <typename CRTP>
CanvasOpenGL<CRTP>::CanvasOpenGL( wxFrame* parent, int* args )
: CanvasBase( parent, args ), 
//  wxGLCanvas( parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE ), 
  m_bPaintInited( false ), m_context( 0 )
{
  m_context = new wxGLContext( this );
  
  // To avoid flashing on MSW
  SetBackgroundStyle( wxBG_STYLE_CUSTOM );
  
  wxGLCanvas::Bind( wxEVT_PAINT, &CanvasOpenGL<CRTP>::HandlePaint, this );
  wxGLCanvas::Bind( wxEVT_SIZE, &CanvasOpenGL<CRTP>::HandleResize, this );
}

template <typename CRTP>
CanvasOpenGL<CRTP>::~CanvasOpenGL( ) {
  if ( 0 != m_context ) delete m_context;
}

template <typename CRTP>
void CanvasOpenGL<CRTP>::HandlePaint( wxPaintEvent& event ) {
  
  if (!IsShown( )) return;

  wxGLCanvas::SetCurrent( *m_context );
  wxPaintDC( this ); // only to be used in paint events. use wxClientDC to paint outside the paint event

  // opacity works with these disabled, so need to work with depth buffer to get these back
  // also need to sort by depth to get best effect
  // since all depth at 0.0, rendering against background, instead of each other
  //   so fixe that, then turn these two things back on
  // related to SETexture::Init
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);  
  
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  
  // debug output
  glEnable(GL_DEBUG_OUTPUT);
  
  // stuff starts here down 
  
  if ( !m_bPaintInited ) {
    m_bPaintInited = true;
    // https://www.opengl.org/sdk/docs/man/html/glGetString.xhtml
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;  
//    auto res1 = glGetString(GL_VERSION);
//    auto res2 = glGetString(GL_RENDERER);
//    auto res3 = glGetString(GL_SHADING_LANGUAGE_VERSION);
//    auto res4 = glGetString(GL_VENDOR);
    
    GLint units;
    glGetIntegerv( GL_MAX_TEXTURE_UNITS, &units );
    std::cout << "Texture Units " << units << std::endl;

    if ( &CanvasOpenGL<CRTP>::OnPaintInit != &CRTP::OnPaintInit ) {
      static_cast<CRTP*>( this )->OnPaintInit();
    }
  }

  if ( &CanvasOpenGL<CRTP>::OnPaint != &CRTP::OnPaint ) {
    static_cast<CRTP*>( this )->OnPaint();
  }

  // stuff ends above to here  
  //
  
  //glFlush( ); // https://www.opengl.org/wiki/Common_Mistakes
  SwapBuffers( );
  
}

template <typename CRTP>
void CanvasOpenGL<CRTP>::HandleResize( wxSizeEvent& event ) {
  //	wxGLCanvas::OnSize(evt);
  
  if ( &CanvasOpenGL<CRTP>::OnResize != &CRTP::OnResize ) {
    static_cast<CRTP*>( this )->OnResize( event.GetSize().GetWidth(), event.GetSize().GetHeight() );
  }
  
  Refresh( );
}

