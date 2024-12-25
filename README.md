# Projekt Monitorowania Środowiska - ESP8266 BME280 SGP30, InfluxDB i Grafana

Projekt przedstawia system monitorowania środowiska, który wykorzystuje moduł ESP8266 do zbierania danych, Ansible do konfiguracji środowiska oraz Grafanę do wizualizacji wyników.

---

## Opis projektu

System monitorowania środowiska pozwala na zbieranie danych z czujników temperatury, ciśnienia, wilgotności oraz jakości powietrza (TVOC, eCO2). Dane są przesyłane z ESP8266 do serwera, gdzie są zapisywane w bazie danych InfluxDB i wizualizowane w Grafanie.


---

##
                            +-------------------------+
                            |        ESP8266          |
                            |                         |
                            |   +-----------------+   |
                            |   |   VCC           |---|----> 3.3V
                            |   |   GND           |---|----> GND
                            |   |   SDA           |---|----> SDA (I2C)
                            |   |   SCL           |---|----> SCL (I2C)
                            |   +-----------------+   |
                            +-------------------------+
                                    |        |
                             +------|--------|------+
                             |      |        |      |
                   +----------------+    +----------------+
                   |    SGP30        |    |    BME280      |
                   |   +--------+    |    |   +--------+   |
                   |   | VCC    |----|----|---| VCC    |---|
                   |   | GND    |----|----|---| GND    |---|
                   |   | SDA    |----|----|---| SDA    |---|
                   |   | SCL    |----|----|---| SCL    |---|
                   |   +--------+    |    |   +--------+   |
                   +----------------+    +----------------+
Opis:
 - ESP8266: Mikrokontroler, który będzie pełnił rolę mastera I2C.
 - SGP30 i BME280: Dwa czujniki, które są podłączone równolegle do ESP8266.
 - VCC: Zasilanie 3.3V dla obu czujników.
 - GND: Masa, wspólna dla wszystkich komponentów.
 - SDA i SCL: Linie I2C do komunikacji z mikrokontrolerem. Są one połączone równolegle do obu czujników.

Uwagi:
 - Warto upewnić się, że oba czujniki mają różne adresy I2C. Domyślnie SGP30 ma adres 0x58, a BME280 0x76 lub 0x77 (zależnie od wersji).
 - Jeśli używasz ESP8266, linie SDA i SCL są domyślnie podłączone do pinów D2 (SDA) i D1 (SCL). Możesz je zmienić, jeśli tego wymaga Twoje oprogramowanie.
 - Taki schemat pozwala na jednoczesne podłączenie obu czujników do jednego mikrokontrolera za pomocą jednej magistrali I2C.


---

## Sekcja ESP 8622

Kod Arduino obsługuje:
- Łączenie z Wi-Fi.
- Odczyt danych z czujników:
  - **BME280**: temperatura, wilgotność, ciśnienie.
  - **SGP30**: TVOC, eCO2.
- Wysyłanie danych w formacie JSON do serwera HTTP.

Przykładowe elementy kodu:
- Inicjalizacja czujników.
- Wysyłanie danych za pomocą biblioteki `ESP8266HTTPClient`.

---

## Sekcja Ansible

Playbook automatyzuje konfigurację środowiska, w tym:
- Instalację zależności (np. Podman, Docker).
- Uruchamianie kontenerów:
  - **InfluxDB** (port 8086).
  - **Grafana** (port 3000).
- Podstawową konfigurację InfluxDB (użytkownik, organizacja, bucket).
- cd ansible && ansible-playbook -i inventory.yml setup.yml --ask-become-pass

---

## Sekcja Proxy (Python)

Serwer proxy napisany w Pythonie obsługuje:
- Przyjmowanie danych z ESP8266 w formacie JSON.
- Przechowywanie danych w InfluxDB.
- Flask API:
  - Endpoint `/data` do odbioru danych.

Funkcjonalność:
- Walidacja otrzymanych danych.
- Zapis danych w InfluxDB z odpowiednimi tagami i polami.

---

## Sekcja Grafana

TODO:
- Konfiguracja dashboardów w Grafanie.
- Wizualizacja danych, takich jak:
  - Temperatury.
  - Ciśnienie.
  - Wilgotność.
  - TVOC i eCO2.

---

## Wymagania systemowe

- ESP8266 z czujnikami **BME280** i **SGP30**.
- Serwer z Pythonem (Flask), InfluxDB i Grafaną.
- Zainstalowany Ansible do automatyzacji.

---

## Instalacja i uruchomienie

1. Wgraj kod Arduino na moduł ESP8266, podając adres IP serwera InfluxDB
2. Wykonaj playbook Ansible, aby skonfigurować InfluxDB i Grafanę. cd `ansible && ansible-playbook -i inventory.yml setup.yml --ask-become-pass`
3. Utwórz środowisko micromamba `create -n esp8622_sqp30_bme280 && micromamba activate esp8622_sqp30_bme280`
4. Zainstaluj `micromamba install influxdb_client flask`
5. Uruchom cd proxy && python main.py`
6. Skonfiguruj dashboardy w Grafanie.

---

## Licencja

Projekt dostępny na licencji MIT.
