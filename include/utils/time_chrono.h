#ifndef TIME_CHRONO
#define TIME_CHRONO
#include <functional>
#include <chrono>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
void WithTime(function<void()> func, const char* PreInfo);
class TimeKeeper{
    public:
        TimeKeeper(); // 构造函数
        ~TimeKeeper(); // 析构函数

        void Init(); // 初始化计时器
        void dur() const; // 输出执行时间
        void dur_withPre(const char*);
        void dur_withPre_Init(const char*);
    private:
        std::chrono::high_resolution_clock::time_point startTime; // 存储开始时间
};

class Watcher {
public:
    const int default_MaxDeep = 20;
    Watcher();
    Watcher(int);

    int GetDeep();

    int levelup(const char* );

    int leveldown();

private:
    std::chrono::high_resolution_clock::time_point *Time_Start;
    int Deep,DeepMaxLen;
    const char* *Level_Scripts;
};
#endif // !TIME_CHRONO
