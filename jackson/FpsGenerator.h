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

#include <boost/thread/thread.hpp>

class FpsGenerator {
public:
  
  FpsGenerator( );
  virtual ~FpsGenerator( );
  
protected:  
private:

  bool m_bStopThread;
  bool m_bThreadRunning;
  
  boost::thread m_thread;
  
  void Thread( void ); // stuff running in the thread
};

#endif	/* FPSGENERATOR_H */

