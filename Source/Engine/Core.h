#pragma once

#include <functional>
#include <map>

#include "pch.h"
#include "Log.h"

namespace ST {
template <typename T>
using ST_REF = std::shared_ptr<T>;

template <typename T>
using ST_SCOPE = std::unique_ptr<T>;

template <typename T>
using ST_WEAK_REF = std::weak_ptr<T>;

#define ST_MAKE_REF std::make_shared

using ST_STRING = std::string;

template <class Ty, class Alloc = std::allocator<Ty>>
using ST_VECTOR = std::vector<Ty, Alloc>;

template <class Kty, class Ty, class Pr = std::less<Kty>, class Alloc = std::allocator<std::pair<const Kty, Ty>>>
using ST_MAP = std::map<Kty, Ty, Pr, Alloc>;

template <class Fty>
using ST_FUNC = std::function<Fty>;

#define ST_BIND_EVENT(func) std::bind(&func,this,std::placeholders::_1)
#define ST_FUNC std::function

}
