#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
bool putinteger(int a) {
  printf("%i\n", a);
  return true;
}

int getinteger() {
  int ret;
  scanf("%i", &ret);
  getchar();
  return ret;
}

bool getbool() {
  int ret;
  scanf("%i", &ret);
  getchar();
  return (ret == 1);
}

bool putbool(bool in) {
  printf("%i\n", in);
  return true;
}

float getfloat() {
  float ret;
  scanf("%f", &ret);
  getchar();
  return ret;
}

bool putfloat(float in) {
  printf("%f\n", in);
  return true;
}

char* getstring() {
  int max_length = 256;
  char *string = malloc(max_length * sizeof(char));
  fgets(string, max_length, stdin);
  if ((strlen(string) > 0) && (string[strlen(string) - 1] == '\n')) {
    string[strlen(string) - 1] = '\0';
  }
  return string;
}

bool putstring(char *string) {
  printf("%s\n", string);
  return true;
}

float mysqrt(int in) {
  return sqrt((double)in);
}

void boundsError() {
  printf("Error - out of bounds exception!\n");
  exit(1);
}