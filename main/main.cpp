#include <pokepico/pokepico.h>
#include <sdkconfig.h>
#include <string>

extern "C" {
void app_main();
}

using namespace m2d::ESP32;
pokepico::Device *device;

void app_main()
{
	device = new pokepico::Device("pokepico", 0x0c);
	device->begin();
}
