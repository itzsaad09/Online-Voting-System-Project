#include <gtest/gtest.h>
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

// Test case for F-REQ-001 & NF-REQ-001: Admin Authentication
TEST(VotingSystemTests, AdminAuthentication) {
    resetGlobalState();

    {
        RedirectIO io("admin123\n");
        EXPECT_TRUE(authenticateAdmin());
    }

    {
        RedirectIO io("wrongpassword\n");
        EXPECT_FALSE(authenticateAdmin());
    }
}

// Test case for F-REQ-002: Party Management
TEST(VotingSystemTests, AddParty) {
    resetGlobalState();

    {
        RedirectIO io("\nDemocracy Party\n");
        addParty();
        EXPECT_EQ(partyCount, 1);
        EXPECT_EQ(parties[0].first, "Democracy Party");
    }

    {
        RedirectIO io("\nDemocracy Party\n");
        addParty();
        EXPECT_EQ(partyCount, 1); // Duplicates should be prevented
    }

    {
        RedirectIO io("\nInvalid123\n");
        addParty();
        EXPECT_EQ(partyCount, 1); // Invalid party should not be added
    }
}

// Test case for F-REQ-003 & F-REQ-004: Candidate Management
TEST(VotingSystemTests, CandidateManagement) {
    resetGlobalState();

    {
        RedirectIO io("\nBeta Party\n");
        addParty();
    }

    {
        RedirectIO io("\nBeta Party\nJane Doe\n");
        addCandidate();
        ASSERT_NE(parties[0].second, nullptr);
        EXPECT_EQ(parties[0].second->name, "Jane Doe");
        EXPECT_EQ(parties[0].second->votes, 0);
    }

    {
        RedirectIO io("\nBeta Party\nJane Doe\n");
        addCandidate();
        // Duplicates within the same party prevented
        int count = 0;
        CandidateNode* curr = parties[0].second;
        while (curr) {
            if (curr->name == "Jane Doe") count++;
            curr = curr->next;
        }
        EXPECT_EQ(count, 1);
    }
}

// Test case for F-REQ-005: Voter Registration
TEST(VotingSystemTests, VoterRegistration) {
    resetGlobalState();

    {
        RedirectIO io("1234567890123\n");
        addVoter();
        EXPECT_EQ(voters.size(), 1);
        EXPECT_NE(voters.find("1234567890123"), voters.end());
    }

    {
        RedirectIO io("12345\n"); // too short
        addVoter();
        EXPECT_EQ(voters.size(), 1);
    }

    {
        RedirectIO io("1234567890123\n"); // Duplicate
        addVoter();
        EXPECT_EQ(voters.size(), 1);
    }
}

// Test case for F-REQ-007: Vote Casting
TEST(VotingSystemTests, VoteCastingAndHistory) {
    resetGlobalState();

    {
        RedirectIO io("\nVote Party\n");
        addParty();
    }
    {
        RedirectIO io("\nVote Party\nAlice Smith\n");
        addCandidate();
    }
    voters["9876543210123"] = Voter("9876543210123");

    {
        RedirectIO io("1\nAlice Smith\n");
        castVote("9876543210123");

        EXPECT_TRUE(voters["9876543210123"].hasVoted);
        EXPECT_EQ(parties[0].second->votes, 1);
        EXPECT_EQ(voters["9876543210123"].voteHistory.size(), 1);
        EXPECT_EQ(voters["9876543210123"].voteHistory.top(), "Alice Smith");
    }
}

// Test case for Persistence
TEST(VotingSystemTests, FilePersistence) {
    resetGlobalState();

    voters["1234567890123"] = Voter("1234567890123");
    voters["1234567890123"].hasVoted = true;
    saveVotersToFile();

    voters.clear();
    loadVotersFromFile();
    EXPECT_EQ(voters.size(), 1);
    EXPECT_TRUE(voters["1234567890123"].hasVoted);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
