//
//  main.cpp
//  encryptUtil
//
//  Created by Adarsh Joseph on 12/7/18.
//  Copyright © 2018 Adarsh Joseph. All rights reserved.
//

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
#include "inputs.hpp"    // input class
using namespace std;

typedef unsigned int uint;
bool IsFileEmpty(std::ifstream &check);
long FileSize(ifstream &input_file);
void PSAB(string s);
class Input;
void printBoolVec(vector<bool> input);
class Segment;
void EmptyThreadPool(list<future<bool>> &thread_status,
                     mutex &mx_thread_status);

atomic<bool> INPUT_DONE{false};

bool IsFileEmpty(std::ifstream &check_file) {
    return check_file.peek() == std::ifstream::traits_type::eof();
}

long FileSize(ifstream &input_file) {
    streampos begin, end;
    begin = input_file.tellg();
    input_file.seekg(0, ios::end);
    end = input_file.tellg();
    return end - begin;
}

void PSAB(string s) {
    cout << s << " ";
    for (char a : s) {
        cout << bitset<8>(a) << " ";
    }
    cout << endl;
}

void CSAB(char a) {
    cout << a << " ";
    cout << bitset<8>(a) << " ";
    cout << endl;
}

void printBoolVec(vector<bool> input) {
    for (bool i : input) {
        cout << i;
    }
    cout << endl;
}


class Segment {
public:
    long number;
    string rotated_key = "";
    string input_segment;
    string result;
    static long serial;
    static mutex mx_cout;
    
    Segment(long n, string assign) {
        this->number = n;
        this->input_segment = assign;
    }
    
    bool Process() {
        this->rotated_key = Input::RotatedKeyArray(this->number);
        this->result = XOR(this->input_segment, this->rotated_key);
        Output();
        return true;
    }
    
    static string XOR(string a, string b) {
        //    char* segment = new char[a.size()];
        string segment = "";
        for (uint i = 0; i < a.size(); i++) {
            //        cout<<"a: "<<bitset<8>(a[i])<<endl;
            //        cout<<"b: "<<bitset<8>(b[i])<<endl;
            //            CSAB(a[i]);
            //            CSAB(b[i]);
            segment += (char)(a[i] ^ b[i]);
            //        cout<<"s: "<<bitset<8>(segment[i])<<endl;
        }
        //        PSAB(segment);
        return segment;
    }
    
    void Output() {
        while (true) {
            if (this->number == serial) {
                //                cout<<endl<<this->number<<endl;
                mx_cout.lock();
                serial++;
                cout << this->result;
                mx_cout.unlock();
                break;
            } else {
                this_thread::sleep_for(chrono::microseconds(1));
            }
        }
    }
};
long Segment::serial = 0;
mutex Segment::mx_cout;

// void EmptyThreadPool(list<future<bool>> &thread_status, mutex
// &mx_thread_status){
////    while(INPUT_DONE){
////        cout<<"here";
////        this_thread::sleep_for(chrono::milliseconds(5));        //change
/// back to lower number /    }
//    while(!INPUT_DONE){
//        mx_thread_status.lock();
//        auto it= thread_status.begin();
//        auto sz=thread_status.size();
//        mx_thread_status.unlock();
//        while(sz!=0){
//            if(it->wait_for(0ms)==future_status::ready){
//                mx_thread_status.lock();
//                thread_status.erase(it);
//                it=thread_status.begin();
//                mx_thread_status.unlock();
//                break;
//            }
//            else{it++;}
//        }
//    }
//}



void ReadInput(list<shared_ptr<Segment>> &input_segments,
               mutex &mx_input_segments, int key_size_bytes) {
    static long input_order{0};
    char input_char;
    while (cin.get(input_char)) {
        //        CSAB(input_char);
        string thread_assignment;
        thread_assignment.push_back(input_char);
        for (uint i = 1; i < key_size_bytes && cin.get(input_char); ++i) {
            thread_assignment += input_char;
        }
        shared_ptr<Segment> segment =
        make_shared<Segment>(input_order, thread_assignment);
        mx_input_segments.lock();
        input_segments.push_back(segment);
        mx_input_segments.unlock();
        
        //        mx_thread_status.lock();
        //        cout<<thread_status.size()<<endl;
        //        while(thread_status.size() >= inputs->thread_pool_size){
        //            mx_thread_status.unlock();
        //            this_thread::sleep_for(chrono::milliseconds(1));
        //        }
        //        thread_status.push_back(async(&Segment::Process, segment,
        //        ref(mx_cout))); mx_thread_status.unlock();
        input_order++;
    }
    INPUT_DONE = true;
}

void PerformanceTimer(bool start) {
    //    using namespace std::chrono_literals;   // Starting timer here to
    //    measure Performance
    static std::chrono::high_resolution_clock::time_point start_time;
    static std::chrono::high_resolution_clock::time_point end_time;
    if (start) {
        start_time = std::chrono::high_resolution_clock::now();
    } else {
        end_time = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>(
                                                                                end_time - start_time)
        .count();
        double duration_secs = duration / 1000000;
        cout.precision(std::numeric_limits<double>::max_digits10);
        cerr << "Total runtime: " << duration_secs << "s" << endl;
    }
}

int main(int argc, char **argv) {
    //    Input* input = new Input(argc, argv);
    PerformanceTimer(true);
    shared_ptr<Input> inputs = make_shared<Input>(argc, argv);
    //    Input* inputs = shr_ptr.get();
    if (!inputs->status) {
        return inputs->error_code;
    }
    list<shared_ptr<Segment>> input_segments;
    static mutex mx_input_segments;
    
    static mutex mx_cout;
    
    async(launch::async, ReadInput, ref(input_segments), ref(mx_input_segments),
          inputs->key_size_bytes);
    list<future<bool>> thread_status;
    //    static mutex mx_thread_status;
    while (!INPUT_DONE) {
        while (!INPUT_DONE && (thread_status.size() < inputs->thread_pool_size)) {
            mx_input_segments.lock();
            if (input_segments.size() == 0) {
                mx_input_segments.unlock();
                break;
            }
            shared_ptr<Segment> this_segment = input_segments.front();
            input_segments.pop_front();
            mx_input_segments.unlock();
            thread_status.push_back(
                                    async(launch::async, &Segment::Process, this_segment));
        }
        while (!thread_status.empty() &&
               (thread_status.front().wait_for(0ms) == std::future_status::ready)) {
            thread_status.pop_front();
        }
        //        this_thread::sleep_for(chrono::milliseconds(5));
    }
    while (input_segments.size() != 0) {
        while (input_segments.size() != 0 &&
               (thread_status.size() < inputs->thread_pool_size)) {
            shared_ptr<Segment> this_segment = input_segments.front();
            input_segments.pop_front();
            thread_status.push_back(
                                    async(launch::async, &Segment::Process, this_segment));
        }
        while (!thread_status.empty() &&
               (thread_status.front().wait_for(0ms) == std::future_status::ready)) {
            thread_status.pop_front();
        }
    }
    PerformanceTimer(false);
    //    bitset<>
    //
    //    cin>> hex >> x;
    //    cout<<"x:"<<x<<endl;
    //    x>>=10;
    //    cout<<"x shifted:"<<x<<endl;
    //    cout<<"hex x"<< hex <<x<<endl;
    return 0;
}

