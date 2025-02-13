#include <iostream>
#include <map>
#include <string>
#include <stack>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <sstream>

using namespace std;

// Structure to hold voter information
struct Voter {
    string voterID;
    bool hasVoted;
    stack<string> voteHistory; // Stack to manage undo operations

    Voter(string id = ""){
        voterID=id; 
        hasVoted=false;
	} // Constructor with default values
};

// Structure for a candidate (linked list node)
struct CandidateNode {
    string name;
    int votes;
    CandidateNode* next;

    CandidateNode(string candidateName){
    	
        name=candidateName; 
        votes=0; 
        next=nullptr; 
    }
};

// Global data structures
map<string, Voter> voters;
pair<string, CandidateNode*> parties[10];
int partyCount = 0;
const string adminPassword = "admin123";

// Function prototypes
void mainScreen();
void adminPanel();
void voterPanel();
void addParty();
void addCandidate();
void addVoter();
void viewResults();
void castVote(const string& voterID);
bool authenticateAdmin();
void insertCandidate(CandidateNode*& head, const string& candidateName);
void displayCandidates(CandidateNode* head);
CandidateNode* findCandidate(CandidateNode* head, const string& name);
void saveVotersToFile();
void loadVotersFromFile();
void savePartiesToFile();
void loadPartiesFromFile();

// Main function
int main() {
    loadVotersFromFile();
    loadPartiesFromFile();
    mainScreen();
    saveVotersToFile();
    savePartiesToFile();
    return 0;
}

// Function to display the main screen
void mainScreen() {
    int choice;

    do {
        cout << "\n\n\n";  // Add some vertical spacing

        cout << "                      Welcome to Online Voting System\n\n";  // Centered by adding spaces
        cout << "                             1. Admin Panel\n";
        cout << "                             2. Voter Panel\n";
        cout << "                             3. Exit\n";
        cout << "                      Choose an option: ";

        // Input validation loop
        while (!(cin >> choice) || choice < 1 || choice > 3) {
            if (cin.fail()) {
                cin.clear(); // Clear the error flag on cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
                cout << "                      Invalid input! Please enter a number between 1 and 3.\n";
            } else if (choice < 1 || choice > 3) {
                cout << "                      Please choose a valid option (1, 2, or 3).\n";
            }
            cout << "                      Choose an option: ";
        }

        switch (choice) {
            case 1:
                if (authenticateAdmin()) adminPanel();
                else cout << "                      Incorrect password!\n";
                break;
            case 2:
                voterPanel();
                break;
            case 3:
                cout << "                      Exiting...\n";
                break;
        }

    } while (choice != 3);
}

// Function to authenticate the admin
bool authenticateAdmin() {
    string password;
    cout << "                      Enter Admin Password: ";
    cin >> password;
    return password == adminPassword;
}

// Function to handle the admin panel
void adminPanel() {
    int choice;
    do {
        cout <<"                      1. Add Party\n                      2. Add Candidate\n                      3. Add Voter\n                      4. View Results\n                      5. Exit Admin Panel\n                      Choose an option: ";
        // Input validation loop
        while (true) {
            string input;
            cin >> input;

            // Check if input is a digit and convert to integer
            if (input.length() == 1 && isdigit(input[0])) {
                choice = input[0] - '0'; // Convert char to int
                break; // Valid input, exit loop
            } else {
                cout << "                      Invalid input! Please enter a digit between 1 and 6: ";
            }
        }

        switch (choice) {
            case 1:
                addParty();
                break;
            case 2:
                addCandidate();
                break;
            case 3:
                addVoter();
                break;
            case 4:
                viewResults();
                break;
            case 5:
                cout << "                      Exiting Admin Panel...\n";
                break;
            default:
                cout << "                      Invalid option! Try again.\n";
        }
    } while (choice != 5);
}

// Function to add a party
void addParty() {
    if (partyCount >= 10) {
        cout << "                      Maximum limit of parties reached!\n";
        return;
    }
    
    string partyName;
    cout << "                      Enter Party Name: ";
    cin.ignore();
    getline(cin, partyName);

    if (partyName.empty() || any_of(partyName.begin(), partyName.end(), [](char c) { return !isalpha(c) && !isspace(c); })) {
        cout << "                      Invalid party name. Only alphabets allowed.\n";
        return;
    }

    for (int i = 0; i < partyCount; ++i) {
        if (parties[i].first == partyName) {
            cout << "                      Party already exists!\n";
            return;
        }
    }

    parties[partyCount++] = make_pair(partyName, nullptr);
    cout << "                      Party added successfully!\n";
    savePartiesToFile();
}

