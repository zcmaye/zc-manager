#pragma once

#ifdef USE_REDIS_CACHE
	#include "RedisCache.hpp"
	using DataCache = RedisCache;
#else
	#include "LocalCache.hpp"
	using DataCache = LocalCache;
#endif
