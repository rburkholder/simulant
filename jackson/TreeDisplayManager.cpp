/* 
 * File:   TreeDisplayManager.cpp
 * Author: rpb
 * 
 * Created on April 5, 2015, 10:11 PM
 */

//#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/core/argument.hpp>
#include <boost/shared_ptr.hpp>

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

#include "common.h"
#include "DecodeH264.h"

//#include "tut1.h"
#include "tex2.h"
#include "OglGrid.h"

#include "eventImage.h"

#include "Outline.h"

#include "TreeDisplayManager.h"

IMPLEMENT_DYNAMIC_CLASS( TreeDisplayManager, wxTreeCtrl )

class TreeItemBase {
public:
  
  typedef boost::shared_ptr<TreeItemBase> pTreeItem_t;
  
  TreeItemBase( TreeDisplayManager* pTree_, wxTreeItemId id_ ): m_pTree( pTree_ ), m_id( id_ ) {}
  virtual ~TreeItemBase( void ) {}
  virtual void ShowContextMenu( void ) {}
  virtual void SetSelected( void ) {}
  virtual void RemoveSelected( void ) {}
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

class TreeItemCanvasGrid: public TreeItemBase {
  // so can add/delete the grid object, and possibly handle the mouse stuff for setting up the transform
public:
  
  typedef boost::shared_ptr<ScreenFrame> pScreenFrame_t;
  typedef Outline::pOutline_t pOutline_t;
  
  TreeItemCanvasGrid( TreeDisplayManager* pTree_, wxTreeItemId id_, pScreenFrame_t pScreenFrame, pOutline_t pOutline );
  ~TreeItemCanvasGrid( void );
  
  virtual void ShowContextMenu( void );
  
  // need to return transformation matrix managed by the Grid
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIDelete
  };
  
  typedef boost::shared_ptr<OglGrid> pOglGrid_t;
  
  float m_floatFactor;
  
  pScreenFrame_t m_pScreenFrame;
  pOutline_t m_pOutline;
  pOglGrid_t m_pOglGrid;
  
  glm::mat4 m_mat4Transform;
  
  void ResetTransformMatrix( void );
  
  void HandleDelete( wxCommandEvent& event );
  void HandleMouseWheel( wxMouseEvent& event );
  void HandleMouseMoved( wxMouseEvent& event );
};

TreeItemCanvasGrid::TreeItemCanvasGrid( TreeDisplayManager* pTree_, wxTreeItemId id_, pScreenFrame_t pScreenFrame, pOutline_t pOutline )
: TreeItemBase( pTree_, id_ ), m_pScreenFrame( pScreenFrame ), m_pOutline( pOutline ), m_floatFactor( 1.0f ) {
  
  std::cout << "Tree Item Show Grid" << std::endl;
  
  ResetTransformMatrix();
  
  int argsCanvas[] = { WX_GL_CORE_PROFILE, WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
  m_pOglGrid.reset( new OglGrid( m_pScreenFrame->GetFrame(), argsCanvas ) );
  wxRect rect( 500, 100, 300, 600 );
//  pOutline_t pOutline( m_pScreenFrame->GetFrame()->GetOutline() );
  if ( 0 != pOutline.use_count() ) {
    rect = pOutline->GetBoundingBox();
  }
  m_pOglGrid->SetSize( rect.GetSize() );
  m_pOglGrid->Move( rect.GetTopLeft() );
  
  m_pOglGrid->Bind( wxEVT_MOUSEWHEEL, &TreeItemCanvasGrid::HandleMouseWheel, this );
  m_pOglGrid->Bind( wxEVT_MOTION, &TreeItemCanvasGrid::HandleMouseMoved, this );
  
}

TreeItemCanvasGrid::~TreeItemCanvasGrid( void ) {
}

void TreeItemCanvasGrid::ResetTransformMatrix( void ) {
  m_mat4Transform = glm::mat4( 1.0f );
}

void TreeItemCanvasGrid::HandleMouseWheel( wxMouseEvent& event ) {
  std::cout << "mouse wheel " << event.GetWheelDelta() << ", " << event.GetWheelRotation() << std::endl;
  static const float scaleMajor( 0.10f );
  static const float scaleMinor( 0.01f );
  float factor( 1.0f );
  int n( event.GetWheelRotation() );
  if ( 0 != n ) {
    glm::vec3 vScale;
    if ( 0 < n ) { // positive
      // zoom in, and update transform matrix
      factor = 1.0f + scaleMajor;
      vScale = glm::vec3( factor, factor, 0.0f );
    }
    else { // negative
      // zoom out, and update transform matrix
      factor = 1.0f / ( 1.0f + scaleMajor );
      vScale = glm::vec3( factor, factor, 0.0f );
    }
    m_floatFactor *= factor;
    std::cout << "factor: " << m_floatFactor << std::endl;
    m_mat4Transform *= glm::scale( vScale );
    m_pOglGrid->UpdateTransform( m_mat4Transform );
  }
  event.Skip();
}

void TreeItemCanvasGrid::HandleMouseMoved( wxMouseEvent& event ) {
  event.Skip();
}

//  glm::vec3 vScaleTo2( 2.0f / nCols, 2.0f / nRows, 0.0f );
//  glm::vec3 vTranslateAround0( -1.0f, -1.0f, 0.0f );
//  glm::vec3 vScaleDownABit( 0.99f, 0.99f, 0.0f );

//  m_mat4Transform = glm::mat4( 1.0f ); // identity matrix
//  m_mat4Transform *= glm::scale( vScaleDownABit ); // gets all four sides into the window
//  m_mat4Transform *= glm::translate( vTranslateAround0 ); // then translate to straddle zero
//  m_mat4Transform *= glm::scale( vScaleTo2 );  // first scale to 0..2

void TreeItemCanvasGrid::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemCanvasGrid::HandleDelete, this, MIDelete );
  
  m_pTree->PopupMenu( pMenu );
}

