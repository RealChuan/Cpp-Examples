#include "atomicqueue.hpp"

int main()
{
    AtomicQueue queue;
    vector<thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.push_back(thread([&queue, i]() {
            for (int j = 0; j < 100; ++j) {
                queue.push(i * 100 + j);
            }
        }));
    }
    for (auto &t : threads) {
        t.join();
    }
    int data;
    while (queue.pop(data)) {
        cout << data << endl;
    }
    return 0;
}