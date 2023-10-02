﻿#pragma once
#include <type_traits>

#define ALL_INTEGRAL_TYPES bool, char, signed char, unsigned char, wchar_t, char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long

#define ALL_FLOATING_POINT_TYPES float, double, long double

namespace fustd {
namespace details {
#define FUSTD_DETAILS_IS_DETECTED_DECL(name, ...) \
template<template<__VA_ARGS__> class, __VA_ARGS__, class = void> \
struct name : std::false_type {}

#define FUSTD_DETAILS_IS_DETECTED_IMPL_TEMPLATE(...) \
template<template<__VA_ARGS__> class OpT, __VA_ARGS__>
#define FUSTD_DETAILS_IS_DETECTED_IMPL(name, ...) \
struct name<OpT, __VA_ARGS__, std::void_t<OpT<__VA_ARGS__>>> : std::true_type {}

FUSTD_DETAILS_IS_DETECTED_DECL(is_detected, class);
FUSTD_DETAILS_IS_DETECTED_IMPL_TEMPLATE(class T) FUSTD_DETAILS_IS_DETECTED_IMPL(is_detected, T);

FUSTD_DETAILS_IS_DETECTED_DECL(is_detected2, class, class);
FUSTD_DETAILS_IS_DETECTED_IMPL_TEMPLATE(class T1, class T2) FUSTD_DETAILS_IS_DETECTED_IMPL(is_detected2, T1, T2);

FUSTD_DETAILS_IS_DETECTED_DECL(is_detected3, class, class, class);
FUSTD_DETAILS_IS_DETECTED_IMPL_TEMPLATE(class T1, class T2, class T3) FUSTD_DETAILS_IS_DETECTED_IMPL(is_detected3, T1, T2, T3);

FUSTD_DETAILS_IS_DETECTED_DECL(is_detected4, class, class, class, class);
FUSTD_DETAILS_IS_DETECTED_IMPL_TEMPLATE(class T1, class T2, class T3, class T4) FUSTD_DETAILS_IS_DETECTED_IMPL(is_detected4, T1, T2, T3, T4);

template <class T>
struct conditional_rvalue_ref {
	using type = typename std::conditional<std::is_class<T>::value, T&&, T>::type;
};

template<class T>
struct nude {
	using type = std::decay_t<T>;
};

template<typename T>
struct nude<T[]> {
	using type = typename nude<T>::type;
};

template<typename T>
struct nude<T*> {
	using type = typename nude<T>::type;
};
}

#define FUSTD_IS_DETECTED_V_TEMPLATE(...) \
template<template<__VA_ARGS__> class OpT, __VA_ARGS__>
#define FUSTD_IS_DETECTED_V(name, ...) \
constexpr bool name##_v = details::name<OpT, __VA_ARGS__>::value

FUSTD_IS_DETECTED_V_TEMPLATE(class T) FUSTD_IS_DETECTED_V(is_detected, T);
FUSTD_IS_DETECTED_V_TEMPLATE(class T1, class T2) FUSTD_IS_DETECTED_V(is_detected2, T1, T2);
FUSTD_IS_DETECTED_V_TEMPLATE(class T1, class T2, class T3) FUSTD_IS_DETECTED_V(is_detected3, T1, T2, T3);
FUSTD_IS_DETECTED_V_TEMPLATE(class T1, class T2, class T3, class T4) FUSTD_IS_DETECTED_V(is_detected4, T1, T2, T3, T4);

template<class T, class U>
static constexpr bool is_decay_same_v = std::is_same_v<std::decay_t<T>, std::decay_t<U>>;
template<class T, class U>
static constexpr bool is_decay_convertible_v = std::is_convertible_v<std::decay_t<T>, std::decay_t<U>>;

// 判断Types是否都等于CompareT, 比如is_all_same_v<int, int, int> = true;
template<class CompareT, class... Types>
inline constexpr bool is_all_same_v = (sizeof...(Types) != 0 && (... && std::is_same_v<CompareT, Types>));
// 判断FromTypes是否都可转换为ToT, 比如is_all_convertible_v<int, float, long long> = true;
template<class ToT, class... FromTypes>
inline constexpr bool is_all_convertible_v = (sizeof...(FromTypes) != 0 && (... && std::is_convertible_v<FromTypes, ToT>));
// 判断Types的其中一个是否是CompareT, 比如is_any_of_v<int, float, int> = true;
template<class CompareT, class... Types>
inline constexpr bool is_any_of_v = (sizeof...(Types) != 0 && (... || std::is_same_v<CompareT, Types>));
// 判断FromTypes的其中一个是否可转换为ToT, 比如is_any_of_convertible_v<int, float, const char*> = true;
template<class ToT, class... FromTypes>
inline constexpr bool is_any_of_convertible_v = (sizeof...(FromTypes) != 0 && (... || std::is_convertible_v<FromTypes, ToT>));

// 如果T是可右值引用的(T是个类), 返回T&&, 否则返回T
template <class T>
using conditional_rvalue_ref_t = typename details::conditional_rvalue_ref<T>::type;

// 完全去除T的所有装饰, 比如fustd::nude_t<const volatile int*[]> = int
template<typename T>
using nude_t = typename details::nude<T>::type;

// 判断T是否是数字类型(包括浮点数)
template<class T>
inline constexpr bool is_number_v = std::is_integral_v<T> || std::is_floating_point_v<T>;
// 判断T是否是字符串(const char*, char*, char[], const char[])
template<class T>
inline constexpr bool is_charptr_v = is_any_of_v<T, const char*, char*, char[], const char[]>;
template<class T>
// 判断T是否是宽字符串(const wchar_t*, wchar_t*, wchar_t[], const wchar_t[])
inline constexpr bool is_wcharptr_v = is_any_of_v<T, const wchar_t*, wchar_t*, wchar_t[], const wchar_t[]>;
template<class T>
// 判断T是否是字符串(包括宽字符串)
inline constexpr bool is_str_v = is_charptr_v<T> || is_wcharptr_v<T>;
// 判断T是否是nullptr
template<class T>
inline constexpr bool is_nullptr_v = std::is_same_v<T, std::nullptr_t>;
// 判断T是否是结构体(如果一个struct有了类的行为, 比如成员函数, 是不算结构体的!!!)
template<class T>
inline constexpr bool is_struct_v = std::is_pod_v<T> && !is_number_v<T> && !std::is_pointer_v<T> && !std::is_array_v<T> && !is_nullptr_v<T>;
// 判断T是否是类(包括有了类行为的struct)
template<class T>
inline constexpr bool is_class_v = !std::is_pod_v<T>;
// 判断T是否是对象(结构体或者类)
template<class T>
inline constexpr bool is_object_v = is_struct_v<T> || is_class_v<T>;
// 判断T是否是指针
template<class T>
inline constexpr bool is_pointer_v = std::is_pointer_v<T>;
// 判断T是否是非字符串指针
template<class T>
inline constexpr bool is_not_str_ptr_v = is_pointer_v<T> && !is_str_v<T>;

// 类似is_all_same_v, 但是自动去除所有装饰, 比如is_decay_all_same_v<int, const int, int&> = true;
template<class CompareT, class... Types>
inline constexpr bool is_decay_all_same_v = (sizeof...(Types) != 0 && (... && is_decay_same_v<CompareT, Types>));
// 类似is_all_convertible_v, 但是自动去除所有装饰, 比如is_decay_all_convertible_v<int, const float, long long&> = true;
template<class ToT, class... FromTypes>
inline constexpr bool is_decay_all_convertible_v = (sizeof...(FromTypes) != 0 && (... && is_decay_convertible_v<FromTypes, ToT>));
// 类似is_any_of_v, 但是自动去除所有装饰, 比如is_decay_all_same_v<int, const int, float&> = true;
template<class CompareT, class... Types>
inline constexpr bool is_decay_any_of_v = (sizeof...(Types) != 0 && (... || is_decay_same_v<CompareT, Types>));
// 类似is_any_of_convertible_v, 但是自动去除所有装饰, 比如is_decay_any_of_convertible_v<int, const float&, const char*> = true;
template<class ToT, class... FromTypes>
inline constexpr bool is_decay_any_of_convertible_v = (sizeof...(FromTypes) != 0 && (... || is_decay_convertible_v<FromTypes, ToT>));

template<class T>
concept integeral_t = std::is_integral_v<T>;
}