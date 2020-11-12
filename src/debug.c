/**
 * @file debug.c
 * @author Marek Filip (xfilip46, Wecros), FIT BUT 
 * @date 12/11/2020
 * @brief File holding debug function definitions.
 */

#include "debug.h"

void debug_lit_value(token_t t) {
  if (token_is_lit(t.type)) {
    switch (t.type) {
      case STRING_LIT:
        debug("Got VALUE: %s", strGetStr(&t.attribute.str_val));
        break;
      case FLOAT64_LIT:
        debug("Got VALUE: %g", t.attribute.float_val);
        break;
      case INT_LIT:
        debug("Got VALUE: %ld", t.attribute.int_val);
        break;
      default:
        break;
    }
  }
}
