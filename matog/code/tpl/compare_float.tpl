{
	const auto sValue = src.get{{>NAME}}({{>VALUES}});
	const auto dValue = dest.get{{>NAME}}({{>VALUES}});
	const auto diff   = std::abs(sValue - dValue);
	if(diff > 0.001) {
		L_ERROR("data mismatch @ {{GLOBAL_NAME}}{{>NAME}}");
		return false;
	}
}
