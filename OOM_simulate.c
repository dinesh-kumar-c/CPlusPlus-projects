# include <iostream>
# include <sys/param.h>
# include <string.h>

void handler(int nSignum, siginfo_t* si, void* vcontext) 
{
  std::cout << "Segmentation fault" << std::endl;
  ucontext_t* context = (ucontext_t*)vcontext;
  context->uc_mcontext.gregs[REG_RIP]++;
}


int main() 
{
//  std::cout << "double size on this host::" << sizeof(double);

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = handler;
    sigaction(SIGSEGV, &action, NULL);
    unsigned long mem_consume;

    //simulate OOM
    for (int i = 1; i < 1000000; i++) {
        double* pi = new double[2000000];
        mem_consume =(i*8*2000000)/(1024);
        std::cout <<  "Iteration : "<< i << "  " << "memory taken::" << mem_consume << " kb" << std::endl;
    }

    return 0;
} 
