#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <wchar.h>
#include <string.h>
#include <locale.h>

int main(char *argv[], int argc) {

  const char *hello = "fj\u00E4rde";

  
  size_t s = 100;
  wchar_t wdst[s];

  mbstate_t ps;

  setlocale(LC_ALL,"en_US.UTF-8");
  
  int r = mbsrtowcs(wdst, &hello, s, &ps);

  printf(" as wide char (%d)  %S\n", r, wdst);

  for(int i = 0; i < (r+1); i++) {
    printf(" wdst[%d] = 0x%hhx (%d) \n", i, wdst[i], wdst[i]);
  }  

  
  char dst[s];

  const wchar_t *wsrc = wdst;
  
  size_t t = wcstombs(dst,  wsrc, s);

  printf(" as multi-byte (%ld) char %s\n", t, dst);  

    
}
