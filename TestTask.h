/* File:    TestTask.h
   Topic:   Definition of execution classes for Lab3
   Date:    October 6 2017 / Orjan Sterner
*/

#ifndef TESTTASK_H
#define TESTTASK_H
#include <string>
#include <fstream>
#include <signal.h>  // sig_atomic_t


class TestTask {
public:
   /** 
     @param doneFlag - ends the test when set to non-zero in handler for SIGUSR2
   */
  TestTask(volatile sig_atomic_t &doneFlag)
  :isDone(doneFlag) 
  {}
  virtual ~TestTask() {};
  virtual void run()=0;  // Run the test task
protected:
  volatile sig_atomic_t &isDone; // Stop test when set in signal handler
};


class IOtask : public TestTask {
public:
   /** 
     @param doneFlag - ends the test when set to non-zero in handler for SIGUSR2
     @param startFile - file to be copied, Daffodils.txt
   */
  IOtask(volatile sig_atomic_t &doneFlag, const std::string &startFile);
  virtual void run();  // Run the test
private:
  const std::string tmpF1, tmpF2; // Temp file names
};


class CPUtask : public TestTask {
public:
  /** 
     @param doneFlag - ends the test when set to non-zero in handler for SIGUSR2
   */
  CPUtask(volatile sig_atomic_t &doneFlag);
  virtual void run();
  static void doSomeRandoms(); // Creates N_RAND random values
private:
  static const int N_RAND = 10000; // # random values in each iteration
};

class CPUandIOtask : public TestTask {
public:
   /** 
     @param doneFlag - ends the test when set to non-zero in handler for SIGUSR2
     @param startFile - file to be copied, Daffodils.txt
   */
  CPUandIOtask(volatile sig_atomic_t &doneFlag, const std::string &startFile);
  virtual void run();
private:
  const std::string tmpF1, tmpF2; // Temp file names
};


#endif
