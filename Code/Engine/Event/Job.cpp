#include "Job.hpp"

#include "Engine/Core/EngineCommon.hpp"

STATIC unsigned int Job::s_NextJobIndex = 0;

Job::Job()
{
    m_JobIndex = s_NextJobIndex;
    s_NextJobIndex++;
}

unsigned int Job::GetJobIndex() const
{
    return m_JobIndex;
}
