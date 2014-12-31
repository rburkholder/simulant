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

  //libraw_output_params_t& Params( void ) { return m_raw.imgdata.params; };
  libraw_processed_image_t* ObtainImage( const std::string& sFileName );

protected:
private:

  LibRawOutputParamsHandler_t m_OnLibRawOutputParams;

  //LibRaw m_raw;

};

