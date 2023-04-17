// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCKAPI_
#include <windows.h>
#include <winsock.h>
#include <WinSock2.h>
#include <iostream>
#include <unordered_map>
#include <map>
#include <thread>
#include <cmath>
#include <algorithm>
#include <vector>
#include <gl/GL.h>
#include <random>
#include <ranges>
#include <shared_mutex>
#include <iomanip>
#include <sstream>
#include <format>
#include "jni.h"

#endif //PCH_H
