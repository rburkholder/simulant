/* 
 * File:   TreeDisplayManager.cpp
 * Author: rpb
 * 
 * Created on April 5, 2015, 10:11 PM
 */

#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

//#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/core/argument.hpp>

#include <boost/signals2.hpp>
#include <boost/chrono/chrono_io.hpp>

#include <wx/menu.h>
#include <wx/image.h>

#include <wx/bitmap.h>
#include <wx/rawbmp.h>

#include <wx/filedlg.h>

extern "C" {
#include <libswscale/swscale.h>
}

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "common.h"
#include "DecodeH264.h"

//#include "tut1.h"
#include "tex2.h"

#include "EventImage.h"
#include "EventGenerateFrame.h"

#include "Outline.h"

#include "FpsGenerator.h"

#include "SceneElement.h"
#include "SEGrid.h"
#include "SceneManager.h"

#include "InteractiveTransform.h"
#include "TreeDisplayManager.h"

FpsGenerator fps;  // generate signals for frame rate control

IMPLEMENT_DYNAMIC_CLASS( TreeDisplayManager, wxTreeCtrl )

class TreeItemBase {
public:
  
  typedef boost::shared_ptr<TreeItemBase> pTreeItem_t;
  
  TreeItemBase( TreeDisplayManager* pTree_, wxTreeItemId id_ ): m_pTree( pTree_ ), m_id( id_ ) {}
  virtual ~TreeItemBase( void ) {}
  virtual void ShowContextMenu( void ) {}
  virtual void SetSelected( void ) {}
  virtual void RemoveSelected( void ) {}
  virtual void DeletingChild( wxTreeItemId id ) {};
  wxTreeItemId GetTreeItemId( void ) { return m_id; }
protected:
  wxTreeItemId m_id;  // identifier of this part of the tree control
  TreeDisplayManager* m_pTree;  // used for assigning the popup, plus other base class functions, eg for Bind, etc
private:
};

// ================

class TreeItemRoot: public TreeItemBase {
public:
  // deals with constructing display surfaces
  TreeItemRoot( TreeDisplayManager* pTree_, wxTreeItemId id_ ): TreeItemBase( pTree_, id_ ) {
  };
  virtual void ShowContextMenu( void ) {
    //wxMenu* pMenu = new wxMenu( "Surfaces");
    wxMenu* pMenu = new wxMenu();
    pMenu->Append( MIAddScreenFrame, "&Add Screen Frame" );
    pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemRoot::HandleAddScreenFrame, this, MIAddScreenFrame );
    m_pTree->PopupMenu( pMenu );
  }
protected:
private:
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddScreenFrame
  };
  void HandleAddScreenFrame( wxCommandEvent& event ) {  // for remote displays, will use wizard dialog
    std::cout << "Add Remote Screen Frame" << std::endl;  
  }
};

// ================

class TreeItemVisualCommon: public TreeItemBase, public InteractiveTransform {
public:
  
  typedef boost::shared_ptr<PhysicalDisplay> pPhysicalDisplay_t;
  typedef boost::shared_ptr<SceneManager> pSceneManager_t;
  
  typedef boost::signals2::signal<void ( const glm::mat4& )> signalTransformUpdated_t;
  typedef signalTransformUpdated_t::slot_type slotTransformUpdated_t;
  
  boost::signals2::connection Connect( const slotTransformUpdated_t& slot ) {
    return m_signalTransformUpdated.connect( slot );
  }
  
  void GetTransformMatrix( glm::mat4& matrix ) const { matrix = m_mat4Transform; };
  
  TreeItemVisualCommon( TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  virtual ~TreeItemVisualCommon( void );
  
  void HandleDelete( wxCommandEvent& event );  // compiler doesn't like in protected
  void HandleReset( wxCommandEvent& event );  // compiler doesn't like in protected
  
protected:
  
  volatile bool m_bActive;
  
  signalTransformUpdated_t m_signalTransformUpdated;
  
  boost::signals2::connection m_slotTimer;
  
  pPhysicalDisplay_t m_pPhysicalDisplay;
  pSceneManager_t m_pSceneManager;
  
  virtual void UpdateTransformMatrix( void ) {};

  void SetSelected( void );  // from tree menu
  void RemoveSelected( void );  // from tree menu
  
  void HandleRefreshTimer( FpsGenerator::FPS fps );  // is in work thread
  void HandleRefresh( EventGenerateFrame& event );
  
private:
};

TreeItemVisualCommon::TreeItemVisualCommon( TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager ) 
: TreeItemBase( pTree, id ), 
  InteractiveTransform( pPhysicalDisplay->GetFrame()->GetClientSize().GetWidth(), pPhysicalDisplay->GetFrame()->GetClientSize().GetHeight() ), 
  m_bActive( false ), m_pPhysicalDisplay( pPhysicalDisplay ), m_pSceneManager( pSceneManager )
{
  
  wxApp::GetInstance()->Bind( EVENT_GENERATEFRAME, &TreeItemVisualCommon::HandleRefresh, this ); 
  //m_pScreenFrame->GetFrame()->Bind( EVENT_GENERATEFRAME, &TreeItemCanvasGrid::HandleRefresh, this );  // doesn't propagate properly
  
  namespace args = boost::phoenix::arg_names;
  m_slotTimer = fps.Connect( FpsGenerator::fps24, boost::phoenix::bind( &TreeItemVisualCommon::HandleRefreshTimer, this, args::arg1 ) );
 
  m_bActive = true;
  
}

TreeItemVisualCommon::~TreeItemVisualCommon( void ) {
  m_slotTimer.disconnect();
  wxApp::GetInstance()->Unbind( EVENT_GENERATEFRAME, &TreeItemVisualCommon::HandleRefresh, this );
  m_bActive = false;
}

void TreeItemVisualCommon::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Tree Item Delete" << std::endl;
  m_pTree->Delete( this->m_id );
}

