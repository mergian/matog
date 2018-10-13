template<bool T>
class __accessor_{{>IDENT}}{{>SUPERCLASS}} {
	{{>INIT_FUNCTIONS}}
	
public:
	__matog_inline__ __accessor_{{>IDENT}}({{#PARAMS}}{{>PARAM}}{{#PARAMS_separator}}, {{/PARAMS_separator}}{{/PARAMS}}) : 
		{{#INIT}}{{>INIT}}{{#INIT_separator}},
		{{/INIT_separator}}{{/INIT}}
	{}
		
	{{>VARS}}
	{{>OPERATOR}}
};
