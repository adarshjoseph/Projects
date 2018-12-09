//
//  main.cpp
//  trialSegments
//
//  Created by Adarsh Joseph on 12/8/18.
//  Copyright © 2018 Adarsh Joseph. All rights reserved.
//
#include <bitset>
#include <iostream>
#include<string>
#include <fstream>
#include <vector>
using namespace std;

template <size_t N1, size_t N2 >
bitset <N1 + N2> concat( const bitset <N1> & b1, const bitset <N2> & b2 ) {
    string s1 = b1.to_string();
    string s2 = b2.to_string();
    return bitset <N1 + N2>( s1 + s2 );
}


int main(int argc, const char * argv[]) {
    vector<char> ar = {'a','b','c','d','e','f','g','h'};
    rotate(ar.begin(), ar.begin()+1, ar.end());
    for(char a: ar){
        cout<<a<<" ";
    }
    cout<<endl;
//    const char *filename = "/Users/adarshjoseph/Desktop/repos/Projects/encryptUtil/key_files/64b";
//    ifstream infile(filename, fstream::in);
//    char a[64];
//    infile>>hex>>a;
//    cout<<a[0]<<a[1];
    
//    bitset<64> x;
//    x.set(str);
////    bitset<64>
//    x|=x<<40;
//    cout<<x;
    return 0;
}
