/* 
 * File:   CueList.h
 * Author: rpb
 *
 * Created on April 3, 2015, 1:33 PM
 */

#ifndef CUESHEET_H
#define	CUESHEET_H

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/chrono.hpp>

#include "Cue.h"

class CueList {
  // need to build an 'action register', based upon fades and delays, actions are carried out at 
  //   the required time offset
  // require a pause mode, and a cancel mode
public:
  CueList( );
  virtual ~CueList( );
private:
  typedef boost::shared_ptr<Cue> pCue_t;
  typedef std::map<Cue::CueNumber_t, pCue_t> mapCue_t;
  
  mapCue_t m_vCueList;
};

#endif	/* CUESHEET_H */

