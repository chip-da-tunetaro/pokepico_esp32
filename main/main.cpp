#include <StreamLogger/Logger.h>
#include <sdkconfig.h>
#include <string>
#include <pokepico/pokepico.h>

extern "C" {
void app_main();
}

using namespace m2d::ESP32;
pokepico::Device *device;

void app_main()
{
	device = new pokepico::Device("pokepico", 0x0c);

	static Logger::Group logger("pokepico");
	device->begin();
	device->service_callbacks.connect_handler = [&] {
		logger.info << "connected";
		logger.info << Logger::endl;
	};

	device->service_callbacks.disconnect_handler = [&] {
		logger.info << "disconnected";
		logger.info << Logger::endl;
	};

	device->characteristic_callbacks.write_handler = [&] (std::string rx) {
		// https://qiita.com/KazuyukiEguchi/items/e166ede5c97438b90187
		char midi[5];
		int pos = 0;
		if (rx.length() > 0) {
        for (int i = 0; i < rx.length(); i++) {
          midi[pos] = rx[i];
          pos++;
          if(pos == 5) {
			logger.info << midi[2] << midi[3] << midi[4];
			logger.info << Logger::endl;
            pos = 0;
          }          
        }
      }
	};
}
