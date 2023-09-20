import java.security.*;
import java.io.*;
import java.nio.file.*;
import java.security.spec.*;
import java.text.SimpleDateFormat;
import java.util.*;

/**
 * I/O toa nd from specified wallet destinations
 *  Also has helpful security utility files
 */
class Wallet{

    public KeyPair pair;
    public String name;
    public String publicKey;
    public String privateKey;
    public String signature;

    /**
     * Constructor. 
     *  Case 1: for a new wallet - generate and save key/value pairs to file
     *  Case 2: for an existing file read the existing key/value pairs
     */
    public Wallet(final String walletName){
        this.name = walletName;

        File walletFile = new File(walletName);
        if(walletFile.exists()){
            try {
                this.pair = LoadKeyPair(walletName);
            } catch (final Exception e) {
                System.out.println("Error occurred while attempting to obtain a key pair from existing wallet: " + walletName);
                e.printStackTrace();
            }
        }
        else{
            try {
                this.pair = createKeyPair();
                SaveKeyPair(walletName, pair);
            } catch (final Exception e) {
                System.out.println("Error occurred while attempting to generate a key pair for new wallet: "+ walletName);
                e.printStackTrace();
            }
        }
        try {
            Scanner scanner = new Scanner(new File(this.name));
            this.publicKey = scanner.nextLine();
            this.privateKey = scanner.nextLine();
            scanner.close();
            this.signature = Wallet.getHashOfString(this.publicKey).substring(0, 16);
        } catch (final FileNotFoundException e) {
            System.out.println("Error Attempting to read wallet file. The file: " + this.name + " does not exist!");
            e.printStackTrace();
        } catch (final NoSuchAlgorithmException e) {
            System.out.println("Error Attempting to create the signature (hash of public key) for wallet: " + this.name);
            e.printStackTrace();
        } 
        catch (final Exception e) {
            System.out.println("Error Attempting to read wallet file: " + this.name);
            e.printStackTrace();
        }
    }

    // All functions below are static helper files
    public static boolean walletExists(final String name){
        File walletFile = new File(name);
        return walletFile.exists();
    }

    //Provide a signature given this wallet and a message
    public String getSignatureOfMessage(final String message) throws Exception {
        Signature privateSignature = Signature.getInstance("SHA256withRSA");
        privateSignature.initSign(this.pair.getPrivate());
        privateSignature.update(message.getBytes("UTF-8"));

        byte[] signature = privateSignature.sign();

        return Base64.getEncoder().encodeToString(signature);
    }

    // Given a message, private signature and public key verify the message is legitimate
    public static boolean verify(final String messageToVerify, final String signature, PublicKey key) throws Exception{
        Signature publicSignature = Signature.getInstance("SHA256withRSA");
        publicSignature.initVerify(key);
        publicSignature.update(messageToVerify.getBytes("UTF-8"));

        byte[] signatureBytes = Base64.getDecoder().decode(signature);

        return publicSignature.verify(signatureBytes);
    }

    public static int getBalance(String walletAddress) {
        int balance = 0;

        int blockIndex = 1;
        while (true) {
            File block = new File("block_" + blockIndex + ".txt");
            if (! block.exists())
                break;
            String[] transactionRecords = Block.getTransactionRecords(block);
            for (int i = 0; i < transactionRecords.length; i++) {
                String[] parsedRecord = Transaction.parseRecord(transactionRecords[i]); //index {0: sender, 1: receiver, 2: amount}
                if (parsedRecord[0].equals(walletAddress))
                    balance -= Integer.parseInt(parsedRecord[2]);
                else if (parsedRecord[1].equals(walletAddress))
                    balance += Integer.parseInt(parsedRecord[2]);
            }
            blockIndex++;
        }

        String[] transactionRecords = Mempool.getTransactionRecords(false).split("\n");
        if (transactionRecords[0] == "")
            return balance; //nothing in mempool
        for (int i = 0; i < transactionRecords.length; i++) {
            String[] parsedRecord = Transaction.parseRecord(transactionRecords[i]); //index {0: sender, 1: receiver, 2: amount}
            if (parsedRecord[0].equals(walletAddress))
                balance -= Integer.parseInt(parsedRecord[2]);
            else if (parsedRecord[1].equals(walletAddress))
                balance += Integer.parseInt(parsedRecord[2]);
        }

        return balance;
    }