// Function to add a candidate
void addCandidate() {
    string partyName, candidateName;
    cout << "                      Enter Party Name: ";
    cin.ignore();
    getline(cin, partyName);

    for (int i = 0; i < partyCount; ++i) {
        if (parties[i].first == partyName) {
            cout << "                      Enter Candidate Name: ";
            getline(cin, candidateName);

            if (candidateName.empty() || any_of(candidateName.begin(), candidateName.end(), [](char c) { return !isalpha(c) && !isspace(c); })) {
                cout << "                      Invalid candidate name. Only alphabets allowed.\n";
                return;
            }

            if (findCandidate(parties[i].second, candidateName)) {
                cout << "                      Candidate already exists!\n";
                return;
            }

            insertCandidate(parties[i].second, candidateName);
            cout << "                      Candidate added successfully!\n";
            savePartiesToFile();
            return;
        }
    }

    cout << "                      Party not found!\n";
}

// Function to add a voter
void addVoter() {
    string voterID;
    cout << "                      Enter Voter ID (13 digits): ";
    cin >> voterID;

    if (voterID.length() == 13 && all_of(voterID.begin(), voterID.end(), ::isdigit) && voters.find(voterID) == voters.end()) {
        voters[voterID] = Voter(voterID);
        cout << "                      Voter added successfully!\n";
    } else {
        cout << "                      Invalid Voter ID or it already exists!\n";
    }
    saveVotersToFile();
}

// Function to view results
void viewResults() {
    cout << "                    Election Results:\n";
    ofstream resultFile("results.txt");

    for (int i = 0; i < partyCount; ++i) {
        cout << "Party: " << parties[i].first << "\n";
        resultFile << "Party: " << parties[i].first << "\n";
        displayCandidates(parties[i].second);
    }

    resultFile.close();
}

// Function to handle the voter panel
void voterPanel() {
    string voterID;
    cout << "                      Enter Voter ID (13 digits): ";
    cin >> voterID;

    if (voters.find(voterID) == voters.end() || voterID.length() != 13 || !all_of(voterID.begin(), voterID.end(), ::isdigit)) {
        cout << "                      Invalid Voter ID!\n";
        return;
    }

    
    int choice;

    do {
        cout << "                      1. Cast Vote\n                      2. View Results\n                      3. Exit\n                      Choose an option: ";

        // Input validation loop
        while (!(cin >> choice) || choice < 1 || choice > 3) {
            // Check if the input is non-numeric or out of range
            if (cin.fail()) {
                cin.clear(); // Clear the error flag on cin
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
                cout << "                      Invalid input! Please enter a number between 1 and 3.\n";
            } else if (choice < 1 || choice > 3) {
                cout << "                      Please choose a valid option (1, 2, or 3).\n";
            }
            cout << "                      Choose an option: ";
        }

        // Process the valid input
        switch (choice) {
            case 1:
                if (voters[voterID].hasVoted) {
                    cout << "                      You have already voted!\n";
                } else {
                    castVote(voterID);
                }
                break;
            case 2:
                viewResults();
                break;
            case 3:
                cout << "                      Exiting Voter Panel...\n";
                break;
            default:
                cout << "                      Invalid option! Try again.\n";
        }
    } while (choice != 3);
}

