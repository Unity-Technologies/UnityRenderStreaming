/*
 * Copyright (C) 2011, 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_COMPILER_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_WTF_COMPILER_H_

#include "base/compiler_specific.h"
#include "build/build_config.h"

/* ==== Compiler features ==== */

/* OBJC_CLASS */

#ifndef OBJC_CLASS
#ifdef __OBJC__
#define OBJC_CLASS @class
#else
#define OBJC_CLASS class
#endif
#endif

/* NO_SANITIZE_UNRELATED_CAST - Disable runtime checks related to casts between
 * unrelated objects (-fsanitize=cfi-unrelated-cast or -fsanitize=vptr). */

#if defined(__clang__)
#define NO_SANITIZE_UNRELATED_CAST \
  __attribute__((no_sanitize("cfi-unrelated-cast", "vptr")))
#define NO_SANITIZE_CFI_ICALL __attribute__((no_sanitize("cfi-icall")))
#else
#define NO_SANITIZE_UNRELATED_CAST
#define NO_SANITIZE_CFI_ICALL
#endif

#if defined(COMPILER_MSVC)
#define WTF_NOINLINE __declspec(noinline)
#else
#define WTF_NOINLINE __attribute__((noinline))
#endif

#endif /* WTF_Compiler_h */
