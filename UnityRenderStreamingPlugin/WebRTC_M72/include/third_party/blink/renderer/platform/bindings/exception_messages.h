/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_EXCEPTION_MESSAGES_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_EXCEPTION_MESSAGES_H_

#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/wtf/allocator.h"
#include "third_party/blink/renderer/platform/wtf/math_extras.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

namespace blink {

class Decimal;

class PLATFORM_EXPORT ExceptionMessages {
  STATIC_ONLY(ExceptionMessages);

 public:
  enum BoundType {
    kInclusiveBound,
    kExclusiveBound,
  };

  static String ArgumentNullOrIncorrectType(int argument_index,
                                            const String& expected_type);
  static String ConstructorNotCallableAsFunction(const char* type);

  static String FailedToConvertJSValue(const char* type);

  static String FailedToConstruct(const char* type, const String& detail);
  static String FailedToEnumerate(const char* type, const String& detail);
  static String FailedToExecute(const char* method,
                                const char* type,
                                const String& detail);
  static String FailedToGet(const char* property,
                            const char* type,
                            const String& detail);
  static String FailedToSet(const char* property,
                            const char* type,
                            const String& detail);
  static String FailedToDelete(const char* property,
                               const char* type,
                               const String& detail);
  static String FailedToGetIndexed(const char* type, const String& detail);
  static String FailedToSetIndexed(const char* type, const String& detail);
  static String FailedToDeleteIndexed(const char* type, const String& detail);

  template <typename NumType>
  static String FormatNumber(NumType number) {
    return FormatFiniteNumber(number);
  }

  static String IncorrectPropertyType(const String& property,
                                      const String& detail);

  template <typename NumberType>
  static String IndexExceedsMaximumBound(const char* name,
                                         NumberType given,
                                         NumberType bound) {
    bool eq = given == bound;
    StringBuilder result;
    result.Append("The ");
    result.Append(name);
    result.Append(" provided (");
    result.Append(FormatNumber(given));
    result.Append(") is greater than ");
    result.Append(eq ? "or equal to " : "");
    result.Append("the maximum bound (");
    result.Append(FormatNumber(bound));
    result.Append(").");
    return result.ToString();
  }

  template <typename NumberType>
  static String IndexExceedsMinimumBound(const char* name,
                                         NumberType given,
                                         NumberType bound) {
    bool eq = given == bound;
    StringBuilder result;
    result.Append("The ");
    result.Append(name);
    result.Append(" provided (");
    result.Append(FormatNumber(given));
    result.Append(") is less than ");
    result.Append(eq ? "or equal to " : "");
    result.Append("the minimum bound (");
    result.Append(FormatNumber(bound));
    result.Append(").");
    return result.ToString();
  }

  template <typename NumberType>
  static String IndexOutsideRange(const char* name,
                                  NumberType given,
                                  NumberType lower_bound,
                                  BoundType lower_type,
                                  NumberType upper_bound,
                                  BoundType upper_type) {
    StringBuilder result;
    result.Append("The ");
    result.Append(name);
    result.Append(" provided (");
    result.Append(FormatNumber(given));
    result.Append(") is outside the range ");
    result.Append(lower_type == kExclusiveBound ? '(' : '[');
    result.Append(FormatNumber(lower_bound));
    result.Append(", ");
    result.Append(FormatNumber(upper_bound));
    result.Append(upper_type == kExclusiveBound ? ')' : ']');
    result.Append('.');
    return result.ToString();
  }

  static String InvalidArity(const char* expected, unsigned provided);

  static String NotASequenceTypeProperty(const String& property_name);
  static String NotAFiniteNumber(double value,
                                 const char* name = "value provided");
  static String NotAFiniteNumber(const Decimal& value,
                                 const char* name = "value provided");

  static String NotEnoughArguments(unsigned expected, unsigned provided);

  static String ReadOnly(const char* detail = nullptr);

 private:
  template <typename NumType>
  static String FormatFiniteNumber(NumType number) {
    if (number > 1e20 || number < -1e20)
      return String::Format("%e", 1.0 * number);
    return String::Number(number);
  }

  template <typename NumType>
  static String FormatPotentiallyNonFiniteNumber(NumType number) {
    if (std::isnan(number))
      return "NaN";
    if (std::isinf(number))
      return number > 0 ? "Infinity" : "-Infinity";
    if (number > 1e20 || number < -1e20)
      return String::Format("%e", number);
    return String::Number(number);
  }

  static String OrdinalNumber(int number);
};

template <>
PLATFORM_EXPORT String ExceptionMessages::FormatNumber<float>(float number);

template <>
PLATFORM_EXPORT String ExceptionMessages::FormatNumber<double>(double number);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_BINDINGS_EXCEPTION_MESSAGES_H_
