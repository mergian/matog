__matog_inline__ {{TYPE}} count{{>NAME}}(const uint32_t dim) const {
	const uint32_t counts[] = {{{#DIMS}}{{VALUE}}{{#DIMS_separator}}, {{/DIMS_separator}}{{/DIMS}}};
	assert(dim < {{DIM_COUNT}});
	return counts[dim];
}
