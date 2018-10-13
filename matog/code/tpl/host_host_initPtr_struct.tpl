char* ptr = reinterpret_cast<char*>(m_dataPtr);

const uint64_t cnt = element_count();
uint64_t offset = 0;

{{#FIELDS}}m_ptr.soa.{{FIELD}} = reinterpret_cast<{{#IS_FIELD}}__{{FIELD}}{{/IS_FIELD}}{{#IS_TYPE}}{{TYPE}}{{/IS_TYPE}}*>(ptr + offset);
offset += align(sizeof({{#IS_FIELD}}__{{FIELD}}{{/IS_FIELD}}{{#IS_TYPE}}{{TYPE}}{{/IS_TYPE}}) * cnt);
{{/FIELDS}}