#ifndef JVMEXCEPTION_HXX
#define JVMEXCEPTION_HXX

class JvmException : public std::runtime_error
{
public:
    JvmException();
};

#endif // JVMEXCEPTION_HXX
