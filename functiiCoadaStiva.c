/* Cărăușu Nelu-Rareș-Ionuț  -  323CB */

#include "header_coada.h"
#include "header_stiva.h"
#include "header_lista.h"

/* functie care creeaza o celula */
nod *creare_nod(void *valoare) {
  nod *node = NULL;
  node = (nod *)malloc(sizeof(nod));

  node->info = valoare;
  node->next = NULL;

  return node;
}

/* functie care creeaza o coada */
coada *creare_coada() {
  coada *queue = NULL;
  queue = (coada *)malloc(sizeof(coada));

  queue->head = NULL;
  queue->tail = NULL;
  queue->lungime = 0;

  return queue;
}

/* functie care adauga un element in coada */
void enQueue(coada *queue, void *info) {
  nod *node = NULL;
  node = creare_nod(info);

  if (queue->lungime > 0) {
    queue->tail->next = node;
    queue->tail = node;
  } else {
    queue->head = queue->tail = node;
  }

  queue->lungime = queue->lungime + 1;
}

/* functie care verifica daca o coada e goala */
int coada_goala(coada *queue) {
  if (queue->lungime == 0) {
    return 1;
  }

  return 0;
}

/* functie care elimina un element din coada */
int deQueue(coada *queue) {
  if (queue->lungime == 0) {
    return 0;
  }

  nod *p = queue->head;
  queue->head = queue->head->next;

  queue->lungime = queue->lungime - 1;

  free(p);

  return 1;
}

/* functie care creeaza un duplicat al unei cozi */
coada *copiere(coada *queue) {
  coada *copy = creare_coada();
  coada *q = creare_coada();

  while (coada_goala(queue) == 0) {
    enQueue(copy, queue->head->info);
    enQueue(q, queue->head->info);
    deQueue(queue);
  }

  while (coada_goala(q) == 0) {
    enQueue(queue, q->head->info);
    deQueue(q);
  }

  return copy;
}

/* functie care creeaza un element de tip Task */
Task *creare_task(int ID, Thread *thread, int prioritate, int timpExec) {
  Task *task = NULL;
  task = (Task *)malloc(sizeof(Task));

  task->ID = ID;
  task->thread = thread;
  task->prioritate = prioritate;
  task->timpExec = timpExec;
  task->timpExecInit = timpExec;

  return task;
}

/* functie care returneaza capatul unei cozi fara a o modifica pe aceasta */
Task *frontQueue(coada *queue) {
  Task *task = (Task *)(queue->head->info);
  return task;
}

/* cauat in coada pozitia elementului ce trebuie pus primul in coada
 * si o salvez intr-o variabila, pos, apoi fac pos - 1 operatii de eliminare 
 * a elementului de la inceputul cozii si adaugare a acestuia de final, 
 * astfel incat elementul care trebuie sa fie la inceputul cozii sa ajunga 
 * sa fie acolo 
 */
void reorder(coada *queue, Task *elem, int n, int k) {
  int pos = -1;
  for (int i = 1; i < n + 2 - k; i++) {
    Task *current = frontQueue(queue);
    deQueue(queue);
    if (current->prioritate == elem->prioritate && current->ID == elem->ID &&
        current->timpExec == elem->timpExec) {
      pos = i;
    }
    enQueue(queue, current);
  }

  for (int i = 1; i < pos; i++) {
    Task *current = frontQueue(queue);
    deQueue(queue);
    enQueue(queue, current);
  }
}

/* caut mai intai primul element care trebuie pus in coada sortata, apoi
 * al doilea element si tot asa cu fiecare, iar pentru a nu mai cauta in 
 * elementele deja sortate, le pun pe acestea intr-o alta coada, iar la 
 * finalul cautarii le pun inapoi in coada initiala
 */
void findElem(coada *queue, int n, int k) {
  coada *q = creare_coada();
  for (int i = 0; i < k - 1; i++) {
    Task *c = frontQueue(queue);
    deQueue(queue);
    enQueue(q, c);
  }

  Task *elem = frontQueue(queue);

  for (int i = k; i < n + 1; i++) {
    Task *current = frontQueue(queue);
    deQueue(queue);

    if (current->prioritate > elem->prioritate) {
      elem = current;
    } else if (current->prioritate == elem->prioritate) {
      if (current->timpExec < elem->timpExec) {
        elem = current;
      } else if (current->timpExec == elem->timpExec) {
        if (current->ID < elem->ID) {
          elem = current;
        }
      }
    }
    enQueue(queue, current);
  }
  reorder(queue, elem, n, k);

  int sizeQueue = queue->lungime;

  while (coada_goala(q) == 0) {
    Task *t = frontQueue(q);
    deQueue(q);
    enQueue(queue, t);
  }

  for (int i = 0; i < sizeQueue; i++) {
    Task *current = frontQueue(queue);
    deQueue(queue);
    enQueue(queue, current);
  }
}

/* functie de sortare a elementelor dintr-o coada
 * in functie de anumite specificatii din enunt:
 * descrescător după prioritate
 * crescător după timp de execuție rămas, în caz de priorități egale
 * crescător după ID, în caz de prioritate și timp de execuție rămas egale.
 */
void sort(coada *queue, int n) {
  for (int i = 1; i <= n; i++) {
    findElem(queue, n, i);
  }
}

