#include <iostream>
#include <map>
#include <fstream>
#include <sstream>

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


public:
    Airplane(const string& flight, const string& date, int seats_per_row, int total_rows, map<int, double> price)
    : flight_num(flight), date(date), seats_per_row(seats_per_row), total_seats(seats_per_row * total_rows),
    row_prices(price) {

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
                cout << row << seat_letter << " " << price << ", ";
            }
        }
        cout << endl;
    }
};

class File_Reader
{
public:
};

int main()
{
}
