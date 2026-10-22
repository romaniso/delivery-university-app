# Plan prac — Kolejka Paczek

Zadania w kolejności etapów z README. Każdy etap = osobna gałąź i pull request.
Odhaczamy `[x]` w tym samym PR, który zamyka zadanie.

## Do ustalenia przed startem

- [x] Jednostka czasu w programie: dni, numerowane od 1 (`arrivalDay`, `deadlineDay`, `deliveredDay` w `Delivery`).
- [x] Format rekordu `DELIVERY` w pliku wejściowym: `DELIVERY;adres;waga_kg;NORMAL|URGENT;dzien_przyjecia;termin`.
- [x] Kolejka kierowcy: jedna `std::queue<int>` z `id` paczek, ułożona wg strategii przy przydziale.
- [x] Paczki, które się nie zmieściły: status `POSTPONED`, zostają w `Warehouse`, następnego dnia przed `WAITING`. Bez anulowania paczek.
- [ ] Wagi w score: ile punktów za `URGENT`, za każdy dzień czekania, za zbliżający się termin (do ustalenia po porównaniu strategii na metrykach).

## Etap 0 — szkielet bez bazy danych

- [x] Usunięcie SQLite, `sql/`, `src/db/`.
- [x] `model::Driver`.
- [x] `storage::Warehouse` z kierowcami w `std::vector`.
- [x] `io::InputFile` (rekord `DRIVER`) i `io::OutputFile` (lista kierowców).
- [x] `assignment::DriverCapacityGreater` + `std::sort` w `main.cpp`.
- [x] README: plik wejściowy/wyjściowy, wykorzystanie STL.

## Etap 1 — model (`src/enums/`, `src/model/`)

- [x] Enum `Priority` (`NORMAL`, `URGENT`) + zamiana na tekst i z tekstu.
- [x] Enum `DeliveryStatus` (`WAITING`, `POSTPONED`, `ASSIGNED`, `IN_TRANSIT`, `DELIVERED`) + zamiana na tekst.
- [x] Enum `QueueStatus` (`OPEN`, `DEPARTED`) + zamiana na tekst.
- [x] Klasa `Delivery`: `id`, adres, waga, priorytet, dzień przyjęcia, termin, status, `id` kierowcy, dzień doręczenia.
- [x] Klasa `DriverQueue`: `id` kierowcy, kolejka `deliveries` (`std::queue<int>` z `id` paczek), status, aktualne obciążenie w kg.

## Etap 2 — dane i pliki (`src/storage/`, `src/io/`, `data/`)

- [x] `Warehouse::addDelivery`, wyszukiwanie kierowcy i paczki po `id`.
- [x] `InputFile`: obsługa rekordu `DELIVERY` (walidacja jak przy `DRIVER`, z numerem linii).
- [x] `data/input.txt`: 30 paczek NORMAL/URGENT w dniach 1–5, łącznie więcej kg niż suma ładowności dostępnych kierowców (dni 1 i 3 powyżej 1700 kg).
- [x] `OutputFile`: lista wczytanych paczek.

## Etap 3 — przydział (`src/assignment/`)

- [x] `OutputFile`: kursy dzień po dniu (lista adresów każdego kierowcy w kolejności doręczania), paczki pozostałe w magazynie; w tabeli paczek kierowca i dzień doręczenia.
- [x] `model::DriverDay` — kurs kierowcy w jednym dniu (zapisany przy doręczeniu, do raportu).
- [x] Funkcja score: priorytet + aging (czas oczekiwania) + bliskość terminu (`assignment/DeliveryScore.h`).
- [x] Funktor `ScoreOrder` dla `std::priority_queue` (uwaga: `true` = a ma być niżej niż b; `POSTPONED` zawsze przed `WAITING`).
- [x] Kolejka paczek czekających: `std::priority_queue<int, std::vector<int>, ScoreOrder>` z `id` paczek `WAITING` i `POSTPONED`, budowana od nowa każdego dnia.
- [x] Zachłanny `assign()` w `assignment::DeliveryManager`: zdejmuj paczkę z góry kolejki, przydziel kierowcy metodą best fit (push do `deliveries`); jeśli nikt jej nie weźmie — `POSTPONED`.
- [x] Zdarzenia (metody `DeliveryManager`):
  - [x] `NEW_DELIVERY` — filtr `arrivalDay <= dziś` w `assign()`.
  - [x] `DRIVER_AVAILABLE` — `driversAvailable()`: kierowca wraca, kolejka znów `OPEN`.
  - [x] `DEPART` — `depart()`: kolejka kierowcy zamknięta (`DEPARTED`), paczki `IN_TRANSIT`.
  - [x] `DELIVERED` — `deliver()`: zdejmowanie paczek z kolejki kierowcy, `DELIVERED`, zwraca `DriverDay`.
- [x] Ostrzeżenie po wczytaniu: paczka cięższa niż każdy dostępny kierowca / brak dostępnych kierowców.
- [x] Przypadki brzegowe (każdy jako osobny plik w `data/`):
  - [x] `edge-heavy.txt` — paczka cięższa niż każdy kierowca,
  - [x] `edge-no-drivers.txt` — brak dostępnych kierowców,
  - [x] `edge-full.txt` — paczka `URGENT`, gdy jedyny kierowca jest pełny,
  - [x] `edge-all-urgent.txt` — same paczki `URGENT`.

## Etap 4 — symulacja (`src/simulation/`)

- [x] Pętla po dniach: `simulation::Simulation` (jedna symulacja = jedna strategia, `run()`, `days()`, `driverDays()`).
- [x] Strategie jako różne komparatory/funktory (`assignment/Comparators.h`) + `enums::Strategy`:
  - [x] A — FIFO (kolejność przyjęcia, `FifoOrder`),
  - [x] B — sam priorytet (`PriorityOrder`),
  - [x] C — pełny score (`ScoreOrder`).
- [ ] Wybór strategii przez użytkownika (teraz stała `STRATEGY` w `main.cpp`).
- [ ] Metryki: średni czas oczekiwania, przekroczone terminy, wykorzystanie ładowności, paczki pozostałe w magazynie.
- [ ] Zapis metryk do `output/` (CSV do wykresów).
- [ ] Generator losowych plików wejściowych (stałe ziarno, żeby wyniki dało się powtórzyć).

## Etap 5 — oddanie projektu

- [ ] CLI: dodać input uzytkownika do wskazania pliku wejściowego z foldera data; zamienić format pliku wejściowego oraz wyjściowego na csv
- [ ] README: aktualny format pliku wejściowego, przykładowy wynik, opis strategii.
- [ ] Porównanie strategii A/B/C na tych samych danych (tabela lub wykres z CSV).
- [ ] Przegląd wymagań: `std::vector`, `std::queue`, `std::priority_queue`, `std::sort`, własne komparatory/funktory — każde użyte i opisane.
- [ ] Projekt buduje się bez ostrzeżeń na każdym systemie w zespole.
