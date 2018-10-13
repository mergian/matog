__matog_inline__ {{TYPE}} count(const uint32_t dim) const {
	{{#DIMS}}
	if(dim == {{DIM}}) return {{#IS_STATIC}}{{VALUE}}{{/IS_STATIC}}{{#IS_DYNAMIC}}m_cnt[{{INDEX}}]{{/IS_DYNAMIC}};
	{{/DIMS}}
	THROW();
	return 0;
}
