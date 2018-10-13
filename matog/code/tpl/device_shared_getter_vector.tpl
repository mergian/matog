// getter_vector
__matog_inline__ {{>TYPE}} get{{>NAME}}({{>INDEX}}) const {
	#if __{{GLOBAL_NAME:x-uppercase}}_L == {{VALUE}}
		return *(const {{>TYPE}}*)const_ptr{{>NAME}}_{{AOS_FIELD}}({{>VALUES}});	
	#else
		const {{>TYPE}} value = {
			{{#FIELDS}}get{{>NAME}}_{{FIELD}}({{>VALUES}}){{#FIELDS_separator}},{{/FIELDS_separator}}
			{{/FIELDS}}
		};
		return value;
	#endif
}

__matog_inline__ void set{{>NAME}}({{>INDEX}}, const {{>TYPE}}& value) {
	#if __{{GLOBAL_NAME:x-uppercase}}_L == {{VALUE}}
		*({{>TYPE}}*)ptr{{>NAME}}_{{AOS_FIELD}}({{>VALUES}}) = value;
	#else
		{{#FIELDS}}set{{>NAME}}_{{FIELD}}({{>VALUES}}, value.{{VFIELD}});
		{{/FIELDS}}
	#endif
}

