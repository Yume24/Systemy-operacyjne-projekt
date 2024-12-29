# Kompilator i flagi
CC = gcc

# Pliki źródłowe i obiektowe
SRC = dyspozytor.c dyspozytor_utils.c pracownik.c pracownik_ciezarowka_utils.c ciezarowka.c
OBJ = $(SRC:.c=.o)
BIN = dyspozytor pracownik ciezarowka

# Domyślna kompilacja
all: $(BIN)

# Kompilacja dyspozytora
dyspozytor: dyspozytor.o dyspozytor_utils.o
	$(CC) -o dyspozytor dyspozytor.o dyspozytor_utils.o

# Kompilacja pracownika
pracownik: pracownik.o pracownik_ciezarowka_utils.o
	$(CC) -o pracownik pracownik.o pracownik_ciezarowka_utils.o

# Kompilacja ciężarówki
ciezarowka: ciezarowka.o pracownik_ciezarowka_utils.o
	$(CC) -o ciezarowka ciezarowka.o pracownik_ciezarowka_utils.o

# Kompilacja plików obiektowych
%.o: %.c
	$(CC) -c $< -o $@

# Usuwanie plików obiektowych i binarnych
clean:
	rm -f *.o $(BIN)

# Uruchomienie dyspozytora
run_dispatcher:
	./dyspozytor

# Uruchomienie pracownika
run_worker:
	./pracownik 1

# Uruchomienie ciężarówki
run_truck:
	./ciezarowka

# Reguły domyślne
.PHONY: all clean run_dispatcher run_worker run_truck
