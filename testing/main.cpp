#include <iostream>
#include <string>

using namespace std;

int main(int argc, char * argv[])
{
    string host(argv[1]);

    cout << host.substr(0, host.find(':')) << endl;
}