void TreeItemVisualCommon::HandleReset( wxCommandEvent& event ) {
  std::cout << "Reset" << std::endl;
  ResetTransformMatrix();
  UpdateTransformMatrix();
}

void TreeItemVisualCommon::SetSelected( void ) {
  InteractiveTransform::Activate( m_pSceneManager.get() );
}

void TreeItemVisualCommon::RemoveSelected( void ) {
  InteractiveTransform::DeActivate();
}

void TreeItemVisualCommon::HandleRefreshTimer( FpsGenerator::FPS fps ) {
  if ( m_bActive ) { // cross thread action
    wxApp::GetInstance()->QueueEvent( new EventGenerateFrame( EVENT_GENERATEFRAME, m_pPhysicalDisplay->GetFrame()->GetId() ) );
  }
}

void TreeItemVisualCommon::HandleRefresh( EventGenerateFrame& event ) {
  if ( m_pPhysicalDisplay->GetFrame()->GetId() == event.GetId() ) {
    m_pSceneManager->Refresh(); // this isn't the right way, as it will get called to many times
    //  when registering, registers with a specific fps queue
    // this should set a flag, so SceneManager draws everything, or does an auto refresh.
  }
  else {
    //std::cout << "not our event" << std::endl;  // this does get hit, so the if above is appropriate
  }
  event.Skip( true );  // let other ones process this as well
}

// ================

class TreeItemGrid: public TreeItemVisualCommon {
public:
  
  typedef TreeItemVisualCommon::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef TreeItemVisualCommon::pSceneManager_t pSceneManager_t;
  
  TreeItemGrid( 
    TreeDisplayManager* pTree_, wxTreeItemId id_, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  ~TreeItemGrid( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIDelete, MIReset
  };
  
  typedef SceneManager::key_t key_t;
  typedef boost::shared_ptr<SEGrid> pSEGrid_t;
  
  pSEGrid_t m_pGrid;
  key_t m_keyGrid;
  
  virtual void UpdateTransformMatrix( void );
  
};

TreeItemGrid::TreeItemGrid( 
  TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager)
: 
  TreeItemVisualCommon( pTree, id, pPhysicalDisplay, pSceneManager ),
  m_keyGrid( 0 )
{
  
  std::cout << "Tree Item Add Grid" << std::endl;
  
  wxImage::AddHandler( new wxJPEGHandler );

  m_pGrid.reset( new SEGrid );
  m_keyGrid = m_pSceneManager->Add( m_pGrid );
 
  ResetTransformMatrix();
  UpdateTransformMatrix();
  
}

TreeItemGrid::~TreeItemGrid( void ) {
  if ( 0 != m_keyGrid ) 
    m_pSceneManager->Delete( m_keyGrid );
}

void TreeItemGrid::UpdateTransformMatrix( void ) {
  m_pGrid->UpdateTransform( m_mat4Transform );
  m_signalTransformUpdated( m_mat4Transform );
}

void TreeItemGrid::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIReset, "Reset" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleReset, this, MIReset );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleDelete, this, MIDelete );
  
  m_pTree->PopupMenu( pMenu );
}

// ================

class TreeItemImageCommon: public TreeItemVisualCommon {
public:
  
  typedef TreeItemVisualCommon::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef TreeItemVisualCommon::pSceneManager_t pSceneManager_t;
  
