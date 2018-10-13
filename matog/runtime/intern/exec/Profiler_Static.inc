private:
static shared::Profiling::Profiler												s_defaultAlgorithm;
static uint32_t																	s_profilingId;
static std::unordered_map<const shared::Kernel*, uint32_t>						s_kernelRunCounts;
static std::unordered_map<const shared::Kernel*, shared::Profiling::Profiler>	s_algorithms;
static uint32_t																	s_metricCount;
static std::unordered_map<CUdevice, std::vector<CUpti_MetricID> >				s_metricIds;
static bool																		s_logEnabled;
	
static			void						initCupti			(void);
static			void						initMetrics			(void);
static			shared::Profiling::Profiler	getAlgorithm		(const shared::Kernel& kernel);
static			shared::Profiling::Profiler	getAlgorithmByName	(const char* algorithm);

public:
static			void				init			(void);
static			void				finalize		(void);
static	inline	uint32_t			getId			(void) { return s_profilingId; }
static			void				notifyAll		(const uint32_t compilerHash);
static			uint32_t			getMetricCount	(void);
static			CUpti_MetricID*		getMetricIds	(CUdevice device);

static			Profiler*			getProfiler		(EXECUTOR_PARAMS);
static			void				setLogEnabled	(const bool enabled);
static			bool				isLogEnabled	(void);
