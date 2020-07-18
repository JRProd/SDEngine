#include "JobSystem.hpp"

#include "Engine/Console/Console.hpp"

#include <thread>

#pragma optimize("", off)
JobSystem& JobSystem::INSTANCE()
{
    static JobSystem jobSystem;
    return jobSystem;
}

void JobSystem::Startup()
{
    const unsigned int numCores = std::thread::hardware_concurrency();
    m_Threads.reserve( numCores );

#if !defined( ENGINE_DISABLE_CONSOLE )
    g_Console->Log( LOG_INFO, Stringf( "JobSystem::JobSystem - Created %u threads", numCores ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)

    for( unsigned int core = 0; core < numCores; ++core )
    {
        m_Threads.push_back( new std::thread( &JobSystem::JobSearchThreadEntry, this ) );
    }
}

void JobSystem::Shutdown()
{
    m_StopJobSearching = true;

    for( std::thread* thread : m_Threads )
    {
        thread->join();
        delete thread;
        thread = nullptr;
    }
}

void JobSystem::AddWorkerThreads( const unsigned int numberToAdd )
{
    for( unsigned int extra = 0; extra < numberToAdd; ++extra )
    {
        m_Threads.push_back( new std::thread( &JobSystem::JobSearchThreadEntry, this ) );
    }
}

unsigned int JobSystem::ScheduleJob( Job& newJob )
{
#if !defined(ENGINE_DISABLE_CONSOLE)
    g_Console->Log( LOG_LOG, Stringf("New Job %u added to queue", newJob.m_JobIndex ) );
#endif // !defined(ENGINE_DISABLE_CONSOLE)

    m_QueuedJobMutex.lock();
    m_QueuedJobList.push_back( &newJob );
    m_QueuedJobMutex.unlock();

    return newJob.m_JobIndex;
}

void JobSystem::ClaimJob( unsigned int jobIndex, bool deleteAfter )
{
    m_CompletedJobMutex.lock();
    for( Job*& job : m_CompletedJobList )
    {
        if( job && job->m_JobIndex == jobIndex )
        {
            job->Callback();
            if(deleteAfter )
            {
                delete job;
            }
            job = nullptr;
        }
    }
}

void JobSystem::ClaimJobs( unsigned int jobType, bool deleteAfter )
{
    m_CompletedJobMutex.lock();
    for( Job*& job : m_CompletedJobList )
    {
        if( job && job->m_JobType == jobType )
        {
            job->Callback();
            if( deleteAfter )
            {
                delete job;
            }
            job = nullptr;
        }
    }

}

void JobSystem::ClaimAllJobs( bool deleteAfter )
{
    std::vector<Job*> completedJobs;
    m_CompletedJobMutex.lock();
    m_CompletedJobList.swap( completedJobs );
    m_CompletedJobMutex.unlock();

    for( Job*& job : completedJobs )
    {
        if( job == nullptr ) { continue; }

        job->Callback();
        if( deleteAfter )
        {
            delete job;
        }
        job = nullptr;
    }
}

void JobSystem::FinishJob( const unsigned int jobIndex, const bool deleteAfter )
{
    if( !IsJobPresent( jobIndex ) )
    {
        return;
    }

    bool jobCompleted = false;
    do
    {
        m_CompletedJobMutex.lock();
        for( Job*& job : m_CompletedJobList )
        {
            if( job && job->m_JobIndex == jobIndex )
            {
                job->Callback();
                if( deleteAfter )
                {
                    delete job;
                }
                job = nullptr;

                jobCompleted = true;
                break;
            }
        }
        m_CompletedJobMutex.unlock();
        std::this_thread::sleep_for( std::chrono::microseconds( 25 ) );
    } while( !jobCompleted );
}

void JobSystem::FinishJobs( const unsigned int jobType, const bool deleteAfter )
{
    if( !AreAnyJobsOfTypePresent( jobType ) )
    {
        return;
    }

    // Loop endlessly until all jobs of type are in the completed list
    while( AreAnyJobsOfTypeQueued( jobType ) || AreAnyJobsOfTypeActive( jobType ) )
    {
        std::this_thread::sleep_for( std::chrono::microseconds( 25 ) );
    }

    m_CompletedJobMutex.lock();
    for( Job*& job : m_CompletedJobList )
    {
        if( job && job->m_JobIndex == jobType )
        {
            job->Callback();
            if( deleteAfter )
            {
                delete job;
            }
            job = nullptr;
        }
    }
    m_CompletedJobMutex.unlock();
}

void JobSystem::FinishAllJobs( const bool deleteAfter )
{
    if( !AreAnyJobsPresent() )
    {
        return;
    }

    // Loop endlessly until all jobs of type are in the completed list
    while( AreAnyJobsQueued() || AreAnyJobsActive() )
    {
        std::this_thread::sleep_for( std::chrono::microseconds( 25 ) );
    }

    m_CompletedJobMutex.lock();
    for( Job*& job : m_CompletedJobList )
    {
        if( job == nullptr ) { continue; }

        job->Callback();
        if( deleteAfter )
        {
            delete job;
        }
        job = nullptr;
    }
    m_CompletedJobMutex.unlock();
}

bool JobSystem::IsJobPresent( unsigned int jobIndex ) const
{
    return IsJobQueued( jobIndex ) || IsJobActive( jobIndex ) || IsJobComplete( jobIndex );
}

bool JobSystem::IsJobQueued( unsigned int jobId ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );
    bool isQueued = false;

    notConst->m_QueuedJobMutex.lock();
    for( const Job* job : m_QueuedJobList )
    {
        if( job && job->m_JobIndex == jobId )
        {
            isQueued = true;
            break;
        }
    }
    notConst->m_QueuedJobMutex.unlock();

    return isQueued;
}

