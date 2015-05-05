

#include <cstdlib>
#include <iostream>
#include <vector>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <wx/wx.h>
#include <wx/display.h>
#include <wx/sizer.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
#include <libavformat/avformat.h>
}

#include "panelSurfaceSources.h"
#include "appProjection.h"

IMPLEMENT_APP( AppProjection )

bool AppProjection::OnInit( ) {
  
  typedef PhysicalDisplay::pPhysicalDisplay_t pPhysicalDisplay_t;
  
  m_pFrameMain = new FrameMain( (wxFrame *)NULL, -1, wxT( "Projection Demo" ), wxPoint( 10, 10 ), wxSize( 500, 250 ) );
  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppProjection::OnClose, this );  // start close of windows and controls

  FrameMain::vpMenuItems_t vMenuItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
//  vMenuItems.push_back( new mi( "select picture", boost::phoenix::bind( &AppProjection::LoadPicture, this ) ) );
  vMenuItems.push_back( new mi( "check media file", boost::phoenix::bind( &AppProjection::CheckMedia, this ) ) );
//  vMenuItems.push_back( new mi( "image->opengl", boost::phoenix::bind( &AppProjection::Image2OpenGL, this ) ) ) ;
  m_pFrameMain->AddDynamicMenu( "Actions", vMenuItems );
  
//  vMenuItems.clear();
//  vMenuItems.push_back( new mi( "Canvas", boost::phoenix::bind( &AppProjection::CreateCanvas, this ) ) );
//  vMenuItems.push_back( new mi( "Movie Screen", boost::phoenix::bind( &AppProjection::CreateMovieScreen, this ) ) );
//  m_pFrameMain->AddDynamicMenu( "Create", vMenuItems );
  
  wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
  m_pSurfaceSources = new panelSurfaceSources( m_pFrameMain, -1 );
  sizer->Add( m_pSurfaceSources, 1, wxEXPAND );

  unsigned int nDisplays = wxDisplay::GetCount();
  std::cout << "#connected displays: " << nDisplays << std::endl;
  
  for ( int ix = 0; ix < nDisplays; ++ix ) {
    
    wxDisplay display( ix );
    wxRect rectClientArea = display.GetClientArea();

    std::cout << "building on " << rectClientArea.x << ", " << rectClientArea.y << std::endl;
    
    // need to keep track of projection frames, so can iconize them sometime for visual reference in the gui
    // force frame size for the time being
    m_pSurfaceSources->Append( pPhysicalDisplay_t( new PhysicalDisplay( ix, m_pFrameMain, wxPoint( rectClientArea.x, rectClientArea.y ), wxSize( 1920, 1080 ) ) ) );
    
  }
  
  m_pFrameMain->SetSizer( sizer );
  m_pFrameMain->SetAutoLayout( true );

  m_pFrameMain->Show( );
  // serialize the following for session to session persistence
  m_pFrameMain->Move( wxPoint( 1950, 150 ) );
  
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
  
//  m_pSurfaceSources->Bind( wxEVT_TOGGLEBUTTON, &AppProjection::HandleToggleEditMode, this, panelSurfaceSources::ID_BTN_EDITMODE );
//  m_pSurfaceSources->Bind( wxEVT_BUTTON, &AppProjection::HandleUndo, this, panelSurfaceSources::ID_BTN_UNDO );
  
  return true;
}

