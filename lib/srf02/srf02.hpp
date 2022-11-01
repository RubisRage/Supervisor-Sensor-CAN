#include <cstdint>
#include <functional>

class Srf02 {
public:
    Srf02(int8_t address);
    ~Srf02();

    enum Unit {
        inc,
        cm,
        ms
    };

    inline void unit(Unit unit) { unit_ = unit; }
    inline Unit unit(void) { return unit_; }

    enum OperationMode {
        one_shot,
        on_period,
        off
    };

    void operationMode(OperationMode opMode, std::function<void(int16_t)> callback = nullptr);
    

private: 

    Unit unit_;

};