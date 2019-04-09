// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_IIR_FILTER_NODE_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_IIR_FILTER_NODE_H_

#include "third_party/blink/renderer/core/typed_arrays/array_buffer_view_helpers.h"
#include "third_party/blink/renderer/core/typed_arrays/dom_typed_array.h"
#include "third_party/blink/renderer/modules/webaudio/audio_basic_processor_handler.h"
#include "third_party/blink/renderer/modules/webaudio/audio_node.h"
#include "third_party/blink/renderer/modules/webaudio/iir_processor.h"

namespace blink {

class BaseAudioContext;
class ExceptionState;
class IIRFilterOptions;

class IIRFilterHandler : public AudioBasicProcessorHandler {
 public:
  static scoped_refptr<IIRFilterHandler> Create(
      AudioNode&,
      float sample_rate,
      const Vector<double>& feedforward_coef,
      const Vector<double>& feedback_coef,
      bool is_filter_stable);

 private:
  IIRFilterHandler(AudioNode&,
                   float sample_rate,
                   const Vector<double>& feedforward_coef,
                   const Vector<double>& feedback_coef,
                   bool is_filter_stable);
};

class IIRFilterNode : public AudioNode {
  DEFINE_WRAPPERTYPEINFO();

 public:
  static IIRFilterNode* Create(BaseAudioContext&,
                               const Vector<double>& feedforward,
                               const Vector<double>& feedback,
                               ExceptionState&);

  static IIRFilterNode* Create(BaseAudioContext*,
                               const IIRFilterOptions*,
                               ExceptionState&);

  IIRFilterNode(BaseAudioContext&,
                const Vector<double>& denominator,
                const Vector<double>& numerator,
                bool is_filter_stable);

  void Trace(blink::Visitor*) override;

  // Get the magnitude and phase response of the filter at the given
  // set of frequencies (in Hz). The phase response is in radians.
  void getFrequencyResponse(NotShared<const DOMFloat32Array> frequency_hz,
                            NotShared<DOMFloat32Array> mag_response,
                            NotShared<DOMFloat32Array> phase_response,
                            ExceptionState&);

 private:
  IIRProcessor* GetIIRFilterProcessor() const;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_WEBAUDIO_IIR_FILTER_NODE_H_
