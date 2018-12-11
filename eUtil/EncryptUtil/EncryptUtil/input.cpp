//
//  input.cpp
//  encryptUtil
//
//  Created by Adarsh Joseph on 12/10/18.
//  Copyright © 2018 Adarsh Joseph. All rights reserved.
//

#include "input.hpp"
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
using namespace std;

long FileSize(ifstream &input_file) {
    streampos begin, end;
    begin = input_file.tellg();
    input_file.seekg(0, ios::end);
    end = input_file.tellg();
    return end - begin;
}

uint Input::key_size_bytes;
uint Input::key_size_bits;
vector<bool> Input::org_key_bits;
unordered_map<uint, string> Input::key_table;


Input::Input(int argc, char **argv) {
    /* parse input stream
     
     if key file valid, returns size of key file
     */
    //        Input input;
    bool key_flag = false;
    bool thread_flag = false;
    this->thread_pool_size = 1;
    int c;
    while ((c = getopt(argc, argv, "k:n:")) != -1) {
        switch (c) {
            case 'n':
                thread_flag = true;
                this->thread_pool_size = atoi(optarg);
                break;
            case 'k':
                key_flag = true;
                this->key_file_location = optarg;
            default:
                break;
        }
    }
    if (!key_flag) {
        cerr << "This tool requires a key file" << endl;
        this->status = false;
        this->error_code = 20;
        //            return input;
    } else if (!thread_flag) {
        cerr << "This tool requires the number of threads to use" << endl;
        this->status = false;
        this->error_code = 30;
        //            return input;
    } else {
        ifstream check_file(this->key_file_location, ios::binary);
        long size = FileSize(check_file);
        check_file.close();
        key_size_bytes = (uint)size;
        if (key_size_bytes == 0) {
            cerr << "Key file has size 0 bytes" << endl;
            this->status = false;
            this->error_code = 21;
            //                return input;
        } else if (key_size_bytes > this->key_size_limit) {
            cerr << "Key file size is too big >" << this->key_size_limit << " bytes"
            << endl;
            this->status = false;
            this->error_code = 29;
            //                return input;
        } else {
            ifstream key_file(this->key_file_location, ifstream::binary);
            this->status = true;
            this->error_code = 0;
            char *buffer = new char[key_size_bytes];
            key_file.read(buffer, key_size_bytes);
            if (!key_file) {
                cerr << "Error: only " << key_file.gcount() << " could be read";
            }
            key_size_bits = key_size_bytes * 8;
            for (uint i = 0; i < key_size_bytes; i++) {
                char byte = buffer[i];
                //                cout<<"byte: "<<byte<<"  ";
                for (uint j = 0; j < 8; j++) {
                    bool bit = (byte & 128) > 0 ? true : false;
                    org_key_bits.push_back(bit);
                    //                        uint k= (8*i)+j;
                    //                    cout<<BR(input.org_key_bits[k]);
                    byte <<= 1;
                }
                //                cout<<endl;
            }
            delete[] buffer;
            //                return input;
        }
    }
    //-----------------------------------------------------
}
