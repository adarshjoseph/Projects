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


//struct S {
//    // three-bit unsigned field,
//    // allowed values are 0...7
//    static const int a;
//    unsigned int b : a;
//};
//int main()
//{
//    S s = {126};
//    ++s.b; // unsigned overflow (guaranteed wrap-around)
//    std::cout << s.b << '\n'; // output: 0
//    return 0;
//}

int main(int argc, const char * argv[]) {
//    vector<char> ar = {'a','b','c','d','e','f','g','h'};
//    rotate(ar.begin(), ar.begin()+1, ar.end());
//    int a=8;
//    char cr[a];
//    char c;
//    for (unsigned int i = 0; i < 8 && cin.get(c); ++i) {
//        cr[i]=c;
//        cout<<cr[i]<<" ";
//    }
//    bitset<8> z;
////    z.set();
////    z.set(0,1);
//    z.set(8-1,1);
//    cout<<z<<endl;
    string a ="ajdlfjaa";
    string b ="?<>+_|-}";
    char r[8];
//    cout<<"a:"<<bitset<8>(a)<<endl;
    for(int i=0; i<a.size(); i++){
        cout<<"a:"<<bitset<8>(a[i])<<endl;
        cout<<"b:"<<bitset<8>(b[i])<<endl;
        r[i]=a[i]^b[i];
        bitset<8> x(r[i]);
        cout<<"r:"<<x<<endl;
        cout<<"back: "<<r[i]<<endl;
    }
    cout<<endl;
//    char z = (char)ar[1]^ar[2];
//    bitset<8> x(z);
//    cout<<x<<endl;
    return 0;
}
//    cout<<endl;
////    const char *filename = "/Users/adarshjoseph/Desktop/repos/Projects/encryptUtil/key_files/64b";
////    ifstream infile(filename, fstream::in);
////    char a[64];
////    infile>>hex>>a;
////    cout<<a[0]<<a[1];
//
////    bitset<64> x;
////    x.set(str);
//////    bitset<64>
////    x|=x<<40;
////    cout<<x;


