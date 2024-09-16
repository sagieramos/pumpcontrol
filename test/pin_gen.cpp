#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct PinGen {
  unsigned long gen = 0;
  uint8_t pin[4];

  PinGen() : gen(0) { regenerate(); }

  void regenerate() {
    srand(time(NULL));
    gen = (rand() % (9999999999UL - 1000000000UL + 1)) + 1000000000UL;

    char pin_str[11];
    sprintf(pin_str, "%lu", gen);

    for (int i = 0; i < 4; i++) {
      int k = pin_str[i] - pin_str[7 - i];
      pin[i] = static_cast<uint8_t>(abs(k));
    }
  }
};

int main() {
  PinGen pin_gen;
  printf("Generated pin: %lu\n", pin_gen.gen);
  printf("Pin: %d%d%d%d\n", pin_gen.pin[0], pin_gen.pin[1], pin_gen.pin[2],
         pin_gen.pin[3]);

  sleep(1);

  pin_gen.regenerate();

  printf("Generated pin: %lu\n", pin_gen.gen);
  printf("Pin: %d%d%d%d\n", pin_gen.pin[0], pin_gen.pin[1], pin_gen.pin[2],
         pin_gen.pin[3]);
  return 0;
}