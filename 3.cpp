#include <iostream>
#include <vector>
#include <string>
#include <exception>
using namespace std;

// CUSTOM EXCEPTION

class ParkingException : public exception {
public:
    virtual const char* what() const noexcept {
        return "Parking system error";
    }
};

class NoAvailableSlotException : public ParkingException {
public:
    const char* what() const noexcept override {
        return "Tidak ada slot parkir tersedia!";
    }
};

// ABSTRACT CLASS : VEHICLE


class Vehicle {
protected:
    string plate;
    int hours;

public:
    Vehicle(string p, int h) : plate(p), hours(h) {}
    virtual int calculateRate() = 0;
    virtual string getType() = 0;
    virtual ~Vehicle() {}
};

// VEHICLE DERIVED CLASSES

class Car : public Vehicle {
public:
    Car(string p, int h) : Vehicle(p, h) {}

    int calculateRate() override {
        return hours * 5000;
    }

    string getType() override {
        return "Car";
    }
};

class Motorcycle : public Vehicle {
public:
    Motorcycle(string p, int h) : Vehicle(p, h) {}

    int calculateRate() override {
        return hours * 2000;
    }

    string getType() override {
        return "Motorcycle";
    }
};

class EV : public Vehicle {
    bool chargingRequired;

public:
    EV(string p, int h, bool c) : Vehicle(p, h), chargingRequired(c) {}

    int calculateRate() override {
        int cost = hours * 4000;
        if (chargingRequired)
            cost += 3000; // charging fee
        return cost;
    }

    string getType() override {
        return "EV";
    }
};

 //ABSTRACT CLASS : PARKING SLOT
   

class ParkingSlot {
protected:
    bool occupied = false;
    Vehicle* vehicle = nullptr;

public:
    virtual bool canPark(Vehicle* v) = 0;

    void park(Vehicle* v) {
        vehicle = v;
        occupied = true;
    }

    int unpark() {
        int cost = vehicle->calculateRate();
        vehicle = nullptr;
        occupied = false;
        return cost;
    }

    bool isOccupied() {
        return occupied;
    }

    Vehicle* getVehicle() {
        return vehicle;
    }

    virtual ~ParkingSlot() {}
};

// SLOT TYPES
  

class RegularSlot : public ParkingSlot {
public:
    bool canPark(Vehicle* v) override {
        return v->getType() != "EV"; // EV khusus EVSlot
    }
};

class EVSlot : public ParkingSlot {
public:
    bool canPark(Vehicle* v) override {
        return v->getType() == "EV";
    }
};

class VIPSlot : public ParkingSlot {
public:
    bool canPark(Vehicle* v) override {
        return true;
    }

    int unpark() {
        int base = ParkingSlot::unpark();
        return base + (base * 0.5); // surcharge 50%
    }
};

// PARKING SYSTEM

class ParkingSystem {
private:
    vector<ParkingSlot*> slots;

public:
    ParkingSystem() {
        slots.push_back(new RegularSlot());
        slots.push_back(new RegularSlot());
        slots.push_back(new EVSlot());
        slots.push_back(new VIPSlot());
    }

    void parkVehicle(Vehicle* v) {
        for (auto slot : slots) {
            if (!slot->isOccupied() && slot->canPark(v)) {
                slot->park(v);
                cout << v->getType() << " berhasil diparkir.\n";
                return;
            }
        }
        throw NoAvailableSlotException();
    }

    void unparkVehicle(Vehicle* v) {
        for (auto slot : slots) {
            if (slot->isOccupied() && slot->getVehicle() == v) {
                int cost = slot->unpark();
                cout << "Biaya parkir: Rp " << cost << endl;
                return;
            }
        }
        throw ParkingException();
    }

    ~ParkingSystem() {
        for (auto slot : slots)
            delete slot;
    }
};

// MAIN PROGRAM

int main() {
    ParkingSystem system;
    int choice, hours;
    string plate;

    try {
        cout << "=== ADVANCED PARKING SYSTEM ===\n";
        cout << "1. Car\n2. Motorcycle\n3. EV\nPilih kendaraan: ";
        cin >> choice;

        cout << "Plat nomor: ";
        cin >> plate;

        cout << "Lama parkir (jam): ";
        cin >> hours;

        Vehicle* v = nullptr;

        if (choice == 1)
            v = new Car(plate, hours);
        else if (choice == 2)
            v = new Motorcycle(plate, hours);
        else if (choice == 3) {
            bool charge;
            cout << "Butuh charging? (1/0): ";
            cin >> charge;
            v = new EV(plate, hours, charge);
        }
        else
            throw ParkingException();

        system.parkVehicle(v);
        system.unparkVehicle(v);

        delete v;
    }
    catch (exception& e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}
