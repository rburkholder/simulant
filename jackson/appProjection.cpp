

#include <cstdlib>
#include <iostream>

#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/core/argument.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/chrono/chrono_io.hpp>

#include <wx/wx.h>
#include <wx/display.h>
#include <wx/bitmap.h>
#include <wx/rawbmp.h>
#include <wx/sizer.h>

#include <wx/filedlg.h>

extern "C" {
#include <libswscale/swscale.h>
}

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


#include "common.h"
#include "DecodeH264.h"
#include "panelSurfaceSources.h"
#include "appProjection.h"


IMPLEMENT_APP( AppProjection )


bool AppProjection::OnInit( ) {
  
  m_pTut1 = 0;  
  m_pTex = 0;
  
  m_sPictureDirectory = wxT( "~/Pictures/");
  m_sVideoDirectory = wxT( "~/Videos/");
  
  wxImage::AddHandler( new wxJPEGHandler );

  wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
  m_pFrameMain = new FrameMain( (wxFrame *)NULL, -1, wxT( "Projection Demo" ), wxPoint( 2000, 150 ), wxSize( 400, 200 ) );
  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppProjection::OnClose, this );  // start close of windows and controls

  unsigned int nDisplays = wxDisplay::GetCount();
  std::cout << "#connected displays: " << nDisplays << std::endl;
  
  //m_vFrameProjection.size( nDisplays );
  m_vScreenFrame.clear();
  m_vScreenFrame.resize( nDisplays );
  
  for ( int ix = 0; ix < nDisplays; ++ix ) {
    
    wxDisplay display( ix );
    wxRect rectClientArea = display.GetClientArea();

    std::cout << "building on " << rectClientArea.x << ", " << rectClientArea.y << std::endl;
    
    // need to keep track of projection frames, so can iconize them sometime for visual reference in the gui
    // force frame size for the time being
    m_vScreenFrame[ ix ].reset( new ScreenFrame( ix, m_pFrameMain, wxPoint( rectClientArea.x, rectClientArea.y ), wxSize( 1920, 1080 ) ) );
    
  }
  
  FrameMain::vpMenuItems_t vMenuItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vMenuItems.push_back( new mi( "select picture", boost::phoenix::bind( &AppProjection::LoadPicture, this ) ) );
  vMenuItems.push_back( new mi( "select video", boost::phoenix::bind( &AppProjection::LoadVideo, this ) ) );
  vMenuItems.push_back( new mi( "image->opengl", boost::phoenix::bind( &AppProjection::Image2OpenGL, this ) ) ) ;
  m_pFrameMain->AddDynamicMenu( "Actions", vMenuItems );
  
  vMenuItems.clear();
  vMenuItems.push_back( new mi( "Canvas", boost::phoenix::bind( &AppProjection::CreateCanvas, this ) ) );
  vMenuItems.push_back( new mi( "Movie Screen", boost::phoenix::bind( &AppProjection::CreateMovieScreen, this ) ) );
  m_pFrameMain->AddDynamicMenu( "Create", vMenuItems );
  
  m_pSurfaceSources = new panelSurfaceSources( m_pFrameMain, -1 );
  sizer->Add( m_pSurfaceSources, 1, wxEXPAND );

  m_pFrameMain->SetSizer( sizer );
  m_pFrameMain->SetAutoLayout( true );

  m_pFrameMain->Show( );
  // serialize the following for session to session persistence
  m_pFrameMain->Move( wxPoint( 2000, 150 ) );
  
  // workers for the movie action
  m_pWork = new boost::asio::io_service::work(m_Srvc);  // keep the asio service running 
  //m_thrdWorkers = boost::thread( boost::phoenix::bind( &AppProjection::Workers, this ) );
  for ( std::size_t ix = 0; ix < 2; ix++ ) {
    m_threadsWorkers.create_thread( boost::phoenix::bind( &boost::asio::io_service::run, &m_Srvc ) );
  }
  
  // WX_GL_CORE_PROFILE limits to older 3.x version
//  int argsCanvas[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_CORE_PROFILE, WX_GL_DEPTH_SIZE, 16, 0 };
//  int argsCanvas[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
//  m_pTut1 = new tut1( m_vScreenFrame[1]->GetFrame(), argsCanvas );
//  m_pTut1->SetSize( 400, 400 );
//  m_pTut1->Move( 100, 100 );
  
  // templates for windows requiring them
