CUdeviceptr ptr = m_dataPtr;

const uint64_t cnt = element_count();
uint64_t offset = 0;

{{#FIELDS}}m_ptr.{{FIELD}} = ptr + offset;
offset += align(sizeof({{#IS_FIELD}}Host::__{{FIELD}}{{/IS_FIELD}}{{#IS_TYPE}}{{TYPE}}{{/IS_TYPE}}) * cnt);
{{/FIELDS}}