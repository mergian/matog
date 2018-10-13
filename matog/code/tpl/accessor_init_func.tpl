template<bool TT = T> __matog_inline__ typename std::enable_if<TT, const {{TYPE}}>::type __init_{{FIELD}}(void) { return  this->m_root->get{{>NAME}}({{>VALUES}}); }
template<bool TT = T> __matog_inline__ typename std::enable_if<!TT, {{TYPE}}&>::type     __init_{{FIELD}}(void) { return *this->m_root->ptr{{>NAME}}({{>VALUES}}); }
