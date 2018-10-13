__matog_inline__ {{TYPE}}& operator[](const {{INDEX_TYPE}} A) {
	return *ptr(A);
}

__matog_inline__ {{TYPE}} operator[](const {{INDEX_TYPE}} A) const {
	return get(A);
}