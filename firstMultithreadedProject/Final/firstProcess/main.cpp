//
//  main.cpp
//  firstProcess
//
//  Created by Adarsh Joseph on 12/2/18.
//  Copyright © 2018 Adarsh Joseph. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <limits>
#include <future>
#include <chrono>
#include <deque>
#include <unordered_set>
#include <unistd.h>
using namespace std;

// Intializing global counters
static atomic<int> TOTAL_THREAD_COUNT={1};
static atomic<bool> INPUT_DONE={false};
static atomic<int> VALID_COUNT={0};
static atomic<int> INVALID_COUNT={0};
static int WASTED_THREAD_COUNT=0;

class Line{
    /* Class Definition for each line of input. Each object will have a mutex and a status variable, the string of that line and the line number
        3 Member functions to evaluate each Row, Column and Square of the input Sudoku string    */
private:
    bool status=true;       // This variable holds the status of the line and is a protected by a mutex as any of the member functions can modify this
    mutex mstatus;          // mutex that protects the above variable
    
public:
    const int num;
    const string contents;
    
    Line(int no, string ln) : num(no), contents(ln){}
    
    bool getLineStatus(){
        return this->status;
    }
    
    // Method that marks the class object status as false, if called by any of the member functions
    void markInvalid(){
        this->mstatus.lock();
        this->status=false;
        this->mstatus.unlock();
    }
    
    void EvalRow(){
        TOTAL_THREAD_COUNT++;
        if(this->contents.size()!=81){markInvalid();return;}
        for(int i=0; i<81; i+=9){
            unordered_set<char> rowSet;
            for(int j=0; j<9; j++){
                int k =i+j;
                if(this->contents[k]!='.'){
                    if(rowSet.count(this->contents[k])==0){
                        rowSet.insert(this->contents[k]);
                    }
                    else{
                        markInvalid();return;
                    }
                }
            }
        }
    }
    
    void EvalCol(){
        TOTAL_THREAD_COUNT++;
        for(int i=0; i<9; i++){
            unordered_set<char> colSet;
            for(int j=0; j<81; j+=9){
                int k=i+j;
                if(this->contents[k]!='.'){
                    if(colSet.count(this->contents[k])==0){
                        colSet.insert(this->contents[k]);
                    }
                    else{
                        markInvalid();return;
                    }
                }
            }
        }
    }
    
    void EvalSquare(){
        TOTAL_THREAD_COUNT++;
        int arr[9] = {0,3,6,27,30,33,54,57,60};
        for(int i=0; i<9; i++){
            int val = arr[i];
            unordered_set<char> sqSet;
            for(int j=0; j<27; j+=9){
                for(int k=0; k<3; k++){
                    int m= val+j+k;
                    if(this->contents[m]!='.'){
                        if(sqSet.count(this->contents[m])==0){
                            sqSet.insert(this->contents[m]);
                        }
                        else{
                            markInvalid();return;
                        }
                    }
                }
            }
        }
    }
};



void ReadFile(string file_name, mutex &line_mx, list<shared_ptr<Line>> &input_lines){
    TOTAL_THREAD_COUNT++;
    ifstream slist(file_name);
    string each_line;
    static int line_number=0;
    /* Reading each line of the input file and adding it to the shared_ptr list of Line objects */
    while(getline(slist, each_line)){
        if(each_line.size()<=1){continue;}
        line_number++;
        istringstream iss(each_line);
        shared_ptr<Line> shr_line_ptr = make_shared<Line>(line_number, each_line);
        line_mx.lock();
        input_lines.push_back(shr_line_ptr);
        line_mx.unlock();
    }
//    cout<<"read done"<<endl;
}