  TreeItemImageCommon( TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  ~TreeItemImageCommon( void );
  
protected:
  
  typedef SETexture::pImage_t pImage_t;
  
  static wxString m_sPictureDirectory;
  static wxString m_sVideoDirectory;
  
  void SetImage( pImage_t pImage );
  
  virtual void UpdateTransformMatrix( void );
  
private:
  
  typedef SceneManager::key_t key_t;
  typedef boost::shared_ptr<SETexture> pSETexture_t;
  
  pImage_t m_pImage;
  
  pSETexture_t m_pTexture;
  key_t m_keyTexture;
  
};

wxString TreeItemImageCommon::m_sPictureDirectory( wxT( "~/Pictures/" ) );
wxString TreeItemImageCommon::m_sVideoDirectory( wxT( "~/Videos/") );

TreeItemImageCommon::TreeItemImageCommon( 
  TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager )
: TreeItemVisualCommon( pTree, id, pPhysicalDisplay, pSceneManager ),
  m_keyTexture( 0 )
{
  
  std::cout << "Tree Item Add Image Common" << std::endl;
  
  wxImage::AddHandler( new wxJPEGHandler );

  m_pTexture.reset( new SETexture );
  m_keyTexture = m_pSceneManager->Add( m_pTexture );
  m_pTexture->SetTransform( InteractiveTransform::m_mat4Transform );
    
}

TreeItemImageCommon::~TreeItemImageCommon( void ) {
  if ( 0 != m_keyTexture ) 
    m_pSceneManager->Delete( m_keyTexture );
}

void TreeItemImageCommon::SetImage( pImage_t pImage ) {  // load picture and create object
  assert( 0 != m_pTexture.use_count() );
  assert( 0 != pImage.use_count() );
  if ( pImage->IsOk() ) {
    m_pTexture->SetImage( pImage );
  }
  // some old remnants for posterity's reference
//    FrameProjection* pfp = m_pPhysicalDisplay->GetFrame();
//    wxClientDC dc( pfp );
//    dc.DrawBitmap( bitmap, wxPoint( 0, 0 ) );
  
}

void TreeItemImageCommon::UpdateTransformMatrix( void ) {
  if ( 0 != m_pTexture.get() ) {
    m_pTexture->SetTransform( m_mat4Transform );
  }
  m_signalTransformUpdated( m_mat4Transform );
}

// ================

class TreeItemImage: public TreeItemImageCommon {
public:
  
  typedef TreeItemVisualCommon::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef TreeItemVisualCommon::pSceneManager_t pSceneManager_t;
  
  TreeItemImage( TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  ~TreeItemImage( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIDelete, MIReset, MILoadImage
  };
  
  typedef SETexture::pImage_t pImage_t;
  
  void LoadImage( void );
  void LoadImageCommon( pImage_t pImage );

  void HandleLoadImage( wxCommandEvent& event );  // reuse existing Scene Element
  
};

TreeItemImage::TreeItemImage( 
  TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager )
: TreeItemImageCommon( pTree, id, pPhysicalDisplay, pSceneManager )
{
  
  std::cout << "Tree Item Add Image" << std::endl;
  
  ResetTransformMatrix();
  
  LoadImage();
  
  UpdateTransformMatrix();
  
}

TreeItemImage::~TreeItemImage( void ) {
}

void TreeItemImage::HandleLoadImage( wxCommandEvent& event ) {
  LoadImage();
}

void TreeItemImage::LoadImage( void ) {  // on class instantiation, load image, create related objects
  
  pImage_t pImage( new wxImage );
  
  LoadImageCommon( pImage );
  TreeItemImageCommon::SetImage( pImage );
}

void TreeItemImage::LoadImageCommon( pImage_t pImage ) {
  
  std::cout << "TreeItemCanvasGrid LoadImage" << std::endl;  
  
  wxFileDialog dialogOpenFile( 
    m_pPhysicalDisplay->GetFrame(), wxT("Select Image" ), m_sPictureDirectory, "", 
    //"JPG Files (*.jpg)|*.jpg", 
    _("Image Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sPictureDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    std::cout << "dir " << m_sPictureDirectory << std::endl;
    assert( 0 != pImage.use_count() );
    //wxImage& image( pImage.)
    assert( pImage->LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );
  }
  
}

void TreeItemImage::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MILoadImage, "Load &Image" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemImage::HandleLoadImage, this, MILoadImage );
  
  pMenu->Append( MIReset, "Reset" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleReset, this, MIReset );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleDelete, this, MIDelete );
  
  m_pTree->PopupMenu( pMenu );
}

// ================

class TreeItemVideo: public TreeItemImageCommon {
public:
  
  typedef TreeItemVisualCommon::pPhysicalDisplay_t pPhysicalDisplay_t;
  typedef TreeItemVisualCommon::pSceneManager_t pSceneManager_t;
  
  TreeItemVideo( TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager );
  ~TreeItemVideo( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIReset, MIDelete, MISelectVideo
  };
  
  typedef SceneManager::key_t key_t;
  typedef boost::shared_ptr<SETexture> pSETexture_t;
  
