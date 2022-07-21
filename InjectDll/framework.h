// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//
#ifndef _FRAMEWORK_H
#define _FRAMEWORK_H

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <Windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>


#endif