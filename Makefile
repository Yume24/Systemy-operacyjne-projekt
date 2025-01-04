# Kompilator i flagi
CC = gcc

# Pliki źródłowe i obiektowe
SRC = dyspozytor.c dyspozytor_utils.c pracownik.c pracownik_ciezarowka_utils.c ciezarowka.c utils.c
OBJ = $(SRC:.c=.o)
BIN = dyspozytor pracownik ciezarowka

# Domyślna kompilacja
all: $(BIN)

# Kompilacja dyspozytora
dyspozytor: dyspozytor.o dyspozytor_utils.o utils.o
	$(CC) $(CFLAGS) -o dyspozytor dyspozytor.o dyspozytor_utils.o utils.o

# Kompilacja pracownika
pracownik: pracownik.o pracownik_ciezarowka_utils.o utils.o
	$(CC) $(CFLAGS) -o pracownik pracownik.o pracownik_ciezarowka_utils.o utils.o

# Kompilacja ciężarówki
ciezarowka: ciezarowka.o pracownik_ciezarowka_utils.o utils.o
	$(CC) $(CFLAGS) -o ciezarowka ciezarowka.o pracownik_ciezarowka_utils.o utils.o

# Reguły dla plików obiektowych
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Czyszczenie plików tymczasowych
clean:
	rm -f $(OBJ) $(BIN)

# Resetowanie wszystkiego
reset: clean
	rm -f *~ core.*
