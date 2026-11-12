/* Cărăușu Nelu-Rareș-Ionuț  -  323CB */

#include "functiiCoadaStiva.c"

/* functie care cauta thread-ul cu id-ul specificat
 * si afiseaza mesajul corespunzator
 */
void get_thread(int thread_id, stiva *pool_threaduri, coada *coada_running,
                FILE *file) {
  stiva *dup = duplicateStack(pool_threaduri);
  while (stiva_goala(dup) == 0) {
    Thread *thread = frontStack(dup);
    if (thread->ID == thread_id) {
      fprintf(file, "Thread %d is idle.\n", thread->ID);
      return;
    }
    pop(dup);
  }

  int count = 0;
  while (count < coada_running->lungime) {
    Task *task = frontQueue(coada_running);
    if (task->thread->ID == thread_id) {
      fprintf(file, "Thread %d is running task %d (remaining_time = %d).\n",
              thread_id, task->ID, task->timpExec);
      return;
    }
    count++;
  }
}

/* functie care afiseaza continutul cozii waiting */
void print_waiting(coada *coada_waiting, FILE *file) {
  sort(coada_waiting, coada_waiting->lungime);

  fprintf(file, "====== Waiting queue =======");
  fprintf(file, "\n[");

  for (int i = 0; i < coada_waiting->lungime; i++) {
    Task *task = frontQueue(coada_waiting);
    if (i == coada_waiting->lungime - 1) {
      fprintf(file, "(%d: priority = %d, remaining_time = %d)", task->ID,
              task->prioritate, task->timpExecInit);
    } else {
      fprintf(file, "(%d: priority = %d, remaining_time = %d),\n", task->ID,
              task->prioritate, task->timpExecInit);
    }
    deQueue(coada_waiting);
    enQueue(coada_waiting, task);
  }
  fprintf(file, "]\n");
}

/* functie care afiseaza continutul cozii running */
void print_running(coada *coada_running, FILE *file) {
  sort(coada_running, coada_running->lungime);

  fprintf(file, "====== Running in parallel =======");
  fprintf(file, "\n[");

  for (int i = 0; i < coada_running->lungime; i++) {
    Task *task = frontQueue(coada_running);
    if (i == coada_running->lungime - 1) {
      fprintf(file,
              "(%d: priority = %d, remaining_time = %d, running_thread = %d)",
              task->ID, task->prioritate, task->timpExec, task->thread->ID);
    } else {
      fprintf(
          file,
          "(%d: priority = %d, remaining_time = %d, running_thread = %d),\n",
          task->ID, task->prioritate, task->timpExec, task->thread->ID);
    }
    deQueue(coada_running);
    enQueue(coada_running, task);
  }
  fprintf(file, "]\n");
}

/* functie care afiseaza continutul cozii finished */
void print_finished(coada *coada_finished, FILE *file) {
  fprintf(file, "====== Finished queue =======");
  fprintf(file, "\n[");

  for (int i = 0; i < coada_finished->lungime; i++) {
    Task *task = frontQueue(coada_finished);
    if (i == coada_finished->lungime - 1) {
      fprintf(file, "(%d: priority = %d, executed_time = %d)", task->ID,
              task->prioritate, task->timpExecInit);
    } else {
      fprintf(file, "(%d: priority = %d, executed_time = %d),\n", task->ID,
              task->prioritate, task->timpExecInit);
    }
    deQueue(coada_finished);
    enQueue(coada_finished, task);
  }
  fprintf(file, "]\n");
}

/* functie care afiseaza timpul in care au rulat task-urile +
 * timpul de care mai este nevoie pentru ca task-urile din
 * coada running sa se termine de rulat
 * */
void finish(int timp, coada *coada_finished, FILE *file) {
  int max = 0;
  for (int i = 0; i < coada_finished->lungime; i++) {
    Task *task = frontQueue(coada_finished);
    deQueue(coada_finished);
    if (task->timpExec > max) {
      max = task->timpExec;
    }
    enQueue(coada_finished, task);
  }

  timp = timp + max;

  fprintf(file, "Total time: %d\n", timp);
}

