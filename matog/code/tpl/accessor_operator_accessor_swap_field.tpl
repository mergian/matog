{ const {{TYPE}} tmp = this->m_root->get{{>NAME}}({{>VALUES}});
this->m_root->set{{>NAME}}({{>VALUES}}, other.m_root->get{{>NAME}}({{>OTHER_VALUES}}));
other.m_root->set{{>NAME}}({{>OTHER_VALUES}}, tmp); }
