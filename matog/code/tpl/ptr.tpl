// ptr
__matog_inline__ {{TYPE}}* ptr{{>NAME}}({{>INDEX}}) {
	{{>IDX}}
	{{>LAYOUT}}
	{{>PTR}}
	{{#IS_THROW}}
	THROW();
	return 0;{{/IS_THROW}}
}

__matog_inline__ const {{TYPE}}* const_ptr{{>NAME}}({{>INDEX}}) const {
	{{>IDX}}
	{{>LAYOUT}}
	{{>CONST_PTR}}
	{{#IS_THROW}}
	THROW();
	return 0;{{/IS_THROW}}
}

