#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <exception>

using namespace std;

class Room {
public:
    int roomNumber;
    string type;
    double pricePerDay;
    bool isAvailable;

    Room(int room, string t, double price, bool available) : roomNumber(room), type(t), pricePerDay(price), isAvailable(available) {}
};

class Customer {
public:
    string name;
    int roomNumber;
    string email;
    string DOB;
    string phone_no;
    time_t checkInTime;
    time_t checkOutTime;

    Customer(string n, int num, string mail, string dob, string phone, time_t in, time_t out)
        : name(n), roomNumber(num), email(mail), DOB(dob), phone_no(phone), checkInTime(in), checkOutTime(out) {}
};

class Hotel {
private:
    vector<Room> rooms;
    vector<Customer> customers;
    string hotelName;
    int totalRooms;
    const int secondsPerDay = 100; // 5 minutes per day

public:
    Hotel(string hName, int total) : hotelName(hName), totalRooms(total) {}
    Hotel() : hotelName(""), totalRooms(0) {} // Default constructor

 

    void updateRoomDetails(int roomNumber, string newType, double newPrice) {
        for (auto& room : rooms) {
            if (room.roomNumber == roomNumber) {
                room.type = newType;
                room.pricePerDay = newPrice;
                cout << "Room " << roomNumber << " details updated." << endl;
                return;
            }
        }
        cout << "Room " << roomNumber << " not found." << endl;
    }

    void deleteRoom(int roomNumber) {
        for (auto it = rooms.begin(); it != rooms.end(); ++it) {
            if (it->roomNumber == roomNumber) {
                rooms.erase(it);
                totalRooms--;
                cout << "Room " << roomNumber << " deleted." << endl;
                return;
            }
        }
        cout << "Room " << roomNumber << " not found." << endl;
    }

    void displayHotelDescription() {
        cout << "Hotel: " << hotelName << endl;
        cout << "Total Rooms: " << totalRooms << endl;
        for (auto& room : rooms) {
            cout << "Room Number: " << room.roomNumber << "| Type: " << room.type << "| Price per Day: " << room.pricePerDay << "| Available: " << (room.isAvailable ? "Yes" : "No") << endl;
        }
    }

    void checkInCustomer(string name, int roomNumber, string email, string DOB, string phone_no, int numofDays) {
        bool foundRoom = false;
        for (auto& room : rooms) {
            if (room.roomNumber == roomNumber && room.isAvailable) {
                time_t now = time(0);
                room.isAvailable = false;
                customers.push_back(Customer(name, roomNumber, email, DOB, phone_no, now, now + numofDays * secondsPerDay));
                saveCustomerToFile("customer.txt");
                cout << "Customer " << name << " checked into room " << roomNumber << " for " << numofDays << " day(s)." << endl;
                foundRoom = true;
                break;
            }
        }
        if (!foundRoom) {
            cout << "Room " << roomNumber << " is not available" << endl;
        }
    }

    void checkOutCustomer(int roomNumber) {
        for (auto& room : rooms) {
            if (room.roomNumber == roomNumber && !room.isAvailable) {
                room.isAvailable = true;
                for (auto it = customers.begin(); it != customers.end(); ++it) {
                    if (it->roomNumber == roomNumber) {
                        double billAmount = calculateBill(it->roomNumber, it->checkInTime, it->checkOutTime);
                        cout << "Customer " << it->name << " checked out of room " << roomNumber << endl;
                        cout << "Total bill: $" << fixed << setprecision(2) << billAmount << endl;
                        customers.erase(it);
                        saveCustomerToFile("customer.txt");
                        return;
                    }
                }
            }
        }
        cout << "Room " << roomNumber << " is now available" << endl;
    }

    double calculateBill(int roomNumber, time_t checkInTime, time_t checkOutTime) {
        for (auto& room : rooms) {
            if (room.roomNumber == roomNumber) {
                double daysStayed = difftime(checkOutTime, checkInTime) / secondsPerDay;
                return daysStayed * room.pricePerDay;
            }
        }
        return 0.0;
    }

    void viewAvailableRooms() {
        for (auto& room : rooms) {
            if (room.isAvailable) {
                cout << "Room Number: " << room.roomNumber << "| Type: " << room.type << "| Price per Day: " << room.pricePerDay << "| Available: " << (room.isAvailable ? "Yes" : "No") << endl;
            }
        }
    }

