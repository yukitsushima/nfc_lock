/*
 * 2018.06.29
 * Scitech Door Lock System
 * Card Registration Program
 * Yuki Tsushima
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  FILE *fp, *fp2, *pipe;
  char tmp[100], search[101];
  int flag;
  fp = fopen("IDlist.dat", "a");
  fp2= fopen("IDlist.dat", "r");
  if (fp == NULL | fp2 == NULL) {
    fprintf(stderr, "File IO Error: Can't open IDlist.dat\n");
    exit(1);
  }
  while (1) {
    printf("Please Input User Name. (Ctrl-C to Exit.)\n");
    scanf("%99s", tmp);
    while(getchar() != '\n'); //clear stdin
    //search name to avoid registering the same name
    flag = 0;
    while (fgets(search, 99, fp2) != NULL) {
      if (search[0] == '#' && strcmp(search[1], tmp) == 0) {
        flag = 1; break;
      }
    }
    fseek(fp2, 0, SEEK_SET);
    if (flag) {
      fprintf(stderr, "%s is already registered!\n", tmp);
      continue;
    }
    fprintf(fp, "#%s\n", tmp);
    printf("Please Touch Felica.\n");
    pipe = popen("python nfc_sense.py 2>/dev/null", "r");
    if (pipe==NULL) {
      fprintf(stderr, "Error: Can't Execute Python.\n");
      exit(1);
    }
    fscanf(pipe, "%99s", tmp);
    if (pclose(pipe) == 0) {
      //search name to avoid registering the same card
      flag = 0;
      while (fgets(search, 99, fp2) != NULL) {
        if (search[0] != '#' && strcmp(search, tmp) == 0) {
          flag = 1; break;
        }
      }
      fseek(fp2, 0, SEEK_SET);
      if (flag) {
        fprintf(stderr, "This card is already registered!\n");
        continue;
      }
      fprintf(fp, "%s\n", tmp);
    } else {
      fprintf(stderr, "Eror: Python Program had Error.\n");
      fclose(fp);
      exit(1);
    }
    fflush(fp);
  }
  return 0;
}