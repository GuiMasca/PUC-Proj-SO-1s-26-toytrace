#include "trace_runtime.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

#if !defined(__x86_64__)
#error "Este runtime didatico suporta apenas Linux x86_64."
#endif

static void fill_event_from_regs(pid_t pid,
                                 int entering,
                                 const struct user_regs_struct *regs,
                                 struct syscall_event *ev)
{
    /*
     * TODO Semana 4:
     *
     * Preencha struct syscall_event usando os registradores x86_64.
     *
     * Dicas:
     * - regs->orig_rax contem o numero da syscall.
     * - regs->rax contem o retorno, valido na saida.
     * - os seis argumentos ficam em rdi, rsi, rdx, r10, r8 e r9.
     * - ev->entering deve copiar o parametro entering.
     */
    memset(ev, 0, sizeof(*ev));
    ev->pid = pid;
    ev->entering = entering;
}

static pid_t launch_tracee(char *const argv[])
{
    /*
     * TODO Semana 2:
     *
     * Crie o processo monitorado.
     *
     * Fluxo esperado:
     * - fork()
     * - no filho:
     *   - ptrace(PTRACE_TRACEME, ...)
     *   - raise(SIGSTOP)
     *   - execvp(argv[0], argv)
     * - no pai:
     *   - retornar o pid do filho
     *
     * Em erro, imprima uma mensagem com perror() e retorne -1.
     */
    //nossa solução:(Guilherme)
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // processo filho

        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            perror("ptrace TRACEME");
            exit(1);
        }

        raise(SIGSTOP);

        execvp(argv[0], argv);

        // só chega aqui se execvp falhar
        perror("execvp");
        exit(1);
    }


   // fprintf(stderr, "erro: TODO Semana 2: implementar launch_tracee()\n");
   //processo pai
    return pid;
}

static int wait_for_initial_stop(pid_t child)
{
    /*
     * TODO Semana 2:
     *
     * O filho chama raise(SIGSTOP) antes de executar o programa alvo.
     * O pai precisa esperar essa parada inicial com waitpid().
     *
     * Retorne 0 se o filho parou como esperado, -1 em erro.
     */
    //nossa solução: (Atualizado(Marcelo 07/05) - finalizar os pedidos da semana 2 ))
    if(pid == 0){
        raise(SIGSTOP);
    }else if(pid>0){
        waitpid(pid, status, 0);

        if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGSTOP) { // veriificação se está parado e se foi parado pelo SIGSTOP
            return 0; // OK: filho parou como esperado
        } else {
            fprintf(stderr, "Filho não parou\n");
            return -1;
        }
    }else{
        perror("fork");
        return -1;
    }   
}

static int configure_trace_options(pid_t child)
{
    /*
     * TODO Semana 3:
     *
     * Configure PTRACE_O_TRACESYSGOOD com PTRACE_SETOPTIONS.
     * Isso ajuda a diferenciar paradas de syscall de outros sinais.
     */
    //nossa solução
    //usamos o ptrace(que dentro do if já 'roda' o comando) para diferenciar as syscall
    if(ptrace(PTRACE_SETOPTIONS,child, 0,PTRACE_O_TRACESYSGOOD) == -1){ // senão for por syscall ele retorna -1
        perror("ptrace -- SETOPTIONS / TRACESYSGOOD");
        return -1;
    }

}

static int resume_until_next_syscall(pid_t child, int signal_to_deliver)
{
    /*
     * TODO Semana 3:
     *
     * Use ptrace(PTRACE_SYSCALL, ...) para deixar o filho executar ate a
     * proxima entrada ou saida de syscall.
     *
     * signal_to_deliver deve ser repassado como quarto argumento do ptrace.
     */
    //Nossa solução
    //usamos o ptrce para verificae e rodar ao mesmo tempo no if passando os argumentos para o ptrace (iguak ao configure_trace_options)
    if(ptrace(PTRACE_SYSCALL,child, 0, signal_to_deliver) == -1)
        perror("ptrace(PTRACE_SYSCALL)");
        return -1;
    }
}

static int wait_for_syscall_stop(pid_t child, int *status)
{
    /*
     * TODO Semana 3:
     *
     * Espere o filho com waitpid().
     *
     * Retorne:
     *   1 se a parada foi uma parada de syscall;
     *   0 se o filho terminou normalmente ou por sinal;
     *  -1 em erro.
     *
     * Dicas:
     * - WIFEXITED e WIFSIGNALED indicam fim do processo.
     * - WIFSTOPPED indica que o processo parou.
     * - com PTRACE_O_TRACESYSGOOD, syscall-stops aparecem com bit 0x80.
     * - paradas SIGTRAP comuns nao devem ser entregues de volta ao filho.
     */
    //Nossa solução:
    while(1){
     if(waitpid(child, status,0)== -1){ //validação
            return -1;
        }
        //retorno 0, caso normal
        if(WIFEXITED(status) || WIFSIGNALED(status)){
            return 0;
        }
        //Parado do filho
        if(WIFSTOPPED(status)){
            if(WSTOPSIG(status) == (SIGTRAP|0x80)){
                printf("Filho parou por SYSCALL"
                return 1; //Parado por siscal
            }
            //Se ele não parou, ele vai executar a função resume_until_next_syscall com seus parametros
            resume_until_next_syscall(child,0);

             //filho não deve recebrer SIGTRAP comum
            //Podemos usar também a função resume_until_nextsyscall;
        }
    }
}

int trace_program(char *const argv[],
                  trace_observer_fn observer,
                  void *userdata)
{
    pid_t child;
    int status = 0;
    int entering = 1;

    if (argv == NULL || argv[0] == NULL) {
        fprintf(stderr, "erro: programa alvo ausente\n");
        return -1;
    }

    child = launch_tracee(argv);
    if (child < 0) {
        return -1;
    }

    if (wait_for_initial_stop(child) < 0) {
        return -1;
    }

    if (configure_trace_options(child) < 0) {
        return -1;
    }

    if (resume_until_next_syscall(child, 0) < 0) {
        return -1;
    }

    while (1) {
        struct user_regs_struct regs;
        struct syscall_event ev;
        int stop_kind;

        stop_kind = wait_for_syscall_stop(child, &status);
        if (stop_kind < 0) {
            return -1;
        }
        if (stop_kind == 0) {
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            return 0;
        }

        /*
         * TODO Semana 4:
         *
         * Use PTRACE_GETREGS para preencher regs.
         * Depois chame fill_event_from_regs() e observer().
         */
        memset(&regs, 0, sizeof(regs));
        fill_event_from_regs(child, entering, &regs, &ev);
        if (observer != NULL) {
            observer(&ev, userdata);
        }

        entering = !entering;

        if (resume_until_next_syscall(child, 0) < 0) {
            return -1;
        }
    }
}
