# Agar.io Clone (Networking & Multithreading)

Klon popularnej gry sieciowej **Agar.io** zaimplementowany w architekturze **Klient-Serwer** przy użyciu niskopoziomowych socketów sieciowych (**TCP/UDP**). Projekt stanowił moje pierwsze praktyczne zderzenie z programowaniem sieciowym oraz zaawansowaną współbieżnością (*multithreading*).

## 🚀 Funkcje i Aspekty Techniczne

* **Architektura Klient-Serwer:** Rozdzielenie logiki gry (serwer) od warstwy renderowania i interfejsu gracza (klient).
* **Komunikacja TCP/UDP:** Wykorzystanie socketów sieciowych do przesyłania stanów obiektów, pozycji graczy oraz obsługi zdarzeń w czasie rzeczywistym.
* **Wielowątkowość (Multithreading):** Implementacja równoległego przetwarzania danych na serwerze w celu jednoczesnej obsługi wielu połączonych klientów oraz mechaniki gry.
* **Synchronizacja wątków:** Praktyczna walka z problemami współbieżności (np. wyścigi do danych / *data races*) i wdrażanie mechanizmów synchronizacji procesów.

[Watch preview](https://github.com/SmutnyBartus/Agario_fork/raw/main/tictactoe.mp4)
