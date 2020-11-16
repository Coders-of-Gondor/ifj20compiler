/* 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file debug.h
 * @author Zed A. Shaw
 * @author Ondřej Míchal <xmicha80>
 * @author Marek Filip <xfilip46>
 * @date 13/11/2020
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include "token.h"

#ifdef NDEBUG // if NDEBUG -> don't debug
// use this macro to silence unused parameter warning when compiling with NDEBUG
#define unused(x) ((void) x)

#define debug(M, ...)
#define debug_entry()
inline void debug_lit_value(token_t t) {
  unused(t);
}

#else
#define debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d: " M "\n",\
        __FILE__, __LINE__, ##__VA_ARGS__)

#define debug_entry() fprintf(stderr, "[DEBUG] ENTRY: %s():%d\n" ,\
        __FUNCTION__, __LINE__)

/**
 * @brief Debug the literal value of a literal.
 * @description Inline debugging function.
 */
inline void debug_lit_value(token_t t) {
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
#endif

#endif // __DEBUG_H__
