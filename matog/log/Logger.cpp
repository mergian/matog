// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include "Logger.h"
#include <mutex>
#include <stdio.h>
#include "Timer.h"
#include <cstring>

namespace matog {
	namespace log {
//-------------------------------------------------------------------
#ifdef WIN32
#define PRINT printf_s
#else
#define PRINT printf
#endif

/// Class to encapsule Log
struct Logger {
	/// Default Constructor
	Logger(void);

	/// Copying not allowed
	Logger(const Logger&) = delete;

	/// Prints the total application execution time
	~Logger(void);
};

//-------------------------------------------------------------------
/// mutex for multithreaded logging
std::mutex s_mutex;

/// Logger
Logger s_instance;

/// debug level
Level s_level = INFO;

/// enables color messages
bool s_useColors = true;

/// enables filename
bool s_printFilename = true;

/// enables time
bool s_printTime = true;

/// enables level
bool s_printLevel = true;

#if WIN32
/// Windows specific consoel handle
HANDLE						s_hCon;

/// Windows default console color (used to reset color)
CONSOLE_SCREEN_BUFFER_INFO	s_defaultColor;
#else
/// Linux default console color
Color						s_defaultColor;
#endif

/// Timer
Timer s_timer;

//-------------------------------------------------------------------
static const char* extractFile(const char* file) {
#ifdef WIN32
	return strrchr(file, '\\') + 1;
#else
	return strrchr(file, '/') + 1;
#endif
}

//-------------------------------------------------------------------
Logger::Logger(void) {
#ifdef WIN32
	s_hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(s_hCon, &s_defaultColor);
#else
	s_defaultColor = Color::WHITE;
#endif
}

//-------------------------------------------------------------------
Logger::~Logger(void) {
	if(isInfo()) {
		Timer::tm tm;
		s_timer.getTM(tm);

		printLevel(INFO);
		printTime(INFO);
		printFile(__FILE__, __LINE__);
		setColor(Color::TEAL);
		printf("application terminated in ");

		if(tm.days)			printf("%ud %uh %um %us %ums", tm.days, tm.hours, tm.minutes, tm.seconds, tm.milliseconds);
		else if(tm.hours)	printf("%uh %um %us %ums", tm.hours, tm.minutes, tm.seconds, tm.milliseconds);
		else if(tm.minutes)	printf("%um %us %ums", tm.minutes, tm.seconds, tm.milliseconds);
		else if(tm.seconds)	printf("%us %ums", tm.seconds, tm.milliseconds);
		else printf("%ums", tm.milliseconds);

		resetColor();
		printf("\n");
	}
}

//-------------------------------------------------------------------
static void setLevelColor(const Level level) {
	switch(level) {
	case ERROR:		setColor(RED);		break;
	case WARN:		setColor(YELLOW);	break;
	case INFO:		setColor(GREEN);	break;
	case DEBUG:		setColor(TEAL);		break;
	case TRACE:		setColor(GRAY);		break;
	case INSANE:	setColor(PINK);		break;
	}
}

//-------------------------------------------------------------------
bool MATOG_LOG_DLL isUseColors(void) {
	return s_useColors;
}

//-------------------------------------------------------------------
bool MATOG_LOG_DLL isPrintFilename(void) {
	return s_printFilename;
}

//-------------------------------------------------------------------
bool MATOG_LOG_DLL isPrintTime(void) {
	return s_printTime;
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL setUseColors(const bool value) {
	s_useColors = value;
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL setPrintFilename(const bool value) {
	s_printFilename = value;
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL setPrintTime(const bool value) {
	s_printTime = value;
}

//-------------------------------------------------------------------
double MATOG_LOG_DLL getSeconds(void) {
	return s_timer.getSeconds();
}

//-------------------------------------------------------------------
uint64_t MATOG_LOG_DLL getMicroSeconds(void) {
	return s_timer.getMicroSeconds();
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL printTime(const Level level) {
	if(!isPrintTime())
		return;

	PRINT("[");
	setLevelColor(level);
	PRINT("%9.3f", getSeconds());
	resetColor();
	PRINT("] ");
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL printFile(const char* file, const int line) {
	if(!isPrintFilename())
		return;

	char buffer[50];
	
	#ifdef WIN32
	_snprintf_s(buffer, sizeof(buffer), sizeof(buffer)-1, "%s (%u): ", extractFile(file), line);
	#else
	snprintf(buffer, sizeof(buffer)-1, "%s (%u): ", extractFile(file), line);
	#endif

	PRINT("%-50s", buffer);
	
	#undef BUFSIZE
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL setColor(const Color color) {
	if(!isUseColors() || color == Color::DEFAULT)
		return;

#ifdef WIN32
	SetConsoleTextAttribute(s_hCon, (WORD)color);
#else
	switch(color) {
		case Color::BLACK:		PRINT("\033[0;30m"); break;
		case Color::DARKBLUE:	PRINT("\033[0;34m"); break;
		case Color::DARKGREEN:	PRINT("\033[0;32m"); break;
		case Color::DARKTEAL:	PRINT("\033[0;36m"); break;
		case Color::DARKRED:	PRINT("\033[0;31m"); break;
		case Color::DARKPINK:	PRINT("\033[0;35m"); break;
		case Color::DARKYELLOW:	PRINT("\033[0;33m"); break;
		case Color::GRAY:		PRINT("\033[0;37m"); break;
		case Color::DARKGRAY:	PRINT("\033[1;30m"); break;
		case Color::BLUE:		PRINT("\033[1;34m"); break;
		case Color::GREEN:		PRINT("\033[1;32m"); break;
		case Color::TEAL:		PRINT("\033[1;36m"); break;
		case Color::RED:		PRINT("\033[1;31m"); break;
		case Color::PINK:		PRINT("\033[1;35m"); break;
		case Color::YELLOW:		PRINT("\033[1;33m"); break;
		case Color::WHITE:		PRINT("\033[1;37m"); break;
		default:				break;
	};
#endif
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL resetColor(void) {
	if(!isUseColors())
		return;

#ifdef WIN32
	SetConsoleTextAttribute(s_hCon, s_defaultColor.wAttributes);
#else
	PRINT("\033[0m");
#endif
}

//-------------------------------------------------------------------
MATOG_LOG_DLL std::mutex& getLock(void) {
	return s_mutex;
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL setLevel(const Level value) {
	s_level = value;
}

//-------------------------------------------------------------------
Level MATOG_LOG_DLL getLevel(void) {
	return s_level;
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL setPrintLevel(const bool value) {
	s_printLevel = value;	
}

//-------------------------------------------------------------------
void MATOG_LOG_DLL printLevel(const Level level) {
	if(isPrintLevel()) {
		printf("[");
		setLevelColor(level);
		
		switch(level) {
		case ERROR:		printf(" ERROR");	break;
		case WARN:		printf(" WARN ");	break;
		case INFO:		printf(" INFO ");	break;
		case DEBUG:		printf(" DEBUG");	break;
		case TRACE:		printf(" TRACE");	break;
		case INSANE:	printf("INSANE");	break;
		}

		resetColor();
		printf("]");
	}
}
//-------------------------------------------------------------------
bool MATOG_LOG_DLL isPrintLevel(void) {
	return s_printLevel;
}

//-------------------------------------------------------------------
Level MATOG_LOG_DLL getLevelFromString(const char* str) {
	if(strcmp(str, "error") == 0)	return ERROR;
	if(strcmp(str, "warn") == 0)	return WARN;
	if(strcmp(str, "info") == 0)	return INFO;
	if(strcmp(str, "debug") == 0)	return DEBUG;
	if(strcmp(str, "trace") == 0)	return TRACE;
	if(strcmp(str, "insane") == 0)	return INSANE;
	return INFO;
}

//-------------------------------------------------------------------
	}
}