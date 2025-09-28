#include <string>

namespace lunaria
{
    class Engine 
    {
        public:

            Engine();
            void run();

            bool isClient = true;

            static inline const char gameName[] = "lunaria test game";
    };
}
