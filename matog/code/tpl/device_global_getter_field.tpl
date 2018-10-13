// device_global_getter_field
__matog_inline__ {{>TYPE}} get{{>NAME}}({{>INDEX}}) const {
	if(__memory() == 1) // Texture
		return __{{GLOBAL_NAME:x-uppercase}}_TEX<{{>TYPE}}>(PID, const_ptr{{>NAME}}({{>VALUES}}), (const {{>TYPE}}*)m_ptr{{#IS_STRUCT}}.aos{{/IS_STRUCT}});
		
	return *const_ptr{{>NAME}}({{>VALUES}});
}

__matog_inline__ void set{{>NAME}}({{>INDEX}}, const {{>TYPE}}& value) {
	*ptr{{>NAME}}({{>VALUES}}) = value;
}