void TreeItemCanvasGrid::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Tree Item Delete" << std::endl;
  m_pTree->Delete( this->m_id );
  // need a refresh
}

// ================

class TreeItemCanvas: public TreeItemBase {
  // may need to change name to only a place holder as it is no longer a canvas,
  // but a placeholder for an outline for creating displayable objects
public:
  
  typedef boost::shared_ptr<ScreenFrame> pScreenFrame_t;
  typedef Outline::pOutline_t pOutline_t;
  
  TreeItemCanvas( TreeDisplayManager* pTree_, wxTreeItemId id_, pScreenFrame_t pScreenFrame, pOutline_t pOutline );
  ~TreeItemCanvas( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddPicture, MIAddVideo, MIShowGrid, MIDelete
  };
  
  //typedef boost::shared_ptr<OglGrid> pOglGrid_t;
  
  bool m_bHasGrid;  // need a signal from grid when to clear, should only have one grid assigned
  // but allow multiple for now, but each is going to overwrite the supplied outline
  // unless the outline is read only, and used only as a starting point
  // will allow multiple objects on the canvas, so may not need m_bHasGrid
  
  pScreenFrame_t m_pScreenFrame;
  pOutline_t m_pOutline;
  
  void SetSelected( void );
  void RemoveSelected( void );
  
  void HandleAddPicture( wxCommandEvent& event );
  void HandleAddVideo( wxCommandEvent& event );
  void HandleShowGrid( wxCommandEvent& event );
  void HandleDelete( wxCommandEvent& event );
  
};

TreeItemCanvas::TreeItemCanvas( TreeDisplayManager* pTree_, wxTreeItemId id_, pScreenFrame_t pScreenFrame, pOutline_t pOutline )
: TreeItemBase( pTree_, id_ ), m_pScreenFrame( pScreenFrame ), m_pOutline( pOutline ), m_bHasGrid( false ) {
}

TreeItemCanvas::~TreeItemCanvas( void ) {
}

void TreeItemCanvas::ShowContextMenu( void ) {
  
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIShowGrid, "Show Grid" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemCanvas::HandleShowGrid, this, MIShowGrid );
  
  pMenu->Append( MIAddPicture, "Add &Picture" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemCanvas::HandleAddPicture, this, MIAddPicture );
  
  pMenu->Append( MIAddVideo, "Add &Video" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemCanvas::HandleAddVideo, this, MIAddVideo );
  
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemCanvas::HandleDelete, this, MIDelete );
  
  m_pTree->PopupMenu( pMenu );
}

void TreeItemCanvas::SetSelected( void ) {
  //std::cout << "Tree Item Canvas Selected" << std::endl;
  m_pScreenFrame->GetFrame()->SetOutline( m_pOutline );
  m_pScreenFrame->GetFrame()->Refresh();
}

