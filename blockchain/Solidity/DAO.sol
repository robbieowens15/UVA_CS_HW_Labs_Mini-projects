// SPDX-License-Identifier: Unlicnesed

import "./IDAO.sol";
import "./NFTManager.sol";

pragma solidity ^0.8.16;

contract DAO is IDAO {

    mapping (uint => Proposal) public proposals;
    uint constant public override minProposalDebatePeriod = 600;
    address public tokens;
    NFTManager public nfts; 
    string public purpose;
    mapping (address => mapping(uint => bool)) public votedYes;
    mapping (address => mapping(uint => bool)) public votedNo;
    uint public numberOfProposals;
    string public howToJoin;
    uint public reservedEther;
    address public curator;

    mapping (address => bool) public isMember;


    constructor() {
        numberOfProposals = 0;
        reservedEther = 0;
        nfts = new NFTManager();
        tokens = address(nfts);
        purpose = "I want your money. Give me your money!";
        howToJoin = "Send the blood of your first born child to Rumplestilsken - also 1 Eth";
        curator = msg.sender;
        isMember[msg.sender] = true;
        nfts.mintWithURI(msg.sender, substring(Strings.toHexString(msg.sender),2,34));
    }

    receive() external payable override {}

    function newProposal(address recipient, uint amount, string memory description, 
            uint debatingPeriod) public override payable returns (uint) {
        
        require(debatingPeriod >= minProposalDebatePeriod, "Proposed Debate time is too short!");
        require(isMember[msg.sender] == true, "The proposal submitter is not a member of the DAO");
        require(address(this).balance > reservedEther + amount, "Not enough funds to run this proposal");

        Proposal memory p = Proposal(
            recipient,
            amount,
            description,
            block.timestamp + debatingPeriod, 
            true,
            false,
            0,
            0,
            msg.sender    
        );

        reservedEther += amount;

        uint id = numberOfProposals;
        proposals[id] = p;
        numberOfProposals++;

        emit NewProposal(id, recipient, amount, description);
        return id;
    }

    function vote(uint proposalID, bool supportsProposal) public override {
        require(proposalID < numberOfProposals, "Not a valid proposal ID");
        require(isMember[msg.sender] == true, "The voter is not a member of the DAO");

        if (supportsProposal) {
            votedYes[msg.sender][proposalID] = true;
            proposals[proposalID].yea ++;
        } else {
            votedNo[msg.sender][proposalID] = true;
            proposals[proposalID].nay ++;
        }
        emit Voted(proposalID, supportsProposal, msg.sender);
    }

    function closeProposal(uint proposalID) public override {
        require(proposalID < numberOfProposals, "Not a valid proposal ID");
        require(block.timestamp > proposals[proposalID].votingDeadline, "Votes are stil tabulating...");
        require(isMember[msg.sender] == true, "The sender is not a member of the DAO");

        bool result = false;

        proposals[proposalID].open = false;
        if (proposals[proposalID].yea > proposals[proposalID].nay) {
            // Measure Passes; yaya!!
            result = true;

            // send money to msg.sender
            (bool success, ) = payable(proposals[proposalID].recipient).call{value: proposals[proposalID].amount}("");
            require (success, "Failed to pay the intended address");

        }
        reservedEther -= proposals[proposalID].amount;

        emit ProposalClosed(proposalID, result);
    }

    function substring(string memory str, uint startIndex, uint endIndex) private pure returns (string memory) {
        bytes memory strBytes = bytes(str);
        bytes memory result = new bytes(endIndex-startIndex);
        for(uint i = startIndex; i < endIndex; i++)
            result[i-startIndex] = strBytes[i];
        return string(result);
    }

    function addMember(address who) public override {
        require(isMember[msg.sender] == true, "Only existing members can add members to the DAO");

        isMember[who] = true;

        nfts.mintWithURI(who, substring(Strings.toHexString(who),2,34));
    }

    function requestMembership() pure public override {
        revert();
    }

    function supportsInterface(bytes4 interfaceId) public override pure returns (bool){
        return (interfaceId == type(IDAO).interfaceId
            || interfaceId == type(IERC165).interfaceId); 
    }
}