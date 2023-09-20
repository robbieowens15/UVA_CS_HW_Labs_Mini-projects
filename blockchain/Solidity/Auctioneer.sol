// SPDX-License-Identifier: GPL-3.0-or-later

pragma solidity ^0.8.16;

import "./IAuctioneer.sol";
import "./NFTManager.sol";

contract Auctioneer is IAuctioneer {

    uint public num_auctions;
    address public nftmanager;
    NFTManager the_nft_manager;
    uint public totalFees;
    uint public unpaidFees;
    mapping (uint => Auction) public auctions;
    address public deployer;

    // function auctions(uint id) override public view
    //         returns (uint, uint, string memory, uint, address, address, uint, uint, bool) {
        
    //         require(id < num_auctions, "Not a valid auction ID");
    //         Auction memory a = auctions[id];
    //         return (a.id, a.num_bids, a.data, a.highestBid, a.winner, a.initiator, a.nftid, a.endTime, a.active);
    //     }

    // function nftmanager() external view returns(address) {
    //     return address(nftmanager);
    // }


    constructor() {
        deployer = msg.sender;
        the_nft_manager = new NFTManager();
        nftmanager = address(the_nft_manager);
    }

    function collectFees() public override {
        require(msg.sender == deployer, "Only the contract deployer may collect fees!");

        (bool success, ) = payable(deployer).call{value: unpaidFees}("");
        require(success, "Failed to send Ether and collect fees");
        totalFees += unpaidFees;
        unpaidFees = 0;
    }

    function startAuction(uint m, uint h, uint d, string memory data, 
                          uint reserve, uint nftid) public override returns (uint) {
        require(msg.sender != address(0), "No null address for inititator");

        uint num_seconds = m*60 + h*60*60 + d*24*60*60;
        require(num_seconds != 0, "Auction must occur for non-zero seconds");
        require(keccak256(abi.encodePacked("")) != keccak256(abi.encodePacked(data)), "Auction must have a discription");
        for (uint i = 0; i < num_auctions; i++) {
            require(auctions[i].nftid != nftid || (auctions[i].nftid == nftid && auctions[i].active == false), "A live Auction for this NFT already exists");
        }

        // Attempt to retreive NFT
        require(the_nft_manager.getApproved(nftid) == address(this), "This auction contract not given premission to recieve NFT");
        the_nft_manager.transferFrom(msg.sender, address(this), nftid);

        //Create Auction
        Auction memory new_auction = Auction({
            id: num_auctions,
            num_bids: 0,
            data: data,
            highestBid: reserve,
            winner: address(0),
            initiator: msg.sender,
            nftid: nftid,
            endTime: block.timestamp + num_seconds,
            active: true
        });

        auctions[num_auctions] = new_auction;
        num_auctions++;

        //Emmit Event
        emit auctionStartEvent(auctions[num_auctions-1].id);

        return auctions[num_auctions-1].id;
    }

    function closeAuction(uint id) public override {
        require(id < num_auctions, "Not a valid auction ID");
        Auction memory the_auction = auctions[id];
        require(block.timestamp > the_auction.endTime, "Can not end the auction before the end time");

        the_auction.active = false;

        if (the_auction.num_bids == 0){
            //initate NFT transfer
            the_nft_manager.approve(the_auction.initiator, the_auction.nftid);
        }
        else {
            uint to_nft_owner = (the_auction.highestBid * 99) / 100;
            uint to_fees = the_auction.highestBid - to_nft_owner;

            require(to_nft_owner < the_auction.highestBid, "Bad auction payout vs. fee split");

            //initate NFT transfer
            require(the_auction.winner != address(0), "Can not transfer NFT to null address");
            the_nft_manager.approve(the_auction.winner, the_auction.nftid);

            //preform payments
            (bool success, ) = payable(the_auction.initiator).call{value: to_nft_owner}("");
            require(success, "Failed to transfer ETH to nft owner");
            unpaidFees += to_fees;
        }
        auctions[id] = the_auction;
        emit auctionCloseEvent(the_auction.id);
    }

    function placeBid(uint id) payable public {
        require(id < num_auctions, "Not a valid auction ID");
        Auction memory the_auction = auctions[id];
        require(block.timestamp < the_auction.endTime, "Can not bid on an ended auction!");
        require(msg.sender != the_auction.winner, "Bids can not be placed by null address");
        require(the_auction.highestBid < msg.value || (the_auction.num_bids == 0 && the_auction.highestBid <= msg.value), "Bid is not high enough to be winning this auction");
        require(msg.sender != address(0), "Bids can not be placed by null address");

        if (the_auction.winner != address(0)){
            // refund old high bidder
            (bool success, ) = payable(the_auction.winner).call{value: the_auction.highestBid}("");
            require(success, "Failed to transfer ETH to previous high bidder");
        }

        // update the auction
        the_auction.highestBid = msg.value;
        the_auction.winner = msg.sender;
        the_auction.num_bids++;
        auctions[id] = the_auction;

        emit higherBidEvent(the_auction.id);
    }

    function auctionTimeLeft(uint id) public view override returns (uint) {
        require(id < num_auctions, "Not a valid auction ID");
        Auction memory the_auction = auctions[id];

        if (block.timestamp > the_auction.endTime){
            return 0;
        } else{
            return the_auction.endTime - block.timestamp;
        }
    }


    // also supportsInterface(), because IAuctioneer inherits from IERC165
    function supportsInterface(bytes4 interfaceId) public override(IERC165) pure returns (bool){
        return (interfaceId == type(IAuctioneer).interfaceId
            || interfaceId == type(IERC165).interfaceId); 
    }
}