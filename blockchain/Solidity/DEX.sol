// SPDX-License-Identifier: Unlicensed
pragma solidity ^0.8.16;

import "./IDEX.sol";
import "./TokenCC.sol";

contract DEX is IDEX {

    // Events
    // event liquidityChangeEvent();

    bool private erc20Recieved_on = true;
    bool private is_setup = false;
    address private deployer;

    //liquidity - defining the indifference curve
    uint public k; // indifference curve level
    uint public x; // liquidity amount of Hootherium (fETH)
    uint public y; // liquidity amount of MXC (Token)
    mapping (address => uint) public etherLiquidityForAddress;
    mapping (address => uint) public tokenLiquidityForAddress;

    //Fees
    // define the fee amout as a ratio for precentage
    uint public feeNumerator;
    uint public feeDenominator;
    uint public feesEther;
    uint public feesToken;

    //ether pricer
    address public etherPricer;
    address public erc20Address;
    
    //Getters
    // --------------
    function symbol() public override view returns (string memory) {
        ERC20 token = ERC20(erc20Address);
        return token.symbol();
    }

    function decimals() public override view returns (uint) {
        ERC20 token = ERC20(erc20Address);
        return token.decimals();
    }

    function getEtherPrice() public override view returns(uint) {
        IEtherPriceOracle pricer = IEtherPriceOracle(etherPricer);
        return pricer.price();
    }

    function getTokenPrice() public override view returns(uint) {
        IEtherPriceOracle pricer = IEtherPriceOracle(etherPricer);
        uint ethPrice = pricer.price();
        // ??? Close but incorrect !!!
        ERC20 token = ERC20(erc20Address);
        // uint amoutEth = x / (10**18);
        // uint amountTC = y / (10**token.decimals());
        return ((ethPrice * x * (10**token.decimals())) / ((10**18) * y));
    }

    function getPoolLiquidityInUSDCents() public override view returns (uint) {
        IEtherPriceOracle pricer = IEtherPriceOracle(etherPricer);
        uint cryptoToCentsExchangeRate = pricer.price();
        uint valueEth = (x * cryptoToCentsExchangeRate) / (10**18);
        return valueEth*2;
    }

    // Functions for debugging and grading
    function setEtherPricer(address p) public override {
        etherPricer = p;
    }
    // ---------------

    // Functions for efficiency
    function getDEXinfo() public view override returns (address, string memory, string memory, 
                            address, uint, uint, uint, uint, uint, uint, uint, uint){
        
        ERC20 token = ERC20(erc20Address);
        return (address(this), token.symbol(), token.name(), erc20Address, 
            k, x, y, feeNumerator, feeDenominator, token.decimals(), feesEther, feesToken);
    }

    constructor(){
        deployer = msg.sender;
        erc20Recieved_on = true;
    }
    
    // Pool creation
    function createPool(uint _tokenAmount, uint _feeNumerator, uint _feeDenominator, 
                        address _erc20token, address _etherPricer) public override payable {
        
        erc20Recieved_on = false;
        require(is_setup == false, "Can only call create pool once!");
        require(msg.sender == deployer, "Only the contract deployer can call this function.");

        feeNumerator = _feeNumerator;
        feeDenominator = _feeDenominator;
        require(feeNumerator < feeDenominator, "Cannot charge fees of 100% or higher");
        erc20Address = _erc20token;
        etherPricer = _etherPricer;

        x = msg.value;
        y = _tokenAmount;
        require(x > 0, "Must deposit postive eth for liquidity");
        require(y > 0, "Must deposit positve token for liquidity");
        k = x*y;

        //attempt to get the ERC20 currency
        ERC20 token = ERC20(erc20Address);
        require(token.transferFrom(msg.sender, address(this), y), "transferFrom reverted - did sender call approve()?");

        etherLiquidityForAddress[msg.sender] = x;
        tokenLiquidityForAddress[msg.sender] = y;


        is_setup = true;
        erc20Recieved_on = true;
        emit liquidityChangeEvent();
    }

    // Managing pool liquidity - REMEMBER TO "TURN OFF" onERC20Received!
    function addLiquidity() public override payable {
        erc20Recieved_on = false;
        uint eth_transfered = msg.value;
        require(eth_transfered > 0, "Must have added a positive amount of eth liquidity");

        // Is this correct equation to keep liquidity balenced between the two assets. (inc k, maintain x/y)
        uint mxc_to_capture = (eth_transfered * y) / x;
        require(mxc_to_capture > 0, "liquitidy balencing computation is incorrect");

        ERC20 token = ERC20(erc20Address);
        require(token.transferFrom(msg.sender, address(this), mxc_to_capture), "transferFrom reverted - did sender call approve()? with appropriate amount?");

        x += eth_transfered;
        y += mxc_to_capture;
        k = x*y;

        etherLiquidityForAddress[msg.sender] += eth_transfered;
        tokenLiquidityForAddress[msg.sender] += mxc_to_capture;

        erc20Recieved_on = true;
        emit liquidityChangeEvent();
    }

    function removeLiquidity(uint amountEther) public override {
        erc20Recieved_on = false;
        uint amountToken = (amountEther * y) / x;

        require(etherLiquidityForAddress[msg.sender] >= amountEther, "sender attempting to withdraw more liquidity than they have (Eth)");
        require(x > amountEther, "cannot withdraw all liquidity (Eth)");
        require(tokenLiquidityForAddress[msg.sender] >= amountToken, "sender attempting to withdraw more liquidity than they have (Token)");
        require(y > amountToken, "cannot withdraw all liquidity (Token)");

        // send money to msg.sender
        (bool success, ) = payable(msg.sender).call{value: amountEther}("");
        require (success, "remove liquidity - eth payment didn't work");
        ERC20 token = ERC20(erc20Address);
        require(token.approve(msg.sender, amountToken), "Trying to approve return of token - but reverted");

        //update dex state
        tokenLiquidityForAddress[msg.sender] -= amountToken;
        etherLiquidityForAddress[msg.sender] -= amountEther;
        x -= amountToken;
        y -= amountEther;
        k = x*y;

        erc20Recieved_on = true;
        emit liquidityChangeEvent();
    }

    // When the DEX recieves ETH return to the sender exchange rate MXC - FEES
    receive() external payable override { // might need 'override' also
        uint amountEth = msg.value;

        // compute conversion
        x += amountEth;
        uint maintToken = k / x;
        uint amountToken = y - maintToken;
        y = maintToken;
        require(x*y == k, "Indifference curve was not maintained! - receive()");

        //take fee
        uint fee = (amountToken * feeNumerator) / feeDenominator;
        amountToken -= fee;
        feesToken += fee;

        // Send token back to sender; completing exchange
        ERC20 token = ERC20(erc20Address);
        require(token.approve(msg.sender, amountToken), "Trying to approve return of token - but reverted");
    }

    // When the DEX recieves MXC return to the sender exchange rate ETH - FEES
    function onERC20Received(address from, uint amount) public override returns (bool) {
        if (erc20Recieved_on == false) {
            return true; //do nothing - "turned off" temporarily by liquidity management
        }

        uint amountToken = amount;

        // compute conversion
        y += amountToken;
        uint maintEth = k / y;
        uint amountEth = x - maintEth;
        x = maintEth;
        require(x*y == k, "Indifference curve was not maintained! - onERC20Received()");

        //take fee
        uint fee = (amountEth * feeNumerator) / feeDenominator;
        amountEth -= fee;
        feesEther += fee;

        // send money to msg.sender
        (bool success, ) = payable(from).call{value: amountEth}("");
        require (success, "onERC20Received - eth payment didn't work");

        return true;
    }


    function supportsInterface(bytes4 interfaceId) public override pure returns (bool){
        return (interfaceId == type(IDEX).interfaceId
            || interfaceId == type(IERC165).interfaceId
            || interfaceId == type(IERC20Receiver).interfaceId); 
    }

}