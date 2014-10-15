#include <DSP2802x/DSP28x_Project.h>

#include <sysctrl/clock.h>
#include <pie/pie.h>

void devices_init();

/*
 * Init
 */
void devices_init(
  void
)
{
    sysctrl_clock_init();

    pie_init(); // Zero filling

}


/*
 * main.c
 */
int main(
  void
)
{
    devices_init();
    return 0;
}
