#if (__{{GLOBAL_NAME:x-uppercase}}_L % 3) == 0
struct {
	{{#CHILDREN}}
	{{PRE}}{{TYPE}} {{NAME}}[{{>ELEMENT_COUNTS}}];
	{{/CHILDREN}}
} m_ptr;
#elif (__{{GLOBAL_NAME:x-uppercase}}_L % 3) == 1
struct {
	{{#CHILDREN}}
	{{PRE}}{{TYPE}} {{NAME}};
	{{/CHILDREN}}
} m_ptr[{{>ELEMENT_COUNTS}}];
#else
struct {
	{{#CHILDREN}}
	{{PRE}}{{TYPE}} {{NAME}}[32];
	{{/CHILDREN}}
} m_ptr[({{>ELEMENT_COUNTS}}) / 32 + (({{>ELEMENT_COUNTS}}) % 32 == 0 ? 0 : 1)];
#endif