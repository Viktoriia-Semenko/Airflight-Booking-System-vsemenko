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

public:
    Ticket (const string& name, int seat, const string& flight, double ticket_price, const string& id)
    : passenger_name (name), seat_num (seat), flight_num(flight), price(ticket_price), is_booked (false){}

    void book_ticket() {
        is_booked = true;
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
};

class Airplane
{
private:
    string flight_num;
    string date;
    int total_seats;
    int seats_per_row;
    bool* available_seats;
    map<int, double> row_prices;

public:
    Airplane(const string& flight, const string& date, int seats_per_row, int total_rows, map<int, double> price)
    : flight_num(flight), date(date), seats_per_row(seats_per_row), row_prices(price) {

        total_seats = seats_per_row * total_rows;
        available_seats = new bool[total_seats];

        for (int i = 0; i < total_seats; ++i) {
            available_seats[i] = false;
        }
    }

    ~Airplane() {
        delete[] available_seats;
    }

    string get_flight_number() const {
        return flight_num;
    }
    string get_flight_date() const {
        return date;
    }
    bool is_seat_available(int seat_num) const {
        return !available_seats[seat_num - 1];
    }
    void set_seat_availability(int seat_num, bool available) {
        available_seats[seat_num - 1] = available;
    }

    int get_total_seats() const {
        return total_seats;
    }

    int get_seats_per_row() const {
        return seats_per_row;
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

};
class RAII_Wrapper {
private:
    ifstream file_stream;
    string file_name;

public:
    RAII_Wrapper(const string& name) : file_name(name) {
        file_stream.open(file_name);
        if (!file_stream.is_open()) {
            throw std::runtime_error("Error opening file");
        }
    }

    ~RAII_Wrapper() {
        if (file_stream.is_open()) {
            file_stream.close();
        }
    }

    ifstream& get_stream() {
        return file_stream;
    }

    bool is_open() const {
        return file_stream.is_open();
    }

};
class File_Reader {
public:
    static void load_config(const string& file_name, Airplane** airplanes, int& airplane_count) {
       try {
           RAII_Wrapper file(file_name);

           string date, flight_num, row_info1, row_info2, price1, price2;
           int seats_per_row, row_start1, row_end1, row_start2, row_end2;
           map<int, double> row_prices;

           while (file.get_stream() >> date >> flight_num >> seats_per_row >> row_info1 >> price1 >> row_info2
                                    >> price2) {
               row_prices.clear();

               sscanf(row_info1.c_str(), "%d-%d", &row_start1, &row_end1);
               sscanf(row_info2.c_str(), "%d-%d", &row_start2, &row_end2);

               row_prices[row_end1] = stod(price1.substr(0, price1.size() - 1));
               row_prices[row_end2] = stod(price2.substr(0, price2.size() - 1));

               int total_rows = row_end2;

               airplanes[airplane_count] = new Airplane(flight_num, date, seats_per_row, total_rows, row_prices);
               airplane_count++;
           }
       } catch (const runtime_error& e){
           cerr << e.what() << endl;
       }
    }
};

class Command_Line_Interface
{
private:
    Airplane* airplanes[100];
    Ticket* tickets[1000];
    map<string, int> id_map;
    int airplane_count;
    int ticket_count;
    int booking_id_count;

public:
    Command_Line_Interface() : airplane_count(0), ticket_count(0), booking_id_count(10000) {
        for (int i = 0; i < 1000; ++i) {
            tickets[i] = nullptr;
        }
    }

    void initialize(const string& file_name) {
        File_Reader::load_config(file_name, airplanes, airplane_count);
    }

    Airplane* find_airplane(const string& flight_number) const {
        for (int i = 0; i < airplane_count; ++i) {
            if (airplanes[i]->get_flight_number() == flight_number) {
                return airplanes[i];
            }
        }
        return nullptr;
    }

    void show_available_seats(const string& flight_num) const {
        Airplane* airplane = find_airplane(flight_num);

        int total_seats = airplane->get_total_seats();
        if (total_seats <= 0) {
            cout << "No seats available." << endl;
            return;
        }

        for (int i = 0; i < total_seats; ++i) {
            if (airplane->is_seat_available(i + 1)) {
                double price = airplane->get_seat_price(i + 1);
                char seat_letter = 'A' + (i % airplane->get_seats_per_row());
                int row = (i / airplane->get_seats_per_row()) + 1;
                cout << row << seat_letter << " " << price << "$, ";
            }
        }
        cout << endl;
    }

