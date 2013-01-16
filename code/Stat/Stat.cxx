# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <common.h>
# include <sys/resource.h>
# include <sys/times.h>

int parseLine(char* line){
  int i = strlen(line);
  while (*line < '0' || *line > '9') line++;
  line[i-3] = '\0';
  i = atoi(line);
  return i;
}

double getMemUsage() { //Note: this value is in KB!
  FILE* file = fopen("/proc/self/status", "r");
  int result = -1;
  char line[128];
    
  while (fgets(line, 128, file) != NULL){
    if (strncmp(line, "VmSize:", 7) == 0){
      result = parseLine(line);
      break;
    }
  }
  fclose(file);
  return (result/((double)1024.00));
}

double getCPUTime() {
  struct rusage rusage;

  if (getrusage( RUSAGE_SELF, &rusage ) != -1)
    return (double)rusage.ru_utime.tv_sec +
      (double)rusage.ru_utime.tv_usec / 1000000.0;
}
