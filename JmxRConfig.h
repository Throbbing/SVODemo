/**
*　　　　　　　　┏┓　　　┏┓+ +
*　　　　　　　┏┛┻━━━┛┻┓ + +
*　　　　　　　┃　　　　　　　┃ 　
*　　　　　　　┃　　　━　　　┃ ++ + + +
*　　　　　　 ████━████ ┃+
*　　　　　　　┃　　　　　　　┃ +
*　　　　　　　┃　　　┻　　　┃
*　　　　　　　┃　　　　　　　┃ + +
*　　　　　　　┗━┓　　　┏━┛
*　　　　　　　　　┃　　　┃　　　　　　　　　　　
*　　　　　　　　　┃　　　┃ + + + +
*　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting　　　　　　　
*　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug　　
*　　　　　　　　　┃　　　┃
*　　　　　　　　　┃　　　┃　　+　　　　　　　　　
*　　　　　　　　　┃　 　　┗━━━┓ + +
*　　　　　　　　　┃ 　　　　　　　┣┓
*　　　　　　　　　┃ 　　　　　　　┏┛
*　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
*　　　　　　　　　　┃┫┫　┃┫┫
*　　　　　　　　　　┗┻┛　
*/
#pragma once
#include<cfloat>
#include<cassert>
#include<iostream>
#include<cstdlib>
#include<vector>
#include<sstream>
#include<cstdio>
#ifdef NDEBUG

#define Assert(expr) 0
#else
#define Assert(expr) assert(expr) 

#endif // NDEBUG


#if defined(_DEBUG ) || defined (DEBUG)

#ifndef Isnan

#define Isnan _isnan
#endif

#else

#ifndef Isnan(f)

#define Isnan(f) false

#endif

#endif

//Platform

#if defined (_WIN32)|| defined(_WIN64)
#define JmxR_IS_WINDOW

#elif defined(__APPLE__)
#define JmxR_IS_APPLE

#endif


#ifndef JmxR_POINTER_SIZE

#if defined(__amd64__) || defined(_M_X64)
#define JmxR_POINTER_SIZE 8
#elif defined(__i386__) || defined(_M_IX86)
#define JmxR_POINTER_SIZE 4
#endif

#endif

#ifndef JmxR_HAS_64_BIT_ATOMICS

#if (JmxR_POINTER_SIZE == 8)
#define JmxR_HAS_64_BIT_ATOMICS
#endif

#endif // 




#ifndef FORCEINLINE

#define FORCEINLINE __forceinline
#endif // !FORCELINE


#ifndef Align

#define Align(n) __declspec(align(n))

#endif // !Align(n)



#ifndef JmxR_EXPORT
#define JmxR_EXPORT __declspec(dllexport)
#endif // !JmxR_EXPORT

#ifndef JmxR_IMPORT
#define JmxR_IMPORT __declspec(dllimport)
#endif // !JmxR_IMPORT



//Memory Macro
#define JmxR_CACHE_LINE_SIZE 64


#pragma warning (disable : 4305) // double constant assigned to float
#pragma warning (disable : 4244) // int -> float conversion
#pragma warning (disable : 4267) // size_t -> unsigned int conversion

#ifndef _Out
#define _Out
#endif

#ifndef _In
#define _In
#endif


using s8 = __int8;
using s16 = __int16;
using s32 = __int32;
using s64 = __int64;
using u8 = unsigned __int8;
using u16 = unsigned __int16;
using u32 = unsigned __int32;
using u64 = unsigned __int64;
using f32 = float;
using f64 = double;




