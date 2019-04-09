// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_MOCK_FILE_CHOOSER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_MOCK_FILE_CHOOSER_H_

#include "mojo/public/cpp/bindings/binding_set.h"
#include "services/service_manager/public/cpp/interface_provider.h"
#include "third_party/blink/public/mojom/choosers/file_chooser.mojom-blink.h"

namespace blink {

class MockFileChooser : public mojom::blink::FileChooser {
  using FileChooser = mojom::blink::FileChooser;
  using FileChooserFileInfoPtr = mojom::blink::FileChooserFileInfoPtr;
  using FileChooserParamsPtr = mojom::blink::FileChooserParamsPtr;
  using FileChooserRequest = mojom::blink::FileChooserRequest;

 public:
  // |reached_callback| is called when OpenFileChooser() or
  // |EnumerateChosenDirectory() is called.
  MockFileChooser(service_manager::InterfaceProvider* provider,
                  base::OnceClosure reached_callback)
      : provider_(provider), reached_callback_(std::move(reached_callback)) {
    service_manager::InterfaceProvider::TestApi test_api(provider);
    test_api.SetBinderForName(
        FileChooser::Name_,
        WTF::BindRepeating(&MockFileChooser::OnFileChooserRequest,
                           WTF::Unretained(this)));
  }

  ~MockFileChooser() override {
    service_manager::InterfaceProvider::TestApi test_api(provider_);
    test_api.ClearBinderForName(FileChooser::Name_);
  }

  void ResponseOnOpenFileChooser(FileChooserFileInfoList files) {
    DCHECK(callback_)
        << "OpenFileChooser() or EnumerateChosenDirectory() should "
           "be called beforehand.";
    std::move(callback_).Run(mojom::blink::FileChooserResult::New(
        std::move(files), base::FilePath()));
    bindings_.FlushForTesting();
  }

 private:
  void OnFileChooserRequest(mojo::ScopedMessagePipeHandle handle) {
    bindings_.AddBinding(this, FileChooserRequest(std::move(handle)));
  }

  void OpenFileChooser(FileChooserParamsPtr params,
                       OpenFileChooserCallback callback) override {
    DCHECK(!callback_);
    callback_ = std::move(callback);
    if (reached_callback_)
      std::move(reached_callback_).Run();
  }

  void EnumerateChosenDirectory(
      const base::FilePath& directory_path,
      EnumerateChosenDirectoryCallback callback) override {
    DCHECK(!callback_);
    callback_ = std::move(callback);
    if (reached_callback_)
      std::move(reached_callback_).Run();
  }

  service_manager::InterfaceProvider* provider_;
  mojo::BindingSet<FileChooser> bindings_;
  OpenFileChooserCallback callback_;
  FileChooserParamsPtr params_;
  base::OnceClosure reached_callback_;
};

}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_MOCK_FILE_CHOOSER_H_
