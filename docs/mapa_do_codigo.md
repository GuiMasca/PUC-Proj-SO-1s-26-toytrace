# Mapa do Código - toytrace

## 1. Ponto de entrada do programa

O programa começa na função main, que é responsável por ler e processar os argumentos da linha de comando e iniciar a execução.

A CLI recebe comandos como:
./toytrace trace -- programa

A partir disso, o fluxo segue para o runtime de tracing.

---

## 2. Criação do processo alvo

A criação do processo monitorado ocorre na função launch_tracee, localizada em:

src/student/trace_runtime.c

Essa função utiliza um fork() para criar o processo filho, um ptrace(PTRACE_TRACEME) no filho, um raise(SIGSTOP) para pausar o filho e um execvp() para executar o programa alvo

---

## 3. Loop de tracing

O loop principal de tracing ocorre também em trace_runtime.c.

Esse loop utiliza o waitpid() para esperar eventos do processo e o ptrace(PTRACE_SYSCALL) para avançar até a próxima syscall.

Esse é o núcleo do funcionamento do toytrace.

---

## 4. Callback de eventos

Quando uma syscall é detectada, o runtime monta um evento e chama um callback.

Esse callback é responsável por tratar e exibir as informações das syscalls.

---

## 5. Arquivos que o grupo deve modificar

Os arquivos principais que devem ser modificados estão em:

src/student/. Principalmente em trace_runtime.c

---

## 6. Primeiro TODO encontrado

Ao executar o programa inicialmente, o primeiro erro encontrado foi:

"TODO Semana 2: implementar launch_tracee()"

---

## 7. Dúvidas iniciais do grupo

A principio o grupo inicialmente teve dúvidas em relação a organização em si do projeto e como entender e prosseguir em relação a primeira etapa.
(Atualização 07/05 - Após uma análise mais profunda do projeto, houve dúvudas sobre algumas divergências entre o pdf guia e os comentários no esqueleto do código