  pSETexture_t m_pTexture;
  key_t m_keyTexture;
  
  wxString m_sPictureDirectory;
  wxString m_sVideoDirectory;
  
  wxImage m_image;
  
  boost::thread_group m_threadsWorkers;
  boost::asio::io_service m_Srvc;
  boost::asio::io_service::work* m_pWork;
  
  void Workers( void );  // background processing of video
  
  void ProcessVideoFile( boost::shared_ptr<DecodeH264> pDecoder );
  
  void HandleOnFrame( AVCodecContext* context, AVFrame* frame, AVPacket* pkt, void* user, structTimeSteps perf );
  void HandleFrameTransform( AVFrame* pRgb, uint8_t* buf, void* user, structTimeSteps perf, int srcX, int srcY );
  void HandleEventImage( EventImage& );
  
  void HandleLoadVideo( wxCommandEvent& event );  // need to recode (this is where it actually starts)
  void LoadVideo( void );
  
};

TreeItemVideo::TreeItemVideo( 
  TreeDisplayManager* pTree, wxTreeItemId id, pPhysicalDisplay_t pPhysicalDisplay, pSceneManager_t pSceneManager )
: TreeItemImageCommon( pTree, id, pPhysicalDisplay, pSceneManager )
{
  
  std::cout << "Tree Item Add Video" << std::endl;
  
  wxImage::AddHandler( new wxJPEGHandler );

  m_sPictureDirectory = wxT( "~/Pictures/");
  m_sVideoDirectory = wxT( "~/Videos/");

    // workers for the movie action
  m_pWork = new boost::asio::io_service::work(m_Srvc);  // keep the asio service running 
  //m_thrdWorkers = boost::thread( boost::phoenix::bind( &AppProjection::Workers, this ) );
  for ( std::size_t ix = 0; ix < 2; ix++ ) {
    m_threadsWorkers.create_thread( boost::phoenix::bind( &boost::asio::io_service::run, &m_Srvc ) );
  }

  // **** will need to set a specific instance id so that multiple frames can be run
  wxApp::GetInstance()->Bind( EVENT_IMAGE, &TreeItemVideo::HandleEventImage, this );
  
  ResetTransformMatrix();
  
  LoadVideo();
  
  UpdateTransformMatrix();  // probably won't have coordinates at this time, so may not be necessary
  
}

TreeItemVideo::~TreeItemVideo( void ) {
  delete m_pWork;
  m_pWork = 0;
  m_threadsWorkers.join_all();
}

void TreeItemVideo::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();

  pMenu->Append( MISelectVideo, "Load Video" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVideo::HandleLoadVideo, this, MISelectVideo );

  pMenu->Append( MIReset, "Reset" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleReset, this, MIReset );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemVisualCommon::HandleDelete, this, MIDelete );
  
  m_pTree->PopupMenu( pMenu );
}

void TreeItemVideo::Workers( void ) {
  m_Srvc.run(); 
}

void TreeItemVideo::HandleOnFrame( AVCodecContext* context, AVFrame* frame, AVPacket* pkt, void* user, structTimeSteps perf ) {
  
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

  m_Srvc.post( boost::phoenix::bind( &TreeItemVideo::HandleFrameTransform, this, pRGB, buf, user, perf, srcX, srcY ) );
  
  // ** note decode currently works faster than the transform, so transform work will queue up.
  //    need to deal with proper timing of frames.
  //    need to base frame timing on what is in the file
  //    need sync so stops at high water mark, resumes decode at low water mark
  //    be aware that if using multiple threads, that processing needs to be sync'd so frames stay in order
  //      or do frame ordering in final presentation to screen, buffer loop
}

void TreeItemVideo::HandleFrameTransform( AVFrame* pRgb, uint8_t* buf, void* user, structTimeSteps perf, int srcX, int srcY ) {
  
  perf.queue1 = boost::chrono::high_resolution_clock::now();
  
  uint8_t* pSrcFrame( *pRgb->data );
  
  boost::shared_ptr<wxImage> pImage( new wxImage( srcX, srcY, false ) );
  wxImage* image( pImage.get() );
  wxImagePixelData data( *image );
  wxImagePixelData::Iterator pDestImage( data );
  
  for ( int iy = 0; iy < srcY; ++iy ) {
    for ( int ix = 0; ix < srcX; ++ix ) {
      //++pSrc;  // skip A
      pDestImage.Blue() =  *pSrcFrame; ++pSrcFrame;
      pDestImage.Green() = *pSrcFrame; ++pSrcFrame;
      pDestImage.Red() =   *pSrcFrame; ++pSrcFrame;
      ++pDestImage;
      ++pSrcFrame; // skip alpha?
    }
  }

  perf.copied = boost::chrono::high_resolution_clock::now();
  
  // may need to set a unique itemid if multiple events running simultaneously
  // need to change this to the specific canvas into which the frame is going to be displayed
  wxApp::GetInstance()->QueueEvent( new EventImage( EVENT_IMAGE, m_pPhysicalDisplay->GetFrame()->GetId(), pImage, user, perf ) );
  //m_pScreenFrame->GetFrame()->QueueEvent( new EventImage( EVENT_IMAGE, -1, pImage, user, perf ) );
  //m_pScreenFrame->GetFrame()->get QueueEvent( new EventImage( EVENT_IMAGE, -1, pImage, user, perf ) );
  
  av_free( buf );
  av_free( pRgb );  
  
}

