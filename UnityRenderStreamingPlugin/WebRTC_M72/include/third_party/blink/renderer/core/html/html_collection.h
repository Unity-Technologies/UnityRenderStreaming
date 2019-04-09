/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2011, 2012 Apple Inc. All
 * rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_COLLECTION_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_COLLECTION_H_

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/dom/live_node_list_base.h"
#include "third_party/blink/renderer/core/html/collection_items_cache.h"
#include "third_party/blink/renderer/core/html/collection_type.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

// A simple iterator based on an index number in an HTMLCollection.
// This doesn't work if the HTMLCollection is updated during iteration.
template <class CollectionType, class NodeType>
class HTMLCollectionIterator {
  STACK_ALLOCATED();

 public:
  explicit HTMLCollectionIterator(const CollectionType* collection)
      : collection_(collection) {}
  NodeType* operator*() { return collection_->item(index_); }

  void operator++() {
    if (index_ < collection_->length())
      ++index_;
  }

  bool operator!=(const HTMLCollectionIterator& other) const {
    return collection_ != other.collection_ || index_ != other.index_;
  }

  static HTMLCollectionIterator CreateEnd(const CollectionType* collection) {
    HTMLCollectionIterator iterator(collection);
    iterator.index_ = collection->length();
    return iterator;
  }

 private:
  Member<const CollectionType> collection_;
  unsigned index_ = 0;
};

// blink::HTMLCollection implements HTMLCollection IDL interface.
class CORE_EXPORT HTMLCollection : public ScriptWrappable,
                                   public LiveNodeListBase {
  DEFINE_WRAPPERTYPEINFO();
  USING_GARBAGE_COLLECTED_MIXIN(HTMLCollection);

 public:
  enum ItemAfterOverrideType {
    kOverridesItemAfter,
    kDoesNotOverrideItemAfter,
  };

  static HTMLCollection* Create(ContainerNode& base, CollectionType);
  HTMLCollection(ContainerNode& base, CollectionType, ItemAfterOverrideType);
  ~HTMLCollection() override;
  void InvalidateCache(Document* old_document = nullptr) const override;
  void InvalidateCacheForAttribute(const QualifiedName*) const;

  // DOM API
  unsigned length() const;
  Element* item(unsigned offset) const;
  virtual Element* namedItem(const AtomicString& name) const;
  bool NamedPropertyQuery(const AtomicString&, ExceptionState&);
  void NamedPropertyEnumerator(Vector<String>& names, ExceptionState&);

  // Non-DOM API
  void NamedItems(const AtomicString& name, HeapVector<Member<Element>>&) const;
  bool IsEmpty() const { return collection_items_cache_.IsEmpty(*this); }
  bool HasExactlyOneItem() const {
    return collection_items_cache_.HasExactlyOneNode(*this);
  }
  bool ElementMatches(const Element&) const;

  // CollectionIndexCache API.
  bool CanTraverseBackward() const { return !OverridesItemAfter(); }
  Element* TraverseToFirst() const;
  Element* TraverseToLast() const;
  Element* TraverseForwardToOffset(unsigned offset,
                                   Element& current_element,
                                   unsigned& current_offset) const;
  Element* TraverseBackwardToOffset(unsigned offset,
                                    Element& current_element,
                                    unsigned& current_offset) const;

  using Iterator = HTMLCollectionIterator<HTMLCollection, Element>;
  Iterator begin() const { return Iterator(this); }
  Iterator end() const { return Iterator::CreateEnd(this); }

  void Trace(blink::Visitor*) override;

 protected:
  class NamedItemCache final : public GarbageCollected<NamedItemCache> {
   public:
    static NamedItemCache* Create() {
      return MakeGarbageCollected<NamedItemCache>();
    }

    NamedItemCache();

    const HeapVector<Member<Element>>* GetElementsById(
        const AtomicString& id) const {
      auto it = id_cache_.find(id.Impl());
      if (it == id_cache_.end())
        return nullptr;
      return &it->value;
    }
    const HeapVector<Member<Element>>* GetElementsByName(
        const AtomicString& name) const {
      auto it = name_cache_.find(name.Impl());
      if (it == name_cache_.end())
        return nullptr;
      return &it->value;
    }
    void AddElementWithId(const AtomicString& id, Element* element) {
      AddElementToMap(id_cache_, id, element);
    }
    void AddElementWithName(const AtomicString& name, Element* element) {
      AddElementToMap(name_cache_, name, element);
    }

    void Trace(blink::Visitor* visitor) {
      visitor->Trace(id_cache_);
      visitor->Trace(name_cache_);
    }

   private:
    typedef HeapHashMap<StringImpl*, HeapVector<Member<Element>>>
        StringToElementsMap;
    static void AddElementToMap(StringToElementsMap& map,
                                const AtomicString& key,
                                Element* element) {
      HeapVector<Member<Element>>& vector =
          map.insert(key.Impl(), HeapVector<Member<Element>>())
              .stored_value->value;
      vector.push_back(element);
    }

    StringToElementsMap id_cache_;
    StringToElementsMap name_cache_;
  };

  bool OverridesItemAfter() const { return overrides_item_after_; }
  virtual Element* VirtualItemAfter(Element*) const;
  bool ShouldOnlyIncludeDirectChildren() const {
    return should_only_include_direct_children_;
  }
  virtual void SupportedPropertyNames(Vector<String>& names);

  virtual void UpdateIdNameCache() const;
  bool HasValidIdNameCache() const { return named_item_cache_; }

  void SetNamedItemCache(NamedItemCache* cache) const {
    DCHECK(!named_item_cache_);
    // Do not repeat registration for the same invalidation type.
    if (InvalidationType() != kInvalidateOnIdNameAttrChange)
      GetDocument().RegisterNodeListWithIdNameCache(this);
    named_item_cache_ = cache;
  }

  NamedItemCache& GetNamedItemCache() const {
    DCHECK(named_item_cache_);
    return *named_item_cache_;
  }

 private:
  void InvalidateIdNameCacheMaps(Document* old_document = nullptr) const {
    if (!HasValidIdNameCache())
      return;

    // Make sure we decrement the NodeListWithIdNameCache count from
    // the old document instead of the new one in the case the collection
    // is moved to a new document.
    UnregisterIdNameCacheFromDocument(old_document ? *old_document
                                                   : GetDocument());

    named_item_cache_.Clear();
  }

  void UnregisterIdNameCacheFromDocument(Document& document) const {
    DCHECK(HasValidIdNameCache());
    // Do not repeat unregistration for the same invalidation type.
    if (InvalidationType() != kInvalidateOnIdNameAttrChange)
      document.UnregisterNodeListWithIdNameCache(this);
  }

  const unsigned overrides_item_after_ : 1;
  const unsigned should_only_include_direct_children_ : 1;
  mutable Member<NamedItemCache> named_item_cache_;
  mutable CollectionItemsCache<HTMLCollection, Element> collection_items_cache_;
};

DEFINE_TYPE_CASTS(HTMLCollection,
                  LiveNodeListBase,
                  collection,
                  IsHTMLCollectionType(collection->GetType()),
                  IsHTMLCollectionType(collection.GetType()));

DISABLE_CFI_PERF
inline void HTMLCollection::InvalidateCacheForAttribute(
    const QualifiedName* attr_name) const {
  if (!attr_name ||
      ShouldInvalidateTypeOnAttributeChange(InvalidationType(), *attr_name))
    InvalidateCache();
  else if (*attr_name == html_names::kIdAttr ||
           *attr_name == html_names::kNameAttr)
    InvalidateIdNameCacheMaps();
}

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_HTML_COLLECTION_H_
