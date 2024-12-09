#pragma once
#include<cstdio>
#define ST_LOG printf

#define ST_ERROR(...) do{ST_LOG(__VA_ARGS__); __debugbreak();}while(false)

#define ENABLE_ASSERT

#ifdef ENABLE_ASSERT
#define ST_ASSERT(condition,...) do{if(condition) {}else { ST_LOG(__VA_ARGS__); __debugbreak();}}while(false)
#else
#define ST_ASSERT(condition,...) 
#endif
