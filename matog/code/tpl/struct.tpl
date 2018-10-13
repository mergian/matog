struct __soa {
	{{#CHILDREN}}
	{{PRE}}{{TYPE}}* __restrict__ {{NAME}};
	{{/CHILDREN}}
};

struct __aos {
	{{#CHILDREN}}
	{{PRE}}{{TYPE}} {{NAME}};
	{{/CHILDREN}}
};

struct __aosoa {
	{{#CHILDREN}}
	{{PRE}}{{TYPE}} {{NAME}}[32];
	{{/CHILDREN}}
};