void TreeItemVideo::HandleEventImage( EventImage& event ) {

  typedef boost::chrono::milliseconds ms;
  typedef boost::chrono::microseconds mu;
  static boost::chrono::duration<int64_t, boost::milli> onesec( 1000 );
  
  structTimeSteps ts( event.GetTimeSteps() );
  
  ts.queue2 = boost::chrono::high_resolution_clock::now();
  
  // this is where would instead send to OpenGL buffers and draw
  TreeItemImageCommon::SetImage( event.GetImage() );
  
//  wxBitmap bitmap( *event.GetImage() );
//  FrameProjection* pfp = (FrameProjection*) event.GetVoid();
//  wxClientDC dc( pfp );
//  dc.DrawBitmap( bitmap, wxPoint( 10, 10 ) );
  
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

void TreeItemVideo::ProcessVideoFile( boost::shared_ptr<DecodeH264> pDecoder ) { // is in background  thread
  pDecoder->ProcessFile();
}

void TreeItemVideo::HandleLoadVideo( wxCommandEvent& event ) {
  // may want to block this while one is currently playing
  // or kill current in-process one and replace with new one
  LoadVideo();  
}

void TreeItemVideo::LoadVideo( void ) {
  
  std::cout << "LoadPicture" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_pPhysicalDisplay->GetFrame(), wxT("Select Video" ), m_sVideoDirectory, "", 
    //"Video Files (*.ts)|*.ts", 
    "Video Files (*.h264)|*.h264", 
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sVideoDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    std::cout << "dir " << m_sVideoDirectory << std::endl;
    //assert( m_image.LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );

    boost::shared_ptr<DecodeH264> pDecoder( new DecodeH264 ( m_pPhysicalDisplay->GetFrame() ) );
    
    namespace args = boost::phoenix::arg_names;
    pDecoder->m_OnFrame.connect( 
      boost::phoenix::bind( &TreeItemVideo::HandleOnFrame, this, args::arg1, args::arg2, args::arg3, args::arg4, args::arg5 ) );

    pDecoder->load( dialogOpenFile.GetPath() );
    // ProcessFile to be handled in thread

    m_Srvc.post( boost::phoenix::bind( &TreeItemVideo::ProcessVideoFile, this, pDecoder ) );
    
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

// ================

class TreeItemPlaceHolder: public TreeItemBase {
public:
  
  typedef boost::shared_ptr<PhysicalDisplay> pPhysicalDisplay_t;
  typedef Outline::pOutline_t pOutline_t;
  typedef boost::shared_ptr<SceneManager> pSceneManager_t;
  
  TreeItemPlaceHolder( TreeDisplayManager* pTree_, wxTreeItemId id_, 
              pPhysicalDisplay_t pScreenFrame, pOutline_t pOutline, pSceneManager_t pSceneManager );
  ~TreeItemPlaceHolder( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddPicture, MIAddVideo, MIAddGrid, MIDelete
  };
  
  typedef SceneManager::key_t key_t;
  typedef SceneManager::pSceneElement_t pSceneElement_t;
  typedef boost::signals2::connection connection;
  
  // note:  no way to delete this yet, may need a signal for it
  struct SceneElementInfo {
    //key_t m_key;
    //pSceneElement_t m_pSceneElement;
    connection m_connectTransformSupplier;
    glm::mat4 m_mat4Transform;  // The transformation matrix built by Grid
    SceneElementInfo( void ): m_mat4Transform( 1.0f ) {};
    //structSceneElement( key_t key, pSceneElement_t pSceneElement, connection connectGrid, glm::mat4 mat4Transform )
    //  : m_key( key ), m_pSceneElement( pSceneElement ), m_connectGrid( connectGrid ), m_mat4Transform( mat4Transform ) {}
    SceneElementInfo( connection connectTransformSupplier, glm::mat4 mat4Transform )
      : m_connectTransformSupplier( connectTransformSupplier ), m_mat4Transform( mat4Transform ) {}
    ~SceneElementInfo( void ) {
      m_connectTransformSupplier.disconnect();
    }
    void HandleUpdateTransform( const glm::mat4& matrix ) {
      m_mat4Transform = matrix;
    }
  };
  
  typedef boost::shared_ptr<SceneElementInfo> pSceneElementInfo_t;
  typedef std::map<wxTreeItemId,pSceneElementInfo_t> mapSceneElementInfo_t;
  
  bool m_bHasGrid;  // need a signal from grid when to clear, should only have one grid assigned
  // but allow multiple for now, but each is going to overwrite the supplied outline
  // unless the outline is read only, and used only as a starting point
  // will allow multiple objects on the canvas, so may not need m_bHasGrid
  
  pPhysicalDisplay_t m_pPhysicalDisplay;
  pOutline_t m_pOutline;
  pSceneManager_t m_pSceneManager;
  
  mapSceneElementInfo_t m_mapSceneElementInfo;
  
  void SetSelected( void );
  void RemoveSelected( void );
  void DeletingChild( wxTreeItemId );
  
  void HandleAddPicture( wxCommandEvent& event );
  void HandleAddVideo( wxCommandEvent& event );
  void HandleAddGrid( wxCommandEvent& event );
  void HandleDelete( wxCommandEvent& event );
  
};

TreeItemPlaceHolder::TreeItemPlaceHolder( 
  TreeDisplayManager* pTree_, wxTreeItemId id_, pPhysicalDisplay_t pPhysicalDisplay, pOutline_t pOutline, pSceneManager_t pSceneManager )
: TreeItemBase( pTree_, id_ ), m_pPhysicalDisplay( pPhysicalDisplay ), 
  m_pOutline( pOutline ), m_pSceneManager( pSceneManager ), m_bHasGrid( false ) {
  //m_mat4Transform = glm::mat4( 1.0f );  // identity matrix to start
}

TreeItemPlaceHolder::~TreeItemPlaceHolder( void ) {
  RemoveSelected();
}

void TreeItemPlaceHolder::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIAddGrid, "Add &Grid" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPlaceHolder::HandleAddGrid, this, MIAddGrid );
  
  pMenu->Append( MIAddPicture, "Add &Image" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPlaceHolder::HandleAddPicture, this, MIAddPicture );
  
  pMenu->Append( MIAddVideo, "Add &Video" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPlaceHolder::HandleAddVideo, this, MIAddVideo );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPlaceHolder::HandleDelete, this, MIDelete );
  
  m_pTree->PopupMenu( pMenu );
}

