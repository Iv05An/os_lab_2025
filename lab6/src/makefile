# CC = gcc
# LDFLAGS = -lpthread

# all: client server

# client: client.c
# 	$(CC) -o client client.c $(LDFLAGS)

# server: server.c
# 	$(CC) -o server server.c $(LDFLAGS)

# clean:
# 	rm -f client server

# .PHONY: all clean


CC = gcc
LDFLAGS = -lpthread

# Цели
TARGETS = client server
LIBRARY = factorial_lib.o

all: $(TARGETS)

# Сборка библиотеки
$(LIBRARY): factorial_lib.c factorial_lib.h
	$(CC) -c -o $(LIBRARY) factorial_lib.c

# Сборка клиента с библиотекой
client: client.c $(LIBRARY)
	$(CC) -o client client.c $(LIBRARY) $(LDFLAGS)

# Сборка сервера с библиотекой
server: server.c $(LIBRARY)
	$(CC) -o server server.c $(LIBRARY) $(LDFLAGS)

# Очистка
clean:
	rm -f $(TARGETS) $(LIBRARY)

# Пересборка
rebuild: clean all

.PHONY: all clean rebuild