#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

class Ticket {
private:
    string passenger_name;
    int seat_num;
    string flight_num;
    double price;
    bool is_booked;

public:
    Ticket (const string& name, int seat, const string& flight, double price)
    : passenger_name (name), seat_num (seat), flight_num(flight), price(price), is_booked (false) {}

    void book_ticket() {
        is_booked = true;
    }

    string get_passenger_name() const {
        return passenger_name;
    }

    int get_seat_num() const {
        return seat_num;
    }

    string get_flight_name() const {
        return passenger_name;
    }

    double get_price() const {
        return price;
    }

    bool get_booked_status() const {
        return is_booked;
    }
};

class Airplane
{
private:
    string flight_num;
    string date;
    int total_seats;
    int seats_per_row;
    int max_num_of_tickets;
    int booked_tickets;
    bool* available_seats;
    Ticket** tickets;

    int middle_row;
    double price1;
    double price2;


public:
    Airplane(const string& flight, const string& date, int seats_per_row, int total_rows, int mid_row, double price1, double price2)
    : flight_num(flight), date(date), seats_per_row(seats_per_row), total_seats(seats_per_row * total_rows),
    middle_row(mid_row), price1(price1), price2(price1), booked_tickets(0) {

        available_seats = new bool[total_seats];
        tickets = new Ticket * [total_seats];

        for (int i = 0; i < total_seats; ++i) {
            available_seats[i] = false;
            tickets[i] = nullptr;
        }
    }

    ~Airplane() {
        delete[] available_seats;
        for (int i = 0; i < booked_tickets; ++i) {
            delete tickets[i];
        }
        delete[] tickets;
    }

    string get_flight_num() const {
        return flight_num;
    }

    string get_date() const {
        return date;
    }

    double get_seat_price(int seat_num) const {
        int row = (seat_num - 1) / (seats_per_row + 1); // calculating the row os the seat
        if (row <= middle_row) {
            return price1;
        } else {
            return price2;
        }
    }

};

