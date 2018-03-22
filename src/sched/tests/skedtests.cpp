#include "scy/base.h"
#include "scy/datetime.h"
#include "scy/logger.h"
#include "scy/sched/scheduler.h"
#include "scy/test.h"


using namespace std;
using namespace scy;
using namespace scy::test;


static sched::Scheduler scheduler;
static int taskRunTimes = 0;


// =============================================================================
// Test Scheduled Task
//
struct ScheduledTask : public sched::Task
{
    ScheduledTask()
        : sched::Task("ScheduledTask")
    {
    }

    virtual ~ScheduledTask() {}

    void run() { taskRunTimes++; }

    void serialize(json::value& root)
    {
        sched::Task::serialize(root);

        root["CustomField"] = "blah";
    }

    void deserialize(json::value& root)
    {
        json::assertMember(root, "CustomField");

        sched::Task::deserialize(root);
    }
};


int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", Level::Trace));
    test::init();

    // Register tasks and triggers
    scheduler.factory().registerTask<ScheduledTask>("ScheduledTask");
    scheduler.factory().registerTrigger<sched::OnceOnlyTrigger>("OnceOnlyTrigger");
    scheduler.factory().registerTrigger<sched::DailyTrigger>("DailyTrigger");
    scheduler.factory().registerTrigger<sched::IntervalTrigger>("IntervalTrigger");

    describe("once only task serialization", []() {
        json::value json;
        Timespan hundredMs(0, 1);

        // Schedule a once only task to run in 100ms time.
        {
            taskRunTimes = 0;

            auto task = new ScheduledTask();
            auto trigger = task->createTrigger<sched::OnceOnlyTrigger>();

            DateTime dt;
            dt += hundredMs;
            trigger->scheduleAt = dt;

            scheduler.start(task);

            // Serialize the task
            scheduler.serialize(json);

            // Wait for the task to complete
            scy::sleep(1500);
            expect(taskRunTimes == 1);
        }

        // Deserialize the previous task from JSON and run it again
        {
            taskRunTimes = 0;

            // Set the task to run in 100ms time
            {
                DateTime dt;
                dt += hundredMs;
                json[(int)0]["trigger"]["scheduleAt"] =
                    DateTimeFormatter::format(dt, DateTimeFormat::ISO8601_FORMAT);
            }

            // Dynamically create the task from JSON
            LDebug("Sched Input JSON:\n", json.dump(4))
            scheduler.deserialize(json);

            // Print to cout
            // LDebug("##### Sched Print Output:")
            // scheduler.print(cout);
            // LDebug("##### Sched Print Output END")

            // Output scheduler tasks as JSON before run
            json::value before;
            scheduler.serialize(before);
            SDebug << "Sched Output JSON Before Run:\n"
                   << before.dump(4) << endl;

            // Wait for the task to complete
            scy::sleep(1500);
            expect(taskRunTimes == 1);

            // Output scheduler tasks as JSON after run
            json::value after;
            scheduler.serialize(after);
            SDebug << "Sched Output JSON After Run:\n"
                   << json.dump(4) << endl;
        }
    });

    describe("interval task", []() {
        LDebug("Running Scheduled Task Test")

        taskRunTimes = 0;

        // Schedule an interval task to run 3 times at 100ms intervals
        {
            auto task = new ScheduledTask();
            sched::IntervalTrigger* trigger =
                task->createTrigger<sched::IntervalTrigger>();

            trigger->interval = Timespan(0, 1);
            trigger->maxTimes = 3;

            scheduler.start(task);

            // Print to cout
            // LDebug("##### Sched Print Output:")
            // scheduler.print(cout);
            // LDebug("##### Sched Print Output END")

            // Wait for the task to complete
            scy::sleep(1000);
            expect(taskRunTimes == 3);
        }

        LDebug("Running Scheduled Task Test: END")
    });

    // // Schedule to fire once now, and in two days time.
    // {
    //     auto task = new ScheduledTask();
    //     auto trigger = task->createTrigger<DailyTrigger>();
    //
    //     // 2 secs from now
    //     DateTime dt;
    //     Timespan ts(2, 0);
    //     dt += ts;
    //     trigger->timeOfDay = dt;
    //
    //     // skip tomorrow
    //     trigger->daysExcluded.push_back((DaysOfTheWeek)(dt.dayOfWeek() + 1));
    //
    //     scheduler.schedule(task);
    //
    //     // TODO: Assert running date
    //     expect(task->trigger().remaining() == 1);
    //     expect(task->trigger().timesRun == 1);
    // }

    test::runAll();

    Logger::destroy();

    return test::finalize();
}
