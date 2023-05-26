#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;

// Structure to represent a job with its ID and processing times on each machine
struct Job {
    int id;
    vector<int> times;
};

// Function to compare two jobs based on their total processing time
// Used for sorting jobs in descending order of total processing time
bool compareJobs(Job& a, Job& b) {
    int sumA = accumulate(a.times.begin(), a.times.end(), 0);
    int sumB = accumulate(b.times.begin(), b.times.end(), 0);
    if (sumA != sumB) {
        return sumA > sumB;
    } else {
        return a.id < b.id;
    }
}

// Function to calculate the makespan (Cmax) of a schedule
int calculateCmax(vector<Job>& jobs, int M) {
    vector<int> cMax(M, 0);

    // Calculate the completion time on each machine for each job
    for (const auto& job : jobs) {
        cMax[0] += job.times[0];
        for (int j = 1; j < M; j++) {
            cMax[j] = max(cMax[j], cMax[j-1]) + job.times[j];
        }
    }
    return cMax[M-1]; // The maximum completion time across all machines is the makespan
}

// Implementation of the NEH algorithm for job scheduling
vector<Job> nehAlgorithm(vector<Job>& jobs, int M) {
    // Sort jobs in descending order of total processing time
    vector<Job> sortedJobs = jobs;
    sort(sortedJobs.begin(), sortedJobs.end(), compareJobs);
    
    vector<Job> schedule;
    schedule.push_back(sortedJobs[0]);

    // Insert each job at the best position in the schedule
    for (int i = 1; i < sortedJobs.size(); ++i) {
        int minCmax = INT_MAX;
        vector<Job> bestSchedule;
        for (int j = 0; j <= schedule.size(); ++j) {
            vector<Job> tempSchedule = schedule;
            tempSchedule.insert(tempSchedule.begin() + j, sortedJobs[i]);
            int tempCmax = calculateCmax(tempSchedule, M);
            // Choose the schedule with the lowest makespan (Cmax)
            // If two schedules have the same makespan, choose the one with the smaller job ID
            if (tempCmax < minCmax || (tempCmax == minCmax && sortedJobs[i].id < tempSchedule[j].id)) {
                minCmax = tempCmax;
                bestSchedule = tempSchedule;
            }
        }
        schedule = bestSchedule;
    }

    return schedule;
}

int main() {
    // Open input file
    ifstream inputdata("data.txt");
    int N, M;

    int globalDataID = 0;

    // Process each dataset in the input file
    while (!inputdata.eof()) {
        string dataset;
        // Format dataset name based on the globalDataID
        if (globalDataID < 10) {
            dataset = "data.00" + to_string(globalDataID) + ":";
        }
        else if (globalDataID < 100) {
            dataset = "data.0" + to_string(globalDataID) + ":";
        }
        else {
            dataset = "data." + to_string(globalDataID) + ":";
        }
        globalDataID++;
        
        string caret;
        // Find the start of the current dataset in the input file
        while (caret != dataset && !inputdata.eof()) inputdata >> caret;

        if (caret != dataset) break;

        cout << dataset;
        // Read the number of jobs (N) and machines (M) for the current dataset
        inputdata >> N >> M;

        // Read the processing times for each job
        vector<Job> jobs(N);
        for (int i = 0; i < N; i++) {
            Job job;
            job.id = i + 1;
            job.times.resize(M);
            for (int j = 0; j < M; j++) {
                inputdata >> job.times[j];
            }
            jobs[i] = job;
        }

        // Apply the NEH algorithm to generate a schedule
        vector<Job> schedule = nehAlgorithm(jobs, M);
        int cMax = calculateCmax(schedule, M);

        // Print the makespan (Cmax) and the job order in the schedule
        cout << " Cmax = " << cMax << endl;
        
        //Print the orders
        for (const auto& job : schedule) {
            cout << job.id << " ";
        }
        cout << endl;
    }

    // Close the input file
    inputdata.close();

    return 0;
}
