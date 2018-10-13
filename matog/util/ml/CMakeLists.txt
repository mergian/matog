# Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved.
# Use of this source code is governed by the BSD 3-Clause license that can be
# found in the LICENSE file.

# add include files
SET(SRC 
	util/ml/Type.h
	util/ml/Type.cpp
	util/ml/Model.h
	util/ml/Model.cpp
	util/ml/DotNN.h
	util/ml/DotNN.cpp
	util/ml/NN.h
	util/ml/NN.cpp
	util/ml/MultiplyNN.h
	util/ml/MultiplyNN.cpp
	util/ml/SVM.h
	util/ml/SVM.cpp
	util/ml/None.h
	util/ml/None.cpp
)
SOURCE_GROUP("util\\ml" FILES ${SRC})
LIST(APPEND UTIL_SRC ${SRC})