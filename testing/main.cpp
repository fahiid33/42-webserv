#include <iostream>
#include <string>

using namespace std;

int main(int argc, char * argv[])
{
    string file(argv[1]);

    cout << file.substr(file.find_last_of('/') + 1, file.find_first_of('?') - file.find_last_of('/') - 1) << endl;
}