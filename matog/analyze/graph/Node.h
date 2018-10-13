// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#ifndef __MATOG_ANALYSE_GRAPH_NODE
#define __MATOG_ANALYSE_GRAPH_NODE

#include <matog/analyze.h>
#include <ostream>
#include "node/Compare.h"

namespace matog {
	namespace analyze {
		namespace graph {
//-------------------------------------------------------------------
class Node {
private:
	static uint32_t s_uid;
	const uint32_t  m_uid;
	const uint32_t	m_id;
	event::Type m_type;
	node::Map m_parents;
	node::Map m_children;

public:
	Node(const uint32_t id, const event::Type type);
	virtual ~Node(void);

	inline const node::Map&					getParents			(void) const { return m_parents; }
	inline const node::Map&					getChildren			(void) const { return m_children; }
			Node*							getChild			(Decision* edge) const;
			Node*							getParent			(Decision* edge) const;
	inline uint32_t							getId				(void) const { return m_id; }
	inline uint32_t							getUID				(void) const { return m_uid; }
	inline event::Type						getType				(void) const { return m_type; }
			
	void									setParent			(Decision* edge, Node* parent);
	void									setChild			(Decision* edge, Node* child);
	virtual void							remove				(void);
	virtual void							exportDot			(::std::ostream& ss);
	virtual void							print				(void) = 0;
};

//-------------------------------------------------------------------
		}
	}
}

#endif