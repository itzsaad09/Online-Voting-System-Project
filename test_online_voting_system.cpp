// =============================================================================
// test_online_voting_system.cpp
//
// Unit Tests for the Online Voting System
// Quality Engineering Lab – Manual Testing Table
//
// Compile:  g++ -std=c++11 -o test_online_voting_system test_online_voting_system.cpp
// Run:      ./test_online_voting_system
//
// 5 units under test, 5 test cases each (25 tests total)
// =============================================================================

#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

// =============================================================================
// Data-structure definitions (mirrored from Online_Voting_System.cpp)
// =============================================================================

struct Voter {
    string voterID;
    bool hasVoted;
    stack<string> voteHistory;

    Voter(string id = "") {
        voterID  = id;
        hasVoted = false;
    }
};

struct CandidateNode {
    string name;
    int    votes;
    CandidateNode* next;

    CandidateNode(string candidateName) {
        name  = candidateName;
        votes = 0;
        next  = nullptr;
    }
};

// =============================================================================
// Pure / testable helper functions
// (extracted from Online_Voting_System.cpp for unit-testing purposes)
// =============================================================================

// Unit 1 – Admin authentication
// Returns true only when the supplied password matches the admin password.
bool authenticateAdminLogic(const string& password) {
    const string adminPassword = "admin123";
    return password == adminPassword;
}

// Unit 2 – Voter ID validation (logic used inside addVoter)
// Returns true when the ID is exactly 13 digits and not already registered.
bool isValidNewVoterID(const string& voterID,
                        const map<string, Voter>& voters) {
    return voterID.length() == 13 &&
           all_of(voterID.begin(), voterID.end(), ::isdigit) &&
           voters.find(voterID) == voters.end();
}

// Unit 3 – findCandidate (copied verbatim from Online_Voting_System.cpp)
CandidateNode* findCandidate(CandidateNode* head, const string& name) {
    while (head) {
        if (head->name == name) return head;
        head = head->next;
    }
    return nullptr;
}

