# ifndef STAT_H
# define STAT_H

# define MEM_USAGE_UNIT "MB"
# define CPU_TIME_UNIT "s"
/* Returns mem consumption in KB */
extern double getMemUsage(void);
/* Returns CPU time in seconds */
extern double getCPUTime(void);

# endif
