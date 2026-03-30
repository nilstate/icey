#include "icy/base.h"
#include "icy/datetime.h"
#include "icy/logger.h"
#include "icy/platform.h"
#include "icy/sched/scheduler.h"
#include "icy/test.h"

#include <atomic>
#include <memory>
#include <thread>
#include <vector>


using namespace icy;
using namespace icy::test;


namespace {


std::atomic<int> taskRunTimes{0};
std::atomic<bool> blockingTaskEntered{false};
std::atomic<bool> blockingTaskAllowExit{false};
std::atomic<bool> blockingTaskCompleted{false};
std::atomic<bool> blockingTaskDestroyedBeforeCompletion{false};

constexpr long kShortDelayUsec = 100000;
constexpr long kMediumDelayUsec = 300000;


bool waitUntil(const std::function<bool()>& predicate, int timeoutMs = 2000)
{
    for (int waited = 0; waited < timeoutMs; waited += 10) {
        if (predicate())
            return true;
        icy::sleep(10);
    }
    return predicate();
}


void resetBlockingTaskState()
{
    blockingTaskEntered = false;
    blockingTaskAllowExit = false;
    blockingTaskCompleted = false;
    blockingTaskDestroyedBeforeCompletion = false;
}


struct ScheduledTask : public sched::Task
{
    ScheduledTask()
        : sched::Task("ScheduledTask", "ScheduledTask")
    {
    }

    void run() override
    {
        ++taskRunTimes;
    }

    void serialize(json::Value& root) override
    {
        sched::Task::serialize(root);
        root["custom-field"] = "ok";
    }

    void deserialize(json::Value& root) override
    {
        json::assertMember(root, "custom-field");
        sched::Task::deserialize(root);
    }
};


struct BlockingTask : public sched::Task
{
    BlockingTask()
        : sched::Task("BlockingTask", "BlockingTask")
    {
    }

    ~BlockingTask() override
    {
        if (!blockingTaskCompleted.load())
            blockingTaskDestroyedBeforeCompletion = true;
    }

    void run() override
    {
        blockingTaskEntered = true;
        while (!blockingTaskAllowExit.load())
            icy::sleep(10);
        blockingTaskCompleted = true;
    }
};


void registerFixtureTypes()
{
    auto& factory = sched::Scheduler::factory();
    factory.registerTask<ScheduledTask>("ScheduledTask");
    factory.registerTask<BlockingTask>("BlockingTask");
    factory.registerTrigger<sched::OnceOnlyTrigger>("OnceOnlyTrigger");
    factory.registerTrigger<sched::DailyTrigger>("DailyTrigger");
    factory.registerTrigger<sched::IntervalTrigger>("IntervalTrigger");
}


std::unique_ptr<ScheduledTask> onceOnlyTask(const Timespan& delay)
{
    auto task = std::make_unique<ScheduledTask>();
    auto* trigger = task->createTrigger<sched::OnceOnlyTrigger>();
    DateTime when;
    when += delay;
    trigger->scheduleAt = when;
    return task;
}


std::unique_ptr<ScheduledTask> intervalTask(const Timespan& interval, int maxTimes)
{
    auto task = std::make_unique<ScheduledTask>();
    auto* trigger = task->createTrigger<sched::IntervalTrigger>();
    trigger->interval = interval;
    trigger->maxTimes = maxTimes;
    return task;
}


json::Value makeIntervalSnapshot(const Timespan& interval, const DateTime& scheduleAt,
                                 int maxTimes, uint32_t id)
{
    json::Value snapshot = json::Value::array();
    auto task = std::make_unique<ScheduledTask>();
    auto* trigger = task->createTrigger<sched::IntervalTrigger>();
    trigger->interval = interval;
    trigger->maxTimes = maxTimes;
    trigger->createdAt = scheduleAt;
    trigger->scheduleAt = scheduleAt;
    trigger->lastRunAt = scheduleAt;

    task->serialize(snapshot[0]);
    snapshot[0]["id"] = id;
    trigger->serialize(snapshot[0]["trigger"]);
    return snapshot;
}


json::Value makeDailySnapshot(const DateTime& scheduleAt, const DateTime& timeOfDay,
                              const std::vector<sched::DaysOfTheWeek>& exclusions,
                              uint32_t id)
{
    json::Value snapshot = json::Value::array();
    auto task = std::make_unique<ScheduledTask>();
    auto* trigger = task->createTrigger<sched::DailyTrigger>();
    trigger->createdAt = scheduleAt;
    trigger->scheduleAt = scheduleAt;
    trigger->lastRunAt = scheduleAt;
    trigger->timeOfDay = timeOfDay;
    trigger->daysExcluded = exclusions;

    task->serialize(snapshot[0]);
    snapshot[0]["id"] = id;
    trigger->serialize(snapshot[0]["trigger"]);
    return snapshot;
}


} // namespace


