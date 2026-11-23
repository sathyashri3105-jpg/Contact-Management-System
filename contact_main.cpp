
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <limits>
using namespace std;

// Member Structure
struct Member
{
    string id;
    string name;
    string joinedDate;
};

// Loan Structure
struct Loan
{
    string itemID;
    string memberID;
    time_t checkoutDate;
};

// Base Class
class MediaItem
{
protected:
    string id;
    string title;
    string author;
    int totalCopies;
    int availableCopies;

public:
    MediaItem(string i, string t, string a, int copies)
    {
        id = i;
        title = t;
        author = a;
        totalCopies = copies;
        availableCopies = copies;
    }

    virtual ~MediaItem() {}

    string getID() const
    {
        return id;
    }

    string getTitle() const
    {
        return title;
    }

    string getAuthor() const
    {
        return author;
    }

    int getAvailableCopies() const
    {
        return availableCopies;
    }

    int getTotalCopies() const
    {
        return totalCopies;
    }

    void setAvailableCopies(int copies)
    {
        availableCopies = copies;
    }

    bool checkoutCopy()
    {
        if (availableCopies <= 0)
            return false;

        availableCopies--;
        return true;
    }

    void returnCopy()
    {
        if (availableCopies < totalCopies)
            availableCopies++;
    }

    virtual string getType() const = 0;

    virtual double calculateFine(int overdueDays) const = 0;

    virtual void display() const = 0;
};

// Derived Class: Book
class Book : public MediaItem
{
private:
    string category;

public:
    Book(string i, string t, string a, string c, int copies)
        : MediaItem(i, t, a, copies)
    {
        category = c;
    }

    string getType() const override
    {
        return "Book";
    }

    double calculateFine(int overdueDays) const override
    {
        return overdueDays * 2.0;
    }

    void display() const override
    {
        cout << "\nID: " << id;
        cout << "\nType: Book";
        cout << "\nTitle: " << title;
        cout << "\nAuthor: " << author;
        cout << "\nCategory: " << category;
        cout << "\nTotal Copies: " << totalCopies;
        cout << "\nAvailable Copies: " << availableCopies;
        cout << "\n----------------------------------\n";
    }

    string getCategory() const
    {
        return category;
    }
};

// Derived Class: Journal
class Journal : public MediaItem
{
private:
    int issueNumber;

public:
    Journal(string i, string t, string a, int issue, int copies)
        : MediaItem(i, t, a, copies)
    {
        issueNumber = issue;
    }

    string getType() const override
    {
        return "Journal";
    }

    double calculateFine(int overdueDays) const override
    {
        return overdueDays * 5.0;
    }

    void display() const override
    {
        cout << "\nID: " << id;
        cout << "\nType: Journal";
        cout << "\nTitle: " << title;
        cout << "\nAuthor: " << author;
        cout << "\nIssue Number: " << issueNumber;
        cout << "\nTotal Copies: " << totalCopies;
        cout << "\nAvailable Copies: " << availableCopies;
        cout << "\n----------------------------------\n";
    }

    int getIssueNumber() const
    {
        return issueNumber;
    }
};

// Library Management Class
class Library
{
private:
    vector<Member> members;
    vector<unique_ptr<MediaItem> > items;
    vector<Loan> loans;

    const string memberSample = "members.txt";
    const string bookSample = "books.txt";

    const string memberFile = "library_members.txt";
    const string bookFile = "library_books.txt";
    const string journalFile = "library_journals.txt";
    const string loanFile = "library_loans.txt";

    // Generate next item ID
    string generateID(string prefix, int width)
    {
        int maxID = 0;

        for (const auto &item : items)
        {
            string itemID = item->getID();

            if (itemID.find(prefix) == 0)
            {
                int number = stoi(itemID.substr(prefix.length()));

                if (number > maxID)
                    maxID = number;
            }
        }

        stringstream ss;
        ss << prefix << setw(width)
           << setfill('0') << maxID + 1;

        return ss.str();
    }

    // Generate next member ID
    string generateMemberID()
    {
        int maxID = 0;

        for (const Member &m : members)
        {
            if (m.id.find("MEM-") == 0)
            {
                int number = stoi(m.id.substr(4));

                if (number > maxID)
                    maxID = number;
            }
        }

        stringstream ss;
        ss << "MEM-" << setw(4)
           << setfill('0') << maxID + 1;

        return ss.str();
    }

