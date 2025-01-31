#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H


class Log {
public:
    void Show();

    bool showLog = true;
private:
    bool coloredText = true;
    bool autoScroll = true;
};


#endif
