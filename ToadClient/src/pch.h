// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#define _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCKAPI_
#include <windows.h>
#include <winsock.h>
#include <WinSock2.h>
#include <shlobj_core.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <thread>
#include <cmath>
#include <algorithm>
#include <array>
#include <vector>
#include <gl/GL.h>
#include <random>
#include <ranges>
#include <shared_mutex>
#include <iomanip>
#include <sstream>
#include <format>
#include <fstream>
#include <queue>

#include "jni.h"
#include <jvmti.h>

#endif //PCH_H
