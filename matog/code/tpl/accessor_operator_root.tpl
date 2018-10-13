__matog_inline__ __accessor_{{>IDENT}}<false> operator[](const {{INDEX_TYPE}} {{>IDENT}}) {
	return __accessor_{{>IDENT}}<false>(this, {{>IDENT}});
}

__matog_inline__ __accessor_{{>IDENT}}<true> operator[](const {{INDEX_TYPE}} {{>IDENT}}) const {
	return __accessor_{{>IDENT}}<true>(this, {{>IDENT}});
}