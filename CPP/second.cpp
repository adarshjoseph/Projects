#include<iostream>

using namespace std;

int main()
{
    unsigned char half_limit = 150;
    int j=0;
    cout<<"prod="<<2*half_limit<<endl;
    for (unsigned char i = 0; i < half_limit; i++)
    {
        cout<<i<<" ";
        j++;
    }
    unsigned char i = 254;
    cout<<i<<++i<<endl;
    cout<<"j="<<j<<endl;
    return 0;
}


