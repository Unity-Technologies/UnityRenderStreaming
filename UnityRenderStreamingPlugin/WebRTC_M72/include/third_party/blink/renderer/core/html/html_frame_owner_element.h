/*
 * Copyright (C) 2006, 2007, 2009 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_FRAME_OWNER_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_FRAME_OWNER_ELEMENT_H_

#include "third_party/blink/public/common/frame/frame_owner_element_type.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/feature_policy/feature_policy.h"
#include "third_party/blink/renderer/core/frame/dom_window.h"
#include "third_party/blink/renderer/core/frame/embedded_content_view.h"
#include "third_party/blink/renderer/core/frame/frame_owner.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/scroll/scroll_types.h"
#include "third_party/blink/renderer/platform/weborigin/security_policy.h"
#include "third_party/blink/renderer/platform/wtf/hash_counted_set.h"

namespace blink {

class ExceptionState;
class Frame;
class LayoutEmbeddedContent;
class LazyLoadFrameObserver;
class WebPluginContainerImpl;

class CORE_EXPORT HTMLFrameOwnerElement : public HTMLElement,
                                          public FrameOwner {
  USING_GARBAGE_COLLECTED_MIXIN(HTMLFrameOwnerElement);

 public:
  ~HTMLFrameOwnerElement() override;

  DOMWindow* contentWindow() const;
  Document* contentDocument() const;

  virtual void DisconnectContentFrame();

  // Most subclasses use LayoutEmbeddedContent (either LayoutEmbeddedObject or
  // LayoutIFrame) except for HTMLObjectElement and HTMLEmbedElement which may
  // return any LayoutObject when using fallback content.
  LayoutEmbeddedContent* GetLayoutEmbeddedContent() const;

  // Whether to collapse the frame owner element in the embedder document. That
  // is, to remove it from the layout as if it did not exist.
  virtual void SetCollapsed(bool) {}

  virtual FrameOwnerElementType OwnerType() const = 0;

  Document* getSVGDocument(ExceptionState&) const;

  void SetEmbeddedContentView(EmbeddedContentView*);
  EmbeddedContentView* ReleaseEmbeddedContentView();
  EmbeddedContentView* OwnedEmbeddedContentView() const {
    return embedded_content_view_;
  }

  class PluginDisposeSuspendScope {
    STACK_ALLOCATED();

   public:
    PluginDisposeSuspendScope() { suspend_count_ += 2; }
    ~PluginDisposeSuspendScope() {
      suspend_count_ -= 2;
      if (suspend_count_ == 1)
        PerformDeferredPluginDispose();
    }

   private:
    void PerformDeferredPluginDispose();

    // Low bit indicates if there are plugins to dispose.
    static int suspend_count_;

    friend class HTMLFrameOwnerElement;
  };

  // FrameOwner overrides:
  Frame* ContentFrame() const final { return content_frame_; }
  void SetContentFrame(Frame&) final;
  void ClearContentFrame() final;
  void AddResourceTiming(const ResourceTimingInfo&) final;
  void DispatchLoad() final;
  SandboxFlags GetSandboxFlags() const final { return sandbox_flags_; }
  bool CanRenderFallbackContent() const override { return false; }
  void RenderFallbackContent(Frame*) override {}
  void IntrinsicSizingInfoChanged() override {}
  AtomicString BrowsingContextContainerName() const override {
    return getAttribute(html_names::kNameAttr);
  }
  ScrollbarMode ScrollingMode() const override { return kScrollbarAuto; }
  int MarginWidth() const override { return -1; }
  int MarginHeight() const override { return -1; }
  bool AllowFullscreen() const override { return false; }
  bool AllowPaymentRequest() const override { return false; }
  bool IsDisplayNone() const override { return !embedded_content_view_; }
  AtomicString RequiredCsp() const override { return g_null_atom; }
  const ParsedFeaturePolicy& ContainerPolicy() const override;
  bool ShouldLazyLoadChildren() const final;

  // For unit tests, manually trigger the UpdateContainerPolicy method.
  void UpdateContainerPolicyForTests() { UpdateContainerPolicy(); }

  // This function is to notify ChildFrameCompositor of pointer-events changes
  // of an OOPIF.
  void PointerEventsChanged();

  void CancelPendingLazyLoad();

  void ParseAttribute(const AttributeModificationParams&) override;

  void Trace(blink::Visitor*) override;

 protected:
  HTMLFrameOwnerElement(const QualifiedName& tag_name, Document&);

  void SetSandboxFlags(SandboxFlags);

  bool LoadOrRedirectSubframe(const KURL&,
                              const AtomicString& frame_name,
                              bool replace_current_item);
  bool IsKeyboardFocusable() const override;

  void DisposePluginSoon(WebPluginContainerImpl*);
  void FrameOwnerPropertiesChanged();

  // Return the origin which is to be used for feature policy container
  // policies, as "the origin of the URL in the frame's src attribute" (see
  // https://wicg.github.io/feature-policy/#iframe-allow-attribute).
  // This method is intended to be overridden by specific frame classes.
  virtual scoped_refptr<const SecurityOrigin> GetOriginForFeaturePolicy()
      const {
    return SecurityOrigin::CreateUniqueOpaque();
  }

  // Return a feature policy container policy for this frame, based on the
  // frame attributes and the effective origin specified in the frame
  // attributes.
  virtual ParsedFeaturePolicy ConstructContainerPolicy(
      Vector<String>* /*  messages */) const = 0;

  // Update the container policy and notify the frame loader client of any
  // changes.
  void UpdateContainerPolicy(Vector<String>* messages = nullptr);

 private:
  // Intentionally private to prevent redundant checks when the type is
  // already HTMLFrameOwnerElement.
  bool IsLocal() const final { return true; }
  bool IsRemote() const final { return false; }

  bool IsFrameOwnerElement() const final { return true; }

  virtual ReferrerPolicy ReferrerPolicyAttribute() {
    return kReferrerPolicyDefault;
  }

  Member<Frame> content_frame_;
  Member<EmbeddedContentView> embedded_content_view_;
  SandboxFlags sandbox_flags_;

  ParsedFeaturePolicy container_policy_;

  Member<LazyLoadFrameObserver> lazy_load_frame_observer_;
  bool should_lazy_load_children_;
};

