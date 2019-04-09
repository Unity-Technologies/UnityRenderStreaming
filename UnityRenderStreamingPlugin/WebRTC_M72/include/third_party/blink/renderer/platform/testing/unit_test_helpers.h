/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_UNIT_TEST_HELPERS_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TESTING_UNIT_TEST_HELPERS_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/platform/timer.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class SharedBuffer;

namespace test {

// Note: You may want to use TestingPlatformSupportWithMockScheduler to
// provides runUntilIdle() method that can work with WebURLLoaderMockFactory.
void RunPendingTasks();

// Waits for delayed task to complete or timers to fire for |delay|.
void RunDelayedTasks(TimeDelta delay);

void EnterRunLoop();
void ExitRunLoop();

void YieldCurrentThread();

// Returns Blink top directory as an absolute path, e.g.
// /src/third_party/blink.
String BlinkRootDir();

// Returns Blink LayoutTests directory as an absolute path, e.g.
// /src/third_party/WebKit/LayoutTests.
String BlinkLayoutTestsDir();

// Returns directory containing the current executable as absolute path.
String ExecutableDir();

// Returns test data absolute path for webkit_unit_tests in core, i.e.
// <blinkRootDir>/Source/core/testing/data/<relativePath>.
// It returns the top web test directory if |relativePath| was not specified.
String CoreTestDataPath(const String& relative_path = String());

// Returns test data absolute path for blink_platform_unittests, i.e.
// <blinkRootDir>/Source/platform/testing/data/<relativePath>.
// It returns the top platform test directory if |relativePath| was not
// specified.
String PlatformTestDataPath(const String& relative_path = String());

// Returns test data absolute path for accessibility unittests, i.e.
// <blinkRootDir>/renderer/modules/accessibility/testing/data/<relativePath>. It
// returns the top accessibility test directory if |relativePath| was not
// specified.
String AccessibilityTestDataPath(const String& relative_path = String());

scoped_refptr<SharedBuffer> ReadFromFile(const String& path);

class LineReader {
 public:
  LineReader(const std::string& text);
  bool GetNextLine(std::string* line);

 private:
  std::string text_;
  size_t index_;
};

}  // namespace test
}  // namespace blink

#endif
