#pragma once

//Handles defines and importing standard library
#ifdef HBUFFER_USE_PCH
#ifndef HBUFFER_PCH_DIR
    #error HBUFFER no pch location specified
#endif

#include HBUFFER_PCH_DIR
else

#include <string>
#include <map>
#include <memory>

#endif