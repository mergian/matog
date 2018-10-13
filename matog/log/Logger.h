// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_LOG_LOGGER
#define __MATOG_LOG_LOGGER

#if WIN32
#include <Windows.h>
#endif
#include "dll.h"
#include <mutex>

namespace matog {
	namespace log {
//-------------------------------------------------------------------
#ifdef WIN32
class MATOG_LOG_DLL std::mutex;
#endif

//-------------------------------------------------------------------
/// console colors
enum Color {
	BLACK = 0,
	DARKBLUE, 
	DARKGREEN, 
	DARKTEAL, 
	DARKRED, 
	DARKPINK, 
	DARKYELLOW, 
	GRAY, 
	DARKGRAY, 
	BLUE, 
	GREEN, 
	TEAL, 
	RED, 
	PINK, 
	YELLOW, 
	WHITE,
	DEFAULT = 9999
};

#ifdef ERROR
#undef ERROR
#endif

/// console debug levels
enum Level {
	ERROR = 0,
	WARN,
	INFO,
	DEBUG,
	TRACE,
	INSANE
};

/// Sets the console color
extern void MATOG_LOG_DLL setColor(const Color color);

/// Resets the consoel color to m_defaultColor
extern void MATOG_LOG_DLL resetColor(void);

/// Gets the application execution time in seconds
extern double MATOG_LOG_DLL getSeconds(void);

/// Gets the application executime time in nanoseconds
extern uint64_t MATOG_LOG_DLL getMicroSeconds(void);

/// Helper function to print the time stamp
extern void MATOG_LOG_DLL printTime(const Level level);

/// Helper function to print the level
extern void MATOG_LOG_DLL printLevel(const Level level);

/// Helper function to print the file name and line
extern void MATOG_LOG_DLL printFile(const char* file, const int line);

/// Helper function to acquire the mutex
MATOG_LOG_DLL extern std::mutex& getLock(void);

/// Enables/Disables colors
extern void MATOG_LOG_DLL setUseColors(const bool value);

/// Enables/Disables filename
extern void MATOG_LOG_DLL setPrintFilename(const bool value);

/// Enables/Disables time
extern void MATOG_LOG_DLL setPrintTime(const bool value);

/// Enables/Disables level
extern void MATOG_LOG_DLL setPrintLevel(const bool value);

/// Enables/Disables debug level
extern void MATOG_LOG_DLL setLevel(const Level value);

/// Enables/Disables get level
extern Level MATOG_LOG_DLL getLevel(void);

/// Returns if exit message is shown
extern bool MATOG_LOG_DLL isPrintTime(void);

/// Returns if exit message is shown
extern bool MATOG_LOG_DLL isPrintLevel(void);

/// Returns if color is enabled
bool MATOG_LOG_DLL isUseColors(void);

/// Returns if filename is logged
bool MATOG_LOG_DLL isPrintFilename(void);

/// Returns level from string
Level MATOG_LOG_DLL getLevelFromString(const char* str);

/// is...
inline bool isError (void) { return getLevel() >= ERROR; }
inline bool isWarn  (void) { return getLevel() >= WARN; }
inline bool isInfo  (void) { return getLevel() >= INFO; }
inline bool isDebug (void) { return getLevel() >= DEBUG; }
inline bool isTrace (void) { return getLevel() >= TRACE; }
inline bool isInsane(void) { return getLevel() >= INSANE; }

//-------------------------------------------------------------------
	}
}

//-------------------------------------------------------------------
/// Starts a new message block and sets the color
#define __LOG_START(LEVEL) [&](void){ \
	if((LEVEL) > matog::log::getLevel()) return; \
	std::lock_guard<std::mutex> _lock(matog::log::getLock()); \
	matog::log::printLevel(LEVEL); \
	matog::log::printTime(LEVEL); \
	matog::log::printFile(__FILE__, __LINE__);

/// Starts a new message block and sets the color
#define __CLOG_START(LEVEL, COLOR) [&](void){ \
	if((LEVEL) > matog::log::getLevel()) return; \
	std::lock_guard<std::mutex> _lock(matog::log::getLock()); \
	matog::log::printLevel(LEVEL); \
	matog::log::printTime(LEVEL); \
	matog::log::printFile(__FILE__, __LINE__); \
	matog::log::setColor(COLOR);

/// Ends a message block
#define __LOG_END \
	matog::log::resetColor(); \
	printf("\n"); \
}()

#define L_END __LOG_END;

#define L_ERROR_  __LOG_START(matog::log::ERROR)	///< start error message
#define L_WARN_   __LOG_START(matog::log::WARN)		///< start warning message
#define L_INFO_   __LOG_START(matog::log::INFO)		///< start info message
#define L_DEBUG_  __LOG_START(matog::log::DEBUG)	///< start debug message
#define L_TRACE_  __LOG_START(matog::log::TRACE)	///< start trace message
#define L_INSANE_ __LOG_START(matog::log::INSANE)	///< start trace message

#define L_ERROR(...)  L_ERROR_  printf(__VA_ARGS__); __LOG_END
#define L_WARN(...)   L_WARN_   printf(__VA_ARGS__); __LOG_END
#define L_INFO(...)   L_INFO_   printf(__VA_ARGS__); __LOG_END
#define L_DEBUG(...)  L_DEBUG_  printf(__VA_ARGS__); __LOG_END
#define L_TRACE(...)  L_TRACE_  printf(__VA_ARGS__); __LOG_END
#define L_INSANE(...) L_INSANE_ printf(__VA_ARGS__); __LOG_END

#define C_ERROR_(COLOR)  __CLOG_START(matog::log::ERROR,  COLOR)	///< start error message
#define C_WARN_(COLOR)   __CLOG_START(matog::log::WARN,   COLOR)	///< start warning message
#define C_INFO_(COLOR)   __CLOG_START(matog::log::INFO,   COLOR)	///< start info message
#define C_DEBUG_(COLOR)  __CLOG_START(matog::log::DEBUG,  COLOR)	///< start debug message
#define C_TRACE_(COLOR)  __CLOG_START(matog::log::TRACE,  COLOR)	///< start trace message
#define C_INSANE_(COLOR) __CLOG_START(matog::log::INSANE, COLOR)	///< start trace message

#define C_ERROR(COLOR, ...)  C_ERROR_(COLOR)  printf(__VA_ARGS__); __LOG_END
#define C_WARN(COLOR, ...)   C_WARN_(COLOR)   printf(__VA_ARGS__); __LOG_END
#define C_INFO(COLOR, ...)   C_INFO_(COLOR)   printf(__VA_ARGS__); __LOG_END
#define C_DEBUG(COLOR, ...)  C_DEBUG_(COLOR)  printf(__VA_ARGS__); __LOG_END
#define C_TRACE(COLOR, ...)  C_TRACE_(COLOR)  printf(__VA_ARGS__); __LOG_END
#define C_INSANE(COLOR, ...) C_INSANE_(COLOR) printf(__VA_ARGS__); __LOG_END

#define L_EMPTY printf("\n")										///< print empty line with timestamp, file and line

#endif
