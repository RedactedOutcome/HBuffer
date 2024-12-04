#pragma once
#undef HBUFFER_USE_PCH
//Handles defines and importing standard library
#ifdef HBUFFER_USE_PCH
#ifndef HBUFFER_PCH_DIR
    #error HBUFFER no pch location specified
#endif

#include HBUFFER_PCH_DIR
else

#include <string>
#include <string.h>
#include <map>
#include <memory>

#endif