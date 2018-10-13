// Copyright (c) 2015 Nicolas Weber / GCC / TU-Darmstadt. All rights reserved. 
// Use of this source code is governed by the BSD 3-Clause license that can be
// found in the LICENSE file.

#include <matog/analyze/graph/Node.h>
#include <matog/analyze/graph/Decision.h>
#include <matog/log.h>

namespace matog {
	namespace analyze {
		namespace graph {
//-------------------------------------------------------------------
uint32_t Node::s_uid = 0;

//-------------------------------------------------------------------
Node::Node(const uint32_t id, const event::Type type) : m_uid(s_uid++), m_id(id), m_type(type) {
}

//-------------------------------------------------------------------
Node::~Node(void) {
}

//-------------------------------------------------------------------
void Node::remove(void) {
	for(auto& it : m_parents) {
		Decision* edge = it.first;
		Node* parent = it.second;
		parent->m_children.erase(edge);

		if(edge->getLast() == this)
			edge->setLast(parent);
	}

	for(auto& it : m_children) {
		Decision* edge = it.first;
		Node* child = it.second;
		child->m_parents.erase(edge);
	}
}

//-------------------------------------------------------------------
void Node::exportDot(std::ostream& ss) {
	for(auto& item : m_children) {
		ss << "N" << getUID() << " -> N" << item.second->getUID() << " [label=\"" << item.first->getId() << "\"];" << std::endl;
	}
}

//-------------------------------------------------------------------
void Node::setParent(Decision* edge, Node* parent) {
	m_parents[edge] = parent;
}

//-------------------------------------------------------------------
void Node::setChild(Decision* edge, Node* child) {
	m_children[edge] = child;
}

//-------------------------------------------------------------------
Node* Node::getChild(Decision* edge) const {
	auto it = m_children.find(edge);

	if(it == m_children.end())
		return 0;

	return it->second;
}

//-------------------------------------------------------------------
Node* Node::getParent(Decision* edge) const {
	auto it = m_parents.find(edge);

	if(it == m_parents.end())
		return 0;

	return it->second;
}

//-------------------------------------------------------------------
		}
	}
}