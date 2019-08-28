#ifndef __RGB_HSV_H
#define __RGB_HSV_H

#include "stdint.h"

void rgb2hsv(uint8_t r, uint8_t g, uint8_t b, float *h, float *s, float *v);
void hsv2rgb(float h, float s, float v, uint8_t *r, uint8_t *g, uint8_t *b);

#endif /* __RGB_HSV_H */

