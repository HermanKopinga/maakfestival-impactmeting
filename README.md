Source code voor het feedback knoppenpaneel (formeel de impactmeting) voor Maakfestival Groningen 2025 in en rond het Forum in Groningen.

Hardware:
  - 22 knoppen waarvan 12 met geintegreerd ledje
  - 10 arcade knoppen zonder ledje
  - 10 losse ledjes
  - Teensy 3.2 (blijkbaar bijna vintage microcontroller, PRJC verkoopt alleen de 4.0 nog)
    - Heeft 34 gpio waarvan 24 breadboard compatible zijn.
  - SX1509 multiplexer chip voor 16 extra gpio, super leuk ding!

Heb dus 44 GPIO nodig voor de knoppen en leds Plus 3 voor de SX1509. In totaal 47, Teensy plus expander is 50.

![Eerste versie](https://github.com/HermanKopinga/maakfestival-impactmeting/blob/main/eerste.jpg)
