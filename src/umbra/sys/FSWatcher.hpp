#pragma once

#include "spdlog/spdlog.h"
#include <chrono>
#include <vector>
#include <filesystem>

#include <poll.h>
#include <sys/inotify.h>
#include <unistd.h>

namespace umbra {

enum class EventKind {
    FileAdded,
    FileEdited,
    FileRemoved,
    DirectoryAdded,
    DirectoryRemoved,
};

struct WatcherConfig {
    std::chrono::milliseconds commandDelayTimeout{0};
};

struct Event {
    EventKind kind;
    std::filesystem::path eventPath;

    std::chrono::steady_clock::time_point ts;
};

/**
 * \brief Utility class for watching for filesystem changes on Linux through inotify
 *
 * \note This class is a candidate for merging into stc. It must be kept free of umbra deps
 */
class FSWatcher {
private:
    int fd;
    WatcherConfig conf;

    std::unordered_map<int, std::filesystem::path> watchDescriptors;
public:
    FSWatcher(
        const std::vector<std::filesystem::path>& initialWatches,
        const WatcherConfig& conf = {}
    ): conf(conf) {
        fd = inotify_init1(IN_NONBLOCK);

        for (auto& path : initialWatches) {
            watch(std::filesystem::canonical(path));

            if (std::filesystem::is_directory(path)) {
                for (
                    auto& f : std::filesystem::recursive_directory_iterator(
                        std::filesystem::canonical(path)
                    )
                ) {
                    auto abs = std::filesystem::canonical(f);
                    if (std::filesystem::is_directory(abs)) {
                        watch(abs);
                    }
                }
            }
        }
    }

    ~FSWatcher() {
        close(fd);
    }

    void block(const std::function<bool(EventKind event, const std::filesystem::path& path)>& onEvent) {
        // TODO: add event_fd (similar to how raven is set up for termination) so this can be automatically tested
        std::array<pollfd, 1> fds = {
            pollfd {
                .fd = this->fd,
                .events = POLLIN,
                .revents = 0
            }
        };
        std::array<char, 4096> buff;
        std::chrono::steady_clock::time_point ts = std::chrono::steady_clock::now();
        while (true) {
            ssize_t events = poll(
                fds.data(),
                fds.size(),
                -1
            );

            if (events == -1) {
                if (errno == EINTR) {
                    continue;
                }
                perror("poll");
                throw std::runtime_error("error while polling for events");
            }

            if (events > 0) {
                // TODO: This is not optimal, but not sure what else to do. The events do not carry timestamp
                // information, so this appears to be necessary to allow timeout logic
                std::vector<Event> events;
                if ((fds[0].revents & POLLIN) != 0) {
                    // inotify ready
                    spdlog::debug("Has events");
                    while (true) { // Read loop
                        ssize_t size = read(
                            fd,
                            buff.data(),
                            buff.size()
                        );
                        if (size == -1 && errno != EAGAIN) {
                            perror("read");
                            throw std::runtime_error("Failed to read inotify events");
                        } else if (size == -1 && errno == EAGAIN) {
                            // Events exhausted
                            break;
                        }

                        const inotify_event* event;
                        for (
                            char *ptr = buff.data();
                            ptr < buff.data() + size;
                            ptr += sizeof(struct inotify_event) + event->len
                        ) {
                            event = (const inotify_event *) ptr;

                            // Guard against edge-case errors where folders are created and deleted rapidly, for example
                            // during tests
                            auto parentIt = this->watchDescriptors.find(event->wd);
                            if (parentIt == this->watchDescriptors.end()) {
                                continue;
                            }
                            std::filesystem::path eventPath = parentIt->second;
                            if (event->len) {
                                eventPath /= event->name;
                            }

                            EventKind kind;
                            if ((event->mask & IN_ISDIR) != 0) {
                                // Directory
                                if ((event->mask & IN_CREATE) != 0) {
                                    kind = EventKind::DirectoryAdded;

                                    watch(eventPath);
                                } else if ((event->mask & IN_DELETE) != 0) {
                                    kind = EventKind::DirectoryRemoved;

                                    // deregister watchDescriptors
                                    this->watchDescriptors.erase(
                                        this->watchDescriptors.find(event->wd)
                                    );
                                } else {
                                    continue;
                                }
                            } else {
                                // File
                                if ((event->mask & IN_CLOSE_WRITE) != 0) {
                                    kind = EventKind::FileEdited;
                                } else if ((event->mask & IN_DELETE) != 0) {
                                    kind = EventKind::FileRemoved;
                                } else if ((event->mask & IN_CREATE) != 0) {
                                    kind = EventKind::FileAdded;
                                } else {
                                    continue;
                                }
                            }

                            spdlog::debug("EventKind = {} on {}", static_cast<int>(kind), eventPath.string());
                            events.push_back({
                                    kind,
                                    eventPath,
                                    std::chrono::steady_clock::now(),
                            });
                        }
                    }
                }

                for (auto& event : events) {
                    if (event.ts < ts + conf.commandDelayTimeout) {
                        continue;
                    }
                    ts = event.ts;
                    if (!onEvent(event.kind, event.eventPath)) {
                        goto done;
                    }
                }

                // Reset the time so the timer also applies between loops if `onEvent` takes a while:tm:
                if (events.size()) {
                    ts = std::chrono::steady_clock::now();
                }
            }
        }

    done:
    }

    void watch(const std::filesystem::path& path) {
        spdlog::debug("Watching {}", path.string());
        auto wd = inotify_add_watch(
            fd,
            path.c_str(),
            IN_CLOSE_WRITE | IN_CREATE | IN_DELETE
        );

        if (wd < 0) {
            spdlog::error("Failed to watch {}", path.string());
            return;
        }
        this->watchDescriptors[wd] = path;
    }
};

}
