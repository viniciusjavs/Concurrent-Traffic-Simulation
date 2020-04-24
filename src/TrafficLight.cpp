#include <iostream>
#include <random>
#include "TrafficLight.h"

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
    }
    _cond_var.notify_one(); // notify client after pushing new T into queue
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() : _currentPhase{TrafficLightPhase::red} {}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase() const
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
}
