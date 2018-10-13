// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.


#ifndef __MATOG_OPTIONS_H
#define __MATOG_OPTIONS_H

#include <matog/dll.h>
#include <json/json.h>
#include <matog/templates/Singleton.h>

namespace matog {
//-------------------------------------------------------------------
/// Class that reads the MATOG config file
class MATOG_INTERNAL_DLL Options : private templates::Singleton<Options> {
private:
	/// Options Instance
	static Options* s_instance;

	/// Json Root Element
			Json::Value m_root;

	/// Default constructor.
			Options				(void);
	
	/// Reads the log file. Uses MATOG.JSON or value from ENV{MATOG_CONFIG}
			void				loadJson				(void);

	/// Returns a config value. Fails, if type is not compatible.
	template<typename T> T		getValue				(const std::initializer_list<const char*>& key, const T defaultValue) const;

	/// Returns a config value. Fails, if type is not compatible.
	template<typename T> T		getValue				(const std::initializer_list<const char*>& key, const T defaultValue, T (*func)(const char*)) const {
		Json::Value node = getNode(key);

		if(node.empty())
			return defaultValue;

		return func(node.asCString());
	}

	/// Returns a Json::Value for a given key
	Json::Value					getNode					(const std::initializer_list<const char*>& key) const;

	/// Prints error if necessary
	static void					errorif					(const bool isError, const std::initializer_list<const char*>& key);

public:
	/// Saves given config for a profiling id
	static	void				dbInsert				(const uint32_t profilingId);

	/// Deletes the config entry for a profiling id
	static	void				dbDelete				(const uint32_t profilingId);

	/// Deletes all config entries in the db
	static	void				dbDeleteAll				(void);

	/// Prints the config
			void				print					(void) const;
			
	/// Inits the config
	static	void				init					(void);

	/// Returns the default config file name
	static	const char*			getDefaultFileName		(void);

	/// Static body for getValue
	template<typename T> static T get(const std::initializer_list<const char*>& key, const T defaultValue) {
		return getInstance().getValue<T>(key, defaultValue);
	}

	/// Static body for getValue
	template<typename T> static T get(const std::initializer_list<const char*>& key, const T defaultValue, T (*func)(const char*)) {
		return getInstance().getValue<T>(key, defaultValue, func);
	}

	/// Requires to be friend of Singleton
	friend class templates::Singleton<Options>;
};

//-------------------------------------------------------------------
}


#endif