bool JobSystem::IsJobActive( unsigned int jobIndex ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );
    bool isActive = false;

    notConst->m_ActiveJobMutex.lock();
    for( const Job* job : m_ActiveJobList )
    {
        if( job && job->m_JobIndex == jobIndex )
        {
            isActive = true;
            break;
        }
    }
    notConst->m_ActiveJobMutex.unlock();

    return isActive;
}

bool JobSystem::IsJobComplete( unsigned int jobIndex ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );
    bool isComplete = false;

    notConst->m_CompletedJobMutex.lock();
    for( const Job* job : m_CompletedJobList )
    {
        if( job && job->m_JobIndex == jobIndex )
        {
            isComplete = true;
            break;
        }
    }
    notConst->m_CompletedJobMutex.unlock();

    return isComplete;
}

bool JobSystem::AreAnyJobsPresent() const
{
    return AreAnyJobsQueued() || AreAnyJobsActive() || AreAnyJobsCompleted();
}

bool JobSystem::AreAnyJobsQueued() const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );
    notConst->m_QueuedJobMutex.lock();
    bool isEmpty = true;
    for( const Job* job : m_QueuedJobList )
    {
        if( job )
        {
            isEmpty = false;
            break;
        }
    }
    notConst->m_QueuedJobMutex.unlock();
    return !isEmpty;
}

bool JobSystem::AreAnyJobsActive() const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );
    notConst->m_ActiveJobMutex.lock();
    bool isEmpty = true;
    for( const Job* job : m_ActiveJobList )
    {
        if( job )
        {
            isEmpty = false;
            break;
        }
    }
    notConst->m_ActiveJobMutex.unlock();
    return !isEmpty;
}

bool JobSystem::AreAnyJobsCompleted() const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );
    notConst->m_CompletedJobMutex.lock();
    bool isEmpty = true;
    for( const Job* job : m_CompletedJobList )
    {
        if( job )
        {
            isEmpty = false;
            break;
        }
    }
    notConst->m_CompletedJobMutex.unlock();
    return !isEmpty;
}

bool JobSystem::AreAllJobsOfTypeComplete( unsigned int jobType ) const
{
    if( !AreAnyJobsOfTypePresent( jobType ) )
    {
        return true;
    }
    return !AreAnyJobsOfTypeQueued( jobType ) && !AreAnyJobsOfTypeActive( jobType );
}

bool JobSystem::AreAnyJobsOfTypePresent( unsigned int jobType ) const
{
    return AreAnyJobsOfTypeQueued( jobType ) || AreAnyJobsOfTypeActive( jobType ) || AreAnyJobsOfTypeCompleted( jobType );
}

bool JobSystem::AreAnyJobsOfTypeQueued( unsigned int jobType ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    bool jobTypeQueued = false;
    notConst->m_QueuedJobMutex.lock();
    for( const Job* job : m_QueuedJobList )
    {
        if( job && job->m_JobType == jobType )
        {
            jobTypeQueued = true;
            break;
        }
    }
    notConst->m_QueuedJobMutex.unlock();

    return jobTypeQueued;
}

bool JobSystem::AreAnyJobsOfTypeActive( unsigned int jobType ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    bool jobTypeQueued = false;
    notConst->m_ActiveJobMutex.lock();
    for( const Job* job : m_ActiveJobList )
    {
        if( job && job->m_JobType == jobType )
        {
            jobTypeQueued = true;
            break;
        }
    }
    notConst->m_ActiveJobMutex.unlock();

    return jobTypeQueued;
}

bool JobSystem::AreAnyJobsOfTypeCompleted( unsigned int jobType ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    bool jobTypeQueued = false;
    notConst->m_CompletedJobMutex.lock();
    for( const Job* job : m_CompletedJobList )
    {
        if( job && job->m_JobType == jobType )
        {
            jobTypeQueued = true;
            break;
        }
    }
    notConst->m_CompletedJobMutex.unlock();

    return jobTypeQueued;
}

unsigned int JobSystem::GetNumPresentJobs() const
{
    return GetNumQueuedJobs() + GetNumActiveJobs() + GetNumCompletedJobs();;
}