    // this converts an array of bytes into a hexadecimal number in
    // text format
    private static String getHexString(byte[] b) {
        String result = "";
        for (int i = 0; i < b.length; i++) {
            int val = b[i];
            if ( val < 0 )
            val += 256;
            if ( val <= 0xf )
            result += "0";
            result += Integer.toString(val, 16);
        }
        return result;
    }
    
    // this converts a hexadecimal number in text format into an array
    // of bytes
    private static byte[] getByteArray(String hexstring) {
        byte[] ret = new byte[hexstring.length()/2];
        for (int i = 0; i < hexstring.length(); i += 2) {
            String hex = hexstring.substring(i,i+2);
            if ( hex.equals("") )
            continue;
            ret[i/2] = (byte) Integer.parseInt(hex,16);
        }
        return ret;
    }

    private static KeyPair createKeyPair() throws NoSuchAlgorithmException {
        KeyPairGenerator keyGen = KeyPairGenerator.getInstance("RSA");
        keyGen.initialize(1024);
        KeyPair pair = keyGen.generateKeyPair();
        return pair;
    }


    // This will write the public/private key pair to a file in text
    // format.  It is adapted from the code from
    // https://snipplr.com/view/18368/saveload--private-and-public-key-tofrom-a-file/
    private static void SaveKeyPair(String filename, KeyPair keyPair) throws Exception {
        X509EncodedKeySpec x509EncodedKeySpec = new X509EncodedKeySpec(keyPair.getPublic().getEncoded());
        PKCS8EncodedKeySpec pkcs8EncodedKeySpec = new PKCS8EncodedKeySpec(keyPair.getPrivate().getEncoded());
        PrintWriter fout = new PrintWriter(new FileOutputStream(filename));
        fout.println(getHexString(x509EncodedKeySpec.getEncoded()));
        fout.println(getHexString(pkcs8EncodedKeySpec.getEncoded()));
        fout.close();
    }

    // This will read a public/private key pair from a file.  It is
    // adapted from the code from
    // https://snipplr.com/view/18368/saveload--private-and-public-key-tofrom-a-file/
    private static KeyPair LoadKeyPair(String filename) throws Exception {
        // Read wallet
        Scanner sin = new Scanner(new File(filename));
        byte[] encodedPublicKey = getByteArray(sin.next());
        byte[] encodedPrivateKey = getByteArray(sin.next());
        sin.close();
        // Generate KeyPair.
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");
        X509EncodedKeySpec publicKeySpec = new X509EncodedKeySpec(encodedPublicKey);
        PublicKey publicKey = keyFactory.generatePublic(publicKeySpec);
        PKCS8EncodedKeySpec privateKeySpec = new PKCS8EncodedKeySpec(encodedPrivateKey);
        PrivateKey privateKey = keyFactory.generatePrivate(privateKeySpec);
        return new KeyPair(publicKey, privateKey);
    }
    
    // This will get the SHA-256 hash of a file, and is the same as
    // calling the `sha256sum` command line program
    public static String getHashOfFile(String filename) throws Exception {
        byte[] filebytes = Files.readAllBytes(Paths.get(filename));
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        byte[] encodedHash = digest.digest(filebytes);
        return getHexString(encodedHash);
    }

    // This will get the SHA-256 hash of a String, and is the same as
    // calling the `sha256sum` command line program
    public static String getHashOfString(String s) throws Exception {
        byte[] strbytes = s.getBytes();
        MessageDigest digest = MessageDigest.getInstance("SHA-256");
        byte[] encodedHash = digest.digest(strbytes);
        return getHexString(encodedHash);
    }
}

/**
 * Proposal for a new transaction on the public blockchain ledger
 * 
 * validate() - can is be preformed? Formatted? Funds? Signature valid?
 * toTransactionRecord() - transformation into mempool and blockchain record
 */
class Transaction {
    
    private static final boolean False = false;

