#include <optional>
#include <shared_mutex>

template<typename T>
class RwLock
{
public:
    class ReadLockguard
    {
        using Self = RwLock<T>::ReadLockguard;
        using Parent = RwLock<T>;
        using Lock = std::shared_lock<std::shared_mutex>;

    public:
        ReadLockguard(Parent* parent, Lock&& lock) //TODO: refactor lockguards to use common base
            : m_Parent(parent)
            , m_Lock(std::move(lock))
        {
        }

        ReadLockguard(const ReadLockguard&) = delete;
        ReadLockguard& operator=(const ReadLockguard&) = delete;
        ReadLockguard(ReadLockguard&&) = default;

        //const T& operator*()
        //{
        //    return m_Parent->m_Data;
        //}

        const T& value() const
        {
            return m_Parent->m_Data;
        }

        operator const T&() const
        {
            return m_Parent->m_Data;
        }

        bool operator==(const T& other) const
        {
            return m_Parent->m_Data == other;
        }

        friend std::ostream& operator<<(std::ostream& os, Self& lockguard)
        {
            os << lockguard.value();
            return os;
        }

    private:
        Parent* m_Parent;
        Lock m_Lock;
    };

    class WriteLockguard
    {
        using Self = RwLock<T>::WriteLockguard;
        using Parent = RwLock<T>;
        using Lock = std::unique_lock<std::shared_mutex>;

    public:
        WriteLockguard(Parent* parent, Lock&& lock)
            : m_Parent(parent)
            , m_Lock(std::move(lock))
        {
        }

        WriteLockguard(const WriteLockguard&) = delete;
        WriteLockguard& operator=(const WriteLockguard&) = delete;
        WriteLockguard(WriteLockguard&&) = default;

        //T& operator*()
        //{
        //    return m_Parent->m_Data;
        //}

        const T& value() const
        {
            return *this;
        }

        operator T&()
        {
            return m_Parent->m_Data;
        }

        bool operator==(const T& other) const
        {
            return m_Parent->m_Data == other;
        }

        Self& operator=(const T& val)
        {
            m_Parent->m_Data = val;
            return *this;
        }

    private:
        Parent* m_Parent;
        Lock m_Lock;
    };

    template<typename U>
    RwLock(U&& data)
        : m_Data(std::forward<U>(data)) // TODO: verify that this moves/copies as needed
    {
    }

    RwLock(const RwLock&) = delete;
    RwLock& operator=(const RwLock&) = delete;

    typename RwLock<T>::ReadLockguard read()
    {
        std::shared_lock<std::shared_mutex> lock(m_Mutex);
        return RwLock<T>::ReadLockguard{ this, std::move(lock) };
    }

    std::optional<typename RwLock<T>::ReadLockguard> try_read()
    {
        std::shared_lock<std::shared_mutex> lock(m_Mutex, std::try_to_lock);
        if (lock.owns_lock())
        {
            return std::optional<typename RwLock<T>::ReadLockguard>(std::in_place, this, std::move(lock));
        }
        else
        {
            return std::nullopt;
        }
    }

    typename RwLock<T>::WriteLockguard write()
    {
        std::unique_lock<std::shared_mutex> lock(m_Mutex);
        return RwLock<T>::WriteLockguard{ this, std::move(lock) };
    }
    
    std::optional<typename RwLock<T>::WriteLockguard> try_write()
    {
        std::unique_lock<std::shared_mutex> lock(m_Mutex, std::try_to_lock);
        if (lock.owns_lock())
        {
            return typename RwLock<T>::WriteLockguard{ this, std::move(lock) };
        }
        else
        {
            return std::nullopt;
        }
    }

private:
    T m_Data;
    std::shared_mutex m_Mutex;
};
