__matog_inline__ operator {{VTYPE}}(void) {
	return this->m_root->get{{>NAME}}({{>VALUES}});
}

__matog_inline__ void operator=(const {{VTYPE}}& value) {
	this->m_root->set{{>NAME}}({{>VALUES}}, value);
}
