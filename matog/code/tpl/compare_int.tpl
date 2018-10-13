if(src.get{{>NAME}}({{>VALUES}}) != dest.get{{>NAME}}({{>VALUES}})) {
	L_ERROR("data mismatch @ {{GLOBAL_NAME}}{{>NAME}}");
	return false;
}
