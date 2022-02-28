#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "TestTask.h"
#include "signal.h"
#include <stdio.h>
#include <sstream>
#include <iomanip>
#include <sys/time.h>
#include <cstring>


#define MILLION 1000000
#define TIMER_INTERVAL 5

static volatile sig_atomic_t stopFlag = 0;

double volatile realTime, virtualTime, profTime;

bool cmdLineHandler(int argc, char **argv, std::string &arg);
void setDefault(std::string &taskType);
void startTask(const std::string& taskType);
int setupTimers();
void signalHandler(int signum);


/**
 * Test class for signal handlers and interval timers
 * */
int main(int argc, char* argv[]) {
    std::string taskType;

    if (!cmdLineHandler(argc, argv, taskType)) {
        setDefault(taskType);
    }

    /* install signal handlers */
    struct sigaction sa{};
    sa.sa_handler = &signalHandler;

    // SIGUSR1 setup
    if ( (sigemptyset(&sa.sa_mask) == -1) || (sigaction(SIGUSR1, &sa, nullptr) == -1) )
        perror("Failed to install SIGUSR1 signal handler");

    // SIGUSR2 setup
    if ((sigaction(SIGUSR2, &sa, nullptr) == -1))
        perror("Failed to install SIGUSR2 signal handler");

    // SIGALRM setup
    if ( (sigaction(SIGALRM, &sa, nullptr) == -1))
        perror("Failed to install SIGALRM signal handler");

    // SIGVTALRM setup
    if ((sigaction(SIGVTALRM, &sa, nullptr) == -1))
        perror("Failed to install SIGVTALRM signal handler");

    // SIGPROF setup
    if ((sigaction(SIGPROF, &sa, nullptr) == -1))
        perror("Failed to install SIGPROF signal handler");

    if (setupTimers() == -1)
        throw(std::string("Exception in setupTimers: " + std::string(strerror(errno))));

    startTask(taskType);

    return 0;
}

/** Prints REAL, VIRTUAL, PROF time since start using getitimer() - as time and percentage of absolute time */
void printTimes(itimerval &realValue, itimerval &virtualValue, itimerval &profValue) {
    getitimer(ITIMER_REAL, &realValue);
    getitimer(ITIMER_VIRTUAL, &virtualValue);
    getitimer(ITIMER_PROF, &profValue);

    double realTimeElapsed = realTime + (TIMER_INTERVAL - (realValue.it_value.tv_sec +
                                                           ((double) realValue.it_value.tv_usec /
                                                            1000000)));

    double virtualTimeElapsed = virtualTime + (TIMER_INTERVAL - (virtualValue.it_value.tv_sec +
                                                                 ((double) virtualValue.it_value.tv_usec /
                                                                  1000000)));

    double profTimeElapsed = profTime + (TIMER_INTERVAL - (profValue.it_value.tv_sec +
                                                           ((double) profValue.it_value.tv_usec /
                                                            1000000)));

    std::ostringstream os;

    os << "-----" << std::endl << std::fixed << std::setprecision(2)
       << "Real time   : " << realTimeElapsed << " s, " << realTimeElapsed / realTimeElapsed * 100 << "%" << std::endl
       << "Virtual time: " << virtualTimeElapsed << " s, " << virtualTimeElapsed / realTimeElapsed * 100 << "%" << std::endl
       << "Prof time   : " << profTimeElapsed << " s, " << profTimeElapsed / realTimeElapsed * 100 << "%" << std::endl
       << "-----" << std::endl;

    std::cout << os.str();
}

/** Handles incoming signals: SIGUSR1, SIGUSR2, SIGTERM, SIGALRM, SIGVTALRM */
void signalHandler(int signum) {
    struct itimerval realValue{};
    struct itimerval virtualValue{};
    struct itimerval profValue{};

    switch (signum) {
        case SIGUSR1: {
            printTimes(realValue, virtualValue, profValue);

        } break;

        case SIGUSR2: {
            std::cout << "Stopping ..." << std::endl;
            auto stopTime = realTime + 30;
            while (realTime < stopTime) {}  // wait 30 seconds before altering the stopFlag

            stopFlag = 1;
            printTimes(realValue, virtualValue, profValue);

            if (kill(getppid(), SIGTERM) == -1)
                perror("Failed to kill parent");

        } break;

        case SIGALRM: {     // real
            double realTimeElapsed = realTime + (TIMER_INTERVAL - (realValue.it_value.tv_sec +
                                                                   ((double) realValue.it_value.tv_usec /
                                                                    1000000)));
            realTime = realTimeElapsed;
        } break;

        case SIGVTALRM: {   // virtual
            double virtualTimeElapsed = virtualTime + (TIMER_INTERVAL - (virtualValue.it_value.tv_sec +
                                                                         ((double) virtualValue.it_value.tv_usec /
                                                                          1000000)));
            virtualTime = virtualTimeElapsed;
        } break;

        case SIGPROF: {     // prof
            double profTimeElapsed = profTime + (TIMER_INTERVAL - (profValue.it_value.tv_sec +
                                                                   ((double) profValue.it_value.tv_usec /
                                                                    1000000)));
            profTime = profTimeElapsed;
        } break;
    }
}

/** Configure timers */
int setupTimers() {
    struct itimerval value;
    value.it_interval.tv_sec = TIMER_INTERVAL;
    value.it_interval.tv_usec = 0;
    value.it_value = value.it_interval;

    if (setitimer(ITIMER_REAL, &value, NULL) == -1){
        perror("Failed to set real timer");
        return -1;
    }

    if (setitimer(ITIMER_VIRTUAL, &value, NULL) == -1) {
        perror("Failed to set virtual timer");
        return -1;
    }

    if (setitimer(ITIMER_PROF, &value, NULL) == -1){
        perror("Failed to set prof timer");
        return -1;
    }

    return 0;
}

/** Start task based on command-line arg (default I/O task) */
void startTask(const std::string& taskType) {
    TestTask* task;

    if (taskType == "io") {
        task = new IOtask(stopFlag, "Daffodils.txt");
    }
    else if (taskType == "cpu") {
        task = new CPUtask(stopFlag);
    }
    else if (taskType == "cpu-io") {
        task = new CPUandIOtask(stopFlag, "Daffodils.txt");
    }

    task->run();
}

/** Set default task if no command-line arg provided */
void setDefault(std::string &taskType) {
    taskType = "io";
    std::cout << "Missing/invalid arg: Using default task [" << taskType << "]" << std::endl;
}

/** Check command-line arg provided */
bool cmdLineHandler(int argc, char *argv[], std::string &arg) {
    if (argc > 1) {
        arg = argv[1];
        return true;
    }
    return false;
}

