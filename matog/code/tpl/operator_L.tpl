__matog_inline__ _accessor operator[](const uint64_t index) {
	return _accessor(this, index);
}

__matog_inline__ const_accessor operator[](const uint64_t index) const {
	return const_accessor(this, index);
}