/* 
 * File:   FpsGenerator.h
 * Author: rpb
 * Frames Per Second Generator
 *   provides time base for video frame and graphics frame presentation
 *
 * Created on April 18, 2015, 6:08 PM
 */

#pragma once

#include <map>

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/signals2.hpp>

class FrameRate;

class FpsGenerator {
public:
  
  enum FPS { fps24x = 23, fps24 = 24, fps25 = 25, fps30x = 29, fps30 = 30, fps48 = 48, fps60 = 60, fps100 = 100 }; 
  
  typedef boost::signals2::signal<void ( FPS )> signalFrame_t;  // signal definition
  typedef signalFrame_t::slot_type slotFrame_t;       // slot definition
  
  FpsGenerator( );
  virtual ~FpsGenerator( );
  
  boost::signals2::connection Connect( FPS, const slotFrame_t& );
  
  FPS FindFrameRate( size_t num, size_t den );
  
protected:  
private:
  
  typedef boost::shared_ptr<FrameRate> pFrameRate_t;
  
  bool m_bStopThread;
  bool m_bThreadRunning;
  
  boost::thread m_thread;
  
  typedef std::map<FPS, pFrameRate_t> mapFrameRate_t;
  mapFrameRate_t m_mapFrameRate;
  
  void Thread( void ); // stuff running in the thread
};