    // Find member
    Member* findMember(string memberID)
    {
        for (auto &m : members)
        {
            if (m.id == memberID)
                return &m;
        }

        return nullptr;
    }

    // Find media item
    MediaItem* findItem(string itemID)
    {
        for (auto &item : items)
        {
            if (item->getID() == itemID)
                return item.get();
        }

        return nullptr;
    }

    // Load members
    void loadMembers()
    {
        ifstream file(memberFile);

        if (!file.is_open())
            file.open(memberSample);

        if (!file.is_open())
        {
            cout << "Members file not found!\n";
            return;
        }

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);
            Member m;

            getline(ss, m.id, ',');
            getline(ss, m.name, ',');
            getline(ss, m.joinedDate);

            if (m.id.find("MEM-") == 0 && !m.name.empty())
                members.push_back(m);
        }

        file.close();

        cout << "Members loaded: " << members.size() << "\n";
    }

    // Load books
    void loadBooks()
    {
        ifstream file(bookFile);

        if (!file.is_open())
            file.open(bookSample);

        if (!file.is_open())
        {
            cout << "Books file not found!\n";
            return;
        }

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            string id, title, author, category, copiesText;

            getline(ss, id, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, category, ',');
            getline(ss, copiesText);

            if (id.find("BOOK-") != 0)
                continue;

            try
            {
                int copies = stoi(copiesText);

                if (copies <= 0)
                    continue;

                items.push_back(
                    unique_ptr<MediaItem>(
                        new Book(id, title, author, category, copies)
                    )
                );
            }
            catch (...)
            {
                continue;
            }
        }

        file.close();

        cout << "Books loaded: " << items.size() << "\n";
    }

    // Load journals
    void loadJournals()
    {
        ifstream file(journalFile);

        if (!file.is_open())
            return;

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            string id, title, author, issueText, copiesText;

            getline(ss, id, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, issueText, ',');
            getline(ss, copiesText);

            if (id.find("JOUR-") != 0)
                continue;

            try
            {
                int issue = stoi(issueText);
                int copies = stoi(copiesText);

                if (issue <= 0 || copies <= 0)
                    continue;

                items.push_back(
                    unique_ptr<MediaItem>(
                        new Journal(id, title, author, issue, copies)
                    )
                );
            }
            catch (...)
            {
                continue;
            }
        }

        file.close();
    }

    // Load active loans
    void loadLoans()
    {
        ifstream file(loanFile);

        if (!file.is_open())
            return;

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            Loan loan;
            long long dateValue;

            getline(ss, loan.itemID, ',');
            getline(ss, loan.memberID, ',');

            if (!(ss >> dateValue))
                continue;

            loan.checkoutDate = static_cast<time_t>(dateValue);

            if (findItem(loan.itemID) &&
                findMember(loan.memberID))
            {
                loans.push_back(loan);
            }
        }

        file.close();

        // Recalculate available copies
        for (auto &item : items)
        {
            int borrowed = 0;

            for (const Loan &loan : loans)
            {
                if (loan.itemID == item->getID())
                    borrowed++;
            }

            int available = item->getTotalCopies() - borrowed;

            if (available < 0)
                available = 0;

            item->setAvailableCopies(available);
        }
    }

    // Save all records
    void saveAll()
    {
        ofstream memberOut(memberFile);

        for (const Member &m : members)
        {
            memberOut << m.id << ","
                      << m.name << ","
                      << m.joinedDate << "\n";
        }

        memberOut.close();

        ofstream bookOut(bookFile);
        ofstream journalOut(journalFile);

        for (const auto &item : items)
        {
            Book *book = dynamic_cast<Book*>(item.get());

            if (book != nullptr)
            {
                bookOut << book->getID() << ","
                        << book->getTitle() << ","
                        << book->getAuthor() << ","
                        << book->getCategory() << ","
                        << book->getTotalCopies() << "\n";
            }

            Journal *journal = dynamic_cast<Journal*>(item.get());

            if (journal != nullptr)
            {
                journalOut << journal->getID() << ","
                           << journal->getTitle() << ","
                           << journal->getAuthor() << ","
                           << journal->getIssueNumber() << ","
                           << journal->getTotalCopies() << "\n";
            }
        }

        bookOut.close();
        journalOut.close();

        ofstream loanOut(loanFile);

        for (const Loan &loan : loans)
        {
            loanOut << loan.itemID << ","
                    << loan.memberID << ","
                    << static_cast<long long>(loan.checkoutDate)
                    << "\n";
        }

        loanOut.close();
    }