    /**
     * Creates a transaction record file
     * FOR FUNDING FROM ADAM
     */
    public static boolean createTransaction(String receiver, int amount, String date, String filename) {

        try {
            File newTransactionFile = new File(filename);
            if(!newTransactionFile.createNewFile()) {
                return false;
            }
            FileWriter myWriter = new FileWriter(newTransactionFile);
            myWriter.write(
                "From: " + CMoney.moneyPrinter + "\n"
                +"To: " + receiver + "\n"
                +"Amount: " + Integer.toString(amount) + "\n"
                +"Date: "+ date + "\n"
                +"\n"
            );
            myWriter.close();

        } catch (final Exception e) {
            return false;
        }
    
        return true;
    }
    /**
     * Creates a transaction record file
     * FOR PEER TO PEER PAYMENTS
     */
    public static boolean createTransaction(Wallet senderWallet, String receiver, int amount, String date, String filename) {

        try {
            File newTransactionFile = new File(filename);
            if(!newTransactionFile.createNewFile()) {
                return false;
            }
            FileWriter myWriter = new FileWriter(newTransactionFile);
            myWriter.write(
                "From: " + senderWallet.signature + "\n"
                +"To: " + receiver + "\n"
                +"Amount: " + Integer.toString(amount) + "\n"
                +"Date: "+ date + "\n"
                +"\n"
            );
            myWriter.close();

            final String msgHash = Wallet.getHashOfFile(filename);
            String sig = senderWallet.getSignatureOfMessage(msgHash);
            myWriter = new FileWriter(newTransactionFile, true);
            myWriter.write(sig + "\n");
            myWriter.close();

        } catch (final Exception e) {
            return false;
        }
    
        return true;
    }

    // 0 = standard 1 = from god 2 = error
    public static short verify(Wallet wallet, final String transactionFile) {
        File file = new File(transactionFile);
        String[] lines = new String[6];
        if(file.exists()){
            try {
                Scanner sin = new Scanner(new File(transactionFile));
                int i = 0;
                while(sin.hasNextLine()){
                    lines[i] = sin.nextLine();
                    i++;
                }
                sin.close();
            } catch (final Exception e) {
                return 2;
            }
        }
        else {
            return 2;
        }

        final String sender = lines[0].split(" ", 2)[1];
        final String receiver = lines[1].split(" ", 2)[1];
        final int amount = Integer.parseInt(lines[2].split(" ", 2)[1]);
        final String date = lines[3].split(" ", 2)[1];

        final String record = Transaction.toRecordFormat(sender, receiver, amount, date);
        // FUND Case - automatically verified
        if (sender.equals(CMoney.moneyPrinter) && wallet.signature.equals(receiver)) {
            Mempool.writeRecord(record);
            return 1;
        } else if (sender.equals(CMoney.moneyPrinter) && !wallet.signature.equals(receiver)){
            return 2;
        }

        final String msg = "From: " + sender + "\n"
            +"To: " + receiver + "\n"
            +"Amount: " + Integer.toString(amount) + "\n"
            +"Date: "+ date + "\n"
            +"\n";

        try {
            final String msgHash = Wallet.getHashOfString(msg);
            final String sig = lines[5];
            final PublicKey publicKey = wallet.pair.getPublic();

            if (Wallet.verify(msgHash, sig, publicKey) == False) {
                return 2;
            }
        } catch (final Exception e) {
            return 2;
        }

        int currentBalance = Wallet.getBalance(wallet.signature);
        if (amount > currentBalance) {
            return 2;
        }

        Mempool.writeRecord(record);
        return 0;
    }

    private static String toRecordFormat(String sender, String receiver, int amount, String date) {
        return sender + " transferred " + amount + " to " + receiver + " on " + date;
    }

    public static String[] parseRecord(String transactionRecord){
        String[] parts = transactionRecord.split(" ", 8);
        String[] ans = new String[3];
        ans[0] = parts[0];
        ans[1] = parts[4];
        ans[2] = parts[2];
        return ans;
    }
}

/**
 * All required fields from writeup
 * I/O to write and read state from .txt
 */

class Block {

