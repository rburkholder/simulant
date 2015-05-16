/* 
 * File:   Cue.h
 * Author: rpb
 *
 * Created on April 3, 2015, 2:36 PM
 */

#pragma once

// Each video or picture is a particlar CueSet, each CueSet has multiple Cues, each with specialized transformations (sound, video, picture
// Need to create a time line  per CueSet:  pre-load, placement, fade-in, delay, fade-out, translate/scale/rotation
// so need an idea of time between each frame, or not, what is the time base? 10ms?  runs off the global FpsGenearator in the SceneManager?
// might be appropriate as SceneElements are where the transformation coordinates are

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
  // has absolute + relative start capabilities based upon keyframe alighnments
  // may be able to perform time offsets, SceneManager is working at 1ms intervals
};

class CueRelative: public Cue {
public:
protected:
  boost::chrono::milliseconds m_delayFadeIn;
  boost::chrono::milliseconds m_delayWait;
  boost::chrono::milliseconds m_delayFadeOut;
private:
};

class CueForVideo: public Cue {
  // start frame n1
  // fade in rate from frame n1 to frame n2
  // delay to frame n3
  // fade out from frame n3 to frame n4
  // n1 < n2 <= n3 < n4
  // if allowed backwards, may need to queue frames from going forward, and then reverse the sequence, as seeking may be difficult on certain streams
  // fades are transitions, so each of n1, n2, n3, n4 are settings, and need to linearly interpolate between values for each frame
  // range of settings 0-255
  // then need to figure out how to handle the fade in OpenGL: set A (opacity) for each vertex?
public:
  // 
protected:
private:
  // n1, n2, n3, n4
  // rather than specific profile, use keyframe concept, variable array of numbers
  // each keyframe is a cue?
  // with this framework, a cue starts a keyframe set
  // index for starting keyframe + level, intermediate keyframes and levels, ending keyframe and level
  // keyframe, like premier pro, can have multiple attributes:
    // opacity, transform matrix, frame number, special effect modifier
};

// watch a video and watch timestamp updates to see how to seek frames