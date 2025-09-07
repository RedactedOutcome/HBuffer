#pragma once

//Handles defines and importing standard library
#ifdef HBUFFER_USE_PCH
#ifndef HBUFFER_PCH_DIR
    #error HBUFFER no pch location specified
#endif

#include HBUFFER_PCH_DIR
#else
#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <memory>
#include <algorithm>

#endif
//#if __cplusplus < 201103L
//#error HBUFFER ONLY SUPPORTS CPP11+ remind the dev to actually work on his code 
//#endif

#ifndef HBUFF_NO_CONSTEXPR
#define HBUFF_CONSTEXPR constexpr
#endif

#ifndef HBUFF_NO_NOEXCEPT
#define HBUFF_NOEXCEPT noexcept
#endif