#pragma once
#include <syncstream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <thread>
#include <filesystem>
#include <sstream>
using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    auto GetFileTimeStamp() const
    {
        const auto t_c = std::chrono::system_clock::to_time_t(GetTime());
        return std::put_time(std::localtime(&t_c), "%Y_%m_%d");
    }

    Logger() = default;
    Logger(const Logger&) = delete;

public:
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    // Выведите в поток все аргументы.
    template<class... Ts>
    void Log(const Ts&... args)
    {
        std::osyncstream sync_stream(log_file_);
        sync_stream << GetTimeStamp() <<":" << " ";
        ((sync_stream<< args),...);//cpp17 свертки
        sync_stream << std::endl;

    }

    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts)
    {
        std::lock_guard lockGuard(this->m_);
        manual_ts_ = ts;
        auto new_ts = GetFileTimeStamp();

        std::stringstream ss;
        ss <<"/var/log/" <<  "sample_log_" << new_ts <<".log";
        if (!log_file_.is_open())
        {
            filepath = ss.str();
            log_file_.open(filepath);
            return;
        } else
        {
            std::filesystem::path new_filepath = ss.str();
            if ( filepath == new_filepath)
            {
                return;
            }
            filepath = new_filepath;
            log_file_.close();
            log_file_.open(filepath);
        }


    }



private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    std::filesystem::path filepath;
    std::mutex m_;
    std::ofstream log_file_;

};
