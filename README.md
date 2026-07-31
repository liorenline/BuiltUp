# ⚡ BuiltUp


A lightweight C++ console utility that monitors system resources in real time — CPU load, RAM usage, and disk space. Metrics are logged to a JSON file, and alerts are triggered when thresholds are exceeded.

Built for sysadmins and DevOps engineers who need a simple, dependency-free monitoring tool.

---

## 🔍 What it does

- Reads CPU, RAM, and disk metrics from the system continuously
- Logs all measurements to a JSON file with precise timestamps
- Prints `WARNING` alerts to the console when any metric exceeds a configured threshold
- Runs as an infinite loop — just start it and forget it

**Why it matters:**
- **Diagnose outages** — the log shows the exact moment something spiked before a service went down
- **Prevent crashes** — get warned at 85% RAM before the server actually falls over
- **Spot patterns** — accumulate data over time to see recurring load spikes (e.g. every day at 14:00)
- **Plan capacity** — if RAM is consistently at 90%, the log makes that obvious at a glance

---

## 🛠 Tech Stack

- **Language:** C++
- **Build system:** CMake

---

## 🚀 Building from source

### Prerequisites

- CMake ≥ 3.15
- C++17 compatible compiler (GCC, Clang, MSVC)

### Build steps

```bash
git clone https://github.com/liorenline/BuiltUp.git
cd BuiltUp
mkdir build && cd build
cmake ..
cmake --build .
./builtup
```

---

## 📁 Project Structure

```
BuiltUp/
├── cpu.cpp / cpu.hpp        # CPU usage reader
├── mem.cpp / mem.h          # RAM usage reader
├── disk.cpp / disk.h        # Disk usage reader
├── Logger.cpp / Logger.h    # JSON log writer
├── AlertManager.cpp / .h    # Threshold checker & warnings
├── main.cpp                 # Entry point & main loop
├── CMakeLists.txt           # Build configuration
└── Dockerfile               # Container build
```

---

## 📊 Output example

```json
{
  "timestamp": "2025-05-20T14:03:21",
  "cpu_percent": 78.4,
  "ram_percent": 61.2,
  "disk_percent": 45.0
}
```
