template<bool T>
class __accessor_{{>IDENT}}_data{{>SUPERCLASS}} {
public:
	{{#ROOT}}typename std::conditional<T, const {{ROOT}}*, {{ROOT}}*>::type m_root;{{/ROOT}}
	const {{INDEX_TYPE}} {{>IDENT}};
	
	__matog_inline__ __accessor_{{>IDENT}}_data({{#PARAMS}}{{>PARAM}}{{#PARAMS_separator}}, {{/PARAMS_separator}}{{/PARAMS}}) : 
		{{#INIT}}{{>INIT}}{{#INIT_separator}},
		{{/INIT_separator}}{{/INIT}}
	{}
};
