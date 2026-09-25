/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2005, 2006, 2007 Eric Ehlers

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - https://www.quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <https://www.quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#if defined(HAVE_CONFIG_H)
    #include <oh/config.hpp>
#endif

#include <oh/logger.hpp>
#include <oh/exception.hpp>
#include <boost/filesystem.hpp>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace ObjectHandler {

    namespace {
        // Map numeric level to a short label for the log line prefix.
        const char* levelLabel(int level) {
            switch (level) {
                case 1: return "FATAL";
                case 2: return "ERROR";
                case 3: return "WARN ";
                case 4: return "INFO ";
                case 5: return "DEBUG";
                default: return "INFO ";
            }
        }

        std::string timestamp() {
            std::time_t t = std::time(nullptr);
            std::tm tm = {};
#ifdef _WIN32
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif
            std::ostringstream ss;
            ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            return ss.str();
        }
    }

    Logger::Logger() : level_(0) {}

    Logger::~Logger() {
        if (file_.is_open())
            file_.close();
    }

    void Logger::setFile(const std::string &logFileName,
                         const int &logLevel) {
        boost::filesystem::path path(logFileName);
        if (!path.parent_path().empty()) {
            OH_REQUIRE(boost::filesystem::exists(path.parent_path()),
                       "Invalid parent path : " << logFileName);
        }

        if (file_.is_open())
            file_.close();

        file_.open(logFileName, std::ios::app);
        OH_REQUIRE(file_.is_open(),
                   "Logger::setFile: unable to open log file: " << logFileName);

        filename_ = logFileName;
        setLevel(logLevel);
    }

    void Logger::setConsole(const int &console,
                            const int &logLevel) {
        console_ = (console != 0);
        setLevel(logLevel);
    }

    void Logger::setLevel(const int &logLevel) {
        OH_REQUIRE(logLevel >= 0 && logLevel <= 5,
                   "Logger::setLevel: invalid logLevel: " << logLevel);
        level_ = logLevel;
    }

    void Logger::writeMessage(const std::string &message,
                              const int &level) {
        try {
            if (level_ == 0) return;
            if (level > level_) return;

            std::string line = "[" + timestamp() + "] "
                             + levelLabel(level) + " " + message + "\n";

            if (file_.is_open())
                file_ << line << std::flush;

            if (console_)
                std::cout << line << std::flush;

        } catch (...) {}
    }

    const std::string Logger::file() const {
        return filename_;
    }

    const int Logger::level() const {
        return level_;
    }

}
