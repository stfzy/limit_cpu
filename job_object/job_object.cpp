#include <iostream>

#include <windows.h>

int main() {
  std::cout << "请输入要限制CPU的进程id:" << std::endl;
  uint32_t pid;
  std::cin >> pid;

  auto job_handle = CreateJobObjectW(nullptr, L"32B87E24-DDE6-4DE5-BBE8-7DC9168A786A");
  if (nullptr == job_handle) {
    std::cout << "CreateJobObjectW failed with code " << GetLastError() << std::endl;
    return 0;
  }

  // 设置CPU占用
  JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cpu_rate_control_info{0};
  cpu_rate_control_info.ControlFlags = JOB_OBJECT_CPU_RATE_CONTROL_ENABLE | JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP;
  cpu_rate_control_info.CpuRate = 1 * 100;  // 限制为1%

  auto ret = SetInformationJobObject(job_handle, JobObjectCpuRateControlInformation, &cpu_rate_control_info, sizeof(JOBOBJECT_CPU_RATE_CONTROL_INFORMATION));
  if (0 == ret) {
    std::cout << "SetInformationJobObject for JobObjectCpuRateControlInformation failed with code " << ret << " and last error " << GetLastError() << std::endl;
    CloseHandle(job_handle);
    return 0;
  }

  // 设置为 job句柄关闭后，绑定在此job上的进程全部退出
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION ext_limit_info{0};
  ext_limit_info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

  ret = SetInformationJobObject(job_handle, JobObjectExtendedLimitInformation, &ext_limit_info, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));
  if (0 == ret) {
    std::cout << "SetInformationJobObject for JobObjectBasicLimitInformation failed with code " << ret << " and last error " << GetLastError() << std::endl;
    CloseHandle(job_handle);
    return 0;
  }

  auto proc_handle = OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, FALSE, pid);
  if (!proc_handle) {
    std::cout << "OpenProcess failed with code " << GetLastError() << std::endl;
    CloseHandle(job_handle);
    return 0;
  }

  if (0 == AssignProcessToJobObject(job_handle, proc_handle)) {
    std::cout << "AssignProcessToJobObject failed with code " << GetLastError() << std::endl;
    CloseHandle(job_handle);
    CloseHandle(proc_handle);
    return 0;
  }

  std::cout << "Control process cpu success!" << std::endl;

  system("pause");

  CloseHandle(job_handle);
  CloseHandle(proc_handle);
  return 0;
}
