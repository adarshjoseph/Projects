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
#include <future>
#include <chrono>
#include <deque>
#include <unordered_set>
#include <unistd.h>
using namespace std;

//class Line{
//public:
//    const int num;
//    const string contents;
//    bool status=true;
//
//    Line(int no, string ln) : num(no), contents(ln){}
//
//    void ProcessLine(thread t1){
//        cout<<this->num<<":"<<this->contents<<endl;
//        t1.join();
//    }
//
//    void EvalRows(){
//
//    }
//
//};

class Line{
public:
    const int num;
    const string contents;
    bool status=true;
    mutex stat;
    
    Line(int no, string ln) : num(no), contents(ln){}
    
    void markInvalid(){
        this->stat.lock();
        this->status=false;
        this->stat.unlock();
    }
    
    void EvalRow(){
        for(int i=0; i<81; i+=9){
            unordered_set<char> rowSet;
            for(int j=0; j<9; j++){
                int k =i+j;
                if(this->contents[k]!='.'){
                    if(rowSet.count(this->contents[k])==0){
                        rowSet.insert(this->contents[k]);
                    }
                    else{
                        cout<<"HERE:"<<this->contents[k]<<endl;
                        markInvalid();break;
                    }
                }
            }
        }
    }
    
    void EvalCol(){
        for(int i=0; i<9; i++){
            unordered_set<char> colSet;
            for(int j=0; j<81; j+=9){
                int k=i+j;
                if(this->contents[k]!='.'){
                    if(colSet.count(this->contents[k])==0){
                        colSet.insert(this->contents[k]);
                    }
                    else{
                        cout<<"HERE:"<<this->contents[k]<<endl;
                        markInvalid();break;
                    }
                }
            }
        }
    }
    
    void EvalSquare(){
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
                            cout<<"HERE:"<<this->contents[k]<<endl;
                            markInvalid();break;
                        }
                    }
                }
            }
        }
    }
};

static bool input_done=false;

//struct Status{
//    const int num;
//    bool status=true;
//};

void ReadFile(string file_name, mutex &line_mx, list<shared_ptr<Line>> &input_lines){
    ifstream slist(file_name);
    string each_line;
    static int line_number=1;
    while(getline(slist, each_line)){
        istringstream iss(each_line);
        shared_ptr<Line> shr_line_ptr = make_shared<Line>(line_number, each_line);
//        Line line_obj(line_number, each_line);
//        Line* line_obj_ptr = &line_obj;
        line_mx.lock();
        input_lines.push_back(shr_line_ptr);
        line_mx.unlock();
        line_number++;
    }
    cout<<"read done"<<endl;
}

bool ProcessLine(mutex &line_mx, list<shared_ptr<Line>> &input_lines, mutex &output_mx, ofstream &outFile){
//    cout<<"process"<<endl;
    shared_ptr<Line> this_line=nullptr;
    line_mx.lock();
    if(!input_done){
        this_line = input_lines.front();
        input_lines.pop_front();
//        cout<<"size:"<<input_lines.size()<<endl;
        if(input_lines.empty()){input_done=true;}
    }
    line_mx.unlock();
    if(!this_line){return true;}
    thread tr(&Line::EvalRow, this_line);
    thread tc(&Line::EvalCol, this_line);
    thread ts(&Line::EvalSquare, this_line);
    tr.join();
    tc.join();
    ts.join();
    
    bool state = this_line->status;
    string status = state? "valid" : "invalid";
    string out= to_string(this_line->num)+" : "+status+"\n";
    
    output_mx.lock();
    outFile<<out;
    output_mx.unlock();
    return true;
}

std::string get_working_path()
{
    char temp[PATH_MAX];
    return ( getcwd(temp, sizeof(temp)) ? std::string( temp ) : std::string("") );
}


int main(int argc, const char * argv[]) {
    using namespace std::chrono_literals;
    // insert code here...
    string file_name;
    cout << "Get File link:";
//    cin>>file_name;
    file_name= "/Users/adarshjoseph/Downloads/sample.txt";
    static const int max_pool_size=5;
    
    deque<future<bool>> thread_pool;
    
    static mutex line_mx;
    list<shared_ptr<Line>> input_lines;
    
    static mutex output_mx;
    ofstream outFile;
    
    string loc = get_working_path()+ "/output.txt";
//    cout<<loc<<endl;
    outFile.open(loc);
    
    async(ReadFile, file_name, std::ref(line_mx), std::ref(input_lines));
//    readThread.join();
    cout<<"size:"<<input_lines.size()<<endl;
    while(!input_done){
        while(!input_done && thread_pool.size() < max_pool_size){
            cout<<"Adding thread"<<endl;
            thread_pool.push_back(std::async(std::launch::async, ProcessLine, std::ref(line_mx), std::ref(input_lines), std::ref(output_mx), std::ref(outFile)));
            continue;
        }
        while(!thread_pool.empty() && thread_pool.front().wait_for(0ms)==std::future_status::ready){
            cout<<thread_pool.front().get()<<"thread finished"<<endl;
            thread_pool.pop_front();
        }
//        for(auto it=thread_pool.begin(); it!=thread_pool.end();){
//            cout<<"here"<<endl;
//            if(it->wait_for(0ms)==std::future_status::ready){thread_pool.erase(it);break;}
//            else{it++; continue;}
//        }
    }
    cout<<"input done, thread pool size:"<<thread_pool.size()<<endl;
    while(!thread_pool.empty()){
        cout<<"finally"<<endl;
        if(thread_pool.front().wait_for(0ms)==std::future_status::ready){
            cout<<thread_pool.front().get()<<"thread finished"<<endl;
            thread_pool.pop_front();
        }
    }

    outFile.close();
    return 0;
}