/* functie care adauga un numar de <numOfTasks> in coada de asteptare */
void add_tasks(int numOfTasks, int timpExec, int prioritate, int *v,
               coada *coada_waiting, FILE *file) {
  for (int i = 1; i <= numOfTasks; i++) {
    Task *task;
    int k = i - 1;

    while (k < NR_MAX_TASKURI) {
      if (v[k] == 0) {
        task = creare_task(k + 1, NULL, prioritate, timpExec);
        v[k] = 1;
        break;
      }
      k++;
    }
    enQueue(coada_waiting, task);
    fprintf(file, "Task created successfully : ID %d.\n", task->ID);
  }
  sort(coada_waiting, coada_waiting->lungime);
}

/* functie care cauta un task in coada running */
int get_running(int task_id, coada *coada_running, FILE *file) {
  int flag = 0;
  for (int i = 0; i < coada_running->lungime; i++) {
    Task *task = frontQueue(coada_running);
    if (task->ID == task_id) {
      fprintf(file, "Task %d is running (remaining_time = %d).\n", task_id,
              task->timpExec);
      flag = 1;
    }
    deQueue(coada_running);
    enQueue(coada_running, task);
  }
  return flag;
}

/* functie care cauta un task in coada waiting */
int get_wating(int task_id, coada *coada_waiting, FILE *file) {
  int flag = 0;
  for (int i = 0; i < coada_waiting->lungime; i++) {
    Task *task = frontQueue(coada_waiting);
    if (task->ID == task_id) {
      fprintf(file, "Task %d is waiting (remaining_time = %d).\n", task->ID,
              task->timpExec);
      flag = 1;
    }
    deQueue(coada_waiting);
    enQueue(coada_waiting, task);
  }
  return flag;
}

/* functie care cauta un task in coada finished */
int get_finished(int task_id, coada *coada_finished, FILE *file) {
  int flag = 0;
  for (int i = 0; i < coada_finished->lungime; i++) {
    Task *task = frontQueue(coada_finished);
    if (task->ID == task_id) {
      fprintf(file, "Task %d is finished (executed_time = %d).\n", task_id,
              task->timpExec);
      flag = 1;
    }
    deQueue(coada_finished);
    enQueue(coada_finished, task);
  }
  return flag;
}

/* functie care cauta un task in toate cozile */
void get_task(int task_id, coada *coada_waiting, coada *coada_running,
              coada *coada_finished, FILE *file) {
  if (get_running(task_id, coada_running, file) == 1) {
    return;
  }
  if (get_wating(task_id, coada_waiting, file) == 1) {
    return;
  }
  if (get_finished(task_id, coada_finished, file) == 1) {
    return;
  }
  fprintf(file, "Task %d not found.\n", task_id);
}

/* functie care creeaza o stiva */
stiva *creare_stiva() {
  stiva *stack = NULL;
  stack = (stiva *)malloc(sizeof(stiva));

  stack->lungime = 0;
  stack->head = NULL;

  return stack;
}

/* functie care adauga un element in stiva */
void push(stiva *stiva, void *val) {
  nod *node = NULL;
  node = creare_nod(val);

  node->next = stiva->head;
  stiva->head = node;
  stiva->lungime++;
}

/* functie care verifica daca o stiva e goala */
int stiva_goala(stiva *stiva) {
  if (stiva->lungime == 0) {
    return 1;
  }
  return 0;
}

/* functie care elimina un element din stiva */
int pop(stiva *stiva) {
  if (stiva_goala(stiva) == 1) {
    return 0;
  }

  nod *p = stiva->head;

  stiva->head = stiva->head->next;
  stiva->lungime = stiva->lungime - 1;

  p->next = NULL;
  free(p);

  return 1;
}

/* functie care returneaza elementul din varful stivei fara a modifica stiva */
Thread *frontStack(stiva *stack) {
  Thread *thread = (Thread *)(stack->head->info);
  return thread;
}

/* functie care creeaza un duplicat al unei stive */
stiva *duplicateStack(stiva *stack) {
  stiva *dup = creare_stiva();
  stiva *aux = creare_stiva();

  while (stiva_goala(stack) == 0) {
    Thread *thread = frontStack(stack);
    pop(stack);
    push(aux, thread);
  }

  while (stiva_goala(aux) == 0) {
    Thread *thread = frontStack(aux);
    pop(aux);
    push(stack, thread);
    push(dup, thread);
  }

  return dup;
}

/* functie care inverseaza o stiva */
stiva *reverseStack(stiva *stack) {
  stiva *dup = duplicateStack(stack);
  stiva *reverse = creare_stiva();

  while (stiva_goala(dup) == 0) {
    Thread *thread = frontStack(dup);
    pop(dup);
    push(reverse, thread);
  }

  return reverse;
}

/* functie care creeaza un element de tip Thread */
Thread *creare_thread(int x) {
  Thread *thread = NULL;
  thread = (Thread *)malloc(sizeof(Thread));

  thread->ID = x;

  return thread;
}

/* functie care creeaza stiva cu thread-uri */
stiva *creare_pool_threaduri(int n) {
  stiva *stack = creare_stiva();

  for (int i = n - 1; i >= 0; i--) {
    Thread *thread = creare_thread(i);
    push(stack, thread);
  }

  return stack;
}