    void viewBookedRooms() {
        char buffer[80];
        for (auto& customer : customers) {
            cout << "\nCustomer Name: " << customer.name << endl;
            cout << "Room Number: " << customer.roomNumber << endl;


            tm checkInTm;
            localtime_s(&checkInTm, &customer.checkInTime);
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &checkInTm);
            cout << "Check-In Time: " << buffer << endl;

            // Format check-out time
            tm checkOutTm;
            localtime_s(&checkOutTm, &customer.checkOutTime);
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &checkOutTm);
            cout << "Check-Out Time: " << buffer << endl;
        }
    }

    void saveToFile(string filename) {
        ofstream out(filename);
        if (!out) {
            cerr << "Error in opening the file" << endl;
            return;
        }

        out << hotelName << endl;
        out << totalRooms << endl;
        for (auto& room : rooms) {
            out << room.roomNumber << " " << room.type << " " << room.pricePerDay << " " << room.isAvailable << endl;
        }
        out.close();
    }

    void loadFromFile(string filename) {
        ifstream in(filename);
        if (in.is_open()) {
            in >> hotelName;
            in >> totalRooms;
            rooms.clear();
            customers.clear();
            int num;
            string type;
            double price;
            bool avail;
            for (int i = 0; i < totalRooms; ++i) {
                if (in >> num >> type >> price >> avail) {
                    rooms.push_back(Room(num, type, price, avail));
                }
                else {
                    cerr << "Error reading room data from file." << endl;
                    break;
                }
            }
            in.close();
        }
        else {
            cerr << "Error opening file" << endl;
        }
    }

    void saveCustomerToFile(string filename) {
        ofstream out(filename);
        if (!out) {
            cerr << "Error in opening the file" << endl;
            return;
        }
        for (auto& customer : customers) {
            out << customer.name << " " << customer.roomNumber << " " << customer.email << " " << customer.DOB << " " << customer.phone_no << " " << customer.checkInTime << " " << customer.checkOutTime << endl;
        }
        out.close();
    }

    void loadCustomerFromFile(string filename) {
        ifstream in(filename);
        if (in.is_open()) {
            customers.clear();
            string name, email, DOB, phone_no;
            int roomNumber;
            time_t checkInTime;
            time_t checkOutTime;
            while (in >> name >> roomNumber >> email >> DOB >> phone_no >> checkInTime >> checkOutTime) {
                customers.push_back(Customer(name, roomNumber, email, DOB, phone_no, checkInTime, checkOutTime));
            }
            in.close();
        }
        else {
            cerr << "Error opening file" << endl;
        }
    }

    void update() {
        time_t now = time(0);
        for (auto it = customers.begin(); it != customers.end(); ) {
            if (difftime(now, it->checkOutTime) >= 0) {
                cout << "Customer " << it->name << " should check out now." << endl;
                checkOutCustomer(it->roomNumber);
                it = customers.erase(it); // Remove the customer who checked out
            }
            else {
                ++it;
            }
        }
    }

    void searchCustomer(const string& name) {
        for (auto& customer : customers) {
            if (customer.name == name) {
                char buffer[80];
                cout << "Customer Found!" << endl;
                cout << "Name: " << customer.name << endl;
                cout << "Room Number: " << customer.roomNumber << endl;
                cout << "Email: " << customer.email << endl;
                cout << "DOB: " << customer.DOB << endl;
                cout << "Phone Number: " << customer.phone_no << endl;

                tm checkInTm;
                localtime_s(&checkInTm, &customer.checkInTime);
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &checkInTm);
                cout << "Check-In Time: " << buffer << endl;

                // Format check-out time
                tm checkOutTm;
                localtime_s(&checkOutTm, &customer.checkOutTime);
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &checkOutTm);
                cout << "Check-Out Time: " << buffer << endl;

            }
        }
    }

    void saveFeedbackToFile(const string& filename, const string& feedback, const string& customerName, int roomNumber, const string& email) {
        ofstream out(filename, ios::app);
        if (!out) {
            cerr << "Error opening file for feedback." << endl;
            return;
        }
        out << "Customer Name: " << customerName << "\nRoom Number: " << roomNumber << "\nEmail: " << email << "\nFeedback: " << feedback << "\n\n";
        out.close();
    }
};

void displayMenu() {
    cout << "-------------------------------" << endl;
    cout << "Hotel Management System Menu:" << endl;
    cout << "1. Initialize System (Admin only)" << endl;
    cout << "2. Display Hotel Description" << endl;
    cout << "3. Check In Customer" << endl;
    cout << "4. Check Out Customer" << endl;
    cout << "5. View Available Rooms" << endl;
    cout << "6. View Booked Rooms" << endl;
    cout << "7. Save Current State" << endl;
    cout << "8. Load State from File" << endl;
    cout << "9. Update Room Details (Admin only)" << endl;
    cout << "10. Delete Room (Admin only)" << endl;
    cout << "11. Search Customer" << endl;
    cout << "12. Leave Feedback" << endl;
    cout << "13. Exit" << endl;
}

