import json
import sys
import hashlib
from hashlib import sha256
from binascii import unhexlify, hexlify

""""
Utility Functions to Read Types in Bitcoin
"""
# byte[] |-> 4-byte little endian integer
def get_4byte_int(b_array):
    # return struct.unpack("<I", b_array)[0]
    return int.from_bytes(b_array, "little")

# byte[] |-> 8-byte little endian integer
def get_8byte_int(b_array):
    # return struct.unpack("<Q", b_array)[0]
    return int.from_bytes(b_array, "little")

# byte[] |-> (Integer, bytes_read)
def get_compact_sized_int(b_array):
    byte_arr = []
    for i in range(9):
        byte_arr.append(b_array[i:(i+1)])

    first_byte = int.from_bytes(byte_arr[0], "big", signed=False)

    if first_byte < 0xfd:
        return first_byte, 1
    elif first_byte == 0xfd:
        # use next 2 bytes
        bytes = byte_arr[1] + byte_arr[2]
        return int.from_bytes(bytes, "little", signed=False), 3
    elif first_byte == 0xfe:
        # use next 4 bytes
        bytes = byte_arr[1] + byte_arr[2] + byte_arr[3] + byte_arr[4]
        return int.from_bytes(bytes, "little", signed=False), 5
    elif first_byte == 0xff:
        # use next 8 bytes
        bytes = byte_arr[1] + byte_arr[2] + byte_arr[3] + byte_arr[4] + byte_arr[5] + byte_arr[6] + byte_arr[7] + byte_arr[8]
        return int.from_bytes(bytes, "little", signed=False), 9
    return -1

# byte[] |-> 32-byte little endian string
def get_32byte_hash(b_array):

    byte_arr = []
    for i in range(32):
        byte_arr.append(b_array[i:(i+1)])

    byte_arr.reverse()
    
    hash = ""
    for byte in byte_arr:
        byte_as_int = int.from_bytes(byte, "little")
        b = "{0:#0{1}x}".format(byte_as_int,4)
        hash += b[2::]
    
    return hash

def get_string(b_array, num_bytes):
    byte_arr = []
    for i in range(num_bytes):
        byte_arr.append(b_array[i:(i+1)])

    str = ""
    for byte in byte_arr:
        byte_as_int = int.from_bytes(byte, "big")
        b = "{0:#0{1}x}".format(byte_as_int,4)
        str += b[2::]

    return str

def format_num_as_hex(num):
    return hex(num)[2::]

def compute_merkle_root_hash(transactions):
    # initial hashes
    hash_list = []
    for i in range(len(transactions)):
        hash_list.append(double_hash(transactions[i]))

    #base cases
    while len(hash_list) != 1:

        next_list = []
        i = 0
        while i < len(hash_list) - 1:
            cat = hash_list[i] + hash_list[i+1]
            next_list.append(double_hash(cat))
            i += 2

        if len(hash_list) % 2 == 1:
            cat = hash_list[-1] + hash_list[-1]
            next_list.append(double_hash(cat))

        hash_list = next_list
        next_list = []

    byte_list = list(hash_list[0])
    byte_list.reverse()
    return bytes(byte_list).hex()

# byte[] |-> byte[32] hash
def double_hash(byte_arr):
    hash_1 = sha256(byte_arr).digest()
    return sha256(hash_1).digest()

