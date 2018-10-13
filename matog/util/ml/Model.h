// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_UTIL_ML_MODEL
#define __MATOG_UTIL_ML_MODEL

#include <matog/util.h>

namespace matog {
	namespace util {
		namespace ml {
			class MATOG_UTIL_DLL Model {
				Type m_type;
			
			protected:
				Model(const Type type);

			public:
				virtual ~Model(void);

				inline	Type			getType	(void) const { return m_type; }
				virtual uint32_t		predict	(const float* data) const	= 0;
				virtual blob			save	(void) const				= 0;
				virtual void			load	(const blob model)			= 0;
				virtual void			train	(const float* input, const int32_t* output, const int cols, const int rows, const uint32_t* counts) = 0;

				static Model* create(const Type type);
			};
		}
	}
}
#endif