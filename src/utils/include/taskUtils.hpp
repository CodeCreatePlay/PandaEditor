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

// Utility function to create and add a task in one step
template<class Callable>
void add_task(Callable callable, const std::string& name, int sort = 0, int priority = 0) {
	if (!has_task(name))
		AsyncTaskManager::get_global_ptr()->add(make_task(std::move(callable), name, sort, priority));
	else {
		std::cout << "Task: " << name << " already exists." << std::endl;
	}
}

// Check if task exists
inline bool has_task(AsyncTask* task) {
    auto task_mgr = AsyncTaskManager::get_global_ptr();
    bool task_ = task_mgr->has_task(task);
    if (task_) return true;
	return false;
}

// Check if task exists
inline AsyncTask* has_task(const std::string name) {
    auto task_mgr = AsyncTaskManager::get_global_ptr();
    PT(AsyncTask) task = task_mgr->find_task(name);
    if (!task) return nullptr;
	return task;
}

// Utility function to remove a task by name
inline void remove_task(const std::string& name) {
    auto task_mgr = AsyncTaskManager::get_global_ptr();
    PT(AsyncTask) task = task_mgr->find_task(name);
    if (has_task(name)) {
		std::cout << "removed task: " << name << std::endl;
        task_mgr->remove(task);
    }
}

// Utility function to remove a task by its pointer
inline void remove_task(AsyncTask* task) {
    if (has_task(task)) {
        AsyncTaskManager::get_global_ptr()->remove(task);
    }
}

#endif // TASK_UTILS
