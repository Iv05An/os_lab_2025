// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>

// #include <errno.h>
// #include <getopt.h>
// #include <netdb.h>
// #include <netinet/in.h>
// #include <netinet/ip.h>
// #include <sys/socket.h>
// #include <sys/types.h>

// struct Server {
//   char ip[255];
//   int port;
// };

// uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
//   uint64_t result = 0;
//   a = a % mod;
//   while (b > 0) {
//     if (b % 2 == 1)
//       result = (result + a) % mod;
//     a = (a * 2) % mod;
//     b /= 2;
//   }

//   return result % mod;
// }

// bool ConvertStringToUI64(const char *str, uint64_t *val) {
//   char *end = NULL;
//   unsigned long long i = strtoull(str, &end, 10);
//   if (errno == ERANGE) {
//     fprintf(stderr, "Out of uint64_t range: %s\n", str);
//     return false;
//   }

//   if (errno != 0)
//     return false;

//   *val = i;
//   return true;
// }

// int main(int argc, char **argv) {
//   uint64_t k = -1;
//   uint64_t mod = -1;
//   char servers[255] = {'\0'}; // TODO: explain why 255

//   while (true) {
//     int current_optind = optind ? optind : 1;

//     static struct option options[] = {{"k", required_argument, 0, 0},
//                                       {"mod", required_argument, 0, 0},
//                                       {"servers", required_argument, 0, 0},
//                                       {0, 0, 0, 0}};

//     int option_index = 0;
//     int c = getopt_long(argc, argv, "", options, &option_index);

//     if (c == -1)
//       break;

//     switch (c) {
//     case 0: {
//       switch (option_index) {
//       case 0:
//         ConvertStringToUI64(optarg, &k);
//         // TODO: your code here
//         break;
//       case 1:
//         ConvertStringToUI64(optarg, &mod);
//         // TODO: your code here
//         break;
//       case 2:
//         // TODO: your code here
//         memcpy(servers, optarg, strlen(optarg));
//         break;
//       default:
//         printf("Index %d is out of options\n", option_index);
//       }
//     } break;

//     case '?':
//       printf("Arguments error\n");
//       break;
//     default:
//       fprintf(stderr, "getopt returned character code 0%o?\n", c);
//     }
//   }

//   if (k == -1 || mod == -1 || !strlen(servers)) {
//     fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
//             argv[0]);
//     return 1;
//   }

//   // TODO: for one server here, rewrite with servers from file
//   unsigned int servers_num = 1;
//   struct Server *to = malloc(sizeof(struct Server) * servers_num);
//   // TODO: delete this and parallel work between servers
//   to[0].port = 20001;
//   memcpy(to[0].ip, "127.0.0.1", sizeof("127.0.0.1"));

//   // TODO: work continiously, rewrite to make parallel
//   for (int i = 0; i < servers_num; i++) {
//     struct hostent *hostname = gethostbyname(to[i].ip);
//     if (hostname == NULL) {
//       fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
//       exit(1);
//     }

//     struct sockaddr_in server;
//     server.sin_family = AF_INET;
//     server.sin_port = htons(to[i].port);
//     server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

//     int sck = socket(AF_INET, SOCK_STREAM, 0);
//     if (sck < 0) {
//       fprintf(stderr, "Socket creation failed!\n");
//       exit(1);
//     }

//     if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
//       fprintf(stderr, "Connection failed\n");
//       exit(1);
//     }

//     // TODO: for one server
//     // parallel between servers
//     uint64_t begin = 1;
//     uint64_t end = k;

//     char task[sizeof(uint64_t) * 3];
//     memcpy(task, &begin, sizeof(uint64_t));
//     memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
//     memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

//     if (send(sck, task, sizeof(task), 0) < 0) {
//       fprintf(stderr, "Send failed\n");
//       exit(1);
//     }

//     char response[sizeof(uint64_t)];
//     if (recv(sck, response, sizeof(response), 0) < 0) {
//       fprintf(stderr, "Recieve failed\n");
//       exit(1);
//     }

//     // TODO: from one server
//     // unite results
//     uint64_t answer = 0;
//     memcpy(&answer, response, sizeof(uint64_t));
//     printf("answer: %llu\n", answer);

//     close(sck);
//   }
//   free(to);

//   return 0;
// }



// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>

