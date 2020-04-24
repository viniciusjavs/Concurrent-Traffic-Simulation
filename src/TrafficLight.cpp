#include <chrono>
#include <iostream>
#include <random>
#include "TrafficLight.h"

using namespace std::chrono;

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mutex);

    _cond_var.wait(lock, [this] {
      return !_queue.empty(); // while no messages block!
    });

    // remove last vector element from queue
    T v = std::move(_queue.back());
    _queue.pop_back();

    return v; // return value optimization (RVO)
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    {
      // perform queue modification under the lock
      std::lock_guard<std::mutex> lock(_mutex);
      _queue.push_back(std::move(msg)); // add message to queue
      std::cout << "   Message " << static_cast<int>(msg) << " has been sent to the queue\n";
    }
    _cond_var.notify_one(); // notify client after pushing new T into queue
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() : _currentPhase{TrafficLightPhase::red} {}

void TrafficLight::waitForGreen()
{
    // Wakes up when a new element is available in the message queue
    while (_messages.receive() != TrafficLightPhase::green) {}
}

TrafficLightPhase TrafficLight::getCurrentPhase() const
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    auto start = high_resolution_clock::now();
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(4, 6);
    auto interval = distr(eng); // get a random 'interval' between 4 and 6

    while(true) {
	std::this_thread::sleep_for(milliseconds(1)); // avoid high cpu load
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<seconds>(end - start).count();
	if (duration == interval) {
	    start = end; // reset time
	    interval = distr(eng); // get a new random interval
	    // toggle current phase
	    auto currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
	    _currentPhase = currentPhase;
	    // send message
	    _messages.send(std::move(currentPhase));
	}
    }
}
