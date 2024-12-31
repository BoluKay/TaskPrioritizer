#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

struct Task {
    std::string name = "";    // Default to an empty string
    int priority = 0;         // Default priority
    int duration = 0;         // Default duration in minutes
    time_t deadline = 0;      // Default to epoch time (January 1, 1970)

    double calculatePriorityScore() const {
        double urgency = difftime(deadline, time(0)) / 3600.0; // Hours until deadline
        return priority * 10 - urgency - duration / 60.0;
    }

    std::string formatDeadline() const {
        char buffer[20];
        struct tm tm_info;
        localtime_s(&tm_info, &deadline); // Use localtime_s instead of localtime
        strftime(buffer, 20, "%Y-%m-%d %H:%M", &tm_info);
        return buffer;
    }

};

class TaskManager {
private:
    std::vector<Task> tasks;

    void saveToFile() {
        std::ofstream file("tasks.txt");
        for (const auto& task : tasks) {
            file << task.name << "|" << task.priority << "|" << task.duration << "|" << task.deadline << "\n";
        }
        file.close();
    }

    void loadFromFile() {
        std::ifstream file("tasks.txt");
        if (!file.is_open()) return;

        Task task;
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = 0;
            task.name = line.substr(0, pos = line.find('|'));
            task.priority = std::stoi(line.substr(pos + 1, pos = line.find('|', pos + 1)));
            task.duration = std::stoi(line.substr(pos + 1, pos = line.find('|', pos + 1)));
            task.deadline = std::stoll(line.substr(pos + 1));
            tasks.push_back(task);
        }
        file.close();
    }

public:
    TaskManager() {
        loadFromFile();
    }

    ~TaskManager() {
        saveToFile();
    }

    void addTask() {
        Task task;
        std::cout << "Enter task name: ";
        std::cin.ignore();
        std::getline(std::cin, task.name);

        std::cout << "Enter priority (1=Low, 2=Medium, 3=High): ";
        std::cin >> task.priority;

        std::cout << "Enter estimated duration (minutes): ";
        std::cin >> task.duration;

        int year, month, day, hour, minute;
        std::cout << "Enter deadline (YYYY MM DD HH MM): ";
        std::cin >> year >> month >> day >> hour >> minute;
        struct tm tm = { 0 };
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        task.deadline = mktime(&tm);

        tasks.push_back(task);
        std::cout << "Task added successfully!\n";
    }

    void viewTasks() {
        if (tasks.empty()) {
            std::cout << "No tasks available.\n";
            return;
        }

        std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.calculatePriorityScore() > b.calculatePriorityScore();
            });

        std::cout << std::left << std::setw(20) << "Name"
            << std::setw(10) << "Priority"
            << std::setw(15) << "Duration (min)"
            << std::setw(20) << "Deadline"
            << "Score\n";
        std::cout << std::string(75, '-') << "\n";
        for (const auto& task : tasks) {
            std::cout << std::left << std::setw(20) << task.name
                << std::setw(10) << task.priority
                << std::setw(15) << task.duration
                << std::setw(20) << task.formatDeadline()
                << task.calculatePriorityScore() << "\n";
        }
    }

    void deleteTask() {
        std::string name;
        std::cout << "Enter task name to delete: ";
        std::cin.ignore();
        std::getline(std::cin, name);

        auto it = std::remove_if(tasks.begin(), tasks.end(), [&](const Task& task) {
            return task.name == name;
            });

        if (it != tasks.end()) {
            tasks.erase(it, tasks.end());
            std::cout << "Task deleted successfully!\n";
        }
        else {
            std::cout << "Task not found!\n";
        }
    }

    void showStatistics() {
        std::cout << "Total tasks: " << tasks.size() << "\n";
        int overdueCount = 0;
        for (const auto& task : tasks) {
            if (difftime(task.deadline, time(0)) < 0) {
                overdueCount++;
            }
        }
        std::cout << "Overdue tasks: " << overdueCount << "\n";
    }
};

int main() {
    TaskManager manager;
    int choice;

    do {
        std::cout << "\n--- Intelligent Task Prioritizer ---\n";
        std::cout << "1. Add Task\n";
        std::cout << "2. View Tasks\n";
        std::cout << "3. Delete Task\n";
        std::cout << "4. Show Statistics\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            manager.addTask();
            break;
        case 2:
            manager.viewTasks();
            break;
        case 3:
            manager.deleteTask();
            break;
        case 4:
            manager.showStatistics();
            break;
        case 5:
            std::cout << "Goodbye!\n";
            break;
        default:
            std::cout << "Invalid choice, please try again.\n";
        }
    } while (choice != 5);

    return 0;
}
