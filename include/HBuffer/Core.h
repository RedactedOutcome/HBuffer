#pragma once
#undef HBUFFER_USE_PCH
//Handles defines and importing standard library
#ifdef HBUFFER_USE_PCH
#pragma warning USING PCH
#ifndef HBUFFER_PCH_DIR
    #error HBUFFER no pch location specified
#endif

#include HBUFFER_PCH_DIR
else

#include <string.h>
#include <map>
#include <memory>
#include <algorithm>

#endif
#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <memory>
#include <algorithm>

//#if __cplusplus < 201103L
//#error HBUFFER ONLY SUPPORTS CPP11+ remind the dev to actually work on his code 
//#endif

#define HBUFF_CONSTEXPR constexpr
#define HBUFF_NOEXCEPT noexcept