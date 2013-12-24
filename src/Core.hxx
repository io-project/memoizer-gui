#ifndef CORE_HXX
#define CORE_HXX

class Core
{
public:
    Core();

    int run(int argc,char*argv[]);

private:
    void registerMetaTypes();
};

#endif // CORE_HXX
