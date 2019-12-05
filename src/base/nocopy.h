#ifndef NOCOPY_H_
#define NOCOPY_H_

namespace ws{

    class Nocopy{
        public:
            Nocopy(const Nocopy&) = delete;
            Nocopy &operator=(const Nocopy& para) = delete;
        protected:
            constexpr Nocopy() = default;
            virtual ~Nocopy() = default;
    };
}

#endif 