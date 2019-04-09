/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple, Inc. All rights
 * reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2012 Samsung Electronics. All rights reserved.
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
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_COMPOSITING_COMPOSITING_TRIGGERS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_COMPOSITING_COMPOSITING_TRIGGERS_H_

namespace blink {

enum CompositingTrigger {
  kScrollableInnerFrameTrigger = 1 << 1,
  kOverflowScrollTrigger = 1 << 2,
  kViewportConstrainedPositionedTrigger = 1 << 3,
  kAllCompositingTriggers = 0xFFFFFFFF,
};

typedef unsigned CompositingTriggerFlags;

}  // namespace blink

#endif
