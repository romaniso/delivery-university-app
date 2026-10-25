# Sprawozdanie z eksperymentów i statystyk

## 1. Cel zadania

Celem było dodanie statystyk do symulacji kolejki paczek oraz porównanie trzech
strategii przydziału:

- A — FIFO, czyli kolejność przyjęcia,
- B — najpierw paczki `URGENT`,
- C — pełny score uwzględniający priorytet, czas oczekiwania i termin.

Każdy pomiar jest wykonywany dla tego samego pliku wejściowego i osobno dla
strategii A, B oraz C.

## 2. Schemat klas

```text
Warehouse
  ├── vector<Driver>
  └── vector<Delivery>
          │
          ▼
Simulation
  ├── DeliveryManager
  │     └── vector<DriverQueue>
  └── vector<DriverDay>
          │
          ▼
statistics::collect()
  └── statistics::Report
```

Najważniejsze pliki:

- `src/storage/Warehouse.h` — przechowuje kierowców i paczki,
- `src/simulation/Simulation.h` — prowadzi symulację dzień po dniu,
- `src/assignment/DeliveryManager.h` — przydziela paczki kierowcom,
- `src/statistics/Statistics.h` — oblicza statystyki końcowe,
- `src/main.cpp` — wyświetla statystyki i zapisuje `output/metrics.csv`.

## 3. Zbierane statystyki

| Statystyka | Sposób obliczenia |
|---|---|
| Średni czas oczekiwania | średnia z `deliveredDay - arrivalDay` dla dostarczonych paczek |
| Maksymalny czas oczekiwania | największa wartość `deliveredDay - arrivalDay` |
| Obsłużone w terminie | `deliveredDay <= deadlineDay` |
| Spóźnione | `deliveredDay > deadlineDay` |
| Pozostałe zgłoszenia | paczki, które nie mają statusu `DELIVERED` po końcu symulacji |
| Wykorzystanie wykonawców | suma wykorzystanej masy / suma dostępnej ładowności w dniach symulacji |

Wykorzystanie wykonawców jest liczone jako wykorzystanie ładowności. Dzięki
temu wynik można porównywać między scenariuszami z różną liczbą paczek.

## 4. Scenariusze eksperymentalne

| Scenariusz | Plik | Charakterystyka |
|---|---|---|
| Małe obciążenie | `data/experiment-low.txt` | 6 paczek i duży zapas ładowności |
| Przeciążenie systemu | `data/experiment-overload.txt` | więcej kilogramów niż kierowcy mogą przewieźć w jednym dniu |
| Skokowy napływ | `data/experiment-spike.txt` | większość paczek pojawia się pierwszego dnia |

Eksperymenty uruchamia skrypt `experiments/run_experiments.ps1`. Wymaga on
ścieżki do zbudowanego programu:

```powershell
.\experiments\run_experiments.ps1 -Executable .\build\Debug\kolejka.exe
```

Wyniki trzech uruchomień są zapisywane do:

```text
output/experiments.csv
```

## 5. Tabela wyników

Plik `output/experiments.csv` zawiera jeden wiersz dla każdej pary scenariusz–
strategia. Tabela ma następujące kolumny:

```text
scenario,strategy,total_deliveries,delivered,on_time,late,remaining,
average_waiting_days,max_waiting_days,executor_utilization_percent
```

W tym środowisku nie ma dostępnego kompilatora CMake/C++, dlatego nie wpisuję
do sprawozdania zmyślonych wartości liczbowych. Po zbudowaniu programu tabela
jest generowana automatycznie przez skrypt eksperymentalny.

## 6. Analiza porównawcza

W scenariuszu małego obciążenia różnice między strategiami powinny być małe,
ponieważ kierowcy mają wystarczającą ładowność. Najważniejszym wynikiem będzie
wysoki procent paczek dostarczonych w terminie.

W scenariuszu przeciążenia strategie muszą odłożyć część paczek na następne
dni. Strategia B powinna szybciej obsługiwać paczki `URGENT`, ale może zwiększać
czas oczekiwania paczek `NORMAL`. Strategia C powinna lepiej reagować na paczki
zbliżające się do terminu, ponieważ score zawiera aging i deadline.

W scenariuszu skokowego napływu widoczny będzie wpływ kolejki priorytetowej.
Paczki przyjęte wcześniej, ale nieprzydzielone, mają status `POSTPONED` i są
obsługiwane przed nowymi paczkami `WAITING`. Można więc porównać maksymalny i
średni czas oczekiwania oraz liczbę spóźnień.

## 7. Wnioski

1. Przy małym obciążeniu najważniejsza jest poprawność i terminowość, a wybór
   strategii ma niewielki wpływ na wynik.
2. Przy przeciążeniu nie da się dostarczyć wszystkich paczek w tym samym dniu,
   więc rośnie czas oczekiwania i wykorzystanie kierowców.
3. FIFO jest proste i przewidywalne, ale nie rozróżnia ważności paczek.
4. Strategia priorytetowa poprawia obsługę `URGENT`, lecz może powodować
   głodzenie `NORMAL` przy stałym napływie pilnych paczek.
5. Score jest najbardziej rozbudowaną strategią, ponieważ uwzględnia również
   czas oczekiwania i termin. Nadal jest to jednak algorytm zachłanny.
6. Złożoność czasowa symulacji pozostaje zgodna z analizą: `O(DN(K + log N))`,
   ponieważ dodanie statystyk wykonuje tylko dodatkowe przejście po paczkach i
   kursach, czyli nie zmienia dominującej złożoności przydziału.
