#include <iostream>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include <safeshared/rwlock.h>

SCENARIO("RwLock can be initialized and accessed")
{
    GIVEN("An integer number")
    {
        RwLock<int> foo{ 1234 };

        THEN("The RwLock exposes the assigned value through inline read command")
        {
            REQUIRE(foo.read() == 1234);
        }
        AND_THEN("The RwLock exposes the assigned value through explicit lockguard variable")
        {
            auto val = foo.read();
            REQUIRE(val == 1234);
        }
    }

    GIVEN("A string")
    {
        RwLock<std::string> foo{ "Test string" };

        THEN("The RwLock exposes the assigned value through inline read command")
        {
            REQUIRE(foo.read() == "Test string");
        }
        AND_THEN("The RwLock exposes the assigned value through explicit lockguard variable")
        {
            auto val = foo.read();
            REQUIRE(val == "Test string");
        }
    }

    GIVEN("A pointer")
    {
        float* ptr = new float{ 1234.5678f };
        RwLock<float*> foo{ ptr };

        THEN("The RwLock exposes the assigned value through inline read command")
        {
            REQUIRE(foo.read() == ptr);
            REQUIRE(*foo.read() == 1234.5678f);
        }
        AND_THEN("The RwLock exposes the assigned value through explicit lockguard variable")
        {
            auto val = foo.read();
            REQUIRE(val == ptr);
            REQUIRE(*val == 1234.5678f);
        }
    }
}

SCENARIO("Can acquire multiple concurrent read locks")
{
    GIVEN("A RwLock containing any value")
    {
        RwLock<std::string> a{ "thread-safe content" };

        WHEN("Two read-lockguards are requested")
        {
            auto lock1 = a.try_read();
            auto lock2 = a.try_read();

            THEN("The first lockguard has been acquired and exposes the assigned value")
            {
                REQUIRE(lock1.has_value());
                REQUIRE(lock1 == "thread-safe content");
            }
            AND_THEN("The second lockguard has been acquired and exposes the assigned value")
            {
                REQUIRE(lock2.has_value());
                REQUIRE(lock2 == "thread-safe content");
            }
        }
    }
}

SCENARIO("Can only acquire one concurrent write lock")
{
    GIVEN("A RwLock containing any value")
    {
        RwLock<std::string> a{ "thread-safe content" };

        WHEN("Two write-lockguards are requested")
        {
            auto lock1 = a.try_write();
            auto lock2 = a.try_write();

            THEN("The first lockguard has been acquired and exposes the assigned value")
            {
                REQUIRE(lock1.has_value());
                REQUIRE(lock1 == "thread-safe content");
            }
            AND_THEN("The second lockguard has not been acquired")
            {
                REQUIRE(!lock2.has_value());
            }
        }
    }
}

SCENARIO("Cannot acquire read lock when write lock exists")
{
    GIVEN("A RwLock containing any value")
    {
        RwLock<std::string> a{ "thread-safe content" };

        WHEN("A write-lockguard is requested")
        AND_WHEN("A read-lockguard is requested from the same RwLock")
        {
            auto lock1 = a.try_write();
            auto lock2 = a.try_write();

            THEN("The write-lockguard has been acquired and contains the assigned value")
            {
                REQUIRE(lock1.has_value());
                REQUIRE(lock1 == "thread-safe content");
            }
            AND_THEN("The read-lockguard has not been acquired")
            {
                REQUIRE(!lock2.has_value());
            }
        }
    }
}

SCENARIO("std::shared_ptr can hold a RwLock")
{
    GIVEN("A std::shared_ptr holding a RwLock with any value")
    {
        std::shared_ptr<RwLock<std::string>> sharedFoo{ new RwLock<std::string>{ "Safe shared content" } };

        THEN("We can access the value through the shared_ptr")
        {
            REQUIRE(sharedFoo.get() != nullptr);
            REQUIRE(sharedFoo->read() == "Safe shared content");
        }

        WHEN("The shared_ptr is copied")
        {
            auto sharedBar = sharedFoo;

            THEN("Both copies of the shared_ptr can read the value")
            {
                REQUIRE(sharedFoo.get() != nullptr);
                REQUIRE(sharedFoo->read() == "Safe shared content");
                REQUIRE(sharedBar.get() != nullptr);
                REQUIRE(sharedBar->read() == "Safe shared content");
            }
        }
    }
}
