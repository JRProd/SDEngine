#pragma once
#include "Engine/Event/Job.hpp"


#include <atomic>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

class JobSystem
{
public:
    static JobSystem& INSTANCE();
    ~JobSystem() = default;

    void Startup();
    void Shutdown();

    JobSystem( const JobSystem& ) = delete;
    void operator=( const JobSystem& ) = delete;

    void AddWorkerThreads( unsigned int numberToAdd );

    unsigned int ScheduleJob( Job& newJob );

    void ClaimJob( unsigned int jobIndex, bool deleteAfter = true );
    void ClaimJobs( unsigned int jobType, bool deleteAfter = true );
    void ClaimAllJobs( bool deleteAfter = true );

    void FinishJob( unsigned int jobIndex, bool deleteAfter = true );
    void FinishJobs( unsigned int jobType, bool deleteAfter = true );
    void FinishAllJobs( bool deleteAfter = true );

    // Status Queries
    bool IsJobPresent( unsigned int jobIndex ) const;
    bool IsJobQueued( unsigned int jobIndex ) const;
    bool IsJobActive( unsigned int jobIndex ) const;
    bool IsJobComplete( unsigned int jobIndex ) const;
    bool AreAnyJobsPresent() const;
    bool AreAnyJobsQueued() const;
    bool AreAnyJobsActive() const;
    bool AreAnyJobsCompleted() const;
    bool AreAllJobsOfTypeComplete( unsigned int jobType ) const;
    bool AreAnyJobsOfTypePresent( unsigned int jobType ) const;
    bool AreAnyJobsOfTypeQueued( unsigned int jobType ) const;
    bool AreAnyJobsOfTypeActive( unsigned int jobType ) const;
    bool AreAnyJobsOfTypeCompleted( unsigned int jobType ) const;
    unsigned int GetNumPresentJobs( ) const;
    unsigned int GetNumIncompleteJobs() const;
    unsigned int GetNumQueuedJobs() const;
    unsigned int GetNumActiveJobs() const;
    unsigned int GetNumCompletedJobs() const;
    unsigned int GetNumPresentJobsOfType( unsigned int jobType ) const;
    unsigned int GetNumIncompleteJobsOfType( unsigned int jobType ) const;
    unsigned int GetNumQueuedJobsOfType( unsigned int jobType ) const;
    unsigned int GetNumActiveJobsOfType( unsigned int jobType ) const;
    unsigned int GetNumCompletedJobsOfType( unsigned int jobType ) const;

private:
    std::atomic<bool> m_StopJobSearching = false;

    std::vector<std::thread*> m_Threads;
    std::deque<Job*> m_QueuedJobList;
    std::mutex m_QueuedJobMutex;
    std::vector<Job*> m_ActiveJobList;
    std::mutex m_ActiveJobMutex;
    std::vector<Job*> m_CompletedJobList;
    std::mutex m_CompletedJobMutex;

    JobSystem() = default;
    void JobSearchThreadEntry();

    void AddJobToActiveList( Job* job );
    void RemoveJobFromActiveList( const Job* job );

    void AddJobToCompletedList( Job* job );
};
