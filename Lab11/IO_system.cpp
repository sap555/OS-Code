#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

// Abstract base class
class IODevice {
protected:
    string name;
    bool opened = false;

public:
    explicit IODevice(string n) : name(move(n)) {}
    virtual ~IODevice() = default;

    virtual bool open() = 0;
    virtual void close() = 0;
    virtual string read(size_t n) = 0;
    virtual bool write(const string&) = 0;
    virtual string getInfo() const = 0;

    const string& getName() const { return name; }
    bool isOpen() const { return opened; }
};

/////////////////////////////////////////////////////
// Keyboard Device
/////////////////////////////////////////////////////

class KeyboardDevice : public IODevice {
public:
    KeyboardDevice(string n) : IODevice(n) {}

    bool open() override {
        opened = true;
        return true;
    }

    void close() override {
        opened = false;
    }

    string read(size_t n) override {
        if (!opened) throw runtime_error("Keyboard not open");
        string input;
        cout << "Keyboard input: ";
        getline(cin, input);
        return input.substr(0, n);
    }

    bool write(const string&) override {
        cout << "Cannot write to keyboard\n";
        return false;
    }

    string getInfo() const override {
        return "Keyboard Device: " + name;
    }
};

/////////////////////////////////////////////////////
// Disk Device
/////////////////////////////////////////////////////

class DiskDevice : public IODevice {
    string data;

public:
    DiskDevice(string n) : IODevice(n) {}

    bool open() override {
        opened = true;
        return true;
    }

    void close() override {
        opened = false;
    }

    string read(size_t n) override {
        if (!opened) throw runtime_error("Disk not open");
        return data.substr(0, n);
    }

    bool write(const string& s) override {
        if (!opened) return false;
        data += s;
        return true;
    }

    string getInfo() const override {
        return "Disk Device: " + name;
    }
};

/////////////////////////////////////////////////////
// Network Device
/////////////////////////////////////////////////////

class NetworkDevice : public IODevice {
public:
    NetworkDevice(string n) : IODevice(n) {}

    bool open() override {
        opened = true;
        return true;
    }

    void close() override {
        opened = false;
    }

    string read(size_t n) override {
        if (!opened) throw runtime_error("Network not open");
        return "PacketData".substr(0, n);
    }

    bool write(const string& s) override {
        if (!opened) return false;
        cout << "Sending packet: " << s << endl;
        return true;
    }

    string getInfo() const override {
        return "Network Device: " + name;
    }
};

/////////////////////////////////////////////////////
// Device Registry
/////////////////////////////////////////////////////

class DeviceRegistry {
    map<string, unique_ptr<IODevice>> devices;

public:
    void registerDevice(unique_ptr<IODevice> dev) {
        devices[dev->getName()] = move(dev);
    }

    IODevice* getDevice(const string& name) {
        if (devices.count(name))
            return devices[name].get();
        return nullptr;
    }

    void listDevices() {
        cout << "\nRegistered Devices:\n";
        for (auto& d : devices) {
            cout << d.second->getInfo() << endl;
        }
    }
};

/////////////////////////////////////////////////////
// main()
/////////////////////////////////////////////////////

int main() {

    DeviceRegistry registry;

    registry.registerDevice(make_unique<KeyboardDevice>("keyboard"));
    registry.registerDevice(make_unique<DiskDevice>("disk"));
    registry.registerDevice(make_unique<NetworkDevice>("network"));

    registry.listDevices();

    IODevice* disk = registry.getDevice("disk");
    disk->open();
    disk->write("HelloDisk");
    cout << "Disk read: " << disk->read(5) << endl;
    disk->close();

    IODevice* net = registry.getDevice("network");
    net->open();
    net->write("HelloNetwork");
    cout << "Network read: " << net->read(5) << endl;
    net->close();

    return 0;
}