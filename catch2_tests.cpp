#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

// Include the implementation file directly (main() is excluded when DOCTEST_CONFIG_DISABLE is not defined)
#include "Online_Voting_System.cpp"

// Helper class to redirect cin and cout for interactive testing
struct RedirectIO {
    std::streambuf* old_cin;
    std::streambuf* old_cout;
    std::stringstream new_cin;
    std::stringstream new_cout;

    RedirectIO(const std::string& input = "") {
        new_cin << input;
        old_cin = std::cin.rdbuf(new_cin.rdbuf());
        old_cout = std::cout.rdbuf(new_cout.rdbuf());
    }

    ~RedirectIO() {
        std::cin.rdbuf(old_cin);
        std::cout.rdbuf(old_cout);
    }
    
    std::string output() {
        return new_cout.str();
    }
};

// Helper to reset the global state before each test
void resetGlobalState() {
    voters.clear();
    for (int i = 0; i < 10; ++i) {
        CandidateNode* current = parties[i].second;
        while (current) {
            CandidateNode* next = current->next;
            delete current;
            current = next;
        }
        parties[i] = std::make_pair("", nullptr);
    }
    partyCount = 0;
}

TEST_CASE("F-REQ-001 & NF-REQ-001: Admin Authentication", "[admin][auth]") {
    resetGlobalState();

    SECTION("Correct Password") {
        RedirectIO io("admin123\n");
        REQUIRE(authenticateAdmin() == true);
    }

    SECTION("Incorrect Password") {
        RedirectIO io("wrongpassword\n");
        REQUIRE(authenticateAdmin() == false);
    }
}

TEST_CASE("F-REQ-002 & NF-REQ-002: Party Management - Add Party", "[party]") {
    resetGlobalState();

    SECTION("Add valid party") {
        RedirectIO io("\nDemocracy Party\n");
        addParty();
        REQUIRE(partyCount == 1);
        REQUIRE(parties[0].first == "Democracy Party");
    }

    SECTION("Add duplicate party") {
        {
            RedirectIO io("\nDemocracy Party\n");
            addParty();
        }
        {
            RedirectIO io("\nDemocracy Party\n");
            addParty();
        }
        REQUIRE(partyCount == 1); // Duplicate should not be added
    }

    SECTION("Add invalid party name (contains numbers)") {
        RedirectIO io("\nParty 123\n");
        addParty();
        REQUIRE(partyCount == 0);
    }

    SECTION("Enforce 10 parties maximum limit") {
        for (int i = 0; i < 10; ++i) {
            RedirectIO io("Party" + std::to_string(i) + "\n"); // valid names (alphabetic only, wait, 'Party0' has numbers but wait, is it letters only?)
            // Let's use words like "PartyA", "PartyB", etc.
            std::string name = "Party";
            name += (char)('A' + i);
            RedirectIO io2("\n" + name + "\n");
            addParty();
        }
        REQUIRE(partyCount == 10);

        // Try adding the 11th party
        RedirectIO io("\nPartyK\n");
        addParty();
        REQUIRE(partyCount == 10);
    }
}

TEST_CASE("F-REQ-003 & F-REQ-004: Candidate Management", "[candidate]") {
    resetGlobalState();

    // Setup: add a party first
    {
        RedirectIO io("\nAlpha Party\n");
        addParty();
    }

    SECTION("Add valid candidate") {
        RedirectIO io("\nAlpha Party\nJohn Doe\n");
        addCandidate();
        REQUIRE(parties[0].second != nullptr);
        REQUIRE(parties[0].second->name == "John Doe");
        REQUIRE(parties[0].second->votes == 0);
    }

    SECTION("Add duplicate candidate in the same party") {
        {
            RedirectIO io("\nAlpha Party\nJohn Doe\n");
            addCandidate();
        }
        {
            RedirectIO io("\nAlpha Party\nJohn Doe\n");
            addCandidate();
        }
        // Linked list should only contain one John Doe
        int count = 0;
        CandidateNode* curr = parties[0].second;
        while (curr) {
            if (curr->name == "John Doe") count++;
            curr = curr->next;
        }
        REQUIRE(count == 1);
    }

    SECTION("Add invalid candidate name (numeric)") {
        RedirectIO io("\nAlpha Party\nJohn123\n");
        addCandidate();
        REQUIRE(parties[0].second == nullptr);
    }
}

TEST_CASE("F-REQ-005 & NF-REQ-002: Voter Registration - Add Voter", "[voter]") {
    resetGlobalState();

    SECTION("Valid 13-digit Voter ID") {
        RedirectIO io("1234567890123\n");
        addVoter();
        REQUIRE(voters.size() == 1);
        REQUIRE(voters.find("1234567890123") != voters.end());
    }

    SECTION("Invalid Voter ID (too short)") {
        RedirectIO io("12345\n");
        addVoter();
        REQUIRE(voters.empty());
    }

    SECTION("Invalid Voter ID (non-numeric)") {
        RedirectIO io("1234567890abc\n");
        addVoter();
        REQUIRE(voters.empty());
    }

    SECTION("Duplicate Voter ID prevention") {
        {
            RedirectIO io("1234567890123\n");
            addVoter();
        }
        {
            RedirectIO io("1234567890123\n");
            addVoter();
        }
        REQUIRE(voters.size() == 1);
    }
}

TEST_CASE("F-REQ-007, F-REQ-009, & F-REQ-010: Vote Casting and History", "[voting]") {
    resetGlobalState();

    // Setup: 1 party, 1 candidate, 1 voter
    {
        RedirectIO io("\nBeta Party\n");
        addParty();
    }
    {
        RedirectIO io("\nBeta Party\nJane Smith\n");
        addCandidate();
    }
    voters["9876543210123"] = Voter("9876543210123");

    SECTION("Cast valid vote") {
        // Option 1 for Beta Party, then Jane Smith
        RedirectIO io("1\nJane Smith\n");
        castVote("9876543210123");

        REQUIRE(voters["9876543210123"].hasVoted == true);
        REQUIRE(parties[0].second->votes == 1);
        REQUIRE(voters["9876543210123"].voteHistory.size() == 1);
        REQUIRE(voters["9876543210123"].voteHistory.top() == "Jane Smith");
    }
}

TEST_CASE("F-REQ-014 & F-REQ-015: Voter Data Persistence", "[voter][file]") {
    resetGlobalState();

    voters["1111111111111"] = Voter("1111111111111");
    voters["2222222222222"] = Voter("2222222222222");
    voters["2222222222222"].hasVoted = true;

    saveVotersToFile();
    
    // Clear and reload
    voters.clear();
    loadVotersFromFile();

    REQUIRE(voters.size() == 2);
    REQUIRE(voters["1111111111111"].hasVoted == false);
    REQUIRE(voters["2222222222222"].hasVoted == true);
}

TEST_CASE("F-REQ-016 & F-REQ-017: Party and Candidate Data Persistence", "[party][file]") {
    resetGlobalState();

    {
        RedirectIO io("\nGamma Party\n");
        addParty();
    }
    {
        RedirectIO io("\nGamma Party\nAlice\n");
        addCandidate();
    }

    savePartiesToFile();

    // Clear and reload
    resetGlobalState();
    loadPartiesFromFile();

    REQUIRE(partyCount == 1);
    REQUIRE(parties[0].first == "Gamma Party");
    REQUIRE(parties[0].second != nullptr);
    REQUIRE(parties[0].second->name == "Alice");
}
