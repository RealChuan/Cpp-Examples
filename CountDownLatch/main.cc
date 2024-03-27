#include "countdownlatch.hpp"

#include <iostream>
#include <thread>

struct Job
{
    const std::string name;
    std::string product{"not worked"};
    std::thread action{};
};

void countDownLatchTest()
{
    Job jobs[]{{"Annika"}, {"Buru"}, {"Chuck"}};

    CountDownLatch work_done{std::size(jobs)};
    CountDownLatch start_clean_up{1};

    auto work = [&](Job &my_job) {
        my_job.product = my_job.name + " worked";
        work_done.countDown();
        start_clean_up.wait();
        my_job.product = my_job.name + " cleaned";
    };

    std::cout << "Work is starting... ";
    for (auto &job : jobs) {
        job.action = std::thread{work, std::ref(job)};
    }

    work_done.wait();
    std::cout << "done:\n";
    for (auto const &job : jobs) {
        std::cout << "  " << job.product << '\n';
    }

    std::cout << "Workers are cleaning up... ";
    start_clean_up.countDown();
    for (auto &job : jobs) {
        job.action.join();
    }

    std::cout << "done:\n";
    for (auto const &job : jobs) {
        std::cout << "  " << job.product << '\n';
    }
}

auto main() -> int
{
    countDownLatchTest();

    return 0;
}
