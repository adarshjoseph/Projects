//
//  input.hpp
//  encryptUtil
//
//  Created by Adarsh Joseph on 12/10/18.
//  Copyright © 2018 Adarsh Joseph. All rights reserved.
//

#ifndef input_hpp
#define input_hpp

#include <stdio.h>
#include <algorithm> //rotate
#include <atomic>    //atomic variables
#include <bitset>    //bitsets,
#include <deque>     //double-ended queues
#include <fstream>   //ifstream, filesize
#include <future>    // futures for threads
#include <iostream>
#include <list>          // lists
#include <mutex>         // mutexes
#include <string>        // strings,
#include <thread>        //threads
#include <unistd.h>      //getopt
#include <unordered_map> //hash-maps
#include <vector>        //vectors

typedef unsigned int uint;

using namespace std;

#endif /* input_hpp */

class Input {
public:
    bool status;
    int error_code;
    static uint key_size_bytes;
    static uint key_size_bits;
    static vector<bool> org_key_bits;
    static unordered_map<uint, string> key_table;
    string key_file_location;
    uint thread_pool_size;
    uint key_size_limit = 256000;
    
    Input(int , char**);
    
    
    static vector<bool> RotateKey(uint by) {
        vector<bool> rotated_key = org_key_bits;
        rotate(rotated_key.begin(), rotated_key.begin() + by, rotated_key.end());
        return rotated_key;
    }
    
    static string RotatedKeyArray(long By) {
        if (By > key_size_bits) {
            By %= key_size_bits;
        }
        uint by = (uint)By;
        if (key_table.find(by) != key_table.end()) {
            return key_table[by];
        }
        string rotated_key = "";
        vector<bool> rotated = RotateKey(by);
        for (uint i = 0; i < key_size_bytes; i++) {
            //            char byt_char[8];
            bitset<8> byte;
            for (uint j = 0; j < 8; j++) {
                uint k = i * 8 + j;
                if (rotated[k]) {
                    byte.set(7 - j, 1);
                }
            }
            //            string byte_str=byte.to_string();
            char c = char(byte.to_ulong());
            //            copy(byte_str.begin(), byte_str.end(), byt_char);
            rotated_key += c;
        }
        key_table[by] = rotated_key;
        return rotated_key;
    }
};

