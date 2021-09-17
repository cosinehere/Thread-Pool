/**
 * @file ThreadPool11.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-09-17
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#ifndef _THREADPOOL11_HPP_
#define _THREADPOOL11_HPP_

#include <cstdio>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <chrono>

class CPool11 {
public:
    using Task = std::function<void()>;

    CPool11() : _running(false), _threadnum(0) {};
    ~CPool11() { if (_running) { shutdown(); } };

private:
    std::atomic_bool _running;
    int _threadnum;

    std::vector<std::thread> _threads;
    std::queue<Task> _tasks;

    std::mutex _mtx;
    std::condition_variable _cond;

    void routine() {
        while (_running) {
            Task task;
            std::unique_lock<std::mutex> lock(_mtx);
            while (_running && _tasks.empty()) {
                _cond.wait(lock);
            }

            if (_tasks.empty()) {
                continue;
            }

            task = _tasks.front();
            _tasks.pop();
            if (task) {
                task();
            }
        }
    }

public:
    void startup(int num) {
        if (_running) { return; }
        _threadnum = num;
        _running = true;

        for (int i = 0; i < _threadnum; ++i) {
            _threads.emplace_back(std::thread(&CPool11::routine, this));
        }
    }

    void shutdown(bool force = false) {
        if (_running) {
            do {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::unique_lock<std::mutex> lock(_mtx);
                if (_tasks.empty()) {
                    _running = false;
                    _cond.notify_all();
                    break;
                }
            } while (force);

            for (std::thread &th : _threads) {
                if(th.joinable()) {
                    th.join();
                }
            }
        }
    }

    void append(Task task) {
        std::unique_lock<std::mutex> lock(_mtx);
        _tasks.push(task);
        _cond.notify_one();
    }
};

#endif