    public static String[] getTransactionRecords(File block) {
        ArrayList<String> records = new ArrayList<String>();
        try{
            Scanner sin = new Scanner(block);
            sin.nextLine();
            sin.nextLine();
            while(sin.hasNextLine()){
                String line = sin.nextLine();
                if (line.equals(""))
                    break;
                records.add(line);
            }
            sin.close();
        } catch (final Exception e) {
            System.out.println("Error while trying to get all records from block");
            e.printStackTrace();
        }
        return records.toArray(new String[records.size()]);
    }

    public static void mine(int leadingZeros){
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < leadingZeros; i++){
            sb.append("0");
        }
        final String prefix = sb.toString();

        int i = 0;
        while (true) {
            File walletFile = new File("block_" + i + ".txt");
            if(!walletFile.exists())
                break;
            i++;
        }
        final String prevBlock = "block_" + (i-1) + ".txt";
        final String newBlock = "block_" + i + ".txt";
        try {
            final String prevHash = Wallet.getHashOfFile(prevBlock);
            final String transactionRecords = Mempool.getTransactionRecords(true);
            int nonce = -1;
            String proposedBlock = "";
            String proposedBlockHash = "";
            do {
                nonce ++;
                proposedBlock = prevHash + "\n\n" + transactionRecords + "\n" + "nonce: " + nonce + "\n";
                proposedBlockHash = Wallet.getHashOfString(proposedBlock);
            } while (! proposedBlockHash.substring(0, leadingZeros).equals(prefix));

            File f = new File(newBlock);
            f.createNewFile();
            FileWriter myWriter = new FileWriter(f, true);
            myWriter.write(proposedBlock);
            myWriter.close();
            System.out.println("Mempool transactions moved to " + newBlock + " and mined with difficulty " + leadingZeros + " and nonce " + nonce);
        } catch (Exception e) {
            System.out.println("Error has occurred");
        }
    }
}


/**
 * I/O - read from and write to the mempool file
 * public function to accept a new Transaction Statement
 * function to expose mempool state to createBlock()
 */
class Mempool {
    private static final String fileName = "mempool.txt";

    public static void writeRecord(String transactionRecord) {
        try {
            File file = new File(fileName);
            if(!file.exists()) {
                file.createNewFile();
            }
            FileWriter myWriter = new FileWriter(fileName, true);
            myWriter.write(transactionRecord + "\n");
            myWriter.close();
        } catch (IOException e) {
            System.out.println("An error occurred while creating the genesis block...");
            e.printStackTrace();
        }
    }
    //return all file lines. delete mempool
    public static String getTransactionRecords(boolean clear) {
        StringBuilder lines = new StringBuilder();
        File mempool = new File(fileName);
        if (! mempool.exists())
            return "";
        try {
            Scanner sin = new Scanner(mempool);
            while(sin.hasNextLine()){
                lines.append(sin.nextLine() + "\n");
            }
            sin.close();
            if (clear)
                mempool.delete();
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        return lines.toString();
    }
}
/**
 * Main class. contains code to interpret command line input and run all other relevant code
 */
class CMoney {
    public static final String coinName = "TempleToken";
    public static final String moneyPrinter = "Adam";