    bool book_seat(const string& flight_num, const string& passenger_name, const string& seat) {
        Airplane* airplane = find_airplane(flight_num);

        int row = stoi(seat.substr(0, seat.size() - 1));
        char seat_letter = seat.back();
        int seats_per_row = airplane->get_seats_per_row();
        int total_seats = airplane->get_total_seats();
        int seat_num = (row - 1) * seats_per_row + (seat_letter - 'A' + 1);

        if (seat_letter < 'A' || seat_letter >= ('A' + seats_per_row)) {
            cout << "Invalid seat letter.\n";
            return false;
        }

        if (seat_num <= 0 || seat_num > total_seats) {
            cout << "Invalid seat number.\n";
            return false;
        }

        if (!airplane->is_seat_available(seat_num)) {
            cout << "Seat already booked.\n";
            return false;
        }

        double price = airplane->get_seat_price(seat_num);
        if (price == 0.0) {
            cout << "Invalid row number." << endl;
            return false;
        }

        string confirmation_id = to_string(booking_id_count++);

        tickets[seat_num - 1] = new Ticket(passenger_name, seat_num, flight_num, price, confirmation_id);
        tickets[seat_num - 1]->book_ticket();
        airplane->set_seat_availability(seat_num, true);

        id_map[confirmation_id] = seat_num;

        cout << "Confirmed with ID " << confirmation_id << endl;

        return true;
    }

    bool return_ticket(const string& confirmation_id) {

        if (id_map.find(confirmation_id) == id_map.end()) {
            return false;
        }
        int seat_num = id_map[confirmation_id];
        Ticket* ticket = tickets[seat_num - 1];

        string flight_number = ticket->get_flight_num();

        double refund_amount = tickets[seat_num - 1]->get_price();
        string passenger_name = tickets[seat_num - 1]->get_passenger_name();

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
        Ticket* ticket = tickets[seat_num - 1];

        string flight_number = ticket->get_flight_num();
        Airplane* airplane = find_airplane(ticket->get_flight_num());

        int seats_per_row = airplane->get_seats_per_row();
        char seat_letter = 'A' + ((seat_num - 1) % seats_per_row);

        cout << "Flight " << ticket->get_flight_num() << ", " << airplane->get_flight_date() << ", ";
        cout << "seat " << ticket->get_seat_num() << seat_letter << ", price " << ticket->get_price() << "$, ";
        cout << ticket->get_passenger_name() << endl;
        return true;
    }

    bool view_user_tickets(const string& passenger_name) const {
        bool found = false;

        for (int i = 0; i < airplane_count; ++i) {
            Ticket* ticket = tickets[i];

            if (ticket && ticket->get_passenger_name() == passenger_name) {
                string flight_number = ticket->get_flight_num();
                Airplane* airplane = find_airplane(flight_number);

                if (airplane) {
                    int seat_num = ticket->get_seat_num();
                    char seat_letter = 'A' + (seat_num - 1) % airplane->get_seats_per_row();
                    double price = ticket->get_price();

                    cout << "Flight " << airplane->get_flight_number() << ", " << airplane->get_flight_date()
                         << ", seat " << seat_num << seat_letter << ", price " << price << "$" << endl;
                    found = true;
                }
            }
            if (!found) {
                return false;
            }
        }
        return found;
    }

    bool view_all_tickets(const string& flight_number) const {
        bool found = false;
        Airplane* airplane = nullptr;

        for (int i = 0; i < airplane_count; ++i) {
            if (airplanes[i]->get_flight_number() == flight_number) {
                airplane = airplanes[i];
                break;
            }
        }
        if (airplane) {

            for (int i = 0; i < airplane->get_total_seats(); ++i) {

                if (tickets[i] && tickets[i]->get_flight_num() == flight_number) {
                    int seat_num = tickets[i]->get_seat_num();
                    char seat_letter = 'A' + (seat_num - 1) % airplane->get_seats_per_row();
                    int row = (seat_num - 1) / airplane->get_seats_per_row() + 1;

                    string name = tickets[i]->get_passenger_name();
                    double price = tickets[i]->get_price();

                    cout << row << seat_letter << " " << name << " " << price << "$" << endl;
                    found = true;
                }
            }
        }
        return found;
    }

    void process_commands() {
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

            Airplane *selectedAirplane = nullptr;
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
                show_available_seats(flight_number);
            } else if (command == "book") {
                book_seat(flight_number, username, seat);
            } else if (command == "return" || command == "view") {
                string confirmation_id;
                iss >> confirmation_id;
                bool found = false;
                for (int i = 0; i < airplane_count; ++i) {
                    if (command == "return") {
                        if (return_ticket(confirmation_id)) {
                            found = true;
                            break;
                        }
                    } else if (command == "view") {
                        if (view_ticket(confirmation_id)) {
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

                bool found = view_user_tickets(username); 
                if (!found) {
                    cout << "No tickets found for " << username << endl;
                }
            } else if (command == "view_all") {
                iss >> flight_number;

                bool found = view_all_tickets(flight_number);
                if (!found) {
                    cout << "No tickets booked for " << flight_number << endl;
                }
            } else {
                cout << "Invalid command!" << endl;
            }
        }

    }

    ~Command_Line_Interface() {
        for (int i = 0; i < airplane_count; ++i) {
            delete airplanes[i];
        }
        for (int i = 0; i < ticket_count; ++i) {
            delete tickets[i];
        }
    }

};

int main() {

    Command_Line_Interface cli;
    cli.initialize("/Users/vikasemenko/Documents/OOP 1 term/Airflight-Booking-System-vsemenko/config.txt");
    cli.process_commands();
}
