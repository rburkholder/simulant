/* 
 * File:   common.h
 * Author: rpb
 *
 * Created on March 15, 2015, 5:13 PM
 */

#ifndef COMMON_H
#define	COMMON_H

#include <boost/chrono/duration.hpp>
#include <boost/chrono/system_clocks.hpp>

typedef boost::chrono::high_resolution_clock::time_point tp;

struct structTimeSteps {
  tp start;
  tp parse;
  tp decoded;
  tp filled;
  tp scaled;
  tp queue1;
  tp copied;
  tp queue2;
  tp drawn;
};


#endif	/* COMMON_H */

