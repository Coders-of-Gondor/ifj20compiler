/* 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* debug.h
 * Zed A. Shaw, Ondřej Míchal <xmicha80>
 * FIT BUT
 * 05/11/2020
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

#ifdef NDEBUG
#define debug(M, ...)
#define debug_entry()
#else
#define debug(M, ...) fprintf(stderr, "[DEBUG] %s:%d: " M "\n",\
        __FILE__, __LINE__, ##__VA_ARGS__)

#define debug_entry() fprintf(stderr, "[DEBUG] ENTRY: %s():%d\n" ,\
        __FUNCTION__, __LINE__)
#endif

#endif // __DEBUG_H__
