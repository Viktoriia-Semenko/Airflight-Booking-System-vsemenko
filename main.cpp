#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

class Ticket {
private:
    string passenger_name;
    int seat_num;
    string flight_num;
    double price;
    bool is_booked;

public:
    Ticket (const string& name, int seat, const string& flight, double ticket_price)
    : passenger_name (name), seat_num (seat), flight_num(flight), price(ticket_price), is_booked (false) {}

    void book_ticket() {
        is_booked = true;
    }

    void return_ticket() {
        is_booked = false;
    }

    string get_passenger_name() const {
        return passenger_name;
    }

    int get_seat_num() const {
        return seat_num;
    }

    string get_flight_num() const {
        return flight_num;
    }

    double get_price() const {
        return price;
    }

    bool get_booked_status() const {
        return is_booked;
    }

    void view_tickets() const {
        cout << seat_num << passenger_name << price;
    }
};

class Airplane
{
private:
    string flight_num;
    string date;
    int total_seats;
    int seats_per_row;
    bool* available_seats;
    Ticket** tickets;  // array for ticket
    map<int, double> row_prices;
    int booking_id_counter;


public:
    Airplane(const string& flight, const string& date, int seats_per_row, int total_rows, map<int, double> price)
    : flight_num(flight), date(date), seats_per_row(seats_per_row), row_prices(price), booking_id_counter(10000)  {

        total_seats = seats_per_row * total_rows;
        available_seats = new bool[total_seats];
        tickets = new Ticket * [total_seats];

        for (int i = 0; i < total_seats; ++i) {
            available_seats[i] = false;
            tickets[i] = nullptr;
        }
    }

    ~Airplane() {
        delete[] available_seats;
        for (int i = 0; i < total_seats; ++i) {
            delete tickets[i];
        }
        delete[] tickets;
    }
    string get_flight_number() const {
        return flight_num;
    }

    double get_seat_price(int seat_num) const {
        int row = (seat_num - 1) / seats_per_row + 1; // calculating the row of the seat
        for (const auto& ticket_price : row_prices) {
            if (row <= ticket_price.first) {
                return ticket_price.second;
            }
        }
        cout << "Invalid input data" << endl;
        return 0.0;
    }

    void check_seats() const {
        for (int i = 0; i < total_seats; ++i) {
            if (!available_seats[i]) {
                double price = get_seat_price(i + 1);
                char seat_letter = 'A' + (i % seats_per_row);
                int row = (i / seats_per_row) + 1;
                cout << row << seat_letter << " " << price << "$, ";
            }
        }
        cout << endl;
    }

    bool book_seat(const string& passenger_name, int seat_num) {
        if (seat_num <= 0 || seat_num > total_seats) {
            cout << "Invalid seat number.\n";
            return false;
        }
        if (available_seats[seat_num - 1]) {
            cout << "Seat already booked.\n";
            return false;
        }

        double price = get_seat_price(seat_num);
        if (price == 0.0) {
            cout << "Invalid row number." << endl;
            return false;
        }

        tickets[seat_num - 1] = new Ticket(passenger_name, seat_num, flight_num, price);
        tickets[seat_num - 1]->book_ticket();
        available_seats[seat_num - 1] = true;

        // Generate confirmation ID
        string confirmation_id = "ID" + to_string(booking_id_counter++);
        cout << "Confirmed with ID " << confirmation_id << endl;
        return true;
    }
};

class File_Reader {
public:
    void load_config(const string& file_name, Airplane** airplanes, int& num_of_records) {
        ifstream infile(file_name);
        if (!infile) {
            cerr << "Error opening file." << endl;
            return;
        }

        num_of_records = 0;
        string line;
        while (getline(infile, line)) {
            istringstream iss(line);
            string date, flight_num;
            int seats_per_row;
            string range1, range2;
            string price1_str, price2_str;
            double price1, price2;

            if (!(iss >> date >> flight_num >> seats_per_row >> range1 >> price1_str >> range2 >> price2_str)) {
                cerr << "Error reading line: " << line << endl;
                continue;
            }

            // removing $
            price1_str.erase(remove(price1_str.begin(), price1_str.end(), '$'), price1_str.end());
            price2_str.erase(remove(price2_str.begin(), price2_str.end(), '$'), price2_str.end());

            // parsing price 1
            int range1_start = stoi(range1.substr(0, range1.find('-')));
            int range1_end = stoi(range1.substr(range1.find('-') + 1));

            // parsing price 2
            int range2_start = stoi(range2.substr(0, range2.find('-')));
            int range2_end = stoi(range2.substr(range2.find('-') + 1));

            int total_rows = max(range2_end, range1_end);

            map<int, double> row_prices;
            row_prices[range1_end] = price1;
            row_prices[total_rows] = price2;

            airplanes[num_of_records] = new Airplane(flight_num, date, seats_per_row, total_rows, row_prices);
            num_of_records++;
        }

        infile.close();
    }
};

int main() {
    Airplane* airplanes[100];
    int airplane_count = 0;

    File_Reader file_reader;
    file_reader.load_config("/Users/vikasemenko/Documents/OOP 1 term/Airflight-Booking-System-vsemenko/config.txt", airplanes, airplane_count);

    string command_line;
    while (true) {
        cout << "> ";
        getline(cin, command_line);

        if (command_line == "exit") break;

        istringstream iss(command_line);
        string command;
        iss >> command;

        if (command == "check") {
            string date, flight_number;
            iss >> date >> flight_number;

            Airplane* selected_plane = nullptr;
            for (int i = 0; i < airplane_count; ++i) {
                if (airplanes[i]->get_flight_number() == flight_number) {
                    selected_plane = airplanes[i];
                    break;
                }
            }

            if (!selected_plane) {
                cout << "Flight not found." << endl;
                continue;
            }
            selected_plane->check_seats();
        } else if (command == "book") {
            string date, flight_number, seat_num, name;
            int row_number;
            iss >> date >> flight_number >> seat_num >> row_number >> name;

            Airplane* selected_plane1 = nullptr;
            for (int i = 0; i < airplane_count; ++i) {
                if (airplanes[i]->get_flight_number() == flight_number) {
                    selected_plane1 = airplanes[i];
                    break;
                }
            }
            selected_plane1->book_seat(name, row_number);
        }
        else {
            cout << "Invalid command." << endl;
        }
    }

    for (int i = 0; i < airplane_count; ++i) {
        delete airplanes[i];
    }

    return 0;
}