void TreeItemCanvas::RemoveSelected( void ) {
  pOutline_t pOutline;
  m_pScreenFrame->GetFrame()->SetOutline( pOutline );
  m_pScreenFrame->GetFrame()->Refresh();
}

void TreeItemCanvas::HandleShowGrid( wxCommandEvent& event ) {
  wxTreeItemId id = m_pTree->AppendItem( m_id, "Grid" );
  m_pTree->EnsureVisible( id );
  
  pTreeItem_t pTreeItem( new TreeItemCanvasGrid( m_pTree, id, m_pScreenFrame, m_pOutline ) );
  m_pTree->Add( id, pTreeItem );
}

void TreeItemCanvas::HandleAddPicture( wxCommandEvent& event ) {
  std::cout << "Tree Item Add Picture" << std::endl;
}

void TreeItemCanvas::HandleAddVideo( wxCommandEvent& event ) {
  std::cout << "Tree Item Add Video" << std::endl;
}

void TreeItemCanvas::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Tree Item Delete" << std::endl;
  m_pTree->Delete( this->m_id );
}

// ================

class TreeItemSurfaceWithPicture: public TreeItemBase {
public:
  
  TreeItemSurfaceWithPicture( TreeDisplayManager* pTree_, wxTreeItemId id_ );
  ~TreeItemSurfaceWithPicture( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
};

// ================

class TreeItemScreenFrame: public TreeItemBase {
public:
  
  typedef boost::shared_ptr<ScreenFrame> pScreenFrame_t;
  
  TreeItemScreenFrame( TreeDisplayManager* pTree_, wxTreeItemId id_, pScreenFrame_t pScreenFrame );
  ~TreeItemScreenFrame( void );
  
  virtual void ShowContextMenu( void );
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddOpenGLCanvas, 
    MIAddOutline, MISelectPicture, MISelectVideo, MIImageToOpenGL, MIMovieScreen
  };
  
  typedef Outline::pOutline_t pOutline_t;
  
  wxString m_sPictureDirectory;
  wxString m_sVideoDirectory;
  
  wxImage m_image;
  
  boost::thread_group m_threadsWorkers;
  boost::asio::io_service m_Srvc;
  boost::asio::io_service::work* m_pWork;
  
  pScreenFrame_t m_pScreenFrame;
  
  //tut1* m_pTut1;
  tex2* m_pTex;
  
  void Workers( void );
  
  void Image2OpenGL( void );
  void ProcessVideoFile( boost::shared_ptr<DecodeH264> pDecoder );
  
  void HandleAddOutline( wxCommandEvent& event );
  void HandleAddCanvas( wxCommandEvent& event );
  void HandleOnFrame( AVCodecContext* context, AVFrame* frame, AVPacket* pkt, void* user, structTimeSteps perf );
  void HandleFrameTransform( AVFrame* pRgb, uint8_t* buf, void* user, structTimeSteps perf, int srcX, int srcY );
  void HandleEventImage( EventImage& );
  
  void HandleLoadPicture( wxCommandEvent& event );
  void HandleLoadVideo( wxCommandEvent& event );
  void HandleImage2OpenGL( wxCommandEvent& event ) { Image2OpenGL(); }
  void HandleCreateMovieScreen(  wxCommandEvent& event ) {};
  
};

TreeItemScreenFrame::TreeItemScreenFrame( TreeDisplayManager* pTree_, wxTreeItemId id_, pScreenFrame_t pScreenFrame )
: TreeItemBase( pTree_, id_ ), m_pScreenFrame( pScreenFrame ) {
  // add a right click pop up to add displayable objects and surfaces
  // which are then serialized for session persistence
  // use text or enum keys to register objects, for subsequent re-creation

  //m_pTut1 = 0;  
  m_pTex = 0;

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
  wxApp::GetInstance()->Bind( EVENT_IMAGE, &TreeItemScreenFrame::HandleEventImage, this );
}

TreeItemScreenFrame::~TreeItemScreenFrame( void ) {
  delete m_pWork;
  m_pWork = 0;
  m_threadsWorkers.join_all();
}

void TreeItemScreenFrame::HandleAddOutline(  wxCommandEvent& event  ) {  // for remote displays, will use wizard dialog
  std::cout << "Add Outline" << std::endl;  
  pOutline_t m_pOutline( new Outline( wxRect( 300, 300, 600, 600 ) ) );
  m_pScreenFrame->GetFrame()->SetOutline( m_pOutline );
  m_pScreenFrame->GetFrame()->Refresh();
}

