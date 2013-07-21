#ifndef TOOLINTERFACE_H
#define TOOLINTERFACE_H

class ToolInterface
{
public:
    virtual ~ToolInterface(){}
    virtual QStringList requiredInputs() = 0;
    virtual QStringList requiredOutputs() = 0;
    virtual QStringList currentInputs() = 0;
    virtual QStringList currentOutputs() = 0;
};

#endif // TOOLINTERFACE_H
