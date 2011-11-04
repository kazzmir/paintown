#ifndef test_init_h
#define test_init_h
namespace Screen{
    void fakeInit(int x=640, int y=480);
    void fakeFinish();
    void realInit(int x=640, int y=480);
    void realFinish();
    void change(int x, int y);
}
#endif