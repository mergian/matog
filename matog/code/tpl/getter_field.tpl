// getter_field
__matog_inline__ {{>TYPE}} get{{>NAME}}({{>INDEX}}) const {
	return *const_ptr{{>NAME}}({{>VALUES}});
}

__matog_inline__ void set{{>NAME}}({{>INDEX}}, const {{>TYPE}}& value) {
	*ptr{{>NAME}}({{>VALUES}}) = value;
}
