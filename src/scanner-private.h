/* scanner-private.h
 * Ondřej Míchal <xmicha80>
 * 03/10/2020
 */

#include <stdio.h>
#include "scanner.h"

typedef enum scanner_state
{
  INIT
} scanner_state;

struct scanner
{
  FILE *file; /**< the scanned file*/
  int position; /**< reading position*/

  scanner_state state; /**< scanner's state*/

  char character; /**< currently assessed character*/
};