//  wxApp::Bind( EVENT_IMAGE, &AppProjection::HandleEventImage, this );
//  wxApp::Bind( wxEVT_MOTION, &AppProjection::HandleMouseMoved, this );
//  wxApp::Bind( wxEVT_MOUSEWHEEL, &AppProjection::HandleMouseWheel, this );
//  wxApp::Bind( wxEVT_LEFT_DOWN, &AppProjection::HandleMouseLeftDown, this );
//  wxApp::Bind( wxEVT_LEFT_UP, &AppProjection::HandleMouseLeftUp, this );
//  wxApp::Bind( wxEVT_RIGHT_DOWN, &AppProjection::HandleMouseRightDown, this );
  //wxApp::Bind( wxEVT_KEY_DOWN, &AppProjection::HandleKey, this );
  //wxApp::Bind( wxEVT_KEY_UP, &AppProjection::HandleKey, this );
  //wxApp::Bind( wxEVT_CHAR, &AppProjection::HandleKey, this ); 
  //wxApp::Bind( wxEVT_CHAR_HOOK, &AppProjection::HandleKey, this ); 
  //wxApp::Bind( wxEVT_ENTER_WINDOW, &AppProjection::HandleEnterWindow, this );  // window specific, not in app
  //wxApp::Bind( wxEVT_LEAVE_WINDOW, &AppProjection::HandleLeaveWindow, this );  // window specific, not in app
  //wxApp::Bind( wxEVT_SIZE, &AppProjection::HandleMouseMoved, this );  // window specific, not in app
  
  m_pSurfaceSources->Bind( wxEVT_TOGGLEBUTTON, &AppProjection::HandleToggleEditMode, this, panelSurfaceSources::ID_BTN_EDITMODE );
  m_pSurfaceSources->Bind( wxEVT_BUTTON, &AppProjection::HandleUndo, this, panelSurfaceSources::ID_BTN_UNDO );
  m_bInEditMode = false;
  
  return true;
}

// http://docs.wxwidgets.org/trunk/classwx_key_event.html
void AppProjection::HandleKey( wxKeyEvent& event ) {
  //if ( event.GetKeyCode() = )
  std::cout << event.GetKeyCode() << std::endl;
  event.Skip();
}