    public static void main(String[] args) {
        String command = args[0];
        if (command.equals("name")) {
            /**
             * Print the name of the cryptocurrency (name). This is the name from the Overview section, above. 
             * There are no additional command line parameters to this function.
             */
            System.out.println(coinName);
        } else if (command.equals("genesis")) {
            /**
             * Create the genesis block (genesis): this is the initial block in the block chain, and the block should always be the same. 
             * Come up with a fun quote! It should always be written to a block_0.txt file. There are no additional command line parameters to this function.
             */
            final String blockName = "block_0.txt";
            try {
                File genesisBlock = new File(blockName);
                if(!genesisBlock.createNewFile()) {
                    System.out.println("Cannot recreate the genesis block");
                    System.exit(1);
                }
                FileWriter myWriter = new FileWriter(blockName);
                myWriter.write(
                    "The only things guaranteed in this life are death and taxes.\n"
                    +"\n"
                    +"well... at least that was the case before " + coinName + "\n"
                    +"Viva La Revolution - Death to Taxes!\n"
                    +"\n"
                    +"P.S. for my name inspiration check out: https://www.youtube.com/watch?v=h7gf5M04hdg\n");
                myWriter.close();
            } catch (IOException e) {
                System.out.println("An error occurred while creating the genesis block...");
                System.exit(0);
            }

            System.out.println("Genesis block created in '" + blockName + "'");

        } else if (command.equals("generate")) {
            /**
             * Generate a wallet (generate): this will create RSA public/private key set (1024 bit keys is appropriate for this assignment). 
             * The resulting wallet file MUST BE TEXT – you will have to convert any binary data to text to save it (and convert it in the other direction when loading). 
             * You can see the provided helper functions, above, to help with this. The file name to save the wallet to will be provided as an additional command line parameter.
             * 
             * Create a wallet file (Public Private Key Pair + Signature)
             */

            final String walletName = args[1];
            if (Wallet.walletExists(walletName)) {
                System.out.println("Can not create a new wallet with the same name!");
                System.exit(0);
            }
            
            Wallet newWallet = new Wallet(walletName);
            System.out.println("New wallet generated in '" + newWallet.name + "' with signature " + newWallet.signature);
        } else if (command.equals("address")) {
            /**
             * Get wallet tag (address): this will print out the tag of the public key for a given wallet, which is likely the hash of the public key. 
             * Note that it only prints out that tag (no other output). When the other commands talk about naming a wallet, this is what it actually means. 
             * You are welcome to use the first 16 characters of the hash of the public key for this assignment; you don’t need to use the entire hash. 
             * The file name of the wallet file will be provided as an additional command line parameter.
             */
            final String walletName = args[1];
            if (!Wallet.walletExists(walletName)) {
                System.out.println("Can not obtain the address of a non-existing wallet!");
                System.exit(0);
            }
            Wallet wallet = new Wallet(walletName);
            System.out.println(wallet.signature);

        } else if (command.equals("fund")) {
            /**
             * Fund wallets (fund): this allows us to add as much money as we want to a wallet. 
             * While this is obviously not practical in the real world, it will allow us to test your program. 
             * (Although there still needs to be a way to fund wallets in the real world also.) 
             * Create a special case ID (‘bigfoot’, ‘daddy_warbucks’, ‘lotto’, or whatever) that your program knows to use as the source for a fund request, 
             * and also knows not to verify when handling verification, below. This means that ‘bigfoot’ (or whatever) will appear alongside the hash of the public keys as the source of funds. 
             * This function will be provided with three additional command line parameters: 
             *  the destination wallet address, the amount to transfer, and the file name to save the transaction statement to. 
             *  All funded amounts are integers; we are not using floating-point numbers in this assignment at all.
             */

            final String receiverWalletSig = args[1];
            final int amount = Integer.parseInt(args[2]);
            final String fileName = args[3];
            // Source: https://stackoverflow.com/questions/5175728/how-to-get-the-current-date-time-in-java
            String date = new SimpleDateFormat("MM/dd/yyyy HH:mm:ss").format(Calendar.getInstance().getTime());

            if(Transaction.createTransaction(receiverWalletSig, amount, date, fileName))
                System.out.println("Funded wallet " + receiverWalletSig + " with " + Integer.toString(amount) + " " + CMoney.coinName + " on " + date);
            else
                System.out.println("Error: the transaction record could not be created");
        } else if (command.equals("transfer")) {
            /**
             * Transfer funds (transfer): this is how we pay with our cryptocurrency. 
             * It will be provided with four additional command line parameters: the source wallet file name (not the address!), 
             * the destination wallet address (not the file name!), the amount to transfer, and the file name to save the transaction statement to. 
             * Any reasonable format for the transaction statement is fine for this, as long as the transaction statement is text and thus readable by a human. 
             * Recall that it must have five pieces of information, described above in the “Transaction statement versus transaction record” section. 
             * Note that this command does NOT add anything to the mempool. All transferred amounts are integers; we are not using floating-point numbers in this assignment at all.
             */

            final String senderWalletName = args[1];
            final String receiverWalletSig = args[2];
            final int amount = Integer.parseInt(args[3]);
            final String fileName = args[4];
            // Source: https://stackoverflow.com/questions/5175728/how-to-get-the-current-date-time-in-java
            String date = new SimpleDateFormat("MM/dd/yyyy HH:mm:ss").format(Calendar.getInstance().getTime());

            if (!Wallet.walletExists(senderWalletName)) {
                System.out.println("The wallet " + senderWalletName + " does not exist");
                System.exit(0);
            }

            final Wallet senderWallet = new Wallet(senderWalletName);

            if(Transaction.createTransaction(senderWallet, receiverWalletSig, amount, date, fileName))
                System.out.println("Transferred " + Integer.toString(amount) + " from " + senderWalletName + " to " + receiverWalletSig + " and the statement to " + fileName + " on " + date);
            else
                System.out.println("Error: the transaction record could not be created");
        } else if (command.equals("balance")) {
            /**
             * Check a balance (balance): based on the transactions in the block chain AND ALSO in the mempool, 
             * compute the balance for the provided wallet. This does not look at transaction statements, only the transaction records in the blocks and the mempool. 
             * The wallet address to compute the balance for is provided as an additional command line parameter.
             * NOTE: this should ONLY print the balance as a number, as an integer, and nothing else!
             */

            final String walletAddress = args[1];

            System.out.println(Wallet.getBalance(walletAddress));

        } else if (command.equals("verify")) {
            /**
             * Verify a transaction (verify): verify that a given transaction statement is valid, which will require checking the signature and the availability of funds. 
             * Once verified, it should be added to the mempool as a transaction record. This is the only way that items are added to the mempool. 
             * The wallet file name (whichever wallet created the transaction) and the transaction statement being verified are the additional command line parameters.
             */

            final String walletName = args[1];
            final String transactionFile = args[2];

            Wallet wallet = new Wallet(walletName);
            short flag = Transaction.verify(wallet, transactionFile);
            if(flag == 0){
                System.out.println("The transaction in file '" + transactionFile + "' with wallet '" + walletName + "' is valid, and was written to the mempool");
            } else if (flag == 1) {
                System.out.println("Any funding request (i.e., from "+ CMoney.moneyPrinter + ") is considered valid; written to the mempool");
            }
            else {
                System.out.println("Error: the transaction record is not verified");
            }

        } else if (command.equals("mine")) {
            /**
             * Create, mine, and sign block (mine): this will form another block in the blockchain. 
             * The mempool will be emptied of transaction records, as they will all go into the current block being computed. 
             * A nonce will have to be computed to ensure the hash is below a given value. Recall that the first line in any block is the SHA-256 of the last block file. 
             * The difficulty for the mining will be the additional parameter to this command. 
             * For simplicity, the difficulty will be the number of leading zeros to have in the hash value – so a value of 3 would imply that the hash must start with three leading zeros. 
             * We will be using very small difficulties here, so a brute-force method for finding the nonce is sufficient. The nonce must be a single unsigned 32 bit (or 64 bit) integer.
             */

            final int leadingZeros = Integer.parseInt(args[1]);
            Block.mine(leadingZeros);
        } else if (command.equals("validate")) {
            /**
             * Validate the blockchain (validate): this should go through the entire block chain, validating each one. 
             * This means that starting with block 1 (the block after the genesis block), ensure that the hash listed in that file, which is the hash for the previous block file, is correct. 
             * There are no additional command-line parameters for this function.
             */
             boolean flag = true;
             int blockIndex = 0;
             while(true) {
                blockIndex++;
                String prevBlock = "block_" + (blockIndex - 1) + ".txt";
                String block = "block_" + blockIndex + ".txt";

                try {
                    File blockFile = new File(block);
                    if(!blockFile.exists())
                        break;

                    Scanner scanner = new Scanner(blockFile);
                    String prevHashTest = scanner.nextLine();
                    scanner.close();

                    String prevHash = Wallet.getHashOfFile(prevBlock);

                    if (! prevHashTest.equals(prevHash)) {
                        flag = false;
                        break;
                    }
                } catch (final Exception e) {
                    e.printStackTrace();
                    flag = false;
                    break;
                }
             }
            if (flag)
                System.out.println("True");
            else
                System.out.println("False");
            
        } else {
            System.out.println("Unknown Command: " + command);
        }
    }
}