/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


#ifndef _ENCODEINTRA_H_
#define _ENCODEINTRA_H_
#include "onyx_int.h"

int vp8_encode_intra(VP8_COMP *cpi, MACROBLOCK *x, int use_dc_pred);
void vp8_encode_intra16x16mby(const VP8_ENCODER_RTCD *, MACROBLOCK *x);
void vp8_encode_intra16x16mbuv(const VP8_ENCODER_RTCD *, MACROBLOCK *x);
void vp8_encode_intra4x4mby(const VP8_ENCODER_RTCD *, MACROBLOCK *mb);
void vp8_encode_intra4x4block(const VP8_ENCODER_RTCD *rtcd,
                              MACROBLOCK *x, int ib);
#endif
