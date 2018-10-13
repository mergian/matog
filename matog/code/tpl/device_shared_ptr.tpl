// ptr
__matog_inline__ {{TYPE}}* ptr{{>NAME}}({{>INDEX}}) {
	{{>IDX}}
	{{>LAYOUT}}
	{{>PTR}}
}

__matog_inline__ const {{TYPE}}* const_ptr{{>NAME}}({{>INDEX}}) const {
	{{>IDX}}
	{{>LAYOUT}}
	{{>CONST_PTR}}
}

