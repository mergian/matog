__matog_inline__ {{TYPE}} element_count{{>NAME}}(void) const { 
	{{#DIMS}}#define _{{CHAR}} count{{>NAME}}({{DIM}})
	{{/DIMS}}
	
	return {{EQUATION}};
	
	{{#DIMS}}#undef _{{CHAR}}
	{{/DIMS}}
}