public:

    Library()
    {
        loadMembers();
        loadBooks();
        loadJournals();
        loadLoans();

        saveAll();
    }

    // Display catalogue
    void displayCatalogue()
    {
        if (items.empty())
        {
            cout << "\nNo media items available.\n";
            return;
        }

        cout << "\n========== LIBRARY CATALOGUE ==========\n";

        for (const auto &item : items)
        {
            item->display();
        }

        cout << "Total Media Items: " << items.size() << "\n";
    }

    // Search item
    void searchItem()
    {
        string keyword;

        cout << "\nEnter Item ID or Title: ";
        getline(cin, keyword);

        bool found = false;

        for (const auto &item : items)
        {
            if (item->getID() == keyword ||
                item->getTitle().find(keyword) != string::npos)
            {
                item->display();
                found = true;
            }
        }

        if (!found)
            cout << "\nItem not found!\n";
    }

    // Add book
    void addBook()
    {
        string title, author, category;
        int copies;

        cout << "\nEnter Book Title: ";
        getline(cin, title);

        cout << "Enter Author: ";
        getline(cin, author);

        cout << "Enter Category: ";
        getline(cin, category);

        cout << "Enter Number of Copies: ";

        if (!(cin >> copies) || copies <= 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid number of copies!\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string id = generateID("BOOK-", 4);

        items.push_back(
            unique_ptr<MediaItem>(
                new Book(id, title, author, category, copies)
            )
        );

        saveAll();

        cout << "\nBook added successfully!\n";
        cout << "Book ID: " << id << "\n";
    }

    // Add journal
    void addJournal()
    {
        string title, author;
        int issue, copies;

        cout << "\nEnter Journal Title: ";
        getline(cin, title);

        cout << "Enter Author: ";
        getline(cin, author);

        cout << "Enter Issue Number: ";

        if (!(cin >> issue) || issue <= 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid issue number!\n";
            return;
        }

        cout << "Enter Number of Copies: ";

        if (!(cin >> copies) || copies <= 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid number of copies!\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string id = generateID("JOUR-", 4);

        items.push_back(
            unique_ptr<MediaItem>(
                new Journal(id, title, author, issue, copies)
            )
        );

        saveAll();

        cout << "\nJournal added successfully!\n";
        cout << "Journal ID: " << id << "\n";
    }

    // Register member
    void addMember()
    {
        Member m;

        cout << "\nEnter Member Name: ";
        getline(cin, m.name);

        cout << "Enter Joining Date (YYYY-MM-DD): ";
        getline(cin, m.joinedDate);

        if (m.name.empty() || m.joinedDate.empty())
        {
            cout << "Member details cannot be empty!\n";
            return;
        }

        m.id = generateMemberID();

        members.push_back(m);

        saveAll();

        cout << "\nMember registered successfully!\n";
        cout << "Member ID: " << m.id << "\n";
    }

    // Display members
    void displayMembers()
    {
        cout << "\n========== LIBRARY MEMBERS ==========\n";

        if (members.empty())
        {
            cout << "No members found.\n";
            return;
        }

        for (const Member &m : members)
        {
            cout << "\nMember ID: " << m.id;
            cout << "\nName: " << m.name;
            cout << "\nJoined Date: " << m.joinedDate;
            cout << "\n----------------------------------\n";
        }

        cout << "Total Members: " << members.size() << "\n";
    }

    // Checkout item
    void checkoutItem()
    {
        string itemID, memberID;

        cout << "\nEnter Item ID: ";
        getline(cin, itemID);

        cout << "Enter Member ID: ";
        getline(cin, memberID);

        MediaItem *item = findItem(itemID);
        Member *member = findMember(memberID);

        if (item == nullptr)
        {
            cout << "\nItem not found!\n";
            return;
        }

        if (member == nullptr)
        {
            cout << "\nMember not found!\n";
            return;
        }

        if (!item->checkoutCopy())
        {
            cout << "\nNo available copies!\n";
            return;
        }

        Loan loan;

        loan.itemID = itemID;
        loan.memberID = memberID;
        loan.checkoutDate = time(nullptr);

        loans.push_back(loan);

        saveAll();

        cout << "\nItem checked out successfully!\n";
        cout << "Item: " << item->getTitle() << "\n";
        cout << "Member: " << member->name << "\n";
        cout << "Return period: 14 days\n";
    }

    // Return item
    void returnItem()
    {
        string itemID, memberID;

        cout << "\nEnter Item ID: ";
        getline(cin, itemID);

        cout << "Enter Member ID: ";
        getline(cin, memberID);

        MediaItem *item = findItem(itemID);

        if (item == nullptr)
        {
            cout << "\nItem not found!\n";
            return;
        }

        for (int i = 0; i < loans.size(); i++)
        {
            if (loans[i].itemID == itemID &&
                loans[i].memberID == memberID)
            {
                time_t now = time(nullptr);

                int days = static_cast<int>(
                    difftime(now, loans[i].checkoutDate) /
                    (60 * 60 * 24)
                );

                int overdueDays = days - 14;

                if (overdueDays < 0)
                    overdueDays = 0;

                double fine = item->calculateFine(overdueDays);

                cout << "\n========== RETURN DETAILS ==========\n";
                cout << "Item: " << item->getTitle() << "\n";
                cout << "Borrowed Days: " << days << "\n";
                cout << "Overdue Days: " << overdueDays << "\n";
                cout << "Fine: Rs. " << fixed
                     << setprecision(2) << fine << "\n";

                item->returnCopy();

                loans.erase(loans.begin() + i);

                saveAll();

                cout << "\nItem returned successfully!\n";
                return;
            }
        }

        cout << "\nNo active borrowing record found for this member and item.\n";
    }

    // Display active loans
    void displayLoans()
    {
        cout << "\n========== ACTIVE LOANS ==========\n";

        if (loans.empty())
        {
            cout << "No active loans.\n";
            return;
        }

        for (const Loan &loan : loans)
        {
            MediaItem *item = findItem(loan.itemID);
            Member *member = findMember(loan.memberID);

            cout << "\nItem ID: " << loan.itemID;

            if (item)
                cout << "\nTitle: " << item->getTitle();

            cout << "\nMember ID: " << loan.memberID;

            if (member)
                cout << "\nMember Name: " << member->name;

            cout << "\n----------------------------------\n";
        }

        cout << "Total Active Loans: " << loans.size() << "\n";
    }

    // Main menu
    void menu()
    {
        int choice;

        do
        {
            cout << "\n\n========== LIBRARY MANAGEMENT SYSTEM ==========\n";
            cout << "1. Display All Media Items\n";
            cout << "2. Search Media Item\n";
            cout << "3. Add Book\n";
            cout << "4. Add Journal\n";
            cout << "5. Register Member\n";
            cout << "6. Display Members\n";
            cout << "7. Checkout Item\n";
            cout << "8. Return Item\n";
            cout << "9. Display Active Loans\n";
            cout << "0. Save and Exit\n";
            cout << "================================================\n";

            cout << "Enter your choice: ";

            if (!(cin >> choice))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "\nInvalid input!\n";
                continue;
            }

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice)
            {
                case 1:
                    displayCatalogue();
                    break;

                case 2:
                    searchItem();
                    break;

                case 3:
                    addBook();
                    break;

                case 4:
                    addJournal();
                    break;

                case 5:
                    addMember();
                    break;

                case 6:
                    displayMembers();
                    break;

                case 7:
                    checkoutItem();
                    break;

                case 8:
                    returnItem();
                    break;

                case 9:
                    displayLoans();
                    break;

                case 0:
                    saveAll();
                    cout << "\nData saved successfully!\n";
                    cout << "Exiting Library Management System...\n";
                    break;

                default:
                    cout << "\nInvalid choice! Try again.\n";
            }

        } while (choice != 0);
    }
};

int main()
{
    Library library;

    library.menu();

    return 0;
}

