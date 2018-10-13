{{#DIMS}}#define _{{DIM}} {{DIM}}
{{/DIMS}}

return calc_size_by_elements({{EQUATION}});

{{#DIMS}}#undef _{{DIM}}
{{/DIMS}}