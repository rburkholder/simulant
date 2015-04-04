/* 
 * File:   Cue.h
 * Author: rpb
 *
 * Created on April 3, 2015, 2:36 PM
 */

#ifndef CUE_H
#define	CUE_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/chrono.hpp>

class Cue {
public:
  typedef float CueNumber_t;
  explicit Cue( CueNumber_t CueNumber );
  virtual ~Cue( void );
protected:
  CueNumber_t m_CueNumber;
  std::string m_sDescription;
  CueNumber_t m_CueNext;
private:
};

class CueRelative: public Cue {
public:
protected:
  boost::chrono::microseconds m_delayFadeIn;
  boost::chrono::microseconds m_delayWait;
  boost::chrono::microseconds m_delayFadeOut;
private:
};

#endif	/* CUE_H */

