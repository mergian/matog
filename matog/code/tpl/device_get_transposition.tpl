__matog_inline__ uint32_t __transposition{{>NAME}}(void) const { return (uint32_t)__{{DEGREE}}({{PARAM}}); }

__matog_inline__ {{INTERNAL_INDEX_TYPE}} __index{{>NAME}}({{>INDEX}}) const {
	{{#COUNTS}}#define _{{CHAR}} count{{>NAME}}({{DIM}})
	{{/COUNTS}}

	switch(__transposition{{>NAME}}()) {
	{{#TRANSPOSITIONS}}case {{VALUE}}: return {{CODE}};
	{{/TRANSPOSITIONS}}
	default:;
	}
	
	{{#COUNTS}}#undef _{{CHAR}}
	{{/COUNTS}}
	
	THROW();
	return 0;
}