// Unit 4 – insertCandidate (copied verbatim from Online_Voting_System.cpp)
void insertCandidate(CandidateNode*& head, const string& candidateName) {
    CandidateNode* newNode = new CandidateNode(candidateName);
    if (!head) {
        head = newNode;
    } else {
        CandidateNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

// Helper: free an entire candidate linked list
static void freeList(CandidateNode*& head) {
    while (head) {
        CandidateNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// =============================================================================
// Minimal test harness
// =============================================================================

static int g_passed = 0;
static int g_failed = 0;

#define RUN_TEST(name, condition)                                  \
    do {                                                           \
        if (condition) {                                           \
            cout << "  [PASS] " << (name) << "\n";                \
            ++g_passed;                                            \
        } else {                                                   \
            cout << "  [FAIL] " << (name) << "\n";                \
            ++g_failed;                                            \
        }                                                          \
    } while (0)

// =============================================================================
// Unit 1 – authenticateAdmin (5 test cases)
// =============================================================================

void test_authenticateAdmin() {
    cout << "\nUnit 1: authenticateAdmin\n";

    // TC-1: Correct password returns true
    RUN_TEST("test_authenticateAdmin_correctPassword",
             authenticateAdminLogic("admin123") == true);

    // TC-2: Wrong password returns false
    RUN_TEST("test_authenticateAdmin_wrongPassword",
             authenticateAdminLogic("wrongpass") == false);

    // TC-3: Empty string returns false
    RUN_TEST("test_authenticateAdmin_emptyPassword",
             authenticateAdminLogic("") == false);

    // TC-4: Partial match of the real password returns false
    RUN_TEST("test_authenticateAdmin_partialPassword",
             authenticateAdminLogic("admin") == false);

    // TC-5: Correct letters but wrong case returns false
    RUN_TEST("test_authenticateAdmin_caseSensitivePassword",
             authenticateAdminLogic("Admin123") == false);
}

// =============================================================================
// Unit 2 – Voter ID validation / addVoter (5 test cases)
// =============================================================================

void test_voterIDValidation() {
    cout << "\nUnit 2: Voter ID Validation (addVoter)\n";

    map<string, Voter> voters;
    voters["1234567890123"] = Voter("1234567890123"); // pre-registered voter

    // TC-1: Exactly 13 numeric digits that are not registered → valid
    RUN_TEST("test_addVoter_validID_13Digits",
             isValidNewVoterID("9876543210987", voters) == true);

    // TC-2: Fewer than 13 digits → invalid
    RUN_TEST("test_addVoter_shortID_lessThan13Digits",
             isValidNewVoterID("123456", voters) == false);

    // TC-3: More than 13 digits → invalid
    RUN_TEST("test_addVoter_longID_moreThan13Digits",
             isValidNewVoterID("12345678901234", voters) == false);

    // TC-4: 13 characters but contains letters → invalid
    RUN_TEST("test_addVoter_alphanumericID",
             isValidNewVoterID("1234567890ABC", voters) == false);

    // TC-5: 13-digit ID that already exists in the system → duplicate, invalid
    RUN_TEST("test_addVoter_duplicateID",
             isValidNewVoterID("1234567890123", voters) == false);
}

// =============================================================================
// Unit 3 – findCandidate (5 test cases)
// =============================================================================

void test_findCandidate() {
    cout << "\nUnit 3: findCandidate\n";

    // Build list: Alice -> Bob -> Charlie
    CandidateNode* head = nullptr;
    insertCandidate(head, "Alice");
    insertCandidate(head, "Bob");
    insertCandidate(head, "Charlie");

    // TC-1: Search for a candidate that exists in the middle → returns node
    RUN_TEST("test_findCandidate_existingCandidate",
             findCandidate(head, "Bob") != nullptr);

    // TC-2: Search for a candidate that does not exist → returns nullptr
    RUN_TEST("test_findCandidate_nonExistentCandidate",
             findCandidate(head, "Diana") == nullptr);

    // TC-3: Search in an empty list → returns nullptr
    RUN_TEST("test_findCandidate_emptyList",
             findCandidate(nullptr, "Alice") == nullptr);

    // TC-4: Search for the first candidate → returns the head node
    RUN_TEST("test_findCandidate_firstCandidate",
             findCandidate(head, "Alice") == head);

    // TC-5: Search for the last candidate → returns the tail node with correct name
    CandidateNode* result = findCandidate(head, "Charlie");
    RUN_TEST("test_findCandidate_lastCandidate",
             result != nullptr && result->name == "Charlie");

    freeList(head);
}

// =============================================================================
// Unit 4 – insertCandidate (5 test cases)
// =============================================================================

void test_insertCandidate() {
    cout << "\nUnit 4: insertCandidate\n";

    CandidateNode* head = nullptr;

    // TC-1: Insert into empty list → head is the new node with correct name
    insertCandidate(head, "Alice");
    RUN_TEST("test_insertCandidate_intoEmptyList",
             head != nullptr && head->name == "Alice");

    // TC-2: Insert into non-empty list → appended after existing node
    insertCandidate(head, "Bob");
    RUN_TEST("test_insertCandidate_intoExistingList",
             head->next != nullptr && head->next->name == "Bob");

    // TC-3: Three insertions → order preserved (Alice, Bob, Charlie)
    insertCandidate(head, "Charlie");
    RUN_TEST("test_insertCandidate_multipleCandidates",
             head->name == "Alice" &&
             head->next->name == "Bob" &&
             head->next->next->name == "Charlie");

    // TC-4: Newly inserted candidates start with zero votes
    RUN_TEST("test_insertCandidate_votesInitializedToZero",
             head->votes == 0 &&
             head->next->votes == 0 &&
             head->next->next->votes == 0);

    // TC-5: Candidate name is stored exactly as supplied
    RUN_TEST("test_insertCandidate_nameStoredCorrectly",
             findCandidate(head, "Charlie") != nullptr &&
             findCandidate(head, "Charlie")->name == "Charlie");

    freeList(head);
}

// =============================================================================
// Unit 5 – Voter struct (5 test cases)
// =============================================================================

void test_voterStruct() {
    cout << "\nUnit 5: Voter Struct\n";

    // TC-1: Default constructor sets empty voterID and hasVoted = false
    Voter defaultVoter;
    RUN_TEST("test_voter_defaultConstructor",
             defaultVoter.voterID == "" && defaultVoter.hasVoted == false);

    // TC-2: Parameterized constructor stores the supplied voterID
    Voter paramVoter("1234567890123");
    RUN_TEST("test_voter_parameterizedConstructor",
             paramVoter.voterID == "1234567890123");

    // TC-3: hasVoted is false immediately after construction
    RUN_TEST("test_voter_hasVotedInitiallyFalse",
             paramVoter.hasVoted == false);

    // TC-4: voteHistory stack is empty immediately after construction
    RUN_TEST("test_voter_voteHistoryInitiallyEmpty",
             paramVoter.voteHistory.empty() == true);

    // TC-5: voterID can be read back and matches the value passed to the constructor
    RUN_TEST("test_voter_voterIDStoredCorrectly",
             paramVoter.voterID == "1234567890123");
}

// =============================================================================
// Main – run all test suites and print summary
// =============================================================================

int main() {
    cout << "========================================\n";
    cout << "  Online Voting System – Unit Tests\n";
    cout << "  Quality Engineering Lab\n";
    cout << "========================================\n";

    test_authenticateAdmin();
    test_voterIDValidation();
    test_findCandidate();
    test_insertCandidate();
    test_voterStruct();

    cout << "\n========================================\n";
    cout << "  Results: " << g_passed << " passed, "
         << g_failed  << " failed  (total: "
         << g_passed + g_failed << ")\n";
    cout << "========================================\n";

    return g_failed == 0 ? 0 : 1;
}
