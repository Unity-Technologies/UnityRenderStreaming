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
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_DATE_TIME_FORMAT_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_DATE_TIME_FORMAT_H_

#include "base/macros.h"
#include "third_party/blink/renderer/platform/heap/handle.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/forward.h"

namespace blink {

// DateTimeFormat parses date time format defined in Unicode Technical
// standard 35, Locale Data Markup Language (LDML)[1].
// [1] LDML http://unicode.org/reports/tr35/tr35-6.html#Date_Format_Patterns
class PLATFORM_EXPORT DateTimeFormat {
  STATIC_ONLY(DateTimeFormat);

 public:
  enum FieldType {
    kFieldTypeInvalid,
    kFieldTypeLiteral,

    // Era: AD
    kFieldTypeEra = 'G',

    // Year: 1996
    kFieldTypeYear = 'y',
    kFieldTypeYearOfWeekOfYear = 'Y',
    kFieldTypeExtendedYear = 'u',

    // Quater: Q2
    kFieldTypeQuater = 'Q',
    kFieldTypeQuaterStandAlone = 'q',

    // Month: September
    kFieldTypeMonth = 'M',
    kFieldTypeMonthStandAlone = 'L',

    // Week: 42
    kFieldTypeWeekOfYear = 'w',
    kFieldTypeWeekOfMonth = 'W',

    // Day: 12
    kFieldTypeDayOfMonth = 'd',
    kFieldTypeDayOfYear = 'D',
    kFieldTypeDayOfWeekInMonth = 'F',
    kFieldTypeModifiedJulianDay = 'g',

    // Week Day: Tuesday
    kFieldTypeDayOfWeek = 'E',
    kFieldTypeLocalDayOfWeek = 'e',
    kFieldTypeLocalDayOfWeekStandAlon = 'c',

    // Period: AM or PM
    kFieldTypePeriod = 'a',

    // Hour: 7
    kFieldTypeHour12 = 'h',
    kFieldTypeHour23 = 'H',
    kFieldTypeHour11 = 'K',
    kFieldTypeHour24 = 'k',

    // Minute: 59
    kFieldTypeMinute = 'm',

    // Second: 12
    kFieldTypeSecond = 's',
    kFieldTypeFractionalSecond = 'S',
    kFieldTypeMillisecondsInDay = 'A',

    // Zone: PDT
    kFieldTypeZone = 'z',
    kFieldTypeRFC822Zone = 'Z',
    kFieldTypeNonLocationZone = 'v',
  };

  class TokenHandler {
    STACK_ALLOCATED();

   public:
    TokenHandler() = default;
    virtual ~TokenHandler() = default;
    virtual void VisitField(FieldType, int number_of_pattern_characters) = 0;
    virtual void VisitLiteral(const String&) = 0;

    DISALLOW_COPY_AND_ASSIGN(TokenHandler);
  };

  // Returns true if succeeded, false if failed.
  static bool Parse(const String&, TokenHandler&);
  static void QuoteAndappend(const String&, StringBuilder&);
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_TEXT_DATE_TIME_FORMAT_H_
