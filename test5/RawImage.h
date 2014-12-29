/*
  Copywrite (2014) Raymond Burkholder
  GPL2 License
  Created 2014/12/28
  Contact:  raymond@burkholder.net
*/

#pragma once

#include <libraw/libraw.h>

class RawImage {
public:
  RawImage();
  virtual ~RawImage();

  static libraw_processed_image_t* ObtainImage( const std::string& sFileName );
protected:
private:
};