void TreeItemPlaceHolder::SetSelected( void ) {
  //std::cout << "Tree Item Canvas Selected" << std::endl;
  m_pPhysicalDisplay->GetFrame()->SetOutline( m_pOutline );
  m_pPhysicalDisplay->GetFrame()->Refresh();
}

void TreeItemPlaceHolder::RemoveSelected( void ) {
  pOutline_t pOutline;
  m_pPhysicalDisplay->GetFrame()->SetOutline( pOutline );
  m_pPhysicalDisplay->GetFrame()->Refresh();
}

void TreeItemPlaceHolder::DeletingChild( wxTreeItemId id ) {
  mapSceneElementInfo_t::iterator iter = m_mapSceneElementInfo.find( id );
  assert( m_mapSceneElementInfo.end() != iter );
  iter->second->m_connectTransformSupplier.disconnect();
  m_mapSceneElementInfo.erase( iter );
}

void TreeItemPlaceHolder::HandleAddGrid( wxCommandEvent& event ) {
  
  wxTreeItemId id = m_pTree->AppendItem( m_id, "Grid" );
  m_pTree->EnsureVisible( id );
  
  TreeItemGrid* pGrid = new TreeItemGrid( m_pTree, id, m_pPhysicalDisplay, m_pSceneManager );
  pTreeItem_t pTreeItem( pGrid );
  m_pTree->Add( id, pTreeItem );
  
  pSceneElementInfo_t pInfo( new SceneElementInfo );
  m_mapSceneElementInfo.insert( mapSceneElementInfo_t::value_type( id, pInfo ) );
  
  namespace args = boost::phoenix::arg_names;
  pInfo->m_connectTransformSupplier = pGrid->Connect( boost::phoenix::bind( &SceneElementInfo::HandleUpdateTransform, pInfo.get(), args::arg1 ) );
  pGrid->GetTransformMatrix( pInfo->m_mat4Transform );
  
}

void TreeItemPlaceHolder::HandleAddPicture( wxCommandEvent& event ) {
  
  wxTreeItemId id = m_pTree->AppendItem( m_id, "Image" );
  m_pTree->EnsureVisible( id );
  
  TreeItemImage* pImage = new TreeItemImage( m_pTree, id, m_pPhysicalDisplay, m_pSceneManager );
  pTreeItem_t pTreeItem( pImage );
  m_pTree->Add( id, pTreeItem );
  
  pSceneElementInfo_t pInfo( new SceneElementInfo );
  m_mapSceneElementInfo.insert( mapSceneElementInfo_t::value_type( id, pInfo ) );
  
  namespace args = boost::phoenix::arg_names;
  pInfo->m_connectTransformSupplier = pImage->Connect( boost::phoenix::bind( &SceneElementInfo::HandleUpdateTransform, pInfo.get(), args::arg1 ) );
  pImage->GetTransformMatrix( pInfo->m_mat4Transform );
  
}

