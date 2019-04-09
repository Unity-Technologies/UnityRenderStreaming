// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_INSPECTOR_DOM_SNAPSHOT_AGENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_INSPECTOR_DOM_SNAPSHOT_AGENT_H_

#include "base/macros.h"
#include "third_party/blink/renderer/core/css/css_property_names.h"
#include "third_party/blink/renderer/core/dom/dom_node_ids.h"
#include "third_party/blink/renderer/core/inspector/inspector_base_agent.h"
#include "third_party/blink/renderer/core/inspector/protocol/DOMSnapshot.h"
#include "third_party/blink/renderer/platform/wtf/hash_map.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {

class CharacterData;
class Document;
class Element;
class InspectedFrames;
class Node;
class PaintLayer;

class CORE_EXPORT InspectorDOMSnapshotAgent final
    : public InspectorBaseAgent<protocol::DOMSnapshot::Metainfo> {
 public:
  static InspectorDOMSnapshotAgent* Create(
      InspectedFrames* inspected_frames,
      InspectorDOMDebuggerAgent* dom_debugger_agent) {
    return MakeGarbageCollected<InspectorDOMSnapshotAgent>(inspected_frames,
                                                           dom_debugger_agent);
  }

  InspectorDOMSnapshotAgent(InspectedFrames*, InspectorDOMDebuggerAgent*);
  ~InspectorDOMSnapshotAgent() override;
  void Trace(blink::Visitor*) override;

  void Restore() override;

  protocol::Response enable() override;
  protocol::Response disable() override;
  protocol::Response getSnapshot(
      std::unique_ptr<protocol::Array<String>> style_whitelist,
      protocol::Maybe<bool> include_event_listeners,
      protocol::Maybe<bool> include_paint_order,
      protocol::Maybe<bool> include_user_agent_shadow_tree,
      std::unique_ptr<protocol::Array<protocol::DOMSnapshot::DOMNode>>*
          dom_nodes,
      std::unique_ptr<protocol::Array<protocol::DOMSnapshot::LayoutTreeNode>>*
          layout_tree_nodes,
      std::unique_ptr<protocol::Array<protocol::DOMSnapshot::ComputedStyle>>*
          computed_styles) override;
  protocol::Response captureSnapshot(
      std::unique_ptr<protocol::Array<String>> computed_styles,
      std::unique_ptr<protocol::Array<protocol::DOMSnapshot::DocumentSnapshot>>*
          documents,
      std::unique_ptr<protocol::Array<String>>* strings) override;

  // InspectorInstrumentation API.
  void CharacterDataModified(CharacterData*);
  void DidInsertDOMNode(Node*);

 private:
  // Unconditionally enables the agent, even if |enabled_.Get()==true|.
  // For idempotence, call enable().
  void EnableAndReset();

  int VisitNode(Node*,
                bool include_event_listeners,
                bool include_user_agent_shadow_tree);

  int AddString(const String& string);
  void SetRare(protocol::DOMSnapshot::RareIntegerData* data,
               int index,
               int value);
  void SetRare(protocol::DOMSnapshot::RareStringData* data,
               int index,
               const String& value);
  void SetRare(protocol::DOMSnapshot::RareBooleanData* data, int index);
  void VisitDocument2(Document*);
  int VisitNode2(Node*, int parent_index);

  // Helpers for VisitContainerChildren.
  static Node* FirstChild(const Node& node,
                          bool include_user_agent_shadow_tree);
  static bool HasChildren(const Node& node,
                          bool include_user_agent_shadow_tree);
  static Node* NextSibling(const Node& node,
                           bool include_user_agent_shadow_tree);

  std::unique_ptr<protocol::Array<int>> VisitContainerChildren(
      Node* container,
      bool include_event_listeners,
      bool include_user_agent_shadow_tree);
  void VisitContainerChildren2(Node* container, int parent_index);

  // Collect LayoutTreeNodes owned by a pseudo element.
  void VisitPseudoLayoutChildren(Node* pseudo_node, int index);
  void VisitPseudoLayoutChildren2(Node* pseudo_node, int index);

  std::unique_ptr<protocol::Array<int>> VisitPseudoElements(
      Element* parent,
      int index,
      bool include_event_listeners,
      bool include_user_agent_shadow_tree);
  void VisitPseudoElements2(Element* parent, int parent_index);
  std::unique_ptr<protocol::Array<protocol::DOMSnapshot::NameValue>>
  BuildArrayForElementAttributes(Element*);
  std::unique_ptr<protocol::Array<int>> BuildArrayForElementAttributes2(Node*);

  // Adds a LayoutTreeNode for the LayoutObject to |layout_tree_nodes_| and
  // returns its index. Returns -1 if the Node has no associated LayoutObject.
  // Associates LayoutObjects under a pseudo element with the element.
  int VisitLayoutTreeNode(LayoutObject*, Node*, int node_index);
  int BuildLayoutTreeNode(LayoutObject*, Node*, int node_index);

  // Returns the index of the ComputedStyle in |computed_styles_| for the given
  // Node. Adds a new ComputedStyle if necessary, but ensures no duplicates are
  // added to |computed_styles_|. Returns -1 if the node has no values for
  // styles in |style_whitelist_|.
  int GetStyleIndexForNode(Node*);
  std::unique_ptr<protocol::Array<int>> BuildStylesForNode(Node*);

  // Traverses the PaintLayer tree in paint order to fill |paint_order_map_|.
  void TraversePaintLayerTree(Document*);
  void VisitPaintLayer(PaintLayer*);

  void GetOriginUrl(String*, const Node*);

  struct VectorStringHashTraits;
  using ComputedStylesMap = WTF::HashMap<Vector<String>,
                                         int,
                                         VectorStringHashTraits,
                                         VectorStringHashTraits>;
  using CSSPropertyWhitelist = Vector<std::pair<String, CSSPropertyID>>;
  using PaintOrderMap = WTF::HashMap<PaintLayer*, int>;
  using OriginUrlMap = WTF::HashMap<DOMNodeId, String>;

  // State of current snapshot.
  std::unique_ptr<protocol::Array<protocol::DOMSnapshot::DOMNode>> dom_nodes_;
  std::unique_ptr<protocol::Array<protocol::DOMSnapshot::LayoutTreeNode>>
      layout_tree_nodes_;
  std::unique_ptr<protocol::Array<protocol::DOMSnapshot::ComputedStyle>>
      computed_styles_;

  std::unique_ptr<protocol::Array<String>> strings_;
  WTF::HashMap<String, int> string_table_;

  std::unique_ptr<protocol::Array<protocol::DOMSnapshot::DocumentSnapshot>>
      documents_;
  std::unique_ptr<protocol::DOMSnapshot::DocumentSnapshot> document_;

  // Maps a style string vector to an index in |computed_styles_|. Used to avoid
  // duplicate entries in |computed_styles_|.
  std::unique_ptr<ComputedStylesMap> computed_styles_map_;
  std::unique_ptr<Vector<std::pair<String, CSSPropertyID>>>
      css_property_whitelist_;
  // Maps a PaintLayer to its paint order index.
  std::unique_ptr<PaintOrderMap> paint_order_map_;
  int next_paint_order_index_ = 0;
  // Maps a backend node id to the url of the script (if any) that generates
  // the corresponding node.
  std::unique_ptr<OriginUrlMap> origin_url_map_;
  using DocumentOrderMap = HeapHashMap<Member<Document>, int>;
  DocumentOrderMap document_order_map_;
  Member<InspectedFrames> inspected_frames_;
  Member<InspectorDOMDebuggerAgent> dom_debugger_agent_;
  InspectorAgentState::Boolean enabled_;
  DISALLOW_COPY_AND_ASSIGN(InspectorDOMSnapshotAgent);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_INSPECTOR_INSPECTOR_DOM_SNAPSHOT_AGENT_H_
