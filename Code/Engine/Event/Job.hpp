#pragma once

class Job
{
    friend class JobSystem;
public:
    Job();
    virtual ~Job() = default;

    unsigned int GetJobIndex() const;

protected:

    virtual void Execute() = 0;
    virtual void Callback() = 0;

private:
    static unsigned int s_NextJobIndex;

    unsigned int m_JobIndex = 0;
    unsigned int m_JobType = 0;
    unsigned int m_JobFlags = 0;

};