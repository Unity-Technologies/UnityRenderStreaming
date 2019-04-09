// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NGBaseline_h
#define NGBaseline_h

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/fonts/font_baseline.h"
#include "third_party/blink/renderer/platform/geometry/layout_unit.h"

namespace blink {

class LayoutBox;
class NGLayoutInputNode;

enum class NGBaselineAlgorithmType {
  // Compute baselines for atomic inlines.
  kAtomicInline,
  // Compute baseline of first line box.
  kFirstLine
};

// Baselines are products of layout.
// To compute baseline, add requests to NGConstraintSpace and run Layout().
class CORE_EXPORT NGBaselineRequest {
 public:
  NGBaselineRequest(NGBaselineAlgorithmType algorithm_type,
                    FontBaseline baseline_type)
      : algorithm_type_(static_cast<unsigned>(algorithm_type)),
        baseline_type_(static_cast<unsigned>(baseline_type)) {}

  NGBaselineAlgorithmType AlgorithmType() const {
    return static_cast<NGBaselineAlgorithmType>(algorithm_type_);
  }

  FontBaseline BaselineType() const {
    return static_cast<FontBaseline>(baseline_type_);
  }

  bool operator==(const NGBaselineRequest& other) const;
  bool operator!=(const NGBaselineRequest& other) const {
    return !(*this == other);
  }

 private:
  // TypeId is an integer that identifies all combinations of
  // |NGBaselineRequest|. Visible only to |NGBaselineRequestList| and
  // |NGBaselineList|.
  static constexpr unsigned kTypeIdCount = 4;
  unsigned TypeId() const { return algorithm_type_ | (baseline_type_ << 1); }
  static NGBaselineRequest FromTypeId(unsigned type_id) {
    DCHECK_LE(type_id, kTypeIdCount);
    return NGBaselineRequest(static_cast<NGBaselineAlgorithmType>(type_id & 1),
                             static_cast<FontBaseline>((type_id >> 1) & 1));
  }
  friend class NGBaselineList;
  friend class NGBaselineRequestList;
  friend class NGBaselineTest;

  unsigned algorithm_type_ : 1;  // NGBaselineAlgorithmType
  unsigned baseline_type_ : 1;   // FontBaseline
};

// A list of |NGBaselineRequest| in a packed format, with similar interface as
// |Vector|.
class CORE_EXPORT NGBaselineRequestList {
 public:
  NGBaselineRequestList() = default;

  bool IsEmpty() const { return !type_id_mask_; }

  bool operator==(const NGBaselineRequestList& other) const;

  void push_back(const NGBaselineRequest& request);
  void AppendVector(const NGBaselineRequestList& requests);

  class const_iterator {
   public:
    const_iterator() : type_id_(NGBaselineRequest::kTypeIdCount), mask_(0) {}
    explicit const_iterator(unsigned mask) : type_id_(0), mask_(mask) {
      if (!(mask_ & 1))
        ++(*this);
    }

    const NGBaselineRequest operator*() const {
      return NGBaselineRequest::FromTypeId(type_id_);
    }
    bool operator!=(const const_iterator& other) const {
      return type_id_ != other.type_id_;
    }
    void operator++() {
      while (type_id_ < NGBaselineRequest::kTypeIdCount) {
        ++type_id_;
        mask_ >>= 1;
        if (mask_ & 1)
          break;
      }
    }

   private:
    unsigned type_id_;
    unsigned mask_;
  };

  const_iterator begin() const { return const_iterator(type_id_mask_); }
  const_iterator end() const { return const_iterator(); }

 private:
  // Serialize/deserialize to a bit fields.
  static constexpr unsigned kSerializedBits = NGBaselineRequest::kTypeIdCount;
  unsigned Serialize() const { return type_id_mask_; }
  explicit NGBaselineRequestList(unsigned serialized)
      : type_id_mask_(serialized) {}
  friend class NGConstraintSpace;
  friend class NGConstraintSpaceBuilder;

  unsigned type_id_mask_ = 0;
};

// Represents a computed baseline position.
struct CORE_EXPORT NGBaseline {
  NGBaselineRequest request;
  LayoutUnit offset;

  // @return if the node needs to propagate baseline requests/results.
  static bool ShouldPropagateBaselines(const NGLayoutInputNode);
  static bool ShouldPropagateBaselines(LayoutBox*);
};

// A list of |NGBaseline| in a packed format, with similar interface as
// |Vector|.
class CORE_EXPORT NGBaselineList {
 public:
  NGBaselineList();

  bool IsEmpty() const;

  base::Optional<LayoutUnit> Offset(const NGBaselineRequest request) const;

  void emplace_back(NGBaselineRequest request, LayoutUnit offset);

  class const_iterator {
   public:
    explicit const_iterator(unsigned type_id, const LayoutUnit* offset)
        : type_id_(type_id), offset_(offset) {
      DCHECK(offset);
      if (*offset == kEmptyOffset)
        ++(*this);
    }
    const_iterator()
        : type_id_(NGBaselineRequest::kTypeIdCount), offset_(nullptr) {}

    const NGBaseline operator*() const {
      return NGBaseline{NGBaselineRequest::FromTypeId(type_id_), *offset_};
    }
    bool operator!=(const const_iterator& other) const {
      return type_id_ != other.type_id_;
    }
    void operator++() {
      while (type_id_ < NGBaselineRequest::kTypeIdCount) {
        ++type_id_;
        ++offset_;
        if (type_id_ < NGBaselineRequest::kTypeIdCount &&
            *offset_ != kEmptyOffset)
          break;
      }
    }

   private:
    unsigned type_id_;
    const LayoutUnit* offset_;
  };

  const_iterator begin() const { return const_iterator(0, offsets_); }
  const_iterator end() const { return const_iterator(); }

 private:
  static constexpr LayoutUnit kEmptyOffset = LayoutUnit::Min();

  LayoutUnit offsets_[NGBaselineRequest::kTypeIdCount];
};

}  // namespace blink

#endif  // NGBaseline_h
