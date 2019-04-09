/*
 * Copyright (C) 2014, Google Inc. All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_ACCESSIBILITY_AX_OBJECT_CACHE_IMPL_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_ACCESSIBILITY_AX_OBJECT_CACHE_IMPL_H_

#include <memory>

#include "base/macros.h"
#include "mojo/public/cpp/bindings/binding.h"
#include "third_party/blink/public/platform/modules/permissions/permission.mojom-blink.h"
#include "third_party/blink/public/platform/modules/permissions/permission_status.mojom-blink.h"
#include "third_party/blink/renderer/core/accessibility/ax_object_cache_base.h"
#include "third_party/blink/renderer/core/dom/context_lifecycle_observer.h"
#include "third_party/blink/renderer/modules/accessibility/ax_object.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/hash_set.h"
#include "ui/accessibility/ax_enums.mojom-blink.h"

namespace blink {

class AbstractInlineTextBox;
class AXRelationCache;
class HTMLAreaElement;
class LocalFrameView;

// This class should only be used from inside the accessibility directory.
class MODULES_EXPORT AXObjectCacheImpl
    : public AXObjectCacheBase,
      public mojom::blink::PermissionObserver {
 public:
  static AXObjectCache* Create(Document&);

  explicit AXObjectCacheImpl(Document&);
  ~AXObjectCacheImpl() override;
  void Trace(blink::Visitor*) override;

  Document& GetDocument() { return *document_; }
  AXObject* FocusedObject();

  void Dispose() override;

  //
  // Iterators.
  //

  AXObject::InOrderTraversalIterator InOrderTraversalBegin();
  AXObject::InOrderTraversalIterator InOrderTraversalEnd();

  void SelectionChanged(Node*) override;
  void UpdateReverseRelations(const AXObject* relation_source,
                              const Vector<String>& target_ids);
  void ChildrenChanged(Node*) override;
  void ChildrenChanged(LayoutObject*) override;
  void ChildrenChanged(AccessibleNode*) override;
  void CheckedStateChanged(Node*) override;
  void ListboxOptionStateChanged(HTMLOptionElement*) override;
  void ListboxSelectedChildrenChanged(HTMLSelectElement*) override;
  void ListboxActiveIndexChanged(HTMLSelectElement*) override;
  void LocationChanged(LayoutObject*) override;
  void RadiobuttonRemovedFromGroup(HTMLInputElement*) override;

  void Remove(AccessibleNode*) override;
  void Remove(LayoutObject*) override;
  void Remove(Node*) override;
  void Remove(AbstractInlineTextBox*) override;

  const Element* RootAXEditableElement(const Node*) override;

  // Called by a node when text or a text equivalent (e.g. alt) attribute is
  // changed.
  void TextChanged(LayoutObject*) override;
  void TextChanged(AXObject*, Node* optional_node = nullptr);
  void FocusableChanged(Element* element);
  void DocumentTitleChanged() override;
  // Called when a node has just been attached, so we can make sure we have the
  // right subclass of AXObject.
  void UpdateCacheAfterNodeIsAttached(Node*) override;
  void DidInsertChildrenOfNode(Node*) override;

  void HandleAttributeChanged(const QualifiedName& attr_name,
                              Element*) override;
  void HandleAutofillStateChanged(Element*, bool) override;
  void HandleFocusedUIElementChanged(Node* old_focused_node,
                                     Node* new_focused_node) override;
  void HandleInitialFocus() override;
  void HandleTextFormControlChanged(Node*) override;
  void HandleEditableTextContentChanged(Node*) override;
  void HandleScaleAndLocationChanged(Document*) override;
  void HandleTextMarkerDataAdded(Node* start, Node* end) override;
  void HandleValueChanged(Node*) override;
  void HandleUpdateActiveMenuOption(LayoutMenuList*, int option_index) override;
  void DidShowMenuListPopup(LayoutMenuList*) override;
  void DidHideMenuListPopup(LayoutMenuList*) override;
  void HandleLoadComplete(Document*) override;
  void HandleLayoutComplete(Document*) override;
  void HandleClicked(Node*) override;
  void HandleAttributeChanged(const QualifiedName& attr_name,
                              AccessibleNode*) override;

  void SetCanvasObjectBounds(HTMLCanvasElement*,
                             Element*,
                             const LayoutRect&) override;

  void InlineTextBoxesUpdated(LineLayoutItem) override;
  void ProcessUpdatesAfterLayout(Document&) override;

  // Called when the scroll offset changes.
  void HandleScrollPositionChanged(LocalFrameView*) override;
  void HandleScrollPositionChanged(LayoutObject*) override;

  // Called when scroll bars are added / removed (as the view resizes).
  void HandleLayoutComplete(LayoutObject*) override;
  void HandleScrolledToAnchor(const Node* anchor_node) override;

  const AtomicString& ComputedRoleForNode(Node*) override;
  String ComputedNameForNode(Node*) override;

  void OnTouchAccessibilityHover(const IntPoint&) override;

  AXObject* ObjectFromAXID(AXID id) const { return objects_.at(id); }
  AXObject* Root();

  // used for objects without backing elements
  AXObject* GetOrCreate(ax::mojom::Role);
  AXObject* GetOrCreate(AccessibleNode*);
  AXObject* GetOrCreate(LayoutObject*) override;
  AXObject* GetOrCreate(const Node*);
  AXObject* GetOrCreate(Node*);
  AXObject* GetOrCreate(AbstractInlineTextBox*);

  AXID GetAXID(Node*) override;
  Element* GetElementFromAXID(AXID) override;

  // will only return the AXObject if it already exists
  AXObject* Get(AccessibleNode*);
  AXObject* Get(const Node*) override;
  AXObject* Get(LayoutObject*);
  AXObject* Get(AbstractInlineTextBox*);

  AXObject* FirstAccessibleObjectFromNode(const Node*);

  void Remove(AXID);

  void ChildrenChanged(AXObject*, Node* node_for_relation_update = nullptr);

  void MaybeNewRelationTarget(Node* node, AXObject* obj);

  void HandleActiveDescendantChanged(Node*);
  void HandlePossibleRoleChange(Node*);
  void HandleAriaExpandedChange(Node*);
  void HandleAriaSelectedChanged(Node*);

  bool AccessibilityEnabled();
  bool InlineTextBoxAccessibilityEnabled();

  void RemoveAXID(AXObject*);

  AXID GenerateAXID() const;

  // Counts the number of times the document has been modified. Some attribute
  // values are cached as long as the modification count hasn't changed.
  int ModificationCount() const { return modification_count_; }

  void PostNotification(LayoutObject*, ax::mojom::Event);
  void PostNotification(Node*, ax::mojom::Event);
  void PostNotification(AXObject*, ax::mojom::Event);
  void MarkAXObjectDirty(AXObject*, bool subtree);

  //
  // Aria-owns support.
  //

  // Returns true if the given object's position in the tree was due to
  // aria-owns.
  bool IsAriaOwned(const AXObject*) const;

  // Returns the parent of the given object due to aria-owns.
  AXObject* GetAriaOwnedParent(const AXObject*) const;

  // Given an object that has an aria-owns attributes, and a vector of ids from
  // the value of that attribute, updates the internal state to reflect the new
  // set of children owned by this object, returning the result in
  // |ownedChildren|. The result is validated - illegal, duplicate, or cyclical
  // references have been removed.
  //
  // If one or more ids aren't found, they're added to a lookup table so that if
  // an element with that id appears later, it can be added when you call
  // updateTreeIfElementIdIsAriaOwned.
  void UpdateAriaOwns(const AXObject* owner,
                      const Vector<String>& id_vector,
                      HeapVector<Member<AXObject>>& owned_children);

  // Synchronously returns whether or not we currently have permission to
  // call AOM event listeners.
  bool CanCallAOMEventListeners() const;

  // This is called when an accessibility event is triggered and there are
  // AOM event listeners registered that would have been called.
  // Asynchronously requests permission from the user. If permission is
  // granted, it only applies to the next event received.
  void RequestAOMEventListenerPermission();

 protected:
  void PostPlatformNotification(AXObject*, ax::mojom::Event);
  void LabelChanged(Element*);

  AXObject* CreateFromRenderer(LayoutObject*);
  AXObject* CreateFromNode(Node*);
  AXObject* CreateFromInlineTextBox(AbstractInlineTextBox*);

 private:
  Member<Document> document_;
  HeapHashMap<AXID, Member<AXObject>> objects_;
  // LayoutObject and AbstractInlineTextBox are not on the Oilpan heap so we
  // do not use HeapHashMap for those mappings.
  HeapHashMap<Member<AccessibleNode>, AXID> accessible_node_mapping_;
  HashMap<LayoutObject*, AXID> layout_object_mapping_;
  HeapHashMap<Member<const Node>, AXID> node_object_mapping_;
  HashMap<AbstractInlineTextBox*, AXID> inline_text_box_object_mapping_;
  int modification_count_;

  HashSet<AXID> ids_in_use_;

  std::unique_ptr<AXRelationCache> relation_cache_;

#if DCHECK_IS_ON()
  // Verified when finalizing.
  bool has_been_disposed_ = false;
#endif

  TaskRunnerTimer<AXObjectCacheImpl> notification_post_timer_;
  HeapVector<std::pair<Member<AXObject>, ax::mojom::Event>>
      notifications_to_post_;
  void NotificationPostTimerFired(TimerBase*);

  // ContextLifecycleObserver overrides.
  void ContextDestroyed(ExecutionContext*) override;

  AXObject* FocusedImageMapUIElement(HTMLAreaElement*);

  AXID GetOrCreateAXID(AXObject*);

  void TextChanged(Node*);
  bool NodeIsTextControl(const Node*);
  AXObject* NearestExistingAncestor(Node*);

  Settings* GetSettings();

  // Start listenening for updates to the AOM accessibility event permission.
  void AddPermissionStatusListener();

  // mojom::blink::PermissionObserver implementation.
  // Called when we get an updated AOM event listener permission value from
  // the browser.
  void OnPermissionStatusChange(mojom::PermissionStatus) override;

  // When a <tr> or <td> is inserted or removed, the containing table may have
  // gained or lost rows or columns.
  void ContainingTableRowsOrColsMaybeChanged(Node*);

  // Must be called an entire subtree of accessible objects are no longer valid.
  void InvalidateTableSubtree(AXObject* subtree);

  // Whether the user has granted permission for the user to install event
  // listeners for accessibility events using the AOM.
  mojom::PermissionStatus accessibility_event_permission_;
  // The permission service, enabling us to check for event listener
  // permission.
  mojom::blink::PermissionServicePtr permission_service_;
  mojo::Binding<mojom::blink::PermissionObserver> permission_observer_binding_;

  VectorOf<Node> nodes_changed_during_layout_;
  typedef VectorOfPairs<QualifiedName, Element> AttributesChangedVector;
  AttributesChangedVector attributes_changed_during_layout_;

  DISALLOW_COPY_AND_ASSIGN(AXObjectCacheImpl);
};

// This is the only subclass of AXObjectCache.
DEFINE_TYPE_CASTS(AXObjectCacheImpl, AXObjectCache, cache, true, true);

// This will let you know if aria-hidden was explicitly set to false.
bool IsNodeAriaVisible(Node*);

}  // namespace blink

#endif