void AppProjection::CheckMedia2( const std::string& sFile ) {
  
  std::cout << std::endl << "================" << std::endl;
  std::cout << "Checking " << sFile << std::endl;
#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
    
  avcodec_register_all();
  av_register_all();

  AVFormatContext *context = 0;
  AVDictionaryEntry *dict = 0;
  
  struct StreamInfo {
    AVCodec* pCodec;
    AVCodecContext* pCodecContext;
    StreamInfo( void ) : pCodec( 0 ), pCodecContext( 0 ) {}
    ~StreamInfo( void ) {
      if ( 0 != pCodecContext ) {
        avcodec_close( pCodecContext );
        avcodec_free_context( &pCodecContext );
        pCodecContext = 0;
      }
    }
  };
  
  typedef std::vector<StreamInfo> vStreamInfo_t;
  vStreamInfo_t vStreamInfo;

  int status;

  // https://libav.org/doxygen/master/group__lavf__decoding.html
  // https://libav.org/doxygen/master/transcode_aac_8c-example.html#a7

  // open file, and fill related context
  status = avformat_open_input( &context, sFile.c_str(), NULL, NULL);
  if ( 0 != status ) {
    std::cout << "info: error on open" << std::endl;
  }
  else {
    // emit dictionary
    while ( ( dict = av_dict_get( context->metadata, "", dict, AV_DICT_IGNORE_SUFFIX) ) ) {
      std::cout << "info dict: " << dict->key << "=" << dict->value << std::endl;
    }
    
    // check that there are streams available
    status = avformat_find_stream_info( context, 0 );  // https://libav.org/doxygen/master/group__lavf__decoding.html
    if ( 0 > status ) {
      std::cout << "info strm: none" << std::endl;
    }
    else {
      // when found iterate the streams
      unsigned int nbStreams = context->nb_streams;
      vStreamInfo.resize( nbStreams );
      std::cout << "info dtl: " << "#streams=" << nbStreams << std::endl;
      if ( 0 != nbStreams ) {
        for ( unsigned int ix = 0; ix < nbStreams; ++ix ) {
          
          vStreamInfo[ix].pCodec = avcodec_find_decoder( context->streams[ix]->codec->codec_id );
          if ( 0 == vStreamInfo[ix].pCodec ) {
            std::cout << "info strm " << ix << " no codec" << std::endl;
          }
          else {
            
            
            
            // when a stream with codec found, prepare to use it in decode activities
            std::cout << "info strm " << ix << " " << vStreamInfo[ix].pCodec->name << std::endl;
            vStreamInfo[ix].pCodecContext = avcodec_alloc_context3( vStreamInfo[ix].pCodec );
            
            if ( vStreamInfo[ix].pCodec->capabilities & CODEC_CAP_TRUNCATED ) {
              vStreamInfo[ix].pCodecContext->flags |= CODEC_FLAG_TRUNCATED;
            }
            
            // http://stackoverflow.com/questions/12778978/libav-ffmpeg-avcodec-decode-video2-returns-1-when-separating-demultiplexing
            status = avcodec_copy_context( vStreamInfo[ix].pCodecContext, context->streams[ix]->codec );
            if ( 0 != status ) {
              std::cout << "error on stream " << ix << ": avcodec_copy_context" << std::endl;
            }
      
            status = avcodec_open2( vStreamInfo[ix].pCodecContext, vStreamInfo[ix].pCodec, 0 ); // https://libav.org/doxygen/master/group__lavc__core.html
            if ( 0 != status ) {
              throw std::runtime_error( "couldn't set codec");
              // lots of uncleaned variables here
            }
          }
          AVRational fr( context->streams[ix]->avg_frame_rate );
          std::cout << "info strm " << ix << " fr " << fr.num << "/" << fr.den << std::endl;
          int64_t nbFrames( context->streams[ix]->nb_frames );
          std::cout << "info strm " << ix << " frames " << nbFrames << std::endl;
          std::cout << "info strm " << ix << " buffer size " << context->max_picture_buffer << std::endl;
          std::cout << "info strm " << ix << " w/h " 
            << context->streams[ix]->codec->height << ", " << context->streams[ix]->codec->coded_height << ", "
            << context->streams[ix]->codec->width  << ", " << context->streams[ix]->codec->coded_width << std::endl;
          AVRational ar( context->streams[ix]->codec->sample_aspect_ratio );
          std::cout << "info strm " << ix << " ar " << ar.num << "/" << ar.den << std::endl;
          std::cout << "info strm " << ix << " channels " << context->streams[ix]->codec->channels << std::endl; // audio channels
          AVPixelFormat pf( context->streams[ix]->codec->pix_fmt );
          std::cout << "info strm " << ix << " pixfmt " << pf << std::endl;
          AVMediaType mt( context->streams[ix]->codec->codec_type );
          std::cout << "info strm " << ix << " type " << mt << std::endl;
          AVRational tb( context->streams[ix]->time_base );
          std::cout << "info strm " << ix << " tb " << tb.num << "/" << tb.den << std::endl;
        }
      }
      
    }
    
    // find the best stream for decoding
    int ixBestVideoStream( 0 );
    AVCodec* pBestCodecForVideo( 0 );
    ixBestVideoStream = av_find_best_stream( context, AVMediaType::AVMEDIA_TYPE_VIDEO, -1, -1, &pBestCodecForVideo, 0 );
    if ( 0 > ixBestVideoStream ) {
      std::cout << "no best video stream found" << std::endl;
    }
    
    int ixBestAudioStream( 0 );
    AVCodec* pBestCodecForAudio( 0 );
    ixBestAudioStream = av_find_best_stream( context, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1, &pBestCodecForAudio, 0 );
    if ( 0 > ixBestAudioStream ) {
      std::cout << "no best audio stream found" << std::endl;
    }
    
    std::cout << "best video stream: " << ixBestVideoStream << ", best audio stream: " << ixBestAudioStream << std::endl;

    if ( 0 <= ixBestVideoStream ) {
      // status has stream number
      
      //AV_TIME_BASE
      //AVFormatContext *pFormatCtx;
      //double duration = double(pFormatCtx->duration) / AV_TIME_BASE;
      
      
      AVPacket packet;
      av_init_packet( &packet );
      //packet.data = new uint8_t[ context->max_picture_buffer ];
      //packet.size = context->max_picture_buffer;
      packet.data = 0;
      packet.size = 0;
      AVFrame* pFrame; // https://libav.org/doxygen/master/group__lavu__frame.html
      pFrame = av_frame_alloc();
      //pFrame = avcodec_alloc_frame(); // http://pastebin.com/ByYzePa3  deprecated
      assert( 0 != pFrame );
      int cnt( 100 );
      std::cout << "refcounted frames " << vStreamInfo[ixBestVideoStream].pCodecContext->refcounted_frames << std::endl;
      while ( 0 == ( status = av_read_frame( context, &packet ) ) ) {  // https://libav.org/doxygen/master/group__lavf__decoding.html
        //std::cout << "packet " << packet.stream_index << ": " << packet.pts << ", " << packet.dts << ", " << packet.duration << std::endl;
        std::cout << "packet " << packet.stream_index << ": " << packet.size << ", " << std::endl;
        int got_picture( 0 );
        if ( ixBestVideoStream == packet.stream_index ) {
          // decode the best video stream
          status = avcodec_decode_video2( vStreamInfo[ixBestVideoStream].pCodecContext , pFrame, &got_picture, &packet );
          if ( 0 > status ) {
            std::cout << "decode has an error" << std::endl;
          }
          else {
            if ( 0 != got_picture ) {
              std::cout << "width: " << pFrame->width << " height: " << pFrame->height << " format: " << pFrame->format;
              if ( 1 == pFrame->key_frame ) std::cout << " key frame " << pFrame->display_picture_number;
              std::cout << std::endl;
            }
          }
        }
        --cnt;
        if ( 0 == cnt ) break;
      }
      
      // may want to use seek at some point (when looping) av_seek_frame : https://libav.org/doxygen/master/group__lavf__decoding.html)

      av_frame_free( &pFrame );
      //delete [] packet.data;
      av_free_packet( &packet );
    }
    
    avformat_close_input( &context );
  }
  context = 0;
}

