#include <iostream>

#include <thread>

#include <chrono>

#include <queue>

#include <mutex>



using namespace std;



mutex trafficMutex; // To prevent data race in multithreading



// Traffic Signal States

enum SignalState { RED, GREEN, YELLOW };



// Class for Traffic Signal

class TrafficSignal {

private:

    string name;

    SignalState state;

    int greenTime;

    int redTime;

    int yellowTime;

    int trafficDensity; // Number of vehicles waiting



public:

    TrafficSignal(string n) : name(n), state(RED), greenTime(10), redTime(10), yellowTime(3), trafficDensity(0) {}



    // Getter function to retrieve the signal name

    string getName() const {

        return name;

    }



    // Set traffic density based on user input

    void setTrafficDensity(int density) {

        trafficDensity = density;

        adjustTiming();

    }



    // Adjust timing dynamically based on traffic density

    void adjustTiming() {

        if (trafficDensity > 10) {

            greenTime = 20; // More vehicles -> Increase green time

            redTime = 5;

        } else {

            greenTime = 12;

            redTime = 10;

        }

    }



    // Run the signal control logic

    void runSignal() {

        while (true) {

            trafficMutex.lock();

            cout << "\U0001F6A6 " << name << " Signal - Density: " << trafficDensity << " vehicles - ";



            // Signal cycle

            state = GREEN;

            cout << "[GREEN] for " << greenTime << " sec" << endl;

            trafficMutex.unlock();

            this_thread::sleep_for(chrono::seconds(greenTime));



            trafficMutex.lock();

            state = YELLOW;

            cout << "\U0001F6A6 " << name << " Signal - [YELLOW] for " << yellowTime << " sec" << endl;

            trafficMutex.unlock();

            this_thread::sleep_for(chrono::seconds(yellowTime));



            trafficMutex.lock();

            state = RED;

            cout << "\U0001F6A6 " << name << " Signal - [RED] for " << redTime << " sec" << endl;

            trafficMutex.unlock();

            this_thread::sleep_for(chrono::seconds(redTime));

        }

    }

};



// Class to Handle Emergency Vehicles

class EmergencyVehicle {

public:

    static void givePriority(queue<TrafficSignal*>& signals) {

        trafficMutex.lock();

        cout << "\U0001F691 Emergency Vehicle Detected! Giving Priority..." << endl;



        while (!signals.empty()) {

            TrafficSignal* signal = signals.front();

            signals.pop();

            cout << "\U0001F6A6 " << signal->getName() << " turned GREEN for Emergency Vehicle" << endl;

            cout << "Emergency vehicle passed, resuming normal traffic..." << endl;

        }

        trafficMutex.unlock();

    }

};



int main() {

    // Create multiple traffic signals

    TrafficSignal signal1("North-South");

    TrafficSignal signal2("East-West");



    // Queue for emergency handling

    queue<TrafficSignal*> emergencyQueue;

    emergencyQueue.push(&signal1);

    emergencyQueue.push(&signal2);



    // Take user input for traffic density

    int density1, density2;

    cout << "Enter traffic density for North-South: ";

    cin >> density1;

    signal1.setTrafficDensity(density1);



    cout << "Enter traffic density for East-West: ";

    cin >> density2;

    signal2.setTrafficDensity(density2);



    // Run traffic signals on different threads

    thread t1(&TrafficSignal::runSignal, &signal1);

    thread t2(&TrafficSignal::runSignal, &signal2);



    // Check for emergency vehicle input

    while (true) {

        cout << "Press 1 if a traffic police officer detects an emergency vehicle (0 to exit): ";

        int emergencyInput;

        cin >> emergencyInput;



        if (emergencyInput == 1) {

            thread emergencyThread(&EmergencyVehicle::givePriority, ref(emergencyQueue));

            emergencyThread.join();

        } else if (emergencyInput == 0) {

            break;

        }

    }



    // Join threads

    t1.join();

    t2.join();



    return 0;

}
