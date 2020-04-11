#include <atomic>
#include <chrono>
#include <complex>
#include <iostream>
#include <thread>
#include <vector>

size_t constexpr NR_VALUES = 1000 * 1000;
int constexpr NR_THREADS = 8;
int constexpr ITERATIONS = 400;

std::vector<std::complex<double>> positions(NR_VALUES);

double iterate(std::complex<double> position) {
  for (int i = 0; i < ITERATIONS; ++i) {
    position = position * position;
    if (std::abs(position) > 2) {
      position = std::pow(position, 1 / 4);
    }
  }

  return std::abs(position);
}

auto concurrent(std::vector<double> &buffer) {
  std::atomic_int i(0);

  auto work = [&]() {
    int j;
    while ((j = i++) < NR_VALUES) {
      buffer[j] = iterate(positions[j]);
    }
  };

  auto clock = std::chrono::steady_clock();
  auto timePoint = clock.now();

  std::thread threads[NR_THREADS];
  for (auto &&thread : threads)
    thread = std::thread(work);

  for (auto &&thread : threads)
    thread.join();

  return clock.now() - timePoint;
}

auto parallel(std::vector<double> &buffer) {
  int const blockSize = NR_VALUES / NR_THREADS;
  int const remainder = NR_VALUES % NR_THREADS;

  auto work = [&](int const startPosition) {
    for (int i = startPosition; i < (startPosition + blockSize); i++) {
      buffer[i] = iterate(positions[i]);
    }
  };

  std::thread threads[NR_THREADS];
  int currentPosition = 0;

  auto clock = std::chrono::steady_clock();
  auto timePoint = clock.now();

  threads[0] = std::thread(work, currentPosition);
  currentPosition += blockSize + remainder;

  for (int i = 1; i < NR_THREADS; i++) {
    threads[i] = std::thread(work, currentPosition);
    currentPosition += blockSize;
  }

  if (currentPosition != NR_VALUES) {
    std::cerr << currentPosition << std::endl;
    throw;
  }

  for (auto &&thread : threads)
    thread.join();

  return clock.now() - timePoint;
}

int main() {
  for (int i = 0; i < ITERATIONS; ++i) {
    positions[i] = std::complex<double>(std::rand() / (float)RAND_MAX,
                                        std::rand() / (float)RAND_MAX);
  }
  std::vector<double> concurrentBuffer(NR_VALUES);
  auto concurrentTime = concurrent(concurrentBuffer);

  std::vector<double> parallelBuffer(NR_VALUES);
  auto parallelTime = parallel(parallelBuffer);

  if (concurrentBuffer != parallelBuffer)
    std::cerr << "Not equal";

  std::cout << "Conc: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   concurrentTime)
                   .count()
            << std::endl;

  std::cout << "Para: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                   parallelTime)
                   .count()
            << std::endl;
}