unsigned int JobSystem::GetNumIncompleteJobs() const
{
    return GetNumQueuedJobs() + GetNumActiveJobs();
}

unsigned int JobSystem::GetNumQueuedJobs() const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    unsigned int queuedJobs = 0;
    notConst->m_QueuedJobMutex.lock();
    for( const Job* job : m_QueuedJobList )
    {
        if( job )
        {
            queuedJobs++;
        }
    }
    notConst->m_QueuedJobMutex.unlock();

    return queuedJobs;
}

unsigned int JobSystem::GetNumActiveJobs() const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    unsigned int activeJobs = 0;
    notConst->m_ActiveJobMutex.lock();
    for( const Job* job : m_ActiveJobList )
    {
        if( job )
        {
            activeJobs++;
        }
    }
    notConst->m_ActiveJobMutex.unlock();

    return activeJobs;
}

unsigned int JobSystem::GetNumCompletedJobs() const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    unsigned int completedJobs = 0;
    notConst->m_CompletedJobMutex.lock();
    for( const Job* job : m_CompletedJobList )
    {
        if( job )
        {
            completedJobs++;
        }
    }
    notConst->m_CompletedJobMutex.unlock();

    return completedJobs;
}

unsigned int JobSystem::GetNumPresentJobsOfType( unsigned int jobType ) const
{
    return GetNumQueuedJobsOfType(jobType) + GetNumActiveJobsOfType( jobType ) + GetNumCompletedJobsOfType( jobType );
}

unsigned int JobSystem::GetNumIncompleteJobsOfType( unsigned int jobType ) const
{
    return GetNumQueuedJobsOfType( jobType ) + GetNumActiveJobsOfType( jobType );
}

unsigned int JobSystem::GetNumQueuedJobsOfType( unsigned int jobType ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    unsigned int queuedJobs = 0;
    notConst->m_QueuedJobMutex.lock();
    for( const Job* job : m_QueuedJobList )
    {
        if( job && job->m_JobType == jobType )
        {
            queuedJobs++;
        }
    }
    notConst->m_QueuedJobMutex.unlock();

    return queuedJobs;
}

unsigned int JobSystem::GetNumActiveJobsOfType( unsigned int jobType ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    unsigned int queuedJobs = 0;
    notConst->m_ActiveJobMutex.lock();
    for( const Job* job : m_ActiveJobList )
    {
        if( job && job->m_JobType == jobType )
        {
            queuedJobs++;
        }
    }
    notConst->m_ActiveJobMutex.unlock();

    return queuedJobs;
}

unsigned int JobSystem::GetNumCompletedJobsOfType( unsigned int jobType ) const
{
    JobSystem* notConst = const_cast<JobSystem*>( this );

    unsigned int queuedJobs = 0;
    notConst->m_CompletedJobMutex.lock();
    for( const Job* job : m_CompletedJobList )
    {
        if( job && job->m_JobType == jobType )
        {
            queuedJobs++;
        }
    }
    notConst->m_CompletedJobMutex.unlock();

    return queuedJobs;
}

void JobSystem::JobSearchThreadEntry()
{
    while( !m_StopJobSearching )
    {
        m_QueuedJobMutex.lock();
        if( !m_QueuedJobList.empty() )
        {
            Job* chosenJob = m_QueuedJobList.front();
            m_QueuedJobList.pop_front();
            m_QueuedJobMutex.unlock();

            AddJobToActiveList( chosenJob );
            chosenJob->Execute();
            RemoveJobFromActiveList( chosenJob );

            AddJobToCompletedList( chosenJob );
        }
        else
        {
            m_QueuedJobMutex.unlock();
            std::this_thread::sleep_for( std::chrono::microseconds(25) );
        }
    }
}

void JobSystem::AddJobToActiveList( Job* job )
{
    m_ActiveJobMutex.lock();
    bool found = false;
    for( Job*& foundJob : m_ActiveJobList )
    {
        if( foundJob == nullptr )
        {
            foundJob = job;
            found = true;
            break;
        }
    }
    if( !found )
    {
        m_ActiveJobList.push_back( job );
    }
    m_ActiveJobMutex.unlock();
}

void JobSystem::RemoveJobFromActiveList( const Job* job )
{
    m_ActiveJobMutex.lock();
    for( Job*& foundJob : m_ActiveJobList )
    {
        if( foundJob == job )
        {
            foundJob = nullptr;
            break;
        }
    }
    m_ActiveJobMutex.unlock();
}

void JobSystem::AddJobToCompletedList( Job* job)
{
    m_CompletedJobMutex.lock();
    bool found = false;
    for( Job*& foundJob : m_CompletedJobList )
    {
        if( foundJob == nullptr )
        {
            foundJob = job;
            found = true;
            break;
        }
    }
    if( !found )
    {
        m_CompletedJobList.push_back( job );
    }
    m_CompletedJobMutex.unlock();
}

#pragma optimize("", on)