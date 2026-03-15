#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <unistd.h>


typedef struct {
    const char **items;
    size_t count;
    size_t capacity;
} paths_to_bin;


#define da_append(dynamic_array, append_element)\
    do {\
        if(dynamic_array.count >= dynamic_array.capacity) {\
            if(dynamic_array.capacity == 0) dynamic_array.capacity = 256;\
            else dynamic_array.capacity *= 2;\
            dynamic_array.items = realloc(dynamic_array.items, dynamic_array.capacity*sizeof(*dynamic_array.items));\
        }\
        dynamic_array.items[dynamic_array.count++] = append_element;\
    } while(0)\


bool strinar(char compvar[], char *arr[], int size) {
    for (int i = 0; i<size; ++i) {
        if(strcmp(compvar, arr[i]) == 0) {
            return true;
        }
    }
    return false;
}

bool check_path_exec(char *exec_name) {
    char *path = strdup(getenv("PATH"));
    char *token = strtok(path, ":");
    paths_to_bin current_path = {0};
    DIR *dir;;
    struct dirent *entry;


    while(token) {
        da_append(current_path, token);
        token = strtok(NULL, ":");
    }

    for(int i = 0; i<current_path.count; ++i) {
        if ((dir = opendir(current_path.items[i])) == NULL) {
            //perror("opendir() error");
        } else {
            //puts("contents of current dir");
            while((entry = readdir(dir)) != NULL) {
                char full_path[1024];
                snprintf(full_path, sizeof(full_path), "%s/%s", current_path.items[i], entry->d_name);
                if((strcmp(exec_name, entry->d_name) == 0 && access(full_path, X_OK) == 0)) {
                    printf("%s is %s/%s\n", entry->d_name, current_path.items[i],entry->d_name);
                    free(current_path.items);
                    free(path);
                    return true;
                }

            }

            closedir(dir);
        }
    }
    free(path);
    free(current_path.items);
    return false;
}

int main(int argc, char *argv[]) {
    
    // Flush after every printf
    setbuf(stdout, NULL);

    char *builtin[] = {"echo", "exit", "type"}; 

    while(1) {
        
        printf("$ ");

        char command[1024];
        fgets(command, sizeof(command), stdin);

        //strcspn(something1, something2)
        //from my understanding strcspn is the number of bytes in something1 which arent
        //present in something2. Because only one enter is gotten and it's the last one
        //the length of the command without the \n would be the index of \n in the char array
        command[strcspn(command, "\n")] = '\0';

        //strcmp just compares strings
        if (strcmp(command, "exit") == 0){
            break;
        } else if (strncmp(command, "echo", 4) == 0) {
            printf("%s\n", command + 5);
            continue;
        //strncmp compares the first n bytes of a string
        } else if(strncmp(command, "type", 4) == 0) {
            if(strinar(command + 5, builtin, 3)) {
                printf("%s is a shell builtin\n", command + 5);
            } else if (!check_path_exec(command + 5)){
                printf("%s: not found\n", command + 5);
            }
        } else {
            printf("%s: not found\n", command);
        }

    }

    return 0;
}