int main(int argc, char** argv)
{
    Logger::instance().add(std::make_unique<ConsoleChannel>("debug", Level::Trace));
    test::init();
    registerFixtureTypes();

    describe("schedule assigns scheduler back pointer", []() {
        sched::Scheduler scheduler;

        auto task = onceOnlyTask(Timespan(0, kMediumDelayUsec));
        auto* raw = task.get();
        scheduler.schedule(std::move(task));

        bool hasScheduler = false;
        try {
            hasScheduler = &raw->scheduler() == &scheduler;
        } catch (...) {
            hasScheduler = false;
        }

        expect(hasScheduler);
        scheduler.cancel(raw);
        expect(scheduler.empty());
    });

    describe("once only task serialization round trip", []() {
        sched::Scheduler scheduler;
        json::Value snapshot;

        taskRunTimes = 0;
        scheduler.schedule(onceOnlyTask(Timespan(0, kShortDelayUsec)));
        expect(!scheduler.empty());
        expect(scheduler.size() == 1);

        scheduler.serialize(snapshot);
        icy::sleep(1500);
        expect(taskRunTimes == 1);
        expect(scheduler.empty());

        taskRunTimes = 0;
        DateTime when;
        when += Timespan(0, kShortDelayUsec);
        snapshot[0]["trigger"]["scheduleAt"] =
            DateTimeFormatter::format(when, DateTimeFormat::ISO8601_FORMAT);

        scheduler.deserialize(snapshot);
        expect(scheduler.size() == 1);
        icy::sleep(1500);
        expect(taskRunTimes == 1);
        expect(scheduler.empty());
    });

    describe("interval task waits one interval before first run", []() {
        sched::Scheduler scheduler;

        taskRunTimes = 0;
        scheduler.schedule(intervalTask(Timespan(0, 0, 0, 0, kMediumDelayUsec), 1));
        icy::sleep(150);
        expect(taskRunTimes == 0);
        icy::sleep(400);
        expect(taskRunTimes == 1);
        expect(scheduler.empty());
    });

    describe("deserialize replaces existing schedule", []() {
        sched::Scheduler scheduler;
        json::Value snapshot;

        taskRunTimes = 0;
        scheduler.schedule(intervalTask(Timespan(0, 0, 0, 1, 0), 10));
        expect(scheduler.size() == 1);
        scheduler.serialize(snapshot);

        auto replacement = onceOnlyTask(Timespan(0, kShortDelayUsec));
        replacement->serialize(snapshot[0]);
        replacement->trigger().serialize(snapshot[0]["trigger"]);

        scheduler.deserialize(snapshot);
        expect(scheduler.size() == 1);
        icy::sleep(1500);
        expect(taskRunTimes == 1);
        expect(scheduler.empty());
    });

    describe("interval restore preserves cadence instead of restarting from restore time", []() {
        sched::Scheduler scheduler;
        DateTime now;
        DateTime past = now;
        past -= Timespan(0, 0, 0, 1, 800000); // 1.8s overdue on a 1s interval

        auto snapshot =
            makeIntervalSnapshot(Timespan(0, 0, 0, 1, 0), past, 1, 4242);

        taskRunTimes = 0;
        scheduler.deserialize(snapshot);

        auto* raw = static_cast<sched::Task*>(scheduler.get(4242));
        expect(raw != nullptr);
        expect(raw->remaining() > 0);
        expect(raw->remaining() < 600);

        icy::sleep(900);
        expect(taskRunTimes == 1);
        expect(scheduler.empty());
    });

    describe("daily trigger serializes time of day and excluded days", []() {
        sched::DailyTrigger trigger;
        json::Value data;

        trigger.timeOfDay.assign(2026, 1, 1, 3, 15, 30, 0, 0);
        trigger.daysExcluded = {sched::Saturday, sched::Sunday};
        trigger.serialize(data);

        sched::DailyTrigger restored;
        restored.deserialize(data);

        expect(restored.timeOfDay.hour() == 3);
        expect(restored.timeOfDay.minute() == 15);
        expect(restored.timeOfDay.second() == 30);
        expect(restored.daysExcluded.size() == 2);
        expect(restored.daysExcluded[0] == sched::Saturday);
        expect(restored.daysExcluded[1] == sched::Sunday);
    });

    describe("daily trigger restore skips stale runs and schedules the next valid slot", []() {
        sched::Scheduler scheduler;
        DateTime now;
        DateTime stale = now;
        stale -= Timespan(0, 1, 0, 0, 0);

        auto snapshot = makeDailySnapshot(
            stale,
            DateTime(2026, 1, 1, stale.hour(), stale.minute(), stale.second(), 0, 0),
            {}, 5555);

        taskRunTimes = 0;
        scheduler.deserialize(snapshot);

        auto* raw = static_cast<sched::Task*>(scheduler.get(5555));
        expect(raw != nullptr);
        expect(raw->remaining() > 0);
        icy::sleep(300);
        expect(taskRunTimes == 0);
        scheduler.cancel(raw);
    });

    describe("interval task fires exact max count", []() {
        sched::Scheduler scheduler;

        taskRunTimes = 0;
        scheduler.schedule(intervalTask(Timespan(0, 0, 0, 0, kShortDelayUsec), 3));
        icy::sleep(1000);
        expect(taskRunTimes == 3);
        expect(scheduler.empty());
    });

    describe("cancel removes pending task", []() {
        sched::Scheduler scheduler;

        taskRunTimes = 0;
        auto task = onceOnlyTask(Timespan(0, kShortDelayUsec));
        auto* raw = task.get();
        scheduler.schedule(std::move(task));
        expect(scheduler.size() == 1);

        scheduler.cancel(raw);
        expect(scheduler.empty());
        icy::sleep(1500);
        expect(taskRunTimes == 0);
    });

    describe("clear drops all scheduled work", []() {
        sched::Scheduler scheduler;

        taskRunTimes = 0;
        scheduler.schedule(onceOnlyTask(Timespan(0, kShortDelayUsec)));
        scheduler.schedule(onceOnlyTask(Timespan(0, kShortDelayUsec)));
        expect(scheduler.size() == 2);

        scheduler.clear();
        expect(scheduler.empty());
        icy::sleep(1500);
        expect(taskRunTimes == 0);
    });

    describe("deserialize skips invalid entries", []() {
        sched::Scheduler scheduler;
        json::Value snapshot = json::Value::array();

        auto valid = onceOnlyTask(Timespan(0, kShortDelayUsec));
        valid->serialize(snapshot[0]);
        valid->trigger().serialize(snapshot[0]["trigger"]);

        snapshot[1]["type"] = "MissingTaskType";
        snapshot[1]["name"] = "broken";
        snapshot[1]["trigger"]["type"] = "OnceOnlyTrigger";
        snapshot[1]["trigger"]["scheduleAt"] = snapshot[0]["trigger"]["scheduleAt"];

        scheduler.deserialize(snapshot);
        expect(scheduler.size() == 1);

        taskRunTimes = 0;
        icy::sleep(1500);
        expect(taskRunTimes == 1);
        expect(scheduler.empty());
    });

    describe("scheduler shutdown waits for running task completion", []() {
        resetBlockingTaskState();

        auto scheduler = std::make_unique<sched::Scheduler>();
        auto task = std::make_unique<BlockingTask>();
        task->createTrigger<sched::OnceOnlyTrigger>();
        scheduler->schedule(std::move(task));

        expect(waitUntil([]() { return blockingTaskEntered.load(); }));

        std::thread destroyer(
            [owned = std::move(scheduler)]() mutable { owned.reset(); });

        icy::sleep(100);
        expect(!blockingTaskDestroyedBeforeCompletion.load());
        blockingTaskAllowExit = true;
        destroyer.join();

        expect(blockingTaskCompleted.load());
        expect(!blockingTaskDestroyedBeforeCompletion.load());
    });

    test::runAll();
    Logger::destroy();
    return test::finalize();
}
