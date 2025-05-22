#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

enum class Genre
{
    Fiction,
    NonFiction,
    Periodical,
    Biography,
    Children
};

string genre_to_string(Genre genre)
{
    switch (genre)
    {
    case Genre::Fiction:
        return "Fiction";
    case Genre::NonFiction:
        return "Non-Fiction";
    case Genre::Periodical:
        return "Periodical";
    case Genre::Biography:
        return "Biography";
    case Genre::Children:
        return "Children";
    default:
        return "Unknown";
    }
}

class Book
{
private:
    string ISBN;
    string title;
    string author;
    int copyright_date;
    bool checked_out;
    Genre genre;

public:
    Book(string isbn, string t, string a, int date, Genre g)
        : ISBN(isbn), title(t), author(a), copyright_date(date), checked_out(false), genre(g) {}

    string get_ISBN() const { return ISBN; }
    string get_title() const { return title; }
    string get_author() const { return author; }
    int get_copyright_date() const { return copyright_date; }
    Genre get_genre() const { return genre; }
    bool is_checked_out() const { return checked_out; }

    void check_out() { checked_out = true; }
    void check_in() { checked_out = false; }

    bool operator==(const Book &other) const { return ISBN == other.ISBN; }
    bool operator!=(const Book &other) const { return !(*this == other); }

    friend ostream &operator<<(ostream &os, const Book &book)
    {
        os << "Title: " << book.title << "\nAuthor: " << book.author
           << "\nISBN: " << book.ISBN << "\nGenre: " << genre_to_string(book.genre) << "\n";
        return os;
    }
};

class Patron
{
private:
    string user_name;
    string card_number;
    int owed_fees;

public:
    Patron(string name, string card, int fees = 0)
        : user_name(name), card_number(card), owed_fees(fees) {}

    string get_user_name() const { return user_name; }
    string get_card_number() const { return card_number; }
    int get_owed_fees() const { return owed_fees; }
    void set_owed_fees(int fees) { owed_fees = fees; }
    bool owes_fees() const { return owed_fees > 0; }
};

class Transaction
{
public:
    Book book;
    Patron patron;
    string activity; // "Check Out" or "Check In"
    string date;

    Transaction(const Book &b, const Patron &p, const string &act, const string &d)
        : book(b), patron(p), activity(act), date(d) {}

    friend ostream &operator<<(ostream &os, const Transaction &t)
    {
        os << "Transaction:\nBook: " << t.book.get_title() << "\nPatron: " << t.patron.get_user_name()
           << "\nActivity: " << t.activity << "\nDate: " << t.date << "\n";
        return os;
    }
};

class Library
{
private:
    vector<Book> books;
    vector<Patron> patrons;
    vector<Transaction> transactions;

    void save_books_to_file()
    {
        ofstream file("books.txt");
        for (const auto &book : books)
        {
            file << book.get_ISBN() << ";" << book.get_title() << ";" << book.get_author() << ";"
                 << book.get_copyright_date() << ";" << static_cast<int>(book.get_genre()) << ";" << book.is_checked_out() << "\n";
        }
    }

    void save_patrons_to_file()
    {
        ofstream file("patrons.txt");
        for (const auto &patron : patrons)
        {
            file << patron.get_user_name() << ";" << patron.get_card_number() << ";" << patron.get_owed_fees() << "\n";
        }
    }

    void save_transactions_to_file()
    {
        ofstream file("transactions.txt");
        for (const auto &transaction : transactions)
        {
            file << transaction.book.get_ISBN() << ";" << transaction.patron.get_card_number() << ";"
                 << transaction.activity << ";" << transaction.date << "\n";
        }
    }

public:
    void add_book(const Book &book)
    {
        books.push_back(book);
        save_books_to_file();
    }

    void add_patron(const Patron &patron)
    {
        patrons.push_back(patron);
        save_patrons_to_file();
    }

    bool check_out_book(const string &isbn, const string &card_number, const string &date)
    {
        int book_index = -1;
        for (int i = 0; i < books.size(); ++i)
        {
            if (books[i].get_ISBN() == isbn)
            {
                book_index = i;
                break;
            }
        }

        if (book_index == -1 || books[book_index].is_checked_out())
        {
            cout << "Book not available for checkout or does not exist.\n";
            return false;
        }

        int patron_index = -1;
        for (int i = 0; i < patrons.size(); ++i)
        {
            if (patrons[i].get_card_number() == card_number)
            {
                patron_index = i;
                break;
            }
        }

        if (patron_index == -1 || patrons[patron_index].owes_fees())
        {
            cout << "Patron does not exist or owes fees.\n";
            return false;
        }

        books[book_index].check_out();
        transactions.push_back(Transaction(books[book_index], patrons[patron_index], "Check Out", date));
        save_transactions_to_file();
        save_books_to_file();
        return true;
    }

