const uint64_t aos = sizeof(Host::__aos) * cnt;
const uint64_t soa = {{#FIELDS}}align(sizeof({{#IS_FIELD}}Host::__{{FIELD}}{{/IS_FIELD}}{{#IS_TYPE}}{{TYPE}}{{/IS_TYPE}}) * cnt){{#FIELDS_separator}} + {{/FIELDS_separator}}{{/FIELDS}};
const uint64_t aosoa = sizeof(Host::__aosoa) * (cnt / 32 + (cnt % 32 == 0 ? 0 : 1));
return std::max({aos, soa, aosoa});