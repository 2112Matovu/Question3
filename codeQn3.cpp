#include <Arduino.h>

#define BUFFER_SIZE 64

// TODO: Implement this templated CircularBuffer class
template <typename T>
class CircularBuffer {
private:
  T buffer[BUFFER_SIZE];
  int head;   // Points to the next location to write to
  int tail;   // Points to the next location to read from
  int count;  // Number of elements currently in the buffer

public:
  CircularBuffer() : head(0), tail(0), count(0) {}

  // TODO: Add an item to the buffer. Return true if successful.
  bool enqueue(T item) {
    if (isFull()) {
      return false;
    }
    buffer[head] = item;
    head = (head + 1) % BUFFER_SIZE;
    count++;
    return true;
  }

  // TODO: Remove an item and store it in 'item'. Return true if successful.
  bool dequeue(T& item) {
    if (isEmpty()) {
      return false;
    }
    item = buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    count--;
    return true;
  }

  bool isEmpty() {
    return count == 0;
  }

  bool isFull() {
    return count == BUFFER_SIZE;
  }
};

CircularBuffer<int> sensorDataBuffer;
unsigned long lastProducerTime = 0;
unsigned long lastConsumerTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Starting Circular Buffer Demo...");
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Producer: Simulate a high-speed sensor (1000 Hz -> every 1 ms) ---
  if (currentMillis - lastProducerTime >= 1) {
    lastProducerTime = currentMillis;
    int simulatedSensorValue = random(0, 1000);

    // TODO: Try to add the sensor value to the buffer.
    // If the buffer is full, print "Buffer Overflow!"
    if (!sensorDataBuffer.enqueue(simulatedSensorValue)) {
      Serial.println("Buffer Overflow!");
    }
  }

  // --- Consumer: Simulate a slow processing loop (10 Hz -> every 100 ms) ---
  if (currentMillis - lastConsumerTime >= 100) {
    lastConsumerTime = currentMillis;
    int processedValue = 0;
    int itemsProcessedThisCycle = 0;

    // TODO: Process all available data in the buffer
    // While the buffer is not empty, dequeue an item and print it.
    while (!sensorDataBuffer.isEmpty()) {
      if (sensorDataBuffer.dequeue(processedValue)) {
        Serial.print("Processed: ");
        Serial.println(processedValue);
        itemsProcessedThisCycle++;
      }
    }

    Serial.print("--- Processed ");
    Serial.print(itemsProcessedThisCycle);
    Serial.println(" items this cycle ---");
  }
}

/*
ANALYSIS:

Producer rate = 1000 Hz (1 ms interval)
Consumer rate = 10 Hz (100 ms interval)

100 samples are produced before each consumer cycle.

enqueue()  -> O(1)
dequeue()  -> O(1)

Circular buffer prevents data loss by decoupling
fast producer from slow consumer.

Overflow occurs when producer rate > consumer rate.

https://wokwi.com/projects/457541789679648769

*/