/*
  Copyright (2014) Raymond Burkholder
  GPL2 License
  Created 2014/12/28
  Contact:  raymond@burkholder.net
  */

// 2014/12/29
// to do:
//  serialize window positions to maintain state
//  save picture to file
//  convert 'Frame..' to 'Panel..' for reusability
//  load image for raw conversion and hold in memory

#pragma once

#include "RawImage.h"
#include "FramePicture.h"
#include "FrameLibRawOptions.h"

class AppTest5 : public wxApp {
public:
protected:
private:

  FrameLibRawOptions::eInterpolation_t m_user_qual;

  RawImage m_ri;

  FramePicture* framePicture;
  FrameLibRawOptions* frameLibRawOptions;

  void SetLibRawOutputParams( libraw_output_params_t& params );

  void HandleDemosaicSelection( FrameLibRawOptions::eInterpolation_t );
  void DemosaicImage( void );

  virtual bool OnInit( void );
  virtual int OnExit( void );
  void OnClose( wxCloseEvent& event );
};

// Implements MyApp& wxGetApp()
DECLARE_APP( AppTest5 )