"""
Main loop to read, parse and verify the blockchain
"""
if __name__ == '__main__':

    filename_in = sys.argv[1]
    filename_out = filename_in + ".json"

    block_file = open(filename_in, mode='rb')
    
    def read_compact_size():
        integer, bytes_read = get_compact_sized_int(block_file.read(9))
        block_file.seek(bytes_read - 9, 1)
        return integer
    
    def read_4byte_int():
        return get_4byte_int(block_file.read(4))

    def read_8byte_int():
        return get_8byte_int(block_file.read(8))
    
    def read_32byte_hash():
        return get_32byte_hash(block_file.read(32))

    def read_var_length_str(num_bytes):
        return get_string(block_file.read(num_bytes), num_bytes)

    ledger = {}
    blocks = []
    block_count = 0
    LAST_BYTE = block_file.seek(0, 2)
    block_file.seek(0, 0)

    err_flags = [False, False, False, False, False, False]
    prev_header_hash_chk = None
    prev_timestamp_chk = None
    while block_file.tell() < (LAST_BYTE - 8):
        block_count += 1
        block = {}

        # Read Preamble
        pre_preamble = block_file.tell()
        magic_number = read_4byte_int()
        size = read_4byte_int()
        if magic_number != 0xd9b4bef9:
            err_flags[0] = True
            break
        post_preamble = block_file.tell()

        # Read Header
        pre_header = block_file.tell()
        header_byte_str = block_file.read(80) #? Used for hash of header
        block_file.seek(pre_header, 0)
        header_version = read_4byte_int()
        prev_header_hash = get_32byte_hash(block_file.read(32))
        merkle_root_hash = get_32byte_hash(block_file.read(32))
        unix_time = read_4byte_int()
        nBits = read_4byte_int()
        nonce = read_4byte_int()
        post_header = block_file.tell()

        #header version error check
        if header_version != 1:
            err_flags[1] = True
            break

        #prev_header_hash error check
        if prev_header_hash_chk and prev_header_hash_chk != prev_header_hash:
            err_flags[2] = True
            break

        # timestamp error check
        if (block_count > 1
            and (((unix_time - prev_timestamp_chk) // 3600) < -2)):
            err_flags[3] = True
            break
        prev_timestamp_chk = unix_time

        # Read transactions
        num_transactions = read_compact_size()
        transactions = []
        bin_transactions = []
        for _ in range(num_transactions):
            trans_start_byte = block_file.tell()

            transaction = {} 
            trans_version = read_4byte_int()

            # transaction version error check
            if trans_version != 1:
                err_flags[4] = True
                break

            txn_in_count = read_compact_size()
            txn_inputs = []
            for _ in range(txn_in_count):
                txn_input = {}

                utxo = get_32byte_hash(block_file.read(32))
                utxo_index = read_4byte_int()
                script_size = read_compact_size()
                input_script = read_var_length_str(script_size)
                sequence = read_4byte_int()

                # write transaction inputs object
                txn_input["utxo_hash"] = utxo
                txn_input["index"] = utxo_index
                txn_input["input_script_size"] = script_size
                txn_input["input_script_bytes"] = input_script
                txn_input["sequence"] = sequence

                txn_inputs.append(txn_input)

            txn_out_count = read_compact_size()
            txn_outputs = []
            for _ in range(txn_out_count):
                txn_output = {}

                satoshis = read_8byte_int()
                script_size = read_compact_size()
                output_script = read_var_length_str(script_size)

                # write transaction outputs object
                txn_output["satoshis"] = satoshis
                txn_output["output_script_size"] = script_size
                txn_output["output_script_bytes"] = output_script

                txn_outputs.append(txn_output)
            
            lock_time = read_4byte_int()

            trans_end_byte = block_file.tell()

            #record transaction bytes for merkle hash tree
            block_file.seek(trans_start_byte, 0)
            bin_transactions.append(block_file.read(trans_end_byte - trans_start_byte))


            # write transaction object
            transaction["version"] = trans_version
            transaction["txn_in_count"] = txn_in_count
            transaction["txn_inputs"] = txn_inputs
            transaction["txn_out_count"] = txn_out_count
            transaction["txn_outputs"] = txn_outputs
            transaction["lock_time"] = lock_time

            transactions.append(transaction)

        # identified transaction version error. blockchain invalid
        if err_flags[4] == True:
            break

        merkle_root_hash_chk = compute_merkle_root_hash(bin_transactions)
        if merkle_root_hash != merkle_root_hash_chk:
            err_flags[5] = True
            break
        

        # write block object
        block["height"] = block_count - 1
        block["block_bytes"] = size
        block["block_start_seek"] = pre_header
        block["block_post_seek"] = block_file.tell()
        block["version"] = header_version
        block["previous_hash"] = prev_header_hash
        block["merkle_hash"] = merkle_root_hash
        block["timestamp"] = unix_time
        block["nbits"] = format_num_as_hex(nBits)
        block["nonce"] = nonce
        block["txn_count"] = num_transactions
        block["transactions"] = transactions

        # calculate current header hash for e3 detection
        header_hex = ("01000000" + 
            prev_header_hash + 
            merkle_root_hash + 
            format_num_as_hex(unix_time) + 
            format_num_as_hex(nBits) + 
            format_num_as_hex(nonce)
        )

        # COmpute current hash for validation in next block
        hash = hashlib.sha256(hashlib.sha256(header_byte_str).digest()).digest()
        prev_header_hash_chk = hexlify(hash[::-1]).decode("utf-8")

        blocks.append(block)
    
    ledger["blocks"] = blocks
    ledger["height"] = block_count

    if err_flags[0] == True:
        print("error 1 block", block_count - 1)
    elif err_flags[1] == True:
        print("error 2 block", block_count - 1)
    elif err_flags[2] == True:
        print("error 3 block", block_count - 1)
    elif err_flags[3] == True:
        print("error 4 block", block_count - 1)
    elif err_flags[4] == True:
        print("error 5 block", block_count - 1)
    elif err_flags[5] == True:
        print("error 6 block", block_count - 1)
    else:
        with open(filename_out, 'w', encoding='utf-8') as f:
            json.dump(ledger, f, ensure_ascii=False, indent=4)
        print(f"no errors {block_count} blocks")
