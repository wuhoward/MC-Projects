#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <stdio.h>
#include <stdint.h>
#include "rocc.h"

#define XCUSTOM_MAN 3

#define mandelbrot(rd, x , y)                                       \
  ROCC_INSTRUCTION(XCUSTOM_MAN, rd, x, y, 0);

#endif 