DEFINE_ELEMENT_TYPE_CASTS(HTMLFrameOwnerElement, IsFrameOwnerElement());

class SubframeLoadingDisabler {
  STACK_ALLOCATED();

 public:
  explicit SubframeLoadingDisabler(Node& root)
      : SubframeLoadingDisabler(&root) {}

  explicit SubframeLoadingDisabler(Node* root) : root_(root) {
    if (root_)
      DisabledSubtreeRoots().insert(root_);
  }

  ~SubframeLoadingDisabler() {
    if (root_)
      DisabledSubtreeRoots().erase(root_);
  }

  static bool CanLoadFrame(HTMLFrameOwnerElement& owner) {
    for (Node* node = &owner; node; node = node->ParentOrShadowHostNode()) {
      if (DisabledSubtreeRoots().Contains(node))
        return false;
    }
    return true;
  }

 private:
  // The use of UntracedMember<Node>  is safe as all SubtreeRootSet
  // references are on the stack and reachable in case a conservative
  // GC hits.
  // TODO(sof): go back to HeapHashSet<> once crbug.com/684551 has been
  // resolved.
  using SubtreeRootSet = HashCountedSet<UntracedMember<Node>>;

  CORE_EXPORT static SubtreeRootSet& DisabledSubtreeRoots();

  Member<Node> root_;
};

DEFINE_TYPE_CASTS(HTMLFrameOwnerElement,
                  FrameOwner,
                  owner,
                  owner->IsLocal(),
                  owner.IsLocal());

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_FRAME_OWNER_ELEMENT_H_