    bool check_in_book(const string &isbn, const string &date)
    {
        int book_index = -1;
        for (int i = 0; i < books.size(); ++i)
        {
            if (books[i].get_ISBN() == isbn)
            {
                book_index = i;
                break;
            }
        }

        if (book_index == -1 || !books[book_index].is_checked_out())
        {
            cout << "Book not checked out or does not exist.\n";
            return false;
        }

        books[book_index].check_in();
        save_books_to_file();
        return true;
    }

    vector<string> patrons_with_fees() const
    {
        vector<string> result;
        for (const auto &patron : patrons)
        {
            if (patron.owes_fees())
                result.push_back(patron.get_user_name());
        }
        return result;
    }

    void display_books()
    {
        for (const auto &book : books)
        {
            cout << book << "\n";
        }
    }

    void display_transactions() const
    {
        if (transactions.empty())
        {
            cout << "No transactions to display.\n";
        }
        else
        {
            for (const auto &transaction : transactions)
            {
                cout << transaction << "\n";
            }
        }
    }
};

int main()
{
    Library library;

    library.add_book(Book("111-222-333", "The C++ Programming Language", "Bjarne Stroustrup", 2013, Genre::NonFiction));
    library.add_book(Book("444-555-666", "The Hobbit", "J.R.R. Tolkien", 1937, Genre::Fiction));
    library.add_patron(Patron("Alice", "123"));
    library.add_patron(Patron("Bob", "456", 50));
    library.add_patron(Patron("Walmond", "456", 200));

    string choice;
    while (true)
    {
        cout << "\nLibrary Menu:\n";
        cout << "1. List all books\n";
        cout << "2. Add a book\n";
        cout << "3. Check out a book\n";
        cout << "4. Check in a book\n";
        cout << "5. List patrons with fees\n";
        cout << "6. Display transactions\n"; 
        cout << "7. Add a patron\n";        
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == "1")
        {
            library.display_books();
        }
        else if (choice == "2")
        {
            string isbn, title, author;
            int year, genre;
            cout << "Enter ISBN: ";
            cin >> isbn;
            cout << "Enter Title: ";
            cin.ignore();
            getline(cin, title);
            cout << "Enter Author: ";
            getline(cin, author);
            cout << "Enter Year: ";
            cin >> year;
            cout << "Enter Genre (0: Fiction, 1: Non-Fiction, 2: Periodical, 3: Biography, 4: Children): ";
            cin >> genre;

            library.add_book(Book(isbn, title, author, year, static_cast<Genre>(genre)));
        }
        else if (choice == "3")
        {
            string isbn, card_number, date;
            cout << "Enter ISBN of book to check out: ";
            cin >> isbn;
            cout << "Enter Card Number: ";
            cin >> card_number;
            cout << "Enter Date: ";
            cin >> date;

            if (library.check_out_book(isbn, card_number, date))
                cout << "Book checked out successfully.\n";
            else
                cout << "Unable to check out the book.\n";
        }
        else if (choice == "4")
        {
            string isbn, date;
            cout << "Enter ISBN of book to check in: ";
            cin >> isbn;
            cout << "Enter Date: ";
            cin >> date;

            if (library.check_in_book(isbn, date))
                cout << "Book checked in successfully.\n";
            else
                cout << "Unable to check in the book.\n";
        }
        else if (choice == "5")
        {
            vector<string> patrons_with_fees = library.patrons_with_fees();
            cout << "Patrons with fees: \n";
            for (int i = 0; i < patrons_with_fees.size(); ++i)
            {
                cout << patrons_with_fees[i] << "\n";
            }
        }
        else if (choice == "6")
        {
            library.display_transactions();
        }
        else if (choice == "7")
        {
            string name, card_number;
            int fees;
            cout << "Enter Patron Name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter Card Number: ";
            cin >> card_number;
            cout << "Enter Fees: ";
            cin >> fees;

            library.add_patron(Patron(name, card_number, fees));
        }
        else if (choice == "8")
        {
            break;
        }
        else
        {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
