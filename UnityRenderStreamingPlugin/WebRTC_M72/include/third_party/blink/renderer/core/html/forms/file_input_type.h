/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_FILE_INPUT_TYPE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_FILE_INPUT_TYPE_H_

#include "base/memory/scoped_refptr.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/html/forms/file_chooser.h"
#include "third_party/blink/renderer/core/html/forms/input_type.h"
#include "third_party/blink/renderer/core/html/forms/keyboard_clickable_input_type_view.h"
#include "third_party/blink/renderer/core/page/popup_opening_observer.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class DragData;
class FileList;

class CORE_EXPORT FileInputType final : public InputType,
                                        public KeyboardClickableInputTypeView,
                                        private FileChooserClient {
  USING_GARBAGE_COLLECTED_MIXIN(FileInputType);

 public:
  static InputType* Create(HTMLInputElement&);

  FileInputType(HTMLInputElement&);

  void Trace(blink::Visitor*) override;
  using InputType::GetElement;
  static Vector<String> FilesFromFormControlState(const FormControlState&);
  static FileList* CreateFileList(const FileChooserFileInfoList& files,
                                  const base::FilePath& base_dir);

  void CountUsage() override;

  void SetFilesFromPaths(const Vector<String>&) override;

 private:
  InputTypeView* CreateView() override;
  const AtomicString& FormControlType() const override;
  FormControlState SaveFormControlState() const override;
  void RestoreFormControlState(const FormControlState&) override;
  void AppendToFormData(FormData&) const override;
  bool ValueMissing(const String&) const override;
  String ValueMissingText() const override;
  void HandleDOMActivateEvent(Event&) override;
  LayoutObject* CreateLayoutObject(const ComputedStyle&) const override;
  bool CanSetStringValue() const override;
  FileList* Files() override;
  void SetFiles(FileList*) override;
  ValueMode GetValueMode() const override;
  bool CanSetValue(const String&) override;
  String ValueInFilenameValueMode() const override;
  void SetValue(const String&,
                bool value_changed,
                TextFieldEventBehavior,
                TextControlSetValueSelection) override;
  bool ReceiveDroppedFiles(const DragData*) override;
  String DroppedFileSystemId() override;
  void CreateShadowSubtree() override;
  void DisabledAttributeChanged() override;
  void MultipleAttributeChanged() override;
  String DefaultToolTip(const InputTypeView&) const override;
  void CopyNonAttributeProperties(const HTMLInputElement&) override;

  // KeyboardClickableInputTypeView overrides.
  void HandleKeypressEvent(KeyboardEvent&) override;
  void HandleKeyupEvent(KeyboardEvent&) override;

  // FileChooserClient implementation.
  void FilesChosen(FileChooserFileInfoList files,
                   const base::FilePath& base_dir) override;
  LocalFrame* FrameOrNull() const override;

  // PopupOpeningObserver implementation.
  void WillOpenPopup() override;

  void SetFilesFromDirectory(const String&);

  Member<FileList> file_list_;
  String dropped_file_system_id_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_FILE_INPUT_TYPE_H_
