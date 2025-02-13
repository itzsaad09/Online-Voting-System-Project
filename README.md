# Online Voting System

# Overview
  The Online Voting System is a console-based application written in C++ that allows users to manage an election process. It provides functionalities for both administrators and voters, including adding parties and candidates, casting votes, and viewing election results. The system ensures that each voter can only vote once and maintains a history of votes for potential undo operations.

# Features
  1. Admin Panel: Administrators can add parties, candidates, and voters, as well as view election results.
  2. Voter Panel: Voters can cast their votes, view results, and ensure they have not voted multiple times.
  3. Vote History: Each voter has a history of their votes, allowing for potential undo operations.
  4. Data Persistence: Voter and party information is saved to and loaded from text files, ensuring data is retained between sessions.

# Requirements
  . C++ compiler (e.g., g++, MSVC)
  . Standard C++ library
  . Basic knowledge of console applications

# How to Compile and Run
  1. Clone the Repository: If the code is in a repository, clone it to your local machine.
  2. Open a Terminal: Navigate to the directory where the code is located.
  3. Compile the Code: Use a C++ compiler to compile the code. For example:
    
         g++ -o voting_system voting_system.cpp
  4. Run the Application: Execute the compiled program:

         ./voting_system

# Usage
  1. Upon starting the application, users are presented with a main menu.
  2. Admins can access the admin panel by entering the correct password (admin123).
  3. Voters can enter their Voter ID (13 digits) to access the voter panel.
  4. Admins can add parties, candidates, and voters, and view results.
  5. Voters can cast their votes and view the results of the election.

# Code Structure
  Structures:
    Voter: Holds information about each voter, including their ID, voting status, and vote history.
    CandidateNode: Represents a candidate in a linked list, storing the candidate's name and vote count.

  Global Variables:
    map<string, Voter> voters: Stores all voters by their Voter ID.
    pair<string, CandidateNode*> parties[10]: Stores parties and their associated candidates.
    int partyCount: Keeps track of the number of parties.

  Functions:
    mainScreen(): Displays the main menu and handles user input.
    adminPanel(): Manages admin functionalities.
    voterPanel(): Manages voter functionalities.
    addParty(), addCandidate(), addVoter(): Functions to add parties, candidates, and voters.
    viewResults(): Displays the election results.
    castVote(): Allows voters to cast their votes.
    saveVotersToFile(), loadVotersFromFile(): Functions to save and load voter data.
    savePartiesToFile(), loadPartiesFromFile(): Functions to save and load party data.

# Error Handling
  The application includes input validation to ensure that only valid data is processed.
  Voter IDs must be 13 digits long and numeric.
  Party and candidate names must consist of alphabetic characters only.
