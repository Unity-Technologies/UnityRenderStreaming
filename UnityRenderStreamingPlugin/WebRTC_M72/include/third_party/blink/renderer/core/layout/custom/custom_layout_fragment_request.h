// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_CUSTOM_LAYOUT_FRAGMENT_REQUEST_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_CUSTOM_LAYOUT_FRAGMENT_REQUEST_H_

#include "third_party/blink/renderer/core/layout/custom/custom_layout_constraints_options.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"

namespace blink {

class CustomLayoutChild;
class CustomLayoutFragment;
class LayoutBox;
class SerializedScriptValue;

// This represents a request to perform layout on a child. It is an opaque
// object from the web developers point of view.
class CustomLayoutFragmentRequest : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  CustomLayoutFragmentRequest(
      CustomLayoutChild*,
      const CustomLayoutConstraintsOptions*,
      scoped_refptr<SerializedScriptValue> constraint_data);
  ~CustomLayoutFragmentRequest() override = default;

  // Produces a CustomLayoutFragment from this CustomLayoutFragmentRequest. This
  // may fail if the underlying LayoutBox represented by the CustomLayoutChild
  // has been removed from the tree.
  CustomLayoutFragment* PerformLayout(v8::Isolate*);

  LayoutBox* GetLayoutBox() const;
  bool IsValid() const;

  void Trace(blink::Visitor*) override;

 private:
  Member<CustomLayoutChild> child_;
  Member<const CustomLayoutConstraintsOptions> options_;
  scoped_refptr<SerializedScriptValue> constraint_data_;

  DISALLOW_COPY_AND_ASSIGN(CustomLayoutFragmentRequest);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_CUSTOM_CUSTOM_LAYOUT_FRAGMENT_REQUEST_H_
