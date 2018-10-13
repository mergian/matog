const auto cnt = element_count();

switch((matog::Layout)__layout()) {
case matog::Layout::AOS: return sizeof(Host::__aos) * cnt;
case matog::Layout::SOA: return {{#FIELDS}}align(sizeof({{#IS_FIELD}}Host::__{{FIELD}}{{/IS_FIELD}}{{#IS_TYPE}}{{TYPE}}{{/IS_TYPE}}) * cnt){{#FIELDS_separator}} + {{/FIELDS_separator}}{{/FIELDS}};
case matog::Layout::AOSOA: return sizeof(Host::__aosoa) * (cnt / 32 + (cnt % 32 == 0 ? 0 : 1));
default: ;
};

THROW();
return 0;