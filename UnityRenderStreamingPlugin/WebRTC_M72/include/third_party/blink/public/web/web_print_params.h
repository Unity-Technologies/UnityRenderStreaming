/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_PRINT_PARAMS_H_
#define THIRD_PARTY_BLINK_PUBLIC_WEB_WEB_PRINT_PARAMS_H_

#include "third_party/blink/public/platform/web_rect.h"
#include "third_party/blink/public/platform/web_size.h"
#include "third_party/blink/public/web/web_print_scaling_option.h"

namespace blink {

struct WebPrintParams {
  // Specifies printable content rect in points (a point is 1/72 of an inch).
  WebRect print_content_area;

  // Specifies the selected printer default printable area details in
  // points.
  WebRect printable_area;

  // Specifies the selected printer default paper size in points.
  WebSize paper_size;

  // Specifies user selected DPI for printing.
  int printer_dpi = 72;

  // Specifies the scale factor in percent. 100 is 1:1 (default scaling).
  int scale_factor = 100;

  // Specifies whether to print PDFs as image.
  bool rasterize_pdf = false;

  // Specifies whether to reduce/enlarge/retain the print contents to fit the
  // printable area. (This is used only by plugin printing).
  WebPrintScalingOption print_scaling_option =
      kWebPrintScalingOptionFitToPrintableArea;

  // Specifies whether printing layout needs to be applied.
  bool use_printing_layout = true;

  // Specifies how many pages per sheet. This parameter is for N-up mode.
  size_t pages_per_sheet = 1;

  WebPrintParams() = default;

  WebPrintParams(const WebSize& paper_size)
      : WebPrintParams(paper_size, true) {}

  WebPrintParams(const WebSize& paper_size, bool use_printing_layout)
      : print_content_area(WebRect(0, 0, paper_size.width, paper_size.height)),
        printable_area(print_content_area),
        paper_size(paper_size),
        print_scaling_option(kWebPrintScalingOptionSourceSize),
        use_printing_layout(use_printing_layout) {}
};

}  // namespace blink

#endif
