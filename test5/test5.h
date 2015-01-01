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
#include "PanelPicture.h"
#include "PanelLibRawOptions.h"
#include <PanelLogging.h>

class AppTest5 : public wxApp {
public:
protected:
private:

  //PanelLibRawOptions::eInterpolation_t m_user_qual;
  PanelLibRawOptions::options_t m_options;

  RawImage m_ri;

  wxFrame* frameMain;
  PanelPicture* panelPicture;
  PanelLibRawOptions* panelLibRawOptions;
  ou::PanelLogging* panelLogging;

  void SetLibRawOutputParams( libraw_output_params_t& params );

  void HandleDemosaicSelection( const PanelLibRawOptions::options_t& );
  void LoadImage( void );

  void OnMouseWheel1( wxMouseEvent& event );
  //void OnMouseWheel2( wxMouseEvent& event );

  virtual bool OnInit( void );
  virtual int OnExit( void );
  void OnClose( wxCloseEvent& event );
};

// Implements MyApp& wxGetApp()
DECLARE_APP( AppTest5 )

