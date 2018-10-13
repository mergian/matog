__matog_inline__ {{TYPE}} count{{>NAME}}(void) const { {{#DIMS}}return {{#IS_STATIC}}{{VALUE}}{{/IS_STATIC}}{{#IS_DYNAMIC}}m_cnt[{{INDEX}}]{{/IS_DYNAMIC}};{{/DIMS}} }
