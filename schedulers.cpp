#include "schedulers.h"
#include<bits/stdc++.h>

//Round Robin scheduler implementation. In general, this function maintains a double ended queue
//of processes that are candidates for scheduling (the ready variable) and always schedules
//the first process on that list, if available (i.e., if the list has members)
int RoundRobin(const int& curTime, const vector<Process>& procList, const int& timeQuantum)
{
    static int timeToNextSched = timeQuantum;  //keeps track of when we should actually schedule a new process
    static deque<int> ready;  //keeps track of the processes that are ready to be scheduled

    int idx = -1, chk;
    bool done;

    // first look through the process list and find any processes that are newly ready and
    // add them to the back of the ready queue
    for(int i = 0, i_end = procList.size(); i < i_end; ++i)
    {
        if(procList[i].startTime == curTime)
        {
            ready.push_back(i);
        }
    }

    // now take a look the head of the ready queue, and update if needed
    // (i.e., if we are supposed to schedule now or the process is done)
    if(timeToNextSched == 0 || procList[ready[0]].isDone)
    {
        // the process at the start of the ready queue is being taken off of the
        // processor

        // if the process isn't done, add it to the back of the ready queue
        if(!procList[ready[0]].isDone)
        {
            ready.push_back(ready[0]);
        }

        // remove the process from the front of the ready queue and reset the time until
        // the next scheduling
        ready.pop_front();
        timeToNextSched = timeQuantum;
    }

    // if the ready queue has any processes on it
    if(ready.size() > 0)
    {
        // grab the front process and decrement the time to next scheduling
        idx = ready[0];
        --timeToNextSched;
    }
    // if the ready queue has no processes on it
    else
    {
        // send back an invalid process index and set the time to next scheduling
        // value so that we try again next time step
        idx = -1;
        timeToNextSched = 0;
    }

    // return back the index of the process to schedule next
    return idx;
}

int ShortProcNext(const int& curTime, const vector<Process>& procList)
{
    static deque<int> shortest;  // Queues processes in order of shortest to longest
                                // Top process will always be the shortest active process
    int idx = -1;

    // first add new processes to the queue
    for(int i = 0, i_end = procList.size(); i < i_end; ++i)
    {
        if(procList[i].startTime == curTime)
        {
            shortest.push_back(i);
        }
    }

    // Remove top process if it has completed
    if(procList[shortest[0]].isDone)
    {
        shortest.pop_front();
    }

    if(shortest.size() > 1)
    {
        // need to push shortest totaltimeneeded to top
        for(int i = 1; i < shortest.size(); i++)
        {
            if(procList[shortest[0]].totalTimeNeeded > procList[shortest[i]].totalTimeNeeded
                && procList[shortest[0]].timeScheduled == 0)
            {
                int temp = shortest[i]; // Keep track of original process index
                shortest.erase(shortest.begin() + i);
                shortest.push_front(temp);
            }
        }
    }

    // if the shortest queue has any processes on it
    if(procList.size() > 0)
    {
        // grab the front process and decrement the time to next scheduling
        idx = shortest[0];
    }
    // if the ready queue has no processes on it
    else
    {
        // send back an invalid process index and set the time to next scheduling
        // value so that we try again next time step
        idx = -1;
    }
    return idx;
}

int ShortRemTime(const int& curTime, const vector<Process>& procList)
{
    static deque<int> shortest;  // Keeps track of the processes that are ready to be scheduled
                                // Top process will always be the process with the shortest remaining time
    int idx = -1;

    // first add new processes to the queue
    for(int i = 0, i_end = procList.size(); i < i_end; ++i)
    {
        if(procList[i].startTime == curTime)
        {
            shortest.push_back(i);
        }
    }

    // Remove top process if it has completed
    if(procList[shortest[0]].isDone)
    {
        shortest.pop_front();
    }

    if(shortest.size() > 1)
    {
        for(int i = 1; i < shortest.size(); i++)
        {
            // Compares wait times of first active process against all other active processes
            // The process with the least amount of remaining time gets pushed to the top
            if(procList[shortest[0]].totalTimeNeeded - procList[shortest[0]].timeScheduled 
                > procList[shortest[i]].totalTimeNeeded - procList[shortest[i]].timeScheduled)
            {
                int temp = shortest[i]; // Keep track of original process index
                shortest.erase(shortest.begin() + i);
                shortest.push_front(temp);
            }
        }
    }

    // if the shortest queue has any processes on it
    if(procList.size() > 0)
    {
        // grab the front process and decrement the time to next scheduling
        idx = shortest[0];
    }
    // if the ready queue has no processes on it
    else
    {
        // send back an invalid process index and set the time to next scheduling
        // value so that we try again next time step
        idx = -1;
    }
    return idx;
}

int HighRespRatio(const int& curTime, const vector<Process>& procList)
{
    static deque<int> highestReady;  //keeps track of the processes that are ready to be scheduled
                                     //The top process will always be the process with the highest response ratio
    int idx = -1, chk;

    // first look through the process list and find any processes that are newly ready and
    // add them to the back of the ready queue
    for(int i = 0, i_end = procList.size(); i < i_end; ++i)
    {
        if(procList[i].startTime == curTime)
        {
            highestReady.push_back(i);
        }
    }

    // now take a look the head of the ready queue, and update if needed
    // (i.e., if we are supposed to schedule now or the process is done)
    if(procList[highestReady[0]].isDone)
    {
        highestReady.pop_front();
    }


    if(highestReady.size() > 1)
    {
        // need to push process with highest response ratio to top
        for(int i = 1; i < highestReady.size(); i++)
        {
            //Calculates response ratio for top process
            double respRatio_0 = curTime - procList[highestReady[0]].startTime
                                        + procList[highestReady[0]].totalTimeNeeded;
                respRatio_0 = respRatio_0/procList[highestReady[0]].totalTimeNeeded;
                
            //Calculates response ratio for i process
            double respRatio_i = curTime - procList[highestReady[i]].startTime
                                         + procList[highestReady[i]].totalTimeNeeded;
                   respRatio_i = respRatio_i/procList[highestReady[i]].totalTimeNeeded;

            // If response ratio i is greater and the top process hasn't started
            if(respRatio_i > respRatio_0 && procList[highestReady[0]].timeScheduled == 0)
            {
                int temp = highestReady[i]; // Keep track of original process index
                highestReady.erase(highestReady.begin() + i);
                highestReady.push_front(temp);
            }
        }
    }

    // if the ready queue has any processes on it
    if(highestReady.size() > 0)
    {
        // grab the front process and decrement the time to next scheduling
        idx = highestReady[0];
    }
    // if the ready queue has no processes on it
    else
    {
        // send back an invalid process index and set the time to next scheduling
        // value so that we try again next time step
        idx = -1;
    }

    // return back the index of the process to schedule next
    return idx;
}