void TreeItemScreenFrame::HandleAddCanvas( wxCommandEvent& event ) {
  std::cout << "Add Canvas" << std::endl;  
  // various stages:  
  //   0) popup to get description
  //   1) tree item added - done
  //   2) outline added - done
  //   3) outline changeable - done
  //   4) handle selection event to turn outline back on for resizing events - done
  //   5) add menu items to add pictures or movies - done 
  //   6) create the canvas? - done
  //   7) handle events from outline to adjust canvas
  
  wxTreeItemId id = m_pTree->AppendItem( m_id, "Canvas" );
  m_pTree->EnsureVisible( id );
  
  pOutline_t pOutline( new Outline( wxRect( 300, 300, 600, 600 ), true, false ) );  // instead, use some ratio of the main window

  pTreeItem_t pTreeItem( new TreeItemCanvas( m_pTree, id, m_pScreenFrame, pOutline ) );
  m_pTree->Add( id, pTreeItem );
}
  
void TreeItemScreenFrame::ShowContextMenu( void ) {
  wxMenu* pMenu = new wxMenu();
  
  pMenu->Append( MIAddOpenGLCanvas, "Add Canvas" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScreenFrame::HandleAddCanvas, this, MIAddOpenGLCanvas );
  
  pMenu->AppendSeparator();

  pMenu->Append( MIAddOutline, "&Add Outline" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScreenFrame::HandleAddOutline, this, MIAddOutline );

  pMenu->Append( MISelectPicture, "wx Picture" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScreenFrame::HandleLoadPicture, this, MISelectPicture );

  pMenu->Append( MISelectVideo, "wx Video" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScreenFrame::HandleLoadVideo, this, MISelectVideo );

  pMenu->Append( MIImageToOpenGL, "Image->OpenGL" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScreenFrame::HandleImage2OpenGL, this, MIImageToOpenGL );

  pMenu->Append( MIMovieScreen, "Movie Screen" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemScreenFrame::HandleCreateMovieScreen, this, MIMovieScreen );

  m_pTree->PopupMenu( pMenu );
}
  
void TreeItemScreenFrame::HandleLoadPicture( wxCommandEvent& event ) {
  std::cout << "LoadPicture" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_pScreenFrame->GetFrame(), wxT("Select Image" ), m_sPictureDirectory, "", 
    //"JPG Files (*.jpg)|*.jpg", 
    _("Image Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sPictureDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    std::cout << "dir " << m_sPictureDirectory << std::endl;
    assert( m_image.LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );
    wxBitmap bitmap( m_image );
    FrameProjection* pfp = m_pScreenFrame->GetFrame();
    wxClientDC dc( pfp );
    dc.DrawBitmap( bitmap, wxPoint( 0, 0 ) );
    Image2OpenGL();
  }
  else {
  }
}

