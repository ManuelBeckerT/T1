char *line = NULL;
FILE *file;
size_t len = 0;
ssize_t read;

file = fopen(argv[1], "r");
int process_count = lines(argv[1]);
node* processes = (node*) malloc(sizeof(node)*process_count);

int current_time = 0;
//int finished = 0;

if (file) {
    int i = 0;
    while ((read = getline(&line, &len, file)) != -1) {
      //printf("%s", line);
      ArrayList* list = arraylist_init();
      char* token = strtok(line, " ");
      char* name = malloc(sizeof(token));
      strcpy(name, token);
      token = strtok(NULL, " ");

      int initial_time = atoi(token);
      token = strtok(NULL, " ");

      int cpu_burst = atoi(token);
      token = strtok(NULL, " ");

      while (token) {
        //printf("'%s'\n", token);
        arraylist_append(list, atoi(token));
        token = strtok(NULL, " ");
      }
      //arraylist_print(list);
      Process* process = malloc(sizeof(Process));
      process->timeline = list;
      process->name = name;
      process->curr_idx = 0;
      process->cpu_burst = cpu_burst;
      process->initial_time = initial_time;
      processes[i].item = process;
      i+=1;
    }
    free(line);
}

// CONTAR LINEAS

/* Falló la apertura del archivo */
if(!input_file)
{
  printf("¡El archivo %s no existe!\n", argv[1]);
  return 2;
}

int count = 0;  // Line counter (result)
char c;  // To store a character read from file

// Check if file exists
if (input_file == NULL)
{
    printf("Could not open file %s", argv[1]);
    return 0;
}

// Extract characters from file and store in character c
for (c = getc(input_file); c != EOF; c = getc(input_file))
    if (c == '\n') // Increment count if this character is newline
        count = count + 1;

// Close the file
printf("The file %s has %d lines\n ", argv[1], count);
