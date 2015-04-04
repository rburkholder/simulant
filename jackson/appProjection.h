/* 
 * File:   jackson.h
 * Author: rpb
 *
 * Created on February 28, 2015, 7:38 PM
 */

#pragma once

//#include <vector>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <wx/wx.h>
#include <wx/image.h>

#include "frameMain.h"
//#include "frameProjection.h"
#include "eventImage.h"

#include "structures.h"

#include "tut1.h"
#include "tex2.h"

class AppProjection : public wxApp {
public:
  

protected:
private:
  
  //typedef std::vector<FrameProjection*> vFrameProjection_t;
  
  FrameMain *m_pFrameMain;
  //BasicGLPane * glPane;
  
  tut1* m_pTut1;
  tex2* m_pTex;
  
  wxString m_sPictureDirectory;
  wxString m_sVideoDirectory;
  
  wxImage m_image;
  
  bool m_bInEditMode;
  
  //boost::thread m_thrdWorkers;
  boost::thread_group m_threadsWorkers;
  boost::asio::io_service m_Srvc;
  boost::asio::io_service::work* m_pWork;
  
  vScreenFrame_t m_vScreenFrame; // objects destroyed by wx
  
  virtual bool OnInit( );
  virtual void OnClose( wxCloseEvent& event );
  virtual int OnExit( void );
  
  void HandleToggleEditMode( wxCommandEvent& event );

  void LoadPicture( void );
  void LoadVideo( void );
  void Image2OpenGL( void );
  
  void CreateCanvas( void ) {};
  void CreateMovieScreen( void ) {};
  
  void Workers( void );
  
  void HandleOnFrame( AVCodecContext* context, AVFrame* frame, AVPacket* pkt, void* user, structTimeSteps perf );
  void HandleFrameTransform( AVFrame* pRgb, uint8_t* buf, void* user, structTimeSteps perf, int srcX, int srcY );
  void HandleEventImage( EventImage& );
  
  void ProcessVideoFile( boost::shared_ptr<DecodeH264> pDecoder );
  
  void HandleMouseMoved( wxMouseEvent& event ) {} // EVT_MOTION
  void HandleMouseWheel( wxMouseEvent& event ) {} // EVT_MOUSEWHEEL
  void HandleMouseLeftDown( wxMouseEvent& event ) {} // EVT_LEFT_DOWN
  void HandleMouseLeftUp( wxMouseEvent& event ) {}  // EVT_LEFT_UP
  void HandleMouseRightDown( wxMouseEvent& event ) {} // EVT_RIGHT_DOWN
//  void HandleMouseLeaveWindow( wxMouseEvent& event ) {} // EVT_LEAVE_WINDOW
  void HandleKeyDown( wxKeyEvent& event ) {} // EVT_KEY_DOWN
  void HandleKeyUp( wxKeyEvent& event ) {}  // EVT_KEY_UP
  
};
