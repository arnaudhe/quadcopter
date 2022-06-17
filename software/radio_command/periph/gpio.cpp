#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "gpio.hpp"

#define BCM2708_PERI_BASE   0x20000000
#define GPIO_BASE           (BCM2708_PERI_BASE + 0x200000)  /* GPIO controller */
#define BLOCK_SIZE          (4 * 1024)

// #define SET_GPIO_ALT(g, a)  *(gpio + (((g) / 10))) |= (((a) <= 3 ? (a) + 4 : (a) == 4 ? 3 : 2) << (((g) % 10) * 3))
// #define GPIO_PULL           *(gpio + 37)     // Pull up/pull down
// #define GPIO_PULLCLK0       *(gpio + 38)     // Pull up/pull down clock

Gpio::Gpio(int pin, Direction direction)
{
    int    mem_fd;
    void * gpio_map;

    this->pin       = pin;
    this->direction = direction;

    /* open /dev/mem */
    if ((mem_fd = open("/dev/gpiomem", O_RDWR | O_SYNC)) < 0)
    {
        printf("can't open /dev/gpiomem \n");
        exit(-1);
    }

    /* mmap GPIO */
    gpio_map = mmap(
        NULL,                   // Any adddress in our space will do
        BLOCK_SIZE,             // Map length
        PROT_READ | PROT_WRITE, // Enable reading & writting to mapped memory
        MAP_SHARED,             // Shared with other processes
        mem_fd,                 // File to map
        GPIO_BASE               // Offset to GPIO peripheral
    );

    close(mem_fd); //No need to keep mem_fd open after mmap

    if (gpio_map == MAP_FAILED)
    {
        fprintf(stderr, "mmap error %d : %s\n", (int)gpio_map, strerror(errno));
        exit(-1);
    }

    // Always use volatile pointer!
    this->gpio_reg = (volatile unsigned *)gpio_map;

    // Clears GPIO config (goes back to input mode)
    *(this->gpio_reg + ((this->pin) / 10)) &= ~(7 << (((this->pin) % 10) * 3));

    if (direction == OUTPUT)
    {
        *(this->gpio_reg + ((this->pin) / 10)) |=  (1 << (((this->pin) % 10) * 3));
    }
}

bool Gpio::read()
{
    if (*(this->gpio_reg + 13) & (1 << this->pin))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Gpio::write(bool state)
{
    if (state)
    {
        *(this->gpio_reg + 7) = (1 << this->pin);     // sets bits which are 1 ignores bits which are 0
    }
    else
    {
        *(this->gpio_reg + 10) = (1 << this->pin);    // clears bits which are 1 ignores bits which are 0
    }
}