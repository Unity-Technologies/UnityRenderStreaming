/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_TESTING_INTERNALS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_TESTING_INTERNALS_H_

#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/core/css/css_computed_style_declaration.h"
#include "third_party/blink/renderer/core/page/scrolling/scrolling_coordinator.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/bindings/script_state.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class Animation;
class CallbackFunctionTest;
class CanvasRenderingContext;
class DOMRect;
class DOMRectList;
class DOMRectReadOnly;
class DOMArrayBuffer;
class DOMPoint;
class DOMWindow;
class DictionaryTest;
class Document;
class DocumentMarker;
class Element;
class ExceptionState;
class ExecutionContext;
class GCObservation;
class HitTestLocation;
class HitTestResult;
class HTMLInputElement;
class HTMLMediaElement;
class HTMLSelectElement;
class HTMLVideoElement;
class InternalRuntimeFlags;
class InternalSettings;
class LayerRectList;
class LocalDOMWindow;
class LocalFrame;
class Location;
class Node;
class OriginTrialsTest;
class Page;
class Range;
class RecordTest;
class SequenceTest;
class SerializedScriptValue;
class ShadowRoot;
class StaticSelection;
class TypeConversions;
class UnionTypesTest;
class ScrollState;
template <typename NodeType>
class StaticNodeTypeList;
using StaticNodeList = StaticNodeTypeList<Node>;