// #include <errno.h>
// #include <getopt.h>
// #include <netdb.h>
// #include <netinet/in.h>
// #include <netinet/ip.h>
// #include <sys/socket.h>
// #include <sys/types.h>

// #include <pthread.h>

// struct Server {
//   char ip[255];
//   int port;
// };

// struct ClientArgs {
//   struct Server server;
//   uint64_t begin;
//   uint64_t end;
//   uint64_t mod;
//   uint64_t result;
// };

// uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
//   uint64_t result = 0;
//   a = a % mod;
//   while (b > 0) {
//     if (b % 2 == 1)
//       result = (result + a) % mod;
//     a = (a * 2) % mod;
//     b /= 2;
//   }
//   return result % mod;
// }

// bool ConvertStringToUI64(const char *str, uint64_t *val) {
//   char *end = NULL;
//   unsigned long long i = strtoull(str, &end, 10);
//   if (errno == ERANGE) {
//     fprintf(stderr, "Out of uint64_t range: %s\n", str);
//     return false;
//   }

//   if (errno != 0)
//     return false;

//   *val = i;
//   return true;
// }

// void* ServerThread(void* args) {
//   struct ClientArgs* cargs = (struct ClientArgs*)args;
  
//   printf("DEBUG: Connecting to %s:%d\n", cargs->server.ip, cargs->server.port);
  
//   struct hostent *hostname = gethostbyname(cargs->server.ip);
//   if (hostname == NULL) {
//     fprintf(stderr, "gethostbyname failed with %s\n", cargs->server.ip);
//     cargs->result = 1;
//     return NULL;
//   }

//   struct sockaddr_in server;
//   server.sin_family = AF_INET;
//   server.sin_port = htons(cargs->server.port);
  
//   // ИСПРАВЛЕНИЕ: правильно получаем адрес из h_addr_list
//   if (hostname->h_addr_list[0] == NULL) {
//     fprintf(stderr, "No address found for %s\n", cargs->server.ip);
//     cargs->result = 1;
//     return NULL;
//   }
//   memcpy(&server.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
//   // ИЛИ альтернативный вариант:
//   // server.sin_addr = *((struct in_addr *)hostname->h_addr_list[0]);

//   int sck = socket(AF_INET, SOCK_STREAM, 0);
//   if (sck < 0) {
//     fprintf(stderr, "Socket creation failed! Error: %s\n", strerror(errno));
//     cargs->result = 1;
//     return NULL;
//   }

//   printf("DEBUG: Attempting connection...\n");
  
//   if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
//     fprintf(stderr, "Connection failed to %s:%d. Error: %s\n", 
//             cargs->server.ip, cargs->server.port, strerror(errno));
//     close(sck);
//     cargs->result = 1;
//     return NULL;
//   }

//   printf("DEBUG: Connected successfully!\n");

//   char task[sizeof(uint64_t) * 3];
//   memcpy(task, &cargs->begin, sizeof(uint64_t));
//   memcpy(task + sizeof(uint64_t), &cargs->end, sizeof(uint64_t));
//   memcpy(task + 2 * sizeof(uint64_t), &cargs->mod, sizeof(uint64_t));

//   printf("DEBUG: Sending data: begin=%lu, end=%lu, mod=%lu\n", 
//          cargs->begin, cargs->end, cargs->mod);
  
//   if (send(sck, task, sizeof(task), 0) < 0) {
//     fprintf(stderr, "Send failed. Error: %s\n", strerror(errno));
//     close(sck);
//     cargs->result = 1;
//     return NULL;
//   }

//   printf("DEBUG: Data sent, waiting for response...\n");
  
//   char response[sizeof(uint64_t)];
//   if (recv(sck, response, sizeof(response), 0) < 0) {
//     fprintf(stderr, "Receive failed. Error: %s\n", strerror(errno));
//     close(sck);
//     cargs->result = 1;
//     return NULL;
//   }

//   memcpy(&cargs->result, response, sizeof(uint64_t));
//   printf("DEBUG: Received result: %lu\n", cargs->result);
  
//   close(sck);
//   return NULL;
// }

// int main(int argc, char **argv) {
//   uint64_t k = -1;
//   uint64_t mod = -1;
//   char servers_file[255] = {'\0'};

//   while (true) {
//     int current_optind = optind ? optind : 1;

//     static struct option options[] = {{"k", required_argument, 0, 0},
//                                       {"mod", required_argument, 0, 0},
//                                       {"servers", required_argument, 0, 0},
//                                       {0, 0, 0, 0}};