void TreeItemPlaceHolder::HandleAddVideo( wxCommandEvent& event ) {
  
  wxTreeItemId id = m_pTree->AppendItem( m_id, "Video" );
  m_pTree->EnsureVisible( id );
  
  TreeItemVideo* pVideo = new TreeItemVideo( m_pTree, id, m_pPhysicalDisplay, m_pSceneManager );
  pTreeItem_t pTreeItem( pVideo );
  m_pTree->Add( id, pTreeItem );
  
  pSceneElementInfo_t pInfo( new SceneElementInfo );
  m_mapSceneElementInfo.insert( mapSceneElementInfo_t::value_type( id, pInfo ) );
  
  namespace args = boost::phoenix::arg_names;
  pInfo->m_connectTransformSupplier = pVideo->Connect( boost::phoenix::bind( &SceneElementInfo::HandleUpdateTransform, pInfo.get(), args::arg1 ) );
  pVideo->GetTransformMatrix( pInfo->m_mat4Transform );
  
}

void TreeItemPlaceHolder::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Tree Item Delete" << std::endl;
  m_pTree->Delete( this->m_id );
}


// ================

class TreeItemPhysicalDisplay: public TreeItemBase {
public:
  
  typedef boost::shared_ptr<PhysicalDisplay> pPhysicalDisplay_t;
  
  TreeItemPhysicalDisplay( TreeDisplayManager* pTree_, wxTreeItemId id_, pPhysicalDisplay_t pScreenFrame );
  ~TreeItemPhysicalDisplay( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddPlaceHolder,
    MIAddOutline, 
//    MISelectVideo
  };
  
  typedef Outline::pOutline_t pOutline_t;
  typedef boost::shared_ptr<SceneManager> pSceneManager_t;
  
  pSceneManager_t m_pSceneManager;
  
  pPhysicalDisplay_t m_pPhysicalDisplay;
  
  void HandleAddOutline( wxCommandEvent& event );
  void HandleAddPlaceHolder( wxCommandEvent& event );
  
  void HandleLoadVideo( wxCommandEvent& event );
  
};

TreeItemPhysicalDisplay::TreeItemPhysicalDisplay( TreeDisplayManager* pTree_, wxTreeItemId id_, pPhysicalDisplay_t pPhysicalDisplay )
: TreeItemBase( pTree_, id_ ), m_pPhysicalDisplay( pPhysicalDisplay ) {
  
  // add a right click pop up to add displayable objects and surfaces
  // which are then serialized for session persistence
  // use text or enum keys to register objects, for subsequent re-creation

  int argsCanvas[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };  // WX_GL_CORE_PROFILE deprecated I think
  m_pSceneManager.reset( new SceneManager( m_pPhysicalDisplay->GetFrame(), argsCanvas ) );
  wxRect rect( 10, 10, 10, 10 );
  rect = m_pPhysicalDisplay->GetFrame()->GetClientRect();
  m_pSceneManager->SetSize( rect.GetSize() );
  m_pSceneManager->Move( rect.GetTopLeft() );
}

TreeItemPhysicalDisplay::~TreeItemPhysicalDisplay( void ) {
}

void TreeItemPhysicalDisplay::HandleAddOutline(  wxCommandEvent& event  ) {  // for remote displays, will use wizard dialog
  std::cout << "Add Outline" << std::endl;  
  pOutline_t m_pOutline( new Outline( wxRect( 300, 300, 600, 600 ) ) );
  m_pPhysicalDisplay->GetFrame()->SetOutline( m_pOutline );
  m_pPhysicalDisplay->GetFrame()->Refresh();
}

void TreeItemPhysicalDisplay::HandleAddPlaceHolder( wxCommandEvent& event ) {
  std::cout << "Add PlaceHolder" << std::endl;  
  // various stages:  
  //   0) popup to get description
  //   1) tree item added - done
  //   2) outline added - done
  //   3) outline changeable - done
  //   4) handle selection event to turn outline back on for resizing events - done
  //   5) add menu items to add pictures or movies - done 
  //   6) create the canvas? - done
  //   7) handle events from outline to adjust canvas
  
  wxTreeItemId id = m_pTree->AppendItem( m_id, "PlaceHolder" );
  m_pTree->EnsureVisible( id );
  
  pOutline_t pOutline( new Outline( wxRect( 300, 300, 600, 600 ), true, false ) );  // instead, use some ratio of the main window

  pTreeItem_t pTreeItem( new TreeItemPlaceHolder( m_pTree, id, m_pPhysicalDisplay, pOutline, m_pSceneManager ) );
  m_pTree->Add( id, pTreeItem );
}

