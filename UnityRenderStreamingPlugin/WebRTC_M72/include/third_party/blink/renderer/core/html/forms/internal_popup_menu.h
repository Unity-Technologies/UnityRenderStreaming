// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_INTERNAL_POPUP_MENU_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_INTERNAL_POPUP_MENU_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/html/forms/popup_menu.h"
#include "third_party/blink/renderer/core/page/page_popup_client.h"

namespace blink {

class ChromeClient;
class PagePopup;
class HTMLElement;
class HTMLHRElement;
class HTMLOptGroupElement;
class HTMLOptionElement;
class HTMLSelectElement;

// InternalPopupMenu is a PopupMenu implementation for platforms other than
// macOS and Android. The UI is built with an HTML page inside a PagePopup.
class CORE_EXPORT InternalPopupMenu final : public PopupMenu,
                                            public PagePopupClient {
 public:
  static InternalPopupMenu* Create(ChromeClient*, HTMLSelectElement&);

  InternalPopupMenu(ChromeClient*, HTMLSelectElement&);
  ~InternalPopupMenu() override;
  void Trace(blink::Visitor*) override;

  void Update();

  void Dispose();

 private:
  class ItemIterationContext;
  void AddOption(ItemIterationContext&, HTMLOptionElement&);
  void AddOptGroup(ItemIterationContext&, HTMLOptGroupElement&);
  void AddSeparator(ItemIterationContext&, HTMLHRElement&);
  void AddElementStyle(ItemIterationContext&, HTMLElement&);

  // PopupMenu functions:
  void Show() override;
  void Hide() override;
  void DisconnectClient() override;
  void UpdateFromElement(UpdateReason) override;

  // PagePopupClient functions:
  void WriteDocument(SharedBuffer*) override;
  void SelectFontsFromOwnerDocument(Document&) override;
  void SetValueAndClosePopup(int, const String&) override;
  void SetValue(const String&) override;
  void ClosePopup() override;
  Element& OwnerElement() override;
  float ZoomFactor() override { return 1.0; }
  Locale& GetLocale() override;
  void DidClosePopup() override;

  Member<ChromeClient> chrome_client_;
  Member<HTMLSelectElement> owner_element_;
  PagePopup* popup_;
  bool needs_update_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_FORMS_INTERNAL_POPUP_MENU_H_
