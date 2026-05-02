#include "log.h" 

int main() { 
    Log *log = Log::Instance(); 
    log->message(LOG_NORMAL, "program loaded");
    log->message(LOG_WARNING, "warning");

    int count = 10;
    while (count--){
        log->message(LOG_NORMAL, std::to_string(count));
    }

    log->message(LOG_ERROR, "error happens! help me!"); 
    
    log->print(); 

    return 0;
}
