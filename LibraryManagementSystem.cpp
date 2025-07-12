#include <iostream>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

class Library;

class Book {
private:
    string id;
    string title;
    string author;
    bool isAvailable;
    time_t issueDate;
    time_t returnDate;

public:
    Book(string id, string title, string author) 
        : id(id), title(title), author(author), isAvailable(true) {}

    // Getters
    string getId() const {
         return id;
         }
    string getTitle() const { 
        return title;
     }
    string getAuthor() const { 
        return author; 
    }
    bool getAvailability() const { 
        return isAvailable;
     }
    time_t getIssueDate() const { 
        return issueDate;
     }
    time_t getReturnDate() const {
         return returnDate; 
        }

    // Setters
    void setAvailability(bool status) { 
        isAvailable = status; 
    }
    void setIssueDate() { 
        issueDate = time(0);
     }
    void setReturnDate(int days) { 
        returnDate = issueDate + days * 24 * 60 * 60; 
    }

    void display() const {
        cout << "ID: " << id << ", Title: " << title << ", Author: " << author 
             << ", Available: " << (isAvailable ? "Yes" : "No") << endl;
    }
};

class User {
protected:
    string userId;
    string name;

public:
    User(string id, string name) : userId(id), name(name) {}
    
    virtual void displayInfo() const = 0;
    
    // Getters
    string getUserId() const {
         return userId;
         }
    string getName() const { 
        return name; 
    }

    virtual ~User() {}
};

class Librarian : public User {
public:
    Librarian(string id, string name) : User(id, name) {}

    void displayInfo() const override {
        cout << "Librarian ID: " << userId << ", Name: " << name << endl;
    }

    void addBook(Library& library, Book* book);
    void removeBook(Library& library, string bookId);
};

class Member : public User {
private:
    vector<string> borrowedBooks;

public:
    Member(string id, string name) : User(id, name) {}

    void displayInfo() const override {
        cout << "Member ID: " << userId << ", Name: " << name 
             << ", Borrowed Books: " << borrowedBooks.size() << endl;
    }

    void borrowBook(Library& library, string bookId);
    void returnBook(Library& library, string bookId);

    vector<string> getBorrowedBooks() const { return borrowedBooks; }
};

class Library {
private:
    vector<Book*> books;
    vector<User*> users;

public:
    void addBook(Book* book) {
        books.push_back(book);
        cout << "Book added: " << book->getTitle() << endl;
    }

    void removeBook(string bookId) {
        for (auto it = books.begin(); it != books.end(); ++it) {
            if ((*it)->getId() == bookId) {
                delete *it; // Free memory
                books.erase(it);
                cout << "Book removed: " << bookId << endl;
                return;
            }
        }
        cout << "Book not found: " << bookId << endl;
    }

    void addUser(User* user) {
        users.push_back(user);
        cout << "User added: " << user->getName() << endl;
    }

    // Find a book by ID
    Book* findBook(string bookId) {
        for (Book* book : books) {
            if (book->getId() == bookId) {
                return book;
            }
        }
        return nullptr;
    }

    // Find a member by ID
    Member* findMember(string userId) {
        for (User* user : users) {
            Member* member = dynamic_cast<Member*>(user);
            if (member && member->getUserId() == userId) {
                return member;
            }
        }
        return nullptr;
    }

    void displayBooks() const {
        cout << "\nLibrary Books:\n";
        for (const Book* book : books) {
            book->display();
        }
    }

    void displayUsers() const {
        cout << "\nLibrary Users:\n";
        for (const User* user : users) {
            user->displayInfo();
        }
    }

    ~Library() {
        for (Book* book : books) delete book;
        for (User* user : users) delete user;
    }
};

void Librarian::addBook(Library& library, Book* book) {
    library.addBook(book);
}

void Librarian::removeBook(Library& library, string bookId) {
    library.removeBook(bookId);
}

void Member::borrowBook(Library& library, string bookId) {
    Book* book = library.findBook(bookId);
    if (book && book->getAvailability()) {
        book->setAvailability(false);
        book->setIssueDate();
        book->setReturnDate(14); // 14-day borrowing period
        borrowedBooks.push_back(bookId);
        cout << name << " borrowed " << book->getTitle() << endl;
    } else {
        cout << "Book not available or not found: " << bookId << endl;
    }
}

void Member::returnBook(Library& library, string bookId) {
    Book* book = library.findBook(bookId);
    if (book && !book->getAvailability()) {
        for (auto it = borrowedBooks.begin(); it != borrowedBooks.end(); ++it) {
            if (*it == bookId) {
                book->setAvailability(true);
                borrowedBooks.erase(it);
                cout << name << " returned " << book->getTitle() << endl;
                return;
            }
        }
    }
    cout << "Book not borrowed or not found: " << bookId << endl;
}

int main() {
    Library library;

    // Create a librarian
    Librarian* librarian = new Librarian("L001", "Alice");
    library.addUser(librarian);

    // Create a member
    Member* member = new Member("M001", "Bob");
    library.addUser(member);

    // Add books
    librarian->addBook(library, new Book("B001", "The Great Gatsby", "F. Scott Fitzgerald"));
    librarian->addBook(library, new Book("B002", "1984", "George Orwell"));

    // Display books and users
    library.displayBooks();
    library.displayUsers();

    // Member borrows a book
    member->borrowBook(library, "B001");

    // Display updated book status
    library.displayBooks();

    // Member returns a book
    member->returnBook(library, "B001");

    // Display final book status
    library.displayBooks();

    // Librarian removes a book
    librarian->removeBook(library, "B002");

    // Display final book list
    library.displayBooks();

    return 0;
}