//     int option_index = 0;
//     int c = getopt_long(argc, argv, "", options, &option_index);

//     if (c == -1)
//       break;

//     switch (c) {
//     case 0: {
//       switch (option_index) {
//       case 0:
//         if (!ConvertStringToUI64(optarg, &k)) {
//           fprintf(stderr, "Invalid k value\n");
//           return 1;
//         }
//         break;
//       case 1:
//         if (!ConvertStringToUI64(optarg, &mod)) {
//           fprintf(stderr, "Invalid mod value\n");
//           return 1;
//         }
//         break;
//       case 2:
//         strncpy(servers_file, optarg, sizeof(servers_file) - 1);
//         break;
//       default:
//         printf("Index %d is out of options\n", option_index);
//       }
//     } break;

//     case '?':
//       printf("Arguments error\n");
//       break;
//     default:
//       fprintf(stderr, "getopt returned character code 0%o?\n", c);
//     }
//   }

//   if (k == -1 || mod == -1 || !strlen(servers_file)) {
//     fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
//             argv[0]);
//     return 1;
//   }

//   // Чтение файла с серверами
//   FILE* file = fopen(servers_file, "r");
//   if (file == NULL) {
//     fprintf(stderr, "Cannot open servers file: %s\n", servers_file);
//     return 1;
//   }

//   struct Server* servers = NULL;
//   int servers_num = 0;
//   char line[255];

//   while (fgets(line, sizeof(line), file)) {
//     char ip[255];
//     int port;
    
//     if (sscanf(line, "%[^:]:%d", ip, &port) == 2) {
//       servers = realloc(servers, (servers_num + 1) * sizeof(struct Server));
//       strncpy(servers[servers_num].ip, ip, sizeof(servers[servers_num].ip) - 1);
//       servers[servers_num].port = port;
//       servers_num++;
//     }
//   }
//   fclose(file);

//   if (servers_num == 0) {
//     fprintf(stderr, "No valid servers found in file\n");
//     free(servers);
//     return 1;
//   }

//   printf("Found %d servers\n", servers_num);

//   // Распределение работы между серверами
//   pthread_t threads[servers_num];
//   struct ClientArgs args[servers_num];

//   uint64_t numbers_per_server = k / servers_num;
//   uint64_t remainder = k % servers_num;
//   uint64_t current = 1;

//   for (int i = 0; i < servers_num; i++) {
//     args[i].server = servers[i];
//     args[i].begin = current;
//     args[i].end = current + numbers_per_server - 1;
//     args[i].mod = mod;

//     if (i < remainder) {
//       args[i].end++;
//     }

//     current = args[i].end + 1;

//     if (pthread_create(&threads[i], NULL, ServerThread, (void *)&args[i])) {
//       fprintf(stderr, "Error: pthread_create failed!\n");
//       free(servers);
//       return 1;
//     }
//   }

//   uint64_t total = 1;
//   for (int i = 0; i < servers_num; i++) {
//     pthread_join(threads[i], NULL);
//     if (args[i].result != 1) { // 1 indicates error
//       total = MultModulo(total, args[i].result, mod);
//     } else {
//       fprintf(stderr, "Server %s:%d failed\n", servers[i].ip, servers[i].port);
//     }
//   }

//   printf("Final answer: %lu\n", total);
//   free(servers);

//   return 0;
// }




#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <pthread.h>
#include "factorial_lib.h"  // Добавляем библиотеку

struct Server {
    char ip[255];
    int port;
};

struct ClientArgs {
    struct Server server;
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
    uint64_t result;
};

// Удаляем дублирующиеся функции MultModulo и ConvertStringToUI64

