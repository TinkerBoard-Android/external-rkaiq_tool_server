#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <thread>
#include <vector>

#include "multiframe_process.h"

void DumpRawData(uint16_t *buf, uint32_t len, uint32_t plen) {
  uint32_t a;
  for (uint32_t n = 0; n < len; n++) {
    if (n < plen)
      fprintf(stderr, "buf[%d]      %x\n", n, buf[n]);
  }
}

void DumpRawData32(uint32_t *buf, uint32_t len, uint32_t plen) {
  uint32_t a;
  for (uint32_t n = 0; n < len; n++) {
    if (n < plen)
      fprintf(stderr, "buf[%d]      %x\n", n, buf[n]);
  }
}

void ConverToLE(uint16_t *buf, uint32_t len) {
  uint32_t a;
  for (uint32_t n = 0; n < len; n++) {
    a = buf[n];
    buf[n] = (a << 8) | (a >> 8);
  }
}
// only for even number frame
void MultiFrameAverage(uint32_t *pIn1_pOut, uint16_t *POut, uint16_t width,
                       uint16_t height, uint8_t frameNumber) {
  uint16_t n;
  uint16_t roundOffset = 0;
  switch (frameNumber) {
  case 128:
    n = 7;
    break;
  case 64:
    n = 6;
    break;
  case 32:
    n = 5;
    break;
  case 16:
    n = 4;
    break;
  case 8:
    n = 3;
    break;
  case 4:
    n = 2;
    break;
  case 2:
    n = 1;
    break;
  default:
    printf("frame number error! %d", frameNumber);
    return;
    break;
  }
  roundOffset = pow(2, n - 1);
  int w, h, m;
  for (h = 0; h < height; h++) {
    m = h * width;
    for (w = 0; w < width; w++) {
      pIn1_pOut[m + w] += roundOffset;
      pIn1_pOut[m + w] = pIn1_pOut[m + w] >> n;
      POut[m + w] = (uint16_t)pIn1_pOut[m + w] & 0xFFF0;
    }
  }
}

void MultiFrameAddition(uint32_t *pIn1_pOut, uint16_t *pIn2, uint16_t width,
                        uint16_t height) {
  int w, h, n;
  for (h = 0; h < height; h++) {
    n = h * width;
    for (w = 0; w < width; w++) {
      pIn1_pOut[n + w] +=
          ((uint16_t)(pIn2[n + w] >> 8) | (uint16_t)(pIn2[n + w] << 8));
    }
  }
}

void FrameU32ToU16(uint32_t *pIn, uint16_t *pOut, uint16_t width,
                   uint16_t height) {
  int w, h, n;
  for (h = 0; h < height; h++) {
    n = h * width;
    for (w = 0; w < width; w++) {
      pOut[n + w] += pIn[n + w];
    }
  }
}

void FrameU16ToU32(uint16_t *pIn, uint32_t *pOut, uint16_t width,
                   uint16_t height) {
  int w, h, n;
  for (h = 0; h < height; h++) {
    n = h * width;
    for (w = 0; w < width; w++) {
      pOut[n + w] += pIn[n + w];
    }
  }
}