bool ProcessLine(mutex &line_mx, list<shared_ptr<Line>> &input_lines, mutex &output_mx, ofstream &outFile){
    /* Function that processes each line from the shared_ptr list of Line objects */
    TOTAL_THREAD_COUNT++;
    shared_ptr<Line> this_line=nullptr;
    // Critical section for accesing the shared data structure and removing the line to be processed
    if(!INPUT_DONE){
        line_mx.lock();
        this_line = input_lines.front();
        input_lines.pop_front();
        if(input_lines.empty()){INPUT_DONE=true;}
        line_mx.unlock();
    }
    if(!this_line){return false;}
    
    // Spawning 3 child threads for every worker thread to evaluate Row, Column and Square
    thread tr(&Line::EvalRow, this_line);
    thread tc(&Line::EvalCol, this_line);
    thread ts(&Line::EvalSquare, this_line);
    tr.join();
    tc.join();
    ts.join();
    // Waiting for the above threads to finish before recording status
    
    //  Recording status
    bool state = this_line->getLineStatus();
    state ? VALID_COUNT++ : INVALID_COUNT++;
    string status = state? "valid" : "invalid";
    string out= to_string(this_line->num)+" : "+status+"\n";
    
    // Recording the status in the output file
    output_mx.lock();
    outFile<<out;
    output_mx.unlock();
    return true;
}


//  Helper Methods
std::string GetWorkingPath()
{
    char temp[PATH_MAX];
    return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}

bool IsFileEmpty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

int main(int argc, const char * argv[]) {
    using namespace std::chrono_literals;   // Starting timer here to measure Performance
    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    
    //--------------- Input file check ---------------------
    string file_name;
    if(argc==1){
        cout<<"This tool requires the link to an input text file."<<endl;
        return 1;
    }
    else if(argc==2){
        file_name= argv[1];
        ifstream fileCheck(file_name);
        if(IsFileEmpty(fileCheck)){
            cout<<"Given input file is empty"<<endl;
            return 2;
        }
    }
    else{
        cout<<"This tool takes only one parameter"<<endl;
        return 3;
    }
    //------------------------------------------------------
    
    deque<future<bool>> thread_pool;    // Double ended Queue that maintains the status of all the active threads
    static const int max_pool_size = 10;   // Size of the above thread pool, this is the number of threads that it will actively keep
    
    list<shared_ptr<Line>> input_lines;  // List of shared pointers of the Line object, each object will represent one line of the input file (one puzzle)
    static mutex line_mx;               // mutex that protects the above list
    
    ofstream outFile;                  // Output file that each thread writes status into
    static mutex output_mx;            // mutex that protects the above file
    string loc = GetWorkingPath()+ "/output.txt";
    cout<<"Output filename: "<<loc<<endl;
    outFile.open(loc);
    
    // Triggering the read thread to begin with, which will continously read every line and add it to the shared data structure
    async(ReadFile, file_name, std::ref(line_mx), std::ref(input_lines));
   
    /* Loop that maintains the thread pool, the outer loop runs until all of the input data is done */
    while(!INPUT_DONE){
        /* Inner Loops run until the thread pool has reached it's max size, and then it checks the thread pool to remove threads that are done executing */
        while(!INPUT_DONE && thread_pool.size() < max_pool_size){
            thread_pool.push_back(std::async(std::launch::async, ProcessLine, std::ref(line_mx), std::ref(input_lines), std::ref(output_mx), std::ref(outFile)));
            continue;
        }
        while(!thread_pool.empty() && thread_pool.front().wait_for(0ms)==std::future_status::ready){
            if(!thread_pool.front().get()){WASTED_THREAD_COUNT++;}
            thread_pool.pop_front();
        }
    }
    
    /* Once input is done, we're processing any remaining threads that weren't done executing */
    while(!thread_pool.empty()){
        if(thread_pool.front().wait_for(1ms)==std::future_status::ready){
            if(!thread_pool.front().get()){WASTED_THREAD_COUNT++;}
            thread_pool.pop_front();
        }
    }
    
    //  Print all the stats
    cout<<"Total threads spawned: "<<TOTAL_THREAD_COUNT<<endl;
    cout<<"Thread pool size: "<<max_pool_size<<endl;
    cout<<"Threads wasted: "<<WASTED_THREAD_COUNT<<endl;
    cout<<"Total number of puzzles validated: "<<VALID_COUNT+INVALID_COUNT<<endl;
    cout<<"No. of valid sudoku puzzles: "<<VALID_COUNT<<endl;
    cout<<"No. of invalid sudoku puzzles: "<<INVALID_COUNT<<endl;
    
    //  Closing the output file and stopping timer that's measuring Performance
    outFile.close();
    
    std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::microseconds>( end_time - start_time ).count();
    double duration_secs = duration/1000000;
    cout.precision(std::numeric_limits<double>::max_digits10);
    cout<<"Total program duration: "<<duration_secs<<"s"<<endl;
    return 0;
}