void TreeItemScreenFrame::Image2OpenGL( void ) {
  
  if ( m_image.IsOk() ) {
    //std::cout << "is ok" << std::endl;
    if ( 0 != m_pTex ) {
      delete m_pTex;
      m_pTex = 0;
    }
    if ( 0 == m_pTex ) {
      int argsCanvas[] = { WX_GL_CORE_PROFILE, WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };
      m_pTex = new tex2( m_pScreenFrame->GetFrame(), argsCanvas );
      //m_pTex->SetSize( m_image.GetWidth(), m_image.GetHeight() );
      wxRect rect( 500, 100, 300, 600 );
      pOutline_t pOutline( m_pScreenFrame->GetFrame()->GetOutline() );
      if ( 0 != pOutline.use_count() ) {
        rect = pOutline->GetBoundingBox();
      }
      m_pTex->SetSize( rect.GetSize() );
      m_pTex->Move( rect.GetTopLeft() );
      if ( 0 != pOutline.use_count() ) {
        Outline::vPoints_t vPoints;
        pOutline->GetCoords( vPoints );
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

void TreeItemScreenFrame::Workers( void ) {
  m_Srvc.run(); 
}

//int nFrame=0;
void TreeItemScreenFrame::HandleOnFrame( AVCodecContext* context, AVFrame* frame, AVPacket* pkt, void* user, structTimeSteps perf ) {
  
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
  m_Srvc.post( boost::phoenix::bind( &TreeItemScreenFrame::HandleFrameTransform, this, pRGB, buf, user, perf, srcX, srcY ) );
  
  // ** note decode currently works faster than the transform, so transform work will queue up.
  //    need to deal with proper timing of frames.
  //    need to base frame timing on what is in the file
  //    need sync so stops at high water mark, resumes decode at low water mark
  //    be aware that if using multiple threads, that processing needs to be sync'd so frames stay in order
  //      or do frame ordering in final presentation to screen, buffer loop
}

void TreeItemScreenFrame::HandleFrameTransform( AVFrame* pRgb, uint8_t* buf, void* user, structTimeSteps perf, int srcX, int srcY ) {
  
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
  
  // may need to set a unique itemid if multiple events running simultaneously
  // need to change this to the specific canvas into which the frame is going to be displayed
  wxApp::GetInstance()->QueueEvent( new EventImage( EVENT_IMAGE, m_pScreenFrame->GetFrame()->GetId(), pImage, user, perf ) );
  //m_pScreenFrame->GetFrame()->QueueEvent( new EventImage( EVENT_IMAGE, -1, pImage, user, perf ) );
  //m_pScreenFrame->GetFrame()->get QueueEvent( new EventImage( EVENT_IMAGE, -1, pImage, user, perf ) );
  
  av_free( buf );
  av_free( pRgb );  
  
}

void TreeItemScreenFrame::HandleEventImage( EventImage& event ) {

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

void TreeItemScreenFrame::ProcessVideoFile( boost::shared_ptr<DecodeH264> pDecoder ) {
  pDecoder->ProcessFile();
}

void TreeItemScreenFrame::HandleLoadVideo( wxCommandEvent& event ) {
  std::cout << "LoadPicture" << std::endl;  
  wxFileDialog dialogOpenFile( 
    m_pScreenFrame->GetFrame(), wxT("Select Video" ), m_sVideoDirectory, "", 
    //"Video Files (*.ts)|*.ts", 
    "Video Files (*.h264)|*.h264", 
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    m_sVideoDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    std::cout << "dir " << m_sVideoDirectory << std::endl;
    //assert( m_image.LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );

    boost::shared_ptr<DecodeH264> pDecoder( new DecodeH264 ( m_pScreenFrame->GetFrame() ) );
    
    namespace args = boost::phoenix::arg_names;
    pDecoder->m_OnFrame.connect( 
      boost::phoenix::bind( &TreeItemScreenFrame::HandleOnFrame, this, args::arg1, args::arg2, args::arg3, args::arg4, args::arg5 ) );

    pDecoder->load( dialogOpenFile.GetPath() );
    // ProcessFile to be handled in thread

    m_Srvc.post( boost::phoenix::bind( &TreeItemScreenFrame::ProcessVideoFile, this, pDecoder ) );
    
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

void TreeDisplayManager::Append( pScreenFrame_t pScreenFrame ) {
  wxTreeItemId idRoot = wxTreeCtrl::GetRootItem();
  std::string sId = boost::lexical_cast<std::string>( pScreenFrame->GetId() );
  wxTreeItemId id = wxTreeCtrl::AppendItem( idRoot, "Frame " + sId );
  EnsureVisible( id );
  
  pTreeItem_t pTreeItem( new TreeItemScreenFrame( this, id, pScreenFrame ) );
  Add( id, pTreeItem );
  //m_mapDecoder.insert( mapDecoder_pair_t( id.GetID(), pTreeItem ) );
}

void TreeDisplayManager::Add( const wxTreeItemId& id, pTreeItem_t pTreeItem ) {
  m_mapDecoder.insert( mapDecoder_pair_t( id.GetID(), pTreeItem ) );
}

void TreeDisplayManager::Delete( wxTreeItemId id ) {
  //wxTreeItemId id( pTreeItem->GetTreeItemId() );
  if ( 0 == GetChildrenCount( id ) ) {
    mapDecoder_t::iterator iter = m_mapDecoder.find( id.GetID() );
    assert( m_mapDecoder.end() != iter );
    wxTreeCtrl::Delete( id );
    m_idOld.Unset();
    m_mapDecoder.erase( iter );
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
  
  wxTreeItemId id = wxTreeCtrl::AddRoot( "Projections" );
  pTreeItem_t pTreeItem( new TreeItemRoot( this, id ) );
  m_mapDecoder.insert( mapDecoder_pair_t( id.GetID(), pTreeItem ) );
  
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

wxBitmap TreeDisplayManager::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon TreeDisplayManager::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}
