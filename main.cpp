#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

class Ticket {
private:
    string passenger_name;
    int seat_num;
    string flight_num;
    double price;
    bool is_booked;
    string confirmation_id;

public:
    Ticket (const string& name, int seat, const string& flight, double ticket_price, const string& id)
    : passenger_name (name), seat_num (seat), flight_num(flight), price(ticket_price), is_booked (false), confirmation_id(id) {}

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

    string get_confirmation_id() const {
        return confirmation_id;
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
    map<string, int> id_map;
    int booking_id_count = 10000;


public:
    Airplane(const string& flight, const string& date, int seats_per_row, int total_rows, map<int, double> price)
    : flight_num(flight), date(date), seats_per_row(seats_per_row), row_prices(price), booking_id_count(10000)  {

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

    bool book_seat(const string& passenger_name, const string& seat) {

        int row = stoi(seat.substr(0, seat.size() - 1));
        char seat_letter = seat.back();
        int seat_num = (row - 1) * seats_per_row + (seat_letter - 'A' + 1);

        if (seat_letter < 'A' || seat_letter >= ('A' + seats_per_row)) {
            cout << "Invalid seat letter.\n";
            return false;
        }

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
        string confirmation_id = to_string(booking_id_count++);

        tickets[seat_num - 1] = new Ticket(passenger_name, seat_num, flight_num, price, confirmation_id);
        tickets[seat_num - 1]->book_ticket();
        available_seats[seat_num - 1] = true;

        id_map[confirmation_id] = seat_num;

        cout << "Confirmed with ID " << confirmation_id << endl;

        return true;
    }

    bool return_ticket(const string& confirmation_id) {
        if (id_map.find(confirmation_id) == id_map.end()) {
            return false;
        }

        int seat_num = id_map[confirmation_id];

        if (seat_num <= 0 || seat_num > total_seats || !available_seats[seat_num - 1]) {
            cout << "Invalid seat number or seat is not booked." << endl;
            return false;
        }

        string passenger_name = tickets[seat_num - 1]->get_passenger_name();
        double refund_amount = tickets[seat_num - 1]->get_price();

        available_seats[seat_num - 1] = false;
        delete tickets[seat_num - 1];
        tickets[seat_num - 1] = nullptr;

        id_map.erase(confirmation_id);

        cout << "Confirmed " << refund_amount << "$ refund for " << passenger_name << endl;

        return true;
    }

    bool view_ticket(const string& confirmation_id) const {
        if (id_map.find(confirmation_id) == id_map.end()) {
            return false;
        }

        int seat_num = id_map.at(confirmation_id);
        if (seat_num <= 0 || seat_num > total_seats || !tickets[seat_num - 1]) {
            cout << "Ticket not found or seat is not booked." << endl;
            return false;
        }

        Ticket* ticket = tickets[seat_num - 1];
        cout << "Flight " << flight_num << ", " << date << ", ";
        cout << "seat " << ticket->get_seat_num() << ", price " << ticket->get_price() << "$, ";
        cout << ticket->get_passenger_name() << endl;
        return true;
    }

    bool view_user_tickets(const string& passenger_name) const {
        bool found = false;

        for (int i = 0; i < total_seats; ++i) {
            if (tickets[i] && tickets[i]->get_passenger_name() == passenger_name) {
                int seat_num = tickets[i]->get_seat_num();
                char seat_letter = 'A' + (seat_num - 1) % seats_per_row;
                int row = (seat_num - 1) / seats_per_row + 1;
                double price = tickets[i]->get_price();

                cout << "Flight " << flight_num << ", " << date << ", seat " << row << seat_letter << ", price " << price << "$" << endl;
                found = true;
            }
        }
        return found;
    }
};

class File_Reader {
public:
    void load_config(const string& file_name, Airplane** airplanes, int& num_of_records) {
        ifstream infile(file_name);
        if (!infile) {
            cerr << "Error opening file!" << endl;
            return;
        }

        string date, flight_num, row_info1, row_info2, price1, price2;
        int seats_per_row, row_start1, row_end1, row_start2, row_end2;
        map<int, double> row_prices;

        while (infile >> date >> flight_num >> seats_per_row >> row_info1 >> price1 >> row_info2 >> price2) {
            row_prices.clear();

            sscanf(row_info1.c_str(), "%d-%d", &row_start1, &row_end1);
            sscanf(row_info2.c_str(), "%d-%d", &row_start2, &row_end2);

            row_prices[row_end1] = stod(price1.substr(0, price1.size() - 1));
            row_prices[row_end2] = stod(price2.substr(0, price2.size() - 1));

            int total_rows = row_end2;

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

        if (command_line == "exit") {
            break;
        }

        istringstream iss(command_line);
        string command, date, flight_number, seat, username;

        iss >> command;

        Airplane* selectedAirplane = nullptr;
        if (command == "check" || command == "book") {
            iss >> date >> flight_number >> seat >> username;
            for (int i = 0; i < airplane_count; ++i) {
                if (airplanes[i]->get_flight_number() == flight_number) {
                    selectedAirplane = airplanes[i];
                    break;
                }
            }
            if (!selectedAirplane) {
                cout << "Flight not found!" << endl;
                continue;
            }
        }
        if (command == "check") {
            selectedAirplane->check_seats();
        }
        else if (command == "book") {
            selectedAirplane->book_seat(username, seat);
        }
        else if (command == "return" || command == "view") {
            string confirmation_id;
            iss >> confirmation_id;

            bool found = false;
            for (int i = 0; i < airplane_count; ++i) {
                if (command == "return") {
                    if (airplanes[i]->return_ticket(confirmation_id)) {
                        found = true;
                        break;
                    }
                }
                else if (command == "view") {
                    if (airplanes[i]->view_ticket(confirmation_id)) {
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                cout << "Invalid confirmation ID or ticket not found!" << endl;
            }

        }  else if (command == "view_username") {
            iss >> username;

            bool found = false;
            for (int i = 0; i < airplane_count; ++i) {
                if (airplanes[i]->view_user_tickets(username)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "No tickets found for " << username << endl;
            }
        } else {
            cout << "Invalid command!" << endl;
        }
    }

    // Clean up allocated memory
    for (int i = 0; i < airplane_count; ++i) {
        delete airplanes[i];
    }

    return 0;
}
