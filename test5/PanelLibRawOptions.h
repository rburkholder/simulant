/*
Copyright (2014) Raymond Burkholder
GPL2 License
Created 2014/12/29
Contact:  raymond@burkholder.net
*/

#pragma once

#include <map>

#include <wx/frame.h>
#include <wx/radiobut.h>

#include <FastDelegate.h>

#define SYMBOL_FRAMELIBRAWOPTIONS_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FRAMELIBRAWOPTIONS_TITLE _("FrameLibRawOptions")
#define SYMBOL_FRAMELIBRAWOPTIONS_IDNAME ID_FRAMELIBRAWOPTIONS
#define SYMBOL_FRAMELIBRAWOPTIONS_SIZE wxSize(150, 300)
#define SYMBOL_FRAMELIBRAWOPTIONS_POSITION wxDefaultPosition

class FrameLibRawOptions: public wxFrame {
public:

  enum eInterpolation_t {
    Unknown = -1, Linear = 0, VNG, PPG, AHD, DCB, ModAHD, AFD, VCD, VCDAHD, LMMSE, AMaZE
  };

  typedef fastdelegate::FastDelegate1<eInterpolation_t, void> SelectInterpolationHandler_t;

  FrameLibRawOptions( void );
  virtual ~FrameLibRawOptions();

  FrameLibRawOptions(
    wxWindow* parent,
    wxWindowID id = SYMBOL_FRAMELIBRAWOPTIONS_IDNAME,
    const wxString& caption = SYMBOL_FRAMELIBRAWOPTIONS_TITLE,
    const wxPoint& pos = SYMBOL_FRAMELIBRAWOPTIONS_POSITION,
    const wxSize& size = SYMBOL_FRAMELIBRAWOPTIONS_SIZE,
    long style = SYMBOL_FRAMELIBRAWOPTIONS_STYLE );

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_FRAMELIBRAWOPTIONS_IDNAME,
    const wxString& caption = SYMBOL_FRAMELIBRAWOPTIONS_TITLE,
    const wxPoint& pos = SYMBOL_FRAMELIBRAWOPTIONS_POSITION,
    const wxSize& size = SYMBOL_FRAMELIBRAWOPTIONS_SIZE,
    long style = SYMBOL_FRAMELIBRAWOPTIONS_STYLE );

  void SetSelectInteropolationHandler( SelectInterpolationHandler_t handler ) {
    m_OnSelectInterpolation = handler;
  }

  static bool ShowToolTips( ) { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:
private:

  enum {
    ID_Null = wxID_HIGHEST, ID_FRAMELIBRAWOPTIONS, ID_PANEL, wxID_staticInterpLabel, 
    ID_rbInterpLinear, ID_rbInterpVNG, ID_rbInterpPPG, ID_rbInterpAHD, ID_rbInterpDCB, 
    ID_rbInterpModAHD, ID_rbInterpAFD, ID_rbInterpVCD, ID_rbInterpVCDAHD, ID_rbInterpLMMSE,
    ID_rbInterpAMaZE
  };

  typedef std::map<int, eInterpolation_t> mapLUInterpolation_t;
  mapLUInterpolation_t m_mapLUInterpolation;

  wxRadioButton* m_rbInterpLinear;
  wxRadioButton* m_rbInterpVNG;
  wxRadioButton* m_rbInterpPPG;
  wxRadioButton* m_rbInterpAHD;
  wxRadioButton* m_rbInterpDCB;
  wxRadioButton* m_rbInterpModAHD;
  wxRadioButton* m_rbInterpAFD;
  wxRadioButton* m_rbInterpVCD;
  wxRadioButton* m_rbInterpVCDAHD;
  wxRadioButton* m_rbInterpLMMSE;
  wxRadioButton* m_rbInterpAMaZE;

  void Init( );
  void CreateControls( );

  SelectInterpolationHandler_t m_OnSelectInterpolation;
  void OnRbInterpSelected( wxCommandEvent& event );

  void OnClose( wxCloseEvent& event );

};

