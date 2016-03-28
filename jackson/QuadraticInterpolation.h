/* 
 * File:   QuadraticInterpolation.h
 * Author: rpb
 *
 * Created on February 21, 2016, 8:22 PM
 */

#pragma once

// based upon:
//  http://isezen.com/2012/01/15/quadratic-interpolation-three-point/

// x0 < x1 < x2

// y = f(x) = Ax^2 + Bx + C

// a0 = f(x0) / ( ( x0 - x1 )( x0 - x2 ) )
// a1 = f(x1) / ( ( x1 - x0 )( x1 - x2 ) )
// a2 = f(x2) / ( ( x2 - x0 )( x2 - x1 ) )

// A = a0 + a1 + a2
// B = -[ a0( x1 + x2 ) + a1( x0 + x2 ) + a2( x0 + x1 ) ]
// C = a0 x1 x2 + a1 x0 x2 + a2 x0 x1

// f(x) = a0( x - x1 )( x - x2 ) + a1( x - x0 )( x - x2 ) + a2( x - x0 )( x - x1 )

template<typename inputx_t, typename inputy_t, typename output_t>
class QuadraticInterpolation {
public:
  
  QuadraticInterpolation( 
    inputx_t x0_, inputy_t y0_, 
    inputx_t x1_, inputy_t y1_, 
    inputx_t x2_, inputy_t y2_ 
  ) 
  {
    Reset( x0_, y0_, x1_, y1_, x2_, y2_ );
  }

  QuadraticInterpolation( void ) {
    Reset( 0, 0, 1, 0, 2, 0 );
  }
  
  void Reset(
    inputx_t x0_, inputy_t y0_, 
    inputx_t x1_, inputy_t y1_, 
    inputx_t x2_, inputy_t y2_ 
  ) {
    
    assert( x0 < x1 );
    assert( x1 < x2 );
    
    x0 = x0_; x1 = x1_; x2 = x2_;
    y0 = y0_; y1 = y1_; y2 = y2_;
    
    a0den = ( x0 - x1 ) * ( x0 - x2 );
    a1den = ( x1 - x0 ) * ( x1 - x2 );
    a2den = ( x2 - x0 ) * ( x2 - x1 ); 
    
  }
  
  output_t operator() ( inputx_t x ) { return fx( x ); }
  
protected:
private:
  
  output_t fx( inputx_t x ) {
    output_t term0num = y0 * (inputy_t) ( ( x - x1 ) * ( x - x2 ) );
    output_t term1num = y1 * (inputy_t) ( ( x - x0 ) * ( x - x2 ) );
    output_t term2num = y2 * (inputy_t) ( ( x - x0 ) * ( x - x1 ) );
    output_t term0 = term0num / a0den;
    output_t term1 = term1num / a1den;
    output_t term2 = term2num / a2den;
    return term0 + term1 + term2;
  }
  
  const inputx_t x0;
  const inputx_t x1;
  const inputx_t x2;
  
  const inputy_t y0;
  const inputy_t y1;
  const inputy_t y2;
  
  const output_t a0den;
  const output_t a1den;
  const output_t a2den;
};


/* 
 other smoothing operators:
 
 * http://paulbourke.net/geometry/bezier/
 * http://paulbourke.net/miscellaneous/interpolation/
 
 */