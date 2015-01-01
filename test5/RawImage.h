/*
  Copyright (2014) Raymond Burkholder
  GPL2 License
  Created 2014/12/28
  Contact:  raymond@burkholder.net
*/

#pragma once

#include <FastDelegate.h>

#include <libraw/libraw.h>

class RawImage {
public:

  RawImage();
  virtual ~RawImage();

  typedef fastdelegate::FastDelegate1<libraw_output_params_t&, void> LibRawOutputParamsHandler_t;

  void SetLibRawOutputParams( LibRawOutputParamsHandler_t handler ) {
    m_OnLibRawOutputParams = handler;
  }

  void LoadImage( const std::string& sFileName );
  libraw_processed_image_t* CalcImage( void );
  void FreeImage( libraw_processed_image_t* pImage );

  void FileInfo( void );

protected:
private:

  bool m_bImageLoaded;
  std::string m_sFileName;

  LibRawOutputParamsHandler_t m_OnLibRawOutputParams;  // called just prior to post processing

  LibRaw m_raw;

};

