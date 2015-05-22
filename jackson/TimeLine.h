/* 
 * File:   TimeLine.h
 * Author: rpb
 *
 * Created on May 17, 2015, 7:31 PM
 */

#pragma once

#include <map>
#include <stdexcept>
#include <string>

#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>

// maintains a variable
// has multiple keyframes
// interpolates between keyframes
// uses ticks? to count
// if keyframes are skipped, then current value is used
// each keyframe maintains a value specific to that step
// even better, a timeline is one or more keyframes... maintains consistency
// will therefore need some form of positionaless keyframe when only a single keyframe present, reflecting zero size timeline
// or keyframes can exist outside of a timeline, and a time line must always have one more keyframes

template <typename Value>
class KeyFrame {
public:
  
  typedef boost::shared_ptr<KeyFrame> pKeyFrame_t;
  
  typedef boost::signals2::signal<void ()> signalDeleting_t;
  typedef signalDeleting_t::slot_type slotDeleting_t;
  
  typedef boost::signals2::signal<Value ()> signalChanged_t;
  typedef typename signalChanged_t::slot_type slotChanged_t;
  
  KeyFrame( void );
  explicit KeyFrame( Value ); // does not trigger changed event
  KeyFrame( Value, const std::string& ); // does not trigger changed event
  virtual ~KeyFrame( void );
  
  void SetValue( Value );
  Value GetValue( void ) const;
  
  void SetDescription( const std::string& sDescription ) { m_sDescription = sDescription; }
  const std::string& GetDescription( void ) const { return m_sDescription; }
  
  // are these useful without a related Position identifier?
  // these also render the class non-copyable
  boost::signals2::connection Changed( const slotChanged_t& slot ) { return m_signalChanged.connect( slot ); }
  boost::signals2::connection Deleting( const slotDeleting_t& slot ) { return m_signalDeleting.connect( slot ); }
  
protected:
private:
  std::string m_sDescription;
  Value m_value {};  // value initialization https://akrzemi1.wordpress.com/2013/09/10/value-initialization-with-c/
  signalChanged_t m_signalChanged;
  signalDeleting_t m_signalDeleting;
};

template <typename Value>
KeyFrame<Value>::KeyFrame( void )  {
}

template <typename Value>
KeyFrame<Value>::KeyFrame( Value value ) : m_value( value ) {  // does not trigger changed event
}

template <typename Value>
KeyFrame<Value>::KeyFrame( Value value, const std::string& sDesc )  // does not trigger changed event
: m_value( value ), m_sDescription( sDesc )
{
}

template <typename Value>
KeyFrame<Value>::~KeyFrame( void ) {
  m_signalDeleting();
}

template <typename Value>
void KeyFrame<Value>::SetValue( Value value ) {
  m_value = value;
  m_signalChanged( m_value );
}

template <typename Value>
Value KeyFrame<Value>::GetValue( void ) const {
  return m_value;
}

// ===========

template <typename Position, typename Value>
class TimeLine {
public:
  
  TimeLine( void );
  virtual ~TimeLine( void );
  
  typedef typename KeyFrame<Value>::pKeyFrame_t pKeyFrame_t;
  
  void Insert( Position, pKeyFrame_t );
  //void Delete( Position );  // maybe need a delete
  //void Move( Position, Position );  // maybe need a move)
  
  //Value Interpolate( Position, Value );
  Value Interpolate( Position );
  
  // void Iterate( function f );  // provide position and keyframe&
  
protected:
private:
  
  typedef std::map<Position,pKeyFrame_t> mapKeyFrame_t;
  
  mapKeyFrame_t m_mapKeyFrame;
  
  // interpolation state keeping  to prevent repetitive map lookups
  Position m_pos1;
  Position m_deltaPos;
  Value m_val1;
  Value m_deltaVal;
  // keep two iterators to reduce the number of lookups.
  // invalidate iterators when map changes
  typename mapKeyFrame_t::const_iterator m_iterBegin;
  typename mapKeyFrame_t::const_iterator m_iterEnd;
};

template <typename Position, typename Value>
TimeLine<Position,Value>::TimeLine( void ) {
  m_iterBegin = m_iterEnd = m_mapKeyFrame.end();
}

template <typename Position, typename Value>
TimeLine<Position,Value>::~TimeLine( void ) { 
}

template <typename Position, typename Value>
void TimeLine<Position,Value>::Insert(Position position, pKeyFrame_t pKeyFrame ) {
  assert( 0 != pKeyFrame.get() );
  typename mapKeyFrame_t::const_iterator iter = m_mapKeyFrame.find( position );
  if ( m_mapKeyFrame.end() != iter ) {
    throw std::runtime_error( "position already exists" );
  }
  m_mapKeyFrame.insert( mapKeyFrame_t::value_type( position, pKeyFrame ) );
  m_iterBegin = m_iterEnd = m_mapKeyFrame.end();  // invalidate iterators
}

template <typename Position, typename Value>
Value TimeLine<Position,Value>::Interpolate(Position position ) {
  Value value;
  if ( 0 == m_mapKeyFrame.size() ) {
    throw std::runtime_error( "no keyframes available for interpolation" );
  }
  if ( position < m_mapKeyFrame.begin()->first ) {
    throw std::runtime_error( "position is prior to first keyframe" );
  }
  if ( position > m_mapKeyFrame.rbegin()->first ) {
    throw std::runtime_error( "position is after last keyframe" );
  }
  if ( 1 == m_mapKeyFrame.size() ) {
    return  m_mapKeyFrame.begin()->second->GetValue();
  }
  else {
    bool bReaquireEndpoints( false );
    if ( m_mapKeyFrame.end() == m_iterBegin ) { // iterators are not valid, so init everything
      bReaquireEndpoints = true;
    }
    else { // check iterators still have something valid
      if ( ( position <  m_pos1 ) || ( ( m_pos1 + m_deltaPos ) < position ) ) {
        bReaquireEndpoints = true;
      }
    }
    if ( bReaquireEndpoints ) {
      m_iterBegin = m_mapKeyFrame.lower_bound( position );
      m_iterEnd = m_iterBegin;
      if ( m_iterBegin->first == position ) {
//        value = m_iterBegin->second->GetValue();  // re-arrange in order to use this optimization
      }
      else {
        assert( m_iterBegin != m_mapKeyFrame.begin() );
        --m_iterBegin;
      }
      m_pos1 = m_iterBegin->first;
      m_val1 = m_iterBegin->second->GetValue();
      m_deltaPos = m_iterEnd->first - m_pos1;
      m_deltaVal = m_iterEnd->second->GetValue() - m_val1;
      bReaquireEndpoints = false;
    }
    // perform the interpolation
    if ( 0 == m_deltaPos ) { // optimize degenerate case
      value = m_val1;
    }
    else { // interpolate over the range
      value = m_val1 + ( ( m_deltaVal ) * ( position - m_pos1 ) / ( m_deltaPos ) );
    }
  }
  return value;
}
