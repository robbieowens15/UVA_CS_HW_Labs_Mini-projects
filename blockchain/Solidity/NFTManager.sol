// SPDX-License-Identifier: Unlicensed
// Robert Owens - rao7utn

pragma solidity ^0.8.16;

import "./INFTManager.sol";
import "./ERC721.sol";

contract NFTManager is INFTManager, ERC721 {

    uint public nft_count;
    mapping(uint256 => string) public id_to_uri;
    mapping(uint256 => address) public id_to_owner;
    mapping(string => bool) public uri_is_unique;

    constructor() ERC721("", "Rob's NFT Manager"){}

    function mintWithURI(string memory _uri) public override returns (uint){
        nft_count++;

        require(uri_is_unique[_uri] == false, "Reassignmnet of uri to new id!");

        id_to_owner[nft_count] = msg.sender;
        id_to_uri[nft_count] = _uri;
        uri_is_unique[_uri] = true;

        ERC721._mint(msg.sender, nft_count);

        return nft_count;
    }

    function mintWithURI(address _to, string memory _uri) public override returns (uint){
        nft_count++;

        require(uri_is_unique[_uri] == false, "Reassignmnet of uri to new id!");

        id_to_owner[nft_count] = _to;
        id_to_uri[nft_count] = _uri;
        uri_is_unique[_uri] = true;

        ERC721._mint(_to, nft_count);

        return nft_count;
    }

    function tokenURI(uint256 tokenId) public view virtual override(IERC721Metadata,ERC721) returns (string memory) {
        require((tokenId != 0 && tokenId <= nft_count), "Invalid tokenId!");

        string memory base_url = "https://collab.its.virginia.edu/access/content/group/e9ad2fbb-faca-414b-9df1-6f9019e765e9/ipfs/";
        string memory uri = id_to_uri[tokenId];
        string memory full_uri = string.concat(base_url, uri);

        return full_uri;
    }

    function count() public view returns(uint) {
        return nft_count;
    }

    function supportsInterface(bytes4 interfaceId) public override(IERC165,ERC721) pure returns (bool){
        return (interfaceId == type(INFTManager).interfaceId
            || interfaceId == type(IERC721).interfaceId
            || interfaceId == type(IERC165).interfaceId
            || interfaceId == type(IERC721Metadata).interfaceId); 
    }

}