void* ServerThread(void* args) {
    struct ClientArgs* cargs = (struct ClientArgs*)args;
    
    printf("DEBUG: Connecting to %s:%d\n", cargs->server.ip, cargs->server.port);
    
    struct hostent *hostname = gethostbyname(cargs->server.ip);
    if (hostname == NULL) {
        fprintf(stderr, "gethostbyname failed with %s\n", cargs->server.ip);
        cargs->result = 1;
        return NULL;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(cargs->server.port);
    
    if (hostname->h_addr_list[0] == NULL) {
        fprintf(stderr, "No address found for %s\n", cargs->server.ip);
        cargs->result = 1;
        return NULL;
    }
    memcpy(&server.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
        fprintf(stderr, "Socket creation failed! Error: %s\n", strerror(errno));
        cargs->result = 1;
        return NULL;
    }

    printf("DEBUG: Attempting connection...\n");
    
    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "Connection failed to %s:%d. Error: %s\n", 
                cargs->server.ip, cargs->server.port, strerror(errno));
        close(sck);
        cargs->result = 1;
        return NULL;
    }

    printf("DEBUG: Connected successfully!\n");

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &cargs->begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &cargs->end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &cargs->mod, sizeof(uint64_t));

    printf("DEBUG: Sending data: begin=%lu, end=%lu, mod=%lu\n", 
           cargs->begin, cargs->end, cargs->mod);
    
    if (send(sck, task, sizeof(task), 0) < 0) {
        fprintf(stderr, "Send failed. Error: %s\n", strerror(errno));
        close(sck);
        cargs->result = 1;
        return NULL;
    }

    printf("DEBUG: Data sent, waiting for response...\n");
    
    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
        fprintf(stderr, "Receive failed. Error: %s\n", strerror(errno));
        close(sck);
        cargs->result = 1;
        return NULL;
    }

    memcpy(&cargs->result, response, sizeof(uint64_t));
    printf("DEBUG: Received result: %lu\n", cargs->result);
    
    close(sck);
    return NULL;
}

int main(int argc, char **argv) {
    uint64_t k = -1;
    uint64_t mod = -1;
    char servers_file[255] = {'\0'};

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"k", required_argument, 0, 0},
                                          {"mod", required_argument, 0, 0},
                                          {"servers", required_argument, 0, 0},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 0: {
            switch (option_index) {
            case 0:
                if (!ConvertStringToUI64(optarg, &k)) {
                    fprintf(stderr, "Invalid k value\n");
                    return 1;
                }
                break;
            case 1:
                if (!ConvertStringToUI64(optarg, &mod)) {
                    fprintf(stderr, "Invalid mod value\n");
                    return 1;
                }
                break;
            case 2:
                strncpy(servers_file, optarg, sizeof(servers_file) - 1);
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
        } break;

        case '?':
            printf("Arguments error\n");
            break;
        default:
            fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == -1 || mod == -1 || !strlen(servers_file)) {
        fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
                argv[0]);
        return 1;
    }

    // Чтение файла с серверами
    FILE* file = fopen(servers_file, "r");
    if (file == NULL) {
        fprintf(stderr, "Cannot open servers file: %s\n", servers_file);
        return 1;
    }

    struct Server* servers = NULL;
    int servers_num = 0;
    char line[255];

    while (fgets(line, sizeof(line), file)) {
        char ip[255];
        int port;
        
        if (sscanf(line, "%[^:]:%d", ip, &port) == 2) {
            servers = realloc(servers, (servers_num + 1) * sizeof(struct Server));
            strncpy(servers[servers_num].ip, ip, sizeof(servers[servers_num].ip) - 1);
            servers[servers_num].port = port;
            servers_num++;
        }
    }
    fclose(file);

    if (servers_num == 0) {
        fprintf(stderr, "No valid servers found in file\n");
        free(servers);
        return 1;
    }

    printf("Found %d servers\n", servers_num);

    // Распределение работы между серверами
    pthread_t threads[servers_num];
    struct ClientArgs args[servers_num];

    uint64_t numbers_per_server = k / servers_num;
    uint64_t remainder = k % servers_num;
    uint64_t current = 1;

    for (int i = 0; i < servers_num; i++) {
        args[i].server = servers[i];
        args[i].begin = current;
        args[i].end = current + numbers_per_server - 1;
        args[i].mod = mod;

        if (i < remainder) {
            args[i].end++;
        }

        current = args[i].end + 1;

        if (pthread_create(&threads[i], NULL, ServerThread, (void *)&args[i])) {
            fprintf(stderr, "Error: pthread_create failed!\n");
            free(servers);
            return 1;
        }
    }

    uint64_t total = 1;
    for (int i = 0; i < servers_num; i++) {
        pthread_join(threads[i], NULL);
        if (args[i].result != 1) { // 1 indicates error
            total = MultModulo(total, args[i].result, mod);
        } else {
            fprintf(stderr, "Server %s:%d failed\n", servers[i].ip, servers[i].port);
        }
    }

    printf("Final answer: %lu\n", total);
    free(servers);

    return 0;
}