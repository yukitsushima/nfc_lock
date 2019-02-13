/*
 * 2018.06.30
 * Scitech Door Lock System
 * Wrapper
 * Yuki Tsushima
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
FILE *fp;

void handler(int sig) {
  time_t timer;
  time(&timer);
  fprintf(fp, "%s\tmain has been killed.\n", ctime(&timer));
  fflush(fp);
  fclose(fp);
  exit(0);
}

int main(void) {
  //シグナルハンドラ設定
  signal(SIGTERM, handler);
  int status;
  time_t timer;
  //ログの用意
  fp = fopen("MetaLog.txt", "a");
  if (logfile==NULL) {
    fprintf(stderr, "File IO Error: Can't open MetaLog.txt\n");
    exit(1);
  }
  while (1) {
    //開始を記録
    time(&timer);
    fprintf(fp, "%s\tStart ID Program.\n", ctime(&timer));
    fflush(fp);
    //実行
    status = system("./auth");
    time(&timer);
    fprintf(fp, "%s\tFinished ID Program with status %d.\n", ctime(&timer), status);
    fflush(fp);
    if (status == 5) {
      time(&timer);
      fprintf(fp, "%s\tID Program has been killed.\n", ctime(&timer));
      fflush(fp);
      exit(0);
    }
  }
  fflush(fp);
  fclose(fp);
  return 0;
}