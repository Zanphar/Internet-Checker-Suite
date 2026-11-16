// Internet Connection Utility
// Copyright (c) 2026 Charles McDonald. All rights reserved.
// SPDX-License-Identifier: LicenseRef-Proprietary
//
// This file is part of Internet Connection Utility, proprietary and confidential software.
// See LICENSE.md in the project root for the full terms.
// Unauthorized copying, distribution or modification is prohibited.

#pragma once
// winsock2 must precede windows.h in every translation unit.
#include <winsock2.h>
#include <windows.h>

// Entry point for the Internet Connection Utility UI.
int runApp(HINSTANCE inst, int nCmdShow);
