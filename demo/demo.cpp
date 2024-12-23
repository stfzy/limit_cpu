// demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <thread>

void ThreadRoutine() {
  while (true) {
    std::cout << std::this_thread::get_id() << std::endl;
  }
}

int main() {
  // 创建8个线程去占用cpu
  for (int i = 0; i < 32; i++) {
    std::thread(ThreadRoutine).detach();
  }

  std::cout << "Running 2 thread...." << std::endl;

  getchar();
  return 0;
}
