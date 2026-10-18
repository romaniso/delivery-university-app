# Plan prac — Kolejka Paczek

Zadania w kolejności etapów z README. Każdy etap = osobna gałąź i pull request.
Odhaczamy `[x]` w tym samym PR, który zamyka zadanie.

## Do ustalenia przed startem

- [ ] Jednostka czasu w programie: dni czy godziny (od tego zależy aging, deadline i symulacja).
- [ ] Format rekordu `DELIVERY` w pliku wejściowym (propozycja: `DELIVERY;adres;waga_kg;NORMAL|URGENT;dzien_przyjecia;termin`).
- [ ] Kolejka kierowcy: dwie `std::queue` (URGENT + NORMAL) czy `std::deque` z wstawianiem na początek.
- [ ] Wagi w score: ile punktów za `URGENT`, za każdy dzień czekania, za zbliżający się termin.

## Etap 0 — szkielet bez bazy danych

- [x] Usunięcie SQLite, `sql/`, `src/db/`.
- [x] `model::Driver`.
- [x] `storage::Warehouse` z kierowcami w `std::vector`.
- [x] `io::InputFile` (rekord `DRIVER`) i `io::OutputFile` (lista kierowców).
- [x] `assignment::DriverCapacityGreater` + `std::sort` w `main.cpp`.
- [x] README: plik wejściowy/wyjściowy, wykorzystanie STL.
- [ ] Commit i PR do `main` (najlepiej z gałęzi o nazwie pasującej do zmian, np. `feat/no-db-io`).

## Etap 1 — model (`src/model/`)

- [ ] Enum `Priority` (`NORMAL`, `URGENT`) + zamiana na tekst i z tekstu.
- [ ] Enum `DeliveryStatus` (np. `WAITING`, `ASSIGNED`, `DEPARTED`, `DELIVERED`, `CANCELLED`) + zamiana na tekst.
- [ ] Enum `QueueStatus` (`OPEN`, `DEPARTED`) + zamiana na tekst.
- [ ] Klasa `Delivery`: `id`, adres, waga, priorytet, dzień przyjęcia, termin, status, `id` kierowcy.
- [ ] Klasa `DriverQueue`: `id` kierowcy, kolejka(i) paczek (`std::queue`), status, aktualne obciążenie w kg.

## Etap 2 — dane i pliki (`src/storage/`, `src/io/`, `data/`)

- [ ] `Warehouse::addDelivery`, wyszukiwanie kierowcy i paczki po `id`.
- [ ] `InputFile`: obsługa rekordu `DELIVERY` (walidacja jak przy `DRIVER`, z numerem linii).
- [ ] `data/input.txt`: ~30 paczek NORMAL/URGENT, łącznie więcej kg niż suma ładowności dostępnych kierowców.
- [ ] `OutputFile`: przydział paczek do kierowców (lista adresów dla każdego), paczki pozostałe w magazynie, podsumowanie.

## Etap 3 — przydział (`src/assignment/`)

- [ ] Funkcja score: priorytet + aging (czas oczekiwania) + bliskość terminu.
- [ ] Funktor `DeliveryScoreLess` dla `std::priority_queue` (uwaga: `<` = najwyższy score na górze).
- [ ] Kolejka paczek czekających: `std::priority_queue<Delivery, std::vector<Delivery>, DeliveryScoreLess>`.
- [ ] Zachłanny `assign()`: zdejmuj paczkę z góry kolejki, przydziel dostępnemu kierowcy z wolnym miejscem; jeśli nikt jej nie weźmie — wraca do magazynu.
- [ ] Zdarzenia:
  - [ ] `NEW_DELIVERY` — nowa paczka do kolejki priorytetowej.
  - [ ] `DRIVER_AVAILABLE` — kierowca wraca, ponowny `assign()`.
  - [ ] `DELIVERY_CANCELLED` — `std::priority_queue` nie usuwa ze środka: oznacz paczkę jako `CANCELLED` i pomijaj ją przy zdejmowaniu.
  - [ ] `DEPART` — kolejka kierowcy zamknięta (`DEPARTED`), nic do niej nie dokładamy.
  - [ ] `DELIVERED` — zdejmowanie paczek z kolejki kierowcy, zmiana statusu.
- [ ] Przypadki brzegowe (każdy jako osobny plik w `data/`, np. `data/edge-heavy.txt`):
  - [ ] paczka cięższa niż każdy kierowca,
  - [ ] brak dostępnych kierowców,
  - [ ] paczka `URGENT`, gdy wszystkie kolejki są `DEPARTED`,
  - [ ] same paczki `URGENT`.

## Etap 4 — symulacja (`src/simulation/`)

- [ ] Pętla po dniach: nowe paczki, przydział, wyjazdy, doręczenia, aging pozostałych.
- [ ] Strategie jako różne komparatory/funktory:
  - [ ] A — FIFO (kolejność przyjęcia, `std::queue`),
  - [ ] B — sam priorytet,
  - [ ] C — pełny score.
- [ ] Metryki: średni czas oczekiwania, przekroczone terminy, wykorzystanie ładowności, paczki pozostałe w magazynie.
- [ ] Zapis metryk do `output/` (CSV do wykresów).
- [ ] Generator losowych plików wejściowych (stałe ziarno, żeby wyniki dało się powtórzyć).

## Etap 5 — oddanie projektu

- [ ] README: aktualny format pliku wejściowego, przykładowy wynik, opis strategii.
- [ ] Porównanie strategii A/B/C na tych samych danych (tabela lub wykres z CSV).
- [ ] Przegląd wymagań: `std::vector`, `std::queue`, `std::priority_queue`, `std::sort`, własne komparatory/funktory — każde użyte i opisane.
- [ ] Projekt buduje się bez ostrzeżeń na każdym systemie w zespole.
