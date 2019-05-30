#pragma once

#include <string>
#include <BLEAdvertising.h>
#include <BLEDevice.h>

namespace pokepico
{
    const static std::string MidiServiceUUID = "03b80e5a-ede8-4b33-a751-6ce34ec4c700";
    const static std::string MidiCharacteristicUUID = "7772e5db-3868-4112-a1a9-f2669d106bf3";
class Device
{
private:
    std::string name;
    uint8_t device_udid;
    BLEServer *server;
    BLEService *midi_service;
    BLECharacteristic *midi_characteristic;

public:

    class ServerCallbacks : public BLEServerCallbacks
    {
    public:
        std::function<void()> connect_handler = []() {};
        std::function<void()> disconnect_handler = []() {};

        void onConnect(BLEServer* server) {
			connect_handler();
		};

        void onDisconnect(BLEServer* server) {
			disconnect_handler();
		}
    };

    class CharacteristicCallbacks : public BLECharacteristicCallbacks
    {
    public:
        std::function<void(std::string)> read_handler = [](std::string) {};
		std::function<void(std::string)> write_handler = [](std::string) {};

		void onRead(BLECharacteristic *characteristic)
        {
			read_handler(characteristic->getValue());
		}

		void onWrite(BLECharacteristic *characteristic)
        {
			write_handler(characteristic->getValue());
		}
	};

	ServerCallbacks service_callbacks;
	CharacteristicCallbacks characteristic_callbacks;

	Device(std::string name, uint16_t udid)
    {
        this->name = name;
		this->device_udid = udid;

		BLEDevice::init(this->name);
		this->server = BLEDevice::createServer();
		this->server->setCallbacks(&service_callbacks);
		this->midi_service = server->createService(MidiServiceUUID.c_str());
		this->midi_characteristic = midi_service->createCharacteristic(MidiCharacteristicUUID.c_str(),
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE | 
        BLECharacteristic::PROPERTY_NOTIFY | 
        BLECharacteristic::PROPERTY_WRITE_NR);
        this->midi_characteristic->setCallbacks(&characteristic_callbacks);
	}

    void begin()
    {
        this->midi_service->start();
        
        BLEAdvertising *advertising = this->server->getAdvertising();
        BLEAdvertisementData advertising_data;

        auto udid = std::string((const char*)&this->device_udid);
        advertising_data.setFlags(0x04);
        advertising_data.setManufacturerData(udid);
        advertising_data.setCompleteServices(BLEUUID(MidiServiceUUID));
        advertising->setAdvertisementData(advertising_data);
        this->server->startAdvertising();
    }
};
}