void AppProjection::OnClose( wxCloseEvent& event ) {
  // stuff to do on window close
  std::cout << "OnClose" << std::endl;
  // need to put this elsewhere so queues and gui can finish off?
  delete m_pWork;
  m_pWork = 0;
  m_threadsWorkers.join_all();
//  pm.OnPortfolioLoaded.Remove( MakeDelegate( this, &AppStickShift::HandlePortfolioLoad ) );
//  pm.OnPositionLoaded.Remove( MakeDelegate( this, &AppStickShift::HandlePositionLoaded ) );
  //m_timerGuiRefresh.Stop();
  //DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

int AppProjection::OnExit( void ) {
  // stuff to do on program exit
  std::cout << "OnExit" << std::endl;
  return 0;
}

void AppProjection::HandleUndo( wxCommandEvent& event ) {
  event.Skip();
}

void AppProjection::CreateCanvas( void ) {
  m_pOutline.reset( new Outline( wxRect( 300, 300, 600, 600 ) ) );
  m_vScreenFrame[0]->GetFrame()->SetOutline( m_pOutline );
  m_vScreenFrame[0]->GetFrame()->Refresh();
}

void AppProjection::HandleToggleEditMode( wxCommandEvent& event ) {
  if ( m_bInEditMode ) {
    m_bInEditMode = false;
    m_pSurfaceSources->m_btnEditMode->SetLabel( "Editing Off" );
  }
  else {
    m_bInEditMode = true;
    m_pSurfaceSources->m_btnEditMode->SetLabel( "Editing On" );
  }
}

void AppProjection::Workers( void ) {
  m_Srvc.run(); 
}

void AppProjection::LoadPicture( void ) {
  std::cout << "LoadPicture" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_pFrameMain, wxT("Select Image" ), m_sPictureDirectory, "", 
    //"JPG Files (*.jpg)|*.jpg", 
    _("Image Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sPictureDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    std::cout << "dir " << m_sPictureDirectory << std::endl;
    assert( m_image.LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );
    wxBitmap bitmap( m_image );
    FrameProjection* pfp = m_vScreenFrame[0]->GetFrame();
    wxClientDC dc( pfp );
    dc.DrawBitmap( bitmap, wxPoint( 0, 0 ) );
    Image2OpenGL();
  }
  else {
  }
}

void AppProjection::Image2OpenGL( void ) {
  
  if ( m_image.IsOk() ) {
    //std::cout << "is ok" << std::endl;
    if ( 0 != m_pTex ) {
      delete m_pTex;
      m_pTex = 0;
    }
    if ( 0 == m_pTex ) {
      int argsCanvas[] = { WX_GL_CORE_PROFILE, WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
      m_pTex = new tex2( m_vScreenFrame[0]->GetFrame(), argsCanvas );
      //m_pTex->SetSize( m_image.GetWidth(), m_image.GetHeight() );
      wxRect rect( 500, 100, 300, 600 );
      if ( 0 != m_pOutline.use_count() ) {
        rect = m_pOutline->GetBoundingBox();
      }
      m_pTex->SetSize( rect.GetSize() );
      m_pTex->Move( rect.GetTopLeft() );
      if ( 0 != m_pOutline.use_count() ) {
        Outline::vPoints_t vPoints;
        m_pOutline->GetCoords( vPoints );
        assert( 4 == vPoints.size() );
        
        // last transform on identity is first applied to vector
        glm::mat4 mat4Transform = glm::mat4( 1.0f ); // identity matrix
        mat4Transform *= glm::translate( glm::vec3( -1.0, +1.0 , 0.0 ) );
        mat4Transform *= glm::scale( glm::vec3( 2.0, -2.0, 1.0f ) );  // invert image and expand to window coordinates
        mat4Transform *= glm::scale( glm::vec3( 1.0 / rect.GetWidth(), 1.0 / rect.GetHeight(), 1.0f ) );  // invert image and expand to window coordinates
        mat4Transform *= glm::translate( glm::vec3( -rect.GetLeft(), -rect.GetTop(), 0.0f ) );  // translate to window coordinates
        
        std::vector<glm::vec4> vCoords;
        for ( size_t ix = 0; ix < vPoints.size(); ++ix ) {
          vCoords.push_back( mat4Transform * glm::vec4( vPoints[ix].x, vPoints[ix].y, 0.0, 1.0 ) );
        }
        m_pTex->SetWindowCoords( vCoords );
      }
      m_pTex->SetImage( &m_image );
    }

  }
  
}

//int nFrame=0;
void AppProjection::HandleOnFrame( AVCodecContext* context, AVFrame* frame, AVPacket* pkt, void* user, structTimeSteps perf ) {
  
#define FMT PIX_FMT_RGB32
//#define FMT PIX_FMT_RGB24
  
  int srcX = context->width;
  int srcY = context->height;
  size_t nBytes = avpicture_get_size( FMT, srcX, srcY );
  
  double fps = av_q2d(context->time_base);
  if(fps > 0.0) {
    //frame_delay = fps * 1000ull * 1000ull * 1000ull;
    std::cout << "fps: " << fps << std::endl;
  }
  
  struct SwsContext *swsContext = sws_getContext(srcX, srcY, context->pix_fmt, srcX, srcY, FMT, SWS_BILINEAR, NULL, NULL, NULL);
  //struct SwsContext *swsContext = sws_getContext(srcX, srcY, context->pix_fmt, srcX, srcY, PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);
  //struct SwsContext *swsContext = sws_getContext(srcX, srcY, context->pix_fmt, srcX, srcY, PIX_FMT_RGB32_1, SWS_BICUBIC, NULL, NULL, NULL);
  
  AVFrame* pRGB;
  pRGB = av_frame_alloc();
  assert( 0 != pRGB );
  
  uint8_t* buf  = (uint8_t*)av_malloc( nBytes * sizeof( uint8_t ) );  // *** todo:  keep from call to call, be aware of frame size changes in test material
  avpicture_fill( ( AVPicture*)pRGB, buf, FMT, srcX, srcY );
  
  perf.filled = boost::chrono::high_resolution_clock::now();
  
  sws_scale(swsContext, frame->data, frame->linesize, 0, srcY, pRGB->data, pRGB->linesize );
  
  perf.scaled = boost::chrono::high_resolution_clock::now();

  //HandleFrameTransform( pRGB, buf, user, perf, srcX, srcY );
  m_Srvc.post( boost::phoenix::bind( &AppProjection::HandleFrameTransform, this, pRGB, buf, user, perf, srcX, srcY ) );
  
  // ** note decode currently works faster than the transform, so transform work will queue up.
  //    need to deal with proper timing of frames.
  //    need to base frame timing on what is in the file
  //    need sync so stops at high water mark, resumes decode at low water mark
  //    be aware that if using multiple threads, that processing needs to be sync'd so frames stay in order
  //      or do frame ordering in final presentation to screen, buffer loop
}

void AppProjection::HandleFrameTransform( AVFrame* pRgb, uint8_t* buf, void* user, structTimeSteps perf, int srcX, int srcY ) {
  
  perf.queue1 = boost::chrono::high_resolution_clock::now();
  
  uint8_t* pSrc( *pRgb->data );
  
  boost::shared_ptr<wxImage> pImage( new wxImage( srcX, srcY, false ) );
  wxImage* image( pImage.get() );
  wxImagePixelData data( *image );
  wxImagePixelData::Iterator pDest( data );
  
  for ( int iy = 0; iy < srcY; ++iy ) {
    for ( int ix = 0; ix < srcX; ++ix ) {
      //++pSrc;  // skip A
      pDest.Blue() = *pSrc; ++pSrc;
      pDest.Green() = *pSrc; ++pSrc;
      pDest.Red() = *pSrc; ++pSrc;
      ++pDest;
      ++pSrc; // skip alpha?
    }
  }

  perf.copied = boost::chrono::high_resolution_clock::now();
  
  QueueEvent( new EventImage( EVENT_IMAGE, -1, pImage, user, perf ) );
  
  av_free( buf );
  av_free( pRgb );  
  
}

void AppProjection::HandleEventImage( EventImage& event ) {

  typedef boost::chrono::milliseconds ms;
  typedef boost::chrono::microseconds mu;
  static boost::chrono::duration<int64_t, boost::milli> onesec( 1000 );
  
  structTimeSteps ts( event.GetTimeSteps() );
  
  ts.queue2 = boost::chrono::high_resolution_clock::now();
  
  wxBitmap bitmap( *event.GetImage() );
  FrameProjection* pfp = (FrameProjection*) event.GetVoid();
  wxClientDC dc( pfp );
  dc.DrawBitmap( bitmap, wxPoint( 10, 10 ) );
  
  ts.drawn = boost::chrono::high_resolution_clock::now();
  
  std::cout << "stat:" 
    << "  parse "  << boost::chrono::duration_cast<mu>( ts.parse - ts.start )
    << ", decode " << boost::chrono::duration_cast<ms>( ts.decoded - ts.parse )
    << ", filled " << boost::chrono::duration_cast<mu>( ts.filled - ts.decoded )
    << ", scaled " << boost::chrono::duration_cast<ms>( ts.scaled - ts.filled )
    << ", queue1 " << boost::chrono::duration_cast<mu>( ts.queue1 - ts.scaled )
    << ", xformd " << boost::chrono::duration_cast<ms>( ts.copied - ts.queue1 )
    << ", queue2 " << boost::chrono::duration_cast<mu>( ts.queue2 - ts.copied )
    << ", drawn "  << boost::chrono::duration_cast<ms>( ts.drawn - ts.queue2 )
    << std::endl;
    
}

void AppProjection::ProcessVideoFile( boost::shared_ptr<DecodeH264> pDecoder ) {
  pDecoder->ProcessFile();
}

void AppProjection::LoadVideo( void ) {
  namespace args = boost::phoenix::arg_names;
  std::cout << "LoadPicture" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_pFrameMain, wxT("Select Video" ), m_sVideoDirectory, "", 
    //"Video Files (*.ts)|*.ts", 
    "Video Files (*.h264)|*.h264", 
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sVideoDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    std::cout << "dir " << m_sVideoDirectory << std::endl;
    //assert( m_image.LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );

    boost::shared_ptr<DecodeH264> pDecoder( new DecodeH264 ( m_vScreenFrame[0]->GetFrame() ) );
    pDecoder->m_OnFrame.connect( 
      boost::phoenix::bind( &AppProjection::HandleOnFrame, this, args::arg1, args::arg2, args::arg3, args::arg4, args::arg5 ) );

    pDecoder->load( dialogOpenFile.GetPath() );
    // ProcessFile to be handled in thread

    m_Srvc.post( boost::phoenix::bind( &AppProjection::ProcessVideoFile, this, pDecoder ) );
    
    // need to do processing in background, 
    // put frames into queue
    // pass from background to gui thread for gui update
    // use thread pool when processing multiple streams
    // therefore can handle async termination of stream
    
    // put in logger
    
  }
  else {
    
  }
    
}