/*
 *  Copyright (C) 2003, 2006 Apple Computer, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301 USA
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_XML_DOM_PARSER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_XML_DOM_PARSER_H_

#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

class Document;
class StringOrTrustedHTML;
class ExceptionState;

class DOMParser final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static DOMParser* Create(Document& document) {
    return MakeGarbageCollected<DOMParser>(document);
  }

  explicit DOMParser(Document&);

  Document* parseFromString(const StringOrTrustedHTML&,
                            const String& type,
                            ExceptionState& exception_state);
  Document* parseFromString(const StringOrTrustedHTML&, const String& type);

  void Trace(blink::Visitor*) override;

 private:
  Document* parseFromStringInternal(const String&, const String& type);

  WeakMember<Document> context_document_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_XML_DOM_PARSER_H_
