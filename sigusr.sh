#!/bin/bash

# Sprawdzenie, czy podano argument
if [ -z "$1" ]; then
  echo "Użycie: $0 <1|2>"
  exit 1
fi

# Wybór sygnału na podstawie parametru
if [ "$1" == "1" ]; then
  SIGNAL=SIGUSR1
elif [ "$1" == "2" ]; then
  SIGNAL=SIGUSR2
else
  echo "Nieprawidłowy argument. Użyj 1 lub 2."
  exit 1
fi

# Znalezienie PID procesu "dyspozytor"
PID=$(pgrep dyspozytor)

# Sprawdzenie, czy proces został znaleziony
if [ -z "$PID" ]; then
  echo "Proces 'dyspozytor' nie został znaleziony."
  exit 1
fi

# Wysłanie wybranego sygnału do znalezionego procesu
kill -$SIGNAL $PID

# Komunikat potwierdzający wykonanie
if [ $? -eq 0 ]; then
  echo "Wysłano sygnał $SIGNAL do procesu 'dyspozytor' (PID: $PID)."
else
  echo "Nie udało się wysłać sygnału do procesu 'dyspozytor'."
  exit 1
fi
