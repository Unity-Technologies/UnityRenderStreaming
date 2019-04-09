/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004, 2006, 2007, 2008, 2009, 2012 Apple Inc. All rights
 * reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_PLUGIN_ELEMENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_PLUGIN_ELEMENT_H_

#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/create_element_flags.h"
#include "third_party/blink/renderer/core/html/html_frame_owner_element.h"
#include "third_party/blink/renderer/platform/bindings/shared_persistent.h"
#include "v8/include/v8.h"

namespace blink {

class HTMLImageLoader;
class LayoutEmbeddedContent;
class LayoutEmbeddedObject;
class WebPluginContainerImpl;

enum PreferPlugInsForImagesOption {
  kShouldPreferPlugInsForImages,
  kShouldNotPreferPlugInsForImages
};

class PluginParameters {
 public:
  PluginParameters() {}
  PluginParameters(Vector<String>& param_names, Vector<String>& param_values)
      : names_(param_names), values_(param_values) {}

  const Vector<String>& Names() const;
  const Vector<String>& Values() const;
  void AppendAttribute(const Attribute&);
  void AppendNameWithValue(const String& name, const String& value);
  int FindStringInNames(const String&);

 private:
  Vector<String> names_;
  Vector<String> values_;
};

class CORE_EXPORT HTMLPlugInElement
    : public HTMLFrameOwnerElement,
      public ActiveScriptWrappable<HTMLPlugInElement> {
  USING_GARBAGE_COLLECTED_MIXIN(HTMLPlugInElement);

 public:
  ~HTMLPlugInElement() override;
  void Trace(blink::Visitor*) override;

  bool IsPlugin() const final { return true; }

  bool HasPendingActivity() const final;

  void SetFocused(bool, WebFocusType) override;
  void ResetInstance();
  // TODO(dcheng): Consider removing this, since HTMLEmbedElementLegacyCall
  // and HTMLObjectElementLegacyCall usage is extremely low.
  v8::Local<v8::Object> PluginWrapper();
  // TODO(joelhockey): Clean up PluginEmbeddedContentView and
  // OwnedEmbeddedContentView (maybe also PluginWrapper).  It would be good to
  // remove and/or rename some of these. PluginEmbeddedContentView and
  // OwnedPlugin both return the plugin that is stored as
  // HTMLFrameOwnerElement::embedded_content_view_.  However
  // PluginEmbeddedContentView will synchronously create the plugin if required
  // by calling LayoutEmbeddedContentForJSBindings. Possibly the
  // PluginEmbeddedContentView code can be inlined into PluginWrapper.
  WebPluginContainerImpl* PluginEmbeddedContentView() const;
  WebPluginContainerImpl* OwnedPlugin() const;
  bool CanProcessDrag() const;
  const String& Url() const { return url_; }

  // Public for FrameView::addPartToUpdate()
  bool NeedsPluginUpdate() const { return needs_plugin_update_; }
  void SetNeedsPluginUpdate(bool needs_plugin_update) {
    needs_plugin_update_ = needs_plugin_update;
  }
  void UpdatePlugin();

  bool ShouldAccelerate() const;

  ParsedFeaturePolicy ConstructContainerPolicy(
      Vector<String>* /* messages */) const override;

 protected:
  HTMLPlugInElement(const QualifiedName& tag_name,
                    Document&,
                    const CreateElementFlags,
                    PreferPlugInsForImagesOption);

  // Node functions:
  void RemovedFrom(ContainerNode& insertion_point) override;
  void DidMoveToNewDocument(Document& old_document) override;
  void AttachLayoutTree(AttachContext&) override;

  // Element functions:
  bool IsPresentationAttribute(const QualifiedName&) const override;
  void CollectStyleForPresentationAttribute(
      const QualifiedName&,
      const AtomicString&,
      MutableCSSPropertyValueSet*) override;

  virtual bool HasFallbackContent() const;
  virtual bool UseFallbackContent() const;
  // Create or update the LayoutEmbeddedContent and return it, triggering layout
  // if necessary.
  virtual LayoutEmbeddedContent* LayoutEmbeddedContentForJSBindings() const;

  bool IsImageType() const;
  LayoutEmbeddedObject* GetLayoutEmbeddedObject() const;
  bool AllowedToLoadFrameURL(const String& url);
  bool RequestObject(const PluginParameters& plugin_params);

  void DispatchErrorEvent();
  bool IsErrorplaceholder();
  void LazyReattachIfNeeded();

  void SetUrl(const String& url) {
    url_ = url;
    UpdateServiceTypeIfEmpty();
  }

  void SetServiceType(const String& service_type) {
    service_type_ = service_type;
    UpdateServiceTypeIfEmpty();
  }

  String service_type_;
  String url_;
  KURL loaded_url_;
  Member<HTMLImageLoader> image_loader_;
  bool is_delaying_load_event_;

 private:
  // EventTarget overrides:
  void RemoveAllEventListeners() final;

  // Node overrides:
  bool CanContainRangeEndPoint() const override { return false; }
  bool CanStartSelection() const override;
  bool WillRespondToMouseClickEvents() final;
  void DefaultEventHandler(Event&) final;
  void DetachLayoutTree(const AttachContext& = AttachContext()) final;
  void FinishParsingChildren() final;

  // Element overrides:
  LayoutObject* CreateLayoutObject(const ComputedStyle&) override;
  bool SupportsFocus() const final { return true; }
  bool IsFocusableStyle() const final;
  bool IsKeyboardFocusable() const final;
  void DidAddUserAgentShadowRoot(ShadowRoot&) final;
  scoped_refptr<ComputedStyle> CustomStyleForLayoutObject() final;

  // HTMLElement overrides:
  bool HasCustomFocusLogic() const override;
  bool IsPluginElement() const final;

  // HTMLFrameOwnerElement overrides:
  void DisconnectContentFrame() override;
  void IntrinsicSizingInfoChanged() final;

  // Return any existing LayoutEmbeddedContent without triggering relayout, or 0
  // if it doesn't yet exist.
  virtual LayoutEmbeddedContent* ExistingLayoutEmbeddedContent() const = 0;
  virtual void UpdatePluginInternal() = 0;

  bool LoadPlugin(const KURL&,
                  const String& mime_type,
                  const PluginParameters& plugin_params,
                  bool use_fallback);
  // Perform checks after we have determined that a plugin will be used to
  // show the object (i.e after allowedToLoadObject).
  bool AllowedToLoadPlugin(const KURL&, const String& mime_type);
  // Perform checks based on the URL and MIME-type of the object to load.
  bool AllowedToLoadObject(const KURL&, const String& mime_type);

  enum class ObjectContentType {
    kNone,
    kImage,
    kFrame,
    kPlugin,
  };
  ObjectContentType GetObjectContentType() const;

  void SetPersistedPlugin(WebPluginContainerImpl*);

  bool RequestObjectInternal(const PluginParameters& plugin_params);

  void UpdateServiceTypeIfEmpty();

  v8::Global<v8::Object> plugin_wrapper_;
  bool needs_plugin_update_;
  bool should_prefer_plug_ins_for_images_;
  // Represents |layoutObject() && layoutObject()->isEmbeddedObject() &&
  // !layoutEmbeddedItem().showsUnavailablePluginIndicator()|.  We want to
  // avoid accessing |layoutObject()| in layoutObjectIsFocusable().
  bool plugin_is_available_ = false;

  // Normally the plugin is stored in
  // HTMLFrameOwnerElement::embedded_content_view. However, plugins can persist
  // even when not rendered. In order to prevent confusing code which may assume
  // that OwnedEmbeddedContentView() != null means the frame is active, we save
  // off embedded_content_view_ here while the plugin is persisting but not
  // being displayed.
  Member<WebPluginContainerImpl> persisted_plugin_;

  bool handled_externally_ = false;
};

inline bool IsHTMLPlugInElement(const HTMLElement& element) {
  return element.IsPluginElement();
}

DEFINE_HTMLELEMENT_TYPE_CASTS_WITH_FUNCTION(HTMLPlugInElement);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_PLUGIN_ELEMENT_H_
