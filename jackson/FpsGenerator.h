/* 
 * File:   FpsGenerator.h
 * Author: rpb
 * Frames Per Second Generator
 *   provides time base for video frame and graphics frame presentation
 *
 * Created on April 18, 2015, 6:08 PM
 */

#ifndef FPSGENERATOR_H
#define	FPSGENERATOR_H

#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/signals2.hpp>

class DownCount;

class FpsGenerator {
public:
  
  enum FPS { fps24=0, fps25, fps30, fps48, fps60, fps100 };
  
  typedef boost::signals2::signal<void ()> signalFrame_t;  // signal definition
  typedef signalFrame_t::slot_type slotFrame_t;       // slot definition
  
  FpsGenerator( );
  virtual ~FpsGenerator( );
  
  boost::signals2::connection Connect( FPS, const slotFrame_t& );
  
protected:  
private:
  
  typedef boost::shared_ptr<DownCount> pDownCount_t;
  
  bool m_bStopThread;
  bool m_bThreadRunning;
  
  boost::thread m_thread;
  
  typedef std::vector<pDownCount_t> vDownCount_t;
  vDownCount_t m_vDownCount;
  
  void Thread( void ); // stuff running in the thread
};

#endif	/* FPSGENERATOR_H */

