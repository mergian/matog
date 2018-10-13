template<bool TT = T>
__matog_inline__ typename std::enable_if<!TT, {{TYPE}}&>::type operator[](const {{INDEX_TYPE}} {{>IDENT}}) {
	return *this->m_root->ptr{{>NAME}}({{>VALUES}});
}

template<bool TT = T>
__matog_inline__ typename std::enable_if<TT, {{TYPE}}>::type operator[](const {{INDEX_TYPE}} {{>IDENT}}) {
	return this->m_root->get{{>NAME}}({{>VALUES}});
}