bool login() {
    string username, password;
    cout << "-----------------" << endl;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    // Simple credentials check
    if (username == "admin" && password == "admin123") {
        cout << "Login successful." << endl;
        return true;
    }
    else {
        cout << "Invalid credentials. Access denied." << endl;
        return false;
    }
}

int main() {
    Hotel hotel;
    string filename;
    int choice;
    bool loggedIn = false;

    while (true) {
        displayMenu();
        cout << "Enter from the above given menu: ";
        cin >> choice;
        if (cin.fail()) {
            cin.clear(); // Clear the error state
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the invalid input
            cout << "Invalid input. Please enter a valid choice." << endl;
            continue;
        }
        try {
            switch (choice) {
            case 1:
                if (!loggedIn) {
                    loggedIn = login();
                }
                if (loggedIn) {
                    cout << "Enter filename to initialize from: ";
                    cin >> filename;
                    hotel.loadFromFile(filename);
                    hotel.loadCustomerFromFile("customer.txt");
                }
                break;
            case 2:
                hotel.displayHotelDescription();
                break;
            case 3: {
                string name, email, DOB, phone_no;
                int roomNumber, numofDays;
                cout << "Enter customer name: ";
                cin >> name;
                cout << "Enter room number: ";
                cin >> roomNumber;
                cout << "Enter email: ";
                cin >> email;
                cout << "Enter date of birth (YYYY-MM-DD): ";
                cin >> DOB;
                cout << "Enter phone number: ";
                cin >> phone_no;
                cout << "Enter number of days to stay: ";
                cin >> numofDays;
                hotel.checkInCustomer(name, roomNumber, email, DOB, phone_no, numofDays);
                break;
            }
            case 4: {
                int roomNumber;
                cout << "Enter room number to check out: ";
                cin >> roomNumber;
                hotel.checkOutCustomer(roomNumber);
                break;
            }
            case 5:
                hotel.viewAvailableRooms();
                break;
            case 6:
                hotel.viewBookedRooms();
                break;
            case 7:
                cout << "Enter filename to save current state: ";
                cin >> filename;
                hotel.saveToFile(filename);
                hotel.saveCustomerToFile("customer.txt");
                break;
            case 8:
                cout << "Enter filename to load state from: ";
                cin >> filename;
                hotel.loadFromFile(filename);
                hotel.loadCustomerFromFile("customer.txt");
                break;
            case 9:
                if (!loggedIn) {
                    loggedIn = login();
                }
                if (loggedIn) {
                    int roomNumber;
                    string newType;
                    double newPrice;
                    cout << "Enter room number to update: ";
                    cin >> roomNumber;
                    cout << "Enter new type: ";
                    cin >> newType;
                    cout << "Enter new price: ";
                    cin >> newPrice;
                    hotel.updateRoomDetails(roomNumber, newType, newPrice);
                }
                break;
            case 10:
                if (!loggedIn) {
                    loggedIn = login();
                }
                if (loggedIn) {
                    int roomNumber;
                    cout << "Enter room number to delete: ";
                    cin >> roomNumber;
                    hotel.deleteRoom(roomNumber);
                }
                break;
            case 11: {
                string name;
                cout << "Enter customer name to search: ";
                cin >> name;
                hotel.searchCustomer(name);
                break;
            }
            case 12: {
                string feedback, customerName, email;
                int roomNumber;
                cout << "Enter your name: ";
                cin.ignore(); // To clear the newline left by previous input
                getline(cin, customerName);
                cout << "Enter your room number: ";
                cin >> roomNumber;
                cout << "Enter your email: ";
                cin.ignore();
                getline(cin, email);
                cout << "Enter your feedback: ";
                getline(cin, feedback);
                hotel.saveFeedbackToFile("feedback.txt", feedback, customerName, roomNumber, email);
                cout << "Thank you for your feedback!" << endl;
                break;
            }
            case 13:
                cout << "Exiting..." << endl;
                return 0;
            default:
                cout << "Choice not found. Please try again." << endl;
                break;
            }
        }
        catch (const exception& e) {
            cerr << "Exception: " << e.what() << endl;
        }
    }
    return 0;
}
