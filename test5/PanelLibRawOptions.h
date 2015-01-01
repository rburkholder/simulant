/*
Copyright (2014) Raymond Burkholder
GPL2 License
Created 2014/12/29
Contact:  raymond@burkholder.net
*/

#pragma once

//#include <map>

#include <FastDelegate.h>

#define SYMBOL_PANELLIBRAWOPTIONS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELLIBRAWOPTIONS_TITLE _("PanelLibRawOptions")
#define SYMBOL_PANELLIBRAWOPTIONS_IDNAME ID_PANELLIBRAWOPTIONS
#define SYMBOL_PANELLIBRAWOPTIONS_SIZE wxSize(400, 300)
#define SYMBOL_PANELLIBRAWOPTIONS_POSITION wxDefaultPosition

class PanelLibRawOptions : public wxPanel {
public:

  enum eInterpolation_t {
    Unknown = -1, Linear = 0, VNG, PPG, AHD, DCB, ModAHD, AFD, VCD, VCDAHD, LMMSE, AMaZE
  };

  enum eColourSpace_t {
    RawColour = 0, sRGB, Adobe, WideGamut, KodakProPhoto, XYZ
  };

  struct options_t {
    bool bAutoBrighten;
    bool bAutoWhiteBalance;
    bool bUseCameraWhiteBalance;
    eInterpolation_t eInterpolation;
    eColourSpace_t eColourSpace;
    unsigned int intNoiseThreshold; // change to float
    unsigned int nMedianFilterPasses;
    options_t( void )
      : eInterpolation( Linear ), eColourSpace( sRGB ), intNoiseThreshold( 0 ),
      bAutoBrighten( false ), bAutoWhiteBalance( false ), bUseCameraWhiteBalance( true ),
      nMedianFilterPasses( 0 ) {}
  };

  typedef fastdelegate::FastDelegate1<const options_t&, void> OptionHandler_t;

  PanelLibRawOptions( void );
  virtual ~PanelLibRawOptions( );

  PanelLibRawOptions(
    wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELLIBRAWOPTIONS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELLIBRAWOPTIONS_POSITION, 
    const wxSize& size = SYMBOL_PANELLIBRAWOPTIONS_SIZE, 
    long style = SYMBOL_PANELLIBRAWOPTIONS_STYLE );

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELLIBRAWOPTIONS_IDNAME,
    const wxPoint& pos = SYMBOL_PANELLIBRAWOPTIONS_POSITION,
    const wxSize& size = SYMBOL_PANELLIBRAWOPTIONS_SIZE,
    long style = SYMBOL_PANELLIBRAWOPTIONS_STYLE );

  void SetOptionHandler( OptionHandler_t handler ) {
    m_OnOption = handler;
  }

  static bool ShowToolTips( ) { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:
private:

  enum {
    ID_Null = wxID_HIGHEST, ID_PANELLIBRAWOPTIONS, 
    ID_rbDemosaicType, ID_SliderNoiseThreshold, ID_choiceColourSpace,
    ID_txtMedianFilterPasses, ID_btnProcess, 
    ID_cbAutoBrighten, ID_cbAutoWhiteBalance, ID_cbUseCameraWhiteBalance
  };

  options_t m_options;

//  typedef std::map<int, eInterpolation_t> mapLUInterpolation_t;
//  mapLUInterpolation_t m_mapLUInterpolation;

  wxRadioBox* m_rbDemosaicType;
  wxCheckBox* m_cbAutoBrighten;
  wxCheckBox* m_cbAutoWhiteBalance;
  wxCheckBox* m_cbUseCameraWhiteBalance;
  wxChoice* m_choiceColourSpace;
  wxStaticBox* m_boxMedianFilterPasses;
  wxStaticBoxSizer* m_sizerMedianFilterPasses;
  wxTextCtrl* m_txtMedianFilterPasses;
  wxSlider* m_sliderNoiseThreshold;
  wxButton* m_btnProcess;

  void Init( );
  void CreateControls( );

  OptionHandler_t m_OnOption;
  //void OnRbInterpSelected( wxCommandEvent& event );
  virtual void OnBtnProcessClick( wxCommandEvent& event );

  void OnClose( wxCloseEvent& event );

};

