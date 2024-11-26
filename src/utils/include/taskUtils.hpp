#ifndef TASK_UTILS
#define TASK_UTILS

template<class Callable>
AsyncTask *make_task(Callable callable, const std::string &name, int sort = 0, int priority = 0) {
    class InlineTask final : public AsyncTask {
    public:
        InlineTask(Callable callable, const std::string &name, int sort, int priority) :
                AsyncTask(name),
                _callable(std::move(callable)) {
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

#endif // TASK_UTILS