class Internals final : public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static Internals* Create(ExecutionContext* context) {
    return MakeGarbageCollected<Internals>(context);
  }

  static void ResetToConsistentState(Page*);

  explicit Internals(ExecutionContext*);

  String elementLayoutTreeAsText(Element*, ExceptionState&);

  GCObservation* observeGC(ScriptValue);

  bool isPreloaded(const String& url);
  bool isPreloadedBy(const String& url, Document*);
  bool isLoading(const String& url);
  bool isLoadingFromMemoryCache(const String& url);
  int getResourcePriority(const String& url, Document*);
  String getResourceHeader(const String& url, const String& header, Document*);

  CSSStyleDeclaration* computedStyleIncludingVisitedInfo(Node*) const;

  void setBrowserControlsState(float top_height,
                               float bottom_height,
                               bool shrinks_layout);
  void setBrowserControlsShownRatio(float);

  Node* effectiveRootScroller(Document*);

  ShadowRoot* createUserAgentShadowRoot(Element* host);

  ShadowRoot* shadowRoot(Element* host);
  String shadowRootType(const Node*, ExceptionState&) const;
  bool hasShadowInsertionPoint(const Node*, ExceptionState&) const;
  bool hasContentElement(const Node*, ExceptionState&) const;
  uint32_t countElementShadow(const Node*, ExceptionState&) const;
  const AtomicString& shadowPseudoId(Element*);

  // Animation testing.
  void pauseAnimations(double pause_time, ExceptionState&);
  bool isCompositedAnimation(Animation*);
  void disableCompositedAnimation(Animation*);
  void disableCSSAdditiveAnimations();

  // Advances an animated image. For BitmapImage (e.g., animated gifs) this
  // will advance to the next frame. For SVGImage, this will trigger an
  // animation update for CSS and advance the SMIL timeline by one frame.
  void advanceImageAnimation(Element* image, ExceptionState&);

  bool isValidContentSelect(Element* insertion_point, ExceptionState&);
  Node* treeScopeRootNode(Node*);
  Node* parentTreeScope(Node*);
  unsigned short compareTreeScopePosition(const Node*,
                                          const Node*,
                                          ExceptionState&) const;

  Node* nextSiblingInFlatTree(Node*, ExceptionState&);
  Node* firstChildInFlatTree(Node*, ExceptionState&);
  Node* lastChildInFlatTree(Node*, ExceptionState&);
  Node* nextInFlatTree(Node*, ExceptionState&);
  Node* previousInFlatTree(Node*, ExceptionState&);

  unsigned updateStyleAndReturnAffectedElementCount(ExceptionState&) const;
  unsigned needsLayoutCount(ExceptionState&) const;
  unsigned hitTestCount(Document*, ExceptionState&) const;
  unsigned hitTestCacheHits(Document*, ExceptionState&) const;
  Element* elementFromPoint(Document*,
                            double x,
                            double y,
                            bool ignore_clipping,
                            bool allow_child_frame_content,
                            ExceptionState&) const;
  void clearHitTestCache(Document*, ExceptionState&) const;

  Element* innerEditorElement(Element* container, ExceptionState&) const;

  String visiblePlaceholder(Element*);
  bool isValidationMessageVisible(Element*);
  void selectColorInColorChooser(Element*, const String& color_value);
  void endColorChooser(Element*);
  bool hasAutofocusRequest(Document*);
  bool hasAutofocusRequest();
  Vector<String> formControlStateOfHistoryItem(ExceptionState&);
  void setFormControlStateOfHistoryItem(const Vector<String>&, ExceptionState&);
  DOMWindow* pagePopupWindow() const;

  DOMRectReadOnly* absoluteCaretBounds(ExceptionState&);

  DOMRectReadOnly* boundingBox(Element*);

  void setMarker(Document*, const Range*, const String&, ExceptionState&);
  unsigned markerCountForNode(Node*, const String&, ExceptionState&);
  unsigned activeMarkerCountForNode(Node*);
  Range* markerRangeForNode(Node*,
                            const String& marker_type,
                            unsigned index,
                            ExceptionState&);
  String markerDescriptionForNode(Node*,
                                  const String& marker_type,
                                  unsigned index,
                                  ExceptionState&);
  unsigned markerBackgroundColorForNode(Node*,
                                        const String& marker_type,
                                        unsigned index,
                                        ExceptionState&);
  unsigned markerUnderlineColorForNode(Node*,
                                       const String& marker_type,
                                       unsigned index,
                                       ExceptionState&);
  void addTextMatchMarker(const Range*,
                          const String& match_status,
                          ExceptionState&);
  void addCompositionMarker(const Range*,
                            const String& underline_color_value,
                            const String& thickness_value,
                            const String& background_color_value,
                            ExceptionState&);
  void addActiveSuggestionMarker(const Range*,
                                 const String& underline_color_value,
                                 const String& thickness_value,
                                 const String& background_color_value,
                                 ExceptionState&);
  void addSuggestionMarker(const Range*,
                           const Vector<String>& suggestions,
                           const String& suggestion_highlight_color_value,
                           const String& underline_color_value,
                           const String& thickness_value,
                           const String& background_color_value,
                           ExceptionState&);
  void setTextMatchMarkersActive(Node*,
                                 unsigned start_offset,
                                 unsigned end_offset,
                                 bool);
  void setMarkedTextMatchesAreHighlighted(Document*, bool);

  String viewportAsText(Document*,
                        float device_pixel_ratio,
                        int available_width,
                        int available_height,
                        ExceptionState&);

  bool elementShouldAutoComplete(Element* input_element, ExceptionState&);
  String suggestedValue(Element*, ExceptionState&);
  void setSuggestedValue(Element*, const String&, ExceptionState&);
  void setAutofilledValue(Element*, const String&, ExceptionState&);
  void setEditingValue(Element* input_element, const String&, ExceptionState&);
  void setAutofilled(Element*, bool enabled, ExceptionState&);

  Range* rangeFromLocationAndLength(Element* scope,
                                    int range_location,
                                    int range_length);
  unsigned locationFromRange(Element* scope, const Range*);
  unsigned lengthFromRange(Element* scope, const Range*);
  String rangeAsText(const Range*);

  DOMPoint* touchPositionAdjustedToBestClickableNode(int x,
                                                     int y,
                                                     int width,
                                                     int height,
                                                     Document*,
                                                     ExceptionState&);
  Node* touchNodeAdjustedToBestClickableNode(int x,
                                             int y,
                                             int width,
                                             int height,
                                             Document*,
                                             ExceptionState&);
  DOMPoint* touchPositionAdjustedToBestContextMenuNode(int x,
                                                       int y,
                                                       int width,
                                                       int height,
                                                       Document*,
                                                       ExceptionState&);
  Node* touchNodeAdjustedToBestContextMenuNode(int x,
                                               int y,
                                               int width,
                                               int height,
                                               Document*,
                                               ExceptionState&);

  int lastSpellCheckRequestSequence(Document*, ExceptionState&);
  int lastSpellCheckProcessedSequence(Document*, ExceptionState&);
  void cancelCurrentSpellCheckRequest(Document*, ExceptionState&);
  String idleTimeSpellCheckerState(Document*, ExceptionState&);
  void runIdleTimeSpellChecker(Document*, ExceptionState&);

  Vector<AtomicString> userPreferredLanguages() const;
  void setUserPreferredLanguages(const Vector<String>&);

  unsigned mediaKeysCount();
  unsigned mediaKeySessionCount();
  unsigned pausableObjectCount(Document*);
  unsigned wheelEventHandlerCount(Document*) const;
  unsigned scrollEventHandlerCount(Document*) const;
  unsigned touchStartOrMoveEventHandlerCount(Document*) const;
  unsigned touchEndOrCancelEventHandlerCount(Document*) const;
  unsigned pointerEventHandlerCount(Document*) const;

  LayerRectList* touchEventTargetLayerRects(Document*, ExceptionState&);

  bool executeCommand(Document*,
                      const String& name,
                      const String& value,
                      ExceptionState&);

  AtomicString htmlNamespace();
  Vector<AtomicString> htmlTags();
  AtomicString svgNamespace();
  Vector<AtomicString> svgTags();

  // This is used to test rect based hit testing like what's done on touch
  // screens.
  StaticNodeList* nodesFromRect(Document*,
                                int x,
                                int y,
                                int width,
                                int height,
                                bool ignore_clipping,
                                bool allow_child_frame_content,
                                ExceptionState&) const;

  bool hasSpellingMarker(Document*, int from, int length, ExceptionState&);
  bool hasGrammarMarker(Document*, int from, int length, ExceptionState&);
  void replaceMisspelled(Document*, const String&, ExceptionState&);

  bool canHyphenate(const AtomicString& locale);
  void setMockHyphenation(const AtomicString& locale);

  bool isOverwriteModeEnabled(Document*);
  void toggleOverwriteModeEnabled(Document*);

  unsigned numberOfScrollableAreas(Document*);

  bool isPageBoxVisible(Document*, int page_number);

  InternalSettings* settings() const;
  InternalRuntimeFlags* runtimeFlags() const;
  unsigned workerThreadCount() const;

  void SetDeviceProximity(Document*,
                          const String& event_type,
                          double value,
                          double min,
                          double max,
                          ExceptionState&);

  String layerTreeAsText(Document*, unsigned flags, ExceptionState&) const;
  String layerTreeAsText(Document*, ExceptionState&) const;
  String elementLayerTreeAsText(Element*,
                                unsigned flags,
                                ExceptionState&) const;
  String elementLayerTreeAsText(Element*, ExceptionState&) const;

  bool scrollsWithRespectTo(Element*, Element*, ExceptionState&);

  String scrollingStateTreeAsText(Document*) const;
  String mainThreadScrollingReasons(Document*, ExceptionState&) const;
  void markGestureScrollRegionDirty(Document*, ExceptionState&) const;
  DOMRectList* nonFastScrollableRects(Document*, ExceptionState&) const;

  void evictAllResources() const;

  unsigned numberOfLiveNodes() const;
  unsigned numberOfLiveDocuments() const;
  LocalDOMWindow* OpenDummyInspectorFrontend(const String& url);
  void CloseDummyInspectorFrontend();

  String counterValue(Element*);

  int pageNumber(Element*,
                 float page_width,
                 float page_height,
                 ExceptionState&);
  Vector<String> shortcutIconURLs(Document*) const;
  Vector<String> allIconURLs(Document*) const;

  int numberOfPages(float page_width_in_pixels,
                    float page_height_in_pixels,
                    ExceptionState&);
  String pageProperty(String, int, ExceptionState& = ASSERT_NO_EXCEPTION) const;
  String pageSizeAndMarginsInPixels(
      int,
      int,
      int,
      int,
      int,
      int,
      int,
      ExceptionState& = ASSERT_NO_EXCEPTION) const;

  float pageScaleFactor(ExceptionState&);
  void setPageScaleFactor(float scale_factor, ExceptionState&);
  void setPageScaleFactorLimits(float min_scale_factor,
                                float max_scale_factor,
                                ExceptionState&);

  bool magnifyScaleAroundAnchor(float factor, float x, float y);

  void setIsCursorVisible(Document*, bool, ExceptionState&);

  String effectivePreload(HTMLMediaElement*);
  void mediaPlayerRemoteRouteAvailabilityChanged(HTMLMediaElement*, bool);
  void mediaPlayerPlayingRemotelyChanged(HTMLMediaElement*, bool);
  void setPersistent(HTMLVideoElement*, bool);
  void forceStaleStateForMediaElement(HTMLMediaElement*, int target_state);
  bool isMediaElementSuspended(HTMLMediaElement*);
  void setMediaControlsTestMode(HTMLMediaElement*, bool);

  void registerURLSchemeAsBypassingContentSecurityPolicy(const String& scheme);
  void registerURLSchemeAsBypassingContentSecurityPolicy(
      const String& scheme,
      const Vector<String>& policy_areas);
  void removeURLSchemeRegisteredAsBypassingContentSecurityPolicy(
      const String& scheme);

  TypeConversions* typeConversions() const;
  RecordTest* recordTest() const;
  SequenceTest* sequenceTest() const;
  DictionaryTest* dictionaryTest() const;
  UnionTypesTest* unionTypesTest() const;
  OriginTrialsTest* originTrialsTest() const;
  CallbackFunctionTest* callbackFunctionTest() const;

  Vector<String> getReferencedFilePaths() const;

  void startTrackingRepaints(Document*, ExceptionState&);
  void stopTrackingRepaints(Document*, ExceptionState&);
  void updateLayoutIgnorePendingStylesheetsAndRunPostLayoutTasks(
      Node*,
      ExceptionState&);
  void forceFullRepaint(Document*, ExceptionState&);

  DOMRectList* draggableRegions(Document*, ExceptionState&);
  DOMRectList* nonDraggableRegions(Document*, ExceptionState&);

  DOMArrayBuffer* serializeObject(scoped_refptr<SerializedScriptValue>) const;
  scoped_refptr<SerializedScriptValue> deserializeBuffer(DOMArrayBuffer*) const;

  DOMArrayBuffer* serializeWithInlineWasm(ScriptValue) const;
  ScriptValue deserializeBufferContainingWasm(ScriptState*,
                                              DOMArrayBuffer*) const;

  String getCurrentCursorInfo();

  bool cursorUpdatePending() const;

  bool fakeMouseMovePending() const;

  String markerTextForListItem(Element*);

  void forceReload(bool bypass_cache);

  String getImageSourceURL(Element*);
  void forceImageReload(Element*, ExceptionState&);

  String selectMenuListText(HTMLSelectElement*);
  bool isSelectPopupVisible(Node*);
  bool selectPopupItemStyleIsRtl(Node*, int);
  int selectPopupItemStyleFontHeight(Node*, int);
  void resetTypeAheadSession(HTMLSelectElement*);

  StaticSelection* getSelectionInFlatTree(DOMWindow*, ExceptionState&);
  Node* visibleSelectionAnchorNode();
  unsigned visibleSelectionAnchorOffset();
  Node* visibleSelectionFocusNode();
  unsigned visibleSelectionFocusOffset();
  DOMRect* selectionBounds(ExceptionState&);
  String textAffinity();

  bool loseSharedGraphicsContext3D();

  void forceCompositingUpdate(Document*, ExceptionState&);

  void setZoomFactor(float);

  void setShouldRevealPassword(Element*, bool, ExceptionState&);

  ScriptPromise createResolvedPromise(ScriptState*, ScriptValue);
  ScriptPromise createRejectedPromise(ScriptState*, ScriptValue);
  ScriptPromise addOneToPromise(ScriptState*, ScriptPromise);
  ScriptPromise promiseCheck(ScriptState*,
                             long,
                             bool,
                             const ScriptValue&,
                             const String&,
                             const Vector<String>&,
                             ExceptionState&);
  ScriptPromise promiseCheckWithoutExceptionState(ScriptState*,
                                                  const ScriptValue&,
                                                  const String&,
                                                  const Vector<String>&);
  ScriptPromise promiseCheckRange(ScriptState*, long);
  ScriptPromise promiseCheckOverload(ScriptState*, Location*);
  ScriptPromise promiseCheckOverload(ScriptState*, Document*);
  ScriptPromise promiseCheckOverload(ScriptState*, Location*, long, long);

  void Trace(blink::Visitor*) override;

  void setValueForUser(HTMLInputElement*, const String&);

  void setFocused(bool);
  void setInitialFocus(bool);

  bool ignoreLayoutWithPendingStylesheets(Document*);

  void setNetworkConnectionInfoOverride(bool,
                                        const String&,
                                        const String&,
                                        unsigned long http_rtt_msec,
                                        double downlink_max_mbps,
                                        ExceptionState&);
  void setSaveDataEnabled(bool);

  void clearNetworkConnectionInfoOverride();

  unsigned countHitRegions(CanvasRenderingContext*);

  bool isInCanvasFontCache(Document*, const String&);
  unsigned canvasFontCacheMaxFonts();

  void setScrollChain(ScrollState*,
                      const HeapVector<Member<Element>>& elements,
                      ExceptionState&);

  // Schedule a forced Blink GC run (Oilpan) at the end of event loop.
  // Note: This is designed to be only used from PerformanceTests/BlinkGC to
  //       explicitly measure only Blink GC time.  Normal web tests should use
  //       gc() instead as it would trigger both Blink GC and V8 GC.
  void forceBlinkGCWithoutV8GC();

  String selectedHTMLForClipboard();
  String selectedTextForClipboard();

  void setVisualViewportOffset(int x, int y);

  // Return true if the given use counter exists for the given document.
  // |feature| must be one of the values from the WebFeature enum.
  bool isUseCounted(Document*, uint32_t feature);
  bool isCSSPropertyUseCounted(Document*, const String&);
  bool isAnimatedCSSPropertyUseCounted(Document*, const String&);
  void clearUseCounter(Document*, uint32_t feature);

  Vector<String> getCSSPropertyLonghands() const;
  Vector<String> getCSSPropertyShorthands() const;
  Vector<String> getCSSPropertyAliases() const;

  // Observes changes on Document's UseCounter. Returns a promise that is
  // resolved when |feature| is counted. When |feature| was already counted,
  // it's immediately resolved.
  ScriptPromise observeUseCounter(ScriptState*, Document*, uint32_t feature);

  // Used by the iterable<>.
  unsigned length() const { return 5; }
  int AnonymousIndexedGetter(uint32_t index) const { return index * index; }

  String unscopableAttribute();
  String unscopableMethod();

  DOMRectList* focusRingRects(Element*);
  DOMRectList* outlineRects(Element*);

  void setCapsLockState(bool enabled);

  bool setScrollbarVisibilityInScrollableArea(Node*, bool visible);

  // Translate given platform monotonic time in seconds to high resolution
  // document time in seconds
  double monotonicTimeToZeroBasedDocumentTime(double, ExceptionState&);

  // Returns the run state of the node's scroll animator (see
  // ScrollAnimatorCompositorCoordinater::RunState), or -1 if the node does not
  // have a scrollable area.
  String getScrollAnimationState(Node*) const;

  // Returns the run state of the node's programmatic scroll animator (see
  // ScrollAnimatorCompositorCoordinater::RunState), or -1 if the node does not
  // have a scrollable area.
  String getProgrammaticScrollAnimationState(Node*) const;

  // Returns the visual rect of a node's LayoutObject.
  DOMRect* visualRect(Node*);

  // Intentional crash.
  void crash();

  // Exposed for testing of inspector overlay.
  String evaluateInInspectorOverlay(const String& script);

  // Overrides if the device is low-end (low on memory).
  void setIsLowEndDevice(bool);
  // Returns if the device is low-end.
  bool isLowEndDevice() const;

  // Returns a list of the supported text encoding aliases ("UTF-8", "GBK",
  // "windows-1252", "Latin-1", "iso-8859-1", etc).
  // The order is not defined.
  Vector<String> supportedTextEncodingLabels() const;

  void simulateRasterUnderInvalidations(bool enable);

  void BypassLongCompileThresholdOnce(ExceptionState&);

  // The number of calls to update the blink lifecycle (see:
  // LocalFrameView::UpdateLifecyclePhasesInternal).
  unsigned LifecycleUpdateCount() const;

  void DisableIntersectionObserverThrottleDelay() const;

  void addEmbedderCustomElementName(const AtomicString& name, ExceptionState&);

 private:
  Document* ContextDocument() const;
  LocalFrame* GetFrame() const;
  Vector<String> IconURLs(Document*, int icon_types_mask) const;
  DOMRectList* AnnotatedRegions(Document*, bool draggable, ExceptionState&);
  void HitTestRect(HitTestLocation&,
                   HitTestResult&,
                   int x,
                   int y,
                   int width,
                   int height,
                   Document*);

  DocumentMarker* MarkerAt(Node*,
                           const String& marker_type,
                           unsigned index,
                           ExceptionState&);
  Member<InternalRuntimeFlags> runtime_flags_;
  Member<Document> document_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_TESTING_INTERNALS_H_
