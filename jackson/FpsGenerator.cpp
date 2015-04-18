/* 
 * File:   FpsGenerator.cpp
 * Author: rpb
 * 
 * Created on April 18, 2015, 6:08 PM
 */

#include <boost/phoenix/bind/bind_member_function.hpp>

#include "FpsGenerator.h"

FpsGenerator::FpsGenerator( ) 
: m_bStopThread( false ), m_bThreadRunning( false ), m_thread( boost::phoenix::bind( &FpsGenerator::Thread, this ) )
{
}

FpsGenerator::~FpsGenerator( ) {
  m_bStopThread = true;
  m_thread.join();  // wait for thread to end
}

void FpsGenerator::Thread( void ) {
  m_bThreadRunning = true;
  while ( ! m_bStopThread ) {
    
  }
}