/* functie care ruleaza task-urile */
void run(stiva *pool_threaduri, coada *coada_waiting, coada *coada_running,
         coada *coada_finished, int timp, int q, int *v, FILE *file) {
  fprintf(file, "Running tasks for %d ms...\n", timp);

  if (q >= timp) {
    while (coada_goala(coada_waiting) == 0 &&
           stiva_goala(pool_threaduri) == 0) {
      Task *task = frontQueue(coada_waiting);
      Thread *thread = frontStack(pool_threaduri);
      deQueue(coada_waiting);
      task->thread = thread;
      enQueue(coada_running, task);
      pop(pool_threaduri);
    }

    for (int i = 0; i < coada_running->lungime; i++) {
      Task *task = frontQueue(coada_running);
      if (task->timpExec <= timp) {
        deQueue(coada_running);
        Thread *thread = task->thread;
        task->thread = NULL;
        push(pool_threaduri, thread);
        v[task->ID - 1] = 0;
        enQueue(coada_finished, task);
      } else {
        deQueue(coada_running);
        task->timpExec = task->timpExec - timp;
        enQueue(coada_running, task);
      }
    }
  } else {
    while (timp > q) {
      while (coada_goala(coada_waiting) == 0 &&
             stiva_goala(pool_threaduri) == 0) {
        Task *task = frontQueue(coada_waiting);
        Thread *thread = frontStack(pool_threaduri);
        deQueue(coada_waiting);
        task->thread = thread;
        enQueue(coada_running, task);
        pop(pool_threaduri);
      }

      for (int i = 0; i < coada_running->lungime; i++) {
        Task *task = frontQueue(coada_running);
        if (task->timpExec <= q) {
          deQueue(coada_running);
          Thread *thread = task->thread;
          task->thread = NULL;
          push(pool_threaduri, thread);
          v[task->ID - 1] = 0;
          enQueue(coada_finished, task);
        } else {
          deQueue(coada_running);
          task->timpExec = task->timpExec - q;
          enQueue(coada_running, task);
        }
      }
      timp = timp - q;
    }

    while (coada_goala(coada_waiting) == 0 &&
           stiva_goala(pool_threaduri) == 0) {
      Task *task = frontQueue(coada_waiting);
      Thread *thread = frontStack(pool_threaduri);
      deQueue(coada_waiting);
      task->thread = thread;
      enQueue(coada_running, task);
      pop(pool_threaduri);
    }

    for (int i = 0; i < coada_running->lungime; i++) {
      Task *task = frontQueue(coada_running);
      if (task->timpExec <= timp) {
        deQueue(coada_running);
        Thread *thread = task->thread;
        task->thread = NULL;
        push(pool_threaduri, thread);
        v[task->ID - 1] = 0;
        task->timpExec = task->timpExecInit;
        enQueue(coada_finished, task);
      } else {
        deQueue(coada_running);
        task->timpExec = task->timpExec - timp;
        enQueue(coada_running, task);
      }
    }
  }
}

int main(int argc, char **argv) {
  FILE *input = fopen(argv[1], "rt");
  if (!input) {
    return -1;
  }

  FILE *output = fopen(argv[2], "wt");
  if (!output) {
    return -1;
  }

  // creez cozile waiting, running si finished
  coada *coada_waiting = creare_coada();
  coada *coada_running = creare_coada();
  coada *coada_finished = creare_coada();

  char str[1000];

  // citesc prima linie din fisier unde se afla q, cuanta de timp
  fgets(str, 1000, input);
  //setez terminatorul de sir
  if (str[strlen(str) - 2] == '\n' || str[strlen(str) - 2] == '\r') {
    str[strlen(str) - 2] = '\0';
  }
  if (str[strlen(str) - 1] == '\n' || str[strlen(str) - 1] == '\r') {
    str[strlen(str) - 1] = '\0';
  }
  int q = atoi(str);

  // citesc a doua linie din fisier, unde se afla c, numarul de core-uri
  fgets(str, 1000, input);
  //setez terminatorul de sir
  if (str[strlen(str) - 2] == '\n' || str[strlen(str) - 2] == '\r') {
    str[strlen(str) - 2] = '\0';
  }
  if (str[strlen(str) - 1] == '\n' || str[strlen(str) - 1] == '\r') {
    str[strlen(str) - 1] = '\0';
  }
  int c = atoi(str);

  // creez stiva cu thread-uri
  stiva *pool_threaduri = creare_pool_threaduri(2 * c);

  int timpTotal = 0;

  /* aloc un vector care are la pozitia i ori 0, caz in care, numarul i + 1 
   * este disponibil pentru a fi folosit ca id al unui task, ori 1, caz in 
   * care numarul i + 1 nu este disponibil pentru a fi folosit ca id de task
   */
  int *v = (int *)calloc(NR_MAX_TASKURI, sizeof(int));
  if (!v) {
    fclose(input);
    fclose(output);
    return 0;
  }

  // citesc linie cu linie
  while (fgets(str, 1000, input)) {
    // setez terminatorul de sir la finalul string-ului citit
    if (str[strlen(str) - 2] == '\n' || str[strlen(str) - 2] == '\r') {
      str[strlen(str) - 2] = '\0';
    }
    if (str[strlen(str) - 1] == '\n' || str[strlen(str) - 1] == '\r') {
      str[strlen(str) - 1] = '\0';
    }

    char *token = strtok(str, " ");

    if (strcmp(token, "add_tasks") == 0) {
      token = strtok(NULL, " ");
      int numOfTasks = atoi(token);
      token = strtok(NULL, " ");
      int timpExec = atoi(token);
      token = strtok(NULL, " ");
      int prioritate = atoi(token);

      add_tasks(numOfTasks, timpExec, prioritate, v, coada_waiting, output);
    }

    if (strcmp(token, "get_task") == 0) {
      token = strtok(NULL, " ");
      int task_id = atoi(token);
      get_task(task_id, coada_waiting, coada_running, coada_finished, output);
    }

    if (strcmp(token, "get_thread") == 0) {
      token = strtok(NULL, " ");
      int thread_id = atoi(token);
      get_thread(thread_id, pool_threaduri, coada_running, output);
    }

    if (strcmp(token, "run") == 0) {
      token = strtok(NULL, " ");
      int timp = atoi(token);
      timpTotal += timp;
      run(pool_threaduri, coada_waiting, coada_running, coada_finished, timp, q,
          v, output);
    }

    if (strcmp(token, "print") == 0) {
      token = strtok(NULL, " ");
      if (strcmp(token, "waiting") == 0) {
        print_waiting(coada_waiting, output);
      }
    }

    if (strstr(token, "running")) {
      print_running(coada_running, output);
    }

    if (strstr(token, "finished")) {
      print_finished(coada_finished, output);
    }

    if (strcmp(token, "finish") == 0) {
      finish(timpTotal, coada_finished, output);
    }
  }

  free(v);

  fclose(input);
  fclose(output);

  return 0;
}