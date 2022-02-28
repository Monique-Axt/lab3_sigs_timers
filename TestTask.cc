/* File:    TestTask.cc
   Topic:   Implementation of execution classes for Lab3
   Date:    October 6 2017/ Orjan Sterner
*/

#include <iostream>
#include <ios>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "TestTask.h"
#include "Random.h"
#include <sys/wait.h>

using namespace std;


void checkIOstatus(const string &where, ios &strm, const string &source) {
    if(!strm.good()) {
        cerr << where << " IO-error " << source << endl;
        exit(1);
    }
}

//-------------------------------------------------

IOtask::IOtask(volatile sig_atomic_t &doneFlag, const string &startFile)
        :TestTask(doneFlag), tmpF1("./tmp1.txt"),tmpF2("./tmp2.txt")
{
    // Copy original file to tmpF1
    ifstream inFile(startFile.c_str(),ios_base::in);
    if(!inFile.is_open()) {
        cerr << "IOtask::IOtask, error opening " << tmpF1 << endl;
        exit(1);
    }
    ofstream outFile(tmpF1.c_str(),ios_base::out|ios_base::trunc);
    if(!outFile.is_open()) {
        cerr << "IOtask::IOtask, error opening " << tmpF2 << endl;
        exit(1);
    }
    string s;

    while(getline(inFile,s,'\n'))
        outFile << s << '\n';

    checkIOstatus("IOtask::IOtask",outFile, "writing");

    inFile.close();
    outFile.close();

}
// Copy tmpF1 to tmpF2, delete tmpF1, rename tmpF2->tmpF1
void IOtask::run() {
    string s;
    cout << "PID " << getpid() << ": IOtask copying files..." << endl;
    while(!isDone) {
        ifstream inFile(tmpF1.c_str(),ios_base::in);
        if(!inFile.is_open())
            cerr << "IOtask::run, error opening " << tmpF1 << endl;

        ofstream outFile(tmpF2.c_str(),ios_base::out|ios_base::trunc);
        if(!outFile.is_open())
            cerr << "IOtask::run, error opening " << tmpF2 << endl;

        while(getline(inFile,s,'\n'))
            outFile << s << '\n';

        checkIOstatus("IOtask::run",outFile, "writing");

        inFile.close();
        outFile.close();

        if(unlink(tmpF1.c_str())==-1) {
            cerr << "IOtask::run, unlink fd1" << strerror(errno) << endl;
            exit(1);
        }

        if(rename(tmpF2.c_str(),tmpF1.c_str())==-1) {
            cerr << "IOtask::run, rename fd2" << strerror(errno) << endl;
            exit(1);
        }
    }
}
//-------------------------------------------------

CPUtask::CPUtask(volatile sig_atomic_t &doneFlag)
        :TestTask(doneFlag)
{}

// static member function
void CPUtask::doSomeRandoms() {
    double sum = 0;
    Random myRandEngine;
    for(int i=0;i<N_RAND;++i)
        sum += myRandEngine.next();
}


void CPUtask::run() {
    cout << "PID " << getpid() << ": CPUtask making random numbers..." << endl;
    while(!isDone)
        CPUtask::doSomeRandoms();
}


//--------------------------------------------------

CPUandIOtask::CPUandIOtask(volatile sig_atomic_t &doneFlag, const std::string &startFile)
        :TestTask(doneFlag),tmpF1("./tmp1.txt"),tmpF2("./tmp2.txt")
{

    // Copy original file to tmpF1
    ifstream inFile(startFile.c_str(),ios::in);
    if(!inFile.is_open())
        cerr << "CPUandIOtask::CPUandIOtask, error opening " << tmpF1 << endl;

    ofstream outFile(tmpF1.c_str(),ios::out);
    if(!outFile.is_open())
        cerr << "CPUandIOtask::CPUandIOtask, error opening " << tmpF2 << endl;

    string s;
    while(getline(inFile,s,'\n'))
        outFile << s << '\n';

    checkIOstatus("CPUandIOtask::CPUandIOtask",outFile, "writing");

    inFile.close();
    outFile.close();
}

void CPUandIOtask::run() {
    string s;
    cout << "PID " << getpid() << ": CPUandIOtask copying files and making random numbers..." << endl;
    while(!isDone) {
        // IO task
        ifstream inFile(tmpF1.c_str());
        if(!inFile.is_open())
            cerr << "CPUandIOtask::run, error opening " << tmpF1 << endl;

        ofstream outFile(tmpF2.c_str());
        if(!outFile.is_open())
            cerr << "CPUandIOtask::run, error opening " << tmpF2 << endl;

        while(getline(inFile,s,'\n'))
            outFile << s << '\n';

        checkIOstatus("CPUandIOtask::run",outFile, "writing");


        inFile.close();
        outFile.close();

        if(unlink(tmpF1.c_str())==-1) {
            cerr << "CPUandIOtask::run, unlink fd1" << strerror(errno) << endl;
            exit(1);
        }

        if(rename(tmpF2.c_str(),tmpF1.c_str())==-1) {
            cerr << "CPUandIOtask::run, rename fd2" << strerror(errno) << endl;
            exit(1);
        }
        // CPU part
        CPUtask::doSomeRandoms();
    }
}


