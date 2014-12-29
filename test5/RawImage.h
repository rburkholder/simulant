
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