// Function to cast a vote
void castVote(const string& voterID) {
    auto startTime = chrono::steady_clock::now(); // Start the timer
    cout<<"                      you have 1 minute to cast your vote\n";

    cout << "                      Available Parties:\n";
    for (int i = 0; i < partyCount; ++i) {
        cout << i + 1 << ". " << parties[i].first << "\n";
        displayCandidates(parties[i].second);
    }

    int partyChoice;
    cout << "                      Select a party (1-" << partyCount << "): ";
    cin >> partyChoice;

    if (partyChoice < 1 || partyChoice > partyCount) {
        cout << "                      Invalid party selection!\n";
        return;
    }

    CandidateNode* candidateHead = parties[partyChoice - 1].second;
    cout << "                      Candidates:\n";
    displayCandidates(candidateHead);

    string candidateName;

    // Start a loop to check for the candidate name
    while (true) {
        cout << "                      Enter Candidate Name: ";
        cin.ignore();
        getline(cin, candidateName);

        // Check if 1 minutes have elapsed
        auto currentTime = chrono::steady_clock::now();
        auto elapsedTime = chrono::duration_cast<chrono::minutes>(currentTime - startTime);
        
        if (elapsedTime.count() >= 1) {
            cout << "                      Time limit exceeded! Returning to voter panel.\n";
            return; //  return to the voter panel if time limit exceeded
        }

        CandidateNode* candidate = findCandidate(candidateHead, candidateName);
        if (candidate) {
            candidate->votes++;
            voters[voterID].hasVoted = true;
            voters[voterID].voteHistory.push(candidateName);
            cout << "                      Vote cast successfully for " << candidateName << "!\n";
            savePartiesToFile();
            break; // Exit the loop after successful voting
        } else {
            cout << "                      Candidate not found! Please try again.\n";
        }
    }
}

// Function to insert a candidate into a linked list
void insertCandidate(CandidateNode*& head, const string& candidateName) {
    CandidateNode* newNode = new CandidateNode(candidateName);
    if (!head) head = newNode;
    else {
        CandidateNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

// Function to display candidates
void displayCandidates(CandidateNode* head) {
    while (head) {
        cout << "  Candidate: " << head->name << " - Votes: " << head->votes << "\n";
        head = head->next;
    }
}
// Function to find a candidate in a linked list
CandidateNode* findCandidate(CandidateNode* head, const string& name) {
    while (head) {
        if (head->name == name) return head;
        head = head->next;
    }
    return nullptr;
}

// Function to save voters to a file
void saveVotersToFile() {
    ofstream voterFile("voters.txt");
    for (const auto& entry : voters) {
        voterFile << entry.first << " " << (entry.second.hasVoted ? "1" : "0") << "\n";
    }
    voterFile.close();
}

// Function to load voters from a file
void loadVotersFromFile() {
    ifstream voterFile("voters.txt");
    string voterID;
    int hasVoted;

    while (voterFile >> voterID >> hasVoted) {
        voters[voterID] = Voter(voterID);
        voters[voterID].hasVoted = (hasVoted == 1);
    }
    voterFile.close();
}

// Function to save parties to a file
void savePartiesToFile() {
    ofstream partyFile("parties.txt", ios::trunc);  // Use truncation mode to overwrite the file
    if (!partyFile) {
        cout << "Unable to open parties.txt for writing.\n";
        return;
    }

    for (int i = 0; i < partyCount; ++i) {
        partyFile << parties[i].first << "\n";  // Write the party name
        CandidateNode* candidate = parties[i].second;
        while (candidate) {
            partyFile << "    " << candidate->name << " " << candidate->votes << "\n";  // Indent candidates for clarity
            candidate = candidate->next;
        }
        partyFile << "--\n";  // Write a separator after each party
    }
    partyFile.close();
}

// Function to load parties from a file
void loadPartiesFromFile() {
    ifstream partyFile("parties.txt");
    if (!partyFile) {
        cout << "Unable to open parties.txt for reading.\n";
        return;
    }

    string line;
    CandidateNode* lastCandidate = nullptr;
    while (getline(partyFile, line)) {
        if (line == "--") {  // Check for end of a party block
            partyCount++;  // Prepare to load the next party
            continue;
        }

        if (line.empty() || line[0] != ' ') {  // New party starts here
            parties[partyCount].first = line;
            parties[partyCount].second = nullptr;
            lastCandidate = nullptr;
        } else {  // Line describes a candidate
            string candidateName;
            int votes;
            stringstream ss(line);
            ss >> ws;  // Skip initial whitespace
            ss >> candidateName >> votes;
            CandidateNode* newNode = new CandidateNode(candidateName);
            newNode->votes = votes;
            if (lastCandidate == nullptr) {  // First candidate for this party
                parties[partyCount].second = newNode;
            } else {
                lastCandidate->next = newNode;
            }
            lastCandidate = newNode;
        }
    }
    partyFile.close();
}