void TreeItemPhysicalDisplay::ShowContextMenu( void ) {
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIAddPlaceHolder, "Add PlaceHolder" ); // need to have picture or video ready with dimensions to scale properly
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPhysicalDisplay::HandleAddPlaceHolder, this, MIAddPlaceHolder );
  
  pMenu->AppendSeparator();

  pMenu->Append( MIAddOutline, "&Add Outline" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemPhysicalDisplay::HandleAddOutline, this, MIAddOutline );

  m_pTree->PopupMenu( pMenu );
}

// ====================================
    
TreeDisplayManager::TreeDisplayManager() {
  Init();
}

TreeDisplayManager::TreeDisplayManager( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

bool TreeDisplayManager::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxTreeCtrl::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

TreeDisplayManager::~TreeDisplayManager() {
}

void TreeDisplayManager::Append( pPhysicalDisplay_t pPhysicalDisplay ) {
  wxTreeItemId idRoot = wxTreeCtrl::GetRootItem();
  std::string sId = boost::lexical_cast<std::string>( pPhysicalDisplay->GetId() );
  wxTreeItemId id = wxTreeCtrl::AppendItem( idRoot, "Frame " + sId );
  EnsureVisible( id );
  
  pTreeItem_t pTreeItem( new TreeItemPhysicalDisplay( this, id, pPhysicalDisplay ) );
  Add( id, pTreeItem );
  //m_mapDecoder.insert( mapDecoder_pair_t( id.GetID(), pTreeItem ) );
}

void TreeDisplayManager::Add( const wxTreeItemId& id, pTreeItem_t pTreeItem ) {
  m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), pTreeItem ) );
}

void TreeDisplayManager::Delete( wxTreeItemId id ) {
  //wxTreeItemId id( pTreeItem->GetTreeItemId() );
  if ( 0 == GetChildrenCount( id ) ) {
    
    wxTreeItemId idParent = wxTreeCtrl::GetItemParent( id );
    assert( idParent.IsOk() );
    mapDecoder_t::iterator iterParent = m_mapDecoder.find( idParent.GetID() );
    assert( m_mapDecoder.end() != iterParent );
    iterParent->second->DeletingChild( id );
    
    mapDecoder_t::iterator iterChild = m_mapDecoder.find( id.GetID() );
    assert( m_mapDecoder.end() != iterChild );
    
    wxTreeCtrl::Delete( id );
    m_idOld.Unset();
    m_mapDecoder.erase( iterChild );
  }
  else {
    std::cout << "item has children" << std::endl;
  }
}

void TreeDisplayManager::Init() {
}

void TreeDisplayManager::CreateControls() {    
  
  TreeDisplayManager* item = this;
  
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_MENU, &TreeDisplayManager::HandleContextMenu, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGED, &TreeDisplayManager::HandleSelectionChanged, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGING, &TreeDisplayManager::HandleSelectionChanging, this );
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_ACTIVATED, &TreeDisplayManager::HandleItemActivated, this );
  wxTreeCtrl::Bind( wxEVT_TREE_DELETE_ITEM, &TreeDisplayManager::HandleItemDeleted, this );
  
  
  wxTreeItemId id = wxTreeCtrl::AddRoot( "Projections" );
  pTreeItem_t pTreeItem( new TreeItemRoot( this, id ) );
  m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), pTreeItem ) );
  
}

void TreeDisplayManager::HandleContextMenu( wxTreeEvent& event ) {
  m_mapDecoder[ event.GetItem().GetID() ]->ShowContextMenu();
}

void TreeDisplayManager::HandleSelectionChanged( wxTreeEvent& event ) {
  std::cout << "HandleSelectionChanged " << event.GetItem().GetID() << std::endl;
  m_idOld = event.GetItem();
  m_mapDecoder[ m_idOld.GetID() ]->SetSelected();
  
}

void TreeDisplayManager::HandleSelectionChanging( wxTreeEvent& event ) {
  //std::cout << "HandleSelectionChanging " << event.GetItem().GetID() << std::endl;
  if ( m_idOld.IsOk() ) m_mapDecoder[ m_idOld ]->RemoveSelected();
  m_idOld.Unset();
}

void TreeDisplayManager::HandleItemActivated( wxTreeEvent& event ) {
  std::cout << "HandleItemActivated" << std::endl;
}

void TreeDisplayManager::HandleItemDeleted( wxTreeEvent& event ) {
  std::cout << "HandleItemDeleted" << std::endl;
}

wxBitmap TreeDisplayManager::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon TreeDisplayManager::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}