void AppProjection::CheckMedia( void ) {
  
  std::cout << "Check Media" << std::endl;  
  
  wxString sMediaDirectory( "~/" );
  
  std::string sFile1( "/home/rpb/dwhelper/file-722448200.mp4");
  CheckMedia2( sFile1 );
  std::string sFile2( "/home/rpb/Videos/test.h264" );
  CheckMedia2( sFile2 );
  std::string sFile3( "/home/rpb/Videos/Ender's Game - Trailer.mp4" );
  CheckMedia2( sFile3 );
  std::string sFile4( "/home/rpb/Videos/big_buck_bunny_480p_h264.mov");
  CheckMedia2( sFile4 );
  std::string sFile5( "/home/rpb/Videos/decode_test_plain_nosound_20120726_720p-2M.ts" );
  CheckMedia2( sFile5 );
  std::string sFile6( "/home/rpb/Videos/Last.Knights.2015.HDRip.XViD-ETRG/Last.Knights.2015.HDRip.XViD-ETRG.avi");
  CheckMedia2( sFile6 );
  
    
/*
  wxFileDialog dialogOpenFile( 
    m_pFrameMain, wxT("Select Media" ), sMediaDirectory, "Check Media", 
    //"Video Files (*.ts)|*.ts", 
    //"Video Files (*.h264)|*.h264", 
    "",
    //_(" Files ") + wxImage::GetImageExtWildcard(),
    wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );
  if (dialogOpenFile.ShowModal() == wxID_OK) {
    sMediaDirectory = dialogOpenFile.GetDirectory();
    std::cout << "chose " << dialogOpenFile.GetPath() << std::endl;
    std::cout << "dir " << sMediaDirectory << std::endl;
    //assert( m_image.LoadFile( dialogOpenFile.GetPath(), wxBITMAP_TYPE_JPEG ) );
    
    std::string s( dialogOpenFile.GetPath() );
    
  }
  else {
    
  }
*/  
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
  //m_timerGuiRefresh.Stop();
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

