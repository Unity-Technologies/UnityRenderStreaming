/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DATE_TIME_CHOOSER_PARAMS_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_DATE_TIME_CHOOSER_PARAMS_H_

#include "third_party/blink/public/platform/web_rect.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_vector.h"
#include "third_party/blink/public/web/web_date_time_input_type.h"
#include "third_party/blink/public/web/web_date_time_suggestion.h"

namespace blink {

// This class conveys various information to make date/time chooser UI.
// See WebViewClient::openDateTimeChooser.
struct WebDateTimeChooserParams {
  // The type of chooser to show.
  WebDateTimeInputType type;
  // Bounding rectangle of the requester element.
  WebRect anchor_rect_in_screen;
  // The current value of the requester element as a double.
  // NaN means empty value. Should not be infinity.
  double double_value;
  // <datalist> option suggestions associated to the requester element. The
  // vector size might be 0.
  WebVector<WebDateTimeSuggestion> suggestions;
  // HTMLInputElement::min attribute value parsed in the valusAsNumber rule,
  // that is to say, milliseconds from the epoch for non-month types and
  // months from the epoch for month type. If the min attribute is missing,
  // this field has the hard minimum limit.
  double minimum;
  // Similar to minimum.
  double maximum;
  // Step value represented in milliseconds for non-month types, and
  // represetnted in months for month type.
  double step;
  // Step-base value represeted in milliseconds, or months.
  double step_base;
  // True if the requester element has required attribute.
  bool is_required;
  // True if the requester element is rendered in rtl direction.
  bool is_anchor_element_rtl;

  WebDateTimeChooserParams()
      : minimum(0),
        maximum(0),
        step(0),
        step_base(0),
        is_required(false),
        is_anchor_element_rtl(false) {}
};

}  // namespace blink

#endif
