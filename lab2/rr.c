#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 remaining_time;
  bool started;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}



int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

// Initialize the burst time and start time to the first process 
struct process *active_process;

u32 initial_time = data[0].arrival_time;

// initialize the struct field for remaining time and started boolean
for (u32 i = 0; i < size; i++) {
  active_process = &data[i];
  active_process->remaining_time = active_process->burst_time;
  active_process->started = false;
  if (active_process->arrival_time < initial_time) initial_time = active_process->arrival_time;
}

bool done = false;
u32 slice_counter = 1;
u32 elapsed_time = initial_time;

if (quantum_length == 0) done = true;
struct process *incoming_process;

// round robin logic
while (!done) {

    // check and add new processes
    for (u32 i = 0; i < size; i++) {
      incoming_process = &data[i]; 
      if (incoming_process->arrival_time == initial_time) TAILQ_INSERT_TAIL(&list, incoming_process, pointers);
    }

    // If the process has used its time slice, move it to the back of the queue
    if (slice_counter == quantum_length + 1 && active_process->remaining_time > 0) {
      TAILQ_INSERT_TAIL(&list, active_process, pointers);
      slice_counter = 1;
    }

    
    if (slice_counter == 1) {
      if (TAILQ_EMPTY(&list)) return -1;
      active_process = TAILQ_FIRST(&list);
      TAILQ_REMOVE(&list, active_process, pointers);
    }
    

    if (!active_process->started) {
        total_response_time += elapsed_time - active_process->arrival_time;
        active_process->started = true;
    }

    if (slice_counter < quantum_length + 1) {
      if (active_process->remaining_time > 0) {
          active_process->remaining_time -= 1;
          elapsed_time++;
      }

      // adds waiting time
      if (active_process->remaining_time == 0) {
        total_waiting_time += elapsed_time - active_process->arrival_time - active_process->burst_time;
        slice_counter = 0;
      }
    }
// always need to increment time
    slice_counter++;
    initial_time++;

    // checks if all processes are done
    done = true;
    for (u32 i = 0; i < size; i++) {
      if (data[i].remaining_time > 0) {
          done = false;
          break;
      }
    }
}

  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}