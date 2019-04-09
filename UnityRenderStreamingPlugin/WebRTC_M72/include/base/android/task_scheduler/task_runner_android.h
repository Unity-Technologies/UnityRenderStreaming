// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_ANDROID_TASK_SCHEDULER_TASK_RUNNER_ANDROID_H_
#define BASE_ANDROID_TASK_SCHEDULER_TASK_RUNNER_ANDROID_H_

#include "base/android/jni_weak_ref.h"
#include "base/single_thread_task_runner.h"

namespace base {

// Native implementation backing TaskRunnerImpl.java which posts java tasks onto
// a C++ TaskRunner.
class TaskRunnerAndroid {
 public:
  explicit TaskRunnerAndroid(scoped_refptr<TaskRunner> task_runner);
  ~TaskRunnerAndroid();

  void Finalize(JNIEnv* env, const base::android::JavaRef<jobject>& caller);

  void PostTask(JNIEnv* env,
                const base::android::JavaRef<jobject>& caller,
                const base::android::JavaRef<jobject>& task);

 private:
  const scoped_refptr<TaskRunner> task_runner_;

  DISALLOW_COPY_AND_ASSIGN(TaskRunnerAndroid);
};

}  // namespace base

#endif  // BASE_ANDROID_TASK_SCHEDULER_TASK_RUNNER_ANDROID_H_
