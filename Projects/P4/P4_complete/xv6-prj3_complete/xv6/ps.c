#include "types.h"
#include "mmu.h"
#include "param.h"
#include "proc.h"
#include "user.h"
#include "pstat.h"

int main(int argc, char *argv[])
{
    struct pstat info = {};
    int test;

    test =  getpinfo(&info);
    if(test == -1)
      printf(1, "getpinfo failed\n");
    else
      printf(1, "getpinfo succeeded\n");
    printf(1, "CurrTot# non-UNUSED (used) processes = %d\n", info.num_processes);
    printf(1, "PID\tTICKETS\tTICKS\n");
    for (int i = 0; i < info.num_processes; ++i) {
      printf(1, "%d\t%d\t%d\n", info.pid[i], info.tickets[i], info.ticks[i]);
    }
    exit();
}

