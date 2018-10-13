inline {{TYPE}}{{REF}} operator[](const {{INDEX_TYPE}} index) {
	return get{{NAME:x-camelcase}}(index);
}

inline {{TYPE}} operator[](const {{INDEX_TYPE}} index) const {
	return get{{NAME:x-camelcase}}(index);
}