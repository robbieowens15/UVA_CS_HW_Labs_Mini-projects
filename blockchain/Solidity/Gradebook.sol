// SPDX-License-Identifier: Unlicensed

import "./IGradebook.sol";

pragma solidity ^0.8.16;

contract Gradebook is IGradebook {
    address public override instructor;

    mapping (address => bool) public override tas;

    // todo need sturct for assignments 

    uint public override num_assignments;
    mapping (string => uint) public assignment_lookup;
    mapping (uint => uint) public override max_scores;
    // memeory keyword required after string??
    mapping (uint => string) public override assignment_names;
    mapping (uint => mapping(string=> uint)) public override scores; // (assignment_id, user_id) |-> student score for that assignment

    constructor() {
		// make the message sender the instuctor!
        instructor = msg.sender;
	}

    // The following five functions are ones you must implement

    // Designates the passed address as a teaching assistant; re-designating
    // an address a TA does not do anything special (no revert).  Only the
    // instructor OR other TAs can designated TAs.
    function designateTA(address ta) public override {
        if (instructor == msg.sender || tas[msg.sender]){
            tas[ta] = true;
        }
    }

    // Adds an assignment of the given name with the given maximum score.  It
    // should revert if called by somebody other than the instructor or an
    // already designated teaching assistant.  It does not check if an
    // assignment with the same name already exists; thus, you can have
    // multiple assignments with the same name (but different IDs).  It
    // returns the assignment ID.
    function addAssignment(string memory name, uint max_score) public override returns (uint) {
        require(instructor == msg.sender || tas[msg.sender] == true, "Only the instructor or TA may add assignments");
        assignment_lookup[name] = num_assignments;
        assignment_names[num_assignments] = name;
        max_scores[num_assignments] = max_score;
        num_assignments ++;

        //emit assignment creation event
        emit assignmentCreationEvent(num_assignments);
        return num_assignments;
    }

    // Adds the given grade for the given student and the given assignment.
    // This should revert if (a) the caller is not the instructor or TA, or
    // (b) the assignment ID is invalid, or (c) the score is higher than the
    // allowed maximum score.
    function addGrade(string memory student, uint assignment, uint score) public override {
        require(instructor == msg.sender || tas[msg.sender] == true, "Only the instructor or TA may input grades");
        require(assignment < num_assignments, "This assignment does not exist");
        require(max_scores[assignment] >= score, "Can not score more than the assignment maximun.");

        scores[assignment][student] = score;

        //emit grade event
        emit gradeEntryEvent(assignment);
    }

    // Obtains the average of the given student's scores.  Each assignment is
    // weighted based on the number of points for that assignment.  So a 5/10
    // on one assignment and a 20/20 on another assignment would yield 25/30
    // points, or 83.33.  This returns 100 times that, or 8333.  Note that
    // the value is truncated, not rounded; so if the average were 16.67%, it
    // would return 1666.
    function getAverage(string memory student) public override view returns (uint) {
        uint sum_points = 0;
        uint max_points = 0;
        for (uint i = 0; i < num_assignments; i++) {
            sum_points += scores[i][student];
            max_points += max_scores[i];
        }

        return (sum_points * 100 * 100) / max_points;
    }

    // This function is how we are going to test your program -- we are going
    // to request TA access.  For this assignment, it will automatically make
    // msg.sender a TA, and has no effect if the sender is already a TA
    // (or instructor).  In reality, this would revert(), as only the
    // instructor (and other TAs) can make new TAs.
    function requestTAAccess() external override {
        // make sender TA
        tas[msg.sender] = true;
    }

    function supportsInterface(bytes4 interfaceId) public pure override returns (bool) {
        return interfaceId == type(IGradebook).interfaceId || interfaceId == 0x01ffc9a7;
    }
    
}