#ifndef TASK_UTILS
#define TASK_UTILS

#include <asyncTask.h>
#include <asyncTaskManager.h>
#include <memory>

// Helper function to create an inline task
template<class Callable>
AsyncTask* make_task(Callable callable, const std::string& name, int sort = 0, int priority = 0) {
    class InlineTask final : public AsyncTask {
    public:
        InlineTask(Callable callable, const std::string& name, int sort, int priority) :
                AsyncTask(name), _callable(std::move(callable)) {
            _sort = sort;
            _priority = priority;
        }

        ALLOC_DELETED_CHAIN(InlineTask);

    private:
        virtual DoneStatus do_task() override final {
            return _callable(this);
        }

        Callable _callable;
    };
    return new InlineTask(std::move(callable), name, sort, priority);
}

// Utility Function to create and add a task in one step
template<class Callable>
void add_task(Callable callable, const std::string& name, int sort = 0, int priority = 0) {
    AsyncTaskManager::get_global_ptr()->add(make_task(std::move(callable), name, sort, priority));
}

#endif // TASK_UTILS
