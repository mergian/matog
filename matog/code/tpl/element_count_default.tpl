__matog_inline__ {{TYPE}} element_count{{>NAME}}(void) const { return {{#DIMS}}count{{>NAME}}({{DIM}}){{#DIMS_separator}} * {{/DIMS_separator}}{{/DIMS}}; }
