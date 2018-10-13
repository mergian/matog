__matog_inline__ {{TYPE}} count(const uint32_t dim) const {
	{{#DIMS}}
	if(dim == {{DIM}}) {
		{{#IS_STATIC}}return {{VALUE}};{{/IS_STATIC}}
		{{#IS_DYNAMIC}}const {{TYPE}} overwrite = __{{GLOBAL_NAME}}_COUNT(PID, {{INDEX}}u);
		if(overwrite == 0) {
			if(__{{GLOBAL_NAME}}_COUNT_REF(PID, {{INDEX}})) {
				return __{{GLOBAL_NAME}}_COUNT_VALUE(PID, {{INDEX}}u);
			} else {
				return m_cnt[{{INDEX}}];
			}
		} else {
			return overwrite;
		}{{/IS_DYNAMIC}}
	}
	{{/DIMS}}
	THROW();
	return 0;
}
