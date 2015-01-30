#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H
#include <QException>

class MyException : public QException
{
public:
    MyException(std::exception& err) : e(err),str_what(err.what()) {}
    //MyException(const std::exception& err) : e(err) {}
    void raise() const { throw *this; }
    QException* clone() const { return new MyException(*this); }
    std::exception error() const { return e; }
    QString msg() const { return str_what; }
private:
    std::exception e;
    QString str_what;
};

#endif // MYEXCEPTION_H
