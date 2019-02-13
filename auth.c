/*
 * 2018.06.30
 * Scitech Door Lock System
 * Card Authorization Program
 * Yuki Tsushima
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
//ドアをロックするためのカード情報
#define CLOSE_KEY "35dda5088fa5ec267210ebcdd60b129d"

FILE *fp, *logfile;

void handler(int sig) {
  time_t timer;
  time(&timer);
  fprintf(logfile, "%s\tKilled with SIGTERM.\n", ctime(&timer));
  fflush(logfile);
  fclose(logfile);
  fclose(fp);
  exit(5);
}

int main(void) {
  //イベントハンドラ設定
  signal(SIGTERM, handler);
  //変数定義
  FILE *pipe;
  int flag = 0, isOpen=0;
  time_t timer;
  char tmp[100], name[100], card[100];
  //ログを開く
  logfile = fopen("Log.txt", "a");
  if (logfile==NULL) {
    fprintf(stderr, "File IO Error: Can't open Log.txt\n");
    exit(1);
  }
  //開始を記録
  time(&timer);
  fprintf(logfile, "%s\tStart ID Program.\n", ctime(&timer));
  fflush(logfile);
  //ユーザーリストを開く
  fp = fopen("IDlist.dat", "r");
  if (fp == NULL) {
    fprintf(stderr, "File IO Error: Can't open IDlist.dat\n");
    time(&timer);
    fprintf(logfile, "%s\tERROR: Failed to open IDlist.dat\n", ctime(&timer));
    fflush(logfile);
    exit(1);
  }
  //認証プログラム
  while (1) {
    //NFCリーダープログラムを起動、データ取得
    pipe = popen("python nfc_sense.py 2>/dev/null", "r");
    if (pipe==NULL) {
      fprintf(stderr, "Error: Can't Execute Python.\n");
      time(&timer);
      fprintf(logfile, "%s\tERROR: Failed to execute Python\n", ctime(&timer));
      fflush(logfile);
      exit(1);
    }
    fscanf(pipe, "%s", card);
    pclose(pipe);
    //判定
    if (strcmp(card, CLOSE_KEY)==0) {
      //施錠処理
      if (isOpen) {
        //施錠
        pipe = popen("python PWM_CLOSE.py 2>/dev/null", "r");
        if (pipe==NULL) {
          fprintf(stderr, "Error: Can't Execute Python.\n");
          time(&timer);
          fprintf(logfile, "%s\tERROR: Failed to execute Python\n", ctime(&timer));
          fflush(logfile);
          exit(1);
        } else {
          pclose(pipe);
          time(&timer);
          fprintf(logfile, "%s\tDoor closed\n", ctime(&timer));
          isOpen=0;
        }
      }
      sleep(2);
    } else {
      //判定、解錠
      while (fgets(tmp, 99, fp) != NULL) {
        //改行の除去
        strtok(tmp, "\n");
        if (tmp[0] == '#') {
          //名前はnameのとっておく
          strcpy(name, tmp+1);
        } else {
          if (strcmp(card, tmp) == 0) {
            //カードがtmpの情報と一致した場合
            flag=1;
            time(&timer);
            if (isOpen) {
              fprintf(logfile, "%s\tTouched by %s But already Door Open\n", ctime(&timer), name);
            } else {
              fprintf(logfile, "%s\tOpen by %s\n", ctime(&timer), name);
            }
            break;
          }
        }
      }
      if (flag) {
        //解錠処理
        pipe = popen("python PWM_OPEN.py 2>/dev/null", "r");
        if (pipe==NULL) {
          fprintf(stderr, "Error: Can't Execute Python.\n");
          time(&timer);
          fprintf(logfile, "%s\tERROR: Failed to execute Python\n", ctime(&timer));
          fflush(logfile);
          exit(1);
        } else {
          pclose(pipe);
          isOpen=1;
        }
      } else {
        time(&timer);
        fprintf(logfile, "%s\tNo data with %s\n", ctime(&timer), card);
      }
      fseek(fp, 0, SEEK_SET);
      flag=0;
      fflush(logfile);
      sleep(2);
    }
  }
  fflush(logfile);
  return 0;
}