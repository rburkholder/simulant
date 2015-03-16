/* 
 * File:   jackson.h
 * Author: rpb
 *
 * Created on February 28, 2015, 7:38 PM
 */

#pragma once

#include <vector>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <wx/wx.h>
#include <wx/image.h>

#include "frameMain.h"
#include "frameProjection.h"
#include "eventImage.h"

class AppProjection : public wxApp {
public:
  

protected:
private:
  
  typedef std::vector<FrameProjection*> vFrameProjection_t;
  
  FrameMain *m_pFrameMain;
  //BasicGLPane * glPane;
  
  wxString m_sPictureDirectory;
  wxString m_sVideoDirectory;
  
  wxImage m_image;
  
  //boost::thread m_thrdWorkers;
  boost::thread_group m_threadsWorkers;
  boost::asio::io_service m_Srvc;
  boost::asio::io_service::work* m_pWork;
  
  vFrameProjection_t m_vFrameProjection; // objects destroyed by wx
  
  virtual bool OnInit( );
  virtual void OnClose( wxCloseEvent& event );
  virtual int OnExit( void );

  void LoadPicture( void );
  void LoadVideo( void );
  
  void Workers( void );
  
  void HandleOnFrame( AVCodecContext* context, AVFrame* frame, AVPacket* pkt, void* user, structTimeSteps perf );
  void HandleFrameTransform( AVFrame* pRgb, uint8_t* buf, void* user, structTimeSteps perf, int srcX, int srcY );
  void HandleEventImage( EventImage& );
  
  void ProcessVideoFile( boost::shared_ptr<DecodeH264> pDecoder );
  
};

