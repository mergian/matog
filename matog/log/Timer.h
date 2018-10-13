// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#pragma once
#ifndef __MATOG_LOG_TIMER_H
#define __MATOG_LOG_TIMER_H

#include <stdint.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "dll.h"
#include <chrono>

namespace matog {
	namespace log {
//-------------------------------------------------------------------
/// Timer class used for printing time
class Timer {
private:
	/// used timer
	std::chrono::high_resolution_clock::time_point m_start;

	//------------------------------------------------------------------
	/// get the current timer value as 64bit unsigned int (in microseconds)
	inline uint64_t getCurrentTimerValue(void) const {
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		return (uint64_t)std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count();
	}

public:
	/// time struct
	struct tm {
		uint32_t days;
		uint32_t hours;
		uint32_t minutes;
		uint32_t seconds;
		uint32_t milliseconds;
	};

	/// init Timer
	inline Timer(void) {
		reset();
	}

	/// reset Timer
	inline void reset(void) {
		m_start = std::chrono::high_resolution_clock::now();
	}

	/// get in seconds
	inline double getSeconds(void) const {
		return getCurrentTimerValue() / 1000000.0;
	}

	/// get in milliseconds
	inline double getMilliSeconds(void) const  {
		return getCurrentTimerValue() / 1000.0;
	}

	/// get in microseconds
	inline uint64_t getMicroSeconds(void) const {
		return getCurrentTimerValue();
	}

	/// build std::tm object
	inline void getTM(tm& tm) const {
		static const uint64_t day	= 60 * 60 * 24 * 1000;
		static const uint64_t hour	= 60 * 60* 1000;
		static const uint64_t min	= 60 * 1000;
		static const uint64_t sec	= 1000;
		
		uint64_t time = (uint64_t)getMilliSeconds();
		tm = {0};
		
		if(time >= day) {
			tm.days = (int)(time / day);
			time -= tm.days * day;
		}
		
		if(time >= hour) {
			tm.hours = (int)(time / hour);
			time -= tm.hours * hour;
		}

		if(time >= min) {
			tm.minutes = (int)(time / min);
			time -= tm.minutes * min;
		}

		if(time >= sec) {
			tm.seconds = (int)(time / sec);
			time -= tm.seconds * sec;
		}

		tm.milliseconds = (int)(time);
	}
};

//-------------------------------------------------------------------
	}
}

#endif