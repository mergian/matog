// getter_vector
__matog_inline__ {{>TYPE}} get{{>NAME}}({{>INDEX}}) const {
	const {{>TYPE}} value = {
		{{#FIELDS}}get{{>NAME}}_{{FIELD}}({{>VALUES}}){{#FIELDS_separator}},{{/FIELDS_separator}}
		{{/FIELDS}}
	};
	return value;
}

__matog_inline__ void set{{>NAME}}({{>INDEX}}, const {{>TYPE}}& value) {
	{{#FIELDS}}set{{>NAME}}_{{FIELD}}({{>VALUES}}, value.{{VFIELD}});
	{{/FIELDS}}
}

